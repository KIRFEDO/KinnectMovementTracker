#include "SkeletonMode.h"
#include <stdlib.h>

#define BODY_COUNT 6

namespace KinectAdapters
{
	SkeletonMode::SkeletonMode()
	{
        m_ppBodies = new IBody * [BODY_COUNT] {0};
	}

    SkeletonMode::~SkeletonMode()
    {
        SafeRelease(m_pBodyFrameReader);        
        SafeRelease(m_pCoordinateMapper);
        if (m_pKinectSensor)
        {
            m_pKinectSensor->Close();
        }
        SafeRelease(m_pKinectSensor);
    }

    HRESULT SkeletonMode::Init()
    {
        HRESULT hr;

        hr = GetDefaultKinectSensor(&m_pKinectSensor);
        if (SUCCEEDED(hr))
        {
            IBodyFrameSource* pBodyFrameSource = NULL;

            hr = m_pKinectSensor->Open();

            if (SUCCEEDED(hr))
            {
                hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
            }

            if (SUCCEEDED(hr))
            {
                hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
            }

            if (SUCCEEDED(hr))
            {
                hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
            }

            SafeRelease(pBodyFrameSource);
        }
        return hr;
    }

    void SkeletonMode::ReleaseSpecificResources()
    {
        SafeRelease(m_pBodyFrame);
    }

    HRESULT SkeletonMode::AcquireLatestFrame()
    {
        return m_pBodyFrameReader->AcquireLatestFrame(&m_pBodyFrame);
    }

    HRESULT SkeletonMode::RefreshBodies()
    {
        return m_pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, m_ppBodies);
    }

    HRESULT SkeletonMode::getCurrentFrame(SkeletonModeData* data)
    {
        HRESULT hr;
        hr = AcquireLatestFrame();
        if (SUCCEEDED(hr))
            hr = RefreshBodies();
        if (SUCCEEDED(hr))
            hr = getDataForChosenBody(data->joints);
        return hr;
    }

    HRESULT SkeletonMode::getDataForChosenBody(Joint* joints, UINT8 bodyIdx)
    {
        HRESULT hr = S_OK;
        IBody* pBody = m_ppBodies[bodyIdx];
        if (!pBody)
            return E_POINTER;

        BOOLEAN bTracked = false;
        hr = pBody->get_IsTracked(&bTracked);
        if (SUCCEEDED(hr) && bTracked) {
            hr = pBody->GetJoints(BODY_COUNT, joints);
        }

    }
    ICoordinateMapper* SkeletonMode::getCoordinateMapperPtr() const
    {
        return m_pCoordinateMapper;
    }
}
