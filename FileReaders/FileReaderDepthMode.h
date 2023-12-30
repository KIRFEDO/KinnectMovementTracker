#pragma once
#include "stdafx.h"
#include <fstream>

class __declspec(dllexport) FileReaderDepthMode
{
	public:
		FileReaderDepthMode();
		~FileReaderDepthMode();

		HRESULT Initiate(WCHAR* targetDir, const size_t& targetDirSize);
		HRESULT Reset();
		HRESULT ReadFrame(UINT16** pBuffer);

		int getWidth() const;
		int getHeight() const;
	private:
		WCHAR* m_targetDir;
		size_t m_targetDirLength;
		bool isInitiated;
		const int width = 512;
		const int height = 424;
		std::ifstream is;
};