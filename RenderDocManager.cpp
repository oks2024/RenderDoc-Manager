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

#include "stdafx.h"
#include "RenderDocManager.h"
#include <string>

RenderDocManager::RenderDocManager(HWND p_Handle, LPCWSTR pRenderDocPath, LPCWSTR pCapturePath)
{
    m_Handle = p_Handle;
    m_CaptureStarted = false;

    m_RenderDocDLL = LoadLibrary(pRenderDocPath);

    //Init function pointers
    m_RenderDocGetAPIVersion = (pRENDERDOC_GetAPIVersion)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_GetAPIVersion");
    m_RenderDocSetLogFile = (pRENDERDOC_SetLogFile)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_SetLogFile");

    m_RenderDocSetCaptureOptions = (pRENDERDOC_SetCaptureOptions)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_SetCaptureOptions");
    m_RenderDocGetCapture = (pRENDERDOC_GetCapture)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_GetCapture");
    m_RenderDocSetActiveWindow = (pRENDERDOC_SetActiveWindow)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_SetActiveWindow");
    m_RenderDocTriggerCapture = (pRENDERDOC_TriggerCapture)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_TriggerCapture");
    m_RenderDocStartFrameCapture = (pRENDERDOC_StartFrameCapture)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_StartFrameCapture");
    m_RenderDocEndFrameCapture = (pRENDERDOC_EndFrameCapture)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_EndFrameCapture");

    m_RenderDocGetOverlayBits = (pRENDERDOC_GetOverlayBits)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_GetOverlayBits");
    m_RenderDocMaskOverlayBits = (pRENDERDOC_MaskOverlayBits)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_MaskOverlayBits");

    m_RenderDocSetFocusToggleKeys = (pRENDERDOC_SetFocusToggleKeys)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_SetFocusToggleKeys");
    m_RenderDocSetCaptureKeys = (pRENDERDOC_SetCaptureKeys)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_SetCaptureKeys");

    m_RenderDocInitRemoteAccess = (pRENDERDOC_InitRemoteAccess)GetRenderDocFunctionPointer(m_RenderDocDLL, "RENDERDOC_InitRemoteAccess");

    // Make sure that the code is compatible with the current installed version.
    if (RENDERDOC_API_VERSION != m_RenderDocGetAPIVersion())
    {
        OutputDebugString(L"Render doc API is not compatible !");
        FreeLibrary(m_RenderDocDLL);
        return;
    }

    m_RenderDocSetLogFile(pCapturePath);

    KeyButton captureKey = eKey_F12;

    m_RenderDocSetFocusToggleKeys(NULL, 0);
    m_RenderDocSetCaptureKeys(NULL, 0);

    // Uncomment to define a capture key.
    //KeyButton captureKey = eKey_F12;
    //m_RenderDocSetCaptureKeys(&captureKey, 1);

    CaptureOptions options;
    // These options might slow down your program, enable only the ones you need.
    options.CaptureCallstacks = true;
    options.CaptureAllCmdLists = true;
    options.SaveAllInitials = true;
    
    m_RenderDocSetCaptureOptions(&options);

    // Init remote access.
    m_SocketPort = 0;
    m_RenderDocInitRemoteAccess(&m_SocketPort);

    m_RenderDocMaskOverlayBits(eOverlay_None, eOverlay_None);
}

void RenderDocManager::StartFrameCapture()
{
    m_RenderDocStartFrameCapture(m_Handle);
    m_CaptureStarted = true;
}

// In some cases a capture can fail. It happens when Map() was called before the StartFrameCapture() and then Unmap() is called.
// It also happen if you start recording a command list before the StartFrameCapture() (unless you have the option
// CaptureAllCmdLists enabled).
// In these cases, m_RenderDocEndFrameCapture will return false and start capturing again until a capture succeed, unless 
// m_RenderDocEndFrameCapture is called again.
void RenderDocManager::EndFrameCapture()
{
    if(!m_CaptureStarted)
        return;

    if(m_RenderDocEndFrameCapture(m_Handle))
    {
        m_CaptureStarted = false;
        return;
    }
    
    OutputDebugString(L"Capture has failed !")
        ;
    // The capture has failed, calling m_RenderDocEndFrameCapture several time to make sure it won't keep capturing forever.
    while (!m_RenderDocEndFrameCapture(m_Handle))
    {
    }

    m_CaptureStarted = false;
    return;
}

RenderDocManager::~RenderDocManager(void)
{
    FreeLibrary(m_RenderDocDLL);
}

void* RenderDocManager::GetRenderDocFunctionPointer(HINSTANCE ModuleHandle, LPCSTR FunctionName)
{
    void* OutTarget = NULL;
    OutTarget = (void*)GetProcAddress(ModuleHandle, FunctionName);

    return OutTarget;
}
