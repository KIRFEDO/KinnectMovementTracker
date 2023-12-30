#include "FileReaderDepthMode.h"
#include "stdafx.h"

FileReaderDepthMode::FileReaderDepthMode()
{
    this->isInitiated = false;
}

FileReaderDepthMode::~FileReaderDepthMode()
{
    if (!isInitiated)
        is.close();

    delete[] m_targetDir;
}

HRESULT FileReaderDepthMode::Initiate(WCHAR* targetDir, const size_t& targetDirSize)
{
    HRESULT hr = S_OK;

    if (isInitiated)
        hr = E_NOT_VALID_STATE;

    if (SUCCEEDED(hr)) {
        //TODO is path copying necessary?
        m_targetDir = new WCHAR[targetDirSize];
        memcpy(m_targetDir, targetDir, targetDirSize * sizeof(WCHAR));
        m_targetDirLength = targetDirSize;

        //TODO add timestamp addition
        is.open(targetDir, std::ios::in | std::ios::binary);
        hr = is.is_open() ? S_OK : E_HANDLE;
    }

    if(SUCCEEDED(hr))
        isInitiated = true;
    return hr;
}

HRESULT FileReaderDepthMode::Reset()
{
    HRESULT hr = S_OK;
    if (!isInitiated)
        hr = E_NOT_VALID_STATE;
    if (SUCCEEDED(hr)) {
        delete[] m_targetDir;
        m_targetDir = nullptr;
        m_targetDirLength = 0;
        isInitiated = false;
        is.close();
    }
    return hr;
}

HRESULT FileReaderDepthMode::ReadFrame(UINT16** pBuffer)
{
    HRESULT hr = S_OK;
    if (!isInitiated)
        hr = E_NOT_VALID_STATE;

    auto bufferLength = getWidth() * getHeight();
    *pBuffer = new UINT16[getWidth() * getHeight()];
    if (SUCCEEDED(hr))
    {
        is.read(reinterpret_cast<char*>(*pBuffer), sizeof(UINT16)*bufferLength);
    }

    return hr;
}

int FileReaderDepthMode::getWidth() const
{
    return this->width;
}

int FileReaderDepthMode::getHeight() const
{
    return this->height;
}
