#include "IFrameData.h"
#include "DepthModeData.h"
#include <windows.h>

class __declspec(dllexport) DepthModeDataRecord : public IFrameData
{   
public:
    DepthModeDataRecord();
    DepthModeDataRecord(size_t width, size_t height);
    DepthModeDataRecord(DepthModeData* const depthModeData, time_t TimeFromRecordingStart);
    UINT16* const GetBufferPtr() const;
    ~DepthModeDataRecord();

    time_t GetTimestamp() const;
    virtual HRESULT WriteData(std::ofstream& os) const override;
    virtual HRESULT ReadData(std::ifstream& is) override;
private:
    bool isCleanupRequired = false;
    UINT16* pBuffer = nullptr;
    size_t bufferSize;
    time_t timestamp;
};
