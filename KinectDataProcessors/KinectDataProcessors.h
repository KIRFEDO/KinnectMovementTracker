#pragma once
#include <fstream>
#include <vector>
#include <utility>

#include "DynamicBuffer.h"
#include "FrameData.h"
#include "FileWriters.h"
#include "stdafx.h"

using namespace FileWriters;

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

		std::ifstream m_is;
		BOOL m_isInitiated;
		BOOL m_isFileProcessed;
		DynamicBuffer<Joint>* m_dynamicBuffer;
	};
}