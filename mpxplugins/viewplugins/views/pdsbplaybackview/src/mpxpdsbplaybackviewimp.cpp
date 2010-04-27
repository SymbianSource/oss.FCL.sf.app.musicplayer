/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Progressive Download Playback view
*
*/



// INCLUDE FILES
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <bautils.h>
#include <featmgr.h>
#include <data_caging_path_literals.hrh>
#include <mpxplaybackmessage.h>
#include <mpxcommonplaybackview.rsg>
#include <mpxpdsbplaybackview.rsg>
#include <mpxcmn.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <MProfileEngine.h>
#include <e32property.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <apgwgnam.h>
#include <mpxuser.h>
#include <sacls.h>

#include "mpxcommonuihelper.h"
#include "mpxcommonplaybackview.hrh"
#include "mpxpdsbplaybackview.hrh"
#include "mpxpdsbplaybackviewimp.h"
#include "mpxpdsbplaybackviewcontainer.h"
#include "mpxpdsbplaybackviewlayout.h"
#include "mpxlog.h"
#include "mpxappui.hrh"

#include <akntoolbar.h>
#include <aknbutton.h>

// CONSTANTS
_LIT( KMPXPdSbPlaybackRscPath, "mpxpdsbplaybackview.rsc" );
#define KMPlayerBrowserUid    0x10008D39
#define KMPlayerBrowserUid2   0x1020724D
#define KProgressDownloadUid  0x10207BCD

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewImp::CMPXPdSbPlaybackViewImp()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXPdSbPlaybackViewImp::ConstructL" );

    CMPXCommonPlaybackViewImp::ConstructL();
    iPlaybackUtility->RemoveObserverL( *this );
    iPlaybackUtility->Close();
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid(KProgressDownloadUid) );
    iPlaybackUtility->AddObserverL( *this );

    // Need to set a higher priority because download manager sets it's
    // own priority high, so we need to make sure we're at least the same
    iPlaybackUtility->SetPriority( CActive::EPriorityStandard + 2 );

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXPdSbPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile = parse.FullName();
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iPdResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    BaseConstructL( R_MPX_PD_SB_PLAYBACK_VIEW );
    if(AknLayoutUtils::PenEnabled())
        {
        CAknView::ShowToolbarOnViewActivation(ETrue);
        if ( Toolbar() )
            {
            iToolbar = Toolbar();
            iToolbar->SetToolbarObserver( this );
            }
        }
    iDownloadState = EPbDlStateDownloading;     // Otherwise we will show "Download Stopped"
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewImp* CMPXPdSbPlaybackViewImp::NewL()
    {
    CMPXPdSbPlaybackViewImp* self = CMPXPdSbPlaybackViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewImp* CMPXPdSbPlaybackViewImp::NewLC()
    {
    CMPXPdSbPlaybackViewImp* self = new ( ELeave ) CMPXPdSbPlaybackViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewImp::~CMPXPdSbPlaybackViewImp()
    {
    MPX_DEBUG1( "CMPXPdSbPlaybackViewImp::~CMPXPdSbPlaybackViewImp entering" );

    delete iPlaybackViewLayout;

    if ( iPdResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iPdResourceOffset );
        }

    MPX_DEBUG1( "CMPXPdSbPlaybackViewImp::~CMPXPdSbPlaybackViewImp exiting" );
    }


// ---------------------------------------------------------------------------
// Gets download information
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::GetDownloadInformationL(
        TUint& aTransactionId,
        TMPXPlaybackPdDownloadState& aState,
        TInt& aDownloadedBytes,
        TInt& aTotalBytes )
    {
    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::GetDownloadInformationL() entering");

    // Create the command to send to MPX
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType, EPbCmdGetPdStatus );
    iPlaybackUtility->CommandL( *cmd );

    // Check that return values were provided
    ASSERT( cmd->IsSupported( KMPXCommandPlaybackPDTransactionID ) &&
            cmd->IsSupported( KMPXCommandPlaybackPDState ) &&
            cmd->IsSupported( KMPXCommandPlaybackPDDownloadedBytes ) &&
            cmd->IsSupported( KMPXCommandPlaybackPDTotalBytes ));

    // Copy the return values
    aTransactionId = cmd->ValueTObjectL<TUint>( KMPXCommandPlaybackPDTransactionID );
    aState = cmd->ValueTObjectL<TMPXPlaybackPdDownloadState>( KMPXCommandPlaybackPDState );
    aDownloadedBytes = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDDownloadedBytes );
    aTotalBytes = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDTotalBytes );
    CleanupStack::PopAndDestroy( cmd );

    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::GetDownloadInformationL() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXPdSbPlaybackViewImp::CheckSignalAvailableL
