// ImportTester.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include "SkeletonMode.h"
#include "FileReaders.h"
#include "FileWriters.h"
#include "KinectDataProcessors.h"
#include "stdafx.h"
#include <cmath>
#include <chrono>
#include "engine.h"
#include "MatlabHelpers.h"

//#define READING
//#define DIRECTION
//#define FILE_ANALYSIS
//#define CIRCULAR_BUFFER
#define MATLAB_TEST

const int tabSize = 30;
CameraSpacePoint cameraSpacePoints[tabSize];
Joint spacePoints[tabSize];
time_t times[tabSize];
float firstPos = -1;
void printSpacePoints(int startIndex)
{
    for (int i = 0; i < tabSize; i++)
    {
        auto point = cameraSpacePoints[i];
        printf_s("Frame #%d\nx:%f\ny:%f\nz:%f\n", i, point.X, point.Y, point.Z);
        startIndex += 1;
        if (startIndex == tabSize)
            startIndex = 0;
    }
}

int getPrev(int startIndex) {
    if (startIndex == 0) {
        return tabSize - 1;
    }
    else
    {
        return startIndex - 1;
    }
}

int getNext(int startIndex) {
    if (startIndex == tabSize -1) {
        return 0;
    }
    else
    {
        return startIndex + 1;
    }
}

bool isMovingForward(int startIndex)
{
    float res = cameraSpacePoints[startIndex].Z - cameraSpacePoints[getNext(startIndex)].Z;
    printf_s("Delta: %f\n", res);
    return !(res >= 0.05);
}

void InitMatlab()
{
    const char* args[] = { "-nojvm" };
    bool res = mclInitializeApplication(args, 1);
    if (!res)
    {
        fprintf(stdout,
            "An error occurred while initializing: \n %s ",
            mclGetLastErrorMessage());
    }
    MatlabHelpersInitialize();
}

void DeInitMatlab()
{
    MatlabHelpersTerminate();
    mclTerminateApplication();
}

