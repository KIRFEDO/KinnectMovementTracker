#pragma once

#include "framework.h"
#include "DepthMode.h"
#include "SkeletonMode.h"
#include "KinectRenderer.h"
#include "FileWriters.h"
#include "FileReaderDepthMode.h"
#include "KinectRenderer.h"
#include <d2d1.h>
#include <chrono>

using namespace FileWriters;
using namespace KinectAdapters;
using namespace Renderers;

class App
{
	public:
		App() = delete;
		App(App&) = delete;
		App(HINSTANCE hInstCurr, int nCmdShow);
		HRESULT run();
		~App();
	private:
		//Initializers
		HRESULT InitAndRegisterWindowClasses();
		HRESULT InitGUI();
		HRESULT InitStatic();
		HRESULT InitEdit();
		HRESULT InitButtons();
		HRESULT InitKinectAdapters();
		HRESULT InitKinectRenderer();
		HRESULT InitApp();

		//Cyclic app functions
		HRESULT HandleDataRecording(RECORD_DATA_STATE recordingState, HRESULT hr_depthMode, HRESULT hr_skeletonMode,
									DepthModeData* pDepthModeData, SkeletonModeData* pSkeletonModeData);

		//Recording functions
		HRESULT InitFileWriters();
		HRESULT InitRecordingCounters();
		HRESULT CreateHeaderFile();
		HRESULT ResetFileWriters();
		HRESULT WriteDepthModeData(DepthModeData* depthModeData, HRESULT hr_depthMode);
		HRESULT WriteSkeletonModeData(SkeletonModeData* skeletonModeData, HRESULT hr_skeletonMode);

		time_t GetTimeFromRecordingStart();

		HWND m_hWndMain;									// main window
		HWND m_hWndKinect;									// window for kinect rendering

		HWND m_hWndEditOutputFilePath;						// target file path
		HWND m_hWndEditPatientName;							// patient name
		HWND m_hWndEditAdditionalInfo;						// additional info

		HWND m_hWndBtnChoseOutputFile;						// chosing the output file
		HWND m_hWndBtnStartStopRecording;					// starting/stopping recording
		HWND m_hWndBtnSwitchViewMode;						// switching view mode

		HWND m_hWndStaticPatientName;						// patient name
		HWND m_hWndStaticAdditonalInfo;						// additional info
		HWND m_hWndStaticFilePath;							// file path

		//App offsets
		// //Static
		const UINT16 staticXOffset = 550;
		const UINT16 staticXWidth = 150;
		// //Edit
		const UINT16 editXOffset = staticXOffset + staticXWidth;
		const UINT16 editXWidth = 300;

		HINSTANCE m_hInstCurr;								// current instance
		int m_nCmdShow;										// main window show mode
		const WCHAR m_appTitle[18] = L"Movement recorder";
		KinectRenderer m_kinectRenderer;

		//Kinect adapters
		DepthMode m_depthMode;
		SkeletonMode m_skeletonMode;

		//App window classes
		const WCHAR m_mainWindowName[10] = L"mainClass";
		WNDCLASS m_mainWindow;

		const WCHAR m_kinectViewWindowName[11] = L"kinectView";
		WNDCLASS m_kinectViewWindow;

		//Writers
		FileWriter m_writerDepthMode;
		FileWriter m_writerSkeletonMode;

		//Readers
		FileReaderDepthMode m_readerDepthMode;

		//Recording data
		size_t m_counterDepthModeFrames;
		size_t m_counterSkeletonModeFrames;
		std::chrono::time_point<std::chrono::system_clock> m_recordingStartTime;
};

