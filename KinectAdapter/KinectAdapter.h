#pragma once
#include <utility>
#include <stdexcept>
#include "IKinectMode.h"
#include "DepthMode.h"

namespace KinectAdapter {

	enum class __declspec(dllexport) KinectMode {
		SKELETON,
		DEPTH,
		RGB
	};

	class __declspec(dllexport) ModeFactory {
	public:
		static IKinectMode* getMode(const KinectMode& kinectMode) {
			switch (kinectMode) {
				case KinectMode::DEPTH:
					return new DepthMode();
				case KinectMode::SKELETON:
				case KinectMode::RGB:
				default:
					throw std::runtime_error("Not implemented");
			}
		}
		
	};
}

