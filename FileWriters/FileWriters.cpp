#include "FileWriters.h"
#include <memory>

namespace FileWriters {

    FileWriter::FileWriter()
    {
        isInitiated = false;
    }

    FileWriter::~FileWriter()
    {
        if (IsInit())
            os.close();
    }

    HRESULT FileWriter::Init(WCHAR* targetDir)
    {
        HRESULT hr = S_OK;

        if (isInitiated)
            hr = E_NOT_VALID_STATE;

        if (SUCCEEDED(hr))
        {
            os.open(targetDir, std::ios::out | std::ios::binary | std::ios::app);
            hr = os.is_open() ? S_OK : E_HANDLE;
        }

        if (SUCCEEDED(hr))
            isInitiated = true;

        return hr;
    }

    HRESULT FileWriter::Reset()
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

    BOOL FileWriter::IsInit() const {
        return isInitiated;
    }

    HRESULT FileWriter::WriteFrame(FrameData* pData)
    {
        if (!IsInit())
            return E_NOT_VALID_STATE;
        
        os.write(reinterpret_cast<char const*> (pData->pBuffer), pData->dataSize);
        os.write(reinterpret_cast<char const*> (&(pData->timestamp)), sizeof(time_t));
    }
}