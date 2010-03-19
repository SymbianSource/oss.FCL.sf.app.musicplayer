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
* Description: mpxplaybackutility stub for testing MpNowPlayingBackEnd
*
*/

#include <Qt>
#include <mpxplaybackobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>

#include "stub/inc/mpxplaybackutility.h"

bool gLeaveMediaL = false;
int gMediaCounter = 0;

bool gLeaveUtilityL = false;
bool gLeaveAddObserverL = false;
bool gLeaveRemoveObserverL = false;
bool gLeaveCommandL = false;
bool gLeaveStateL = false;
int gUtilCloseCounter = 0;
int gRemoveObserverCounter = 0;
bool gSourceExists = true;
TMPXPlaybackState gPlaybackState = EPbStateStopped;

// Constants
_LIT( KTitle, "Title" );
_LIT( KUri, "Uri" );
_LIT( KArtist, "Artist" );

/*!
 Stub constructor.
 */
MMPXSource::MMPXSource()
{
}

/*!
 Stub destructor.
 */
MMPXSource::~MMPXSource()
{
}

/*!
 Sets gLeaveMediaL.
 If true MMPXSource::MediaL leaves.
 */
void MMPXSource::setMediaLeave(bool leave)
{
    gLeaveMediaL = leave;
}

/*!
 Returns gMediaCounter.
 gMediaCounter counts the number of MMPXSource::MediaL calls.
 */
int MMPXSource::getMediaCounter()
{
    return gMediaCounter;
}

/*!
 Resets gMediaCounter to zero.
 */
void MMPXSource::resetMediaCounter()
{
    gMediaCounter = 0;
}

/*!
 Causes callback to observer.
 \sa MMPXSource::MediaL
 */
void MMPXSource::sendHandleMediaL(bool title, bool uri, bool artist)
{
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    if ( title ) {
        media->SetTextValueL(KMPXMediaGeneralTitle, KTitle);
    }
    if ( uri ) {
        media->SetTextValueL(KMPXMediaGeneralUri, KUri);
    }
    if ( artist ) {
        media->SetTextValueL(KMPXMediaMusicArtist, KArtist);
    }
    iObserver->HandleMediaL(*media, KErrNone);
    CleanupStack::PopAndDestroy( media );
}

/*!
 Sets the observer
 \sa MMPXSource::sendHandleMediaL.
 */
void MMPXSource::setObserver(MMPXPlaybackCallback* obs)
{
    iObserver = obs;
}

/*!
 Stub function.
 */
