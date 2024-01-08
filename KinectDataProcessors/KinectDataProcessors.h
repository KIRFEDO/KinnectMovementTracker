#pragma once
#include <fstream>
#include <vector>
#include <utility>

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

	template <class T>
	class __declspec(dllexport) DynamicBuffer {
		public:
			DynamicBuffer(const size_t& size);
			~DynamicBuffer();

			size_t GetBufferSize() const;
			void WriteNextValue(const T& val);

			T& First();
			T& Last();

			T& operator[](size_t index);
		private:

			size_t GetNextIndex(size_t index);
			size_t GetPrevIndex(size_t index);

			T* m_buffer;
			size_t m_bufferSize;
			size_t m_currentIndex;
	};

	class __declspec(dllexport) SinglePassExtractor
	{
	public:
		SinglePassExtractor();
		~SinglePassExtractor();

		HRESULT Init(const wchar_t* targetDir);
		HRESULT GetPassSegments();
		HRESULT ProcessFile();
		BOOL IsInit() const;

	private:
		Direction GetWalkingDirection();

		std::ifstream m_is;
		BOOL m_isInitiated;
		BOOL m_isFileProcessed;
		DynamicBuffer<Joint>* m_dynamicBuffer;
	};
}