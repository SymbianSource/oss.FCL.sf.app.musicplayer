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

#include <mpxplaybackutility.h>
#include <mpxcollectionutility.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionplugin.hrh>
#include <mpxcollectioncommanddefs.h>

#include "mpmpxpbframeworkwrapper_p.h"
#include "mpmpxpbframeworkwrapper.h"
#include "mpplaybackdata.h"
#include "mpcommondefs.h"
#include "mptrace.h"

/*!
    \class MpMpxPbFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxPbFrameworkWrapperPrivate::MpMpxPbFrameworkWrapperPrivate( MpMpxPbFrameworkWrapper *qq)
    : q_ptr( qq ),
      iPlaybackUtility(0),
      iCollectionUtility(0),
      iMedia(0),
      iPlaybackData(0)
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxPbFrameworkWrapperPrivate::~MpMpxPbFrameworkWrapperPrivate()
{
    TX_ENTRY

    if ( iPlaybackUtility ) {
        TRAP_IGNORE( ForceStopL() );
        iPlaybackUtility->Close();
    }

    if ( iCollectionUtility ) {
        iCollectionUtility->Close();
    }

    delete iMedia;
    delete iPlaybackData;

    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::init()
{
    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::playPause()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdPlayPause));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::stop()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdStop));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::skipForward()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdNext));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::skipBackward()
{
    TX_ENTRY
    TRAPD(err, iPlaybackUtility->CommandL(EPbCmdPrevious));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::setPosition( int value )
{
    TX_ENTRY_ARGS("value =" << value);
    TRAPD(err, iPlaybackUtility->SetL(EPbPropertyPosition, value));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::setShuffle( bool mode )
{
    TX_ENTRY_ARGS("mode =" << mode);
    TRAPD(err, iPlaybackUtility->SetL(EPbPropertyRandomMode, mode));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::setRepeat( bool mode )
{
    TX_ENTRY_ARGS("mode =" << mode);
    TRAPD(err, iPlaybackUtility->SetL(EPbPropertyRepeatMode, mode ? EPbRepeatAll : EPbRepeatOff));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
MpPlaybackData *MpMpxPbFrameworkWrapperPrivate::playbackData()
{
    return iPlaybackData;
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandlePlaybackMessage( CMPXMessage *aMessage, TInt aError )
{
    TX_ENTRY_ARGS("aError=" << aError);
    if ( aError == KErrNone && aMessage ) {
        TRAP_IGNORE( DoHandlePlaybackMessageL(*aMessage) );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
{
    TX_ENTRY_ARGS("aProperty=" << aProperty << "aValue=" << aValue << "aError=" << aError)
    if ( KErrNone == aError ) {
        switch ( aProperty  ) {
            case EPbPropertyPosition:
                TX_LOG_ARGS("EPbPropertyPosition")
                iPlaybackData->setPosition(aValue);
                break;
            case EPbPropertyDuration:
                TX_LOG_ARGS("EPbPropertyDuration")
                iPlaybackData->setDuration(aValue);
                break;
            default:
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandleSubPlayerNamesL(
    TUid aPlayer,
    const MDesCArray *aSubPlayers,
    TBool aComplete,
    TInt aError )
{
    Q_UNUSED(aPlayer);
    Q_UNUSED(aSubPlayers);
    Q_UNUSED(aComplete);
    Q_UNUSED(aError);
    TX_LOG
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandleMediaL(
    const CMPXMedia& aProperties,
    TInt aError )
{
    TX_ENTRY_ARGS("aError=" << aError);
    if ( KErrNone != aError || !iPlaybackData ) {
        TX_EXIT
        return;
    }

    if ( iMedia ) {
        delete iMedia;
        iMedia = NULL;
    }
    iMedia = CMPXMedia::NewL( aProperties );

    bool changed = false;
    if ( aProperties.IsSupported( KMPXMediaGeneralTitle ) ) {
        changed |= iPlaybackData->setTitle(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaGeneralTitle ).Ptr(),
                aProperties.ValueText( KMPXMediaGeneralTitle ).Length() ) );
    }
    if ( aProperties.IsSupported( KMPXMediaMusicArtist ) ) {
        changed |= iPlaybackData->setArtist(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicArtist ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicArtist ).Length() ) );
    }
    if ( aProperties.IsSupported( KMPXMediaMusicAlbum ) ) {
        changed |= iPlaybackData->setAlbum(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicAlbum ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicAlbum ).Length() ) );
    }

    if ( changed ) {
        // This is required to propagate the playback info to UI at once.
        iPlaybackData->commitPlaybackInfo();
    }

    if ( aProperties.IsSupported( KMPXMediaGeneralUri) ) {
        iPlaybackData->setUri(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaGeneralUri ).Ptr(),
                aProperties.ValueText( KMPXMediaGeneralUri ).Length() ) );
    }
    if (aProperties.IsSupported(TMPXAttribute(KMPXMediaMusicAlbumArtFileName))) {
        iPlaybackData->setAlbumArtUri(
            QString::fromUtf16(
                aProperties.ValueText(KMPXMediaMusicAlbumArtFileName).Ptr(),
                aProperties.ValueText(KMPXMediaMusicAlbumArtFileName).Length()));
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandleCollectionMessage(
    CMPXMessage* aMessage, TInt aError )
{

    Q_UNUSED(aMessage);
    Q_UNUSED(aError);
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandleOpenL(
    const CMPXMedia& aEntries, TInt aIndex, TBool aComplete, TInt aError)
{
    Q_UNUSED(aEntries);
    Q_UNUSED(aIndex);
    Q_UNUSED(aComplete);
    Q_UNUSED(aError);
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandleOpenL(
    const CMPXCollectionPlaylist& aPlaylist, TInt aError )
{
    Q_UNUSED(aPlaylist);
    Q_UNUSED(aError);
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::HandleCollectionMediaL(
    const CMPXMedia& aMedia, TInt aError )
{
    Q_UNUSED(aMedia);
    Q_UNUSED(aError);
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid( MpCommon::KMusicPlayerUid ) );
    iPlaybackUtility->AddObserverL( *this );
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );

    iPlaybackData = new MpPlaybackData();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
{
    TX_ENTRY
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TX_LOG_ARGS("event=" << event << ", type=" << type << ", data=" << data);

        switch ( event ) {
            case TMPXPlaybackMessage::EStateChanged:
                TX_LOG_ARGS("TMPXPlaybackMessage::EStateChanged")
                UpdateStateL();
                break;
            case TMPXPlaybackMessage::EInitializeComplete:
                TX_LOG_ARGS("TMPXPlaybackMessage::EInitializeComplete")
                if ( data ) {
                    // Play the track
                    CMPXCommand *cmd( CMPXCommand::NewL() );
                    CleanupStack::PushL( cmd );
                    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId,
                            KMPXCommandIdPlaybackGeneral );
                    cmd->SetTObjectValueL<TInt>(
                            KMPXCommandPlaybackGeneralType,
                            EPbCmdPlay );
                    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync,
                            ETrue );
                    cmd->SetTObjectValueL<TBool>(
                            KMPXCommandPlaybackGeneralNoBuffer,
                            ETrue );
                    iPlaybackUtility->CommandL( *cmd );
                    CleanupStack::PopAndDestroy( cmd );
                }
                break;
            case TMPXPlaybackMessage::EPropertyChanged:
                TX_LOG_ARGS("TMPXPlaybackMessage::EPropertyChanged")
                TMPXPlaybackProperty property;
                property = static_cast<TMPXPlaybackProperty>( type );
                HandlePropertyL( property, data, KErrNone );
                break;
            case TMPXPlaybackMessage::EMediaChanged:
                TX_LOG_ARGS("TMPXPlaybackMessage::EMediaChanged")
                RetrieveSongDetailsL();
                break;
            default:
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::ForceStopL()
{
    TX_ENTRY
    if ( iPlaybackUtility ) {
        // Stop the track
        CMPXCommand *cmd( CMPXCommand::NewL() );
        CleanupStack::PushL( cmd );
        cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
        cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdStop );
        cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
        cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
        iPlaybackUtility->CommandL( *cmd );
        CleanupStack::PopAndDestroy( cmd );

        iPlaybackUtility->RemoveObserverL(*this);
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::UpdateStateL()
{
    TX_ENTRY
    if ( !iPlaybackUtility->Source() ) {
        TX_LOG_ARGS("There is no source")
        iPlaybackData->setPlaybackState( MpPlaybackData::Stopped );
    }
    else {
        switch ( iPlaybackUtility->StateL() ) {
            case EPbStatePlaying:
                TX_LOG_ARGS("EPbStatePlaying")
                iPlaybackData->setPlaybackState( MpPlaybackData::Playing );
                break;
            case EPbStatePaused:
                TX_LOG_ARGS("EPbStatePaused")
                iPlaybackData->setPlaybackState( MpPlaybackData::Paused );
                break;
            case EPbStateStopped:
                TX_LOG_ARGS("EPbStateStopped")
                iPlaybackData->setPlaybackState( MpPlaybackData::Stopped );
                break;
            default:
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxPbFrameworkWrapperPrivate::RetrieveSongDetailsL()
{
    TX_ENTRY
    MMPXSource *mediaSrc = iPlaybackUtility->Source();
    User::LeaveIfNull( mediaSrc );
    RArray<TMPXAttribute> requestedAttr;
    CleanupClosePushL( requestedAttr );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralTitle ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicArtist ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbum ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralUri ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) );

    mediaSrc->MediaL( requestedAttr.Array(), *this );
    CleanupStack::PopAndDestroy( &requestedAttr );
    TX_EXIT
}

