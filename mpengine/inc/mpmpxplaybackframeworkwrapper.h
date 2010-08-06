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
* Description: Framework wraper for Playback utility.
*
*/

#ifndef MPMPXPLAYBACKFRAMEWORKWRAPPER_H
#define MPMPXPLAYBACKFRAMEWORKWRAPPER_H

#include <QObject>
#include "mpmpxcommondefs.h"
#include "mpcommondefs.h"
#include <xqsharablefile.h>

class MpMpxPlaybackFrameworkWrapperPrivate;
class MpPlaybackData;
class MpSongData;
class QStringList;


class MpMpxPlaybackFrameworkWrapper : public QObject
{
    Q_OBJECT

    friend class MpMpxPlaybackFrameworkWrapperPrivate;

public:

    explicit MpMpxPlaybackFrameworkWrapper( TUid hostUid=TUid::Uid(MpCommon::KMusicPlayerUid),
        MpSongData *songData=0, QObject *parent=0 );
    virtual ~MpMpxPlaybackFrameworkWrapper();
    
    MpPlaybackData *playbackData();
    void setBalance( int balance );
    void applyAudioEffects();
    void applyEqualizer();

    void retrieveSongDetails();

signals:

    void volumePropertyChanged( MpCommon::MpVolumeProperty property, int value );

public slots:
    
    void play( QString aFilename );
    void play( const XQSharableFile& file );
    void play();
    void pause();
    void playPause();
    void stop();
    void skipForward();
    void startSeekForward();
    void stopSeeking();
    void skipBackward();
    void startSeekBackward();
    void setPosition( int position );
    void setShuffle( bool mode );
    void setRepeat( bool mode );
    void getMaxVolume();
    void getVolume();
    void increaseVolume();
    void decreaseVolume();
    void setVolume( int value );
    void getMuteState();
    void mute();
    void unmute();
    void closeCurrentPlayback();

private:

    Q_DISABLE_COPY(MpMpxPlaybackFrameworkWrapper)
    MpMpxPlaybackFrameworkWrapperPrivate *d_ptr;

};

#endif  // MPMPXPLAYBACKFRAMEWORKWRAPPER_H
