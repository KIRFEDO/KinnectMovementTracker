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

            mwArray in_t(1, t.size(), mxClassID::mxDOUBLE_CLASS);
            mwArray in_axisZ(1, t.size(), mxClassID::mxDOUBLE_CLASS);
            mwArray in_axisX(1, t.size(), mxClassID::mxDOUBLE_CLASS);
            mwArray out(1);

            in_t.SetData(t.data(), t.size());
            in_axisZ.SetData(z.data(), z.size());
            in_axisX.SetData(x.data(), x.size());

            GetRotationAngle(1, out, in_t, in_axisZ, in_axisX);
            angles.push_back(out);
            anglesInDegress.push_back((double)out * 180 / 3.1415926);

            t.clear();
            z.clear();
            x.clear();
        }
    }

    //Position rotation
    {
        SkeletonModeFrameData skelFrameData(nullptr);
        skelFrameData.ReserveBufferMemory();
        skelReader.MoveCursorAtFileBeginning();

        KinectWriter writerRaw;
        KinectWriter writerProcessed;

        hr = writerRaw.Init((writeDir + L"/skelRaw.txt").c_str());
        ExitIfHRFailed(hr, "Failed to init skelRaw writer");
        hr = writerProcessed.Init((writeDir + L"/skelProc.txt").c_str());
        ExitIfHRFailed(hr, "Failed to init skelProc writer");

        for (int i = 0; i < usefullSegments.size(); i++)
        {
            auto& segment = usefullSegments[i];
            auto angle = angles[i];
            
            std::vector<CameraSpacePoint> rotatedPoints;

            auto startTime = segment.first;
            auto endTime = segment.second;


            std::vector<std::vector<double>> startPosV;
            startPosV.resize(JointType_Count);
            std::vector<double> currPos;
            currPos.resize(3);
            bool isFirstPos = true;
            mwArray in_startPos(1, 3, mxClassID::mxDOUBLE_CLASS);

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
                    writerRaw.WriteFrame(&skelFrameData);
                    Joint* joints = reinterpret_cast<Joint*>(skelFrameData.pBuffer);

                    for (int i = 0; i < JointType_Count; i++) {

                        auto& startPos = startPosV[i];

                        int posX = 2;
                        int posY = 0;
                        int posZ = 1;
                        
                        /*int posX = 0;
                        int posZ = 1;
                        int posY = 2;*/


                        CameraSpacePoint currSpacePoint = joints[i].Position;
                        currPos[posX] = currSpacePoint.X;
                        currPos[posY] = currSpacePoint.Y;
                        currPos[posZ] = currSpacePoint.Z;

                        if (isFirstPos)
                        {
                            startPos = currPos;
                            in_startPos.SetData(startPos.data(), 3);
                        }

                        mwArray in_angle(1, 1, mxClassID::mxDOUBLE_CLASS);
                        mwArray in_currPos(1, 3, mxClassID::mxDOUBLE_CLASS);
                        mwArray out(1, 3, mxClassID::mxDOUBLE_CLASS);

                        in_angle = angle;
                        in_currPos.SetData(currPos.data(), 3);

                        GetRotatedCoordinate(1, out, in_angle, in_currPos, in_startPos);

                        CameraSpacePoint rotatedPoint;

                        double outData[3];
                        out.GetData(outData, 3);

                        //TODO properly assign values
                        /*rotatedPoint.X = outData[0];
                        rotatedPoint.Y = outData[1];
                        rotatedPoint.Z = outData[2];*/

                        rotatedPoint.Z = outData[posZ];
                        rotatedPoint.Y = outData[posY];
                        rotatedPoint.X = outData[posX];
                        //rotatedPoint.X = rotatedPoint.X * (-1.0);

                        joints[i].Position = rotatedPoint;

                        rotatedPoints.push_back(rotatedPoint);
                    }

                    writerProcessed.WriteFrame(&skelFrameData);
                }

                if (isFirstPos)
                    isFirstPos = false;
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
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
