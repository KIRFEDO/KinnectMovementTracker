#pragma once
#include <fstream>
#include <vector>
#include <utility>

#include "FrameData.h"
#include "FileWriters.h"
#include "stdafx.h"

using namespace FileWriters;

namespace KinectDataProcessors
{	
	std::vector<std::pair<time_t, time_t>> GetUsefullTimeSegments(const wchar_t* filePath) {
		std::ifstream is;
		is.open(filePath, std::ios::binary | std::ios::in);
		if (!is.is_open())
			throw std::runtime_error("Can`t open file");


	}
}