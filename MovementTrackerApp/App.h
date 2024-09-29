#pragma once

#include "framework.h"
#include "DepthMode.h"
#include "SkeletonMode.h"
#include "KinectRenderer.h"
#include "FileWriters.h"
#include "FileReaders.h"
#include "KinectRenderer.h"
#include <d2d1.h>
#include <chrono>

using namespace FileWriters;
using namespace FileReaders;
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
		HRESULT InitStatic_UserControls();
		HRESULT InitStatic_BodyTrackingStatus();

		HRESULT InitEdit();
		HRESULT InitButtons();
		HRESULT InitKinectAdapters();
		HRESULT InitKinectRenderer();
		HRESULT InitApp();

		//Cyclic app functions
		HRESULT UpdateGUI();
		HRESULT LiveModeIteration();
		HRESULT ReadingModeIteration();
		HRESULT HandleDataRecording(RECORD_DATA_STATE recordingState, HRESULT hr_depthMode, HRESULT hr_skeletonMode,
									DepthModeData* pDepthModeData, SkeletonModeData* pSkeletonModeData);
		void UpdateTrackingStates();

		//Recording functions
		HRESULT InitRecordingFolder();
		HRESULT InitFileWriters();
		HRESULT InitRecordingCounters();
		HRESULT CreateHeaderFile();
		HRESULT ResetFileWriters();
		HRESULT ResetRecordingTimer();
		HRESULT WriteDepthModeData(DepthModeData* depthModeData, HRESULT hr_depthMode);
		HRESULT WriteSkeletonModeData(SkeletonModeData* skeletonModeData, HRESULT hr_skeletonMode);
		HRESULT WriteCounters();
		HRESULT UpdateRecordingTimer();
		time_t GetTimeFromRecordingStart();

		//Helper functions
		HWND GetBodyTrackingStateStatic(int pos);

		//Window elements
		HWND m_hWndMain;									// main window
		HWND m_hWndKinect;									// window for kinect rendering

		HWND m_hWndOutputPath;								// target output path
		HWND m_hWndEditPatientName;							// patient name
		HWND m_hWndEditAdditionalInfo;						// additional info

		HWND m_hWndBtnChoseOutputFile;						// chosing the output file
		HWND m_hWndBtnStartStopRecording;					// starting/stopping recording
		HWND m_hWndBtnSwitchAppMode;						// switching app mode
		HWND m_hWndBtnSwitchViewMode;						// switching view mode

		HWND m_hWndStaticPatientName;						// patient name
		HWND m_hWndStaticAdditonalInfo;						// additional info
		HWND m_hWndStaticFilePath;							// file path
		HWND m_hWndStaticCurrentMode;						// current app mode info
		HWND m_hWndStaticReadingInformation;				// information about reading process
		HWND m_hWndStaticRecordingTime;						// recording time

		HWND m_hWndStaticBodyLabel1;
		HWND m_hWndStaticBodyStatus1;
		HWND m_hWndStaticBodyLabel2;
		HWND m_hWndStaticBodyStatus2;
		HWND m_hWndStaticBodyLabel3;
		HWND m_hWndStaticBodyStatus3;
		HWND m_hWndStaticBodyLabel4;
		HWND m_hWndStaticBodyStatus4;
		HWND m_hWndStaticBodyLabel5;
		HWND m_hWndStaticBodyStatus5;
		HWND m_hWndStaticBodyLabel6;
		HWND m_hWndStaticBodyStatus6;

		//App offsets
		const UINT16 xOffset1stCol = 550;
		const UINT16 xWidth1stCol = 150;
		
		const UINT16 xOffset2ndCol = xOffset1stCol + xWidth1stCol;
		const UINT16 xWidth2ndCol = 300;

		const UINT16 xOffset3rdCol = xOffset2ndCol + xWidth2ndCol+25;
		const UINT16 xWidth3rdCol = 200;

		//Needed private members
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
		MetadataWriter m_writerMetadata;
		KinectWriter m_writerDepthMode;
		KinectWriter m_writerSkeletonMode;

		//Readers
		KinectReader m_reader;

		//Recording data
		std::wstring m_recordingDirectoryPath;
		size_t m_counterDepthModeFrames;
		size_t m_counterSkeletonModeFrames;
		std::chrono::time_point<std::chrono::system_clock> m_recordingStartTime;
		size_t timeInSec;
		size_t timeInMin;
};