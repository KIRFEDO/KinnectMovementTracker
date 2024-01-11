#pragma once
#include <fstream>
#include <vector>
#include <utility>

#include "DynamicBuffer.h"
#include "FileReaders.h"
#include "FrameData.h"
#include "FileWriters.h"
#include "stdafx.h"

using namespace FileWriters;
using namespace FileReaders;

namespace KinectDataProcessors
{
	enum class __declspec(dllexport) Direction {
		BACKWARDS,
		STANDING_STILL,
		FORWARD
	};

	class __declspec(dllexport) SinglePassExtractor
	{
		public:
			SinglePassExtractor();
			~SinglePassExtractor();

			HRESULT Init(const wchar_t* targetDir);
			HRESULT GetPassSegments();
			HRESULT ProcessFile(std::vector<std::pair<time_t, time_t>>& segments);
			BOOL IsInit() const;

		private:
			Direction GetWalkingDirection();

			KinectReader m_reader;
			BOOL m_isInitiated;
			BOOL m_isFileProcessed;
			DynamicBuffer<Joint>* m_dynamicBuffer;
	};

	class __declspec(dllexport) AxisRotator {
	public:
		AxisRotator();
		~AxisRotator();

		HRESULT Init(const wchar_t* targetDir, std::vector<std::pair<time_t, time_t>>* segments);
		HRESULT CalculateRotationAngles();
		HRESULT GetRotationAngles();
		HRESULT CreateFileWithRotatedAxis();
		BOOL IsInit() const;
	private:
		std::ifstream m_is;
		std::vector<std::pair<time_t, time_t>>* m_pSegments;
		BOOL m_isInitiated;
	};
}