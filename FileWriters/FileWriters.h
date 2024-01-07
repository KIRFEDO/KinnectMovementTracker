#pragma once
#include "stdafx.h"
#include <string>
#include <fstream>

namespace FileWriters {

	struct __declspec(dllexport) FrameData
	{
		FrameData(const char* pBuffer, time_t timestamp) : pBuffer(pBuffer), timestamp(timestamp)
		{
		}
		
		const char* pBuffer;
		time_t timestamp;
		size_t dataSize;
	};

	struct __declspec(dllexport) DepthModeFrameData : public FrameData
	{
		DepthModeFrameData(const char* pBuffer, time_t timestamp) : FrameData(pBuffer, timestamp)
		{
			//Resolution of Depth mode x sizeof(pixelVal)
			dataSize = 512 * 424 * sizeof(UINT16);
		}
	};

	struct __declspec(dllexport) SkeletonModeFrameData : public FrameData
	{
		SkeletonModeFrameData(const char* pBuffer, time_t timestamp) : FrameData(pBuffer, timestamp)
		{
			dataSize = sizeof(Joint) * JointType_Count;
		}

	};

	class __declspec(dllexport) KinectWriter {
		public:
			KinectWriter();
			~KinectWriter();

			HRESULT Init(const WCHAR* targetDir);
			HRESULT Reset();
			HRESULT WriteFrame(FrameData* pData);
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