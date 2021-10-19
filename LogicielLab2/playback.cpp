#include <new>
#include <windows.h>
#include <dshow.h>
#include <string>
#include "playback.h"

using namespace std;

//Constructeur
DShowPlayer::DShowPlayer(string FileName) :
    m_state(STATE_NO_GRAPH),
    m_pGraph(NULL),
    m_pControl(NULL),
    m_pEvent(NULL),
    pSeek(NULL)
{
    HRESULT hr = InitializeGraph(FileName);
}

//Destructeur
DShowPlayer::~DShowPlayer()
{
    m_pGraph->Release();
    m_pControl->Release();
    m_pEvent->Release();
    pSeek->Release();
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

HRESULT DShowPlayer::SetRate()
{
    double dRate;
    pSeek->GetRate(&dRate);

    //Bascule entre la vitesse normale et accélérée
    if (dRate == 1.0)
        dRate = 2.0;
    else 
        dRate = 1.0;

    HRESULT hr = pSeek->SetRate(dRate);
    if (SUCCEEDED(hr))
    {
        m_state = STATE_RUNNING;
    }
    return hr;
}

HRESULT DShowPlayer::SetPositions()
{
    LONGLONG* rtNow = 0;
    
    HRESULT hr = pSeek->SetPositions(rtNow,
        AM_SEEKING_RelativePositioning, NULL, AM_SEEKING_NoPositioning);

    if (SUCCEEDED(hr))
    {
        m_state = STATE_RUNNING;
    }
    return hr;
}

// Graph building

// Create a new filter graph. 
HRESULT DShowPlayer::InitializeGraph(string FileName)
{
    //Convertie string en wstring (wchar* LPWSTR)
    int size;
    const WCHAR* NewFileName;
    int ssize = (int)FileName.length() + 1;
    size = MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), ssize, NULL, 0);
    NewFileName = new WCHAR[size];
    MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), ssize, (LPWSTR)NewFileName, size);


    // Create the Filter Graph Manager.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        return hr;
    }
    hr = CoCreateInstance(CLSID_FilterGraph, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pGraph));
    if (FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager."); 
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pControl));
    if (FAILED(hr))
    {
        printf("ERROR1");
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&pSeek));
    if (FAILED(hr))
    {
        printf("ERROR2");
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pEvent));
    if (FAILED(hr))
    {
        printf("ERROR3");
        return hr;
    }

    hr = m_pGraph->RenderFile(NewFileName, NULL);
    if (FAILED(hr))
    {
        printf("ERROR4");
        return hr;
    }

    hr = m_pControl->Run();
    m_state = STATE_RUNNING;
    delete[] NewFileName;
}