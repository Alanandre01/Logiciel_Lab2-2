// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include <new>
#include <windows.h>
#include <dshow.h>
#include <string>
#include "playback.h"
#include "video.h"

using namespace std;

DShowPlayer::DShowPlayer(string FileName) :
    m_state(STATE_NO_GRAPH),
    //m_hwnd(hwnd),
    m_pGraph(NULL),
    m_pControl(NULL),
    m_pEvent(NULL),
    m_pVideo(NULL),
    pSeek(NULL)
{
    HRESULT hr = InitializeGraph(FileName);
}

DShowPlayer::~DShowPlayer()
{
    TearDownGraph();
}

HRESULT DShowPlayer::Play()
{
    if (m_state != STATE_PAUSED && m_state != STATE_STOPPED)
    {
        return VFW_E_WRONG_STATE;
    }

    HRESULT hr = m_pControl->Run();
    if (SUCCEEDED(hr))
    {
        m_state = STATE_RUNNING;
    }
    return hr;
}

HRESULT DShowPlayer::Pause()
{
    if (m_state != STATE_RUNNING)
    {
        return VFW_E_WRONG_STATE;
    }

    HRESULT hr = m_pControl->Pause();
    if (SUCCEEDED(hr))
    {
        m_state = STATE_PAUSED;
    }
    return hr;
}

HRESULT DShowPlayer::Stop()
{
    if (m_state != STATE_RUNNING && m_state != STATE_PAUSED)
    {
        return VFW_E_WRONG_STATE;
    }

    HRESULT hr = m_pControl->Stop();
    if (SUCCEEDED(hr))
    {
        m_state = STATE_STOPPED;
    }
    return hr;
}

HRESULT DShowPlayer::SetRate(double dRate)
{
    HRESULT hr = pSeek->SetRate(dRate);
    if (SUCCEEDED(hr))
    {
        m_state = STATE_RUNNING;
    }
    return hr;
}

HRESULT DShowPlayer::SetPositions(LONGLONG* pCurrent)
{
    HRESULT hr = pSeek->SetPositions(pCurrent, AM_SEEKING_RelativePositioning, NULL, AM_SEEKING_NoPositioning);
    if (SUCCEEDED(hr))
    {
        m_state = STATE_RUNNING;
    }
    return hr;
}


// EVR/VMR functionality

BOOL DShowPlayer::HasVideo() const
{
    return (m_pVideo && m_pVideo->HasVideo());
}

// Sets the destination rectangle for the video.

HRESULT DShowPlayer::UpdateVideoWindow(const LPRECT prc)
{
    if (m_pVideo)
    {
        return m_pVideo->UpdateVideoWindow(m_hwnd, prc);
    }
    else
    {
        return S_OK;
    }
}

// Repaints the video. Call this method when the application receives WM_PAINT.

HRESULT DShowPlayer::Repaint(HDC hdc)
{
    if (m_pVideo)
    {
        return m_pVideo->Repaint(m_hwnd, hdc);
    }
    else
    {
        return S_OK;
    }
}


// Notifies the video renderer that the display mode changed.
//
// Call this method when the application receives WM_DISPLAYCHANGE.

HRESULT DShowPlayer::DisplayModeChanged()
{
    if (m_pVideo)
    {
        return m_pVideo->DisplayModeChanged();
    }
    else
    {
        return S_OK;
    }
}


// Graph building

