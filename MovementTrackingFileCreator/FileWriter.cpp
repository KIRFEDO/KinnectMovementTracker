#include "FileWriter.h"
#include <memory>

HRESULT FileWriter::FileWriterDepthMode::Initiate(WCHAR* targetDir, const size_t& targetDirSize)
{
    HRESULT hr = S_OK;

    if (isInitiated)
        hr = E_NOT_VALID_STATE;

    if (SUCCEEDED(hr))
    {
        //TODO is path copying necessary?
        m_targetDir = new WCHAR[targetDirSize];
        memcpy(m_targetDir, targetDir, targetDirSize * sizeof(WCHAR));
        m_targetDirLength = targetDirSize;

        //TODO add timestamp addition
        os.open(targetDir, std::ios::out | std::ios::binary | std::ios::app);
        hr = os.is_open() ? S_OK : E_HANDLE;
    }

    if (SUCCEEDED(hr))
        isInitiated = true;
    return hr;
}

FileWriter::FileWriterDepthMode::FileWriterDepthMode()
{
    isInitiated = false;
    m_targetDir = nullptr;
    m_targetDirLength = 0;
    m_pBufferLength = m_width * m_height;
}

FileWriter::FileWriterDepthMode::~FileWriterDepthMode()
{
    if (!isInitiated)
        os.close();

    delete[] m_targetDir;
}

HRESULT FileWriter::FileWriterDepthMode::Reset()
{
    HRESULT hr = S_OK;
    if (!isInitiated)
        hr = E_NOT_VALID_STATE;
    if (SUCCEEDED(hr))
    {
        delete[] m_targetDir;
        m_targetDir = nullptr;
        m_targetDirLength = 0;
        isInitiated = false;
        os.close();
    }
    return hr;
}

HRESULT FileWriter::FileWriterDepthMode::WriteData(UINT16* pBuffer)
{
    if (!isInitiated)
        return E_NOT_VALID_STATE;
    //TODO check multi-frame recording
    os.write(reinterpret_cast<char const*> (pBuffer), sizeof(UINT16) * m_pBufferLength);
}
