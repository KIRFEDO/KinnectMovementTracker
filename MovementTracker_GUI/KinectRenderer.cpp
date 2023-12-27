//------------------------------------------------------------------------------
// <copyright file="ImageRenderer.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "KinectRenderer.h"

/// <summary>
/// Constructor
/// </summary>
KinectRenderer::KinectRenderer() : 
    m_hWnd(0),
    m_sourceWidth(0),
    m_sourceHeight(0),
    m_sourceStride(0),
    m_pD2DFactory(NULL), 
    m_pRenderTarget(NULL),
    m_pBitmap(0)
{
}

/// <summary>
/// Destructor
/// </summary>
KinectRenderer::~KinectRenderer()
{
    DiscardResources();
    SafeRelease(m_pD2DFactory);
}

/// <summary>
/// Ensure necessary Direct2d resources are created
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT KinectRenderer::EnsureResources()
{
    HRESULT hr = S_OK;

    if (NULL == m_pRenderTarget)
    {
        D2D1_SIZE_U size = D2D1::SizeU(m_sourceWidth, m_sourceHeight);

        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        // Create a hWnd render target, in order to render to the window set in initialize
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(m_hWnd, size),
            &m_pRenderTarget
        );

        if (FAILED(hr))
        {
            return hr;
        }

        // Create a bitmap that we can copy image data into and then render to the target
        hr = m_pRenderTarget->CreateBitmap(
            size, 
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
            &m_pBitmap 
        );

        // light green
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.27f, 0.75f, 0.27f), &m_pBrushJointTracked);

        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f), &m_pBrushJointInferred);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &m_pBrushBoneTracked);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 1.0f), &m_pBrushBoneInferred);

        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.5f), &m_pBrushHandClosed);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 0.5f), &m_pBrushHandOpen);
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 0.5f), &m_pBrushHandLasso);

        if (FAILED(hr))
        {
            SafeRelease(m_pRenderTarget);
            return hr;
        }
    }

    return hr;
}

/// <summary>
/// Dispose of Direct2d resources 
/// </summary>
void KinectRenderer::DiscardResources()
{
    SafeRelease(m_pRenderTarget);
    SafeRelease(m_pBitmap);

    SafeRelease(m_pBrushJointTracked);
    SafeRelease(m_pBrushJointInferred);
    SafeRelease(m_pBrushBoneTracked);
    SafeRelease(m_pBrushBoneInferred);

    SafeRelease(m_pBrushHandClosed);
    SafeRelease(m_pBrushHandOpen);
    SafeRelease(m_pBrushHandLasso);
}

D2D1_POINT_2F KinectRenderer::BodyToScreen(const CameraSpacePoint& bodyPoint, const SkeletonModeData* skeletonModeData)
{
    // Calculate the body's position on the screen
    DepthSpacePoint depthPoint = { 0 };
    skeletonModeData->m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

    float screenPointX = static_cast<float>(depthPoint.X * m_sourceWidth) / cDepthWidth;
    float screenPointY = static_cast<float>(depthPoint.Y * m_sourceHeight) / cDepthHeight;

    return D2D1::Point2F(screenPointX, screenPointY);
}

void KinectRenderer::DrawHand(HandState handState, const D2D1_POINT_2F& handPosition)
{
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(handPosition, c_HandSize, c_HandSize);

    switch (handState)
    {
        case HandState_Closed:
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandClosed);
            break;

        case HandState_Open:
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandOpen);
            break;

        case HandState_Lasso:
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandLasso);
            break;
    }
}

void KinectRenderer::DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1)
{
    TrackingState joint0State = pJoints[joint0].TrackingState;
    TrackingState joint1State = pJoints[joint1].TrackingState;

    // If we can't find either of these joints, exit
    if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
    {
        return;
    }

    // Don't draw if both points are inferred
    if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
    {
        return;
    }

    // We assume all drawn bones are inferred unless BOTH joints are tracked
    if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
    {
        m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneTracked, c_TrackedBoneThickness);
    }
    else
    {
        m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneInferred, c_InferredBoneThickness);
    }
}

