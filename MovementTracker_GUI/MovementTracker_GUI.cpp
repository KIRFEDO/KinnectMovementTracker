// MovementTracker_GUI.cpp : Defines the entry point for the application.
//

#include <stdexcept>

#include "framework.h"
#include "KinectAdapter.h"
#include "ImageRenderer.h"

#include "App.h"

using namespace KinectAdapter;

#define MAX_LOADSTRING 100

// Global Variables:
HWND hWnd;                                      // main window
HWND hWndForKinectRendering;                    // window for kinect rendering
HINSTANCE hInst;                                // current instance
WCHAR szTitle[] = L"Movement recorder";                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class class_name

// Direct2D
IKinectMode* m_kinectAdapter;
ImageRenderer* m_pDrawDepth;
ID2D1Factory* m_pD2DFactory;
RGBQUAD* m_pDepthRGBX = new RGBQUAD[512 * 424];

//D2D legacy variables
INT64                   m_nStartTime;
INT64                   m_nLastCounter;
double                  m_fFreq = 10000000;
INT64                   m_nNextStatusTime;
DWORD                   m_nFramesSinceUpdate;
bool                    m_bSaveScreenshot;

// Forward declarations of functions included in this code module:
BOOL                InitKinectAdapterInstance();
BOOL                InitKinectRenderer();
void                UpdateKinectImage();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    App app(hInstance, nCmdShow);
    auto res = app.run();
    /*if (!InitKinectAdapterInstance())
    {
        return false;
    }
    if (!InitKinectRenderer())
    {
        return false;
    }*/

    //MSG msg = { 0 };

    ////TO DO set up look for periodic update without mouse movement
    //// Main message loop:
    //while (WM_QUIT != msg.message)
    //{
    //    UpdateKinectImage();

    //    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    //    {
    //        // If a dialog message will be taken care of by the dialog proc
    //        if (hWnd && IsDialogMessageW(hWnd, &msg))
    //        {
    //            continue;
    //        }

    //        TranslateMessage(&msg);
    //        DispatchMessageW(&msg);
    //    }
    //}
    return 0;
}

BOOL InitKinectAdapterInstance()
{
    m_kinectAdapter = ModeFactory::getMode(KinectMode::DEPTH);

    HRESULT hr = m_kinectAdapter != nullptr;

    hr = m_kinectAdapter->initiateKinectConnection();
    return SUCCEEDED(hr);
}

BOOL InitKinectRenderer()
{
    auto m_hWnd = hWndForKinectRendering;

    // Init Direct2D
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

    // Create and initialize a new Direct2D image renderer (take a look at ImageRenderer.h)
    // We'll use this to draw the data we receive from the Kinect to the screen

    //TO DO Replace size with dynamic values
    const int cDepthWidth = 512;
    const int cDepthHeight = 424;
    //------

    m_pDrawDepth = new ImageRenderer();
    HRESULT hr = m_pDrawDepth->Initialize(m_hWnd, m_pD2DFactory, cDepthWidth, cDepthHeight, cDepthWidth * sizeof(RGBQUAD));
    if (FAILED(hr))
    {
        return FALSE;
    }

   return TRUE;
}

void UpdateKinectImage()
{
    IKinectData* res = new IKinectData;
    HRESULT hr;
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
                BYTE intensity = static_cast<BYTE>((depth >= res->nDepthMinReliableDistance) && (depth <= res->nDepthMaxDistance) ? (depth % 256) : 0);

                pRGBX->rgbRed = intensity;
                pRGBX->rgbGreen = intensity;
                pRGBX->rgbBlue = intensity;

                ++pRGBX;
                ++pBuffer;
            }

            // Draw the data with Direct2D
            m_pDrawDepth->Draw(reinterpret_cast<BYTE*>(m_pDepthRGBX), res->nWidth * res->nHeight * sizeof(RGBQUAD));
        }
    }
    m_kinectAdapter->releaseSpecificResources();
}
