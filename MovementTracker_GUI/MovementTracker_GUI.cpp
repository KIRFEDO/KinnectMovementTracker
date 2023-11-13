// MovementTracker_GUI.cpp : Defines the entry point for the application.
//

#include <stdexcept>

#include "framework.h"
#include "MovementTracker_GUI.h"
#include "KinectAdapter.h"
#include "ImageRenderer.h"

using namespace KinectAdapter;

#define MAX_LOADSTRING 100

// Global Variables:
HWND hWnd;                                      // main window
HWND hWndForKinectRendering;                    // window for kinect rendering
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

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
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
BOOL                InitKinectAdapterInstance();
BOOL                InitKinectRenderer();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void UpdateKinectImage();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOVEMENTTRACKERGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return false;
    }
    if (!InitKinectAdapterInstance())
    {
        return false;
    }
    if (!InitKinectRenderer())
    {
        return false;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOVEMENTTRACKERGUI));

    MSG msg = { 0 };

    //TO DO set up look for periodic update without mouse movement
    // Main message loop:
    while (WM_QUIT != msg.message)
    {
        UpdateKinectImage();

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if (hWnd && IsDialogMessageW(hWnd, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOVEMENTTRACKERGUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOVEMENTTRACKERGUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
// 
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
      CW_USEDEFAULT, 0, 1000, 1000, nullptr, nullptr, hInstance, nullptr);

   hWndForKinectRendering = CreateWindowW(szWindowClass, szTitle, WS_CHILD | WS_VISIBLE,
       CW_USEDEFAULT, 0, 512, 424, hWnd, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL InitKinectAdapterInstance()
{
    m_kinectAdapter = ModeFactory::getMode(KinectMode::DEPTH);

    HRESULT hr = m_kinectAdapter != nullptr;

    hr = m_kinectAdapter->initiateKinectConnection();
    return SUCCEEDED(hr);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
// 
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
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

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//  Comment: Used for upper drop-down line
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void UpdateKinectImage()
{
    IKinectData* res = new IKinectData;
    RGBQUAD* pRGBX;
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
