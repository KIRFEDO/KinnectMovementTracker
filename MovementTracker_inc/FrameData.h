#pragma once
#include <ctime>
#include "Kinect.h"

struct __declspec(dllexport) FrameData
{
	FrameData(char* pBuffer, time_t timestamp) : pBuffer(pBuffer), timestamp(timestamp)
	{
	}

	char* pBuffer;
	time_t timestamp;
	size_t dataSize;
};

struct __declspec(dllexport) DepthModeFrameData : public FrameData
{
	DepthModeFrameData(char* pBuffer, time_t timestamp = 0) : FrameData(pBuffer, timestamp)
	{
		//Resolution of Depth mode x sizeof(pixelVal)
		dataSize = 512 * 424 * sizeof(UINT16);
	}
};

struct __declspec(dllexport) SkeletonModeFrameData : public FrameData
{
	SkeletonModeFrameData(char* pBuffer, time_t timestamp = 0) : FrameData(pBuffer, timestamp)
	{
		dataSize = sizeof(Joint) * JointType_Count;
	}

};