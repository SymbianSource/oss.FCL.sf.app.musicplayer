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
* Description: Wrapper for mpx framework utilities - private implementation.
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

#include "mpmpxframeworkwrapper_p.h"
#include "mpmpxframeworkwrapper.h"
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
    \class MpMpxFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxFrameworkWrapperPrivate::MpMpxFrameworkWrapperPrivate( MpMpxFrameworkWrapper *wrapper )
    : q_ptr(wrapper),
      iCollectionUtility(0),
      iCollectionUiHelper(0),
      iIncrementalOpenUtil(0),
      iPlaybackUtility(0),
      iHarvesterUtility(0),
      iCollectionData(0),
      iFirstIncrementalOpen(EFalse),
      iUserPlaylists(0)
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxFrameworkWrapperPrivate::~MpMpxFrameworkWrapperPrivate()
{
    TX_ENTRY
    delete iCollectionData;

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

    if ( iHarvesterUtility ) {
        iHarvesterUtility->Close();
    }

    delete iIncrementalOpenUtil;
    delete iUserPlaylists;

    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::init( MpCommon::MpViewMode viewMode )
{

    iViewMode = viewMode;
    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::openCollection( TCollectionContext context )
{
    TRAPD(err, DoOpenCollectionL(context));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::openCollectionItem( int index )
{
    TRAPD(err, DoOpenCollectionItemL(index));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::reopenCollection()
{
    TRAPD(err, DoReopenCollectionL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::back()
{
    TRAPD(err, DoBackL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}
/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::findPlaylists( QStringList &playlists )
{
    TRAPD(err, DoFindPlaylistsL(playlists));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::createPlaylist( QString playlistName, QList<int> selection )
{
    TRAPD(err, DoCreatePlaylistL(playlistName, selection));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::saveToPlaylist( int playlistIndex, QList<int> selection )
{
    TRAPD(err, DoSaveToPlaylistL(playlistIndex, selection));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::deleteSongs( QList<int> selection )
{
    TRAPD(err, DoDeleteSongsL(selection));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::renamePlaylist( QString newName, int index )
{      
    TRAPD(err, DoRenamePlaylistL(newName, index));
    if ( err != KErrNone ) {
    TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::renamePlaylist( QString newName )
{      
    TRAPD(err, DoRenamePlaylistL(newName));
    if ( err != KErrNone ) {
    TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}
/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::setShuffle( bool active )
{
    TRAPD(err, DoSetShuffleL(active));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::scan()
{
    TRAPD(err, DoScanL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::cancelScan()
{
    TRAPD(err, DoCancelScanL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::previewItem( int index )
{
    TRAPD(err, DoPreviewItemL(index));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
MpMpxCollectionData *MpMpxFrameworkWrapperPrivate::collectionData()
{
    return iCollectionData;
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::HandleOpenL(
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool aComplete,
    TInt aError )
{
    Q_UNUSED(aIndex);
    TX_UNUSED(aComplete);
    TX_ENTRY_ARGS("aError=" << aError << "aComplete=" << aComplete);
    if ( aError == KErrNone ) {
        if ( iFirstIncrementalOpen ) {
            iCollectionData->setMpxMedia(aEntries);
            iFirstIncrementalOpen = EFalse;
        }
    }
    else {
        TX_LOG_ARGS("Error: " << aError << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::HandleOpenL(
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
{
    TX_ENTRY_ARGS("aError=" << aError);

    if ( aError == KErrNone ) {
        if ( !iPlaybackUtility ) {
            iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid(MpCommon::KMusicPlayerUid) );
            if ( iViewMode == MpCommon::DefaultView ) {
                iPlaybackUtility->SetL( EPbPropertyRandomMode, MpSettingsManager::shuffle() ? ETrue : EFalse );
                iPlaybackUtility->SetL( EPbPropertyRepeatMode, MpSettingsManager::repeat() ? EPbRepeatAll : EPbRepeatOff);
            }
        }
        iPlaybackUtility->InitL( aPlaylist, ETrue );
        emit q_ptr->collectionPlaylistOpened();
    }
    else {
        TX_LOG_ARGS("Error: " << aError << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::HandleCollectionMessage(
    CMPXMessage* aMsg,
    TInt aErr )
{
    TX_ENTRY_ARGS("aErr=" << aErr);
    if ( aErr == KErrNone && aMsg )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMsg ) );
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
{
    Q_UNUSED(aMedia);
    Q_UNUSED(aError);
}

/*!
 \internal
 Handles the completion of any collection helper event.
 */
void MpMpxFrameworkWrapperPrivate::HandleOperationCompleteL(
    TCHelperOperation aOperation,
    TInt aErr,
    void* aArgument )
{
    TX_ENTRY_ARGS("aErr=" << aErr);
    switch( aOperation ) {
    case EDeleteOp:
        if ( KErrNone == aErr ) {
            emit q_ptr->songsDeleted(true);
        }
        else {
            emit q_ptr->songsDeleted(false);
        }
        break;
    case EAddOp:
        if ( KErrNone == aErr ) {
            emit q_ptr->playlistSaved(true);
        }
        else {
            emit q_ptr->playlistSaved(false);
        }
        break;
    case ERenameOp:
        if ( KErrNone == aErr ) {
            emit q_ptr->playlistsRenamed(true);
        }
        else {
            emit q_ptr->playlistsRenamed(false);
        }
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
void MpMpxFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iCollectionUiHelper = CMPXCollectionHelperFactory:: NewCollectionUiHelperL();
    iIncrementalOpenUtil = CMPXCollectionOpenUtility::NewL( this );

    iHarvesterUtility = CMPXHarvesterFactory::NewL();
    iHarvesterUtility->CheckForSystemEventsL();

    iCollectionData = new MpMpxCollectionData();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoOpenCollectionL(
    TCollectionContext aContext )
{
    TX_ENTRY_ARGS("aContext=" << aContext);

    switch ( aContext ) {
    case ECollectionContextAllSongs:
        {
        CMPXCollectionPath* cpath = iCollectionUiHelper->MusicAllSongsPathL();
        CleanupStack::PushL( cpath );
        iCollectionUtility->Collection().OpenL( *cpath );
        CleanupStack::PopAndDestroy( cpath );
        break;
        }
    case ECollectionContextArtistAlbums:
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
        TX_LOG_ARGS("Error: Unexpected context; should never get here.");
        break;
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoOpenCollectionItemL( TInt aIndex )
    {
    TX_ENTRY_ARGS("aIndex=" << aIndex);
    iCollectionUtility->Collection().OpenL( aIndex );
    TX_EXIT
    }

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoIncrementalOpenL()
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
void MpMpxFrameworkWrapperPrivate::DoReopenCollectionL()
{
    TX_ENTRY
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    cpath->Back();
    iCollectionUtility->Collection().OpenL(*cpath);
    CleanupStack::PopAndDestroy( cpath );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoBackL()
{
    TX_ENTRY
    iCollectionUtility->Collection().BackL();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoFindPlaylistsL( QStringList &playlists )
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
    ary.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    criteria->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );

    iUserPlaylists = iCollectionUtility->Collection().FindAllL( *criteria, attrs.Array() );
    CleanupStack::PopAndDestroy( criteria );
    CleanupStack::PopAndDestroy( &attrs );

    if ( iUserPlaylists ) {
        const CMPXMediaArray* mediaArray =
            iUserPlaylists->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));
        TInt count = mediaArray->Count();
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* media( mediaArray->AtL(i) );
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
void MpMpxFrameworkWrapperPrivate::DoCreatePlaylistL( QString playlistName, QList<int> selection )
{
    TX_ENTRY_ARGS("playlistName=" << playlistName);
    CMPXMedia* tracks = CMPXMedia::NewL();
    CleanupStack::PushL( tracks );

    TPtrC ptr(reinterpret_cast<const TText*>(playlistName.constData()));
    tracks->SetTextValueL( KMPXMediaGeneralTitle, ptr );
    tracks->SetTextValueL( KMPXMediaGeneralUri, KPlaylistPath );
    PreparePlaylistMediaL(*tracks, selection);

    iCollectionUiHelper->IncAddL( *tracks, this, KMPXChunkSize );
    CleanupStack::PopAndDestroy( tracks );
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoSaveToPlaylistL( int playlistIndex, QList<int> selection )
{
    TX_ENTRY_ARGS("playlistIndex=" << playlistIndex);
    CMPXMedia* tracks = CMPXMedia::NewL();
    CleanupStack::PushL( tracks );

    const CMPXMediaArray* mediaArray =
        iUserPlaylists->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ));
    CMPXMedia* media( mediaArray->AtL(playlistIndex) );
    TMPXItemId playlistId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );

    RArray<TUid> ary;
    CleanupClosePushL( ary );
    ary.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
    TUid musicCollection = iCollectionUtility->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    tracks->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, playlistId );
    tracks->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, musicCollection );
    PreparePlaylistMediaL(*tracks, selection);

    iCollectionUiHelper->IncAddL( *tracks, this, KMPXChunkSize );
    CleanupStack::PopAndDestroy( tracks );
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoDeleteSongsL( QList<int> selection )
{
    int count = selection.count();
    TX_ENTRY_ARGS("selection count=" << count);

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );

    for ( TInt i = 0; i < count; i++ ) {
        path->SelectL( selection.at(i) );
    }
    iCollectionUiHelper->DeleteL( *path, this );
    CleanupStack::PopAndDestroy( path );
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoRenamePlaylistL( QString newName, int index)
{
    CMPXMediaArray *mediaArray;  
    const CMPXMedia& container = iCollectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>(container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    CMPXMedia* currentPlaylistMedia( mediaArray->AtL( index ) );
    TMPXItemId id( currentPlaylistMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoRenamePlaylistL( id, newName );
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoRenamePlaylistL( QString newName )
{
    const CMPXMedia& container = iCollectionData->containerMedia();
    TMPXItemId id( container.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    DoRenamePlaylistL( id, newName );
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoRenamePlaylistL( TMPXItemId id, QString newName )
 {
     CMPXMedia* media = CMPXMedia::NewL();
     CleanupStack::PushL( media );
     media->SetTObjectValueL<TMPXGeneralType>(
     KMPXMediaGeneralType, EMPXItem );
     media->SetTObjectValueL<TMPXGeneralCategory>(
     KMPXMediaGeneralCategory, EMPXPlaylist );
     media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
     TPtrC ptr(reinterpret_cast<const TText*>(newName.constData()));
     media->SetTextValueL( KMPXMediaGeneralTitle, ptr );
     iCollectionUiHelper->RenameL( *media, this );
     CleanupStack::PopAndDestroy( media );
 }     

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoSetShuffleL( bool active )
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
void MpMpxFrameworkWrapperPrivate::DoScanL()
{
    TX_ENTRY
    iNumItemsAdded = 0;
    iHarvesterUtility->ScanL();
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoCancelScanL()
{
    TX_ENTRY
    // If harvester crashes, restart it.
    TRAPD( err, iHarvesterUtility->CancelScanL() );
    if ( err != KErrNone ) {
        iHarvesterUtility->Close();
        iHarvesterUtility = NULL;
        iHarvesterUtility = CMPXHarvesterFactory::NewL();
        iHarvesterUtility->CheckForSystemEventsL();
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxFrameworkWrapperPrivate::DoPreviewItemL( int index )
{
    TX_ENTRY
    if ( !iPlaybackUtility ) {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid(MpCommon::KMusicPlayerUid) );
    }

    // Get the current path
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    MPX_DEBUG_PATH(*cpath);
    cpath->Back();

    CMPXMediaArray *mediaArray;
    const CMPXMedia& container = iCollectionData->containerMedia();
    mediaArray = const_cast<CMPXMediaArray*>(container.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    CMPXMedia* currentMedia( mediaArray->AtL( index ) );
    TMPXItemId id( currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

    cpath->AppendL(id); // Top level items of songs
    cpath->Set(0); 		// Select 1st song

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
void MpMpxFrameworkWrapperPrivate::DoHandleCollectionMessageL(
    const CMPXMessage& aMsg )
{
    TX_ENTRY
    TMPXMessageId id( aMsg.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) {
        TInt event( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TX_LOG_ARGS("event=" << event << ", type=" << type << ", data=" << data);

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

        if ( event == TMPXCollectionMessage::EBroadcastEvent ) {
            switch (type) {
                case EMcMsgRefreshStart:
                    emit q_ptr->scanStarted();
                    break;
                case EMcMsgRefreshEnd:
                case EMcMsgDiskInserted:
                    emit q_ptr->scanEnded();
                    break;
                default:
                    break;
            }
        }
    }
    else if( KMPXMessageIdItemChanged == id ) {
        if ( aMsg.IsSupported(KMPXMessageArrayContents) ) {
            // Loop through messages for arrays
            const CMPXMessageArray* messageArray =
                        aMsg.Value<CMPXMessageArray>(KMPXMessageArrayContents);
            User::LeaveIfNull(const_cast<CMPXMessageArray*>(messageArray));

            for ( TInt i=0; i<messageArray->Count(); ++i ) {
                  HandleCollectionMessage( messageArray->AtL( i ), KErrNone );
            }
            emit q_ptr->scanCountChanged(iNumItemsAdded);
        }
        else {
            // Single item
            TMPXChangeEventType changeType( aMsg.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
            TMPXGeneralCategory cat(aMsg.ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory));
            if( changeType == EMPXItemInserted &&
                (cat == EMPXSong || cat == EMPXPlaylist || cat == EMPXPodcast) ) {
                iNumItemsAdded++;
            }
        }
    }
    TX_EXIT
}

void MpMpxFrameworkWrapperPrivate::PreparePlaylistMediaL(
    CMPXMedia& aMedia,
    QList<int> selection )
{
    int count = selection.count();
    TX_ENTRY_ARGS("selection count=" << count);

    const CMPXMedia& container = iCollectionData->containerMedia();
    const CMPXMediaArray* containerArray = container.Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( containerArray ));
    CMPXMediaArray* tracksArray( CMPXMediaArray::NewL() );
    CleanupStack::PushL( tracksArray );

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );
    TMPXItemId collectionId( path->Id( 0 ) );
    CleanupStack::PopAndDestroy( path );

    if ( iCollectionData->context() == ECollectionContextArtistAlbums ) {
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* results;
            CMPXMedia* album( containerArray->AtL( selection[i] ) );
            // Fetch the songs for the selected album
            TMPXItemId albumId = album->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
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
            results = iCollectionUtility->Collection().FindAllL( *findCriteria, attrs.Array());
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( findCriteria );
            CleanupStack::PushL( results );
            
            const CMPXMediaArray* resultsArray = results->Value<CMPXMediaArray>( KMPXMediaArrayContents );
            User::LeaveIfNull( resultsArray );
            for (int j = 0 ; j < resultsArray->Count();j++) {
                CMPXMedia* media( resultsArray->AtL(j) );
                CMPXMedia* entry = CMPXMedia::NewL();
                CleanupStack::PushL( entry );
                entry->SetTextValueL( KMPXMediaGeneralTitle,
                    media->ValueText( KMPXMediaGeneralTitle ) );
                entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem);
                entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
                entry->SetTObjectValueL( KMPXMediaGeneralId,
                    media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                entry->SetTObjectValueL( KMPXMediaGeneralCollectionId, collectionId );
                tracksArray->AppendL( entry );
                CleanupStack::Pop( entry );
            }
            CleanupStack::PopAndDestroy( results );
        }
        
    }
    else {   
        for ( TInt i = 0; i < count; i++ ) {
            CMPXMedia* media( containerArray->AtL(selection.at(i)) );
            CMPXMedia* entry = CMPXMedia::NewL();
            CleanupStack::PushL( entry );
            entry->SetTextValueL( KMPXMediaGeneralTitle,
                media->ValueText( KMPXMediaGeneralTitle ) );
            entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem);
            entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
            entry->SetTObjectValueL( KMPXMediaGeneralId,
                media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            entry->SetTObjectValueL( KMPXMediaGeneralCollectionId, collectionId );
            tracksArray->AppendL( entry );
            CleanupStack::Pop( entry );
        }
    }
    aMedia.SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXItem );
    aMedia.SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXPlaylist );
    aMedia.SetCObjectValueL( KMPXMediaArrayContents, tracksArray );
    aMedia.SetTObjectValueL( KMPXMediaArrayCount, tracksArray->Count() );

    CleanupStack::PopAndDestroy( tracksArray );
    TX_EXIT
}
