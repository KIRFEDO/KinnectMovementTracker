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
#include "stdafx.h"

//#define READING
#define DIRECTION

const int tabSize = 5;
CameraSpacePoint spacePoints[tabSize];
float firstPos = -1;
void printSpacePoints(int startIndex)
{
    for (int i = 0; i < tabSize; i++)
    {
        auto point = spacePoints[i];
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
    float res = spacePoints[startIndex].Z - spacePoints[getNext(startIndex)].Z;
    
    return !(res >= 0.05);
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
            printSpacePoints(spacePointIndex);
            if (firstPos == -1)
                firstPos = spacePoints[spacePointIndex].Z;
            std::string res = isMovingForward(spacePointIndex) ? "Moving forward" : "Moving backwards";
            std::cout << res << std::endl;

            spacePointIndex++;
            if (spacePointIndex == tabSize)
                spacePointIndex = 0;
        }

        m_skeletonMode.ReleaseSpecificResources();
        delete skeletonModeData;
    }

#endif // DIRECION

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