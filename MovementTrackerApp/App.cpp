#include "App.h"
#include "SkeletonModeData.h"
#include "DepthModeData.h"
#include "DepthModeDataRecord.h"
#include "WindowsProcessingFunctions.h"
#include "WindowIDs.h"
#include <tuple>
#include <memory>
#include "d2d1.h"

//TODO remove includes
#include <windows.h>

using namespace KinectAdapters;

VIEW_MODE VIEW_MODE_FLAG_READING;

App::App(HINSTANCE hInstCurr, int nCmdShow)
{
	m_hInstCurr = hInstCurr;
	m_nCmdShow = nCmdShow;
	m_recordingDirectoryPath = L"C:/BuffEnv/Live/";
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

	//std::wstring path = L"C:/BuffEnv/Live/skel.txt";
	/*std::wstring path = L"C:/BuffEnv/Processed/skelRaw.txt";
	std::wstring path = L"C:/BuffEnv/Processed/skelProc.txt";*/

	//std::wstring path = L"C:/BuffEnv/Live/depth.txt";
	//std::wstring path = L"C:/BuffEnv/Processed/depthRaw.txt";

	std::wstring path = L"C:\\Users\\kiril\\Desktop\\magisterka\\Recordings\\MovementTracker_Temp\\depth.txt";
	//std::wstring path = L"C:\\Users\\kiril\\Desktop\\magisterka\\Recordings\\MovementTracker_Temp\\skel.txt";

	VIEW_MODE_FLAG_READING = VIEW_MODE::DEPTH;

	hr = m_reader.Init(path.c_str());

	while (WM_QUIT != msg.message)
	{
		UpdateGUI();
		if (APP_MODE_FLAG == APP_MODE::LIVE)
			LiveModeIteration();
		else
			ReadingModeIteration();

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
		0, 0, 1280, 1000, nullptr, nullptr, m_hInstCurr, nullptr);

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
	InitStatic_UserControls();
	InitStatic_BodyTrackingStatus();

	return S_OK;
}

HRESULT App::InitStatic_UserControls()
{
	m_hWndStaticPatientName = CreateWindowW(L"static", L"Patient name:", WS_CHILD | WS_VISIBLE | SS_LEFT,
		xOffset1stCol, 25, xWidth1stCol, 25, m_hWndMain, (HMENU)STATIC_PATIENT_NAME, m_hInstCurr, nullptr);

	m_hWndStaticAdditonalInfo = CreateWindowW(L"static", L"Additional info:", WS_CHILD | WS_VISIBLE | SS_LEFT,
		xOffset1stCol, 75, xWidth1stCol, 250, m_hWndMain, (HMENU)STATIC_ADDITIONAL_INFO, m_hInstCurr, nullptr);

	m_hWndStaticFilePath = CreateWindowW(L"static", L"File path:", WS_CHILD | WS_VISIBLE | SS_LEFT,
		xOffset1stCol, 350, xWidth1stCol, 25, m_hWndMain, (HMENU)STATIC_ADDITIONAL_INFO, m_hInstCurr, nullptr);

	m_hWndStaticCurrentMode = CreateWindowW(L"static", L"Current mode: LIVE", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		xOffset3rdCol, 25, xWidth3rdCol, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);

	m_hWndStaticRecordingTime = CreateWindowW(L"static", L"0:00", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		xOffset2ndCol + 175 + 140 + 10, 400, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);

	m_hWndStaticReadingInformation = CreateWindowW(L"static", L"Recording...", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		xOffset2ndCol + 175, 460, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);

	if(!(m_hWndStaticPatientName	&&
		m_hWndStaticAdditonalInfo	&&
		m_hWndStaticFilePath		&&
		m_hWndStaticCurrentMode		&&
		m_hWndStaticRecordingTime	&&
		m_hWndStaticReadingInformation))
		return E_HANDLE;

	return S_OK;
}