// This checks for Network available, and Network Strength > none.
// - Return ETrue if network available and network strength
// is Low or Medium or High, otherwise return EFalse.
// -----------------------------------------------------------------------------
//
TBool CMPXPdSbPlaybackViewImp::CheckSignalAvailableL( TUid& aCheck, TInt& aState )
    {
    TBool ret( ETrue );

    // Check the network is available.
    aCheck = KUidNetworkStatus;
    TInt err( RProperty::Get( KUidSystemCategory, KUidNetworkStatusValue, aState ));
    if ( err != KErrNone )
        {
        aState = ESANetworkUnAvailable;
        }
    if( aState != ESANetworkAvailable )
        {
        ret = EFalse;
        }

    // Check network coverage.
    aCheck = KUidNetworkStrength;
    err = RProperty::Get( KUidSystemCategory, KUidNetworkStrengthValue, aState );
    if ( err != KErrNone )
        {
        aState = ESANetworkStrengthUnknown;
        }
    if( aState != ESANetworkStrengthLow &&
        aState != ESANetworkStrengthMedium &&
        aState != ESANetworkStrengthHigh )
        {
        ret = EFalse;
        }

    //passed all tests, OK to send
    return ret;
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "-->CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL" );

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        MPX_DEBUG2( "CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL type = %d", type );
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EDownloadPositionChanged:
                {
                if ( iContainer )
                    {
                    MPX_DEBUG2( "CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL - EDownloadPositionChanged(%d)", data );
                    CMPXPdSbPlaybackViewContainer* container =
                        static_cast<CMPXPdSbPlaybackViewContainer*>(iContainer);
                    container->HandleDownloadPositionChanged( data, iTotalBytes );
                    }
                break;
                }
            case TMPXPlaybackMessage::EDownloadStateChanged:
                {
                TInt data( *aMessage.Value<TInt>( KMPXMessageGeneralData ) );
                MPX_DEBUG2( "CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL - EDownloadStateChanged(%d)", data );
                iDownloadState = static_cast<TMPXPlaybackPdDownloadState>( data );
                if ( iDownloadState == EPbDlStateDownloading ||
                     iDownloadState == EPbDlStateDownloadCompleted )
                    {
                    RequestMediaL();
                    }
                if( iContainer && iDownloadState == EPbDlStateDownloadCompleted )
                    {
                    CMPXPdSbPlaybackViewContainer* container =
                        static_cast<CMPXPdSbPlaybackViewContainer*>(iContainer);
                    container->HandleDownloadPositionChanged( iTotalBytes, iTotalBytes );
                    }
                UpdateViewL();
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL - EStateChanged(%d)", type );

                UpdateToolbarL();
                UpdateMiddleSoftKeyDisplayL();
                TInt data( *aMessage.Value<TInt>( KMPXMessageGeneralData ) );
                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );
                DoHandleStateChangedL( state, data );
                if ( iDownloadState == EPbDlStateDownloadPaused && state == EPbStatePaused )
                    {
                    iCommonUiHelper->DisplayInfoNoteL( R_MPX_DOWNLOAD_PLAYED_ALL_CONTENT );
                    }
                break;
                }
            case TMPXPlaybackMessage::EDownloadStarted:
                {
                iTotalBytes = aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData );
                MPX_DEBUG2( "CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL - DownloadStarted, totalBytes = %d", iTotalBytes );
                break;
                }

            case TMPXPlaybackMessage::EDownloadUpdated:
                {
                iTotalBytes = aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData );
                MPX_DEBUG2( "CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL - DownloadUpdated, totalBytes = %d", iTotalBytes );
                break;
                }


            case TMPXPlaybackMessage::EMediaChanged:

                iPlaybackCompleted = ETrue;
                if ( !iForeground )
                    {
                    //TODO: Add something like AppUi()->HandleCommandL( EMPXCmdGoToNowPlaying ); but it should not bring to foreground
                    }
                break;
            case TMPXPlaybackMessage::EDownloadCmdCancelDownload:
                // stop playback
                // bring back previous now playing view
                AppUi()->HandleCommandL( EMPXCmdGoToNowPlaying );
                // Bring Browser to foreground.
                BringUpBrowserL();
                break;
            case TMPXPlaybackMessage::EDownloadFileMoved:
                {
                // set updated Uri from aMessage for iMedia
                iMedia->SetTextValueL( KMPXMediaGeneralUri, aMessage.ValueText( KMPXMediaGeneralUri ) );
                break;
                }

            default:
                {
                CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL( aMessage );
                break;
                }
            }
        }
    MPX_DEBUG1("<--CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL()");
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::DoHandleStateChangedL(
    TMPXPlaybackState aState,
    TInt aData )
    {
    MPX_DEBUG2( "CMPXPdSbPlaybackViewImp::DoHandleStateChangedL(%d)", aState );

    switch ( aState )
        {
        case EPbStateInitialising:
            {
            UpdatePlaybackStateL( aState );
            UpdateDownloadStateLabelL();
            RequestMediaL();
            break;
            }
        case EPbStatePlaying:
            RequestMediaL();
            CMPXCommonPlaybackViewImp::DoHandleStateChangedL( aState, aData );
            break;
        case EPbStateStopped:
        case EPbStatePaused:
            if ( iExiting )
                {
                AppUi()->RunAppShutter();
                }
        default:
            {
            CMPXCommonPlaybackViewImp::DoHandleStateChangedL( aState, aData );
            break;
            }
        }
    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::DoHandleStateChangedL()");
    }

