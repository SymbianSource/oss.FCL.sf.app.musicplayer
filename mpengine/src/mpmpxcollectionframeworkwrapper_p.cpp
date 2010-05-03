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
* Description: Wrapper for mpx collection framework utilities - private implementation.
*
*/

#include <QStringList>

#include <s32strm.h>
#include <s32mem.h>
#include <apgcli.h>
#include <apacmdln.h>

#include <mpxplaybackutility.h>
#include <mpxcollectionutility.h>
#include <mpxharvesterutility.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionplaylist.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxmessagegeneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxparameter.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionopenutility.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <mpxlog.h>

#include "mpmpxcollectionframeworkwrapper_p.h"
#include "mpmpxcollectionframeworkwrapper.h"
#include "mpmpxcollectiondata.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpsettingsmanager.h"

const TInt KIncrementalDelayNone = 0;
const TInt KIncrementalDelayHalfSecond = 1000000;
const TInt KIncrementalFetchBlockSize = 40;

const TInt KMPXChunkSize = 100;  // number of songs added in each chunk, IncAddL
_LIT( KPlaylistPath, "C:\\Data\\Playlists\\" ); // Todo

/*!
    \class MpMpxCollectionFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxCollectionFrameworkWrapperPrivate::MpMpxCollectionFrameworkWrapperPrivate( MpMpxCollectionFrameworkWrapper *wrapper )
    : q_ptr( wrapper ),
      iCollectionUtility( 0 ),
      iCollectionUiHelper( 0 ),
      iIncrementalOpenUtil( 0 ),
      iIsolatedCollectionHelper( 0 ),
      iPlaybackUtility( 0 ),
      iCollectionData( 0 ),
      iIsolatedCollectionData( 0 ),
      iFirstIncrementalOpen( EFalse ),
      iUserPlaylists( 0 )
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxCollectionFrameworkWrapperPrivate::~MpMpxCollectionFrameworkWrapperPrivate()
{
    TX_ENTRY
    delete iCollectionData;
    delete iIsolatedCollectionData;

    if ( iCollectionUtility ) {
        iCollectionUtility->Collection().CancelRequest();
        iCollectionUtility->Close();
    }

    if ( iCollectionUiHelper ) {
        iCollectionUiHelper->Close();
    }

    if ( iPlaybackUtility ) {
        iPlaybackUtility->Close();
    }

    delete iIncrementalOpenUtil;
    delete iIsolatedCollectionHelper;
    delete iUserPlaylists;

    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::init( MpCommon::MpViewMode viewMode, TUid hostUid )
{

    iViewMode = viewMode;
    mHostUid = hostUid;
    TRAPD( err, DoInitL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::openCollection( TCollectionContext context )
{
    TRAPD( err, DoOpenCollectionL( context ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::openCollectionItem( int index )
{
    TRAPD( err, DoOpenCollectionItemL( index ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::reopenCollection()
{
    TRAPD( err, DoReopenCollectionL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::back()
{
    TRAPD( err, DoBackL() );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}
/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::findPlaylists( QStringList &playlists )
{
    TRAPD( err, DoFindPlaylistsL( playlists ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    TRAPD( err, DoCreatePlaylistL( playlistName, selection, collectionData ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    TRAPD( err, DoSaveToPlaylistL( playlistIndex, selection ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    TRAPD( err, DoSaveToCurrentPlaylistL( selection, collectionData ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
} 

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::deleteSongs( QList<int> &selection )
{
    TRAPD( err, DoDeleteSongsL( selection ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::renamePlaylist( QString &newName, int index )
{      
    TRAPD( err, DoRenamePlaylistL( newName, index ) );
    if ( err != KErrNone ) {
    TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::renamePlaylist( QString &newName )
{      
    TRAPD( err, DoRenamePlaylistL( newName ) );
    if ( err != KErrNone ) {
    TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}
/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::setShuffle( bool active )
{
    TRAPD( err, DoSetShuffleL( active ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::previewItem( int index )
{
    TRAPD( err, DoPreviewItemL( index ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
 void MpMpxCollectionFrameworkWrapperPrivate::openIsolatedCollection( TCollectionContext context )
{
    TRAPD( err, DoOpenIsolatedCollectionL( context ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::releaseIsolatedCollection()
{
    delete iIsolatedCollectionHelper;
    iIsolatedCollectionHelper = 0;
    delete iIsolatedCollectionData;
    iIsolatedCollectionData = 0;
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    TRAPD( err, DoReorderPlaylistL( playlistId, songId, originalOrdinal, newOrdinal ) );
    if ( err != KErrNone ) {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
    }
}

/*!
 \internal
 */
