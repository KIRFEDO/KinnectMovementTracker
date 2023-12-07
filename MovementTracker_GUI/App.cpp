#include "App.h"
#include "AppHelpers.h"

App::App(HINSTANCE hInstCurr, int nCmdShow)
{
	this->m_hInstCurr = hInstCurr;
	this->m_nCmdShow = nCmdShow;
}

HRESULT App::run()
{
	HRESULT hr;
	hr = InitApp();

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return hr;
}

App::~App()
{
	
}

HRESULT App::InitUI()
{	
	/*
	 Fixed size 1280x700 used to get rid od of scaling issues further on
	*/
	m_hWndMain = CreateWindowW(m_mainClassName, m_appTitle, WS_OVERLAPPEDWINDOW,
		0, 0, 1280, 700, nullptr, nullptr, m_hInstCurr, nullptr);

	auto res = GetLastError();

	m_hWndKinect = CreateWindowW(0, m_appTitle, WS_CHILD | WS_VISIBLE,
		CW_USEDEFAULT, 0, 512, 424, m_hWndMain, nullptr, m_hInstCurr, nullptr);


	if (!m_hWndMain)
	{
		return E_HANDLE;
	}

	ShowWindow(m_hWndMain, m_nCmdShow);
	UpdateWindow(m_hWndMain);

	return S_OK;
}

HRESULT App::InitializeAndRegisterMainWindowClass()
{
	m_mainWindow.style = CS_HREDRAW | CS_VREDRAW;
	m_mainWindow.lpfnWndProc = MainWindowProcessor;
	m_mainWindow.hInstance = m_hInstCurr;
	m_mainWindow.lpszClassName = m_mainClassName;
	m_mainWindow.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	return RegisterClass(&m_mainWindow) == 0 ? E_FAIL : S_OK;
}

HRESULT App::InitApp()
{
	HRESULT hr;
	hr = InitializeAndRegisterMainWindowClass();
	if (SUCCEEDED(hr))
		hr = InitUI();

	return hr;
}
