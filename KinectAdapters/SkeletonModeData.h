#pragma once
#include "stdafx.h"

struct __declspec(dllexport) SkeletonModeData
{
	SkeletonModeData()
	{
		leftHandState = HandState_Unknown;
		rightHandState = HandState_Unknown;
		joints = new Joint[JointType_Count];
	};

	~SkeletonModeData()
	{
		delete[] joints;
	}

	HandState leftHandState;
	HandState rightHandState;
	Joint* joints;
};