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
* Description:  Handles commands passed in by other appilcations
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <apgtask.h>
#include <eikenv.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionopenutility.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionuihelper.h>
#include <mpxparameter.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>

#include "mpxtlshelper.h"
#include <mpxconstants.h>
#include "mpxcommandhandler.h"
#include "mpxlog.h"
#include <mpxviewpluginmanager.h>
#include <mpxviewplugin.h>

const TInt KIncrementalFetchCount = 10; // all we need is the first song in all song
const TInt KWaitNoteImpUid = 0x101FFC6C; // refresh wait note UID

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXCommandHandler* CMPXCommandHandler::NewL(
    MMPXPlaybackUtility* aPlaybackUtility,
    TUid aCollectionMode,
    const TDesC8& aParameter )
    {
    CMPXCommandHandler* self = NewLC(
        aPlaybackUtility, aCollectionMode, aParameter );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXCommandHandler* CMPXCommandHandler::NewLC(
    MMPXPlaybackUtility* aPlaybackUtility,
    TUid aCollectionMode,
    const TDesC8& aParameter )
    {
    CMPXCommandHandler* self = new ( ELeave )
        CMPXCommandHandler( aPlaybackUtility );
    CleanupStack::PushL(self);
    self->ConstructL( aCollectionMode, aParameter );
    return self;
    }

// ----------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ----------------------------------------------------------------------------
//
CMPXCommandHandler::CMPXCommandHandler(
    MMPXPlaybackUtility* aPlaybackUtility ):
    iPlaybackUtility( aPlaybackUtility )
    ,iPathInited( EFalse )
    ,iLastPlayedAutostart(EFalse)
    {
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXCommandHandler::~CMPXCommandHandler()
    {
    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        }
    if ( iMusicPlayerCollectionUtility )
        {
        iMusicPlayerCollectionUtility->Close();
        }
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }
    delete iIsolatedIncOpenUtil;
    delete iParameter;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXCommandHandler::ConstructL(
    TUid aCollectionMode,
    const TDesC8& aParameter )
    {
    iIsolatedIncOpenUtil =
        CMPXCollectionOpenUtility::NewL( this, KMcModeIsolated );
    iMusicPlayerCollectionUtility = MMPXCollectionUtility::NewL(
        this, aCollectionMode );
    iPlaybackUtility->AddObserverL( *this );
    iViewUtility = MMPXViewUtility::UtilityL();
    ExtractMessageL( aParameter );
    }

// ---------------------------------------------------------------------------
// Start processing the command
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::ProcessMessageL( TBool aIsUsbInUse, TBool aRefreshing, TBool aFormattingOnGoing )
    {
    MPX_FUNC( "CMPXCommandHandler::ProcessMessageL" );
    iPathInited = EFalse;
    iUsbInUse = aIsUsbInUse;
    iRefreshOngoing = aRefreshing;
    if ( iUsbInUse || iRefreshOngoing || aFormattingOnGoing )
        {
        MPX_DEBUG1( "CMPXCommandHandler::ProcessMessageL USB/Refresh/Formatting ongoing, bring app to foreground only" );
        // USB is in use, bring the app to foreground and ignore the message
        delete iParameter;
        iParameter = NULL;
        // have to use window group to activate, in case some other window
        // has higher priority than us (e.g. playback popup and volume popup)
        RWindowGroup myWin = CEikonEnv::Static()->RootWin();
        myWin.SetOrdinalPosition( 0, ECoeWinPriorityNormal );
        }
    else
        {
        if ( iParameter )
            {
            if ( CEikonEnv::Static()->StartedAsServerApp() )
                {
                // embedded mode, can't check if db exist
                // just process the message
                MPX_DEBUG1( "CMPXCommandHandler::ProcessMessageL embedded mode" );
                DoHandleMessageL();
                }
            else
                {
                // check if DB is created
                MPX_DEBUG1( "CMPXCommandHandler::ProcessMessageL check if db is created" );
                RArray<TMPXAttribute> atts;
                CleanupClosePushL( atts );
                atts.Append( KMPXMediaColDetailDBCreated );
                if ( iParameter->iCollectionPath )
                    {
                    iMusicPlayerCollectionUtility->Collection().MediaL(
                        *iParameter->iCollectionPath, atts.Array() );
                    }
                else
                    {
                    MMPXCollectionUiHelper* colUiHelper =
                        CMPXCollectionHelperFactory::NewCollectionUiHelperL();
                    CMPXCollectionPath* cpath = colUiHelper->MusicMenuPathL();
                    CleanupStack::PushL( cpath );
                    iMusicPlayerCollectionUtility->Collection().MediaL(
                        *cpath, atts.Array() );
                    colUiHelper->Close();
                    CleanupStack::PopAndDestroy( cpath );
                    }
                CleanupStack::PopAndDestroy( &atts );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Returns if a path has been inited in playback engine
// ---------------------------------------------------------------------------
//
TBool CMPXCommandHandler::IsPathInited()
    {
    MPX_FUNC( "CMPXCommandHandler::IsPathInited" );
    return iPathInited;
    }

// ---------------------------------------------------------------------------
// Set usb status
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::SetUsbInUse( TBool aIsUsbInUse )
    {
    MPX_FUNC( "CMPXCommandHandler::SetUsbInUse" );
    iUsbInUse = aIsUsbInUse;
    }

// ---------------------------------------------------------------------------
// Set refresh status
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::SetRefreshState( TBool aRefreshing )
    {
    MPX_FUNC( "CMPXCommandHandler::aRefreshing" );
    iRefreshOngoing = aRefreshing;
    }

// ---------------------------------------------------------------------------
// Determines if the command should be handled by standalone mode.
// ---------------------------------------------------------------------------
//
TBool CMPXCommandHandler::ForwardCmdToStandAloneMode()
    {
    MPX_FUNC( "CMPXCommandHandler::ForwardCmdToStandAloneMode" );
    TBool ret( EFalse );
    if ( iParameter )
        {
        ret = ( iParameter->iCmdForward == EMPXCmdFwdStandAlone );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Determines if the command should be handled by now playing instance
// ---------------------------------------------------------------------------
//
TBool CMPXCommandHandler::ForwardCmdToNowPlaying()
    {
    MPX_FUNC( "CMPXCommandHandler::ForwardCmdToNowPlaying" );
    TBool ret( EFalse );
    if ( iParameter )
        {
        ret = ( iParameter->iCmdForward == EMPXCmdFwdNowPlaying );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Checks if initialize playback engine is needed for current command
// ---------------------------------------------------------------------------
//
TBool CMPXCommandHandler::IsInitPlaybackEngineNeeded()
    {
    MPX_FUNC( "CMPXCommandHandler::IsInitPlaybackEngineNeeded" );
    TBool ret( EFalse );
    if ( iParameter )
        {
        ret = ( iParameter->iPathType == EMPXTypeCollectionPlaylist );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Extracts the activated view type from the command
// ---------------------------------------------------------------------------
//
TInt CMPXCommandHandler::ActivatedViewTypeId()
    {
    MPX_FUNC( "CMPXCommandHandler::ActivatedViewTypeId" );
    TInt id( 0 );
    if ( iParameter )
        {
        id = iParameter->iType.iUid;
        }
    return id;
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle media properties.
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXCommandHandler::HandleCollectionMediaL" );
    TBool bringToForeground( EFalse );

    TUid waitnoteId = TUid::Uid( KMPXPluginTypeWaitNoteDialogUid );
    TUid activeView = iViewUtility->ActiveViewType();
    if( activeView == waitnoteId && !iUsbInUse && !iRefreshOngoing )
        {
        CMPXViewPlugin* pi =
            iViewUtility->ViewPluginManager().PluginL( TUid::Uid(KWaitNoteImpUid) );
        pi->DeactivateView();
        }

    if ( aError == KErrNone )
        {
        if( aMedia.IsSupported( KMPXMediaColDetailDBCreated ) )
            {
            TBool dbCreated( aMedia.ValueTObjectL<TBool>( KMPXMediaColDetailDBCreated ) );
            if ( dbCreated )
                {
                MPX_DEBUG1( "CMPXCommandHandler::HandleCollectionMediaL DB status ok" );
                DoHandleMessageL();
                }
            else
                {
                MPX_DEBUG1( "CMPXCommandHandler::HandleCollectionMediaL DB not created" );
                if ( !iUsbInUse && !iRefreshOngoing )
                    {
                    iViewUtility->ActivateViewL( TUid::Uid( KMPXPluginTypeMainUid ) );
                    }
                else
                    {
                    MPX_DEBUG1( "CMPXCommandHandler::HandleCollectionMediaL USB in use/Refreshing, not activating view" );
                    }
                bringToForeground = ETrue;
                }
            }
        }
    else
        {
        MPX_DEBUG1( "CMPXCommandHandler::HandleCollectionMediaL error getting DB status" );
        if ( !iUsbInUse && !iRefreshOngoing )
            {
            iViewUtility->ActivateViewL( TUid::Uid( KMPXPluginTypeMainUid ) );
            }
        else
            {
            MPX_DEBUG1( "CMPXCommandHandler::HandleCollectionMediaL USB in use/Refreshing, not activating view" );
            }
        bringToForeground = ETrue;
        }

    if ( bringToForeground )
        {
        MPX_DEBUG1( "CMPXCommandHandler::HandleCollectionMediaL bringing app to foreground" );
        // have to use window group to activate, in case some other window
        // has higher priority than us (e.g. playback popup and volume popup)
        RWindowGroup myWin = CEikonEnv::Static()->RootWin();
        myWin.SetOrdinalPosition( 0, ECoeWinPriorityNormal );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle collection message.
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::HandleCollectionMessage(
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
void CMPXCommandHandler::HandleOpenL(
    const CMPXMedia& aEntries,
    TInt /* aIndex */,
    TBool /* aComplete */,
    TInt aError  )
    {
    MPX_FUNC( "CMPXCommandHandler::HandleOpenL 4" );
    if ( iCollectionPlaylistState ==
        TMPXCollectionPlaylistSetupStateOpenPath )
        {
        if ( !aError )
            {
            const CMPXMediaArray* mediaArray(
                aEntries.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
            User::LeaveIfNull( const_cast<CMPXMediaArray*>( mediaArray ));
            if ( mediaArray->Count() > 0 )
                {
                CMPXCollectionPath* cPath( iIsolatedIncOpenUtil->PathL() );
                CleanupStack::PushL( cPath );
                if ( cPath->Id( cPath->Levels() - 1 ) != KMPXInvalidItemId )
                    {
                    iIsolatedIncOpenUtil->Stop();
                    InitPlaybackEngineL( *cPath );
                    }
                CleanupStack::PopAndDestroy( cPath );
                }
            else
                {
                // if there's nothing to open, check if the parameter
                // contains a view to open
                DoHandleMessageL();
                }
            }
        else
            {
            iCollectionPlaylistState = TMPXCollectionPlaylistSetupStateIdle;
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the item being opened.
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    MPX_FUNC("CMPXCommandHandler::HandleOpenL 2");
    // Not used
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// Initialize the playback engine with a collection path
// ---------------------------------------------------------------------------
//
void CMPXCommandHandler::InitPlaybackEngineL( CMPXCollectionPath& aPath )
    {
    MPX_FUNC( "CMPXCommandHandler::InitPlaybackEngineL" );
    CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL( aPath );
    CleanupStack::PushL( playlist );
    if ( iCollectionPlaylistState ==
        TMPXCollectionPlaylistSetupStateOpenPath )
        {
        playlist->SetToIndex(KErrNotFound);
        }
    iCollectionPlaylistState =
        TMPXCollectionPlaylistSetupStateInitPlaybackEngine;
    iPlaybackUtility->InitL( *playlist, ETrue );
    iPathInited = ETrue;
    CleanupStack::PopAndDestroy( playlist );
    }

// -----------------------------------------------------------------------------
// DoHandleMessageL
// -----------------------------------------------------------------------------
//
void CMPXCommandHandler::DoHandleMessageL()
    {
    MPX_FUNC( "CMPXCommandHandler::DoHandleMessageL" );

    TBool isNeedSetWindowGroup = ETrue;
    TBool pathOpened( EFalse );
    iUnhandledMessage = EFalse;
    TUid appToActivate( KAppUidMusicPlayerX );
    if ( CEikonEnv::Static()->StartedAsServerApp() )
        {
        // if it's embedded mode
        appToActivate = MPXTlsHelper::HostUid();
        }

    MPX_DEBUG3( "CMPXCommandHandler::DoHandleMessageL iUsbInUse = %d, iRefreshOngoing = %d", iUsbInUse, iRefreshOngoing );
    if ( iParameter && !iUsbInUse && !iRefreshOngoing )
        {
        if ( iParameter->iCollectionPath )
            {
            if ( iParameter->iPathType != EMPXTypeCollectionPlaylist )
                {
                // if it's a collection path, use music player collection
                // utility to open it.  Collection server understands collection path
                // with indexes but not ids.
                MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL iCollectionPath defined, path" );
                iMusicPlayerCollectionUtility->Collection().OpenL(
                    *iParameter->iCollectionPath );
                }
            else
                {
                MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL iCollectionPath defined, playlist" );
                // if it's collection playlist path, need to use isolated
                // collection utility to translate it to real uids
                iCollectionPlaylistState =
                    TMPXCollectionPlaylistSetupStateOpenPath;
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL( attrs );
                TArray<TMPXAttribute> ary = attrs.Array();
                iIsolatedIncOpenUtil->Stop();
                iIsolatedIncOpenUtil->SetDelay( 0 );
                iIsolatedIncOpenUtil->StartL( *iParameter->iCollectionPath, ary, KIncrementalFetchCount,
                                      0, CMPXCollectionOpenUtility::EFetchNormal );
                CleanupStack::PopAndDestroy( &attrs );
                }
            pathOpened = ETrue;
            }
        if ( iParameter->iType != KNullUid )
            {
            if ( pathOpened )
                {
                MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL path opened, view activation deffered" );
                // path opened, need to wait for callback before handling
                // view switching
                iUnhandledMessage = ETrue;
                delete iParameter->iCollectionPath;
                iParameter->iCollectionPath = NULL;
                }
            else
                {
                MPX_DEBUG2( "CMPXCommandHandler::DoHandleMessageL activate view 0x%x", iParameter->iType.iUid );
                if ( iParameter->iType.iUid == KMPXPluginTypePlaybackUid && appToActivate == KAppUidMusicPlayerX )
                    {
                    // push default view history in case for view jumps
                    iViewUtility->PushDefaultHistoryL();
                    // only do this when it's not embedded mode
                    MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL activate playback view special handling" );
                    // special handling needed for playback view activation
                    MMPXPlayer* player =
                        iPlaybackUtility->PlayerManager().CurrentPlayer();
                    RArray<TUid> array;
                    CleanupClosePushL( array );

                    array.AppendL( iParameter->iType );
                    if ( player )
                        {
                        array.AppendL( player->UidL() );
                        }

                    // add the collection plugin UID to ensure the correct
                    // last playback view is activated
                    MMPXSource* source = iPlaybackUtility->Source();
                    if( source )
                        {
                        CMPXCollectionPlaylist* pl = source->PlaylistL();
                        if ( pl )
                            {
                            CleanupStack::PushL( pl );
                            const CMPXCollectionPath& path = pl->Path();
                            if( path.Levels() > 0 )
                                {
                                MPX_DEBUG2( "CMPXCommandHandler::DoHandleMessageL appending UID 0x%x", path.Id( 0 ).iId1 );
                                array.AppendL( TUid::Uid( path.Id( 0 ) ) );
                                }
                            CleanupStack::PopAndDestroy( pl );
                            }
                        }
                    if ( !iUsbInUse && !iRefreshOngoing )
                        {
                        iViewUtility->ActivateViewL( array );
                        }
                    else
                        {
                        MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL USB in use/Refreshing, not activating view" );
                        }
                    CleanupStack::PopAndDestroy( &array );
                    }
                else if ( iParameter->iType.iUid == KMPXPluginTypeLastPlayedUid && appToActivate == KAppUidMusicPlayerX )
                    {
                    // push default view history in case for view jumps
                    iViewUtility->PushDefaultHistoryL();
                    // only do this when it's not embedded mode
                    MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL activate playback view special handling" );
                    // special handling needed for playback view activation
                    MMPXPlayer* player =
                        iPlaybackUtility->PlayerManager().CurrentPlayer();
                    RArray<TUid> array;
                    CleanupClosePushL( array );

                    array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid) );
                    if ( player )
                        {
                        array.AppendL( player->UidL() );
                        }

                    // add the collection plugin UID to ensure the correct
                    // last playback view is activated
                    MMPXSource* source = iPlaybackUtility->Source();
                    if( source )
                        {
                        CMPXCollectionPlaylist* pl = source->PlaylistL();
                        if ( pl )
                            {
                            CleanupStack::PushL( pl );
                            const CMPXCollectionPath& path = pl->Path();
                            if( path.Levels() > 0 )
                                {
                                MPX_DEBUG2( "CMPXCommandHandler::DoHandleMessageL appending UID 0x%x", path.Id( 0 ).iId1 );
                                array.AppendL( TUid::Uid( path.Id( 0 ) ) );
                                }
                            CleanupStack::PopAndDestroy( pl );
                            }
                        }
                    if ( !iUsbInUse && !iRefreshOngoing )
                        {
                        iLastPlayedAutostart = ETrue;
                        TRAP_IGNORE(iPlaybackUtility->CommandL( EPbCmdPlay ));
                        RWindowGroup myWin = CEikonEnv::Static()->RootWin();
                        if ( myWin.OrdinalPosition() == 0 ) //top window
                            {
                            iViewUtility->ActivateViewL( array );
                            }
                        else 
                            {
                            isNeedSetWindowGroup = EFalse;
                        	}
                        }
                    else
                        {
                        MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL USB in use/Refreshing, not activating view" );
                        }
                    CleanupStack::PopAndDestroy( &array );
                    }
                else if ( appToActivate == KAppUidMusicPlayerX )
                    {
                    if ( !iUsbInUse && !iRefreshOngoing )
                        {
                        RArray<TUid> array;
                        CleanupClosePushL( array );
                        array.AppendL( iParameter->iType );
                        if ( iParameter->iType.iUid == KMPXPluginTypeCollectionUid && appToActivate == KAppUidMusicPlayerX )
                            {
                            CMPXCollectionPath* cpath( iMusicPlayerCollectionUtility->Collection().PathL() );
                            CleanupStack::PushL( cpath );
                            if( cpath->Levels() > 0 )
                                {
                                MPX_DEBUG2( "CMPXCommandHandler::DoHandleMessageL appending UID 0x%x", cpath->Id( 0 ).iId1 );
                                array.AppendL( TUid::Uid( cpath->Id( 0 ) ) );
                                }
                            CleanupStack::PopAndDestroy( cpath );
                            }
                        // push default view history in case for view jumps
                        iViewUtility->PushDefaultHistoryL();
                        // only do this if it's not embedded mode
                        iViewUtility->ActivateViewL( array );
                        CleanupStack::PopAndDestroy( &array );
                        }
                    else
                        {
                        MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL USB in use/Refreshing, not activating view" );
                        }
                    }
                else
                    {
                    MPX_DEBUG1( "CMPXCommandHandler::DoHandleMessageL embedded mode, not activating view" );
                    }
                }
            }
        }
    if ( !iUnhandledMessage || iUsbInUse || iRefreshOngoing )
        {
        // after the message is handled, delete it
        delete iParameter;
        iParameter = NULL;
        // have to use window group to activate, in case some other window
        // has higher priority than us (e.g. playback popup and volume popup)
        if ( isNeedSetWindowGroup )
            {
            RWindowGroup myWin = CEikonEnv::Static()->RootWin();
            myWin.SetOrdinalPosition( 0, ECoeWinPriorityNormal );
            }
        }
    }

// -----------------------------------------------------------------------------
// ExtractMessageL
// -----------------------------------------------------------------------------
//
void CMPXCommandHandler::ExtractMessageL( const TDesC8& aMessage )
    {
    MPX_FUNC( "CMPXCommandHandler::ExtractMessageL" );
    ASSERT( aMessage.Length() > 0 );
    delete iParameter;
    iParameter = NULL;
    CBufBase* buffer = CBufFlat::NewL( aMessage.Size() );
    CleanupStack::PushL( buffer );
    buffer->ResizeL( aMessage.Size() );
    buffer->Write( 0, aMessage );
    RBufReadStream readStream( *buffer );
    CleanupClosePushL( readStream );
    iParameter = new ( ELeave ) CMPXParameter();
    iParameter->InternalizeL( readStream );
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( buffer );
    }

// -----------------------------------------------------------------------------
// Handle playback message
// -----------------------------------------------------------------------------
//
void CMPXCommandHandler::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXCommandHandler::DoHandlePlaybackMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EInitializeComplete:
                {
                if ( iCollectionPlaylistState ==
                    TMPXCollectionPlaylistSetupStateInitPlaybackEngine )
                    {
                    iViewUtility->PushDefaultHistoryL();
                    iPlaybackUtility->CommandL( EPbCmdPlay );
                    iCollectionPlaylistState =
                        TMPXCollectionPlaylistSetupStateIdle;
                    delete iParameter;
                    iParameter = NULL;
                    RWindowGroup myWin = CEikonEnv::Static()->RootWin();
                    myWin.SetOrdinalPosition( 0, ECoeWinPriorityNormal );
                    }
                if (iLastPlayedAutostart)
                    {
                    iPlaybackUtility->CommandL( EPbCmdPlay );
                    iLastPlayedAutostart = EFalse;
                    }
                break;
                }
            default:
                {
                // ignore other playback messages
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMPXCommandHandler::DoHandleCollectionMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXCommandHandler::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        MPX_DEBUG3( "CMPXCommandHandler::HandleCollectionMessage Event = %d, Type() = %d",
            event, type );
        if ( iUnhandledMessage )
            {
            DoHandleMessageL();
            }
        else
            {
            // do nothing
            }
        }
    }

// End of File
