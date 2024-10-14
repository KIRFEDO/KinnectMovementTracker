#include "SkeletonModeData.h"
#include "IFrameData.h"

#include <iostream>

class __declspec(dllexport) SkeletonModeDataRecord : public IFrameData
{
public:
    SkeletonModeDataRecord();
    SkeletonModeDataRecord(SkeletonModeData* const skeletonsDataArray, time_t timeFromRecordingStart);
    ~SkeletonModeDataRecord();

    HRESULT WriteData(std::ofstream& os) const override;
    HRESULT ReadData(std::ifstream& is) override;

    SkeletonModeData* GetBufferPtr() const;

private:
    bool isCleanupRequired = false;
    const size_t singleSkeletonJointsSize = JointType_Count * sizeof(Joint);
    time_t timestamp;
    SkeletonModeData* pBuffer = nullptr;
};