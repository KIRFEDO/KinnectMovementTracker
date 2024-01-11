#pragma once
#include <ctime>
#include "Kinect.h"

struct __declspec(dllexport) FrameData
{
	FrameData(char* pBuffer, time_t timestamp) : pBuffer(pBuffer), timestamp(timestamp)
	{
		isMemoryNeedCleaning = false;
	}

	~FrameData()
	{
		if (isMemoryNeedCleaning)
			delete[] pBuffer;
	}

	virtual void ReserveBufferMemory() = 0;

	char* pBuffer;
	time_t timestamp;
	size_t dataSize;
	bool isMemoryNeedCleaning;
};

struct __declspec(dllexport) DepthModeFrameData : public FrameData
{
	DepthModeFrameData(char* pBuffer, time_t timestamp = 0) : FrameData(pBuffer, timestamp)
	{
		//Resolution of Depth mode x sizeof(pixelVal)
		dataSize = 512 * 424 * sizeof(UINT16);
	}

	virtual void ReserveBufferMemory() override
	{
		isMemoryNeedCleaning = true;
		pBuffer = reinterpret_cast<char*>(new UINT16[512 * 424]);
	}
};

struct __declspec(dllexport) SkeletonModeFrameData : public FrameData
{
	SkeletonModeFrameData(char* pBuffer, time_t timestamp = 0) : FrameData(pBuffer, timestamp)
	{
		dataSize = sizeof(Joint) * JointType_Count;
	}

	virtual void ReserveBufferMemory() override
	{
		isMemoryNeedCleaning = true;
		pBuffer = reinterpret_cast<char*>(new Joint[JointType_Count]);
	}
};