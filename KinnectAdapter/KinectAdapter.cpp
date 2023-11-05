#include "KinectAdapter.h"

void KinnectAdapter::KinectWrapper::InitializeConnection()
{
	GetDefaultKinectSensor(&m_pKinectSensor);
	HRESULT hr;
	hr = m_pKinectSensor->Open();
}

KinnectAdapter::KinectWrapper::~KinectWrapper()
{
	if (m_pKinectSensor) {
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}