HRESULT App::InitStatic_BodyTrackingStatus()
{
	m_hWndStaticBodyLabel1 = CreateWindowW(L"static", L"Body #1: ", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		190, 550, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyStatus1 = CreateWindowW(L"static", L"Untracked", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		350, 550, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyLabel2 = CreateWindowW(L"static", L"Body #2: ", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		190, 600, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyStatus2 = CreateWindowW(L"static", L"Untracked", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		350, 600, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyLabel3 = CreateWindowW(L"static", L"Body #3: ", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		190, 650, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyStatus3 = CreateWindowW(L"static", L"Untracked", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		350, 650, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyLabel4 = CreateWindowW(L"static", L"Body #4: ", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		190, 700, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyStatus4 = CreateWindowW(L"static", L"Untracked", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		350, 700, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyLabel5 = CreateWindowW(L"static", L"Body #5: ", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		190, 750, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyStatus5 = CreateWindowW(L"static", L"Untracked", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		350, 750, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyLabel6 = CreateWindowW(L"static", L"Body #6: ", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		190, 800, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);
	m_hWndStaticBodyStatus6 = CreateWindowW(L"static", L"Untracked", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		350, 800, 140, 50, m_hWndMain, (HMENU)STATIC_APP_MODE, m_hInstCurr, nullptr);

	if (!(
		m_hWndStaticBodyLabel1	&&
		m_hWndStaticBodyStatus1	&&
		m_hWndStaticBodyLabel2	&&
		m_hWndStaticBodyStatus2	&&
		m_hWndStaticBodyLabel2	&&
		m_hWndStaticBodyStatus2	&&
		m_hWndStaticBodyLabel3	&&
		m_hWndStaticBodyStatus3	&&
		m_hWndStaticBodyLabel4	&&
		m_hWndStaticBodyStatus4	&&
		m_hWndStaticBodyLabel5	&&
		m_hWndStaticBodyStatus5	&&
		m_hWndStaticBodyLabel6	&&
		m_hWndStaticBodyStatus6
		))
		return E_HANDLE;

	return S_OK;
}

HRESULT App::InitEdit()
{
	m_hWndEditPatientName = CreateWindowW(L"edit", L"Patient name", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		xOffset2ndCol, 25, xWidth2ndCol, 25, m_hWndMain, (HMENU)EDIT_PATIENT_NAME, m_hInstCurr, nullptr);

	m_hWndEditAdditionalInfo = CreateWindowW(L"edit", L"Additional info", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | ES_LEFT | WS_VSCROLL,
		xOffset2ndCol, 75, xWidth2ndCol, 250, m_hWndMain, (HMENU)EDIT_ADDITIONAL_INFO, m_hInstCurr, nullptr);

	m_hWndOutputPath = CreateWindowW(L"edit", L"Output path", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		xOffset2ndCol, 350, xWidth2ndCol, 25, m_hWndMain, (HMENU)EDIT_FILE_PATH, m_hInstCurr, nullptr);

	if (!(m_hWndEditPatientName &&
		m_hWndEditAdditionalInfo))
		return E_HANDLE;

	return S_OK;
}

HRESULT App::InitButtons()
{
	m_hWndBtnChoseOutputFile = CreateWindowW(L"button", L"Chose output folder", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		xOffset2ndCol, 400, 140, 50, m_hWndMain, (HMENU)BUTTON_CHOOSE_FILE, m_hInstCurr, nullptr);

	m_hWndBtnStartStopRecording = CreateWindowW(L"button", L"Start/Stop recording", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		xOffset2ndCol + 175, 400, 140, 50, m_hWndMain, (HMENU)BUTTON_START_STOP_RECORDING, m_hInstCurr, nullptr);

	m_hWndBtnSwitchAppMode = CreateWindowW(L"button", L"Switch app mode", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
		xOffset3rdCol, 75, xWidth3rdCol, 50, m_hWndMain, (HMENU)BUTTON_SWITCH_APP_MODE, m_hInstCurr, nullptr);
	
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

	HRESULT hr = m_kinectRenderer.Init(m_hWndKinect, m_skeletonMode.getCoordinateMapperPtr(), width, height, width * sizeof(RGBQUAD));
	
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
	timeInSec = 0;
	timeInMin = 0;
	m_recordingStartTime = std::chrono::system_clock::now();
	
	return S_OK;
}

HRESULT App::InitRecordingFolder()
{
	auto pathLength = GetWindowTextLengthW(m_hWndOutputPath);
	wchar_t* buffer = new wchar_t[pathLength+1];
	GetWindowTextW(m_hWndOutputPath, buffer, pathLength + 1);
	m_recordingDirectoryPath = buffer;
	m_recordingDirectoryPath += L"\\MovementTracker_Temp";
	return CreateDirectoryW(m_recordingDirectoryPath.c_str(), NULL) ? S_OK : E_FAIL;
}

HRESULT App::InitFileWriters()
{
	HRESULT hr = E_FAIL;
	
	std::wstring mdPath = m_recordingDirectoryPath + L"\\metadata.txt";
	std::wstring dmPath = m_recordingDirectoryPath + L"\\depth.txt";
	std::wstring smPath = m_recordingDirectoryPath + L"\\skel.txt";
	hr = m_writerMetadata.Init(mdPath.c_str());
	if(SUCCEEDED(hr))
		hr = m_writerDepthMode.Init(dmPath.c_str());
	if(SUCCEEDED(hr))
		hr = m_writerSkeletonMode.Init(smPath.c_str());
	
	if (FAILED(hr))
		ResetFileWriters();

	return hr;
}

HRESULT App::UpdateGUI()
{
	if (APP_MODE_FLAG == APP_MODE::LIVE)
	{
		bool enableWhenNotRecording = RECORD_DATA_FLAG == RECORD_DATA_STATE::DO_NOT_RECORD;
		auto showWhenRecording = enableWhenNotRecording ? SW_HIDE : SW_SHOW;

		EnableWindow(m_hWndEditPatientName, enableWhenNotRecording);
		EnableWindow(m_hWndEditAdditionalInfo, enableWhenNotRecording);
		EnableWindow(m_hWndOutputPath, enableWhenNotRecording);

		ShowWindow(m_hWndStaticReadingInformation, showWhenRecording);
		ShowWindow(m_hWndStaticRecordingTime, showWhenRecording);

		EnableWindow(m_hWndBtnSwitchAppMode, enableWhenNotRecording);
		EnableWindow(m_hWndBtnChoseOutputFile, enableWhenNotRecording);


		std::wstring recordingButtonText = showWhenRecording ? L"Stop recording" : L"Start recording";
		SetWindowText(m_hWndBtnStartStopRecording, recordingButtonText.c_str());
	}
	else
	{

	}
	return S_OK;
}

HRESULT App::LiveModeIteration()
{
	HRESULT hr_depthMode = E_FAIL;
	HRESULT hr_skeletonMode = E_FAIL;

	DepthModeData* depthModeData = nullptr;
	hr_depthMode = m_depthMode.getCurrentFrame(&depthModeData);

	SkeletonModeData* bodyTrackingData = new SkeletonModeData[BODY_COUNT];
	hr_skeletonMode = m_skeletonMode.getCurrentFrame(bodyTrackingData);
	if (SUCCEEDED(hr_skeletonMode))
		UpdateTrackingStates();
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

	m_kinectRenderer.UpdateKinectImage(VIEW_MODE_FLAG, hr_depthMode, hr_skeletonMode, depthModeData, bodyTrackingData);
	HandleDataRecording(RECORD_DATA_FLAG, hr_depthMode, hr_skeletonMode, depthModeData, bodyTrackingData);

	m_depthMode.ReleaseSpecificResources();
	m_skeletonMode.ReleaseSpecificResources();

	delete depthModeData;
	delete[] bodyTrackingData;

	return S_OK;
}

HRESULT App::ReadingModeIteration()
{
	//Snippet for opening one frame from the file below
	//In case of using this snippet additional delition of pBuffer is needed

	HRESULT hr_depthMode = S_OK;
	HRESULT hr_skeletonMode = E_FAIL;
	
	DepthModeDataRecord* frameData = new DepthModeDataRecord();

	m_reader.ReadFrame(frameData);

	DepthModeData dmData(frameData->GetBufferPtr(), 512, 424, 0);
	SkeletonModeData* smData = nullptr;
	m_kinectRenderer.UpdateKinectImage(VIEW_MODE_FLAG_READING, hr_depthMode, hr_skeletonMode, &dmData, smData);

	Sleep((DWORD) 30);

	return S_OK;
}

HRESULT App::HandleDataRecording(RECORD_DATA_STATE recordingState, HRESULT hr_depthMode, HRESULT hr_skeletonMode,
								DepthModeData* pDepthModeData, SkeletonModeData* pSkeletonModeData)
{
	switch (recordingState)
	{
		case RECORD_DATA_STATE::INITIATE_FILE_HANDLES:
			if (FAILED(InitRecordingFolder()))
			{
				RECORD_DATA_FLAG = RECORD_DATA_STATE::DO_NOT_RECORD;
				MessageBox(
					m_hWndMain,
					L"Can`t initiate recording in that file",
					L"File path is not valid",
					MB_OK | MB_ICONERROR
				);
				break;
			}
			InitRecordingCounters();
			if (FAILED(InitFileWriters()))
			{
				RECORD_DATA_FLAG = RECORD_DATA_STATE::DO_NOT_RECORD;
				ResetFileWriters();
				MessageBox(
					m_hWndMain,
					L"Can`t initiate file writers",
					L"File path is not valid",
					MB_OK | MB_ICONERROR
				);
				break;
			}
			CreateHeaderFile();
			RECORD_DATA_FLAG = RECORD_DATA_STATE::RECORDING;
			break;
		case RECORD_DATA_STATE::RECORDING:
			WriteDepthModeData(pDepthModeData, hr_depthMode);
			//WriteSkeletonModeData(pSkeletonModeData, hr_skeletonMode);
			UpdateRecordingTimer();
			break;
		case RECORD_DATA_STATE::FINISH_RECORDING:
			WriteCounters();
			ResetFileWriters();
			ResetRecordingTimer();
			RECORD_DATA_FLAG = RECORD_DATA_STATE::DO_NOT_RECORD;
			break;
	}

	return S_OK;
}

void App::UpdateTrackingStates()
{
	auto trackingState = new BOOLEAN[BODY_COUNT]{ FALSE };

	m_skeletonMode.GetTrackingState(trackingState);
	
	for (int i = 0; i < BODY_COUNT; i++)
	{
		auto isTracked = trackingState[i];
		
		auto trackingStateInfoHwnd = GetBodyTrackingStateStatic(i+1);
		std::wstring trackingStateInfoValue = isTracked == TRUE ? L"Tracked" : L"Untracked";

		constexpr int buff_size = 30;
		auto buff = new wchar_t[buff_size];
		GetWindowText(trackingStateInfoHwnd, buff, buff_size);

		auto res = wcscmp(buff, trackingStateInfoValue.c_str());

		if(res!=0)
			SetWindowText(trackingStateInfoHwnd, trackingStateInfoValue.c_str());
	}
	
	delete[] trackingState;
}

HRESULT App::ResetFileWriters()
{
	HRESULT hr = S_OK;
	hr = m_writerDepthMode.Reset();
	hr = m_writerSkeletonMode.Reset();
	hr = m_writerMetadata.Reset();
	return hr;
}

HRESULT App::ResetRecordingTimer()
{
	SetWindowText(m_hWndStaticRecordingTime, L"0:00");
	return S_OK;
}

HRESULT App::WriteDepthModeData(DepthModeData* depthModeData, HRESULT hr_depthMode)
{
	HRESULT hr = hr_depthMode;
	if (SUCCEEDED(hr))
	{
		m_counterDepthModeFrames += 1;
		DepthModeDataRecord dataRecord(depthModeData, GetTimeFromRecordingStart());
		hr = m_writerDepthMode.WriteFrame(&dataRecord);
	}

	return hr;
}

HRESULT App::WriteSkeletonModeData(SkeletonModeData* skeletonModeData, HRESULT hr_depthMode)
{
	HRESULT hr = hr_depthMode;
	if (SUCCEEDED(hr))
	{
		/*m_counterSkeletonModeFrames += 1;
		FrameData* data = new SkeletonModeFrameData(reinterpret_cast<char*>(skeletonModeData), GetTimeFromRecordingStart());
		hr = m_writerSkeletonMode.WriteFrame(data);
		delete data;*/
	}

	return hr;
}

HRESULT App::WriteCounters()
{
	return m_writerMetadata.WriteFrameCounters(m_counterDepthModeFrames, m_counterSkeletonModeFrames);
}

HRESULT App::UpdateRecordingTimer()
{
	size_t timeInSecNew = GetTimeFromRecordingStart() / 1000;
	size_t timeInMinNew = timeInSecNew / 60;
	if (timeInSecNew != timeInSec || timeInMinNew != timeInMinNew)
	{
		timeInSec = timeInSecNew;
		timeInMin = timeInMinNew;
		std::wstring wstr_timeInSec;
		if (timeInSecNew / 10 == 0) 
		{
			wstr_timeInSec = L"0" + std::to_wstring(timeInSecNew);
		}
		else
		{
			wstr_timeInSec = std::to_wstring(timeInSecNew);
		}
		std::wstring timeStr = std::to_wstring(timeInMinNew) + L":" + wstr_timeInSec;
		SetWindowText(m_hWndStaticRecordingTime, timeStr.c_str());
	}
	return S_OK;
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
	return diff.count() / 10000; //in ms
}

HWND App::GetBodyTrackingStateStatic(int pos)
{
	switch (pos)
	{
	case 1:
		return m_hWndStaticBodyStatus1;
	case 2:
		return m_hWndStaticBodyStatus2;
	case 3:
		return m_hWndStaticBodyStatus3;
	case 4:
		return m_hWndStaticBodyStatus4;
	case 5:
		return m_hWndStaticBodyStatus5;
	case 6:
		return m_hWndStaticBodyStatus6;
	default:
		throw std::runtime_error("Unhandled body id given");

	}
}

HRESULT App::CreateHeaderFile()
{
	return E_NOTIMPL;
}