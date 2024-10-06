#pragma once
#include "stdafx.h"

struct __declspec(dllexport) SkeletonModeData
{
	SkeletonModeData(const bool isTracked = false)
	{
		this->isTracked = isTracked;
		joints = new Joint[JointType_Count];
	};

	~SkeletonModeData()
	{
		delete[] joints;
	}

	bool isTracked;
	HandState leftHandState = HandState_Unknown;
	HandState rightHandState = HandState_Unknown;
	Joint* joints;
};