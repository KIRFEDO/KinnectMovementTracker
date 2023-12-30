#pragma once
#include "DepthModeData.h"
#include "stdafx.h"
#include <utility>

namespace KinectAdapters
{

	class __declspec(dllexport) DepthMode
	{
		public:
			DepthMode();
			~DepthMode();

			HRESULT Init();
			void ReleaseSpecificResources();

			std::pair<int, int> getFrameSize() const;
			HRESULT getCurrentFrame(DepthModeData* pKinectDepthData);

			size_t GetSingleFrameSize();

		private:
			const int m_width = 512;
			const int m_height = 424;

			IKinectSensor* m_pKinectSensor;
			IDepthFrameReader* m_pDepthFrameReader;

			IDepthFrame* m_pDepthFrame = nullptr;
			IFrameDescription* m_pFrameDescription = nullptr;
	};

}
