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
* Description: Framework wraper - Private.
*
*/

#include <mpxplaybackutility.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>

#include "mpmpxdetailsframeworkwrapper_p.h"
#include "mpmpxdetailsframeworkwrapper.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpsongdata.h"

/*!
    \class MpMpxDetailsFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxDetailsFrameworkWrapperPrivate::MpMpxDetailsFrameworkWrapperPrivate( MpMpxDetailsFrameworkWrapper *qq )
    : q_ptr( qq ),
      iSongData(0),
      iPlaybackUtility(0)
{
    TX_ENTRY
	// TODO: Who will call the destructor if it throws ?
    QT_TRAP_THROWING( constructProviderL() );
    TX_EXIT
}

/*!
 \internal
 */
MpMpxDetailsFrameworkWrapperPrivate::~MpMpxDetailsFrameworkWrapperPrivate()
{
    TX_ENTRY
    // TODO: Hkn: Trace the leave on debug, remove any TRAP_IGNORE s
    TRAP_IGNORE( destructProviderL() );
    TX_EXIT
}

/*!
 \internal
 \ TODO: Hkn: rename
 */
void MpMpxDetailsFrameworkWrapperPrivate::constructProviderL()
{
    TX_ENTRY
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL(
            TUid::Uid( MpCommon::KMusicPlayerUid ) );
    iPlaybackUtility->AddObserverL( *this );
    
    iSongData = new MpSongData();
    User::LeaveIfNull(iSongData);
    
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::destructProviderL()
{
    TX_ENTRY
    if ( iPlaybackUtility )
        {        
        iPlaybackUtility->RemoveObserverL( *this );
        iPlaybackUtility->Close();
        iPlaybackUtility = NULL;
        //do not delete pointer data
        }
    
    delete iSongData;
    iSongData=NULL;
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandlePlaybackMessage(
        CMPXMessage *aMessage,
        TInt aError )
{
    Q_UNUSED( aMessage );
    Q_UNUSED( aError );
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandlePropertyL( TMPXPlaybackProperty /*aProperty*/, TInt /*aValue*/, TInt /*aError*/ )
{
    TX_LOG
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandleSubPlayerNamesL( TUid /*aPlayer*/, const MDesCArray */*aSubPlayers*/, TBool /*aComplete*/, TInt /*aError*/ )
{
    TX_LOG    
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandleMediaL(
        const CMPXMedia& aProperties,
        TInt aError )
{
    TX_ENTRY
  
    if ( KErrNone != aError ){
        // TODO: Hkn: Add a proper trace macro and trace
        TX_EXIT
        return;
    }
  
    bool changed = false;
    if ( aProperties.IsSupported( KMPXMediaGeneralTitle ) ) {
        changed |= iSongData->setTitle(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaGeneralTitle ).Ptr(),
                aProperties.ValueText( KMPXMediaGeneralTitle ).Length() ) );
    } else {
        changed |= iSongData->setTitle( QString() );
    }
    
    if ( aProperties.IsSupported( KMPXMediaMusicArtist ) ) {
        changed |= iSongData->setArtist(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicArtist ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicArtist ).Length() ) );
    } else {
        changed |= iSongData->setArtist( QString() );
    }
    
    if ( aProperties.IsSupported( KMPXMediaMusicAlbum ) ) {
        changed |= iSongData->setAlbum(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicAlbum ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicAlbum ).Length() ) );
    } else {
        changed |= iSongData->setAlbum( QString() );
    }
    if ( changed ) {
        iSongData->commitPlaybackInfo( );
    }    
    
    // call back will be called when Album art is retrieved
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) ) ) {
        iSongData->setAlbumArtUri(
            QString::fromUtf16(
                aProperties.ValueText(KMPXMediaMusicAlbumArtFileName).Ptr(),
                aProperties.ValueText(KMPXMediaMusicAlbumArtFileName).Length()));
    } else {
        iSongData->setAlbumArtUri( QString() );
    }
    
    // all following will be for song details
    changed = false;
    if ( aProperties.IsSupported( KMPXMediaGeneralComment ) ) {
        TX_LOG_ARGS( "Comment is supported " );
        changed |= iSongData->setComment(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaGeneralComment ).Ptr(),
                aProperties.ValueText( KMPXMediaGeneralComment ).Length() ) );
    } else {
        changed |= iSongData->setComment( QString() );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicComposer ) ) ) {
        TX_LOG_ARGS( "Composer is supported " );
        changed |= iSongData->setComposer(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicComposer ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicComposer ).Length()));
    } else {
        changed |= iSongData->setComposer( QString() );
    }

    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicYear ) ) ) {
        TInt64 yearInMicroSeconds = aProperties.ValueTObjectL<TInt64>( KMPXMediaMusicYear );
        TTime yearTime( yearInMicroSeconds );
        changed |= iSongData->setYear( yearTime.DateTime().Year() );
    } else {
        // to clear previous result
        changed |= iSongData->setYear( -1 );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicAlbumTrack ) ) ) {
        changed |= iSongData->setAlbumTrack(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicAlbumTrack ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicAlbumTrack ).Length()));
    } else {
        changed |= iSongData->setAlbumTrack( QString() );  
    }

    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicGenre ) ) ) {
        changed |= iSongData->setGenre(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicGenre ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicGenre ).Length()));
    } else {
        changed |= iSongData->setGenre( QString() );
    }
        
    if ( changed ) {
        iSongData->commitSongDetailInfo( );
    }    
    TX_EXIT
}

/*!
 return song data
 */
MpSongData *MpMpxDetailsFrameworkWrapperPrivate::songData()
{
    TX_LOG
    return iSongData;
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::retrieveSong()
{
    TX_ENTRY
    TRAP_IGNORE( doRetrieveSongL() );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::doRetrieveSongL()
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
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralComment ) );

    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumTrack ) );   
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicComposer ) );   
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicYear ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicGenre ) );   

    mediaSrc->MediaL( requestedAttr.Array(), *this );
    CleanupStack::PopAndDestroy( &requestedAttr );
    TX_EXIT
}
