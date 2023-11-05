#include <stdexcept>
#include "DepthMode.h"
#include "stdafx.h"

DepthMode::DepthMode()
{
    HRESULT hr;
    hr = GetDefaultKinectSensor(&m_pKinectSensor);
    if (FAILED(hr))
    {
        throw std::runtime_error("Can`t connect to the kinect sensor");
    }

    if (m_pKinectSensor)
    {
        // Initialize the Kinect and get the depth reader
        IDepthFrameSource* pDepthFrameSource = NULL;

        hr = m_pKinectSensor->Open();

        if (SUCCEEDED(hr))
        {
            hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrameSource->OpenReader(&m_reader);
        }

        SafeRelease(pDepthFrameSource);
    }

    if (FAILED(hr))
    {
        throw std::runtime_error("Can`t connect to the depth frame source");
    }
}

DepthMode::~DepthMode()
{
    SafeRelease(m_reader);
	if (m_pKinectSensor) {
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}

std::pair<int, int> DepthMode::getFrameSize() const
{
    return std::pair<int, int>(m_width, m_height);
}
