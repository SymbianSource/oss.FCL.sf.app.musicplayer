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
#include <mpxpdplaybackview.rsg>
#include <mpxcmn.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <MProfileEngine.h>
#include <e32property.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxuser.h>
#include <sacls.h>

#include "mpxcommonuihelper.h"
#include "mpxcommonplaybackview.hrh"
#include "mpxpdplaybackview.hrh"
#include "mpxpdplaybackviewimp.h"
#include "mpxpdplaybackviewcontainer.h"
#include "mpxpdplaybackviewlayout.h"
#include "mpxlog.h"

#include <mpxplaybackutility.h>
#include <akntoolbar.h>
#include <aknbutton.h>
// CONSTANTS
_LIT( KMPXPdPlaybackRscPath, "mpxpdplaybackview.rsc" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewImp::CMPXPdPlaybackViewImp()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXPdPlaybackViewImp::ConstructL" );

    CMPXCommonPlaybackViewImp::ConstructL();

    // Need to set a higher priority because download manager sets it's
    // own priority high, so we need to make sure we're at least the same
    iPlaybackUtility->SetPriority( CActive::EPriorityStandard + 2 );

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXPdPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile = parse.FullName();
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iPdResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    BaseConstructL( R_MPX_PD_PLAYBACK_VIEW );
    if(AknLayoutUtils::PenEnabled())
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
CMPXPdPlaybackViewImp* CMPXPdPlaybackViewImp::NewL()
    {
    CMPXPdPlaybackViewImp* self = CMPXPdPlaybackViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewImp* CMPXPdPlaybackViewImp::NewLC()
    {
    CMPXPdPlaybackViewImp* self = new ( ELeave ) CMPXPdPlaybackViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewImp::~CMPXPdPlaybackViewImp()
    {
    MPX_DEBUG1( "CMPXPdPlaybackViewImp::~CMPXPdPlaybackViewImp entering" );

    delete iPlaybackViewLayout;

    if ( iPdResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iPdResourceOffset );
        }

    MPX_DEBUG1( "CMPXPdPlaybackViewImp::~CMPXPdPlaybackViewImp exiting" );
    }


// ---------------------------------------------------------------------------
// Gets download information
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::GetDownloadInformationL(
        TUint& aTransactionId,
        TMPXPlaybackPdDownloadState& aState,
        TInt& aDownloadedBytes,
        TInt& aTotalBytes )
    {
    MPX_DEBUG1("CMPXPdPlaybackViewImp::GetDownloadInformationL() entering");

    // Create the command to send to MPX
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType, EPbCmdGetPdStatus );
    iPlaybackUtility->CommandL( *cmd );

    // Check that return values were provided, if not, playback plugin is messed up.
    if ( !cmd->IsSupported( KMPXCommandPlaybackPDTransactionID ) ||
            !cmd->IsSupported( KMPXCommandPlaybackPDState ) ||
            !cmd->IsSupported( KMPXCommandPlaybackPDDownloadedBytes ) ||
            !cmd->IsSupported( KMPXCommandPlaybackPDTotalBytes ) )
        {
        aState = EPbDlStateDownloadError;
        CleanupStack::PopAndDestroy( cmd );
        MPX_DEBUG1("CMPXPdPlaybackViewImp::GetDownloadInformationL() Error in Command info");
        return;
        }

    // Copy the return values
    aTransactionId = cmd->ValueTObjectL<TUint>( KMPXCommandPlaybackPDTransactionID );
    aState = cmd->ValueTObjectL<TMPXPlaybackPdDownloadState>( KMPXCommandPlaybackPDState );
    aDownloadedBytes = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDDownloadedBytes );
    aTotalBytes = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDTotalBytes );
    CleanupStack::PopAndDestroy( cmd );

    MPX_DEBUG1("CMPXPdPlaybackViewImp::GetDownloadInformationL() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXPdPlaybackViewImp::CheckSignalAvailableL
// This checks for Network available, and Network Strength > none.
// - Return ETrue if network available and network strength
// is Low or Medium or High, otherwise return EFalse.
// -----------------------------------------------------------------------------
//
TBool CMPXPdPlaybackViewImp::CheckSignalAvailableL( TUid& aCheck, TInt& aState )
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
void CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "-->CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL" );

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
                    CMPXPdPlaybackViewContainer* container =
                        static_cast<CMPXPdPlaybackViewContainer*>(iContainer);
                    container->HandleDownloadPositionChanged( data, iTotalBytes );
                    }
                break;
                }
            case TMPXPlaybackMessage::EDownloadStateChanged:
                {
                MPX_DEBUG2( "CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL - EDownloadStateChanged(%d)", data );
                iDownloadState = static_cast<TMPXPlaybackPdDownloadState>( data );
                if ( iDownloadState == EPbDlStateDownloading ||
                     iDownloadState == EPbDlStateDownloadCompleted )
                    {
                    RequestMediaL();
                    }
                if( iContainer && iDownloadState == EPbDlStateDownloadCompleted )
                    {
                    CMPXPdPlaybackViewContainer* container =
                        static_cast<CMPXPdPlaybackViewContainer*>(iContainer);
                    container->HandleDownloadPositionChanged( iTotalBytes, iTotalBytes );
                    }
                UpdateViewL();
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL - EStateChanged(%d)", type );

                UpdateMiddleSoftKeyDisplayL();
                UpdateToolbar();
                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );
                DoHandleStateChangedL( state, data );
                break;
                }
            case TMPXPlaybackMessage::EDownloadStarted:
                {
                iTotalBytes = aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData );
                MPX_DEBUG2( "CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL - DownloadStarted, totalBytes = %d", iTotalBytes );
                break;
                }
            default:
                {
                CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL( aMessage );
                break;
                }
            }
        }
    MPX_DEBUG1("<--CMPXPdPlaybackViewImp::DoHandlePlaybackMessageL()");
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::DoHandleStateChangedL(
    TMPXPlaybackState aState,
    TInt aData )
    {
    MPX_DEBUG2( "CMPXPdPlaybackViewImp::DoHandleStateChangedL(%d)", aState );

    switch ( aState )
        {
        case EPbStateInitialising:
            {
            UpdatePlaybackState( aState );
            UpdateDownloadStateLabelL();
            RequestMediaL();
            break;
            }
        case EPbStatePaused:
            {
            //Show the note only when all content played (disconnected and buffer empty), not on user Pause.
            if ( iContainer && KErrUnderflow == aData && EPbDlStateDownloadPaused == iDownloadState )
                {
                iCommonUiHelper->DisplayInfoNoteL( R_MPX_DOWNLOAD_PLAYED_ALL_CONTENT );
                }
            // fall through on purpose
            }
        default:
            {
            CMPXCommonPlaybackViewImp::DoHandleStateChangedL( aState, aData );
            break;
            }
        }
    MPX_DEBUG1("CMPXPdPlaybackViewImp::DoHandleStateChangedL()");
    }

