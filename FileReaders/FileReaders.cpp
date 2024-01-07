#include "FileReaders.h"
#include "stdafx.h"

namespace FileReaders
{

    HRESULT KinectReader::Reset()
    {
        HRESULT hr = S_OK;
        if (!isInitiated)
            hr = E_NOT_VALID_STATE;
        if (SUCCEEDED(hr))
        {
            isInitiated = false;
            is.close();
        }
        return hr;
    }

    HRESULT KinectReader::ReadFrame(FrameData* pData)
    {
        //Memory MUST be allocated before frame reading
        if (is.eof())
            return E_ABORT;
        if (pData->pBuffer == nullptr)
            return E_POINTER;
        if (!IsInit())
            return E_NOT_VALID_STATE;


        try {
            is.read(reinterpret_cast<char*> (pData->pBuffer), pData->dataSize);
            is.read(reinterpret_cast<char*> (&(pData->timestamp)), sizeof(time_t));
        }
        catch (...) {
            return E_FAIL;
        }

        return S_OK;


    }

    KinectReader::KinectReader()
    {
        isInitiated = FALSE;
    }

    KinectReader::~KinectReader()
    {
        if (IsInit())
            is.close();
    }

    HRESULT KinectReader::Init(const WCHAR* filePath)
    {
        HRESULT hr = S_OK;

        if (isInitiated)
            hr = E_NOT_VALID_STATE;

        if (SUCCEEDED(hr))
        {
            is.open(filePath, std::ios::in | std::ios::binary);
            hr = is.is_open() ? S_OK : E_HANDLE;
        }

        if (SUCCEEDED(hr))
            isInitiated = true;

        return hr;
    }

    BOOL KinectReader::IsInit() const
    {
        return isInitiated;
    }

    BOOL KinectReader::IsEOF() const
    {
        if (!IsInit())
            return E_NOT_VALID_STATE;
        return is.eof();
    }

}