void KinectRenderer::DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints)
{
    // Draw the bones

    // Torso
    DrawBone(pJoints, pJointPoints, JointType_Head, JointType_Neck);
    DrawBone(pJoints, pJointPoints, JointType_Neck, JointType_SpineShoulder);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_SpineMid);
    DrawBone(pJoints, pJointPoints, JointType_SpineMid, JointType_SpineBase);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderRight);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderLeft);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipRight);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipLeft);

    // Right Arm    
    DrawBone(pJoints, pJointPoints, JointType_ShoulderRight, JointType_ElbowRight);
    DrawBone(pJoints, pJointPoints, JointType_ElbowRight, JointType_WristRight);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_HandRight);
    DrawBone(pJoints, pJointPoints, JointType_HandRight, JointType_HandTipRight);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_ThumbRight);

    // Left Arm
    DrawBone(pJoints, pJointPoints, JointType_ShoulderLeft, JointType_ElbowLeft);
    DrawBone(pJoints, pJointPoints, JointType_ElbowLeft, JointType_WristLeft);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_HandLeft);
    DrawBone(pJoints, pJointPoints, JointType_HandLeft, JointType_HandTipLeft);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_ThumbLeft);

    // Right Leg
    DrawBone(pJoints, pJointPoints, JointType_HipRight, JointType_KneeRight);
    DrawBone(pJoints, pJointPoints, JointType_KneeRight, JointType_AnkleRight);
    DrawBone(pJoints, pJointPoints, JointType_AnkleRight, JointType_FootRight);

    // Left Leg
    DrawBone(pJoints, pJointPoints, JointType_HipLeft, JointType_KneeLeft);
    DrawBone(pJoints, pJointPoints, JointType_KneeLeft, JointType_AnkleLeft);
    DrawBone(pJoints, pJointPoints, JointType_AnkleLeft, JointType_FootLeft);

    // Draw the joints
    for (int i = 0; i < JointType_Count; ++i)
    {
        D2D1_ELLIPSE ellipse = D2D1::Ellipse(pJointPoints[i], c_JointThickness, c_JointThickness);

        if (pJoints[i].TrackingState == TrackingState_Inferred)
        {
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointInferred);
        }
        else if (pJoints[i].TrackingState == TrackingState_Tracked)
        {
            m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointTracked);
        }
    }
}

/// <summary>
/// Set the window to draw to as well as the video format
/// Implied bits per pixel is 32
/// </summary>
/// <param name="hWnd">window to draw to</param>
/// <param name="pD2DFactory">already created D2D factory object</param>
/// <param name="sourceWidth">width (in pixels) of image data to be drawn</param>
/// <param name="sourceHeight">height (in pixels) of image data to be drawn</param>
/// <param name="sourceStride">length (in bytes) of a single scanline</param>
/// <returns>indicates success or failure</returns>
HRESULT KinectRenderer::Init(HWND hWnd, ID2D1Factory* pD2DFactory, int sourceWidth, int sourceHeight, int sourceStride)
{
    if (NULL == pD2DFactory)
    {
        return E_INVALIDARG;
    }

    m_hWnd = hWnd;

    // One factory for the entire application so save a pointer here
    m_pD2DFactory = pD2DFactory;

    m_pD2DFactory->AddRef();

    // Get the frame size
    m_sourceWidth  = sourceWidth;
    m_sourceHeight = sourceHeight;
    m_sourceStride = sourceStride;

    m_pDepthRGBX = new RGBQUAD[cDepthWidth * cDepthHeight];

    return S_OK;
}