// Create a new filter graph. 
HRESULT DShowPlayer::InitializeGraph(string FileName)
{
    int size;
    const WCHAR* NewFileName;
    int ssize = (int)FileName.length() + 1;
    size = MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), ssize, NULL, 0);
    NewFileName = new WCHAR[size];
    MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), ssize, (LPWSTR)NewFileName, size);

    TearDownGraph();

    // Create the Filter Graph Manager.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        goto done;
    }
    hr = CoCreateInstance(CLSID_FilterGraph, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pGraph));

    if (FAILED(hr))
    {
      
        printf("ERROR - Could not create the Filter Graph Manager.");
        
       
        goto done;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pControl));
    if (FAILED(hr))
    {
        printf("ERROR1");
        goto done;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&pSeek));
    if (FAILED(hr))
    {
        printf("ERROR2");
        goto done;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pEvent));
    if (FAILED(hr))
    {
        printf("ERROR3");
        goto done;
    }

    hr = m_pGraph->RenderFile(NewFileName, NULL);
    hr = m_pControl->Run();
    m_state = STATE_RUNNING;
    delete[] NewFileName;

done:
    
    return hr;
}

void DShowPlayer::TearDownGraph()
{
    // Stop sending event messages
    if (m_pEvent)
    {
        m_pEvent->SetNotifyWindow((OAHWND)NULL, NULL, NULL);
    }

    SafeRelease(&m_pGraph);
    SafeRelease(&m_pControl);
    SafeRelease(&m_pEvent);

    delete m_pVideo;
    m_pVideo = NULL;

    m_state = STATE_NO_GRAPH;
}


HRESULT DShowPlayer::CreateVideoRenderer()
{
    HRESULT hr = E_FAIL;

    enum { Try_EVR, Try_VMR9, Try_VMR7 };

    for (DWORD i = Try_EVR; i <= Try_VMR7; i++)
    {
        switch (i)
        {
        case Try_EVR:
            m_pVideo = new (std::nothrow) CEVR();
            break;

        case Try_VMR9:
            m_pVideo = new (std::nothrow) CVMR9();
            break;

        case Try_VMR7:
            m_pVideo = new (std::nothrow) CVMR7();
            break;
        }

        if (m_pVideo == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        hr = m_pVideo->AddToGraph(m_pGraph, m_hwnd);
        if (SUCCEEDED(hr))
        {
            break;
        }

        delete m_pVideo;
        m_pVideo = NULL;
    }
    return hr;
}


// Render the streams from a source filter. 

HRESULT DShowPlayer::RenderStreams(IBaseFilter* pSource)
{
    BOOL bRenderedAnyPin = FALSE;

    IFilterGraph2* pGraph2 = NULL;
    IEnumPins* pEnum = NULL;
    IBaseFilter* pAudioRenderer = NULL;
    HRESULT hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&pGraph2));
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the video renderer to the graph
    hr = CreateVideoRenderer();
    if (FAILED(hr))
    {
        goto done;
    }

    // Add the DSound Renderer to the graph.
    hr = AddFilterByCLSID(m_pGraph, CLSID_DSoundRender,
        &pAudioRenderer, L"Audio Renderer");
    if (FAILED(hr))
    {
        goto done;
    }

    // Enumerate the pins on the source filter.
    hr = pSource->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        goto done;
    }

    // Loop through all the pins
    IPin* pPin;
    while (S_OK == pEnum->Next(1, &pPin, NULL))
    {
        // Try to render this pin. 
        // It's OK if we fail some pins, if at least one pin renders.
        HRESULT hr2 = pGraph2->RenderEx(pPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL);

        pPin->Release();
        if (SUCCEEDED(hr2))
        {
            bRenderedAnyPin = TRUE;
        }
    }

    hr = m_pVideo->FinalizeGraph(m_pGraph);
    if (FAILED(hr))
    {
        goto done;
    }

    // Remove the audio renderer, if not used.
    BOOL bRemoved;
    hr = RemoveUnconnectedRenderer(m_pGraph, pAudioRenderer, &bRemoved);

done:
    SafeRelease(&pEnum);
    SafeRelease(&pAudioRenderer);
    SafeRelease(&pGraph2);

    // If we succeeded to this point, make sure we rendered at least one 
    // stream.
    if (SUCCEEDED(hr))
    {
        if (!bRenderedAnyPin)
        {
            hr = VFW_E_CANNOT_RENDER;
        }
    }
    return hr;
}