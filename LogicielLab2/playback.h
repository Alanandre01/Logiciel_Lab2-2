#pragma once
#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <string>

using namespace std;

enum PlaybackState
{
    STATE_NO_GRAPH,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
};


class DShowPlayer
{
public:
    DShowPlayer(string FileName);
    ~DShowPlayer();

    PlaybackState State() const { return m_state; }

    HRESULT Play();
    HRESULT Pause();
    HRESULT Stop();
    HRESULT SetRate();
    HRESULT SetPositions(LONGLONG* pCurrent);

private:
    HRESULT InitializeGraph(string FileName);

    PlaybackState   m_state;

    IGraphBuilder* m_pGraph;
    IMediaControl* m_pControl;
    IMediaEventEx* m_pEvent;
    IMediaSeeking* pSeek;
};

#endif