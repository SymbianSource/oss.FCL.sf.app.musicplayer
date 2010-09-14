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
* Description:  Implementation of Playback view
*
*/



// INCLUDE FILES
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <akntitle.h>
#include <apgcli.h>
#include <bautils.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <data_caging_path_literals.hrh>
#include <mpxmedia.h>
#include <mpxplaybackutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxcommonplaybackview.rsg>
#include <mpxembeddedplaybackview.rsg>
#include <mpxmediamusicdefs.h>
#include <mpxmediadrmdefs.h>
#include "mpxcommonuihelper.h"
#include <mpxtlshelper.h>
#include "mpxcommonplaybackview.hrh"
#include "mpxembeddedplaybackview.hrh"
#include "mpxembeddedplaybackviewimp.h"
#include "mpxembeddedplaybackviewcontainer.h"
#include "mpxembeddedplaybackviewlayout.h"
#include <mpxappui.hrh>
#include "mpxlog.h"
#include <mpxplaybackutility.h>
#include <akntoolbar.h>
#include <aknbutton.h>
#include "mpxcommonplaybackview.hrh"
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxuser.h>

// CONSTANTS
_LIT( KMPXEmbeddedPlaybackRscPath, "mpxembeddedplaybackview.rsc" );
const TInt KMPXOneSecInMilliSecs( 1000 );
_LIT(KMPXMusicplayerPrivateFolder, "\\private\\");


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewImp::CMPXEmbeddedPlaybackViewImp()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXEmbeddedPlaybackViewImp::ConstructL" );

    // Load embedded resource
    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXEmbeddedPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iEmbeddedResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    CMPXCommonPlaybackViewImp::ConstructL();
    BaseConstructL( R_MPX_EMBEDDED_PLAYBACK_VIEW );

    if(AknLayoutUtils::PenEnabled())
    {
    CAknView::ShowToolbarOnViewActivation(ETrue);
    if ( Toolbar() )
        {
        iToolbar = Toolbar();
        iToolbar->SetToolbarObserver( this );
        }
    }

    // Override base class setting to allow
    // embedded playback to properly fetch media properties.
    iLastPBViewActivated = ETrue;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewImp* CMPXEmbeddedPlaybackViewImp::NewL()
    {
    CMPXEmbeddedPlaybackViewImp* self = CMPXEmbeddedPlaybackViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewImp* CMPXEmbeddedPlaybackViewImp::NewLC()
    {
    CMPXEmbeddedPlaybackViewImp* self = new ( ELeave ) CMPXEmbeddedPlaybackViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewImp::~CMPXEmbeddedPlaybackViewImp()
    {
    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewImp::~CMPXEmbeddedPlaybackViewImp entering" );

    if ( iEmbeddedResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iEmbeddedResourceOffset );
        }

    delete iPlaybackViewLayout;

    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewImp::~CMPXEmbeddedPlaybackViewImp exiting" );
    }


// ---------------------------------------------------------------------------
// Updates Title Pane.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::UpdateTitlePaneL()
    {
    MPX_FUNC( "CMPXEmbeddedPlaybackViewImp::UpdateTitlePaneL" );

    CAknTitlePane* title( static_cast<CAknTitlePane*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ))));
    if ( title )
        {
        RProcess process;
        TApaAppInfo appInfo;
        RApaLsSession session;
        User::LeaveIfError( session.Connect() ); // May want to ignore error
        CleanupClosePushL( session );

        TInt err = session.GetAppInfo( appInfo, MPXTlsHelper::HostUid() );
        if(!err)
            {
            title->SetTextL( appInfo.iCaption );
            }
       CleanupStack::PopAndDestroy(); // Close RApaLsSession session
        }
    }

