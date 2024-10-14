#pragma once
#include "stdafx.h"
#include "IFrameData.h"
#include <fstream>

namespace FileReaders {

	class __declspec(dllexport) KinectReader
	{
	public:
		KinectReader();
		~KinectReader();

		HRESULT Init(const WCHAR* filePath);
		HRESULT Reset();
		HRESULT MoveCursorAtFileBeginning();
		HRESULT ReadFrame(IFrameData* pData);
		BOOL IsInit() const;
		BOOL IsEOF() const;
	private:
		std::ifstream m_is;
		BOOL m_isInit;
		std::wstring m_filePath;
	};

};