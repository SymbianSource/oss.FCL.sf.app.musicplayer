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
* Description:  Implementation of Upnp Playback view
*  Version     : %version: da1mmcf#35.1.3.1.4 % << Don't touch! Updated by Synergy at check-out.
*
*/



// INCLUDE FILES
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <akntitle.h>
#include <akntoolbar.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <aknnotewrappers.h> // CAknErrorNote
#include <ctsydomainpskeys.h> // KTelephonyCallState
#include <e32property.h>  // RProperty
#include <featmgr.h>

#include <mpxplaybackmessage.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <mpxviewpluginmanager.h>
#include <mpxviewplugin.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediamusicdefs.h>
#include <mpxcommonplaybackview.rsg>
#include <mpxupnpplaybackview.rsg>
#include <mpxuser.h>
#include "mpxcommonplaybackview.hrh"
#include "mpxcommonuihelper.h"
#include "mpxupnpplaybackview.hrh"
#include "mpxupnpplaybackviewimp.h"
#include "mpxupnpplaybackviewcontainer.h"
#include "mpxupnpplaybackviewlayout.h"
#include "mpxlog.h"
#include "mpxconstants.h"


// CONSTANTS
_LIT( KMPXUpnpPlaybackRscPath, "mpxupnpplaybackview.rsc" );
const TUid KMPXUpnpNPVImpmentationUid = { 0x101FFC99 };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackViewImp::CMPXUPnPPlaybackViewImp()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::ConstructL" );

    CMPXCommonPlaybackViewImp::ConstructL();

    // Override base class setting to allow
    // upnp playback to properly fetch media properties.
    iLastPBViewActivated = ETrue;

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXUpnpPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile = parse.FullName();
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iUpnpResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    iPodcastPlayback = EFalse;

    BaseConstructL( R_MPX_UPNP_PLAYBACK_VIEW );
	if ( AknLayoutUtils::PenEnabled() )
        {
        CAknView::ShowToolbarOnViewActivation(ETrue);
        if ( Toolbar() )
            {
            iToolbar = Toolbar();
            iToolbar->SetToolbarObserver( this );
            }
        }
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackViewImp* CMPXUPnPPlaybackViewImp::NewL()
    {
    CMPXUPnPPlaybackViewImp* self = CMPXUPnPPlaybackViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackViewImp* CMPXUPnPPlaybackViewImp::NewLC()
    {
    CMPXUPnPPlaybackViewImp* self = new ( ELeave ) CMPXUPnPPlaybackViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackViewImp::~CMPXUPnPPlaybackViewImp()
    {
    MPX_DEBUG1( "CMPXUPnPPlaybackViewImp::~CMPXUPnPPlaybackViewImp entering" );

    delete iPlaybackViewLayout;

    if ( iUpnpResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iUpnpResourceOffset );
        }

    if ( iLoadingWaitNote )
        {
        iLoadingWaitNote->Cancel();
        delete iLoadingWaitNote;
        }
    MPX_DEBUG1( "CMPXUPnPPlaybackViewImp::~CMPXUPnPPlaybackViewImp exiting" );
    }


// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::DoHandlePlaybackMessageL" );

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EError:
                {
                TInt error( data );
                if ( KErrNone != error )
                    {
                    UpdateTitlePaneL();
                    DismissWaitNoteL();
                    }
                break;
                }
            case TMPXPlaybackMessage::ESubPlayersChanged:
            case TMPXPlaybackMessage::EPlayerChanged:
                {
                MPX_DEBUG2( "HandlePlaybackMessageL - ESubPlayersChanged(%d)", type );
                UpdateTitlePaneL();
                break;
                }
            case TMPXPlaybackMessage::EPlayerUnavailable:
                {
                MPX_DEBUG2( "HandlePlaybackMessageL - EPlayerUnavailable(%d)", type );
                TUid activeView = iViewUtility->ActiveViewType();
                if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
                    {
                    if ( data == KErrNotFound )
                        {
                        if ( iSubPlayerName )
                            {
                            HBufC* dialogText = StringLoader::LoadLC(
                                                R_MPX_NOTE_REMOTE_CONNECTION_FAILED,
                                                *iSubPlayerName );
                            CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                            errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                            errNote->ExecuteLD( *dialogText );
                            CleanupStack::PopAndDestroy( dialogText );
                            }
                        }

                    // Remove from view utility's stack so that activating the
                    // previous view will not activate this one.
                    // have to use plugin implementation UID
                    CMPXViewPlugin* p = iViewUtility->ViewPluginManager().PluginL( KMPXUpnpNPVImpmentationUid );
                    p->DeactivateView();
                    }
                break;
                }
            case TMPXPlaybackMessage::EInitializeComplete:
                {
                DismissWaitNoteL();
                break;
                }
            case TMPXPlaybackMessage::EAccessoryChanged:
                {
                MPX_DEBUG2( "HandlePlaybackMessageL - EAccessoryChanged(%d)", type );

                // Get call status and type.
                TInt callState( EPSCTsyCallStateUninitialized );

                // Ignore error
                RProperty::Get( KPSUidCtsyCallInformation,
                                KCTsyCallState,
                                callState );
                MPX_DEBUG2("CMPXUPnPPlaybackViewImp EAccessoryChanged call state: %d", callState );

                if ( callState != EPSCTsyCallStateConnected )
                    {
                    TMPXPlaybackAccessoryMode mode( static_cast<TMPXPlaybackAccessoryMode>( type ));
                    TBool resetToLocal( EFalse );
#ifdef __ACCESSORY_FW
                    resetToLocal = ( EPbAccessoryWiredHeadset == mode ||
                                     EPbAccessoryWirelessHeadset == mode ||
                                     EPbAccessoryMusicStand == mode ||
                                     EPbAccessoryTVOut == mode ||
                                     EPbAccessoryHeadphones == mode );
#else
                    resetToLocal = ( EPbAccessoryHeadset == mode ||
                                     EPbAccessoryLoopset == mode );
#endif
                    if ( resetToLocal )
                        {
                        SelectNewPlayerL( EMPXPbvCmdUpnpPlayViaLocal );
                        GetCurrentPlayerDetails();
                        }
                    }
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
		        MPX_DEBUG1( "HandlePlaybackMessageL - EStateChanged" );
                UpdateMiddleSoftKeyDisplayL();
                UpdateToolbar();
                break;
                }
            default:
                {
                break;
                }
            }
        }

    CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL( aMessage );
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::DoHandleStateChangedL(
    TMPXPlaybackState aState,
    TInt aData )
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::DoHandleStateChangedL" );
    MPX_DEBUG2( "CMPXUPnPPlaybackViewImp::DoHandleStateChangedL %d", aState );

    if ( ( aState == EPbStatePaused ) &&
         ( aData == KErrDisconnected ) )
        {
        TUid activeView = iViewUtility->ActiveViewType();
        if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
            {
            if ( iSubPlayerName )
                {
                HBufC* dialogText = StringLoader::LoadLC(
                                    R_MPX_NOTE_REMOTE_CONNECTION_FAILED,
                                    *iSubPlayerName );
                CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                errNote->ExecuteLD( *dialogText );
                CleanupStack::PopAndDestroy( dialogText );
                }

            // UPnP connection failed, reset playback to Local
            SelectNewPlayerL( EMPXPbvCmdUpnpPlayViaLocal );
            GetCurrentPlayerDetails();
            }
        }
    else
        {
        CMPXCommonPlaybackViewImp::DoHandleStateChangedL( aState, aData );
        }

    switch ( aState )
        {
        case EPbStateStopped:
        case EPbStatePlaying:
        case EPbStatePaused:
            {
            DismissWaitNoteL();
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackViewImp::UpdateTitlePaneL
// Retrieves the current player name and updates the Label
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::UpdateTitlePaneL()
    {
    TUid activeView = iViewUtility->ActiveViewType();
    if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
        {
        CAknTitlePane* title( NULL );

        TRAP_IGNORE(
            {
            title = static_cast<CAknTitlePane*>
                ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
            } );

        GetCurrentPlayerDetails();

        if ( title )
            {
            if ( iCurrentPlayerType != EPbLocal)
                {
                if ( iSubPlayerName )
                    {
                    title->SetTextL( *iSubPlayerName );
                    }
                }
            else
                {
                CMPXCommonPlaybackViewImp::UpdateTitlePaneL();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates track info field.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::UpdateTrackInfoL(
    const CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::UpdateTrackInfo" );

    CMPXCommonPlaybackViewImp::UpdateTrackInfoL( aMedia );

    if ( iPodcastPlayback &&
         iContainer &&
         !iSwitchingView )
        {
        if ( aMedia )
            {
            // Set ETextArtist field as Podcast Title
            if ( aMedia->IsSupported(KMPXMediaMusicAlbum))
                {
                HBufC* titleText = aMedia->ValueText(KMPXMediaMusicAlbum).AllocL();
                CleanupStack::PushL(titleText);
                if ( titleText->Length() > 0)
                    {
                    iContainer->UpdateLabelL( ETextArtist, *titleText );
                    }
                else
                    {
                    HBufC* unknownText =
                        StringLoader::LoadLC( R_MPX_QTN_NMP_UNKNOWN_TITLE );
                    iContainer->UpdateLabelL(
                        ETextArtist, *unknownText );
                    CleanupStack::PopAndDestroy( unknownText );
                    }
                CleanupStack::PopAndDestroy( titleText );
                }
            else
                {
                HBufC* unknownText =
                        StringLoader::LoadLC( R_MPX_QTN_NMP_UNKNOWN_TITLE );
                iContainer->UpdateLabelL(
                    ETextArtist, *unknownText );
                CleanupStack::PopAndDestroy( unknownText );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    // Call base clase to handle most cases
    CMPXCommonPlaybackViewImp::DynInitMenuPaneL( aResourceId, aMenuPane );

    switch ( aResourceId )
        {
        case R_MPX_UPNP_PODCAST_PLAYBACK_VIEW_MENU1:
            {
            if ( !iPodcastPlayback )
                {
                aMenuPane->SetItemDimmed(
                        EMPXPbvCmdGoToPodcastMenu,
                        ETrue );
                }
            break;
            }
        case R_MPX_PLAYBACK_VIEW_MENU:
            {
            aMenuPane->SetItemDimmed(
                    EMPXPbvCmdOpenMusicSettings,
                    ETrue );

            aMenuPane->SetItemDimmed(
                    EMPXPbvCmdEqualizer,
                    ETrue );



			// Check if FM Transmitter is supported
			if ( !FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
				{
				aMenuPane->SetItemDimmed(
						EMPXPbvCmdFMTransmitter,
						ETrue );
				}

            if ( iPodcastPlayback )
                {
                // dim the options that aren't applicable
                // to podcast collection
                aMenuPane->SetItemDimmed(
                        EMPXPbvCmdGotoMusicMenu,
                        ETrue );
                aMenuPane->SetItemDimmed(
                        EMPXPbvCmdAddToPlaylist,
                        ETrue );
            aMenuPane->SetItemDimmed(
                EMPXPbvCmdUseAsCascade,
                ETrue );
                aMenuPane->SetItemDimmed(
                        EMPXPbvCmdRepeat,
                        ETrue );
                aMenuPane->SetItemDimmed(
                        EMPXPbvCmdRandom,
                        ETrue );
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXUPnPPlaybackViewImp::Id() const
    {
    return TUid::Uid( KMPXUPnPPlaybackViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::HandleCommandL( TInt aCommand )
    {
    MPX_DEBUG2( "CMPXUPnPPlaybackViewImp::HandleCommandL(%d)", aCommand );

    switch( aCommand )
        {
        case EMPXPbvCmdGoToPodcastMenu:
            {
            // Handle Go to main podcast menu
            CMPXCollectionPath* cpath = iCollectionUiHelper->PodCastMenuPathL();
            CleanupStack::PushL( cpath );
            MPX_DEBUG_PATH (*cpath);
            iCollectionUtility->Collection().OpenL( *cpath );
            CleanupStack::PopAndDestroy( cpath );

            // Restore status pane
            if ( StatusPane()->CurrentLayoutResId() !=
                 R_AVKON_STATUS_PANE_LAYOUT_USUAL)
                {
                StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
                }
            break;
            }
        default:
            {
            CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::ProcessCommandL( TInt aCommand )
    {
    MPX_DEBUG2("--->CMPXUPnPPlaybackViewImp::ProcessCommandL(%d)", aCommand);
    if ( aCommand == EAknCmdExit )
        {
        if ( MenuBar() )
            {
            MenuBar()->StopDisplayingMenuBar();
            }
        }
    CAknView::ProcessCommandL( aCommand );
    MPX_DEBUG1("<---CMPXUPnPPlaybackViewImp::ProcessCommandL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::DoActivateL()" );

    if ( !iContainer )
        {
        if ( !iPlaybackViewLayout )
            {
            iPlaybackViewLayout = CMPXUPnPPlaybackViewLayout::NewL();
            }

        iPodcastPlayback = EFalse;
        MMPXSource* source( iPlaybackUtility->Source() );
        if ( source )
            {
            CMPXCollectionPlaylist* pl( source->PlaylistL() );
            if ( pl )
                {
                if ( TUid::Uid((TInt)pl->Path().Id(0)) == KMPXUidPodcastDBPlugin )
                    {
                    iPodcastPlayback = ETrue;
                    }
                delete pl;
                }
            }

        iPlaybackViewLayout->SetAlbumArtLayout( iPodcastPlayback );

        iContainer = new ( ELeave ) CMPXUPnPPlaybackViewContainer(
            this,
            this,
            iPlaybackViewLayout );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );

        iUPnPLoadingWaitNoteDisplayed =EFalse;

        if ( iPlaybackUtility->StateL() == EPbStateInitialising )
            {
            if ( !iLoadingWaitNote )
                {
                iLoadingWaitNote = CIdle::NewL(CActive::EPriorityStandard);
                }
            iLoadingWaitNote->Cancel();
            TCallBack callBack( CMPXUPnPPlaybackViewImp::LoadingSong,
                                this );

            // timer is needed because notes are dismissed by avkon during view
            // activation/deactivation process, so we need to do it later
            iLoadingWaitNote->Start(callBack);
            }
        }

    CMPXCommonPlaybackViewImp::DoActivateL( aPrevViewId , aCustomMessageId, aCustomMessage );
    }

// -----------------------------------------------------------------------------
// Displays  when callback from CIdle
// -----------------------------------------------------------------------------
//
TInt CMPXUPnPPlaybackViewImp::LoadingSong( TAny* aPtr )
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::LoadingSong()");

    CMPXUPnPPlaybackViewImp* upnpPlaybackview =
    static_cast<CMPXUPnPPlaybackViewImp*>(aPtr);

    TRAP_IGNORE( upnpPlaybackview->LoadingSongL() );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Displays  when callback from CIdle
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::LoadingSongL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::LoadingSongL()");

    HBufC* string = StringLoader::LoadLC(
        R_MPX_WAIT_UPNP_LOADING_SONG );

    iUPnPLoadingWaitNoteDisplayed = ETrue;

    // display waiting note
    iCommonUiHelper->ShowWaitNoteL(
        *string,
        R_AVKON_SOFTKEYS_CANCEL,
        EFalse, this );
    CleanupStack::PopAndDestroy( string );
    }

// ---------------------------------------------------------------------------
// Dismisses the loading song error note
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::DismissWaitNoteL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::DismissWaitNoteL()" );
    if ( iUPnPLoadingWaitNoteDisplayed )
        {
        if ( iLoadingWaitNote )
            {
            iLoadingWaitNote->Cancel();
            }

        if ( iCommonUiHelper )
            {
            iCommonUiHelper->DismissWaitNoteL();
            }
        iUPnPLoadingWaitNoteDisplayed = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::HandleErrorL( TInt aError )
    {
    MPX_DEBUG2( "--->CMPXUPnPPlaybackViewImp::HandleErrorL(%d)", aError );
    if ( aError )
        {
        DismissWaitNoteL();
        }

    if ( aError == KErrDisconnected )
        {
        MPX_DEBUG1( "CMPXUPnPPlaybackViewImp::HandleErrorL - KErrDisconnected");
        TUid activeView = iViewUtility->ActiveViewType();
        if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
            {
            if ( iSubPlayerName )
                {
                HBufC* dialogText = StringLoader::LoadLC(
                                    R_MPX_NOTE_REMOTE_CONNECTION_FAILED,
                                    *iSubPlayerName );
                CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                errNote->ExecuteLD( *dialogText );
                CleanupStack::PopAndDestroy( dialogText );
                }

            // UPnP connection failed, reset playback to Local
            SelectNewPlayerL( EMPXPbvCmdUpnpPlayViaLocal );
            GetCurrentPlayerDetails();
            }
        }
    else if ( aError == KErrNotSupported )
        {
        MPX_DEBUG1( "CMPXUPnPPlaybackViewImp::HandleErrorL - KErrNotSupported" );
        TUid activeView = iViewUtility->ActiveViewType();
        if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
            {
            HBufC* dialogText = StringLoader::LoadLC( 
                                                R_MPX_INVALID_GROUP_TEXT );

            CAknErrorNote* errNote = new ( ELeave ) CAknErrorNote( ETrue );
            errNote->SetTimeout( CAknNoteDialog::ELongTimeout );
            errNote->ExecuteLD( *dialogText );
            CleanupStack::PopAndDestroy( dialogText );
            }
        CMPXCommonPlaybackViewImp::HandleErrorL( aError );
        }
    else
        {
        CMPXCommonPlaybackViewImp::HandleErrorL( aError );
        }
    MPX_DEBUG1( "<---CMPXUPnPPlaybackViewImp::HandleErrorL");
    }

// ---------------------------------------------------------------------------
// From MProgressDialogCallback
// Callback method. Get's called when a dialog is dismissed
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewImp::DialogDismissedL( TInt aButtonId )
    {
    MPX_DEBUG2("--->CMPXUPnPPlaybackViewImp::DialogDismissedL(%d)", aButtonId);
    switch (aButtonId)
        {
        case EAknSoftkeyCancel:
            {
            if ( iPlaybackUtility->StateL() == EPbStateInitialising &&
                 iUPnPLoadingWaitNoteDisplayed )
                {
                iPlaybackUtility->CommandL(EPbCmdCancelInit);
                HandleCommandL(EAknSoftkeyBack);
                }
            break;
            }
        case EAknSoftkeyDone:
            {
            break;
            }
        default:
            {
            break;
            }
        }
    MPX_DEBUG1("<---CMPXUPnPPlaybackViewImp::DialogDismissedL()");
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Used to change toolbar settings before toolbar becomes visible
// -----------------------------------------------------------------------------
//

void CMPXUPnPPlaybackViewImp::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::DynInitToolbarL" );
    if ( aResourceId == R_MPX_PBV_TOUCH_TOOLBAR )
        {
        // Put initializations here to take effect before toolbar is shown
        UpdateToolbar();
        aToolbar->UpdateBackground();
        }
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Handles toolbar events
// -----------------------------------------------------------------------------
//

void CMPXUPnPPlaybackViewImp::OfferToolbarEventL( TInt aCommandId )
    {
    MPX_FUNC( "CMPXUPnPPlaybackViewImp::OfferToolbarEventL" );
    TInt commandId = aCommandId;
    TInt eventModifiers = iToolbar->EventModifiers();
    if ( eventModifiers == CAknToolbar::ELongPress )
        {
        if ( aCommandId == EMPXPbvCmdNextListItem )
            {
            commandId = EMPXPbvCmdSeekForward;
            }
        else if ( aCommandId == EMPXPbvCmdPreviousListItem )
            {
            commandId = EMPXPbvCmdSeekBackward;
            }
        }
    else if ( eventModifiers == CAknToolbar::ELongPressEnded )
        {
        if ( aCommandId == EMPXPbvCmdNextListItem || aCommandId == EMPXPbvCmdPreviousListItem )
            {
            commandId = EMPXPbvCmdEndSeek;
            }
        }

    CAknView::ProcessCommandL( commandId );
    }
//  End of File
