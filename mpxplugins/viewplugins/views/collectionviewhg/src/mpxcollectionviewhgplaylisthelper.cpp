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
* Description:  Helper to construct a collection playlist.
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <s32file.h>
#include <e32math.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionuihelper.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxcollectionopenutility.h>
#include <mpxconstants.h>

#include "mpxcollectionviewhgplaylisthelper.h"
#include "mpxlog.h"

// CONSTANTS
const TInt KIncrementalFetchCount = 400;
const TInt KIncrementalNullOffset = 0;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXCollectionViewHgPlaylistHelper* CMPXCollectionViewHgPlaylistHelper::NewL(
	MMPXPlaybackUtility* aPlaybackUtility,
	MMPXCollectionUiHelper* aCollectionUiHelper)
    {
    CMPXCollectionViewHgPlaylistHelper* self = NewLC(aPlaybackUtility, aCollectionUiHelper);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXCollectionViewHgPlaylistHelper* CMPXCollectionViewHgPlaylistHelper::NewLC(
	MMPXPlaybackUtility* aPlaybackUtility,
	MMPXCollectionUiHelper* aCollectionUiHelper )
    {
    CMPXCollectionViewHgPlaylistHelper* self =
    	new (ELeave) CMPXCollectionViewHgPlaylistHelper(aPlaybackUtility,
                                                         aCollectionUiHelper);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ----------------------------------------------------------------------------
//
CMPXCollectionViewHgPlaylistHelper::CMPXCollectionViewHgPlaylistHelper(
	MMPXPlaybackUtility* aPlaybackUtility,
	MMPXCollectionUiHelper* aCollectionUiHelper )
	: iPlaybackUtility(aPlaybackUtility),
	  iCollectionUiHelper( aCollectionUiHelper ),
	  iPathInitialized(0),
	  iIsInitializing(EFalse),
	  iInitCanceled(EFalse),
	  iLoadSavedPath(EFalse),
	  iEnableShuffle(EFalse)
    {
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXCollectionViewHgPlaylistHelper::~CMPXCollectionViewHgPlaylistHelper()
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
void CMPXCollectionViewHgPlaylistHelper::ConstructL()
    {
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeIsolated );
    iIncOpenUtil = CMPXCollectionOpenUtility::NewL( this, KMcModeIsolated );
    }


// ---------------------------------------------------------------------------
// Create the default path and load it in collection
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgPlaylistHelper::LoadDefaultPathL( TInt aDelay )
    {
    MPX_DEBUG1("CMPXCollectionViewHgPlaylistHelper::LoadDefaultPathL");
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
// Create playlist from default path.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgPlaylistHelper::InitPlaylistL(TBool aEnableShuffle)
    {
    MPX_DEBUG1("CMPXCollectionViewHgPlaylistHelper::InitPlaylistL");
    CMPXCollectionPath* cpath = iCollectionUiHelper->CreateDefaultPlaylistPathLC();

    if (!iIsInitializing)
        {
        // Get incremental utility to open the path
        //
        DoIncrementalOpenL( *cpath, KIncrementalNullOffset, 0 );

        iPathInitialized = EFalse;
        iIsInitializing = ETrue;
        iInitCanceled = EFalse;
        iDefaultPath = ETrue;
        iSelectionId = KMPXInvalidItemId;
        iEnableShuffle = aEnableShuffle;
        }
    CleanupStack::PopAndDestroy( cpath );
    }

// ---------------------------------------------------------------------------
// Create playlist from provided path
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgPlaylistHelper::InitPlaylistL(CMPXCollectionPath& aPath, TBool aEnableShuffle)
    {
    MPX_DEBUG1("CMPXCollectionViewHgPlaylistHelper::InitPlaylistL");

    if (!iIsInitializing)
        {
		if (aEnableShuffle)
			{
			TTime time;
			time.UniversalTime();
			TInt64 seed = time.Int64();
			TInt randIndex = Math::Rand(seed) % aPath.Count();
			aPath.Set(randIndex);
			}

        CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL( aPath );
        CleanupStack::PushL( playlist );

        playlist->SetShuffleEnabledL( aEnableShuffle );
        iPlaybackUtility->SetL( EPbPropertyRandomMode, aEnableShuffle );
        iPlaybackUtility->InitL( *playlist, ETrue );
        CleanupStack::PopAndDestroy( playlist );

		//iPlaybackUtility->SetL( EPbPropertyRandomMode, aEnableShuffle );
		//iCollectionUtility->Collection().OpenL(aPath);

        iPathInitialized = ETrue;
        iInitCanceled = EFalse;
        iDefaultPath = EFalse;
        iSelectionId = KMPXInvalidItemId;
        }
    }


// ---------------------------------------------------------------------------
// Check the initialized flag
// ---------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgPlaylistHelper::Initialized()
    {
    return iPathInitialized;
    }

// ---------------------------------------------------------------------------
// Check the iIsInitializing flag
// ---------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgPlaylistHelper::IsInitializing()
    {
    return iIsInitializing;
    }

// ---------------------------------------------------------------------------
// Set the initialized flag
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgPlaylistHelper::SetInitialized( TBool aInit )
    {
    iPathInitialized = aInit;
    iIsInitializing = EFalse;
    }

// ---------------------------------------------------------------------------
// Cancels the initialize operation
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgPlaylistHelper::CancelInit()
    {
    iInitCanceled = ETrue;
    SetInitialized( ETrue );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle media properties.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgPlaylistHelper::HandleCollectionMediaL(
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
void CMPXCollectionViewHgPlaylistHelper::HandleCollectionMessage(
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
void CMPXCollectionViewHgPlaylistHelper::HandleOpenL(
    const CMPXMedia& /* aEntries */,
    TInt /* aIndex */,
    TBool /* aComplete */,
    TInt aError )
    {
    // OpenL() callback from trying to open the default Music Menu -> All Songs
    // or from opening a container
    //
    MPX_DEBUG1("CMPXCollectionViewHgPlaylistHelper::HandleOpenL -- Passing path to playback engine");

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
		InitPlaybackEngineL( *cPath );
        CleanupStack::PopAndDestroy( cPath );
        iIsInitializing = EFalse;
        iPathInitialized = ETrue;
        iInitCanceled = EFalse;
        }
    else
        {
        if( iDefaultPath )
            {
            MPX_DEBUG1("CMPXCollectionViewHgPlaylistHelper::HandleOpenL - Failed to open default path");
            // Failed to open default path, set to initialized
            // "No songs view"
            //
            iDefaultPath = EFalse;
            iPathInitialized = ETrue;
            iIsInitializing = EFalse;
            }
        else
            {
            MPX_DEBUG1("CMPXCollectionViewHgPlaylistHelper::HandleOpenL - Opening default Path");
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
void CMPXCollectionViewHgPlaylistHelper::HandlePlaybackMessage(
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
void CMPXCollectionViewHgPlaylistHelper::HandleOpenL(
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
    {
    MPX_FUNC("CMPXCollectionViewHgPlaylistHelper::HandleOpenL");
    if( aError == KErrNone )
        {
        if ( !iInitCanceled )
            {
            MPX_DEBUG1( "CMPXCollectionViewHgPlaylistHelper::HandleOpenL - Opening playlist" );

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
            MPX_DEBUG1( "CMPXCollectionViewHgPlaylistHelper::HandleOpenL - Init canceled, do nothing" );
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
void CMPXCollectionViewHgPlaylistHelper::InitPlaybackEngineL( CMPXCollectionPath& aPath )
    {
    MPX_FUNC( "CMPXCollectionViewHgPlaylistHelper::InitPlaybackEngineL" );

    if( !iInitCanceled )
        {
        aPath.Remove(0); // we want to remove index 0 which is shuffle item
        TTime time;
        time.UniversalTime();
        TInt64 seed = time.Int64();
        TInt randIndex = Math::Rand(seed) % aPath.Count();
        aPath.Set(randIndex);
        CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL( aPath );
        CleanupStack::PushL( playlist );
        playlist->SetShuffleEnabledL( ETrue );
        iPlaybackUtility->SetL( EPbPropertyRandomMode, iEnableShuffle );
        iPlaybackUtility->InitL( *playlist, ETrue );
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
void CMPXCollectionViewHgPlaylistHelper::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXCollectionViewHgPlaylistHelper::DoHandlePlaybackMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        MPX_DEBUG2( "CMPXCollectionViewHgPlaylistHelper::DoHandlePlaybackMessageL(%d)", event );

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
void CMPXCollectionViewHgPlaylistHelper::DoHandleCollectionMessageL(
	const CMPXMessage& /*aMessage*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgPlaylistHelper::DoHandleCollectionMessageL" );

    }

// -----------------------------------------------------------------------------
// Start the incremental reading operation at an offset
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgPlaylistHelper::DoIncrementalOpenL( CMPXCollectionPath& aPath,
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


// End of File
