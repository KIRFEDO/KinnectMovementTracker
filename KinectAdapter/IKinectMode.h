#pragma once
#include <utility>
#include <d2d1.h>

class __declspec(dllexport) IKinectMode {
public:
	virtual std::pair<int, int> getFrameSize() const = 0;
	virtual RGBQUAD* getCurrentFrame() const = 0;
};