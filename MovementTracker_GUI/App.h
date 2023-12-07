#pragma once

#include "framework.h"

class App
{
public:
	App() = delete;
	App(App&) = delete;
	App(HINSTANCE hInstCurr, int nCmdShow);
	HRESULT run();
	~App();
private:
	HRESULT InitApp();
	HRESULT InitUI();
	HRESULT InitializeAndRegisterMainWindowClass();

	HWND m_hWndMain;									// main window
	HWND m_hWndKinect;									// window for kinect rendering
	HINSTANCE m_hInstCurr;								// current instance
	int m_nCmdShow;										// main window show mode
	const WCHAR m_appTitle[18] = L"Movement recorder";
	const WCHAR m_mainClassName[5] = L"main";
	WNDCLASS m_mainWindow;
};