// ---------------------------------------------------------------------------
// Updates download state label.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::UpdateDownloadStateLabelL()
    {
    MPX_DEBUG1("CMPXPdPlaybackViewImp::UpdateDownloadStateLabelL() entering");

    if ( iContainer && !iSwitchingView )
        {
        TInt resId = KErrNotFound;
        switch ( iDownloadState )
            {
            case EPbDlStateBuffering:
                {
                MPX_DEBUG1("CMPlayerPlaybackContainer::UpdateDownloadStateLabelL() - Buffering");
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
                    MPX_DEBUG1("CMPXPdPlaybackViewImp::UpdateDownloadStateLabelL() -- OffLine ");
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
                        MPX_DEBUG1("CMPXPdPlaybackViewImp::UpdateDownloadStateLabelL() -- No Signal ");
                        resId = R_MPX_DOWNLOAD_STATUS_INTERRUPTED;
                        }
                    }
                break;
                }
            case EPbDlStateDownloadError:
            case EPbDlStateDownloadCanceled:
            case EPbDlStateNotDownloading:
                {
                MPX_DEBUG1("CMPlayerPlaybackContainer::UpdateDownloadStateLabelL() - Download Canceled");
                resId = R_MPX_DOWNLOAD_STATUS_INTERRUPTED;
                break;
                }
            case EPbDlStateDownloadCompleted:
                {
                MPX_DEBUG1("CMPlayerPlaybackContainer::UpdateDownloadStateLabelL() - Download Completed");
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

    MPX_DEBUG1("CMPXPdPlaybackViewImp::UpdateDownloadStateLabelL() exiting");
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::HandleErrorL( TInt aError )
    {
    MPX_DEBUG2("CMPXPdPlaybackViewImp::HandleErrorL(%d): Entering", aError);
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
void CMPXPdPlaybackViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_DEBUG2("CMPXPdPlaybackViewImp::DynInitMenuPaneL(%d): entering", aResourceId);

    // Override base menu actions
    switch ( aResourceId )
        {
        case R_MPX_PD_PLAYBACK_VIEW_MENU:
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
            break;
            }
        default:
            {
            // Call base clase to handle most cases
            CMPXCommonPlaybackViewImp::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }

    MPX_DEBUG1("CMPXPdPlaybackViewImp::DynInitMenuPaneL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::HandleCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXPdPlaybackViewImp::HandleCommandL(%d) entering", aCommand);

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
                query->PublishDialogL(
                        EMPXNoteMusicshopCancel,
*/

                if ( query->ExecuteLD(
                        R_MPX_GENERIC_CONFIRMATION_QUERY,
                        *queryTxt ))
                    {
                    iPlaybackUtility->CommandL( EPbCmdClose );
                    CMPXCommand* cmd( CMPXCommand::NewL() );
                    CleanupStack::PushL( cmd );
                    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
                    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
                    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>(
                        KMPXCommandPlaybackGeneralType,
                        EPbCmdCancelPd );
                    iPlaybackUtility->CommandL( *cmd );
                    CleanupStack::PopAndDestroy( cmd );

                    // Close PD view and go back to host application
                    AppUi()->HandleCommandL( EAknSoftkeyBack );
                    }
                }
            break;
            }
        case EAknSoftkeyBack:
            {
            TUint ignore1(0);
            TMPXPlaybackPdDownloadState state( EPbDlStateNotDownloading );
            TInt ignore2(0);

            GetDownloadInformationL( ignore1,
                                     state,
                                     ignore2,
                                     ignore2);
            if ( EPbDlStateDownloadCompleted == state &&
                 ( EPbStatePaused == iPlaybackState ||
                   EPbStateStopped == iPlaybackState ))
                {
                CMPXCommand* cmd( CMPXCommand::NewL() );
                CleanupStack::PushL( cmd );
                cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
                cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
                cmd->SetTObjectValueL<TMPXPlaybackPdCommand>(
                    KMPXCommandPlaybackGeneralType,
                    EPbCmdFinishPd );
                iPlaybackUtility->CommandL( *cmd );
                CleanupStack::PopAndDestroy( cmd );
                }
            }   // fall through
        default:
            {
            CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
            break;
            }
        }

    MPX_DEBUG1("CMPXPdPlaybackViewImp::HandleCommandL() exiting");
    }

