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

#ifndef MPMPXPBFRAMEWORKWRAPPER_P_H
#define MPMPXPBFRAMEWORKWRAPPER_P_H

#include <mpxplaybackobserver.h>
#include <mpxcollectionobserver.h>

class MMPXPlaybackUtility;
class MMPXCollectionUtility;
class MpMpxPbFrameworkWrapper;
class CMPXMedia;
class MpPlaybackData;


class MpMpxPbFrameworkWrapperPrivate : public MMPXPlaybackObserver,
                                       public MMPXPlaybackCallback,
                                       public MMPXCollectionObserver
{
public:

    explicit MpMpxPbFrameworkWrapperPrivate( MpMpxPbFrameworkWrapper *qq );
    virtual ~MpMpxPbFrameworkWrapperPrivate();

    void init();

    void playPause();
    void stop();
    void skipForward();
    void skipBackward();
    void setPosition( int value );
    void setShuffle( bool mode );
    void setRepeat( bool mode );

    MpPlaybackData *playbackData();

    // From MMPXPlaybackObserver
    void HandlePlaybackMessage( CMPXMessage *aMessage, TInt aError );

    // From MMPXPlaybackCallback
    void HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );
    void HandleSubPlayerNamesL( TUid aPlayer, const MDesCArray *aSubPlayers,
                                TBool aComplete, TInt aError );
    void HandleMediaL( const CMPXMedia& aProperties, TInt aError );

    // From MMPXCollectionObserver
    void HandleCollectionMessage( CMPXMessage* aMessage, TInt aError );
    void HandleOpenL( const CMPXMedia& aEntries, TInt aIndex, TBool aComplete, TInt aError);
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

private:

    void DoInitL();
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );
    void ForceStopL();
    void UpdateStateL();
    void RetrieveSongDetailsL();

private:

    MpMpxPbFrameworkWrapper         *q_ptr;

    MMPXPlaybackUtility             *iPlaybackUtility;     // Own
    MMPXCollectionUtility           *iCollectionUtility;   // Own
    CMPXMedia                       *iMedia;               // Own

    MpPlaybackData                  *iPlaybackData;        // Own

};

#endif  // MPMPXPBFRAMEWORKWRAPPER_P_H
