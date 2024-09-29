#include "SkeletonMode.h"
#include <stdlib.h>
#include <stdexcept>

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
        if(m_ppBodies)
            for (int i = 0; i < BODY_COUNT; i++)
                SafeRelease(m_ppBodies[i]);
        SafeRelease(m_pBodyFrame);
    }

    HRESULT SkeletonMode::AcquireLatestFrame()
    {
        return m_pBodyFrameReader->AcquireLatestFrame(&m_pBodyFrame);
    }

    HRESULT SkeletonMode::UpdateBodies()
    {
        return m_pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, m_ppBodies);
    }

    void SkeletonMode::GetTrackingState(BOOLEAN* trackingState)
    {
        if (trackingState == nullptr)
            throw std::runtime_error("Tracking state array is corrupted");

        if (*m_ppBodies == nullptr)
            return;

        for (int i = 0; i < BODY_COUNT; i++)
        {
            m_ppBodies[i]->get_IsTracked(trackingState+i);
        }

    }

    HRESULT SkeletonMode::getCurrentFrame(SkeletonModeData* data)
    {
        HRESULT hr;
        hr = AcquireLatestFrame();
        if (SUCCEEDED(hr))
            hr = UpdateBodies();

        /*
            For now we need to track only one body.
            Due to the logic of Kinect SDK tracked person
            can be put in any of the 6 body slots and
            that is why for now we are only looking for
            the tracked slot and do not map it in any way.
        */

        //INT8 bodyIdx = -1;
        //for (int i = 0; i<BODY_COUNT; i++) {
        //    BOOLEAN isTracked;
        //    IBody* body = m_ppBodies[i];
        //    body->get_IsTracked(&isTracked);
        //    if (isTracked) {
        //        bodyIdx = i;
        //        break;
        //    }
        //}

        //if (bodyIdx != -1)
        //    getDataForChosenBody(data->joints, bodyIdx);

        return hr;
    }

    HRESULT SkeletonMode::getDataForChosenBody(Joint* joints, UINT8 bodyIdx)
    {
        HRESULT hr = S_OK;
        IBody* pBody = m_ppBodies[bodyIdx];
        if (!pBody)
            return E_POINTER;
        
        hr = pBody->GetJoints(JointType_Count, joints);
        
        return hr;
    }

    ICoordinateMapper* SkeletonMode::getCoordinateMapperPtr() const
    {
        return m_pCoordinateMapper;
    }
}
