#include "stdafx.h"
#include "string"

class IWriter {
	virtual HRESULT writeFrame(UINT16* pBuffer, size_t bufferSize) = 0;		
};