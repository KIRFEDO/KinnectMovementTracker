#include "FileReaders.h"
#include "stdafx.h"

namespace FileReaders
{

    HRESULT KinectReader::Reset()
    {
        HRESULT hr = S_OK;
        if (!IsInit())
            hr = E_NOT_VALID_STATE;
        if (SUCCEEDED(hr))
        {
            m_isInit = false;
            m_is.close();
        }
        return hr;
    }

    HRESULT KinectReader::MoveCursorAtFileBeginning()
    {
        if (!IsInit())
            return E_NOT_VALID_STATE;

        Reset();
        Init(m_filePath.c_str());
    }

    HRESULT KinectReader::ReadFrame(FrameData* pData)
    {
        //Memory MUST be allocated before frame reading
        if (m_is.eof())
            return E_ABORT;
        if (pData->pBuffer == nullptr)
            return E_POINTER;
        if (!IsInit())
            return E_NOT_VALID_STATE;


        try {
            m_is.read(reinterpret_cast<char*> (pData->pBuffer), pData->dataSize);
            m_is.read(reinterpret_cast<char*> (&(pData->timestamp)), sizeof(time_t));
        }
        catch (...) {
            return E_FAIL;
        }

        return S_OK;


    }

    KinectReader::KinectReader()
    {
        m_isInit = FALSE;
    }

    KinectReader::~KinectReader()
    {
        if (IsInit())
            m_is.close();
    }

    HRESULT KinectReader::Init(const WCHAR* filePath)
    {
        HRESULT hr = S_OK;

        if (IsInit())
            hr = E_NOT_VALID_STATE;

        if (SUCCEEDED(hr))
        {
            m_is.open(filePath, std::ios::in | std::ios::binary);
            hr = m_is.is_open() ? S_OK : E_HANDLE;
        }

        if (SUCCEEDED(hr))
        {
            m_isInit = true;
            m_filePath = filePath;
        }

        return hr;
    }

    BOOL KinectReader::IsInit() const
    {
        return m_isInit;
    }

    BOOL KinectReader::IsEOF() const
    {
        if (!IsInit())
            return E_NOT_VALID_STATE;
        return m_is.eof();
    }

}