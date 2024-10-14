#pragma once
#include "stdafx.h"
#include "IFrameData.h"
#include <string>
#include <fstream>

namespace FileWriters {

	class __declspec(dllexport) KinectWriter {
		public:
			KinectWriter();
			~KinectWriter();

			HRESULT Init(const WCHAR* targetDir);
			HRESULT Reset();
			HRESULT WriteFrame(IFrameData* pData);
			BOOL IsInit() const;
		private:
			std::ofstream os;
			bool isInitiated;
	};

	class __declspec(dllexport) MetadataWriter {
		public:
			MetadataWriter();
			~MetadataWriter();

			HRESULT Init(const WCHAR* targetDir);
			HRESULT Reset();
			// TODO write metadata
			// HRESULT WriteNameAndAdditionalInfo();
			HRESULT WriteFrameCounters(size_t counterDepthMode, size_t counterSkeletonMode);

			BOOL IsInit() const;
		private:
			std::ofstream os;
			bool isInitiated;
	};
}