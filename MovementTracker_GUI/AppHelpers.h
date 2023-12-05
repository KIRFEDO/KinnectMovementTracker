#pragma once
#include "framework.h"

LRESULT CALLBACK MainWindowProcessor(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    LRESULT result;

    switch (uMsg)
    {
        case WM_CREATE:
        {
            result = 0;

            break;
        }

        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);

            break;
        }
    }

    return result;
};