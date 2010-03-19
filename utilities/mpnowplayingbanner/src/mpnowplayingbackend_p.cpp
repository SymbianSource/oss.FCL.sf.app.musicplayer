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

#include <mpxplaybackutility.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>

#include <QString>

#include "mpnowplayingbackend_p.h"
#include "mptrace.h"

/*!
    \internal
    \class MpNowPlayingBackEndPrivate
    \brief Wraps the playback utility and provides separation for QT and Symbian code.

    This class wraps the playback utility on the MPX Framework to provide a simple
    interface and separation of QT and Symbian code.
*/


/*!
 \internal
 Constructs the now playing back end private.
 */
MpNowPlayingBackEndPrivate::MpNowPlayingBackEndPrivate( long int playerId, MpNowPlayingBackEnd *qq )
	: q_ptr( qq ),
      mPreviousState( NotPlaying ),
      mPlaybackUtility(0)
{
    TX_ENTRY_ARGS( "Player ID =" << playerId << " Q pointer=" << ( void * )qq )
    if ( !playerId ) {
        playerId = KPbModeDefault.iUid;
    }
    TRAPD( errCode,
        mPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid( playerId ) );    
        mPlaybackUtility->AddObserverL( *this );
    );

    if( KErrNone != errCode && mPlaybackUtility ) {
        mPlaybackUtility->Close();
        mPlaybackUtility = 0;
    }
    TX_EXIT
}

/*!
 \internal
 Destructs the now playing back end private.
 */
MpNowPlayingBackEndPrivate::~MpNowPlayingBackEndPrivate()
{
    TX_ENTRY
    if ( mPlaybackUtility ) {
        TRAP_IGNORE( mPlaybackUtility->RemoveObserverL( *this ) );
        mPlaybackUtility->Close();
    }
    TX_EXIT
}

/*!
 \internal
 Updates the widget with the latest playback information.
 */
void MpNowPlayingBackEndPrivate::update()
{
    TX_ENTRY
    TRAPD( errCode, UpdateStateL() );
    if ( KErrNone == errCode ) {
        TRAP_IGNORE( RequestMediaL() );
    }
    TX_EXIT
}

/*!
 \internal
  Sends a play/pause command.
 */
void MpNowPlayingBackEndPrivate::playPause()
{
    TX_ENTRY
    TRAPD(err, DoPlayPauseL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 Callback function from MMPXPlaybackObserver.
 \sa DoHandlePlaybackMessageL()
 */
void MpNowPlayingBackEndPrivate::HandlePlaybackMessage(
    CMPXMessage* aMessage,
    TInt aError )
{
    TX_ENTRY
    if ( aError == KErrNone && aMessage ) {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
    }
    TX_EXIT
}

/*!
 \internal
 Callback function from MMPXPlaybackObserver.
 */
void MpNowPlayingBackEndPrivate::HandlePropertyL( TMPXPlaybackProperty aProperty,
    TInt aValue, TInt aError)
{
    // Nothing to do.
    Q_UNUSED(aProperty);
    Q_UNUSED(aValue);
    Q_UNUSED(aError);
}

/*!
 \internal
 Callback function from MMPXPlaybackObserver.
 */
void MpNowPlayingBackEndPrivate::HandleSubPlayerNamesL( TUid aPlayer,
    const MDesCArray* aSubPlayers, TBool aComplete, TInt aError )
{
    // Nothing to do.
    Q_UNUSED(aPlayer);
    Q_UNUSED(aSubPlayers);
    Q_UNUSED(aComplete);
    Q_UNUSED(aError);
}

/*!
 \internal
 Callback function from MMPXPlaybackCallback.
 */
void MpNowPlayingBackEndPrivate::HandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError )
{
    TX_ENTRY
    if( KErrNone != aError || mPreviousState == NotPlaying ) {
        TX_EXIT
        return;
    }

    if( aMedia.IsSupported( KMPXMediaGeneralTitle ) ) {
        emit q_ptr->titleChanged(
            QString::fromUtf16( aMedia.ValueText( KMPXMediaGeneralTitle ).Ptr(),
                                aMedia.ValueText( KMPXMediaGeneralTitle ).Length() ) );
    }
    else if (aMedia.IsSupported( KMPXMediaGeneralUri )) {
        TParsePtrC filePath( aMedia.ValueText( KMPXMediaGeneralUri ) );
        emit q_ptr->titleChanged(
            QString::fromUtf16( filePath.Name().Ptr(),
                                filePath.Name().Length() ) );
    }
    else {
        emit q_ptr->titleChanged( QString() );
    }

    if( aMedia.IsSupported( KMPXMediaMusicArtist ) ) {
        emit q_ptr->artistChanged(
            QString::fromUtf16( aMedia.ValueText( KMPXMediaMusicArtist ).Ptr(),
                                aMedia.ValueText( KMPXMediaMusicArtist ).Length() ) );
    }
    else {
        emit q_ptr->artistChanged( QString() );
    }
    TX_EXIT
}

/*!
 \internal
 Leaving function to request Media.
 \sa HandleMediaL()
 */
void MpNowPlayingBackEndPrivate::RequestMediaL()
{
    TX_ENTRY
    MMPXSource* mediaSrc = mPlaybackUtility->Source();
    User::LeaveIfNull( mediaSrc );
    RArray<TMPXAttribute> requestedAttr;
    CleanupClosePushL( requestedAttr );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralTitle ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicArtist ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralUri ) );
    mediaSrc->MediaL( requestedAttr.Array(), *this );
    CleanupStack::PopAndDestroy( &requestedAttr );
    TX_EXIT
}

