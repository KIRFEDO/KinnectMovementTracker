#pragma once

#include "framework.h"
#include "DepthMode.h"
#include "ImageRenderer.h"
#include "FileWriter.h"
#include "FileReaderDepthMode.h"
#include <d2d1.h>

using namespace FileWriter;
using namespace KinectAdapter;

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
		HRESULT InitKinectAdapters();
		HRESULT InitKinectRenderer();
		HRESULT InitApp();

		//Cyclic app functions
		HRESULT UpdateKinectImage(DepthModeData* res);
		HRESULT InitiateFileWriters();
		HRESULT ResetFileWriters();
		HRESULT WriteKinectData(DepthModeData* res);

		HWND m_hWndMain;									// main window
		HWND m_hWndKinect;									// window for kinect rendering
		HWND m_hWndOutputFilePath;							// edit text that have file path inside
		HWND m_hWndChoseOutputFileBtn;						// button for chosing the output file
		HWND m_hWndStartStopRecordingBtn;					// button for starting/stopping recording
		HINSTANCE m_hInstCurr;								// current instance
		int m_nCmdShow;										// main window show mode
		const WCHAR m_appTitle[18] = L"Movement recorder";
		DepthMode m_depthMode;
		ImageRenderer m_imageRenderer;
		RGBQUAD* m_pDepthRGBX;

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

