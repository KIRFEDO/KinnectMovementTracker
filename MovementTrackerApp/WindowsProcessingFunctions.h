#pragma once
#include "framework.h"
#include "stdafx.h"
#include "WindowIDs.h"
#include "Enums.h"

VIEW_MODE VIEW_MODE_FLAG = VIEW_MODE::DEPTH;
RECORD_DATA_STATE RECORD_DATA_FLAG = RECORD_DATA_STATE::DO_NOT_RECORD;
APP_MODE APP_MODE_FLAG = APP_MODE::LIVE;

void OnClickChoseFile(int notificationCode)
{
    return;
}

void OnClickStartStopRecording(int notificationCode)
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

void OnClickSwitchViewMode(int notificationCode)
{
    if (notificationCode == BN_CLICKED)
    {
        switch (VIEW_MODE_FLAG)
        {
            case VIEW_MODE::DEPTH:
                VIEW_MODE_FLAG = VIEW_MODE::SKELETON;
                break;
            case VIEW_MODE::SKELETON:
                VIEW_MODE_FLAG = VIEW_MODE::DEPTH;
                break;
        }
    }
}

void OnClickSwitchAppMode(int notificationCode, HWND hwnd)
{
    if (notificationCode == BN_CLICKED)
    {
        switch (APP_MODE_FLAG)
        {
            case APP_MODE::LIVE:
                APP_MODE_FLAG = APP_MODE::READING;
                break;
            case APP_MODE::READING:
                APP_MODE_FLAG = APP_MODE::LIVE;
                break;
        }
        auto staticAppMode = GetDlgItem(hwnd, STATIC_APP_MODE);
        std::wstring wstr_1 = L"Current mode: ";
        std::wstring wstr_2 = ToWString(APP_MODE_FLAG);
        auto res = wstr_1 + wstr_2;
        SetWindowTextW(staticAppMode, res.c_str());
    }
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
                    OnClickChoseFile(notificationCode);
                    return 0;
                }
                case BUTTON_START_STOP_RECORDING:
                {
                    OnClickStartStopRecording(notificationCode);
                    return 0;
                }
                case BUTTON_SWITCH_VIEW_MODE:
                {
                    OnClickSwitchViewMode(notificationCode);
                    return 0;
                }
                case BUTTON_SWITCH_APP_MODE:
                    OnClickSwitchAppMode(notificationCode, hwnd);
                    return 0;
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