void MMPXSource::MediaL(const TArray<TMPXAttribute>& aAttrs, MMPXPlaybackCallback& aCallback)
{
    Q_UNUSED(aAttrs);
    if (gLeaveMediaL) {
        User::Leave(KErrGeneral);
    }
    iObserver = &aCallback;
    gMediaCounter++;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


/*!
 Stub constructor.
 */
MMPXPlaybackUtility::MMPXPlaybackUtility()
{
}

/*!
 Stub destructor.
 */
MMPXPlaybackUtility::~MMPXPlaybackUtility()
{
}

/*!
 Sets gLeaveUtilityL.
 If true MMPXPlaybackUtility::UtilityL leaves.
 */
void MMPXPlaybackUtility::setUtilityLeave(bool leave)
{
    gLeaveUtilityL = leave;
}

/*!
 Sets gLeaveAddObserverL.
 If true MMPXPlaybackUtility::AddObserverL leaves.
 */
void MMPXPlaybackUtility::setAddObserverLeave(bool leave)
{
    gLeaveAddObserverL = leave;
}

/*!
 Sets gLeaveRemoveObserverL.
 If true MMPXPlaybackUtility::RemoveObserverL leaves.
 */
void MMPXPlaybackUtility::setRemoveObserverLeave(bool leave)
{
    gLeaveRemoveObserverL = leave;
}

/*!
 Sets gLeaveCommandL.
 If true MMPXPlaybackUtility::CommandL leaves.
 */
void MMPXPlaybackUtility::setCommandLeave(bool leave)
{
    gLeaveCommandL = leave;
}

/*!
 Sets gLeaveStateL.
 If true MMPXPlaybackUtility::StateL leaves.
 */
void MMPXPlaybackUtility::setStateLeave(bool leave)
{
    gLeaveStateL = leave;
}

/*!
 Returns gUtilCloseCounter.
 gUtilCloseCounter counts the number of MMPXPlaybackUtility::Close calls.
 */
int MMPXPlaybackUtility::getCloseCounter()
{
    return gUtilCloseCounter;
}

/*!
 Resets gUtilCloseCounter to zero.
 */
void MMPXPlaybackUtility::resetCloseCounter()
{
    gUtilCloseCounter = 0;
}

/*!
 Returns gRemoveObserverCounter.
 gRemoveObserverCounter counts the number of MMPXPlaybackUtility::RemoveObserverL calls.
 */
int MMPXPlaybackUtility::getRemoveObserverCounter()
{
    return gRemoveObserverCounter;
}

/*!
 Resets gRemoveObserverCounter to zero.
 */
void MMPXPlaybackUtility::resetRemoveObserverCounter()
{
    gRemoveObserverCounter = 0;
}

/*!
 Sets gSourceExists.
 If true MMPXPlaybackUtility::Source returns valid pointer.
 */
void MMPXPlaybackUtility::setSource(bool exist)
{
    gSourceExists = exist;
}

/*!
 Sets gPlaybackState.
 \sa MMPXPlaybackUtility::getState()
 \sa MMPXPlaybackUtility::StateL()
 */
void MMPXPlaybackUtility::setState(TMPXPlaybackState state)
{
    gPlaybackState = state;
}

/*!
 Gets gPlaybackState.
 \sa MMPXPlaybackUtility::setState()
 */
TMPXPlaybackState MMPXPlaybackUtility::getState()
{
    return gPlaybackState;
}

/*!
 Causes callback to observer.
 */
void MMPXPlaybackUtility::sendHandlePlaybackMessage(TMPXPlaybackMessage::TEvent event)
{
    CMPXMessage* msg = CMPXMessage::NewL();
    CleanupStack::PushL(msg);
    TMPXMessageId id=static_cast<TMPXMessageId>(KMPXMessageGeneral);
    msg->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, id);
    msg->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent, event);
    iObserver->HandlePlaybackMessage(msg, KErrNone);
    CleanupStack::PopAndDestroy( msg );
}

/*!
 Stub function.
 */
MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL(const TUid& aModeId)
{
    Q_UNUSED(aModeId);
    if (gLeaveUtilityL) {
        User::Leave(KErrGeneral);
    }
    static MMPXPlaybackUtility playbackUtilility;
    return &playbackUtilility;
}

/*!
 Stub function.
 */
void MMPXPlaybackUtility::AddObserverL(MMPXPlaybackObserver& aObs)
{
    if (gLeaveAddObserverL) {
        User::Leave(KErrGeneral);
    }
    iObserver = &aObs;
}

/*!
 Stub function.
 */
void MMPXPlaybackUtility::RemoveObserverL(MMPXPlaybackObserver& aObs)
{
    Q_UNUSED(aObs);
    if (gLeaveRemoveObserverL) {
        User::Leave(KErrGeneral);
    }
    gRemoveObserverCounter++;
}

/*!
 Stub function.
 */
void MMPXPlaybackUtility::Close()
{
    gUtilCloseCounter++;
}

/*!
 Stub function.
 */
void MMPXPlaybackUtility::CommandL(TMPXPlaybackCommand aCmd, TInt aData)
{
    Q_UNUSED(aCmd);
    Q_UNUSED(aData);
    if (gLeaveCommandL) {
        User::Leave(KErrGeneral);
    }
    if ( gPlaybackState == EPbStatePlaying ) {
        gPlaybackState = EPbStatePaused;
    }
    else {
        gPlaybackState = EPbStatePlaying;
    }
}

/*!
 Stub function.
 */
TMPXPlaybackState MMPXPlaybackUtility::StateL() const
{
    if (gLeaveStateL) {
        User::Leave(KErrGeneral);
    }
    return gPlaybackState;
}

/*!
 Stub function.
 */
MMPXSource* MMPXPlaybackUtility::Source()
{
    if ( gSourceExists ) {
        return &iSource;
    }
    else {
        return 0;
    }
}

