#pragma once
#include "IKinectMode.h"
#include "Kinect.h"

namespace KinectAdapter {

	class __declspec(dllexport) DepthMode : public IKinectMode
	{
	public:
		DepthMode();
		~DepthMode();

		virtual HRESULT initiateKinectConnection() override;
		virtual void releaseSpecificResources() override;

		virtual std::pair<int, int> getFrameSize() const override;
		virtual HRESULT getCurrentFrame(IKinectData* pKinectDepthData) override;

	private:
		const int m_width = 512;
		const int m_height = 424;

		IKinectSensor* m_pKinectSensor;
		IDepthFrameReader* m_pDepthFrameReader;

		IDepthFrame* m_pDepthFrame = nullptr;
		IFrameDescription* m_pFrameDescription = nullptr;
	};

}
