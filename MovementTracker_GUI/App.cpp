#include "App.h"
#include "AppHelpers.h"
#include <tuple>
#include "d2d1.h"

using namespace KinectAdapter;

App::App(HINSTANCE hInstCurr, int nCmdShow)
{
	m_kinectAdapter = nullptr;
	m_pDepthRGBX = nullptr;

	this->m_hInstCurr = hInstCurr;
	this->m_nCmdShow = nCmdShow;
}

HRESULT App::run()
{
	HRESULT hr;
	hr = InitApp();

	MSG msg = { 0 };

	// Main message loop:
	while (WM_QUIT != msg.message)
	{
		UpdateKinectImage();
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return hr;
}

App::~App()
{
	
}

HRESULT App::InitAndRegisterWindowClasses()
{
	HRESULT hr;

	m_mainWindow.style = CS_HREDRAW | CS_VREDRAW;
	m_mainWindow.lpfnWndProc = MainWindowProcessor;
	m_mainWindow.hInstance = m_hInstCurr;
	m_mainWindow.lpszClassName = m_mainWindowName;
	m_mainWindow.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	hr = RegisterClass(&m_mainWindow) == 0 ? E_FAIL : S_OK;
	if (FAILED(hr))
		return hr;

	m_kinectViewWindow.lpszClassName = m_kinectViewWindowName;
	m_kinectViewWindow.lpfnWndProc = KinectViewProcessor;
	m_kinectViewWindow.hInstance = NULL;
	
	hr = RegisterClass(&m_kinectViewWindow);
	if (FAILED(hr))
		return hr;

	auto res = GetLastError();

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
	std::tie(width, height) = m_kinectAdapter->getFrameSize();

	m_hWndKinect = CreateWindowW(m_kinectViewWindowName, (LPCTSTR)NULL, WS_CHILD | WS_VISIBLE,
		0, 0, width, height, m_hWndMain, nullptr, m_hInstCurr, nullptr);

	auto res = GetLastError();

	if (!m_hWndMain)
	{
		return E_HANDLE;
	}

	ShowWindow(m_hWndMain, m_nCmdShow);
	UpdateWindow(m_hWndMain);

	return S_OK;
}

HRESULT App::InitKinectConnection()
{
	HRESULT hr;
	if (m_kinectAdapter != nullptr) {
		delete m_kinectAdapter;
		m_kinectAdapter = nullptr;
	}
	m_kinectAdapter = ModeFactory::getMode(KinectMode::DEPTH);
	hr = m_kinectAdapter != nullptr ? S_OK : E_POINTER;
	if (SUCCEEDED(hr))
		hr = m_kinectAdapter->initiateKinectConnection();
	return hr;
}

HRESULT App::InitKinectRenderer()
{
	// Init Direct2D
	ID2D1Factory* m_pD2DFactory;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	int width, height;
	std::tie(width, height) = m_kinectAdapter->getFrameSize();
	m_pDepthRGBX = new RGBQUAD[width * height];

	HRESULT hr = m_imageRenderer.Initialize(m_hWndKinect, m_pD2DFactory, width, height, width * sizeof(RGBQUAD));
	
	return hr;
}

HRESULT App::InitApp()
{
	HRESULT hr;
	hr = InitAndRegisterWindowClasses();
	if (SUCCEEDED(hr))
		hr = InitKinectConnection();
	if (SUCCEEDED(hr))
		hr = InitGUI();
	if (SUCCEEDED(hr))
		hr = InitKinectRenderer();

	return hr;
}

HRESULT App::UpdateKinectImage()
{
	/*
		TODO add handling of empty kinect view instance
	*/ 
	HRESULT hr;
	IKinectData* res = new IKinectData();
	hr = m_kinectAdapter->getCurrentFrame(res);
	if (SUCCEEDED(hr)) {

		// Make sure we've received valid data
		if (m_pDepthRGBX && res->pBuffer && res->validFrame)
		{
			RGBQUAD* pRGBX = m_pDepthRGBX;

			// end pixel is start + width*height - 1
			const UINT16* pBufferEnd = res->pBuffer + (res->nWidth * res->nHeight);
			auto pBuffer = res->pBuffer;

			while (pBuffer < pBufferEnd)
			{
				USHORT depth = *pBuffer;

				// To convert to a byte, we're discarding the most-significant
				// rather than least-significant bits.
				// We're preserving detail, although the intensity will "wrap."
				// Values outside the reliable depth range are mapped to 0 (black).

				// Note: Using conditionals in this loop could degrade performance.
				// Consider using a lookup table instead when writing production code.

				//TODO properly handle death values
				auto depth_max = 250;

				BYTE intensity = static_cast<BYTE>((depth >= res->nDepthMinReliableDistance) && (depth <= res->nDepthMaxDistance) ? (depth % depth_max) : 0);

				pRGBX->rgbRed = intensity;
				pRGBX->rgbGreen = intensity;
				pRGBX->rgbBlue = intensity;

				++pRGBX;
				++pBuffer;
			}

			// Draw the data with Direct2D
			m_imageRenderer.Draw(reinterpret_cast<BYTE*>(m_pDepthRGBX), res->nWidth * res->nHeight * sizeof(RGBQUAD));
		}
	}
	m_kinectAdapter->releaseSpecificResources();
	return hr;
}