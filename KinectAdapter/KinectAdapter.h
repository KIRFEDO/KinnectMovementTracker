#pragma once
#include <utility>
#include <stdexcept>
#include "stdafx.h"
#include "IKinectMode.h"

namespace KinectAdapter {

	__declspec(dllexport) enum class KinectMode {
		SKELETON,
		DEPTH,
		RGB
	};

	class __declspec(dllexport) ModeFactory {
	public:
		IKinectMode* getMode(const KinectMode& kinectMode) {
			switch (kinectMode) {
			case KinectMode::SKELETON:
			case KinectMode::DEPTH:
			case KinectMode::RGB:
			default:
				throw std::runtime_error("Not implemented");
			}
		}
		
	};

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

