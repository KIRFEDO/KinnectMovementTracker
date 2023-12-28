#pragma once
#include "stdafx.h"

struct __declspec(dllexport) SkeletonModeData
{
	SkeletonModeData(ICoordinateMapper* m_pCoordinateMapper)
	{
		leftHandState = HandState_Unknown;
		rightHandState = HandState_Unknown;
		joints = new Joint[JointType_Count];
		this->m_pCoordinateMapper = m_pCoordinateMapper;
	};

	~SkeletonModeData()
	{
		delete[] joints;
	}

	HandState leftHandState;
	HandState rightHandState;
	Joint* joints;
	ICoordinateMapper* m_pCoordinateMapper;
};