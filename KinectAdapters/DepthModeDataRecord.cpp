#include "DepthModeDataRecord.h"
#include <memory>

DepthModeDataRecord::DepthModeDataRecord()
{
    this->bufferSize = 512 * 424 * sizeof(UINT16);
    this->pBuffer = new UINT16[this->bufferSize];
    isCleanupRequired = true;
}

DepthModeDataRecord::DepthModeDataRecord(size_t width, size_t height)
{
    this->bufferSize = width * height * sizeof(UINT16);
    this->pBuffer = new UINT16[this->bufferSize];
    isCleanupRequired = true;
}

DepthModeDataRecord::DepthModeDataRecord(DepthModeData* const depthModeData, time_t TimeFromRecordingStart)
{
    bufferSize = depthModeData->nWidth * depthModeData->nHeight * sizeof(UINT16);
    this->pBuffer = depthModeData->pBuffer;
    timestamp = TimeFromRecordingStart;
}

UINT16* const DepthModeDataRecord::GetBufferPtr() const
{
    return this->pBuffer;
}

DepthModeDataRecord::~DepthModeDataRecord()
{
    if (isCleanupRequired && pBuffer != nullptr)
        delete[] pBuffer;
}

time_t DepthModeDataRecord::GetTimestamp() const
{
    return this->timestamp;
}

HRESULT DepthModeDataRecord::WriteData(std::ofstream& os) const
{
    try
    {
        os.write(reinterpret_cast<const char*>(&(this->timestamp)), sizeof(time_t));
        os.write(reinterpret_cast<const char*>(this->pBuffer), this->bufferSize );
    }
    catch (...)
    {
        return E_FAIL;
    }
    return S_OK;
};

HRESULT DepthModeDataRecord::ReadData(std::ifstream& is)
{
    try
    {
        is.read(reinterpret_cast<char* const> (&(this->timestamp)), sizeof(time_t));
        is.read(reinterpret_cast<char* const> (this->pBuffer), this->bufferSize);
    }
    catch (...)
    {
        return E_FAIL;
    }
    return S_OK;
}