#include "App.h"
#include "AppHelpers.h"

#define ReturnIfFails(func) {auto res = func; if(res!=S_OK){return res;}}

App::App(HINSTANCE hInstCurr, int nCmdShow)
{
	this->m_hInstCurr = hInstCurr;
	this->m_nCmdShow = nCmdShow;
}

HRESULT App::run()
{
	ReturnIfFails(
		InitializeAndRegisterMainWindowClass()
	)
	ReturnIfFails(
		DrawUI();
	)

	return S_OK;
}

App::~App()
{
	
}

HRESULT App::DrawUI()
{	
	/*
	 Fixed size 1280x700 used to get rid od of scaling issues further on
	*/
	m_hWndMain = CreateWindowW(m_mainClassName, m_appTitle, WS_VISIBLE,
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
	m_mainWindow.lpfnWndProc = MainWindowProcessor;
	m_mainWindow.hInstance = m_hInstCurr;
	m_mainWindow.lpszClassName = m_mainClassName;
	return RegisterClass(&m_mainWindow) == 0 ? E_FAIL : S_OK;
}
