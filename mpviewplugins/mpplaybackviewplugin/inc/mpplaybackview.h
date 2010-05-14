/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Music Player Playback view.
*
*/

#ifndef MPPLAYBACKVIEW_H
#define MPPLAYBACKVIEW_H

#include "mpviewbase.h"

class HbAction;
class HbIcon;
class HbMainWindow;
class MpEngine;
class MpPlaybackData;
class MpPlaybackWidget;
class MpEqualizerWidget;
class QTranslator;


class MpPlaybackView : public MpViewBase
{
    Q_OBJECT

public:

    explicit MpPlaybackView();
    virtual ~MpPlaybackView();

    void initializeView();
    void activateView();
    void deactivateView();

signals:

    void command( int aCommand );

public slots:

    void startSettingsView();
    void back();
    void exit();

    void playbackStateChanged();
    
    void flip();
    
    void toggleShuffle();
    void shuffleChanged( bool shuffle );
    void toggleRepeat();
    void repeatChanged( bool repeat );

    void handleSongSelected();
    void showEqualizerDialog();

private:

    void setupMenu();
    void setupToolbar();

private:

    MpEngine                *mMpEngine;         // Own
    MpPlaybackData          *mPlaybackData;     // Not own
    MpPlaybackWidget        *mPlaybackWidget;   // Not own
    MpEqualizerWidget       *mEqualizerWidget;  // Own

    HbMainWindow            *mWindow;           // Not own
    HbAction                *mNavigationBack;   // Own
    bool                    mActivated;

    HbIcon                  *mPlayIcon;         // Own
    HbIcon                  *mPauseIcon;        // Own
    HbIcon                  *mShuffleOnIcon;    // Own
    HbIcon                  *mShuffleOffIcon;   // Own

    HbAction                *mPlayPauseAction;  // Not own
    HbAction                *mRepeatAction;     // Not own
    HbAction                *mShuffleAction;    // Not own
    
    QTranslator             *mMpTranslator;     // Own
    QTranslator             *mCommonTranslator; // Own

    bool                    mShuffle;
    bool                    mRepeat;

    Q_DISABLE_COPY(MpPlaybackView)
};

#endif  // MPPLAYBACKVIEW_H
