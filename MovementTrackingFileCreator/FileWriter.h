#pragma once
#include "stdafx.h"
#include <string>
#include <fstream>

namespace FileWriter {

	class __declspec(dllexport) FileWriterDepthMode
	{
		public:
			FileWriterDepthMode();
			~FileWriterDepthMode();

			HRESULT Initiate(WCHAR* targetDir, const size_t& targetDirSize);
			HRESULT Reset();

			HRESULT WriteData(UINT16* pBuffer);
		private:
			WCHAR* m_targetDir;
			size_t m_targetDirLength;
			bool isInitiated;
			const int m_width = 512;
			const int m_height = 424;
			int m_pBufferLength;
			std::ofstream os;
	};

}