/******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2014 Fredrik Lindh
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
******************************************************************************/

#pragma once
#include "renderdoc_app.h"

class RenderDocManager
{
public:
    RenderDocManager(HWND p_Handle, LPCWSTR pRenderDocPath, LPCWSTR pCapturePath);
    ~RenderDocManager(void);
    void StartFrameCapture();
    void EndFrameCapture();

private:
    HINSTANCE m_RenderDocDLL;
    UINT32 m_SocketPort;
    HWND m_Handle;
    bool m_CaptureStarted;

    //General
    pRENDERDOC_GetAPIVersion m_RenderDocGetAPIVersion;
    pRENDERDOC_SetLogFile m_RenderDocSetLogFile;

    //Capture
    pRENDERDOC_SetCaptureOptions m_RenderDocSetCaptureOptions;
    pRENDERDOC_GetCapture m_RenderDocGetCapture;
    pRENDERDOC_SetActiveWindow m_RenderDocSetActiveWindow;
    pRENDERDOC_TriggerCapture m_RenderDocTriggerCapture;
    pRENDERDOC_StartFrameCapture m_RenderDocStartFrameCapture;
    pRENDERDOC_EndFrameCapture m_RenderDocEndFrameCapture;

    //Overlay
    pRENDERDOC_GetOverlayBits m_RenderDocGetOverlayBits;
    pRENDERDOC_MaskOverlayBits m_RenderDocMaskOverlayBits;

    //Hotkeys
    pRENDERDOC_SetFocusToggleKeys m_RenderDocSetFocusToggleKeys;
    pRENDERDOC_SetCaptureKeys m_RenderDocSetCaptureKeys;

    //Remote access
    pRENDERDOC_InitRemoteAccess m_RenderDocInitRemoteAccess;

    void* GetRenderDocFunctionPointer(HINSTANCE ModuleHandle, LPCSTR FunctionName);
};

