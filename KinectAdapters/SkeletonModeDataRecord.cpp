#include "SkeletonModeDataRecord.h"

SkeletonModeDataRecord::SkeletonModeDataRecord()
{
	this->isCleanupRequired = true;
	pBuffer = new SkeletonModeData[BODY_COUNT];
}

SkeletonModeDataRecord::SkeletonModeDataRecord(SkeletonModeData* const skeletonsDataArray, time_t timeFromRecordingStart)
{
	this->pBuffer = skeletonsDataArray;
	this->timestamp = timeFromRecordingStart;
}

SkeletonModeDataRecord::~SkeletonModeDataRecord()
{
	if (this->isCleanupRequired && this->pBuffer != nullptr)
		delete[] this->pBuffer;
}

HRESULT SkeletonModeDataRecord::WriteData(std::ofstream& os) const
{
	try
	{
		for (int i = 0; i < BODY_COUNT; i++)
		{
			auto* pSkeleton = pBuffer + i;
			os.write(reinterpret_cast<const char*>(&(pSkeleton->isTracked)), sizeof(bool));
			if (pSkeleton->isTracked)
			{
				os.write(reinterpret_cast<const char*>(&(pSkeleton->leftHandState)), sizeof(HandState));
				os.write(reinterpret_cast<const char*>(&(pSkeleton->rightHandState)), sizeof(HandState));
				os.write(reinterpret_cast<const char*>(pSkeleton->joints), this->singleSkeletonJointsSize);
			}
		}
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT SkeletonModeDataRecord::ReadData(std::ifstream& is)
{
	try
	{
		for (int i = 0; i < BODY_COUNT; i++)
		{
			auto* pSkeleton = pBuffer + i;
			is.read(reinterpret_cast<char*>(&(pSkeleton->isTracked)), sizeof(bool));
			if (pSkeleton->isTracked)
			{
				is.read(reinterpret_cast<char*>(&(pSkeleton->leftHandState)), sizeof(HandState));
				is.read(reinterpret_cast<char*>(&(pSkeleton->rightHandState)), sizeof(HandState));
				is.read(reinterpret_cast<char*>(pSkeleton->joints), this->singleSkeletonJointsSize);
			}
		}
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}

SkeletonModeData* SkeletonModeDataRecord::GetBufferPtr() const
{
	return this->pBuffer;
}
