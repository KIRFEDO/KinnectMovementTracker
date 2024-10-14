#pragma once
#include <fstream>

typedef long HRESULT;

class IFrameData {
public:
	virtual HRESULT WriteData(std::ofstream& os) const = 0;
	virtual HRESULT ReadData(std::ifstream& os) = 0;
};