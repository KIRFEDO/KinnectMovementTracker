#include <stdexcept>
#include "DepthMode.h"
#include "stdafx.h"

namespace KinectAdapters {

    DepthMode::DepthMode()
    {
    }

    DepthMode::~DepthMode()
    {
        SafeRelease(m_pDepthFrame);
        SafeRelease(m_pDepthFrameReader);
        if (m_pKinectSensor) {
            m_pKinectSensor->Close();
        }
        SafeRelease(m_pKinectSensor);
    }

    HRESULT DepthMode::Init()
    {
        HRESULT hr;
        hr = GetDefaultKinectSensor(&m_pKinectSensor);

        if (SUCCEEDED(hr))
        {
            // Init the Kinect and get the depth reader
            IDepthFrameSource* pDepthFrameSource = NULL;

            hr = m_pKinectSensor->Open();

            if (SUCCEEDED(hr))
            {
                hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
            }

            if (SUCCEEDED(hr))
            {
                hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
            }

            SafeRelease(pDepthFrameSource);
        }

        return hr;
    }

    void DepthMode::ReleaseSpecificResources()
    {
        //TODO properly handle memory allocated by Kinect resources
        SafeRelease(m_pDepthFrame);
        SafeRelease(m_pFrameDescription);
    }

    std::pair<int, int> DepthMode::getFrameSize() const
    {
        return std::pair<int, int>(m_width, m_height);
    }

    HRESULT DepthMode::getCurrentFrame(DepthModeData* pDepthModeData)
    {
        HRESULT hr = m_pDepthFrameReader->AcquireLatestFrame(&m_pDepthFrame);

        if (SUCCEEDED(hr))
        {
            INT64 nTime = 0;
            int nWidth = 0;
            int nHeight = 0;
            USHORT nDepthMinReliableDistance = 0;
            USHORT nDepthMaxDistance = 0;
            UINT nBufferSize = 0;
            UINT16* pBuffer = NULL;

            hr = m_pDepthFrame->get_RelativeTime(&nTime);

            if (SUCCEEDED(hr))
            {
                hr = m_pDepthFrame->get_FrameDescription(&m_pFrameDescription);
            }

            if (SUCCEEDED(hr))
            {
                hr = m_pFrameDescription->get_Width(&nWidth);
            }

            if (SUCCEEDED(hr))
            {
                hr = m_pFrameDescription->get_Height(&nHeight);
            }

            if (SUCCEEDED(hr))
            {
                hr = m_pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
            }

            if (SUCCEEDED(hr))
            {
                //TO DO
                // Check if data from certain distance can be ignored?

                // In order to see the full range of depth (including the less reliable far field depth)
                // we are setting nDepthMaxDistance to the extreme potential depth threshold
                nDepthMaxDistance = USHRT_MAX;

                // Note:  If you wish to filter by reliable depth distance, uncomment the following line.
                //// hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxDistance);
            }

            bool validFrame = false;
            if (SUCCEEDED(hr))
            {
                hr = m_pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
                validFrame = true;
            }

            DepthModeData res(nTime, pBuffer, nWidth, nHeight,
                            nDepthMinReliableDistance, nDepthMaxDistance, validFrame);
            *pDepthModeData = res;
        }
        return hr;
    }

}