#pragma once
#include "stdafx.h"
#include "SkeletonModeData.h"

namespace KinectAdapters
{
	class __declspec(dllexport) SkeletonMode
	{
		public:
			SkeletonMode();
			~SkeletonMode();

			HRESULT Init();
			void ReleaseSpecificResources();

			HRESULT AcquireLatestFrame();
			HRESULT RefreshBodies();
			HRESULT getCurrentFrame(SkeletonModeData* data);
			HRESULT getDataForChosenBody(Joint* joints, UINT8 bodyIdx = 0);

			ICoordinateMapper* getCoordinateMapperPtr() const;
		private:
			IKinectSensor* m_pKinectSensor;
			ICoordinateMapper* m_pCoordinateMapper;
			IBodyFrameReader* m_pBodyFrameReader;
			IBodyFrame* m_pBodyFrame;
			IBody** m_ppBodies;
	};
}