MpMpxCollectionData *MpMpxCollectionFrameworkWrapperPrivate::collectionData()
{
    return iCollectionData;
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleOpenL( 
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool aComplete,
    TInt aError )
{
    Q_UNUSED( aIndex );
    TX_UNUSED( aComplete );
    TX_ENTRY_ARGS( "aError=" << aError << "aComplete=" << aComplete );
    if ( aError == KErrNone ) {
        if ( iFirstIncrementalOpen ) {
            iCollectionData->setMpxMedia( aEntries );
            iFirstIncrementalOpen = EFalse;
        }
    }
    else {
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleOpenL( 
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
{
    TX_ENTRY_ARGS( "aError=" << aError );

    if ( aError == KErrNone ) {
        if ( !iPlaybackUtility ) {

            iPlaybackUtility = MMPXPlaybackUtility::UtilityL( mHostUid );

            if ( iViewMode == MpCommon::DefaultView ) {
                iPlaybackUtility->SetL( EPbPropertyRandomMode, MpSettingsManager::shuffle() ? ETrue : EFalse );
                iPlaybackUtility->SetL( EPbPropertyRepeatMode, MpSettingsManager::repeat() ? EPbRepeatAll : EPbRepeatOff );
            }
        }
        iPlaybackUtility->InitL( aPlaylist, ETrue );
        emit q_ptr->collectionPlaylistOpened();
    }
    else {
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleCollectionMessage( 
    CMPXMessage* aMsg,
    TInt aErr )
{
    TX_ENTRY_ARGS( "aErr=" << aErr );
    if ( aErr == KErrNone && aMsg )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMsg ) );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleCollectionMediaL( 
    const CMPXMedia& aMedia,
    TInt aError )
{
    Q_UNUSED( aMedia );
    Q_UNUSED( aError );
}

/*!
 \internal
 Handles the completion of any collection helper event.
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleOperationCompleteL( 
    TCHelperOperation aOperation,
    TInt aErr,
    void* aArgument )
{
    TX_ENTRY_ARGS( "aErr=" << aErr );
    switch( aOperation ) {
    case EDeleteOp:
        emit q_ptr->songsDeleted( KErrNone == aErr );
        break;
    case EAddOp:
        emit q_ptr->playlistSaved( KErrNone == aErr );
        break;
    case ERenameOp:
        emit q_ptr->playlistsRenamed( KErrNone == aErr );
        break;
    default:
        break;
    }

    if ( aArgument ) {
        delete ( CBase* )aArgument;
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::HandleIsolatedOpenL( const CMPXMedia& aEntries, TInt aError )
{
    TX_ENTRY_ARGS( "aError=" << aError );
    if ( aError == KErrNone ) {
        if ( iIsolatedCollectionData ) {
            delete iIsolatedCollectionData;
            iIsolatedCollectionData = 0;
        }  
        iIsolatedCollectionData = new MpMpxCollectionData();
        iIsolatedCollectionData->setMpxMedia( aEntries );
        emit q_ptr->isolatedCollectionOpened( iIsolatedCollectionData );
    }
    else {
        TX_LOG_ARGS( "Error: " << aError << "; should never get here." );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY
    if ( iViewMode == MpCommon::FetchView ) {
        iCollectionUtility = MMPXCollectionUtility::NewL( this, mHostUid );
        iCollectionUiHelper = CMPXCollectionHelperFactory:: NewCollectionUiHelperL( mHostUid );
        iIncrementalOpenUtil = CMPXCollectionOpenUtility::NewL( this, mHostUid );
    }
    else {
        iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
        iCollectionUiHelper = CMPXCollectionHelperFactory:: NewCollectionUiHelperL();
        iIncrementalOpenUtil = CMPXCollectionOpenUtility::NewL( this );
    }
    
    iCollectionData = new MpMpxCollectionData();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoOpenCollectionL( 
    TCollectionContext aContext )
{
    TX_ENTRY_ARGS( "aContext=" << aContext );

    switch ( aContext ) {
    case ECollectionContextAllSongs:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicAllSongsPathL();
        CleanupStack::PushL( cpath );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextArtists:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
        CleanupStack::PushL( cpath );
        cpath->AppendL( 2 );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextAlbums:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
        CleanupStack::PushL( cpath );
        cpath->AppendL( 3 );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextPlaylists:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicPlaylistPathL();
        CleanupStack::PushL( cpath );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextGenres:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
        CleanupStack::PushL( cpath );
        // Temp
        cpath->AppendL( 5 );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    default:
        TX_LOG_ARGS( "Error: Unexpected context; should never get here." );
        break;
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoOpenCollectionItemL( TInt aIndex )
    {
    TX_ENTRY_ARGS( "aIndex=" << aIndex );
    iCollectionUtility->Collection().OpenL( aIndex );
    TX_EXIT
    }

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoIncrementalOpenL()
{
    TX_ENTRY
    // Cancel any reads
    iIncrementalOpenUtil->Stop();

    // Start the read
    iFirstIncrementalOpen = ETrue;
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    TArray<TMPXAttribute> ary = attrs.Array();
    iIncrementalOpenUtil->SetDelay( KIncrementalDelayNone );
    iIncrementalOpenUtil->StartL( ary, KIncrementalFetchBlockSize,
                                  KErrNotFound, CMPXCollectionOpenUtility::EFetchNormal );
    iIncrementalOpenUtil->SetDelay( KIncrementalDelayHalfSecond );
    CleanupStack::PopAndDestroy( &attrs );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoReopenCollectionL()
{
    TX_ENTRY
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    cpath->Back();
    iCollectionUtility->Collection().OpenL( *cpath );
    CleanupStack::PopAndDestroy( cpath );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoBackL()
{
    TX_ENTRY
    iCollectionUtility->Collection().BackL();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoFindPlaylistsL( QStringList &playlists )
{
    TX_ENTRY
    delete iUserPlaylists;
    iUserPlaylists = NULL;

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( KMPXMediaGeneralId );
    attrs.Append( KMPXMediaGeneralTitle );

    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TMPXGeneralType>( 
        KMPXMediaGeneralType, EMPXGroup );
    criteria->SetTObjectValueL<TMPXGeneralCategory>( 
        KMPXMediaGeneralCategory, EMPXPlaylist );

    // Look up collection UID and set to criteria
    RArray<TUid> ary;
    CleanupClosePushL( ary );
    ary.AppendL( TUid::Uid( EMPXCollectionPluginMusic ) );
    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    criteria->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );

    iUserPlaylists = iCollectionUtility->Collection().FindAllL( *criteria, attrs.Array() );
    CleanupStack::PopAndDestroy( criteria );
    CleanupStack::PopAndDestroy( &attrs );

    if ( iUserPlaylists ) {
        const CMPXMediaArray* mediaArray =
            iUserPlaylists->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ) );
        TInt count = mediaArray->Count();
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* media( mediaArray->AtL( i ) );
            const TDesC& titleText = media->ValueText( KMPXMediaGeneralTitle );
            if ( titleText.Compare( KNullDesC ) != 0 ) {
            playlists += QString::fromUtf16( titleText.Ptr(), titleText.Length() );
            }
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoCreatePlaylistL( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    TX_ENTRY_ARGS( "playlistName=" << playlistName );
    CMPXMedia* tracks = CMPXMedia::NewL();
    CleanupStack::PushL( tracks );

    TPtrC ptr( reinterpret_cast<const TText*>( playlistName.constData() ) );
    tracks->SetTextValueL( KMPXMediaGeneralTitle, ptr );
    tracks->SetTextValueL( KMPXMediaGeneralUri, KPlaylistPath );
    //if collection data is not provided we use the browsing collection.
    PreparePlaylistMediaL( *tracks, selection, collectionData ? collectionData : iCollectionData );

    iCollectionUiHelper->IncAddL( *tracks, this, KMPXChunkSize );
    CleanupStack::PopAndDestroy( tracks );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSaveToPlaylistL( TMPXItemId playlistId, QList<int> &selection, MpMpxCollectionData *collectionData )
{
    TX_ENTRY_ARGS( "playlistId=" << int( playlistId ) );
    
    CMPXMedia* tracks = CMPXMedia::NewL();
    CleanupStack::PushL( tracks );

    RArray<TUid> ary;
    CleanupClosePushL( ary );
    ary.AppendL( TUid::Uid( EMPXCollectionPluginMusic ) );
    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    tracks->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, playlistId );
    tracks->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );
    PreparePlaylistMediaL( *tracks, selection, collectionData );

    iCollectionUiHelper->IncAddL( *tracks, this, KMPXChunkSize );
    CleanupStack::PopAndDestroy( tracks );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSaveToPlaylistL( int playlistIndex, QList<int> &selection )
{
    TX_ENTRY_ARGS( "playlistIndex=" << playlistIndex );

    const CMPXMediaArray* mediaArray =  iUserPlaylists->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ) );
    CMPXMedia* media( mediaArray->AtL( playlistIndex ) );
    TMPXItemId playlistId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
    DoSaveToPlaylistL( playlistId, selection, iCollectionData );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSaveToCurrentPlaylistL( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    TX_ENTRY

    const CMPXMedia& container = iCollectionData->containerMedia();
    if ( container.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType) != EMPXItem && 
        container.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory) != EMPXPlaylist) {
        User::Leave( KErrArgument );
    }
    TMPXItemId playlistId( container.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoSaveToPlaylistL( playlistId, selection, collectionData );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoDeleteSongsL( QList<int> &selection )
{
    int count = selection.count();
    TX_ENTRY_ARGS( "selection count=" << count );

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );

    for ( TInt i = 0; i < count; i++ ) {
        path->SelectL( selection.at( i ) );
    }
    iCollectionUiHelper->DeleteL( *path, this );
    CleanupStack::PopAndDestroy( path );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRenamePlaylistL( QString &newName, int index )
{
    TX_ENTRY
    CMPXMediaArray *mediaArray;  
    const CMPXMedia& container = iCollectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>( container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    CMPXMedia* currentPlaylistMedia( mediaArray->AtL( index ) );
    TMPXItemId id( currentPlaylistMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoRenamePlaylistL( id, newName );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRenamePlaylistL( QString &newName )
{
    TX_ENTRY
    const CMPXMedia& container = iCollectionData->containerMedia();
    TMPXItemId id( container.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoRenamePlaylistL( id, newName );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoRenamePlaylistL( TMPXItemId id, QString &newName )
 {
     CMPXMedia* media = CMPXMedia::NewL();
     CleanupStack::PushL( media );
     media->SetTObjectValueL<TMPXGeneralType>( 
     KMPXMediaGeneralType, EMPXItem );
     media->SetTObjectValueL<TMPXGeneralCategory>( 
     KMPXMediaGeneralCategory, EMPXPlaylist );
     media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
     TPtrC ptr( reinterpret_cast<const TText*>( newName.constData() ) );
     media->SetTextValueL( KMPXMediaGeneralTitle, ptr );
     iCollectionUiHelper->RenameL( *media, this );
     CleanupStack::PopAndDestroy( media );
 }     

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoSetShuffleL( bool active )
{
    TX_ENTRY
    if ( iPlaybackUtility ) {
        iPlaybackUtility->SetL( EPbPropertyRandomMode, active );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoPreviewItemL( int index )
{
    TX_ENTRY
    if ( !iPlaybackUtility ) {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( mHostUid );
    }

    // Get the current path
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    MPX_DEBUG_PATH( *cpath );
    cpath->Back();

    CMPXMediaArray *mediaArray;
    const CMPXMedia& container = iCollectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>( container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    CMPXMedia* currentMedia( mediaArray->AtL( index ) );
    TMPXItemId id( currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

    cpath->AppendL( id ); // Top level items of songs
    cpath->Set( 0 ); // Select 1st song

    CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *cpath );
    CleanupStack::PushL( playList );
    playList->SetToFirst();

    iPlaybackUtility->InitL( *playList, ETrue );
    emit q_ptr->collectionPlaylistOpened();

    CleanupStack::PopAndDestroy( playList );
    CleanupStack::PopAndDestroy( cpath );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoOpenIsolatedCollectionL( TCollectionContext context )
{

    if ( ECollectionContextAllSongs == context ) {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicAllSongsPathL();
        CleanupStack::PushL( cpath );
        if ( !iIsolatedCollectionHelper ) {
            iIsolatedCollectionHelper = CMpMpxIsolatedCollectionHelper::NewL( this );
        }   
        iIsolatedCollectionHelper->OpenCollectionL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
    }
}

/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoReorderPlaylistL( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    iCollectionUiHelper->ReorderPlaylistL( playlistId, songId, originalOrdinal, newOrdinal, this );           
}


/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::DoHandleCollectionMessageL( 
    const CMPXMessage& aMsg )
{
    TX_ENTRY
    TMPXMessageId id( aMsg.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        TInt event( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TX_LOG_ARGS( "event=" << event << ", type=" << type << ", data=" << data );

        if ( event == TMPXCollectionMessage::EPathChanged &&
             type == EMcPathChangedByOpen &&
             data == EMcContainerOpened ) {
            // Incremental Open when browsing to the next level
            DoIncrementalOpenL();
        }
        else if ( event == TMPXCollectionMessage::EPathChanged &&
                  type == EMcPathChangedByOpen &&
                  data == EMcItemOpened ) {
            // Opened a song
            // This will result in HandleOpenL with CMPXCollectionPlaylist
            iCollectionUtility->Collection().OpenL();
        }
    }
    TX_EXIT
}


/*!
 \internal
 */
void MpMpxCollectionFrameworkWrapperPrivate::PreparePlaylistMediaL( 
        CMPXMedia& aMedia,
        QList<int> &selection,
        MpMpxCollectionData *collectionData )
{
    int count = selection.count();
    TX_ENTRY_ARGS( "selection count=" << count );

    const CMPXMedia& container = collectionData->containerMedia();
    const CMPXMediaArray* containerArray = container.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( containerArray ) );
    CMPXMediaArray* tracksArray( CMPXMediaArray::NewL() );
    CleanupStack::PushL( tracksArray );

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );
    TMPXItemId collectionId( path->Id( 0 ) );
    CleanupStack::PopAndDestroy( path );

    if ( collectionData->context() == ECollectionContextAlbums ) {
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* results;
            CMPXMedia* album( containerArray->AtL( selection[i] ) );
            // Fetch the songs for the selected album
            TMPXItemId albumId = album->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            CMPXMedia* findCriteria = CMPXMedia::NewL();
            CleanupStack::PushL( findCriteria );
            findCriteria->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXGroup );
            findCriteria->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXSong );
            findCriteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, albumId );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL( attrs );
            attrs.Append( TMPXAttribute( KMPXMediaIdGeneral,
                                         EMPXMediaGeneralTitle |
                                         EMPXMediaGeneralId ) );
            attrs.Append( KMPXMediaMusicAlbumTrack );
            results = iCollectionUtility->Collection().FindAllL( *findCriteria, attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( findCriteria );
            CleanupStack::PushL( results );
            
            const CMPXMediaArray* resultsArray = results->Value<CMPXMediaArray>( KMPXMediaArrayContents );
            User::LeaveIfNull( resultsArray );
            for ( int j = 0 ; j < resultsArray->Count();j++ ) {
                CMPXMedia* media( resultsArray->AtL( j ) );
                CMPXMedia* entry = CMPXMedia::NewL();
                CleanupStack::PushL( entry );
                entry->SetTextValueL( KMPXMediaGeneralTitle,
                    media->ValueText( KMPXMediaGeneralTitle ) );
                entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem );
                entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
                entry->SetTObjectValueL( KMPXMediaGeneralId,
                    media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                entry->SetTObjectValueL( KMPXMediaGeneralCollectionId, collectionId );
                CleanupStack::Pop( entry );
                tracksArray->AppendL( entry );
            }
            CleanupStack::PopAndDestroy( results );
        }
        
    }
    else {   
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* media( containerArray->AtL( selection.at( i ) ) );
            CMPXMedia* entry = CMPXMedia::NewL();
            CleanupStack::PushL( entry );
            entry->SetTextValueL( KMPXMediaGeneralTitle,
                media->ValueText( KMPXMediaGeneralTitle ) );
            entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem );
            entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
            entry->SetTObjectValueL( KMPXMediaGeneralId,
                media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            entry->SetTObjectValueL( KMPXMediaGeneralCollectionId, collectionId );
            CleanupStack::Pop( entry );
            tracksArray->AppendL( entry );
        }
    }
    aMedia.SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXItem );
    aMedia.SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXPlaylist );
    aMedia.SetCObjectValueL( KMPXMediaArrayContents, tracksArray );
    aMedia.SetTObjectValueL( KMPXMediaArrayCount, tracksArray->Count() );

    CleanupStack::PopAndDestroy( tracksArray );
    TX_EXIT
}
