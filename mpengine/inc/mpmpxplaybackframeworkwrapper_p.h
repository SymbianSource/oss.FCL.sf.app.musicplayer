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
* Description: Framework wraper for Playback - Private.
*
*/

#ifndef MPMPXPLAYBACKFRAMEWORKWRAPPER_P_H
#define MPMPXPLAYBACKFRAMEWORKWRAPPER_P_H

#include <e32base.h>
#include <mpxplaybackobserver.h>
#include "mpcommondefs.h"


class MMPXPlaybackUtility;
class MpMpxPlaybackFrameworkWrapper;
class QStringList;
class CMPXMedia;
class MpPlaybackData;
class XQSharableFile;


class MpMpxPlaybackFrameworkWrapperPrivate : public MMPXPlaybackObserver,
                                             public MMPXPlaybackCallback
{
public:

    explicit MpMpxPlaybackFrameworkWrapperPrivate( MpMpxPlaybackFrameworkWrapper *wrapper );
    virtual ~MpMpxPlaybackFrameworkWrapperPrivate();

    void init( TUid hostUid );

    void play( QString aFilename );
    void play( const XQSharableFile& file );
    void playPause();
    void stop();
    void skipForward();
    void startSeekForward();
    void stopSeeking();
    void skipBackward();
    void startSeekBackward();
    void setPosition( int value );
    void setShuffle( bool mode );
    void setRepeat( bool mode );
    void setBalance( int balance );
    void applyAudioEffects();
    void applyEqualizer();

    MpPlaybackData *playbackData();

    // From MMPXPlaybackObserver
    void HandlePlaybackMessage( CMPXMessage *aMessage, TInt aError );

    // From MMPXPlaybackCallback
    void HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );
    void HandleSubPlayerNamesL( TUid aPlayer, const MDesCArray *aSubPlayers,
                                TBool aComplete, TInt aError );
    void HandleMediaL( const CMPXMedia& aProperties, TInt aError );
    
private:

    void DoInitL();
    void DoPlayL( QString aFilename );
    void DoPlayL( const XQSharableFile& file );
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );
    void ForceStopL();
    void UpdateStateL();
    void RetrieveSongDetailsL();

private:

    MpMpxPlaybackFrameworkWrapper       *q_ptr;
    MMPXPlaybackUtility                 *iPlaybackUtility;     // Own
    CMPXMedia                           *iMedia;               // Own
    MpPlaybackData                      *iPlaybackData;        // Own
    TUid                                mHostUid;
    
};

#endif  // MPMPXPLAYBACKFRAMEWORKWRAPPER_P_H