/// <summary>
/// Draws a 32 bit per pixel image of previously specified width, height, and stride to the associated hwnd
/// </summary>
/// <param name="pImage">image data in RGBX format</param>
/// <param name="cbImage">size of image data in bytes</param>
/// <returns>indicates success or failure</returns>
HRESULT KinectRenderer::RenderDepthMode(BYTE* pImage, unsigned long cbImage)
{
    // incorrectly sized image data passed in
    if (cbImage < ((m_sourceHeight - 1) * m_sourceStride) + (m_sourceWidth * 4))
    {
        return E_INVALIDARG;
    }

    // create the resources for this draw device
    // they will be recreated if previously lost
    HRESULT hr = EnsureResources();

    if (FAILED(hr))
    {
        return hr;
    }
    
    // Copy the image that was passed in into the direct2d bitmap
    hr = m_pBitmap->CopyFromMemory(NULL, pImage, m_sourceStride);

    if (FAILED(hr))
    {
        return hr;
    }
       
    m_pRenderTarget->BeginDraw();

    // Draw the bitmap stretched to the size of the window
    m_pRenderTarget->DrawBitmap(m_pBitmap);
            
    hr = m_pRenderTarget->EndDraw();

    // Device lost, need to recreate the render target
    // We'll dispose it now and retry drawing
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardResources();
    }

    return hr;
}

HRESULT KinectRenderer::DrawDepthMode(DepthModeData* res)
{
    HRESULT hr = E_FAIL;
    // Make sure we've received valid data
    if (m_pDepthRGBX && res->pBuffer && res->validFrame)
    {
        RGBQUAD* pRGBX = m_pDepthRGBX;

        // end pixel is start + width*height - 1
        auto pBuffer = res->pBuffer;
        const UINT16* pBufferEnd = res->pBuffer + (cDepthWidth * cDepthHeight);
        int localMax = INT_MIN;
        while (pBuffer < pBufferEnd)
        {
            USHORT depth = *pBuffer;
            if (depth > localMax)
                localMax = depth;
            ++pBuffer;
        }

        pBuffer = res->pBuffer;
        while (pBuffer < pBufferEnd)
        {
            USHORT depth = *pBuffer;
            float pixelVal = (float)depth / (float)localMax * 256;

            BYTE intensity = static_cast<BYTE>((depth >= res->nDepthMinReliableDistance) && (depth <= res->nDepthMaxDistance) ? pixelVal : 0);

            pRGBX->rgbRed = intensity;
            pRGBX->rgbGreen = intensity;
            pRGBX->rgbBlue = intensity;

            ++pRGBX;
            ++pBuffer;
        }

        RenderDepthMode(reinterpret_cast<BYTE*>(m_pDepthRGBX), res->nWidth * res->nHeight * sizeof(RGBQUAD));
        hr = S_OK;
    }
    return hr;
}

HRESULT KinectRenderer::DrawSkeletonMode(const SkeletonModeData* skeletonModeData)
{
    HRESULT hr = S_OK;
    m_pRenderTarget->BeginDraw();
    m_pRenderTarget->Clear();

    D2D1_POINT_2F jointPoints[JointType_Count];
    HandState leftHandState = skeletonModeData->leftHandState;
    HandState rightHandState = skeletonModeData->rightHandState;

    if (SUCCEEDED(hr))
    {
        for (int j = 0; j < JointType_Count; ++j)
        {
            jointPoints[j] = BodyToScreen(skeletonModeData->joints[j].Position, skeletonModeData);
        }

        DrawBody(skeletonModeData->joints, jointPoints);

        DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
        DrawHand(rightHandState, jointPoints[JointType_HandRight]);
    }
    m_pRenderTarget->EndDraw();
    return hr;
}

HRESULT KinectRenderer::UpdateKinectImage(const VIEW_MODE& viewMode, HRESULT hr_depthMode, HRESULT hr_skeletonMode, DepthModeData* depthModeData, SkeletonModeData* skeletonModeData)
{
    switch (viewMode) {
        case VIEW_MODE::DEPTH:
            if(SUCCEEDED(hr_depthMode))
                DrawDepthMode(depthModeData);
            break;
        case VIEW_MODE::SKELETON:
            if (SUCCEEDED(hr_skeletonMode))
                DrawSkeletonMode(skeletonModeData);
            break;
        default:
            break;
    }
    return S_OK;
}
