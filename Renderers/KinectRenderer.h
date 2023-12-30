//------------------------------------------------------------------------------
// <copyright file="ImageRenderer.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the drawing of image data

#pragma once

#include <d2d1.h>
#include "DepthModeData.h"
#include "SkeletonModeData.h"
#include "Enums.h"

static const int cDepthWidth = 512;
static const int cDepthHeight = 424;

static const float c_JointThickness = 3.0f;
static const float c_TrackedBoneThickness = 6.0f;
static const float c_InferredBoneThickness = 1.0f;
static const float c_HandSize = 30.0f;

namespace Renderers
{

    class __declspec(dllexport) KinectRenderer
    {
        public:
            /// <summary>
            /// Constructor
            /// </summary>
            KinectRenderer();

            /// <summary>
            /// Destructor
            /// </summary>
            virtual ~KinectRenderer();

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
            HRESULT Init(HWND hwnd, ID2D1Factory* pD2DFactory, int sourceWidth, int sourceHeight, int sourceStride);



            /// <summary>
            /// Draws a body 
            /// </summary>
            /// <param name="pJoints">joint data</param>
            /// <param name="pJointPoints">joint positions converted to screen space</param>
            HRESULT DrawSkeletonMode(const SkeletonModeData* pJoints);

            HRESULT UpdateKinectImage(const VIEW_MODE& viewMode, HRESULT hr_depthMode, HRESULT hr_skeletonMode, DepthModeData* res, SkeletonModeData* joints);

        private:
            HWND                     m_hWnd;

            // Format information
            UINT                     m_sourceHeight;
            UINT                     m_sourceWidth;
            LONG                     m_sourceStride;

            // Direct2D 
            ID2D1Factory* m_pD2DFactory;
            ID2D1HwndRenderTarget* m_pRenderTarget;
            ID2D1Bitmap* m_pBitmap;
            RGBQUAD* m_pDepthRGBX;

            // Body/hand drawing
            ID2D1SolidColorBrush* m_pBrushJointTracked;
            ID2D1SolidColorBrush* m_pBrushJointInferred;
            ID2D1SolidColorBrush* m_pBrushBoneTracked;
            ID2D1SolidColorBrush* m_pBrushBoneInferred;
            ID2D1SolidColorBrush* m_pBrushHandClosed;
            ID2D1SolidColorBrush* m_pBrushHandOpen;
            ID2D1SolidColorBrush* m_pBrushHandLasso;

            HRESULT EnsureResources();
            void DiscardResources();

            //Depth mode functions
            HRESULT RenderDepthMode(BYTE* pImage, unsigned long cbImage);
            HRESULT DrawDepthMode(DepthModeData* res);

            //Skeleton mode functions
            D2D1_POINT_2F BodyToScreen(const CameraSpacePoint& bodyPoint, const SkeletonModeData* skeletonModeData);
            void DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);
            void DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);
            void DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);

            UINT16 localDepthMax = 0;
    };
}