#pragma once

#include "framework.h"
#include "DepthMode.h"
#include "SkeletonMode.h"
#include "KinectRenderer.h"
#include "FileWriter.h"
#include "FileReaderDepthMode.h"
#include <d2d1.h>

using namespace FileWriter;
using namespace KinectAdapters;

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
		HRESULT InitiateFileWriters();
		HRESULT ResetFileWriters();
		HRESULT CreateHeaderFile();
		HRESULT WriteKinectData(DepthModeData* res);

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
		FileWriterDepthMode m_writerDepthMode;

		//Readers
		FileReaderDepthMode m_readerDepthMode;
};

