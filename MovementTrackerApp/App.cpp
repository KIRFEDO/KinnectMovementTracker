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
		HandleDataRecording(RECORD_DATA_FLAG, hr_depthMode, hr_skeletonMode, depthModeData, skeletonModeData);

		m_depthMode.ReleaseSpecificResources();
		m_skeletonMode.ReleaseSpecificResources();

		delete depthModeData;
		delete skeletonModeData;

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

	m_hWndKinect = CreateWindowW(m_kinectViewWindowName, (LPCTSTR)NULL, WS_CHILD | WS_VISIBLE,
		25, 25, 500, 500, m_hWndMain, nullptr, m_hInstCurr, nullptr);

	HRESULT hr;

	hr = InitStatic();
	if(SUCCEEDED(hr))
		hr = InitEdit();
	if (SUCCEEDED(hr))
		hr = InitButtons();	
	
	ShowWindow(m_hWndMain, m_nCmdShow) ? S_OK : E_FAIL;

	return hr;
}

HRESULT App::InitStatic()
{
	m_hWndStaticPatientName = CreateWindowW(L"static", L"Patient name:", WS_CHILD | WS_VISIBLE | SS_LEFT,
		staticXOffset, 25, staticXWidth, 25, m_hWndMain, (HMENU)STATIC_PATIENT_NAME, m_hInstCurr, nullptr);
	
	m_hWndStaticAdditonalInfo = CreateWindowW(L"static", L"Additional info:", WS_CHILD | WS_VISIBLE | SS_LEFT,
		staticXOffset, 75, staticXWidth, 250, m_hWndMain, (HMENU)STATIC_ADDITIONAL_INFO, m_hInstCurr, nullptr);
	
	m_hWndStaticFilePath = CreateWindowW(L"static", L"File path:", WS_CHILD | WS_VISIBLE | SS_LEFT,
		staticXOffset, 350, staticXWidth, 25, m_hWndMain, (HMENU)STATIC_ADDITIONAL_INFO, m_hInstCurr, nullptr);

	if (!(m_hWndStaticPatientName &&
		m_hWndStaticAdditonalInfo &&
		m_hWndStaticFilePath))
		return E_HANDLE;

	return S_OK;
}

HRESULT App::InitEdit()
{
	m_hWndEditPatientName = CreateWindowW(L"edit", L"Patient name", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		editXOffset, 25, editXWidth, 25, m_hWndMain, (HMENU)EDIT_FILE_PATH, m_hInstCurr, nullptr);

	m_hWndEditAdditionalInfo = CreateWindowW(L"edit", L"Additional info", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
		editXOffset, 75, editXWidth, 250, m_hWndMain, (HMENU)EDIT_ADDITIONAL_INFO, m_hInstCurr, nullptr);

	m_hWndEditOutputFilePath = CreateWindowW(L"edit", L"File path", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		editXOffset, 350, editXWidth, 25, m_hWndMain, (HMENU)EDIT_FILE_PATH, m_hInstCurr, nullptr);

	if (!(m_hWndEditPatientName &&
		m_hWndEditAdditionalInfo))
		return E_HANDLE;

	return S_OK;
}

HRESULT App::InitButtons()
{
	m_hWndBtnChoseOutputFile = CreateWindowW(L"button", L"Chose output file", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		editXOffset, 400, 140, 50, m_hWndMain, (HMENU)BUTTON_CHOOSE_FILE, m_hInstCurr, nullptr);

	m_hWndBtnStartStopRecording = CreateWindowW(L"button", L"Start/Stop recording", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		editXOffset+140+25, 400, 140, 50, m_hWndMain, (HMENU)BUTTON_START_STOP_RECORDING, m_hInstCurr, nullptr);

	m_hWndBtnSwitchViewMode = CreateWindowW(L"button", L"Switch view mode", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		25, 550, 150, 50, m_hWndMain, (HMENU)BUTTON_SWITCH_VIEW_MODE, m_hInstCurr, nullptr);

	if (!(m_hWndBtnChoseOutputFile &&
		m_hWndBtnStartStopRecording &&
		m_hWndBtnSwitchViewMode))
		return E_HANDLE;

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

HRESULT App::InitRecordingCounters()
{
	m_counterDepthModeFrames = 0;
	m_counterSkeletonModeFrames = 0;
	m_recordingStartTime = std::chrono::system_clock::now();
	return S_OK;
}

HRESULT App::InitFileWriters()
{
	HRESULT hr = E_FAIL;
	
	WCHAR dmPath[21] = L"C:/BuffEnv/depth.txt";
	WCHAR smPath[20] = L"C:/BuffEnv/skel.txt";
	hr = m_writerDepthMode.Init(dmPath);
	hr = m_writerSkeletonMode.Init(smPath);

	return hr;
}

HRESULT App::HandleDataRecording(RECORD_DATA_STATE recordingState, HRESULT hr_depthMode, HRESULT hr_skeletonMode, 
								DepthModeData* pDepthModeData, SkeletonModeData* pSkeletonModeData)
{
	switch (recordingState)
	{
		case RECORD_DATA_STATE::INITIATE_FILE_HANDLES:
			InitRecordingCounters();
			InitFileWriters();
			CreateHeaderFile();
			RECORD_DATA_FLAG = RECORD_DATA_STATE::RECORDING;
		case RECORD_DATA_STATE::RECORDING:
			WriteDepthModeData(pDepthModeData, hr_depthMode);
			WriteSkeletonModeData(pSkeletonModeData, hr_skeletonMode);
			break;
		case RECORD_DATA_STATE::FINISH_RECORDING:
			ResetFileWriters();
			RECORD_DATA_FLAG = RECORD_DATA_STATE::DO_NOT_RECORD;
	}

	return S_OK;
}

HRESULT App::ResetFileWriters()
{
	HRESULT hr = S_OK;
	hr = m_writerDepthMode.Reset();
	hr = m_writerSkeletonMode.Reset();
	return hr;
}

HRESULT App::WriteDepthModeData(DepthModeData* depthModeData, HRESULT hr_depthMode)
{
	HRESULT hr = hr_depthMode;
	if (SUCCEEDED(hr))
	{
		m_counterDepthModeFrames += 1;
		FrameData* data = new DepthModeFrameData(reinterpret_cast<const char*>(depthModeData->pBuffer), GetTimeFromRecordingStart());
		hr = m_writerDepthMode.WriteFrame(data);
		delete data;
	}

	return hr;
}

HRESULT App::WriteSkeletonModeData(SkeletonModeData* skeletonModeData, HRESULT hr_depthMode)
{
	HRESULT hr = hr_depthMode;
	if (SUCCEEDED(hr))
	{
		m_counterSkeletonModeFrames += 1;
		FrameData* data = new SkeletonModeFrameData(reinterpret_cast<const char*>(skeletonModeData->joints), GetTimeFromRecordingStart());
		hr = m_writerSkeletonMode.WriteFrame(data);
		delete data;
	}

	return hr;
}

time_t App::GetTimeFromRecordingStart()
{
	auto const now = std::chrono::system_clock::now();
	auto const diff = now - m_recordingStartTime;
	/*
		This division is performed due to the win32 limitations.
		Diff is reprenting time difference in 10^-7 seconds. On 32bit system
		max value of size_t is 4294967295 which is 429 secunds ~ 7 min. In order 
		to extend this limit we are limiting precision to 10^-3. 
	*/
	return diff.count() / 10000;
}

HRESULT App::CreateHeaderFile()
{
	return E_NOTIMPL;
}