#pragma once
#include "stdafx.h"

namespace KinectAdapter {

	class __declspec(dllexport) KinectWrapper
	{
	public:
		void InitializeConnection();
		bool isConnectionOpened() const;
		~KinectWrapper();
	private:
		IKinectSensor* m_pKinectSensor;
	};

}

