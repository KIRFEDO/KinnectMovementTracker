#pragma once
#include "basetsd.h"

typedef unsigned short USHORT;

struct __declspec(dllexport) DepthModeData {
    INT64 nTime;
    UINT16* pBuffer;
    int nWidth;
    int nHeight;
    USHORT nDepthMinReliableDistance;
    USHORT nDepthMaxDistance;
    bool validFrame;

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

    DepthModeData() {};
};