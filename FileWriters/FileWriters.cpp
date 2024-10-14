#include "FileWriters.h"
#include <memory>

namespace FileWriters {

    KinectWriter::KinectWriter()
    {
        isInitiated = false;
    }

    KinectWriter::~KinectWriter()
    {
        if (IsInit())
            os.close();
    }

    HRESULT KinectWriter::Init(const WCHAR* targetDir)
    {
        HRESULT hr = S_OK;

        if (isInitiated)
            hr = E_NOT_VALID_STATE;

        if (SUCCEEDED(hr))
        {
            os.open(targetDir, std::ios::out | std::ios::binary | std::ios::trunc);
            hr = os.is_open() ? S_OK : E_HANDLE;
        }

        if (SUCCEEDED(hr))
            isInitiated = true;

        return hr;
    }

    HRESULT KinectWriter::Reset()
    {
        HRESULT hr = S_OK;
        if (!isInitiated)
            hr = E_NOT_VALID_STATE;
        if (SUCCEEDED(hr))
        {
            isInitiated = false;
            os.close();
        }
        return hr;
    }

    BOOL KinectWriter::IsInit() const {
        return isInitiated;
    }

    HRESULT KinectWriter::WriteFrame(IFrameData* pData)
    {
        if (!IsInit())
            return E_NOT_VALID_STATE;
        
        pData->WriteData(os);
    }

    MetadataWriter::MetadataWriter()
    {
        isInitiated = FALSE;
    }

    MetadataWriter::~MetadataWriter()
    {
        if (IsInit())
            os.close();
    }

    HRESULT MetadataWriter::Init(const WCHAR* targetDir)
    {
        HRESULT hr = S_OK;

        if (isInitiated)
            hr = E_NOT_VALID_STATE;

        if (SUCCEEDED(hr))
        {
            os.open(targetDir, std::ios::out | std::ios::binary | std::ios::trunc);
            hr = os.is_open() ? S_OK : E_HANDLE;
        }

        if (SUCCEEDED(hr))
            isInitiated = true;

        return hr;
    }

    HRESULT MetadataWriter::Reset()
    {
        HRESULT hr = S_OK;
        if (!IsInit())
            hr = E_NOT_VALID_STATE;
        if (SUCCEEDED(hr))
        {
            isInitiated = false;
            os.close();
        }
        return hr;
    }

    HRESULT MetadataWriter::WriteFrameCounters(size_t counterDepthMode, size_t counterSkeletonMode)
    {
        try
        {
            os.write(reinterpret_cast<const char*>(&counterDepthMode), sizeof(size_t));
            os.write(reinterpret_cast<const char*>(&counterSkeletonMode), sizeof(size_t));
        }
        catch (...)
        {
            return E_FAIL;
        }

        return S_OK;
    }

    BOOL MetadataWriter::IsInit() const
    {
        return isInitiated;
    }
}