// ---------------------------------------------------------------------------
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXPdPlaybackViewImp::DoHandlePropertyL" );
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
TUid CMPXPdPlaybackViewImp::Id() const
    {
    return TUid::Uid( KMPXPdPlaybackViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::ProcessCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXPdPlaybackViewImp::ProcessCommandL(%d): entering", aCommand);
    if ( aCommand == EAknCmdExit )
        {
        if ( MenuBar() )
            {
            MenuBar()->StopDisplayingMenuBar();
            }
        }
    CAknView::ProcessCommandL( aCommand );
    MPX_DEBUG1("CMPXPdPlaybackViewImp::ProcessCommandL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    MPX_FUNC( "CMPXPdPlaybackViewImp::DoActivateL()" );

    if ( !iContainer )
        {
        if ( !iPlaybackViewLayout )
            {
            iPlaybackViewLayout = CMPXPdPlaybackViewLayout::NewL();
            }
        iContainer = new ( ELeave ) CMPXPdPlaybackViewContainer(
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


// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Used to change toolbar settings before toolbar becomes visible
// -----------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
    {
    if(aResourceId == R_MPX_PBV_TOUCH_TOOLBAR)
        {
        // Put initializations here to take effect before toolbar is shown
        aToolbar->SetItemDimmed( EMPXPbvCmdPreviousListItem, ETrue, EFalse );
        aToolbar->SetItemDimmed( EMPXPbvCmdNextListItem, ETrue, EFalse );
        UpdateToolbar();
        aToolbar->UpdateBackground();
        }
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Handles toolbar events
// -----------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::OfferToolbarEventL( TInt aCommandId )
    {
    CAknView::ProcessCommandL( aCommandId );
    }

// ---------------------------------------------------------------------------
// Updates the middle toolbar button
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewImp::UpdateToolbar()
    {
    MPX_FUNC( "CMPXPdPlaybackViewImp::UpdateToolbar" );

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
            if ((state == EPbStateInitialising) || (state == EPbStatePlaying))
                {
                pausePlayControl->SetCurrentState(0, ETrue);
                }
            else
                {
                pausePlayControl->SetCurrentState(1, ETrue);
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
