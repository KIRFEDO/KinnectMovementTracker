// MovementTracker_GUI.cpp : Defines the entry point for the application.
//

#include <stdexcept>

#include "framework.h"
#include "KinectAdapter.h"
#include "ImageRenderer.h"

#include "App.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    App app(hInstance, nCmdShow);
    auto res = app.run();
    return SUCCEEDED(res) ? 0 : -1;
}