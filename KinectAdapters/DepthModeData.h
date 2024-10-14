#pragma once
#include "basetsd.h"
#include "IFrameData.h"

typedef unsigned short USHORT;

struct __declspec(dllexport) DepthModeData {
    INT64 nTime;
    UINT16* pBuffer = nullptr;
    int nWidth;
    int nHeight;
    USHORT nDepthMinReliableDistance;
    USHORT nDepthMaxDistance;
    bool validFrame;

    DepthModeData() = delete;
    DepthModeData(UINT16* const pBuffer, int nWidth, int nHeight, USHORT nDepthMinReliableDistance, USHORT nDepthMaxDistance = USHRT_MAX)
    {
        size_t bufferSize = nWidth * nHeight * sizeof(UINT16);
        this->pBuffer = new UINT16[bufferSize];
        std::copy(pBuffer, pBuffer + bufferSize, this->pBuffer);
        this->nHeight = nHeight;
        this->nWidth = nWidth;
        this->nDepthMinReliableDistance = nDepthMinReliableDistance;
        this->nDepthMaxDistance = nDepthMaxDistance;
    }
    DepthModeData(
        INT64 nTime,
        UINT16* pBuffer,
        int nWidth,
        int nHeight,
        USHORT nDepthMinReliableDistance,
        USHORT nDepthMaxDistance,
        bool validFrame = false
    ) {
        this->nTime = nTime;
        this->pBuffer = pBuffer;
        this->nWidth = nWidth;
        this->nHeight = nHeight;
        this->nDepthMinReliableDistance = nDepthMinReliableDistance;
        this->nDepthMaxDistance = nDepthMaxDistance;
        this->validFrame = validFrame;
    }
};