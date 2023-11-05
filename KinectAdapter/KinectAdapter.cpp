#include "KinectAdapter.h"

void KinectAdapter::KinectWrapper::InitializeConnection()
{
	GetDefaultKinectSensor(&m_pKinectSensor);
	HRESULT hr;
	hr = m_pKinectSensor->Open();
}

KinectAdapter::KinectWrapper::~KinectWrapper()
{
	if (m_pKinectSensor) {
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}
