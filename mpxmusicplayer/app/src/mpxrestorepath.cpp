/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Saves/restores current playback path
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <s32file.h>

#include <eikenv.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxcollectionopenutility.h>
#include <mpxsubscription.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>

#include "mpxrestorepath.h"
#include <mpxconstants.h>
#include "mpxlog.h"

// CONSTANTS
_LIT( KMPXCollectionPathFileName, "c:\\system\\data\\mpxcollectionpath.dat" );
const TInt KIncrementalFetchCount = 400;
const TInt KIncrementalNullOffset = 0;
const TInt KIncrementalLoadDelay = 1000000; // 1 second in ms
#define KMPXPdSbPlaybackViewImplementationId    0x10207BD0

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXRestorePath* CMPXRestorePath::NewL(MMPXPlaybackUtility* aPlaybackUtility,
                                       MMPXCollectionUiHelper* aCollectionUiHelper)
    {
    CMPXRestorePath* self = NewLC(aPlaybackUtility, aCollectionUiHelper);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXRestorePath* CMPXRestorePath::NewLC(MMPXPlaybackUtility* aPlaybackUtility,
                                        MMPXCollectionUiHelper* aCollectionUiHelper )
    {
    CMPXRestorePath* self = new (ELeave) CMPXRestorePath(aPlaybackUtility,
                                                         aCollectionUiHelper);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ----------------------------------------------------------------------------
//
CMPXRestorePath::CMPXRestorePath(MMPXPlaybackUtility* aPlaybackUtility,
                                 MMPXCollectionUiHelper* aCollectionUiHelper ):
                                             iPlaybackUtility(aPlaybackUtility),
                                             iCollectionUiHelper( aCollectionUiHelper ),
                                             iPathInitialized(0),
                                             iIsInitializing(EFalse),
                                             iInitCanceled(EFalse),
                                             iLoadSavedPath(EFalse),
                                             iRootPathId( KNullUid )
    {
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXRestorePath::~CMPXRestorePath()
    {
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    delete iIncOpenUtil;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXRestorePath::ConstructL()
    {
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeIsolated );

#ifdef __USE_MESSAGE_SUBSCRIPTION
    // Subscribe to only a few messages from collection utility
    CMPXSubscription* subscription( CMPXSubscription::NewL() );
    CleanupStack::PushL( subscription );
    CMPXSubscriptionItem* subItem1( CMPXSubscriptionItem::NewL() );
    CleanupStack::PushL( subItem1 );
    subItem1->SetTObjectValueL( KMPXMessageGeneralId, KMPXMessageGeneral );
    subItem1->SetTObjectValueL( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    subscription->AddItemL( *subItem1 );
    CMPXSubscriptionItem* subItem2( CMPXSubscriptionItem::CopyL( *subItem1 ));
    CleanupStack::PushL( subItem2 );
    subItem2->SetTObjectValueL( KMPXMessageGeneralEvent, TMPXCollectionMessage::EPathChanged );
    subItem2->SetTObjectValueL( KMPXMessageGeneralType, EMcPathChangedByOpen );
    subItem2->SetTObjectValueL( KMPXMessageGeneralData, EMcItemOpened );
    subscription->AddItemL( *subItem2 );
    iCollectionUtility->Collection().AddSubscriptionL( *subscription );
    CleanupStack::PopAndDestroy( subItem2 );
    CleanupStack::PopAndDestroy( subItem1 );
    CleanupStack::PopAndDestroy( subscription );
#endif

    iIncOpenUtil = CMPXCollectionOpenUtility::NewL( this, KMcModeIsolated );
    iMTPStarted = EFalse;
    }

// -----------------------------------------------------------------------------
// Load from file
// -----------------------------------------------------------------------------
//
void CMPXRestorePath::LoadPathL()
    {
    MPX_FUNC("CMPXRestorePath::LoadPathL()");
    if (iIsInitializing) // avoid reenter
        return;
    iDefaultPath = EFalse;

    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll( KMPXCollectionPathFileName );
    TParse parsedName;

    fs.Parse( KMPXCollectionPathFileName, parsedName );
    CFileStore* store( NULL );
    MPX_TRAPD( error, store = CDirectFileStore::OpenL(fs,
                                                 parsedName.FullName(),
                                                 EFileRead ); );

    // If we can internalize a collection path from file, we use that to
    // create a collection playlist. Otherwise, we construct the default
    // path using the collection helper.
    //
    CMPXCollectionPath* cpath( NULL );
    if ( error == KErrNone )
        {
        CleanupStack::PushL( store );

        RStoreReadStream stream;
        stream.OpenLC( *store, store->Root() );

        // Recreate the collection path that we were browsing
        cpath = CMPXCollectionPath::NewL(stream);
        CleanupStack::PushL(cpath);
        if( cpath->Levels() )
            {
            iRootPathId = TUid::Uid( cpath->Id( 0 ) );
            }
        TInt selection = stream.ReadInt32L();

        // bounds checking
        if( selection >=0 && selection < cpath->Count() )
            {
            cpath->Set( selection );

            // Find out what we were selecting
            iSelectionId = cpath->Id();
            iSelectionIndex = cpath->Index();

            MPX_DEBUG1("CMPXRestorePath::LoadPathL -- Opening Path in collection");

            // Rebrowse collection path of the container
            // We re-select the item when the container is opened and
            // in turn open the song
            //
            if( cpath->Levels() )
                {
                cpath->Back();
                }
            DoIncrementalOpenL( *cpath, iSelectionIndex, KIncrementalLoadDelay );
            }
        else
            {
            // Bad collection path, restore to default
            MPX_DEBUG1("CMPXRestorePath::LoadPathL -- bad collection path found");
            LoadDefaultPathL( KIncrementalLoadDelay );
            }

        CleanupStack::PopAndDestroy( cpath);
        CleanupStack::PopAndDestroy(&stream);

        CleanupStack::PopAndDestroy(store);
        }
    else
        {
        delete store;
        cpath = iCollectionUiHelper->CreateDefaultPlaylistPathLC();

        iDefaultPath = ETrue;
        iSelectionId = KMPXInvalidItemId;
        iSelectionIndex = 0;

        DoIncrementalOpenL( *cpath, iSelectionIndex, KIncrementalLoadDelay );
        CleanupStack::PopAndDestroy( cpath );
        }
    }

// ---------------------------------------------------------------------------
// Create the default path and load it in collection
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::LoadDefaultPathL( TInt aDelay )
    {
    MPX_DEBUG1("CMPXRestorePath::LoadDefaultPathL");
    CMPXCollectionPath* cpath = iCollectionUiHelper->CreateDefaultPlaylistPathLC();

    if (!iIsInitializing)
        {
        // Get incremental utility to open the path
        //
        DoIncrementalOpenL( *cpath, KIncrementalNullOffset, aDelay );

        iPathInitialized = EFalse;
        iIsInitializing = ETrue;
        iInitCanceled = EFalse;
        iDefaultPath = ETrue;
        iSelectionId = KMPXInvalidItemId;
        }
    CleanupStack::PopAndDestroy( cpath );
    }

// ---------------------------------------------------------------------------
// Save the currently playing playlist path
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::SavePathL()
    {
    RFs& fs = CEikonEnv::Static()->FsSession();
    fs.MkDirAll( KMPXCollectionPathFileName );
    TParse parsedName;

    fs.Parse( KMPXCollectionPathFileName, parsedName );
    CFileStore* store( NULL );
    MPX_TRAPD( error, store = CDirectFileStore::ReplaceL(fs,
                                                 parsedName.FullName(),
                                                 EFileWrite ); );

    if( error == KErrNone )
        {
        CleanupStack::PushL( store );
        store->SetTypeL(store->Layout());

        RStoreWriteStream wStream;
        TStreamId stream = wStream.CreateLC( *store );
        store->SetRootL( stream );

        MMPXSource* source = iPlaybackUtility->Source();
        CMPXCollectionPlaylist* pl( NULL );
        TInt levels(0);

        // Make sure we have a source, a playlist, and a valid path
        // Playback engine path could be invalid (0 levels) but have
        // a source and playlist if the currently playing playlist
        // is deleted.
        //
        if( source )
            {
            pl = source->PlaylistL();
            if (pl)
                {
                CleanupStack::PushL( pl );
                const CMPXCollectionPath& path = pl->Path();
                levels = path.Levels();
                if( levels > 0 )
                    {
                    wStream << path;
                    wStream.WriteInt32L( path.Index( path.Levels() - 1 ) );
                    wStream.CommitL();
                    }
                CleanupStack::PopAndDestroy( pl );
                }
            }

        CleanupStack::PopAndDestroy( &wStream );
        CleanupStack::PopAndDestroy( store );

        // Delete if source was invalid. Have to do it here
        // because store owned a file handle to the file
        //
        if( !source || !pl || !levels )
            {
            fs.Delete( KMPXCollectionPathFileName );
            }
        }
    else
        {
        // File cannot be opened, so try to delete it so that when
        // music player tries to load the path next time it'll use
        // the default path
        //
        fs.Delete( KMPXCollectionPathFileName );
        }
    }

// ---------------------------------------------------------------------------
// Check the initialized flag
// ---------------------------------------------------------------------------
//
TBool CMPXRestorePath::Initialized()
    {
    return iPathInitialized;
    }

// ---------------------------------------------------------------------------
// Check the iIsInitializing flag
// ---------------------------------------------------------------------------
//
TBool CMPXRestorePath::IsInitializing()
    {
    return iIsInitializing;
    }

// ---------------------------------------------------------------------------
// Set the initialized flag
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::SetInitialized( TBool aInit )
    {
    iPathInitialized = aInit;
    iIsInitializing = EFalse;
    }

// ---------------------------------------------------------------------------
// Cancels the initialize operation
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::CancelInit()
    {
    iInitCanceled = ETrue;
    SetInitialized( ETrue );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle media properties.
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::HandleCollectionMediaL(
    const CMPXMedia& /*aMedia*/,
    TInt /*aError*/ )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle collection message.
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::HandleCollectionMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the collection entries being opened.
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::HandleOpenL(
    const CMPXMedia& /* aEntries */,
    TInt /* aIndex */,
    TBool /* aComplete */,
    TInt aError )
    {
    // OpenL() callback from trying to open the default Music Menu -> All Songs
    // or from opening a container
    //
    MPX_DEBUG1("CMPXRestorePath::HandleOpenL -- Passing path to playback engine");

    // Stop the incremental open algorithm. We are only interested in fetcing
    // a single block containing the item we want to play. This will speed up the
    // initalization procedure. Once the playlist is created, the playlist
    // takes over the rest of the incremental open operation
    //
    iIncOpenUtil->Stop();
    if ( aError == KErrNone )
        {

        CMPXCollectionPath* cPath = iIncOpenUtil->PathL();
        CleanupStack::PushL( cPath );

        if( iSelectionId != KMPXInvalidItemId )
            {
            // Find the Index first
            TInt index = cPath->IndexOfId( iSelectionId );

            // If the item still exists in the path, we open it to init playback
            if( index != KErrNotFound )
                {
                cPath->Set( index );
                iCollectionUtility->Collection().OpenL( *cPath );
                }
            else
                {
                LoadDefaultPathL();
                }
            iSelectionId = KMPXInvalidItemId;
            }
        else  // Opening the default path
            {
            InitPlaybackEngineL( *cPath );
            }
        CleanupStack::PopAndDestroy( cPath );
        }
    else
        {
        if( iDefaultPath )
            {
            MPX_DEBUG1("CMPXRestorePath::HandleOpenL - Failed to open default path");
            // Failed to open default path, set to initialized
            // "No songs view"
            //
            iDefaultPath = EFalse;
            iPathInitialized = ETrue;
            iIsInitializing = EFalse;
            }
        else
            {
            MPX_DEBUG1("CMPXRestorePath::HandleOpenL - Opening default Path");
            iIsInitializing = EFalse;
            // Try opening default
            LoadDefaultPathL();
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }


// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the item being opened.
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::HandleOpenL(
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
    {
    MPX_FUNC("CMPXRestorePath::HandleOpenL");
    if( aError == KErrNone )
        {
        if ( !iInitCanceled )
            {
            MPX_DEBUG1( "CMPXRestorePath::HandleOpenL - Opening playlist" );

            // toggle repeat and shuffle to off if podcast playlist
            if(KMPXUidPodcastDBPlugin == TUid::Uid((TInt)aPlaylist.Path().Id(0)))
                {
                CMPXCollectionPlaylist* tmp =
                                CMPXCollectionPlaylist::NewL( aPlaylist );
                CleanupStack::PushL( tmp );
                tmp->SetRepeatEnabled( EFalse );
                tmp->SetShuffleEnabledL( EFalse );
                iPlaybackUtility->InitL( *tmp, EFalse );
                CleanupStack::PopAndDestroy( tmp );
                }
            else
                {
                iPlaybackUtility->InitL( aPlaylist, EFalse );
                }
            }
        else
            {
            MPX_DEBUG1( "CMPXRestorePath::HandleOpenL - Init canceled, do nothing" );
            }
        }
    else
        {
        if( iDefaultPath )
            {
            // Failed to open default path, set to initialized
            // "No songs view"
            //
            iDefaultPath = EFalse;
            iPathInitialized = ETrue;
            iIsInitializing = EFalse;
            }
        else
            {
            // Try opening default
            LoadDefaultPathL();
            }
        }
    }

// ---------------------------------------------------------------------------
// Initialize the playback engine with a collection path
// ---------------------------------------------------------------------------
//
void CMPXRestorePath::InitPlaybackEngineL( CMPXCollectionPath& aPath )
    {
    MPX_FUNC( "CMPXRestorePath::InitPlaybackEngineL" );
    // Do not load a playlist if USB started, this
    // could happen if the user switches mode from MTP to USB
    // which would lose the playback playlist
    //
    if( !iUSBOngoing && !iInitCanceled )
        {
        CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL( aPath );
        CleanupStack::PushL( playlist );
        iPlaybackUtility->InitL( *playlist, EFalse );
        CleanupStack::PopAndDestroy( playlist );
        }
    else
        {
        iIsInitializing = EFalse;
        iPathInitialized = ETrue;
        iInitCanceled = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// Handle playback message
// -----------------------------------------------------------------------------
//
void CMPXRestorePath::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXRestorePath::DoHandlePlaybackMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        MPX_DEBUG2( "CMPXRestorePath::DoHandlePlaybackMessageL(%d)", event );

        switch ( event )
            {
            default:
                {
                // ignore other messages
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMPXRestorePath::DoHandleCollectionMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXRestorePath::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        MPX_DEBUG3( "CMPXRestorePath::DoHandleCollectionMessageL Event = %d, Type() = %d",
            event, type );

        // When OpenL( *path ) is done we get a path changed msg
        //
        if ( event == TMPXCollectionMessage::EPathChanged &&
            type == EMcPathChangedByOpen &&
            data == EMcItemOpened )
            {
            MPX_DEBUG1("CMPXRestorePath::DoHandleCollectionMessageL -- opening new path");
            iCollectionUtility->Collection().OpenL();
            }
        else if( event == TMPXCollectionMessage::EBroadcastEvent )
            {
            // Restore default path after refresh or end of mtp
            //
            if( type == EMcMsgRefreshEnd )
                {
                MPX_DEBUG1( "CMPXRestorePath::DoHandleCollectionMessageL -- Refresh End" );
                if( iLoadSavedPath )
                    {
                    MPX_DEBUG1("  ---> Loading a saved path");
                    LoadPathL();
                    }
                else
                    {
                    MPX_DEBUG1("  ---> Loading a default path");
                    LoadDefaultPathL();
                    }
                iLoadSavedPath = EFalse;
                }
            else if ( type == EMcMsgUSBMTPEnd )
            	{
                if ( iMTPStarted )
                    {
                    MPX_DEBUG1( "CMPXRestorePath::DoHandleCollectionMessageL -- MTP End" );
                    if( iLoadSavedPath )
                        {
                        MPX_DEBUG1("  ---> Loading a saved path");
                        LoadPathL();
                        }
                    else
                        {
                        MPX_DEBUG1("  ---> Loading a default path");
                        LoadDefaultPathL();
                        }
                    iLoadSavedPath = EFalse;   
                    iMTPStarted = EFalse;
                    }
            	}
            // Disk inserted, we try to keep playback playlist at the
            // currently selected song after the automatic refresh
            //
            // USB mass storage dismounts the disk, but in that case
            // we do want to revert back to first song of all songs
            //
            else if( type == EMcMsgDiskInserted &&
                     !iUSBOngoing )
                {
                SavePathL();
                iLoadSavedPath = ETrue;
                }
            else if( type == EMcMsgUSBMassStorageStart )
                {
                iUSBOngoing = ETrue;
                }
            else if( type == EMcMsgUSBMassStorageEnd )
                {
                iUSBOngoing = EFalse;
                }
            else if( type == EMcMsgDiskRemoved
                     && !iUSBOngoing )
                {
                MPX_DEBUG1("  ---> Save the current path and restore it");
                SavePathL();
                LoadPathL();
                }
            else if( type == EMcMsgFormatEnd )
                {
                MPX_DEBUG1(" --> Loading a default path after formatting ");
                LoadDefaultPathL();
                }
            else if ( type == EMcMsgUSBMTPStart )
                {
                iMTPStarted = ETrue;
                }
            }
        else if ( event == TMPXCollectionMessage::ECollectionChanged && !iInitCanceled )
            {
            //To Handle the case when path is restored and we are in playback view.
            //This will switch to the right playback view to prevent podcast playback on music playback view.
            MMPXViewUtility* viewUtility = MMPXViewUtility::UtilityL();
            if (viewUtility->ActiveViewType() == TUid::Uid( KMPXPluginTypePlaybackUid ) && 
                    viewUtility->ActiveViewImplementationUid() != TUid::Uid(KMPXPdSbPlaybackViewImplementationId))
                {
                MMPXPlayer* player = iPlaybackUtility->PlayerManager().CurrentPlayer();
                if ( player )
                    {
                    RArray<TUid> array;
                    CleanupClosePushL( array );
                    array.AppendL( player->UidL() );
                    array.AppendL(TUid::Uid(data));
                    viewUtility->ActivateViewL( array );
                    CleanupStack::PopAndDestroy( &array );
                    }
                }
            viewUtility->Close();
            }
        else
            {
            // Ignore all other messages
            }
        }
    }

// -----------------------------------------------------------------------------
// Start the incremental reading operation at an offset
// -----------------------------------------------------------------------------
//
void CMPXRestorePath::DoIncrementalOpenL( CMPXCollectionPath& aPath,
                                          TInt aIndex,
                                          TInt aDelay )
    {
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    TArray<TMPXAttribute> ary = attrs.Array();
    iIncOpenUtil->Stop();
    iIncOpenUtil->SetDelay( aDelay );
    iIncOpenUtil->StartL( aPath, ary, KIncrementalFetchCount,
                          aIndex, CMPXCollectionOpenUtility::EFetchNormal );
    CleanupStack::PopAndDestroy( &attrs );
    }

// -----------------------------------------------------------------------------
// Get the Uid of the root path 
// -----------------------------------------------------------------------------
//
TUid CMPXRestorePath::RooPathId()
    {
    return iRootPathId;
    }
// End of File