int main()
{

#ifdef READING

    std::wstring path_depth = L"C:/BuffEnv/Live/depth.txt";
    std::wstring path_skeleton = L"C:/BuffEnv/Live/skel.txt";
    std::wstring path_metadata = L"C:/BuffEnv/Live/metadata.txt";
    std::ifstream depth_reader;
    std::ifstream skeleton_reader;
    std::ifstream metadata_reader;
    metadata_reader.open(path_metadata, std::ios::in | std::ios::binary);
    depth_reader.open(path_depth, std::ios::in | std::ios::binary);
    skeleton_reader.open(path_skeleton, std::ios::in | std::ios::binary);

    size_t depth_counter = 0;
    size_t skeleton_counter = 0;

    metadata_reader.read(reinterpret_cast<char*>(&depth_counter), sizeof(size_t));
    metadata_reader.read(reinterpret_cast<char*>(&skeleton_counter), sizeof(size_t));
    metadata_reader.close();


    uint16_t* pBuffer = new uint16_t[512*424];
    time_t pTime;

    Joint* joints = new Joint[JointType_Count];
    time_t pTimeSkel;


    int counter = 0;
    int counter_skel = 0;

    std::map<time_t, int> times;
    while (!depth_reader.eof()) {
        depth_reader.read(reinterpret_cast<char*>(pBuffer), sizeof(uint16_t) * 512 * 424);
        depth_reader.read(reinterpret_cast<char*>(&pTime), sizeof(time_t));

        times[pTime] = 1;

        counter++;
    }
    
    while (!skeleton_reader.eof()) {

        skeleton_reader.read(reinterpret_cast<char*>(joints), sizeof(Joint) * JointType_Count);
        skeleton_reader.read(reinterpret_cast<char*>(&pTimeSkel), sizeof(time_t));

        if (times[pTimeSkel] == 1) {
            try {
                times.at(pTimeSkel);
                times[pTimeSkel] = 2;
            }
            catch (...) {}
        }
        else if (times[pTimeSkel - 1] == 1) {
            try {
                times.at(pTimeSkel-1);
                times[pTimeSkel - 1] = 2;
            }
            catch (...) {}
        }

        counter_skel++;
    }

    depth_reader.close();
    skeleton_reader.close();

#endif // READING
    
#ifdef DIRECTION

    using namespace KinectAdapters;
    SkeletonMode m_skeletonMode;
    HRESULT hr = m_skeletonMode.Init();
    if (FAILED(hr))
        throw std::runtime_error("Failed to init");

    int counter = 0;
    int spacePointIndex = 0;

    while (1)
    {
        SkeletonModeData* skeletonModeData = new SkeletonModeData(m_skeletonMode.getCoordinateMapperPtr());
        HRESULT hr_skeletonMode = m_skeletonMode.getCurrentFrame(skeletonModeData);

        if (SUCCEEDED(hr_skeletonMode))
        {
            counter++;
            auto pJoints = skeletonModeData->joints;
            spacePoints[spacePointIndex] = pJoints[JointType::JointType_SpineBase].Position;
            
            system("cls");
            //printSpacePoints(spacePointIndex);
            if (firstPos == -1)
                firstPos = spacePoints[spacePointIndex].Z;
            //std::string res = isMovingForward(spacePointIndex) ? "Moving forward" : "Moving backwards";
            auto start = spacePoints[getNext(spacePointIndex)];
            auto end = spacePoints[spacePointIndex];
            auto delta_z = end.Z - start.Z;
            auto delta_x = end.X - start.X;

            auto angle = std::atan2(delta_x, delta_z);
            std::cout << angle * 180 / 3.1415926 << std::endl;

            spacePointIndex++;
            if (spacePointIndex == tabSize)
                spacePointIndex = 0;
        }

        m_skeletonMode.ReleaseSpecificResources();
        delete skeletonModeData;
    }

#endif // DIRECION

#ifdef FILE_ANALYSIS

    using namespace KinectDataProcessors;
    using namespace FileWriters;
    using namespace FileReaders;

    AxisRotator axisRotator;
    KinectReader depthReader;
    KinectWriter depthWriter;
    SinglePassExtractor passExtractor;

    std::wstring smPath = L"C:/BuffEnv/Live/skel.txt";
    axisRotator.Init(smPath.c_str());
    passExtractor.Init(smPath.c_str());

    std::vector<std::pair<time_t, time_t>> segments;
    HRESULT hr = passExtractor.ProcessFile(segments);
    if (FAILED(hr))
        throw std::runtime_error("");

    //axisRotator.CalculateRotationAngles(segments);

    std::wstring dmPath = L"C:/BuffEnv/Live/depth.txt";
    std::wstring dmPath_proc = L"C:/BuffEnv/depthProcessed.txt";



    /*depthReader.Init(dmPath.c_str());
    depthWriter.Init(dmPath_proc.c_str());


    UINT16* data = new UINT16[512 * 424];
    for (const auto& segment : segments)
    {
        auto startTime = segment.first;
        auto endTime = segment.second;
        
        CameraSpacePoint start;
        CameraSpacePoint end = {-1, -1, -1};
        
        bool isFirstRead = true;

        while (!depthReader.IsEOF())
        {
            DepthModeFrameData depthModeFrameData(reinterpret_cast<char*>(data));
            HRESULT hr = depthReader.ReadFrame(&depthModeFrameData);
            if (SUCCEEDED(hr))
            {
                if (depthModeFrameData.timestamp > segment.second)
                    break;
                if (depthModeFrameData.timestamp > segment.first)
                    depthWriter.WriteFrame(&depthModeFrameData);
            }
            else
            {
                throw std::runtime_error("");
            }

            Joint* pJoints = reinterpret_cast<Joint*>(depthModeFrameData.pBuffer);
        }
    }

    delete[] data;*/

#endif // FILE_ANALYSIS

#ifdef CIRCULAR_BUFFER

    using namespace KinectDataProcessors;
    DynamicBuffer<int> buffer(5);
    for (int i = 0; i < 10; i++)
    {
        buffer.WriteNextValue(i);
    }
    std::cout << buffer.First() << std::endl;
    std::cout << buffer.Last() << std::endl;

#endif // CIRCULAR_BUFFER

#ifdef MATLAB_TEST

    using namespace KinectAdapters;
    SkeletonMode m_skeletonMode;
    HRESULT hr = m_skeletonMode.Init();
    if (FAILED(hr))
        throw std::runtime_error("Failed to init");

    int counter = 0;
    int spacePointIndex = 0;

    InitMatlab();

    mwArray in_t(1, tabSize, mxClassID::mxDOUBLE_CLASS);
    mwArray in_axisZ(1, tabSize, mxClassID::mxDOUBLE_CLASS);
    mwArray in_axisX(1, tabSize, mxClassID::mxDOUBLE_CLASS);
    double* dataT = new double[tabSize];
    double* dataAxisZ = new double[tabSize];
    double* dataAxisX = new double[tabSize];
    mwArray out(1);
    auto const startTime = std::chrono::system_clock::now();
    //MatlabProcessors(1, out, in_1, in_2);

    while (1)
    {
        SkeletonModeData* skeletonModeData = new SkeletonModeData(m_skeletonMode.getCoordinateMapperPtr());
        HRESULT hr_skeletonMode = m_skeletonMode.getCurrentFrame(skeletonModeData);

        if (SUCCEEDED(hr_skeletonMode))
        {
            counter++;
            auto pJoints = skeletonModeData->joints;
            cameraSpacePoints[spacePointIndex] = pJoints[JointType::JointType_SpineBase].Position;
            spacePoints[spacePointIndex] = pJoints[JointType::JointType_SpineBase];
            auto const now = std::chrono::system_clock::now();
            times[spacePointIndex] = (now - startTime).count();

            system("cls");
            //printSpacePoints(spacePointIndex);
            if (firstPos == -1)
                firstPos = cameraSpacePoints[spacePointIndex].Z;
            //std::string res = isMovingForward(spacePointIndex) ? "Moving forward" : "Moving backwards";
            auto start = cameraSpacePoints[getNext(spacePointIndex)];
            auto end = cameraSpacePoints[spacePointIndex];
            auto delta_z = end.Z - start.Z;
            auto delta_x = end.X - start.X;

            auto currIdx = getNext(spacePointIndex);
            for (int i = 0; i < tabSize; i++)
            {
                auto point = spacePoints[currIdx];
                dataT[i] = times[currIdx];
                dataAxisZ[i] = point.Position.Z;
                dataAxisX[i] = point.Position.X;
                currIdx++;
            }

            in_t.SetData(dataT, tabSize);
            in_axisZ.SetData(dataAxisZ, tabSize);
            in_axisX.SetData(dataAxisX, tabSize);

            GetRotationAngle(1, out, in_t, in_axisZ, in_axisX);
            auto angle = std::atan2(delta_x, delta_z);
            std::cout << "C++ angle:" << angle * 180 / 3.1415926 << std::endl;
            std::cout << "Matlab angle:" << (double) out * 180 / 3.1415926 << std::endl;

            spacePointIndex++;
            if (spacePointIndex == tabSize)
                spacePointIndex = 0;
        }

        m_skeletonMode.ReleaseSpecificResources();
        delete skeletonModeData;
    }

    DeInitMatlab();    

#endif // MATLAB_TEST

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
