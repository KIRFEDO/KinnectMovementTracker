#pragma once
#include "framework.h"
#include "stdafx.h"
#include "WindowIDs.h"

enum RECORD_DATA_STATE
{
    DO_NOT_RECORD,
    INITIATE_FILE_HANDLES,
    RECORDING,
    FINISH_RECORDING
};

RECORD_DATA_STATE RECORD_DATA_FLAG = RECORD_DATA_STATE::DO_NOT_RECORD;

void handleButtonChoseFile(int notificationCode)
{
    return;
}

void handleButtonStartStopRecording(int notificationCode)
{
    if (notificationCode == BN_CLICKED)
    {
        switch (RECORD_DATA_FLAG)
        {
            case RECORD_DATA_STATE::DO_NOT_RECORD:
                RECORD_DATA_FLAG = RECORD_DATA_STATE::INITIATE_FILE_HANDLES;
                break;
            case RECORD_DATA_STATE::RECORDING:
                RECORD_DATA_FLAG = RECORD_DATA_STATE::FINISH_RECORDING;
                break;
        }
    }
    return;
}

LRESULT CALLBACK MainWindowProcessor(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_COMMAND:
        {
            auto childWindowID = LOWORD(wParam);
            auto notificationCode = HIWORD(wParam);
            switch (childWindowID)
            {
                case BUTTON_CHOOSE_FILE:
                {
                    handleButtonChoseFile(notificationCode);
                    return 0;
                }
                case BUTTON_START_STOP_RECORDING:
                {
                    handleButtonStartStopRecording(notificationCode);
                    return 0;
                }
            }
            return 0;
        }
        default:
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
};

LRESULT CALLBACK KinectViewProcessor(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            return 0;
        }
        default:
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
};