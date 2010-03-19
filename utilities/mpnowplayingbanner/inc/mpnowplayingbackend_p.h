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
* Description: Music Player now playing widget backend - Private.
*
*/

#ifndef MPNOWPLAYINGBACKEND_P_H
#define MPNOWPLAYINGBACKEND_P_H

#include <mpxplaybackobserver.h>
#include "mpnowplayingbackend.h"


//forward declartions
class MMPXPlaybackUtility;

//class declaration
class MpNowPlayingBackEndPrivate : public MMPXPlaybackObserver,
                                   public MMPXPlaybackCallback
{

public:
    explicit MpNowPlayingBackEndPrivate(long int playerId,
        MpNowPlayingBackEnd *qq);
    virtual ~MpNowPlayingBackEndPrivate();

    void update();
    void playPause();

public: //from MMPXPlaybackObserver
    void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

public: //from MMPXPlaybackCallback
    void HandlePropertyL( TMPXPlaybackProperty aProperty,
                          TInt aValue,
                          TInt aError);
    void HandleSubPlayerNamesL( TUid aPlayer,
                                const MDesCArray* aSubPlayers,
                                TBool aComplete,
                                TInt aError );
    void HandleMediaL( const CMPXMedia& aProperties, TInt aError );

private:
    void RequestMediaL();
    void UpdateStateL();
    void DoPlayPauseL();
    void DoHandlePlaybackMessageL( const CMPXMessage& message );

private:
    MpNowPlayingBackEnd     *q_ptr;             // not owned
    SimplifiedPlayerState   mPreviousState;
    MMPXPlaybackUtility     *mPlaybackUtility;  // owned
};

#endif  // MPNOWPLAYINGBACKEND_P_H
