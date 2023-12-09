#pragma once
#include <utility>
#include <winerror.h>
#include "IKinectData.h"

class __declspec(dllexport) IKinectMode {
public:
	virtual std::pair<int, int> getFrameSize() const = 0;
	virtual HRESULT getCurrentFrame(IKinectData* pKinectDepthData) = 0;
	virtual HRESULT initiateKinectConnection() = 0;
	virtual void releaseSpecificResources() = 0;
};