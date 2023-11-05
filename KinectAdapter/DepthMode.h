#pragma once
#include "IKinectMode.h"

class DepthMode : public IKinectMode
{
public:
	std::pair<int, int> getFrameSize() const override;
private:
	const int width = 512;
	const int height = 424;
};