// ---------------------------------------------------------------------------
// Updates download state label.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL()
    {
    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL() entering");

    if ( iContainer && !iSwitchingView )
        {
        TInt resId = KErrNotFound;
        switch ( iDownloadState )
            {
            case EPbDlStateBuffering:
                {
                MPX_DEBUG1("CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL() - Buffering");
                resId = R_MPX_DOWNLOAD_STATUS_BUFFERING;
                break;
                }
            case EPbDlStateDownloadPaused:
                {
                // If offline mode, this case will be entered.
                resId = R_MPX_DOWNLOAD_STATUS_PAUSED;
                TBool offLine( EFalse );

                // Check profile, display "Download Stopped" if
                // Offline enabled.
                MProfileEngine* profileEngine( CreateProfileEngineL() );

                if ( profileEngine->ActiveProfileId() == EProfileOffLineId )
                    {
                    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL() -- OffLine ");
                    offLine = ETrue;
                    resId = R_MPX_DOWNLOAD_STATUS_INTERRUPTED;
                    }
                profileEngine->Release();

                if ( !offLine )
                    {
                    // Check network coverage, display "Download Stopped" if
                    // No coverage.
                    TUid uid;
                    TInt res( 0 );
                    TBool signalAvailable( CheckSignalAvailableL( uid, res ));

                    if ( !signalAvailable )
                        {
                        MPX_DEBUG1("CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL() -- No Signal ");
                        resId = R_MPX_DOWNLOAD_STATUS_INTERRUPTED;
                        }
                    }

                break;
                }
            case EPbDlStateDownloadError:
            case EPbDlStateDownloadCanceled:
            case EPbDlStateNotDownloading:
                {
                MPX_DEBUG1("CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL() - Download Canceled");
                resId = R_MPX_DOWNLOAD_STATUS_INTERRUPTED;
                break;
                }
            case EPbDlStateDownloadCompleted:
                {
                MPX_DEBUG1("CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL() - Download Completed");
                resId = R_MPX_DOWNLOAD_STATUS_COMPLETED;
                break;
                }
            case EPbDlStateDownloading:
            default:
                {
                MPX_DEBUG2(" default state: %d", iDownloadState );
                // Do nothing
                break;
                }
            }
/*
        // Display Album art in progressive download
        if( iActualAlbumArtBitmap )
            {
            MPX_DEBUG1("CMPlayerPlaybackContainer::UpdateDownloadStateLabelL() - Draw ActualAlbumArt");
            iAlbumArtBitmap = iActualAlbumArtBitmap;
            DrawNow();
            }
        else
            {
            MPX_DEBUG1("CMPlayerPlaybackContainer::UpdateDownloadStateLabelL() - Draw DefaultAlbumArt");
            // Display default Album Art
            iAlbumArtBitmap = iDefaultAlbumArtBitmap;
            DrawNow();
            }
*/

        if (resId != KErrNotFound)
            {
            // Display download status text
            HBufC* statusText = StringLoader::LoadLC( resId );
            iContainer->UpdateLabelL( ETextDownloadState, *statusText );
            CleanupStack::PopAndDestroy( statusText );
            }
        else
            {
            // Clear download status text
            iContainer->UpdateLabelL( ETextDownloadState, KNullDesC );
            }
        }

    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::UpdateDownloadStateLabelL() exiting");
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::HandleErrorL( TInt aError )
    {
    MPX_DEBUG2("CMPXPdSbPlaybackViewImp::HandleErrorL(%d): Entering", aError);
    if ( aError )
        {
        TUint transactionId(0);
        TMPXPlaybackPdDownloadState state( EPbDlStateNotDownloading );
        TInt downloadedBytes(0);
        TInt totalBytes(0);
        GetDownloadInformationL( transactionId,
                                 state,
                                 downloadedBytes,
                                 totalBytes );
        if ( EPbDlStateBuffering == state ||
             EPbDlStateDownloading == state ||
             EPbDlStateDownloadPaused == state )
            {
            CMPXCommand* cmd( CMPXCommand::NewL() );
            CleanupStack::PushL( cmd );
            cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
            cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
            cmd->SetTObjectValueL<TMPXPlaybackPdCommand>(
                KMPXCommandPlaybackGeneralType,
                EPbCmdCancelPd );
            iPlaybackUtility->CommandL( *cmd );
            CleanupStack::PopAndDestroy( cmd );
            }
        iCommonUiHelper->HandleErrorL( aError );
        CMPXCommonPlaybackViewImp::HandleCommandL( EAknSoftkeyBack );
        }
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_DEBUG2("CMPXPdSbPlaybackViewImp::DynInitMenuPaneL(%d): entering", aResourceId);

    // Override base menu actions
    switch ( aResourceId )
        {
        case R_MPX_PD_SB_PLAYBACK_VIEW_MENU:
            {
            aMenuPane->SetItemDimmed(
                EMPXPbvCmdResumeDownload,
                iDownloadState != EPbDlStateDownloadPaused);
            aMenuPane->SetItemDimmed(
                EMPXPbvCmdCancelDownload,
                iDownloadState != EPbDlStateDownloading &&
                iDownloadState != EPbDlStateBuffering &&
                iDownloadState != EPbDlStateDownloadPaused);

			// Check if FM Transmitter is supported
			if ( !FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
				{
				aMenuPane->SetItemDimmed(
					EMPXPbvCmdFMTransmitter,
					ETrue );
				}

            if ( iDownloadState == EPbDlStateDownloading ||
                 iDownloadState == EPbDlStateBuffering ||
                 iDownloadState == EPbDlStateDownloadPaused ||
                 iPlaybackState == EPbStateInitialising ||
                 iPlaybackState == EPbStatePlaying ||
                 iPlaybackState == EPbStatePaused  )
                {
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdUseAsCascade, ETrue );
                }
			else
                {
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdUseAsCascade, EFalse );
                }
			break;
            }
        default:
            {
            // Call base clase to handle most cases
            CMPXCommonPlaybackViewImp::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }

    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::DynInitMenuPaneL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::HandleCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXPdSbPlaybackViewImp::HandleCommandL(%d) entering", aCommand);

    switch (aCommand)
        {
        case EMPXPbvCmdResumeDownload:
            {
            if ( EPbDlStateDownloadPaused == iDownloadState )
                {
                CMPXCommand* cmd( CMPXCommand::NewL() );
                CleanupStack::PushL( cmd );
                cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
                cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
                cmd->SetTObjectValueL<TMPXPlaybackPdCommand>(
                    KMPXCommandPlaybackGeneralType,
                    EPbCmdResumePd );
                iPlaybackUtility->CommandL( *cmd );
                CleanupStack::PopAndDestroy( cmd );
                }
            break;
            }
        case EMPXPbvCmdCancelDownload:
            {
            if ( EPbDlStateDownloading == iDownloadState ||
                 EPbDlStateBuffering == iDownloadState ||
                 EPbDlStateDownloadPaused == iDownloadState )
                {
                HBufC* queryTxt = StringLoader::LoadLC( R_MPX_CONF_CANCEL_DOWNLOAD_TXT );
                CAknQueryDialog* query = CAknQueryDialog::NewL(
                                            CAknQueryDialog::EConfirmationTone );

/* TODO: add when we support cover UI
#ifdef __COVER_DISPLAY      //VCPCCMOD_BEGIN
                query->PublishDialogL(
                        EMPXNoteMusicshopCancel,
                        KMPXNoteCategory);
#endif  //__COVER_DISPLAY   //VCPCCMOD_END
*/

                if ( query->ExecuteLD(
                        R_MPX_GENERIC_CONFIRMATION_QUERY,
                        *queryTxt ))
                    {
                    iPlaybackUtility->CommandL( EPbCmdStop );
                    CMPXCommand* cmd( CMPXCommand::NewL() );
                    CleanupStack::PushL( cmd );
                    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
                    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
                    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>(
                        KMPXCommandPlaybackGeneralType,
                        EPbCmdCancelPd );
                    iPlaybackUtility->CommandL( *cmd );
                    CleanupStack::PopAndDestroy( cmd );
                    iPlaybackUtility->CommandL( EPbCmdClose );
                    }
                CleanupStack::PopAndDestroy( queryTxt );
                }
            break;
            }
        case EAknSoftkeyBack:
            {
            if ( iPlaybackCompleted )
                {
                AppUi()->HandleCommandL( EMPXCmdGoToNowPlaying );
                }
            // Goto Browser, Bring Browser to foreground.
            if ( !BringUpBrowserL() )
                {
                //We are in Music Store, launched from Download Mgr. Instead, stop playback
                iPlaybackUtility->CommandL( EPbCmdStop );
                iPlaybackUtility->CommandL( EPbCmdClose );
                iExiting = ETrue;
                }
            }
            break;
        case EMPXPbvCmdExitPDPlaybackView:
            if ( iPlaybackState == EPbStatePlaying )
                {
                iPlaybackUtility->CommandL( EPbCmdStop );
                iExiting = ETrue;
                }
            else
                {
                AppUi()->RunAppShutter();
                }
            break;
        case EMPXPbvCmdGotoMusicMenu:
            AppUi()->HandleCommandL( EMPXCmdGotoCollection );

        default:
            {
            CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
            break;
            }
        }

    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::HandleCommandL() exiting");
    }

// ---------------------------------------------------------------------------
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXPdSbPlaybackViewImp::DoHandlePropertyL" );
    MPX_DEBUG4( "HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    if ( KErrNone == aError )
        {
        switch ( aProperty  )
            {
            case EPbPropertyRandomMode:
            case EPbPropertyRepeatMode:
            // ignore these cases in PD
                break;
            default:
                {
                CMPXCommonPlaybackViewImp::DoHandlePropertyL(aProperty,aValue,aError);
                break;
                }
            }
        }
    else
        {
        HandleErrorL( aError );
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXPdSbPlaybackViewImp::Id() const
    {
    return TUid::Uid( KMPXPdSbPlaybackViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::ProcessCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXPdSbPlaybackViewImp::ProcessCommandL(%d): entering", aCommand);
    if ( aCommand == EAknCmdExit )
        {
        if ( MenuBar() )
            {
            MenuBar()->StopDisplayingMenuBar();
            }
        }
    CAknView::ProcessCommandL( aCommand );
    MPX_DEBUG1("CMPXPdSbPlaybackViewImp::ProcessCommandL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    MPX_FUNC( "CMPXPdSbPlaybackViewImp::DoActivateL()" );

    if ( !iContainer )
        {
        if ( !iPlaybackViewLayout )
            {
            iPlaybackViewLayout = CMPXPdSbPlaybackViewLayout::NewL();
            }
            iContainer = new ( ELeave ) CMPXPdSbPlaybackViewContainer(
                                        this,
                                        this,
                                        iPlaybackViewLayout );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }

    CMPXCommonPlaybackViewImp::DoActivateL( aPrevViewId , aCustomMessageId, aCustomMessage );
    UpdateMiddleSoftKeyDisplayL();
    }


// ---------------------------------------------------------
// Bring Browser application to the foreground.
// ---------------------------------------------------------
//
TBool CMPXPdSbPlaybackViewImp::BringUpBrowserL()
    {
    RWsSession& wsSession=iEikonEnv->WsSession();
    TInt count=wsSession.NumWindowGroups( 0 );

    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL(allWgIds);
    User::LeaveIfError(wsSession.WindowGroupList(&allWgIds));

    count = allWgIds.Count();

    CApaWindowGroupName* windowName;

    TBool browserFound = EFalse;
    for ( TInt index=0; index < count; index++ )
        {
        RWsSession::TWindowGroupChainInfo& info = allWgIds[index];

        if ( info.iParentId <= 0 )
            {
            TInt wgId=info.iId;
            windowName = CApaWindowGroupName::NewLC(wsSession, wgId);
            TUid applicationUid = windowName->AppUid();
            if ( applicationUid == TUid::Uid(KMPlayerBrowserUid))
                {
                TApaTask task( iEikonEnv->WsSession() );
                task.SetWgId( wgId );
                task.BringToForeground();
                browserFound = ETrue;
                }
            CleanupStack::PopAndDestroy();  //windowName
            }
        }

    CleanupStack::PopAndDestroy( &allWgIds );

    return browserFound;
    }


// ---------------------------------------------------------------------------
// From CAknView
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::HandleForegroundEventL( TBool aForeground )
    {
    MPX_DEBUG2( "--->CMPXPdSbPlaybackViewImp::HandleForegroundEventL(%d)", aForeground );

    iForeground = aForeground;
    CMPXCommonPlaybackViewImp::HandleForegroundEventL(aForeground);
    MPX_DEBUG1( "<---CMPXPdSbPlaybackViewImp::HandleForegroundEventL()" );
    }


// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Used to change toolbar settings before toolbar becomes visible
// -----------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
    {
    if(aResourceId == R_MPX_PBV_TOUCH_TOOLBAR)
        {
        // Put initializations here to take effect before toolbar is shown
        aToolbar->SetItemDimmed( EMPXPbvCmdPreviousListItem, ETrue, EFalse );
        aToolbar->SetItemDimmed( EMPXPbvCmdNextListItem, ETrue, EFalse );
        UpdateToolbarL();
        aToolbar->UpdateBackground();
        }
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Handles toolbar events
// -----------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::OfferToolbarEventL( TInt aCommandId )
    {
    CAknView::ProcessCommandL( aCommandId );
    }

// ---------------------------------------------------------------------------
// Updates the middle toolbar button
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewImp::UpdateToolbarL()
    {
    MPX_FUNC( "CMPXPdSbPlaybackViewImp::UpdateToolbar" );

    if ( AknLayoutUtils::PenEnabled() && Toolbar())
        {
        CAknToolbar* toolbar = Toolbar();
        TBool pausePlayDimmed(ETrue);
        MMPXSource* s = iPlaybackUtility->Source();
        if ( s )
            {
            TMPXPlaybackState state = iPlaybackUtility->StateL();
            CAknButton* pausePlayControl;
            pausePlayControl = (CAknButton*)(toolbar->ComponentControl( 1 ));
            if (pausePlayControl)
                {
                if( (state == EPbStateInitialising) || (state == EPbStatePlaying) )
                    {
                    pausePlayControl->SetCurrentState(0, ETrue);
                    }
               else
                   {
                   pausePlayControl->SetCurrentState(1, ETrue);
                   }
                }
            if ( state == EPbStatePlaying ||
                    state == EPbStatePaused ||
                    state == EPbStateStopped )
                {
                pausePlayDimmed = EFalse;
                }
            }
        toolbar->SetItemDimmed( EMPXPbvCmdPlayPause, pausePlayDimmed, ETrue );
        }
    }
//  End of File
