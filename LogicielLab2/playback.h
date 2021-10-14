// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#pragma once

#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <string>

using namespace std;

class CVideoRenderer;

enum PlaybackState
{
    STATE_NO_GRAPH,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
};

const UINT WM_GRAPH_EVENT = WM_APP + 1;

typedef void (CALLBACK* GraphEventFN)(HWND hwnd, long eventCode, LONG_PTR param1, LONG_PTR param2);

class DShowPlayer
{
public:
    DShowPlayer(string FileName);
    ~DShowPlayer();

    PlaybackState State() const { return m_state; }

    HRESULT Play();
    HRESULT Pause();
    HRESULT Stop();
    HRESULT SetRate(double dRate);
    HRESULT SetPositions(LONGLONG* pCurrent);


    BOOL    HasVideo() const;
    HRESULT UpdateVideoWindow(const LPRECT prc);
    HRESULT Repaint(HDC hdc);
    HRESULT DisplayModeChanged();

private:
    HRESULT InitializeGraph(string FileName);
    void    TearDownGraph();
    HRESULT CreateVideoRenderer();
    HRESULT RenderStreams(IBaseFilter* pSource);

    PlaybackState   m_state;

    HWND m_hwnd; // Video window. This window also receives graph events.

    IGraphBuilder* m_pGraph;
    IMediaControl* m_pControl;
    IMediaEventEx* m_pEvent;
    CVideoRenderer* m_pVideo;
    IMediaSeeking* pSeek;
};

#endif