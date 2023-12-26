#pragma once
#include "DepthModeData.h"
#include "Kinect.h"
#include <utility>

namespace KinectAdapter {

	class __declspec(dllexport) DepthMode
	{
	public:
		DepthMode();
		~DepthMode();

		HRESULT Init();
		void ReleaseSpecificResources();

		std::pair<int, int> getFrameSize() const;
		HRESULT getCurrentFrame(DepthModeData* pKinectDepthData);

	private:
		const int m_width = 512;
		const int m_height = 424;

		IKinectSensor* m_pKinectSensor;
		IDepthFrameReader* m_pDepthFrameReader;

		IDepthFrame* m_pDepthFrame = nullptr;
		IFrameDescription* m_pFrameDescription = nullptr;
	};

}
