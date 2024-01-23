#include <iostream>
#include <filesystem>
#include <string>
#include "MatlabHelpers.h"
#include "FileReaders.h"
#include "FileWriters.h"
#include "KinectDataProcessors.h"

using namespace FileReaders;
using namespace FileWriters;
using namespace std::filesystem;
using namespace KinectDataProcessors;

void ClearScreen();
std::wstring StringToWstring(char* str);
bool CheckIfHistoryDataIsPresent(const std::wstring& readPath);
void ExitIfHRFailed(HRESULT hr, std::string errorMsg);

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf_s("Wrong amount of arguments");
        return -1;
    }

    std::wstring readDir = StringToWstring(argv[1]);
    std::wstring writeDir = StringToWstring(argv[2]);

    if (!CheckIfHistoryDataIsPresent(readDir))
        return -1;

    HRESULT hr;

    KinectReader depthReader;
    hr = depthReader.Init((readDir + L"/depth.txt").c_str());
    ExitIfHRFailed(hr, "Failed Depth mode reader initialization\n");
   
    KinectReader skelReader;
    skelReader.Init((readDir + L"/skel.txt").c_str());
    ExitIfHRFailed(hr, "Failed Skeleton mode reader initialization\n");

    SinglePassExtractor passExtractor;
    hr = passExtractor.Init((readDir + L"/skel.txt").c_str());
    ExitIfHRFailed(hr, "Failed passed extractor initialization\n");

    std::vector<std::pair<time_t, time_t>> usefullSegments;
    hr = passExtractor.ProcessFile(usefullSegments);
    ExitIfHRFailed(hr, "Failed to extract useful segments\n");

    const char* args[] = { "-nojvm" };
    hr = mclInitializeApplication(args, 1) ? S_OK : E_FAIL;
    ExitIfHRFailed(hr, "Failed to inialize MATLAB Runtime\n");
    hr = MatlabHelpersInitialize() ? S_OK : E_FAIL;
    ExitIfHRFailed(hr, "Failed to inialize MATLAB GetRotationAngle DLL\n");

    ClearScreen();

    std::vector<double> angles;
    std::vector<double> anglesInDegress;

    /*
        TODO add segment filtering by angle value
    */

    //Angles extraction
    {
        std::vector<double> t;
        std::vector<double> z;
        std::vector<double> x;
        SkeletonModeFrameData skelFrameData(nullptr);
        skelFrameData.ReserveBufferMemory();
        for (const auto& segment : usefullSegments)
        {
            auto startTime = segment.first;
            auto endTime = segment.second;

            while (!skelReader.IsEOF())
            {
                HRESULT hr = skelReader.ReadFrame(&skelFrameData);
                ExitIfHRFailed(hr, "Failed to read skeleton mode frame\n");

                if (skelFrameData.timestamp > endTime)
                {
                    break;
                }
                else if (skelFrameData.timestamp > startTime)
                {
                    t.push_back(skelFrameData.timestamp);
                    Joint* joints = reinterpret_cast<Joint*>(skelFrameData.pBuffer);
                    CameraSpacePoint pointCoordinates = joints[JointType_SpineBase].Position;
                    z.push_back(pointCoordinates.Z);
                    x.push_back(pointCoordinates.X);
                }
            }

            // multiplication by -1.0 is neccessary in order
            // to calculate angles in -90 to +90 degrees range
            for (auto& z_sample : z)
                z_sample *= -1.0;

            mwArray in_t(1, t.size(), mxClassID::mxDOUBLE_CLASS);
            mwArray in_axisX(1, t.size(), mxClassID::mxDOUBLE_CLASS);
            mwArray in_axisZ(1, t.size(), mxClassID::mxDOUBLE_CLASS);
            mwArray out(1);

            in_t.SetData(t.data(), t.size());
            in_axisX.SetData(x.data(), x.size());
            in_axisZ.SetData(z.data(), z.size());

            GetRotationAngle(1, out, in_t, in_axisX, in_axisZ);
            angles.push_back(out);
            anglesInDegress.push_back((double)out * 180 / 3.1415926);

            t.clear();
            z.clear();
            x.clear();
        }
    }

    //Position rotation
    {
        DepthModeFrameData depthFrameData(nullptr);
        depthFrameData.ReserveBufferMemory();

        SkeletonModeFrameData skelFrameData(nullptr);
        skelFrameData.ReserveBufferMemory();

        depthReader.MoveCursorAtFileBeginning();
        skelReader.MoveCursorAtFileBeginning();

        KinectWriter writerDepthExtracted;
        KinectWriter writerSkelExtractedRaw;
        KinectWriter writerSkelExtractedRotated;

        hr = writerDepthExtracted.Init((writeDir + L"/depthRaw.txt").c_str());
        ExitIfHRFailed(hr, "Failed to init depthRaw writer");
        hr = writerSkelExtractedRaw.Init((writeDir + L"/skelRaw.txt").c_str());
        ExitIfHRFailed(hr, "Failed to init skelRaw writer");
        hr = writerSkelExtractedRotated.Init((writeDir + L"/skelProc.txt").c_str());
        ExitIfHRFailed(hr, "Failed to init skelProc writer");

        for (int i = 0; i < usefullSegments.size(); i++)
        {
            auto& segment = usefullSegments[i];
            auto& angle = angles[i];

            auto startTime = segment.first;
            auto endTime = segment.second;

            std::vector<double> currPos;
            currPos.resize(3);

            while (!skelReader.IsEOF())
            {
                HRESULT hr = skelReader.ReadFrame(&skelFrameData);
                ExitIfHRFailed(hr, "Failed to read skeleton mode frame\n");

                if (skelFrameData.timestamp > endTime)
                {
                    break;
                }
                else if (skelFrameData.timestamp > startTime)
                {
                    writerSkelExtractedRaw.WriteFrame(&skelFrameData);
                    Joint* joints = reinterpret_cast<Joint*>(skelFrameData.pBuffer);

                    for (int i = 0; i < JointType_Count; i++) {

                        int posX = 0;
                        int posY = 1;
                        int posZ = 2;
                        
                        CameraSpacePoint currSpacePoint = joints[i].Position;
                        currPos[posX] = currSpacePoint.X;
                        currPos[posY] = currSpacePoint.Y;
                        currPos[posZ] = currSpacePoint.Z;

                        mwArray in_angle(1, 1, mxClassID::mxDOUBLE_CLASS);
                        mwArray in_currPos(1, 3, mxClassID::mxDOUBLE_CLASS);
                        mwArray out(1, 3, mxClassID::mxDOUBLE_CLASS);

                        in_angle = angle;
                        in_currPos.SetData(currPos.data(), 3);

                        GetRotatedCoordinate(1, out, in_angle, in_currPos);

                        CameraSpacePoint& cameraSpacePoint = joints[i].Position;

                        double outData[3];
                        out.GetData(outData, 3);                        

                        cameraSpacePoint.X = outData[0];
                        cameraSpacePoint.Z = outData[2];
                    }

                    writerSkelExtractedRotated.WriteFrame(&skelFrameData);
                }
            }

            while (!depthReader.IsEOF())
            {
                HRESULT hr = depthReader.ReadFrame(&depthFrameData);
                ExitIfHRFailed(hr, "Failed to read skeleton mode frame\n");

                if (depthFrameData.timestamp > endTime)
                {
                    break;
                }
                else if (depthFrameData.timestamp > startTime)
                {
                    writerDepthExtracted.WriteFrame(&depthFrameData);
                }
            }
        }
    }

    MatlabHelpersTerminate();
    mclTerminateApplication();
}

void ClearScreen()
{
    printf("\033[H\033[J");
}

std::wstring StringToWstring(char* str)
{
    std::string tempStr = str;
    return std::wstring(tempStr.begin(), tempStr.end());
}

bool CheckIfHistoryDataIsPresent(const std::wstring& readPath)
{
    auto depthPath = readPath + L"/depth.txt";
    if (!exists(depthPath))
    {
        printf_s("Depth sensor data is not present");
        return false;
    }
    auto skeletonPath = readPath + L"/skel.txt";
    if (!exists(skeletonPath))
    {
        printf_s("Skeleton mode data is not present");
        return false;
    }
    auto metadataPath = readPath + L"/metadata.txt";
    if (!exists(metadataPath))
    {
        printf_s("Metadata is not present");
        return false;
    }
}

void ExitIfHRFailed(HRESULT hr, std::string errorMsg)
{
    if (FAILED(hr)) {
        printf_s("%s", errorMsg.c_str());
        exit(-1);
    }
}