/*!
 \internal
 Leaving function to update the playback state.
 */
void MpNowPlayingBackEndPrivate::UpdateStateL()
{
    TX_ENTRY
    if ( !mPlaybackUtility->Source() ) {
        TX_LOG_ARGS( "NotPlaying" )
        mPreviousState = NotPlaying;
        emit q_ptr->stateUpdate( NotPlaying );

    }
    else {
        TMPXPlaybackState state = mPlaybackUtility->StateL();
        switch ( state ) {
            case EPbStatePlaying:
                if ( mPreviousState != Playing ) {
                    TX_LOG_ARGS( "Playing" )
                    emit q_ptr->stateUpdate( Playing );
                }
                mPreviousState = Playing;
                break;
            case EPbStatePaused:
            case EPbStateStopped: //reinterpreting stop as a pause
                TX_LOG_ARGS("EPbStatePaused or PbStateStopped")
                if (mPreviousState != Paused){
                    TX_LOG_ARGS( "Paused" )
                    emit q_ptr->stateUpdate( Paused );
                }
                mPreviousState = Paused;
                break;
            default:
                //Ignore other states.
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 Leaving function to send play/pause command.
 */
void MpNowPlayingBackEndPrivate::DoPlayPauseL()
{
    mPlaybackUtility->CommandL( EPbCmdPlayPause );
}

/*!
 \internal
 Leaving function to handle the playback /a message.
 \sa HandlePlaybackMessageL()
 */
void MpNowPlayingBackEndPrivate::DoHandlePlaybackMessageL(
    const CMPXMessage& message )
{
    TX_ENTRY
    TMPXMessageId id( message.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        switch ( message.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) ) {
            case TMPXPlaybackMessage::EStateChanged:
                TX_LOG_ARGS( "EStateChanged" )
                UpdateStateL();
                break;
            case TMPXPlaybackMessage::EMediaChanged:
                TX_LOG_ARGS( "EMediaChanged or fall through from EPlaylistUpdated" )
                RequestMediaL();
                break;
            case TMPXPlaybackMessage::EPlaylistUpdated:
                TX_LOG_ARGS( "EPlaylistUpdated" )
            case TMPXPlaybackMessage::EActivePlayerChanged:
                TX_LOG_ARGS( "EActivePlayerChanged or fall through from EPlaylistUpdated" )
                UpdateStateL();
                RequestMediaL();
                break;
            default:
                TX_LOG_ARGS( "unhandled case:" << message.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) << "see TMPXPlaybackMessage" )
                break;
        }
    }
    TX_EXIT
}
