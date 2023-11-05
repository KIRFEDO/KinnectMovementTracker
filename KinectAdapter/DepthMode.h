#pragma once
#include "IKinectMode.h"
#include "Kinect.h"

class DepthMode : public IKinectMode
{
public:
    DepthMode();
    ~DepthMode();

	std::pair<int, int> getFrameSize() const override;

private:
	const int m_width = 512;
	const int m_height = 424;

	IKinectSensor* m_pKinectSensor;
	IDepthFrameReader* m_reader;
};

