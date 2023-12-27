#include "App.h"
#include "SkeletonModeData.h"
#include "WindowsProcessingFunctions.h"
#include "WindowIDs.h"
#include <tuple>
#include <memory>
#include "d2d1.h"

using namespace KinectAdapters;

App::App(HINSTANCE hInstCurr, int nCmdShow)
{
	this->m_hInstCurr = hInstCurr;
	this->m_nCmdShow = nCmdShow;
}

App::~App()
{
}

HRESULT App::run()
{
	HRESULT hr;
	hr = InitApp();
	if (FAILED(hr))
		return hr;

	MSG msg = { 0 };

	while (WM_QUIT != msg.message)
	{
		HRESULT hr_depthMode = E_FAIL;
		HRESULT hr_skeletonMode = E_FAIL;

		DepthModeData* depthModeData = new DepthModeData();
		hr_depthMode = m_depthMode.getCurrentFrame(depthModeData);
		SkeletonModeData* skeletonModeData = new SkeletonModeData(m_skeletonMode.getCoordinateMapperPtr());
		hr_skeletonMode = m_skeletonMode.getCurrentFrame(skeletonModeData);
		
		/*{
			//Snippet for opening one frame from the file below
			//In case of using this snippet additional delition of pBuffer is needed
			WCHAR test[19] = L"C:/BuffEnv/res.txt";
			hr = m_readerDepthMode.Reset();
			hr = m_readerDepthMode.Initiate(test, 19);
			m_readerDepthMode.ReadFrame(&res->pBuffer);
			res->nWidth = 512;
			res->nHeight = 424;
			res->nDepthMinReliableDistance = 0;
			res->nDepthMaxDistance = INT16_MAX;
			hr = S_OK;
		}*/

		m_kinectRenderer.UpdateKinectImage(VIEW_MODE_FLAG, hr_depthMode, hr_skeletonMode, depthModeData, skeletonModeData);
		
		//TODO add handling of two modes
		//switch (RECORD_DATA_FLAG) 
		//{
		//	case RECORD_DATA_STATE::INITIATE_FILE_HANDLES:
		//		//TODO Add header creator
		//		InitiateFileWriters();
		//		RECORD_DATA_FLAG = RECORD_DATA_STATE::RECORDING;
		//	case RECORD_DATA_STATE::RECORDING:
		//		WriteKinectData(res);
		//		break;
		//	case RECORD_DATA_STATE::FINISH_RECORDING:
		//		//TODO compress all files into zip file. Create file with vector of time
		//		ResetFileWriters();
		//		RECORD_DATA_FLAG = RECORD_DATA_STATE::DO_NOT_RECORD;
		//}

		m_depthMode.ReleaseSpecificResources();
		m_skeletonMode.ReleaseSpecificResources();

		delete depthModeData;

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return hr;
}

HRESULT App::InitAndRegisterWindowClasses()
{
	HRESULT hr;

	m_mainWindow.style = CS_HREDRAW | CS_VREDRAW;
	m_mainWindow.lpfnWndProc = MainWindowProcessor;
	m_mainWindow.hInstance = m_hInstCurr;
	m_mainWindow.lpszClassName = m_mainWindowName;
	m_mainWindow.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	m_mainWindow.hCursor = LoadCursor(NULL, IDC_ARROW);

	hr = RegisterClass(&m_mainWindow) == 0 ? E_FAIL : S_OK;
	if (FAILED(hr))
		return hr;

	m_kinectViewWindow.lpszClassName = m_kinectViewWindowName;
	m_kinectViewWindow.lpfnWndProc = KinectViewProcessor;
	m_kinectViewWindow.hInstance = NULL;
	
	hr = RegisterClass(&m_kinectViewWindow);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT App::InitGUI()
{	
	/*
	 Fixed size 1280x700 used to get rid od of scaling issues further on
	*/
	m_hWndMain = CreateWindowW(m_mainWindowName, m_appTitle, WS_OVERLAPPEDWINDOW,
		0, 0, 1280, 700, nullptr, nullptr, m_hInstCurr, nullptr);

	int width, height;
	std::tie(width, height) = m_depthMode.getFrameSize();

	m_hWndKinect = CreateWindowW(m_kinectViewWindowName, (LPCTSTR)NULL, WS_CHILD | WS_VISIBLE,
		0, 0, 640, 700, m_hWndMain, nullptr, m_hInstCurr, nullptr);
	
	m_hWndOutputFilePath = CreateWindowW(L"edit", L"Hint?", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		800, 50, 250, 20, m_hWndMain, (HMENU)EDIT_FILE_PATH, m_hInstCurr, nullptr);
	
	m_hWndChoseOutputFileBtn = CreateWindowW(L"button", L"Chose output file", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		800, 120, 150, 50, m_hWndMain, (HMENU) BUTTON_CHOOSE_FILE, m_hInstCurr, nullptr);
	
	m_hWndStartStopRecordingBtn = CreateWindowW(L"button", L"Start/Stop recording", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		800, 190, 150, 50, m_hWndMain, (HMENU) BUTTON_START_STOP_RECORDING, m_hInstCurr, nullptr);
	
	m_hWndSwitchViewMode = CreateWindowW(L"button", L"Switch view mode", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		800, 260, 150, 50, m_hWndMain, (HMENU) BUTTON_SWITCH_VIEW_MODE, m_hInstCurr, nullptr);

	auto res = GetLastError();

	if (!m_hWndMain)
		return E_HANDLE;

	ShowWindow(m_hWndMain, m_nCmdShow);
	UpdateWindow(m_hWndMain);

	return S_OK;
}

HRESULT App::InitKinectAdapters()
{
	HRESULT hr;
	hr = m_depthMode.Init();
	if (SUCCEEDED(hr))
		hr = m_skeletonMode.Init();
	return hr;
}

HRESULT App::InitKinectRenderer()
{
	// Init Direct2D
	ID2D1Factory* m_pD2DFactory;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	int width, height;
	std::tie(width, height) = m_depthMode.getFrameSize();

	HRESULT hr = m_kinectRenderer.Init(m_hWndKinect, m_pD2DFactory, width, height, width * sizeof(RGBQUAD));
	
	return hr;
}

HRESULT App::InitApp()
{
	HRESULT hr;
	hr = InitAndRegisterWindowClasses();
	if (SUCCEEDED(hr))
		hr = InitKinectAdapters();
	if (SUCCEEDED(hr))
		hr = InitGUI();
	if (SUCCEEDED(hr))
		hr = InitKinectRenderer();

	return hr;
}

HRESULT App::InitiateFileWriters()
{
	HRESULT hr = E_FAIL;
	
	WCHAR test[19] = L"C:/BuffEnv/res.txt";
	hr = m_writerDepthMode.Initiate(test, 19);

	return hr;
}

HRESULT App::ResetFileWriters()
{
	HRESULT hr = S_OK;
	hr = m_writerDepthMode.Reset();
	return hr;
}

HRESULT App::WriteKinectData(DepthModeData* res)
{
	HRESULT hr = E_FAIL;
	hr = m_writerDepthMode.WriteData(res->pBuffer);
	return hr;
}