// ---------------------------------------------------------------------------
// Updates track info field.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::UpdateTrackInfoL(
    const CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXEmbeddedPlaybackViewImp::UpdateTrackInfo" );

    CMPXCommonPlaybackViewImp::UpdateTrackInfoL( aMedia );

    if( iContainer &&
        aMedia &&
        iCommonUiHelper->IsHostPodcastingAppL() )
        {
        // if embedded view is launched via podcasting app
        // display episode name and title

        // Set ETextArtist field as Podcast Title
        if ( aMedia->IsSupported(
                TMPXAttribute( KMPXMediaIdMusic,
                               EMPXMediaMusicAlbum )))
            {
            HBufC* titleText = aMedia->ValueText(
                    TMPXAttribute( KMPXMediaIdMusic,
                                   EMPXMediaMusicAlbum )).AllocL();
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

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::HandleErrorL( TInt aError )
    {
    if ( aError )
        {
        MPX_DEBUG2( "CMPXEmbeddedPlaybackViewImp::HandleErrorL(%d)", aError );

        TBool omaFlag = EFalse;
        TInt plCount( KErrNotFound );
        MMPXSource* source = iPlaybackUtility->Source();
        if ( source )
            {
            CMPXCollectionPlaylist* pl( source->PlaylistL() );
            if ( pl )
                {
                CleanupStack::PushL( pl );
                plCount = pl->Count();
                CleanupStack::PopAndDestroy( pl );
                }
            }

        if ( plCount == 1 )
            {
#ifndef CAMESE_IN_DRM_UTILITY
            // If Camese support is enabled,
            // check if the song is protected WMA
            // and let Camese handle it in case it is.
            if (iCameseSuperDistSupport &&
                    iMedia)
                {
                // Check protection and media type
                if ( iMedia->IsSupported( KMPXMediaDrmProtected ) &&
                        iMedia->IsSupported( KMPXMediaDrmType ) )
                    {
                    TBool prot( iMedia->ValueTObjectL<TBool>( KMPXMediaDrmProtected ) );
                    TMPXMediaDrmType type( iMedia->ValueTObjectL<TMPXMediaDrmType>( KMPXMediaDrmType ) );

                    if (prot && (EMPXDrmTypeWMA == type))
                        {
                        MPX_DEBUG1( "CMPXEmbeddedPlaybackViewImp::HandleErrorL: Let Camese Handle Error");
                        // Let Camese Handle errors in this case.
                        return;
                        }
                    if (prot && (EMPXDrmTypeOMA == type))
                        {
                        omaFlag = ETrue;
                        }
                    }
                }
#endif
            if ( aError == KErrCorrupt || aError == KErrNotSupported )
                {
                // special handling is needed for KErrCorrupt/KErrNotSupported with just 1 song
                iCommonUiHelper->HandleErrorL( aError );
                }
            else if (omaFlag)
                {
                iCommonUiHelper->HandleErrorL( aError, iMedia );
                }
            else
                {
                CMPXCommonPlaybackViewImp::HandleErrorL( aError );
                }

            iPlaybackUtility->CommandL( EPbCmdStop );
            if( aError != KErrArgument && aError != KErrCompletion )
                {
                HandleCommandL( EAknSoftkeyBack );
                }
            }
        else
            {
            // let base class handle it
            CMPXCommonPlaybackViewImp::HandleErrorL( aError );
            }
        }
    }

// ---------------------------------------------------------------------------
// From base class CMPXCommonPlaybackViewImp
//
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXEmbeddedPlaybackViewImp::DoHandlePropertyL" );
    MPX_DEBUG4( "HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    if ( KErrNone == aError )
        {
        switch ( aProperty  )
            {
            case EPbPropertyPosition:
                {
                TInt duration(aValue / KMPXOneSecInMilliSecs);
                if( duration > iDuration)
                    {
                    HandleCommandL(EMPXCmdStop);
                    }
                else
                    {
                    CMPXCommonPlaybackViewImp::DoHandlePropertyL(aProperty,
                        aValue,
                        aError);
                    }
                break;
                }
            default:
                {
                CMPXCommonPlaybackViewImp::DoHandlePropertyL(aProperty,
                    aValue,
                    aError);
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
// From MMPXCHelperEmbeddedOpenObserver
// Handles errors from that occured in embedded mode
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::HandleEmbeddedOpenL(
    TInt aErr,
    TMPXGeneralCategory /*aCategory*/ )
    {
    if ( KErrNone != aErr )
        {
        HandleErrorL( aErr );

        // Kill MPX
        AppUi()->RunAppShutter();
        }
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    // Override base menu actions
    switch ( aResourceId )
        {
        case R_MPX_EMBEDDED_PLAYBACK_VIEW_MENU:
            {
            if ( iCommonUiHelper->IsHostMessagingL() )
                {
                AppUi()->HandleCommandL( EMPXCmdIsFileSaved );
                }
            // Disable Save command if not launched from the specified apps
            if ( !MPXTlsHelper::NeedSave() ||
                 EPbStateNotInitialised == iPlaybackState ||
                 EPbStateInitialising == iPlaybackState )
                {
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdSave,
                    ETrue);
                }
            TBool isOfflineMode( EFalse );
            TBool isFormatNotSupported( EFalse );
            if ( !isOfflineMode && iMedia )
                {
                if ( iMedia->IsSupported( KMPXMediaDrmProtected ) )
                    {
                    if ( iMedia->ValueTObjectL<TBool>( KMPXMediaDrmProtected ) &&
                        iMedia->IsSupported( KMPXMediaDrmCanSetAutomated ) )
                        {
                        // if it's drm protected, check if
                        // KMPXMediaDrmCanSetAutomated
                        isFormatNotSupported = !(
                            iMedia->ValueTObjectL<TBool>( KMPXMediaDrmCanSetAutomated ) );
                        }
                    }
                }
            if ( isOfflineMode ||
                 !iMedia ||
                 isFormatNotSupported )
                {
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdUseAsCascade,
                    ETrue );
                }

            if(iCommonUiHelper->IsHostPodcastingAppL())
                {
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdUseAsCascade,
                    ETrue );

                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdSave,
                    ETrue);
                }

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
            // Call base clase to handle other cases
            CMPXCommonPlaybackViewImp::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXEmbeddedPlaybackViewImp::Id() const
    {
    return TUid::Uid( KMPXEmbeddedPlaybackViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::HandleCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXEmbeddedPlaybackViewImp::HandleCommandL(%d): entering", aCommand);

    switch ( aCommand )
        {
        case EMPXPbvCmdSave:
            {
            HandleSaveL( EMPXNpvOpenAfterSave );
            break;
            }
        case EMPXCmdSaveCancelled:
            {
            iSaveInProgress = EFalse;
            break;
            }
        case EMPXCmdSaveComplete:
            {
            iSaveInProgress = EFalse;
            MPXTlsHelper::SetNeedSave( EFalse );
            break;
            }
        case EMPXCmdSaveNotProgress:
            {
            iSaveInProgress = EFalse;
            break;
            }
        case EAknSoftkeyBack:
            {
            if ( iCommonUiHelper->IsHostMessagingL() )
                {
                AppUi()->HandleCommandL( EMPXCmdIsFileSaved );
                }
            // If this was a playlist, transition back to collection view
            //
            if( MPXTlsHelper::LaunchMode() == EMPXLaunchModePlaylist )
                {
                CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
                }
            // Song Handling, does it need to be saved?
            //
            else if ( MPXTlsHelper::NeedSave() )
                {
                HandleSaveL( EMXPNpvExitAppAfterSave );
                }
            else
                {
                // If back command, only shutdown the embedded view
                AppUi()->RunAppShutter();
                }
            break;
            }
        case EAknCmdExit:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            if ( MPXTlsHelper::NeedSave() )
                {
                HandleSaveL( EMPXNpvExitHostAppAfterSave );
                }
            else
                {
                // If exit command, then close host application as well
                CAknEnv::RunAppShutter();
                }
            break;
            }
    case EMPXPbvCmdUseAsCascade:
    case EMPXPbvCmdAiwCmdAssign:
        {
        const TDesC& savedpath = MPXTlsHelper::FilePath();
        const TDesC& mediapath = iMedia->ValueText( KMPXMediaGeneralUri );
        TFileName fullName(mediapath);
        TParsePtrC fileParse( fullName );
        TBool isPrivate = (fileParse.NamePresent() &&
            !KMPXMusicplayerPrivateFolder().CompareF(
                fileParse.Path().Left(
                    KMPXMusicplayerPrivateFolder().Length())));

        if ( (iCommonUiHelper->IsHostMessagingL() || iCommonUiHelper->IsHostCalendarL() )
                && mediapath.CompareF(savedpath) && isPrivate )
            {
            HandleSaveL( (TMPXNpvSaveMode)aCommand );
            }
        else
            {
            CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
            }
        break;
        }
        default:
            {
            CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
            break;
            }
        }

    MPX_DEBUG1("CMPXEmbeddedPlaybackViewImp::HandleCommandL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    MPX_FUNC( "CMPXEmbeddedPlaybackViewImp::DoActivateL()" );

    if ( !iContainer )
        {
        if ( !iPlaybackViewLayout )
            {
            iPlaybackViewLayout = CMPXEmbeddedPlaybackViewLayout::NewL();
            }
        iContainer = new ( ELeave ) CMPXEmbeddedPlaybackViewContainer(
            this,
            this,
            iPlaybackViewLayout,
            iCommonUiHelper );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }

    CMPXCommonPlaybackViewImp::DoActivateL( aPrevViewId , aCustomMessageId, aCustomMessage );
    }

// ---------------------------------------------------------------------------
// Handle media properties.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::DoHandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC("CMPXEmbeddedPlaybackViewImp::DoHandleMediaL");
    if ( iPrepareToSave )
        {
        delete iMedia;
        iMedia = NULL;
        iPrepareToSave = EFalse;

        if ( KErrNone == aError )
            {
            iMedia = CMPXMedia::NewL( aMedia );
            HandleSaveL( EMPXNpvUsePreviousMode );
            }
        else
            {
            HandleErrorL( aError );
            }
        }
    else
        {
        CMPXCommonPlaybackViewImp::DoHandleMediaL( aMedia, aError );
        }
    }

// ---------------------------------------------------------------------------
// Handle saving a file
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::HandleSaveL( TMPXNpvSaveMode aMode )
    {
    MPX_DEBUG1("CMPXEmbeddedPlaybackViewImp::HandleSaveL(): entering");

    if ( !iSaveInProgress )
        {
        if ( aMode != EMPXNpvUsePreviousMode )
            {
            iSaveMode = aMode;
            }

        if ( !iMedia )
            {
            iPrepareToSave = ETrue;
            RequestMediaL();
            }
        else
            {
            iSaveInProgress = ETrue;
            TInt cmd( EMPXCmdSaveAndReopen );
            if ( iSaveMode == EMXPNpvExitAppAfterSave )
                {
                cmd = EMPXCmdSaveAndExit;
                }
            else if ( iSaveMode == EMPXNpvExitHostAppAfterSave )
                {
                cmd = EMPXCmdSaveAndExitHostApp;
                }
            else if ( iSaveMode == EMPXPbvCmdUseAsCascade )
                {
                cmd = EMPXCmdSaveForUseAsContact;
                }
            else if ( iSaveMode == EMPXPbvCmdAiwCmdAssign )
                {
                cmd = EMPXCmdSaveForUseAsRingtone;
                }
            MPX_TRAPD( err, AppUi()->HandleCommandL( cmd ) );
            if ( KErrNone != err )
                {
                iSaveInProgress = EFalse;
                HandleErrorL( err );
                }
            }
        }

    MPX_DEBUG1("CMPXEmbeddedPlaybackViewImp::HandleSaveL(): exiting");
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewImp::DoHandlePlaybackMessageL");

    TMPXMessageId id( *aMessage.Value<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( *aMessage.Value<TInt>( KMPXMessageGeneralType ) );
        TInt data( *aMessage.Value<TInt>( KMPXMessageGeneralData ) );
        switch ( *aMessage.Value<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EStateChanged:
                UpdateMiddleSoftKeyDisplayL();
                UpdateToolbarL();
                break;

            default:
                break;
            }
        }
    CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL (aMessage);
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Used to change toolbar settings before toolbar becomes visible
// -----------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
    {
    if (aResourceId == R_MPX_PBV_TOUCH_TOOLBAR)
        {
        // Put initializations here to take effect before toolbar is shown
        UpdateToolbarL();
        aToolbar->UpdateBackground();
        }
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Handles toolbar events
// -----------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewImp::OfferToolbarEventL( TInt aCommandId )
    {
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
