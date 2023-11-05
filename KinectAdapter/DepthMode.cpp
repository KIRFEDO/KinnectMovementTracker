#include "DepthMode.h"

std::pair<int, int> DepthMode::getFrameSize() const
{
    return std::pair<int, int>(width, height);
}
