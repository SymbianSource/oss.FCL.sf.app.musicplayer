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
* Description:  Implementation of Common Playback view
*
*/



// INCLUDE FILES
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <akntitle.h>
#include <aknnotewrappers.h>
#include <aknnavi.h>
#include <hlplch.h>
#include <bautils.h>
#include <centralrepository.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <bldvariant.hrh>
#include <caf/caferr.h>
#include <aknsoundsystem.h>
#include <AknDlgShut.h>
#include <ctsydomainpskeys.h>
#include <aknclearer.h>


#include <e32const.h>
#ifdef UPNP_INCLUDED
#include <upnpcopycommand.h>
#endif

#include <apgcli.h> // For FF_FMTX
#include <AknWaitDialog.h>

#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include "mplayersecondarydisplayapi.h"

#include <data_caging_path_literals.hrh>
#include <layoutmetadata.cdl.h>
#include <mpxcommonplaybackview.rsg>


#include <mpxfmtx.rsg>
#include <mpxcommonui.rsg>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessage.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxviewplugin.h>
#include <mpxviewpluginmanager.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxsubscription.h>
#include <mpxuser.h>
#include <mpxpskeywatcher.h>
#include <mpxcenrepwatcher.h>
#include <hwrmfmtx.h> // TFmTxState
#include <AiwServiceHandler.h>
#include <mediarecognizer.h>
#ifdef BACKSTEPPING_INCLUDED
#include <mpxbacksteppingutility.h>
#endif // BACKSTEPPING_INCLUDED
#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>

#include "mpxconstants.h"
#include <mpxappui.hrh>
#include "mpxcommonplaybackview.hrh"
#include "mpxcommonplaybackviewimp.h"
#include "mpxcommonplaybackviewcontainer.h"
#include "mpxcommonplaybackviewlayout.h"
#include "mpxalbumartutil.h"
#include "mpxcommonuihelper.h"
#include <mpxinternalcrkeys.h>
#include <mpxtlshelper.h>
#include "mpxlog.h"

#include <akntoolbar.h>
#include <aknbutton.h>
#include "mpxviewprivatepskeys.h"

// CONSTANTS
const TInt KMPXOneSecInMilliSecs( 1000 );
const TUid KMPXEqualizerViewImplementationId = { 0x101FFC77 };
const TInt KMPXPostponeForHandleDelayedError( 1000000 ); // 1S
const TInt KMPXPlaybackViewWindowBackground = -1;
const TInt KMPXDelayForTNRequest( 3000000 ); // 3S

// for freqency display in the format of "XXX.XX"
const TInt KMPXFMFreqWidth        = 6;
const TInt KMPXFMFreqDecimalPlace = 2;
const TInt KMPXOneMhzInOneKhz     = 1000;
const TUid KFmTxAppUid            = { 0x10282BEF };

#ifdef BACKSTEPPING_INCLUDED
const TInt KMaxIntLen( 10 );
_LIT8( KMVPrefix, "MVviewID:" );
const TInt KMVPrefixLen( 9 );
#endif // BACKSTEPPING_INCLUDED

// ---------------------------------------------------------------------------
// NOTE: All the FM Tx related values below originated from the files:
//       - HWRMFmTx.h
//       - HWRMFmTxDomainCRKeys.h
//       - HWRMFmTxDomainPSKeys.h
//
//       These values should be kept in sync with the original copies.  The
//       intention of keeping a copy here is to de-couple the MPX Music Player
//       from the Hardware Resource Manager who is the owner of the keys, as
//       FM Tx is not yet available for the platform.  Eventually, this should
//       be removed as FM Tx becomes part of the platform.
// ---------------------------------------------------------------------------
//
const TUid KCRUidFmTxCenRes = { 0x20008EA6 }; // FM TX Repository ID
const TUid KPSUidHWRMFmTx   = { 0x101F7A02 }; // FM TX category ID

// FM Tx frequency (kHz) key ID of the FM TX CenRep
const TUint32 KFmTxCenResKeyFrequency = 0x00000001;

// FM Transmitter status key ID of the FM TX property.
// Note: This data is interpreted as TFmTxState below
const TUint32 KHWRMFmTxStatus  = 0x00000001;

_LIT( KMPXCommonPlaybackRscPath, "mpxcommonplaybackview.rsc" );

_LIT( KMPXFMTXRscPath, "mpxfmtx.rsc" );

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
_LIT(KMPXPnRealAudioMimeType, "audio/x-pn-realaudio");
_LIT(KMPXRealAudioMimeType, "audio/x-realaudio");
_LIT(KMPXRnRealAudioMimeType, "audio/vnd.rn-realaudio");
#endif

#define THUMBNAIL_CENREP_UID 0x102830B0 // from thumbnailmanageruids.hrh
const TUint32 KSizeAudioFullscreenWidth = 0x16;
const TUint32 KSizeAudioFullscreenHeight = 0x17;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXCommonPlaybackViewImp::ConstructL" );

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXCommonPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

	if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
		{
    	parse.Set( KMPXFMTXRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    	TFileName fmtxResourceFile( parse.FullName() );
    	User::LeaveIfError( MPXUser::CompleteWithDllPath( fmtxResourceFile ) );
    	BaflUtils::NearestLanguageFile( coeEnv->FsSession(), fmtxResourceFile );
    	iFMTXResourceOffset = coeEnv->AddResourceFileL( fmtxResourceFile );
		}

    // Monitor for view activation
    AppUi()->AddViewActivationObserverL( this );

    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );

    iMPXUtility = CMPXAlbumArtUtil::NewL();

    iEmbedded = iEikonEnv->StartedAsServerApp();

    // Get an instance of view utility
    iViewUtility = MMPXViewUtility::UtilityL();
    iViewUtility->AddObserverL( this );

    TInt flags( 0 );
    CRepository* repository( CRepository::NewLC( KCRUidMPXMPFeatures ));
    User::LeaveIfError( repository->Get( KMPXMPLocalVariation, flags ));
    CleanupStack::PopAndDestroy( repository );
    repository = NULL;

    repository = CRepository::NewLC( TUid::Uid(THUMBNAIL_CENREP_UID));

    TInt xSize( 0 );
    TInt ySize( 0 );
    User::LeaveIfError( repository->Get( KSizeAudioFullscreenWidth, xSize ));
    User::LeaveIfError( repository->Get( KSizeAudioFullscreenHeight, ySize ));
    iFullScreenImageSize.SetSize(xSize,ySize);
    CleanupStack::PopAndDestroy( repository );
    repository = NULL;

    iChangeRTForAllProfiles =
        static_cast<TBool>( flags & KMPXChangeRTForAll );

    iLayout = new (ELeave) CMPXCommonPlaybackViewLayout();

    iCollectionUtility = MMPXCollectionUtility::NewL( this );

#ifdef __USE_MESSAGE_SUBSCRIPTION
    // Subscribe to only a few messages from collection utility
    CMPXSubscription* subscription( CMPXSubscription::NewL() );
    CleanupStack::PushL( subscription );
    CMPXSubscriptionItem* subItem1( CMPXSubscriptionItem::NewL() );
    CleanupStack::PushL( subItem1 );
    subItem1->SetTObjectValueL( KMPXMessageGeneralId, KMPXMessageGeneral );
    subItem1->SetTObjectValueL( KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent );
    subItem1->SetTObjectValueL( KMPXMessageGeneralType, EMcMsgDiskRemoved );
    subscription->AddItemL( *subItem1 );
    CMPXSubscriptionItem* subItem2( CMPXSubscriptionItem::CopyL( *subItem1 ));
    CleanupStack::PushL( subItem2 );
    subItem2->SetTObjectValueL( KMPXMessageGeneralType, EMcMsgUSBMassStorageStart );
    subscription->AddItemL( *subItem2 );
    CMPXSubscriptionItem* subItem3( CMPXSubscriptionItem::CopyL( *subItem1 ));
    CleanupStack::PushL( subItem3 );
    subItem3->SetTObjectValueL( KMPXMessageGeneralType, EMcMsgUSBMTPStart );
    subscription->AddItemL( *subItem3 );
    CMPXSubscriptionItem* subItem4( CMPXSubscriptionItem::CopyL( *subItem1 ));
    CleanupStack::PushL( subItem4 );
    subItem4->SetTObjectValueL( KMPXMessageGeneralType, EMcMsgFormatStart );
    subscription->AddItemL( *subItem4 );
    CMPXSubscriptionItem* subItem5( CMPXSubscriptionItem::CopyL( *subItem1 ));
    CleanupStack::PushL( subItem5 );
    subItem5->SetTObjectValueL( KMPXMessageGeneralType, EMcMsgUSBMassStorageEnd );
    subscription->AddItemL( *subItem5 );
    CMPXSubscriptionItem* subItem6( CMPXSubscriptionItem::CopyL( *subItem1 ));
    CleanupStack::PushL( subItem6 );
    subItem6->SetTObjectValueL( KMPXMessageGeneralType, EMcMsgUSBMTPEnd );
    subscription->AddItemL( *subItem6 );
    CMPXSubscriptionItem* subItem7( CMPXSubscriptionItem::CopyL( *subItem1 ));
    CleanupStack::PushL( subItem7 );
    subItem7->SetTObjectValueL( KMPXMessageGeneralType, EMcMsgFormatEnd );
    subscription->AddItemL( *subItem7 );
    CMPXSubscriptionItem* subItem8( CMPXSubscriptionItem::NewL() );
    CleanupStack::PushL( subItem8 );
    subItem8->SetTObjectValueL( KMPXMessageGeneralId, KMPXMessageGeneral );
    subItem8->SetTObjectValueL( KMPXMessageGeneralEvent, TMPXCollectionMessage::EPathChanged );
    subItem8->SetTObjectValueL( KMPXMessageGeneralType, EMcPathChangedByOpen );
    subItem8->SetTObjectValueL( KMPXMessageGeneralData, EMcItemOpened );
    subscription->AddItemL( *subItem8 );
    iCollectionUtility->Collection().AddSubscriptionL( *subscription );
    CleanupStack::PopAndDestroy( subItem8 );
    CleanupStack::PopAndDestroy( subItem7 );
    CleanupStack::PopAndDestroy( subItem6 );
    CleanupStack::PopAndDestroy( subItem5 );
    CleanupStack::PopAndDestroy( subItem4 );
    CleanupStack::PopAndDestroy( subItem3 );
    CleanupStack::PopAndDestroy( subItem2 );
    CleanupStack::PopAndDestroy( subItem1 );
    CleanupStack::PopAndDestroy( subscription );
#endif

    iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    iCommonUiHelper = CMPXCommonUiHelper::NewL( iCollectionUtility );

    // initialize FeatureManager
    FeatureManager::InitializeLibL();

    if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
        {
        // Since support of FM Tx remains the same within the lifetime of this
        // view, it is safe to determine whether to create the associated
        // PS & CR watcher here.

        // create the PS Key watcher to receive notification when FM
        // transmission status changes
        iPSKeyWatcher =
            CMPXPSKeyWatcher::NewL( KPSUidHWRMFmTx, KHWRMFmTxStatus, this );

        // create the Central Repository watcher to receive notification when
        // the value of the FM Frequency CR key is changed.
        iCRWatcher = CMPXCenRepWatcher::NewL(
            KCRUidFmTxCenRes,
            KFmTxCenResKeyFrequency,
            this );
        }

    iStartPlaybackIndex = KErrNotFound;
    iLastSkipDirection = 1;
    iSkipBtnPressed = EFalse;

    // Set to EFalse to avoid handling
    // errors if the view has not been activated
    // before.
    iLastPBViewActivated = EFalse;

    iDatabaseNotReady = EFalse;
    iIgnoredByUsbEvent = EFalse;
#ifdef _DEBUG
    iExitOptionHidden = EFalse;
#else // _DEBUG
    iExitOptionHidden = iCommonUiHelper->ExitOptionHiddenL() && !iEmbedded;
#endif // _DEBUG

    // Support for Camese Super Distribution
#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT
    iCameseSuperDistSupport = ETrue;
#endif
    iMediaRecognizer = CMediaRecognizer::NewL();
    if ( iServiceHandler == NULL)
        {
        iServiceHandler = CAiwServiceHandler::NewL();
        MPX_DEBUG1("CMPXCollectionViewImp::ConstructL() Attaching 'Assign as' menu service...");
        MPX_TRAP( iErrorAttachAssignMenu, iServiceHandler->AttachMenuL( R_MPX_USE_AS_CASCADE,
                                                   R_MPX_AIW_ASSIGN_INTEREST ) );
        }
#ifdef BACKSTEPPING_INCLUDED
    // Initialize the Back Stepping Service Utility with the MPX Music Player
    iBackSteppingUtility = MMPXBackSteppingUtility::UtilityL();
    iBackSteppingUtility->InitializeL(
        TUid::Uid( KMusicPlayerAppUidConstant ) );
#endif // BACKSTEPPING_INCLUDED

#ifdef UPNP_INCLUDED
    if (!iUpnpCopyCommand )
        {
   	    MPX_TRAPD ( error, iUpnpCopyCommand = CUpnpCopyCommand::NewL() );
		    if ( error == KErrNone )
            {
            iUpnpFrameworkSupport = ETrue;
            }
        else
            {
            iUpnpFrameworkSupport = EFalse;
			iUpnpCopyCommand = NULL;
            }
        }
#endif
    iIsffButtonPressed = EFalse;
    iDelayedErrorTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iFmTxActivity = EFmTxStateUnknown;

    iTNRequestTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXCommonPlaybackViewImp::~CMPXCommonPlaybackViewImp()
    {
    MPX_DEBUG1( "CMPXCommonPlaybackViewImp::~CMPXCommonPlaybackViewImp entering" );

    AppUi()->RemoveViewActivationObserver( this );

    delete iMPXUtility;
    delete iMedia;
    delete iIdle;

    if ( iServiceHandler )
        {
        iServiceHandler->DetachMenu( R_MPX_USE_AS_CASCADE,
                                 R_MPX_AIW_ASSIGN_INTEREST );
        delete iServiceHandler;
        delete iMediaRecognizer;
        }

    delete iDelayedExit;

    if ( iPSKeyWatcher )
        {
        delete iPSKeyWatcher;
        }

    if ( iCRWatcher )
        {
        delete iCRWatcher;
        }

    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    if ( iViewUtility )
        {
        iViewUtility->RemoveObserver( this );
        iViewUtility->Close();
        }

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }

   	if ( iFMTXResourceOffset )
       	{
       	iEikonEnv->DeleteResourceFile( iFMTXResourceOffset );
       	}

#ifdef BACKSTEPPING_INCLUDED
    if( iBackSteppingUtility )
        {
        iBackSteppingUtility->Close();
        }
#endif // BACKSTEPPING_INCLUDED

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    delete iLayout;

    if ( iUpnpFrameworkSupport )
        {
        iPlayersList.Close();
        delete iSubPlayerName;
#ifdef UPNP_INCLUDED
        if (iUpnpCopyCommand)
            {
            delete iUpnpCopyCommand;
            }
#endif
        }
    delete iWaitNote;

    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    if ( iCollectionUiHelper )
        {
        iCollectionUiHelper->Close();
        }

    delete iCommonUiHelper;
    delete iUserPlaylists;

    if ( iKeySoundDisabled )
        {
        iAvkonViewAppUi->KeySounds()->PopContext();
        }

    // free up the FeatureManager
    FeatureManager::UnInitializeLib();
    if(iDelayedErrorTimer)
    	{
    	iDelayedErrorTimer->Cancel();
    	delete iDelayedErrorTimer;
    	}

    delete iOldUri;
    
    if ( iTNRequestTimer )
        {
        iTNRequestTimer->Cancel();
        delete iTNRequestTimer;
        }
    
    MPX_DEBUG1( "CMPXCommonPlaybackViewImp::~CMPXCommonPlaybackViewImp exiting" );
    }

// ---------------------------------------------------------------------------
// Updates playback view.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateViewL()
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateViewL" );

    if ( iContainer && !iSwitchingView )
        {
        UpdatePlaybackState( iPlaybackState );
        UpdateTrackInfoL( iMedia );
        UpdateTrackPlaybackPositionL( iPosition, iDuration );
        UpdateTrackPosInPlaylistL();
        UpdateAlbumArtL( iMedia );
        UpdateDownloadStateLabelL();

        UpdateFMTransmitterInfoL( ETrue );

        // Retrieve current repeat & random modes
        iPlaybackUtility->PropertyL( *this, EPbPropertyRandomMode );
        iPlaybackUtility->PropertyL( *this, EPbPropertyRepeatMode );
        }
    }

// ---------------------------------------------------------------------------
// Updates track info field.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateTrackInfoL(
    const CMPXMedia* aMedia )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateTrackInfo" );

    if ( iContainer && !iSwitchingView )
        {
        if ( aMedia )
            {
            if ( aMedia->IsSupported( KMPXMediaGeneralTitle ) )
                {
                iContainer->UpdateLabelL(
                    ETextTrack,
                    aMedia->ValueText( KMPXMediaGeneralTitle ) );
                }
            else if ( aMedia->IsSupported( KMPXMediaGeneralUri ) )
                {
                TParsePtrC filePath(
                    aMedia->ValueText( KMPXMediaGeneralUri ) );
                iContainer->UpdateLabelL(
                    ETextTrack, filePath.Name() );
                }
            else
                {
                iContainer->UpdateLabelL(
                    ETextTrack, KNullDesC );
                }

            if ( aMedia->IsSupported( KMPXMediaMusicArtist ) )
                {
                const TDesC& artist =
                    aMedia->ValueText( KMPXMediaMusicArtist );
                if ( artist != KNullDesC )
                    {
                    iContainer->UpdateLabelL(
                        ETextArtist,
                        artist );
                    }
                else
                    {
                    // Display unknown artist as artist
                    HBufC* unknownArtistText =
                        StringLoader::LoadLC( R_MPX_PBV_UNKNOWN_ARTIST_LABEL );
                    iContainer->UpdateLabelL(
                        ETextArtist, *unknownArtistText );
                    CleanupStack::PopAndDestroy( unknownArtistText );
                    }
                }
            else
                {
                // Display unknown artist as artist
                HBufC* unknownArtistText =
                    StringLoader::LoadLC( R_MPX_PBV_UNKNOWN_ARTIST_LABEL );
                iContainer->UpdateLabelL(
                    ETextArtist, *unknownArtistText );
                CleanupStack::PopAndDestroy( unknownArtistText );
                }

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
            if ( iMedia->IsSupported( KMPXMediaGeneralMimeType ) )
                {
                // Get mime type
                const TDesC& mimeType = iMedia->ValueText(
                    KMPXMediaGeneralMimeType );
                const TBool realAudioMode =
                    ( mimeType.Compare( KMPXPnRealAudioMimeType ) == 0 ) ||
                    ( mimeType.Compare( KMPXRealAudioMimeType ) == 0) ||
                    ( mimeType.Compare( KMPXRnRealAudioMimeType ) == 0 );

                // Set the real audio mode
                iContainer->SetRealAudioMode( realAudioMode );
                }
#endif
            }
        else
            {
            // Display nothing if properties is NULL
            iContainer->UpdateLabelL(
                ETextTrack, KNullDesC );
            iContainer->UpdateLabelL(
                ETextArtist, KNullDesC );
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
            // reset real audio mode
            iContainer->SetRealAudioMode( EFalse );
#endif
            }
        }
    }

// ---------------------------------------------------------------------------
// Update current playback state.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdatePlaybackState(
    TMPXPlaybackState aPlaybackState )
    {
    MPX_DEBUG2("CMPXCommonPlaybackViewImp::UpdatePlaybackState(%d): Entering", aPlaybackState);

    if ( iContainer && !iSwitchingView )
        {
        TMPXPbvPlaybackMode mode( EInvalidMode );

        switch ( aPlaybackState )
            {
            case EPbStateNotInitialised:
                {
                mode = iPlaybackUtility->Source() ? EUninitialized: ENoTracksMode;
                break;
                }
            case EPbStateInitialising:
                {
                mode = EInitialising;
                break;
                }
            case EPbStatePlaying:
                {
                mode = EPlayMode;
                iIgnoredByUsbEvent = EFalse;
                break;
                }
            case EPbStatePaused:
                {
                mode = EPauseMode;
                break;
                }
            case EPbStateStopped:
                {
                mode = EStopMode;
                break;
                }
            case EPbStateBuffering:
                {
                mode = EBufferingMode;
                break;
                }
            default:
                {
                // Pass
                break;
                }
            }

        if ( EInvalidMode != mode )
            {
            iPlaybackState = aPlaybackState;
            iContainer->SetMode( mode );
            }

        iContainer->UpdateButtons( aPlaybackState );
        }
    else
        {
        // If no container, just need to update state
        switch ( aPlaybackState )
            {
            case EPbStateNotInitialised:
            case EPbStateInitialising:
            case EPbStatePlaying:
            case EPbStatePaused:
            case EPbStateStopped:
                {
                iPlaybackState = aPlaybackState;
                break;
                }
            default:
                {
                // Pass
                // Ignore other state changes such as skipping, etc.
                break;
                }
            }
        }
    MPX_DEBUG1("CMPXCommonPlaybackViewImp::UpdatePlaybackState(): Exiting");
    }

// ---------------------------------------------------------------------------
// Updates track's playback position.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateTrackPlaybackPositionL(
    TInt aPos,
    TInt aDuration )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateTrackPlaybackPosition" );

    if ( iContainer && !iSwitchingView )
        {
        if ( aPos > aDuration )
            {
            aPos = aDuration;
            }
        switch ( iPlaybackState )
            {
            case EPbStatePaused:
            case EPbStatePlaying:
                {
                iContainer->UpdateTimeIndicatorsL( aPos, aDuration );
                break;
                }
            case EPbStateStopped:
            case EPbStateInitialising:
                {
                iContainer->UpdateTimeIndicatorsL( aPos, aDuration );
                } // falls through
            case EPbStateNotInitialised:
                {
                iContainer->UpdateDurationLabelL( aDuration );
                break;
                }
            default:
                {
                // Do nothing
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates track's album art.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateAlbumArtL(
    const CMPXMedia* aMedia )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateAlbumArt" );

    if ( iContainer && !iSwitchingView )
        {
        TInt err( KErrNone );
        if ( aMedia && aMedia->IsSupported(KMPXMediaGeneralUri))
            {
            const TDesC& album = aMedia->ValueText( KMPXMediaGeneralUri );
            if(!iOldUri || iOldUri->Compare(album)!= 0)
                {
                // Request for pre-generated TN size
                MPX_TRAP( err,
                    iMPXUtility->ExtractAlbumArtL(
                    *aMedia,
                    *iContainer,
                    iFullScreenImageSize ); );

                // cancel timer
                if ( iTNRequestTimer->IsActive())
                    {
                    iTNRequestTimer->Cancel();
                    }

                if ( err == KErrNone )
                    {
                    // startup timer for updating album art with custom size
                    TCallBack cb( HandleTNRequestForCustomSizeL, this );
                    iTNRequestTimer->Start( KMPXDelayForTNRequest,
                        KMPXDelayForTNRequest,
                        cb );
                    }  
                }
            
            delete iOldUri;
            iOldUri = NULL;
            iOldUri = album.AllocL();
            }

        if (KErrNone != err )
            {
            // If error, show default album art
            MPX_DEBUG2("CMPXCommonPlaybackViewImp::UpdateAlbumArt(): err = %d", err);
            iContainer->ExtractAlbumArtCompleted( NULL, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates track position in playlist field.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateTrackPosInPlaylistL()
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateTrackPosInPlaylist" );

    if ( iContainer && !iSwitchingView )
        {
        TBool emptyCount( EFalse );
        TInt count( 0 );
        TInt index( 0 );

        MMPXSource* s = iPlaybackUtility->Source();
        if ( s )
            {
            CMPXCollectionPlaylist* playlist = s->PlaylistL();
            if ( playlist )
                {
                count = playlist->Count();
                index = playlist->PathIndex( playlist->Index() );
                delete playlist;
                playlist = NULL;
                }
            }
        MPX_DEBUG3("CMPXCommonPlaybackViewImp::UpdateTrackPosInPlaylistL: index = %d, count = %d", index, count );

        // not display for progressive mode & embedded track mode
        TMPXLaunchMode mode = MPXTlsHelper::LaunchMode();

        if ( count )
            {
            if ( EMPXLaunchModeTrack != mode )
                {
                // Current index (1-based) if playlist is non-empty
                // Magic: array granularity
                CArrayFixFlat<TInt>* params =
                    new ( ELeave ) CArrayFixFlat<TInt>( 2 );
                CleanupStack::PushL( params );
                params->AppendL( index + 1 );
                params->AppendL( count );
                HBufC* plcounter = StringLoader::LoadLC(
                    R_MPX_PBV_PLAYLIST_ITEM_COUNTER_LABEL, *params );
                iContainer->UpdateLabelL(
                    ETextPlaylistCount,
                    *plcounter );
                CleanupStack::PopAndDestroy( plcounter );
                CleanupStack::PopAndDestroy( params );
                //iContainer->UpdateLabelL( ETextEmpty, KNullDesC );
                }
            else
                {
                emptyCount = ETrue;
                }
            }
        else
            {
            emptyCount = ETrue;

            // Display empty state text
            //HBufC* noTracksText =
            //    StringLoader::LoadLC( R_MPX_PBV_NO_TRACKS_LABEL );
            //iContainer->UpdateLabelL(
             //   ETextEmpty, *noTracksText );
            //CleanupStack::PopAndDestroy( noTracksText );
            }

        if ( emptyCount )
            {
            // Empty string if playlist is empty
            iContainer->UpdateLabelL(
                ETextPlaylistCount,
                KNullDesC );
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates download state label.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateDownloadStateLabelL()
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateDownloadStateLabelL" );
    // Do not show opening popup
    }

// ---------------------------------------------------------------------------
// Updates Title Pane.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateTitlePaneL()
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateTitlePaneL" );

    CAknTitlePane* title( static_cast<CAknTitlePane*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ))));
    if ( title )
        {
        HBufC* titleText( StringLoader::LoadLC(
            R_MPX_PLAYBACKVIEW_TITLE ));
        title->SetTextL( *titleText );
        CleanupStack::PopAndDestroy( titleText );
        }
    }

// ---------------------------------------------------------------------------
// Updates FM Transmitter Info
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateFMTransmitterInfoL(
    TBool aForceUpdate )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::UpdateFMTransmitterInfoL" );

    if ( iContainer && !iSwitchingView )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
            {
            // if FM Transmitter is supported feature, retrieve the state of
            // the FM Transmitter
            TInt value(EFmTxStateUnknown);
            TInt freqKHz = 0;

            if ( iPSKeyWatcher->GetValue( value ) == KErrNone )
                {
                MPX_DEBUG2( "CMPXCommonPlaybackViewImp::UpdateFMTransmitterInfoL iPSKeyWatcher value %d", value);
                // EFmTxStatePowerSaveAccessory & EFmTxStatePowerSaveInactivity
                // are considered as off
                if ( value == EFmTxStateActive ||
                     value == EFmTxStateInactive )
                    {
                    // if FM Transmitter is turned on, retrieve the frequency
                    MPX_TRAPD( err, freqKHz = iCRWatcher->CurrentValueL() );
                    if ( err != KErrNone )
                        {
                        freqKHz = 0;
                        }
                    }
                }
            iFmTxActivity = value;

            MPX_DEBUG4( "CMPXCommonPlaybackViewImp::UpdateFMTransmitterInfoL iFMTxFreqKHz %d freqKHz %d aForceUpdate %d", iFMTxFreqKHz, freqKHz, aForceUpdate);
            if ( iFMTxFreqKHz != freqKHz || aForceUpdate )
                {
                // if the FM Transmitter state has been changed from one active
                // to another and it's frequency value changed, or if FM
                // Transmitter is changed from an active to an inactive state,
                // or vice versa, update the frequency display and save the new
                // value
                iFMTxFreqKHz = freqKHz;

                // Create display string and default it to an empty string
                RBuf freqTxt;
                freqTxt.Assign( NULL );

                CleanupClosePushL( freqTxt );

                // display the frequncy if it's a valid value
                if ( iFMTxFreqKHz > 0 )
                    {
                    TReal freqMHz = static_cast<TReal> (iFMTxFreqKHz) /
                        static_cast<TReal> (KMPXOneMhzInOneKhz);

                    TRealFormat realFormat(
                        KMPXFMFreqWidth,
                        KMPXFMFreqDecimalPlace);

                    // display frequecy as : XXX.XX Mhz
                    TBuf<KMPXFMFreqWidth + 1> freqMHzStr;
                    TInt err = freqMHzStr.Num( freqMHz, realFormat );
                    if ( err > 0 )
                        {
                        HBufC* labelFormatText = StringLoader::LoadL(
                            R_MPX_FM_FREQUENCY_LABEL,
                            freqMHzStr );

                        // ownership transferred
                        freqTxt.Assign(labelFormatText);
                        }
                    }

                iContainer->UpdateLabelL(
                    ETextFMFrequency, freqTxt );

                CleanupStack::PopAndDestroy( &freqTxt );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC_EX("CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL");

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessagePbMediaChanged == id )
        {
        MPX_DEBUG1("CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL: PbMediaChanged");
        if ( aMessage.IsSupported( KMPXMessagePbMedia ) )
            {
            CMPXMedia* media( aMessage.Value<CMPXMedia>( KMPXMessagePbMedia ) );
            User::LeaveIfNull( media );
            DoHandleMediaL( *media, KErrNone );
            }
        }
    else if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EPropertyChanged:
                {
                MPX_DEBUG2( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL - EPropertyChanged(%d)", type );
                TMPXPlaybackProperty property(
                    static_cast<TMPXPlaybackProperty>( type ) );
                TInt error( KErrNone );

                DoHandlePropertyL( property, data, error );
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL - EStateChanged(%d)", type );

                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );
                if ( ( data == KErrNotSupported ) &&
                     ( state == EPbStatePaused ) )
                    {
                    if ( iLastPBViewActivated )
                        {
                        // In UPnP case, this will display an error message
                        iCommandSender = ECsMediaKey;
                        IsCommandSupportedL();
                        }
                    }
                else if ( (( data == KErrAccessDenied ) || ( data == KErrDied ) || ( data == KErrInUse )) &&
                          ( state == EPbStatePaused ))
                    {
                    if ( iLastPBViewActivated &&
                         MPXUser::IsCallOngoing( EPSCTsyCallTypeH324Multimedia ))
                        {
                        iCommonUiHelper->HandleErrorL( KMPXErrorVideoCall );
                        }
                    }
                else
                    {
                    DoHandleStateChangedL( state, data );
                    }
                break;
                }
            case TMPXPlaybackMessage::EMediaChanged:
                {
                MPX_DEBUG1( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL - EMediaChanged()");
                    RequestMediaL();
                if ( iIdle )
                    {
                    if ( iIdle->IsActive() )
                        {
                        MPX_DEBUG1( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL() iIdle active, canceling" );
                        iIdle->Cancel();
                        }
                    delete iIdle;
                    iIdle = NULL;
                    }
                break;
                }
            case TMPXPlaybackMessage::ECommandReceived:
                {
                MPX_DEBUG2( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL - ECommandReceived(%d)", type );
                break;
                }
            case TMPXPlaybackMessage::ESkipping:
                {
                MPX_DEBUG2( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL - ESkipping(%d)", data );
                if ( iContainer )
                    {
                    iContainer->SetMode( EMediaKeySkipping );
                    }
                TInt offset( data );
                if ( offset < 0 )
                    {
                    iLastSkipDirection = -1;
                    }
                else
                    {
                    iLastSkipDirection = 1;
                    }
                iSkipping = ETrue;
                break;
                }
            case TMPXPlaybackMessage::ESkipEnd:
                {
                MPX_DEBUG1( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL - ESkipEnd()");
                iSkipping = EFalse;
                break;
                }
            case TMPXPlaybackMessage::EPlaylistUpdated:
                {
                MPX_DEBUG1( "CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL - EPlaylistUpdated()");
                // Do not retrive media again if in the middle of handling an error
                if ( KErrNone == iDelayedError )
                    {
                    TBool noTracks( ETrue );
                    MMPXSource* source = iPlaybackUtility->Source();
                    if ( source )
                        {
                        CMPXCollectionPlaylist* pl( source->PlaylistL() );
                        if ( pl )
                            {
                            CleanupStack::PushL( pl );
                            if ( pl->Count() > 0 )
                                {
                                // Only update media if in foreground
                                if ( iContainer )
                                    {
                                    RequestMediaL();
                                    }
                                else
                                    {
                                    // If in background, wait for view to be activated
                                    // before requesting media again
                                    delete iMedia;
                                    iMedia = NULL;
                                    }
                                noTracks = EFalse;
                                }
                            CleanupStack::PopAndDestroy( pl );
                            }
                        }
                    if ( noTracks )
                        {
                        delete iMedia;
                        iMedia = NULL;
                        iPlaybackState = EPbStateNotInitialised;
                        UpdateViewL();
                        }
                    }
                break;
                }
            case TMPXPlaybackMessage::EInitializeComplete:
                {
                // Reset flag, done opening new item.
                MPX_DEBUG1("CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL()");
                MPX_DEBUG1("    > EInitializeComplete Reset New Item Opened Flag");
                iNewItemOpened = EFalse;
                break;
				}
            default:
                {
                MPX_DEBUG4("CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL(): Ignore message %d %d %d",
                            aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ), type, data);
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback property.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::DoHandlePropertyL" );
    MPX_DEBUG4( "HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    if ( KErrNone == aError )
        {
        switch ( aProperty  )
            {
            case EPbPropertyPosition:
                {
                iPosition = aValue / KMPXOneSecInMilliSecs;
                if( !iIsTapped )
                    {
                    UpdateTrackPlaybackPositionL( iPosition, iDuration );
                    break;
                    }
                else
                    {
                    iIsTapped = EFalse;
                    // retrieve iCurrentPlayerType info
                    GetCurrentPlayerDetails();
                    if ( iCurrentPlayerType == EPbRemote )
                        {
                        UpdateTrackPlaybackPositionL( iOldPosition, iDuration );
                        break;
                        }
                    else
                        {
                        UpdateTrackPlaybackPositionL( iPosition, iDuration );
                        break;
                        }
                    }
                }
            case EPbPropertyDuration:
                {
                iDuration = aValue / KMPXOneSecInMilliSecs;
                break;
                }
            case EPbPropertyRandomMode:
                {
                iRandomMode = aValue;
                if ( iContainer )
                    {
                    iContainer->SetRandomMode( aValue );
                    }
                break;
                }
            case EPbPropertyRepeatMode:
                {
                iRepeatMode = aValue;
                if ( iContainer )
                    {
                    iContainer->SetRepeatMode(
                        EPbRepeatAll == aValue, EPbRepeatOne == aValue );
                    }
                break;
                }
            default:
                {
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
// Handle media properties.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DoHandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::DoHandleMediaL" );
    MPX_DEBUG2("CMPXCommonPlaybackViewImp::DoHandleMediaL(): aError = %d", aError);

    delete iMedia;
    iMedia = NULL;
    if ( KErrNone == aError )
        {
        iMedia = CMPXMedia::NewL( aMedia );

        UpdateTrackPosInPlaylistL();
        UpdateTrackInfoL( iMedia );
        UpdateAlbumArtL( iMedia );


        // Update duration if previously not available
        if ( iMedia->IsSupported( KMPXMediaGeneralDuration ) )
            {
            iDuration = iMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration ) /
                KMPXOneSecInMilliSecs;
            UpdateTrackPlaybackPositionL( iPosition, iDuration );
            }

        // If there's a delayed error, handle it now
        // that the media object has arrived
        if ( iDelayedError != KErrNone )
            {
            // if repeat mode is off, delay to handle this error.
            if ( EPbRepeatOff == iRepeatMode )
                {
                // save current index
                iErrIndex = KErrNotFound;
                if (iPlaybackUtility)
                    {
                    MMPXSource* source( iPlaybackUtility->Source() );
                    if ( source )
                        {
                        CMPXCollectionPlaylist* pl( source->PlaylistL() );
                        if ( pl )
                            {
                            CleanupStack::PushL( pl );
                            iErrIndex = pl->Index();
                            CleanupStack::PopAndDestroy( pl );
                            }
                        }
                    }
                // startup timer for handling delayed error
                TCallBack cb( HandleDelayedError, this );
                if (iDelayedErrorTimer->IsActive())
                    {
                    iDelayedErrorTimer->Cancel();
                    }
                iDelayedErrorTimer->Start( KMPXPostponeForHandleDelayedError,
                        KMPXPostponeForHandleDelayedError,
                        cb );
                // reset iDelayedError
                iLastDelayedErr = iDelayedError;
                iDelayedError = KErrNone;
                }
            else
                {
                // if repeat mode isn't off, handle it at once.
                if (iDelayedErrorTimer->IsActive())
                    {
                    iDelayedErrorTimer->Cancel();
                    }

                // Copy error code and reset iDelayedError, this is becuase
                // a media event may be called while handling this error
                TInt err( iDelayedError );
                iDelayedError = KErrNone;
                HandleErrorL( err );
                }
            }
        }
    else
        {

#ifndef CAMESE_IN_DRM_UTILITY
        // Check if Camese Support is enabled and
        // if the error is caused by PV being unable
        // to retrieve media properties before handling
        // the error here.
        if (iCameseSuperDistSupport && aError == KErrPermissionDenied)
            {
            // Clear delayed errors.
            iDelayedError = KErrNone;
            return;
            }
#endif
        HandleErrorL(aError);

        if ( aError == iDelayedError )
            {
            iDelayedError = KErrNone;
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DoHandleStateChangedL(
    TMPXPlaybackState aState,
    TInt aData )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::DoHandleStateChangedL" );
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::DoHandleStateChangedL %d", aState );
    UpdatePlaybackState( aState );

    switch ( aState )
        {
        case EPbStateStopped:
        case EPbStateNotInitialised:
            {
            iPosition = 0;
            UpdateTrackPlaybackPositionL( iPosition, iDuration );
            break;
            }
        case EPbStateInitialising:
            {
            // Save the start playback index
            if ( KErrNotFound == iStartPlaybackIndex &&
                 KErrNotFound != aData )
                {
                iStartPlaybackIndex = aData;
                }
            UpdateDownloadStateLabelL();
            iDelayedErrorTimer->Cancel();
            break;
            }
        case EPbStatePlaying:
        case EPbStatePaused:
        default:
            {
            MPX_PERF_CHECKPT("Playback state changes to Play/Pause");
            iStartPlaybackIndex = KErrNotFound;
            iLastSkipDirection = 1;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Get simple embedded mode.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonPlaybackViewImp::SimpleEmbeddedMode() const
    {
    return iEmbedded;
    }

// ---------------------------------------------------------------------------
// Get embedded status.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonPlaybackViewImp::IsEmbedded() const
    {
    return iEmbedded;
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleErrorL( TInt aError )
    {
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::HandleErrorL(%d): Entering", aError );
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::HandleErrorL iIgnoredByUsbEvent(%d):", iIgnoredByUsbEvent );
    if ( aError )
        {
        TInt currentIndex( KErrNotFound );
        TInt nextIndex( KErrNotFound );
        TInt plCount( KErrNotFound );
        MMPXSource* source( iPlaybackUtility->Source() );
        if ( source )
            {
            CMPXCollectionPlaylist* pl( source->PlaylistL() );
            if ( pl )
                {
                CleanupStack::PushL( pl );
                currentIndex = pl->Index();
                if ( iLastSkipDirection < 0 )
                    {
                    pl->Previous( ETrue );
                    }
                else
                    {
                    pl->Next( ETrue );
                    }
                nextIndex = pl->Index();
                plCount = pl->Count();
                CleanupStack::PopAndDestroy( pl );
                }
            }

        // MMF framework will return KErrArgument/KErrCompletion if it reaches
        // the end of a partially downloaded file.  The downloaded portion of
        // the file should still be playable in this case.
        if ( KErrNotFound != nextIndex && !iIgnoredByUsbEvent &&
        	 KErrDisMounted != aError &&
            ( ( KErrArgument != aError && KErrCompletion != aError ) ||
              iPlaybackState != EPbStatePlaying ))
            {
            // Check if all tracks are invalid, or if there's only
            // 1 track in the playlist
            if ( nextIndex == iStartPlaybackIndex ||
                 plCount == 1 ||
                 EPbRepeatOne == iRepeatMode)
                {
                aError = KMPXAllTracksInvalid;
                }
            }

        // Igore these messages when USB event just happened
        if (iIgnoredByUsbEvent &&
            (aError == KErrCANoRights ||
            aError == KErrPathNotFound ||
            aError == KErrDisMounted )
            )
            {
            iIgnoredByUsbEvent = EFalse;
            return;
            }

        // else iIgnoredByUsbEvent==EFalse, then furtherly check USB status in central repository.
        else if ( !iIgnoredByUsbEvent )
        	{
            TInt usbStatus;
            RProperty::Get( KPSUidUsbWatcher, KUsbWatcherSelectedPersonality, usbStatus );

            if ( KUsbPersonalityIdMS == usbStatus && KErrDisMounted == aError  )
            	{
            	// Igore this error when USB is avtive in MassStrorage mode.
            	return;
            	}
            }

        switch ( aError )
            {
            case KMPXRightsAboutToExpire:
            case KErrCANotSupported:
            case KErrCANoPermission:
            case KErrCANoRights:
            case KErrCANoAgent:
            case KErrCAOutOfRange:
            case KErrCAPendingRights:
            case KErrCASizeNotDetermined:
            case KErrCANewFileHandleRequired:
            case KErrPermissionDenied:
                {
                TBool skip(ETrue);

                if ( iMedia )
                    {
                    if ( iMedia->IsSupported( KMPXMediaDrmType ) )
                        {
                        TMPXMediaDrmType type(
                            iMedia->ValueTObjectL<TMPXMediaDrmType>(
                                KMPXMediaDrmType ) );

                        // Only display DRM error message for WMDRM
                        // if it's the first song selected
                        if ( EMPXDrmTypeWMA == type &&
                             currentIndex == iStartPlaybackIndex)
                            {
#ifndef CAMESE_IN_DRM_UTILITY
                            MPX_DEBUG2("CMPXCommonPlaybackViewImp::HandleErrorL iCameseSuperDistSupport %d", iCameseSuperDistSupport);
                            // Only handle the error if Camese is disabled.
                            if (!iCameseSuperDistSupport)
                                {
                                MPX_DEBUG2("CMPXCommonPlaybackViewImp::HandleErrorL iNewItemOpened %d", iNewItemOpened);
                                if (iNewItemOpened)
                                    {
                                    iPlaybackUtility->CommandL( EPbCmdStop );
                                    iCommonUiHelper->HandleErrorL( aError, iMedia );
                                    // If Camese is off and we
                                    // received the collection message
                                    // indicating we are opening a new item,
                                    // disable skipping on this error.
                                    // We just stop playback.
                                    skip = EFalse;

                                    // Reset the Start Playback Index.
                                    iStartPlaybackIndex = KErrNotFound;
                                    }
                                }
                            else if (iNewItemOpened)
#else
                                if (iNewItemOpened)
#endif
                                    {
                                    MPX_DEBUG1("CMPXPlaybackViewImp::HandleErrorL Skip Disabled - New Item");

                                // If Camese is on and we
                                // received the collection message
                                // indicating we are opening a new item,
                                // disable skipping on this error. Camese will handle
                                // postdelivery in this case.
                                skip = EFalse;

                                // Reset the Start Playback Index.
                                iStartPlaybackIndex = KErrNotFound;
                                }
                            }
                        }
                    }

                if (skip)
                    {
                    SkipOnErrorL( currentIndex, plCount );
                    }
                break;
                }
            case KErrAccessDenied:
                {
                // Only display error message if first one selected to play
                if ( currentIndex == iStartPlaybackIndex )
                    {
                    iCommonUiHelper->HandleErrorL( aError, iMedia );
                    }
                SkipOnErrorL( currentIndex, plCount );
                break;
                }
            case KErrNotSupported:
            case KErrCorrupt:
            case KErrNotFound:
            case KErrPathNotFound:
            case KErrDivideByZero:
            case KErrGeneral:
                {
                // USB dismounts the disk, it cannot find any records
                if( !iDatabaseNotReady )
                    {
                    SkipOnErrorL( currentIndex, plCount );
                    }
                break;
                }
            case KMPXAllTracksInvalid:
                {
                HandleAllTracksInvalidL();
                break;
                }
            case KErrArgument:
            case KErrCompletion:
                {
                TBool skip( ETrue );

                // UI specs say theres two cases where it stops and
                // does not skip to the next song.
                // First case is if not repeat all and playback has
                // reached the end of the list and gone to the first song.
                // Second case is if not repeat all and the user is skipping
                // backwards and reached the first song
                if ( EPbRepeatAll != iRepeatMode &&
                     0 == currentIndex &&
                     -1 == iLastSkipDirection)
                    {
                    skip = EFalse;
                    }

                // Skip to next/previous track
                if ( skip )
                    {
                    if ( iLastSkipDirection < 0 )
                        {
                        iPlaybackUtility->CommandL( EPbCmdPrevious );
                        }
                    else
                        {
                        iPlaybackUtility->CommandL( EPbCmdNext );
                        }
                    }
                break;
                }
            case KErrDiskFull:
                {
                iCommonUiHelper->HandleErrorL( aError, iMedia );
                break;
                }
            default:
                {
                iPlaybackUtility->CommandL( EPbCmdStop );
                iCommonUiHelper->HandleErrorL( aError, iMedia );
                break;
                }
            }
        }

    // Reset flag after error handling is done.
    MPX_DEBUG1("CMPXPlaybackViewImp::HandleErrorL()");
    MPX_DEBUG1("    > Reset New Item Opened Flag");
    iNewItemOpened = EFalse;

    MPX_DEBUG1( "CMPXCommonPlaybackViewImp::HandleErrorL: Exiting");
    }

// ---------------------------------------------------------------------------
// Displays error note for when all tracks are invalid,
// and goes back to collection list view
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleAllTracksInvalidL()
    {
    MPX_DEBUG1("CMPXCommonPlaybackViewImp::HandleAllTracksInvalidL: Entering");
    MPX_DEBUG2("CMPXCommonPlaybackViewImp::HandleAllTracksInvalidL: iDatabaseNotReady (%d)", iDatabaseNotReady);

    iPlaybackUtility->CommandL( EPbCmdStop );
    UpdatePlaybackState( EPbStateStopped );

    TInt mpxWindowGroupId = CEikonEnv::Static()->RootWin().Identifier();

    // Usb event happened.
    if (iDatabaseNotReady)
        {
        iIgnoredByUsbEvent = ETrue;
        }

    // Do not display error if view is not active
    if ( iContainer && !iDatabaseNotReady &&
         iCommonUiHelper->IsForegroundApplication(mpxWindowGroupId) &&
         iViewUtility->ActiveViewType() != TUid::Uid( KMPXPluginTypeEqualizerUid ) )
        {
        iCommonUiHelper->HandleErrorL( KMPXAllTracksInvalid, iMedia );

        // Check foreground again just in case of user presses end key and switch
        // to idle application while error notes are showing
        // do not switch view when USB events ongoing
        if (!iDatabaseNotReady &&
         iCommonUiHelper->IsForegroundApplication(mpxWindowGroupId))
            {
                HandleCommandL( EAknSoftkeyBack );
            }

        iStartPlaybackIndex = KErrNotFound;
        iLastSkipDirection = 1;

        // Reset the playlist to point to the first item
        // in list, but do not initialize the song for
        // playback
        MMPXSource* source( iPlaybackUtility->Source() );
        if ( source )
            {
            CMPXCollectionPlaylist* pl( source->PlaylistL() );
            if ( pl )
                {
                CleanupStack::PushL( pl );
                pl->SetToFirst();
                iPlaybackUtility->InitL( *pl, EFalse );
                CleanupStack::PopAndDestroy( pl );
                }
            }
        }


    MPX_DEBUG1("CMPXCommonPlaybackViewImp::HandleAllTracksInvalidL: Exiting");
    }

// ---------------------------------------------------------------------------
// Checks whether or not to skip to next track on an error
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::SkipOnErrorL(
    TInt aCurrentIndex,
    TInt aCount )
    {
    MPX_DEBUG3("CMPXCommonPlaybackViewImp::SkipOnErrorL(%d %d): Entering", aCurrentIndex, aCount);

    TBool skip( ETrue );
    TBool displayError( EFalse );

    // If playlist is invalid, don't skip. Fix for error EJPJ-7CVAJX.
    if ( aCurrentIndex == KErrNotFound && aCount == KErrNotFound )
        {
        skip = EFalse;
        iCommonUiHelper->HandleErrorL( KErrCorrupt );
        }
    // UI specs say theres two cases where it stops and
    // does not skip to the next song.
    // First case is if not repeat all and playback has
    // reached the end of the list and gone to the first song.
    // Second case is if not repeat all and the user is skipping
    // backwards and reached the first song
    else if ( EPbRepeatAll != iRepeatMode )
        {
        if ( 0 == aCurrentIndex &&
             -1 == iLastSkipDirection )
            {
            skip = EFalse;
            displayError = ETrue;
            }
        else if ( aCurrentIndex == aCount-1 &&
                  1 == iLastSkipDirection )
            {
            skip = EFalse;
            displayError = ETrue;
            }
        }
   if ( EPbRepeatAll == iRepeatMode )
       {
       if ( aCurrentIndex == aCount-1 &&
                1 == iLastSkipDirection )
           {
           skip = EFalse;
           displayError = ETrue;
           }
        }

    // Skip to next/previous track
    if ( skip )
        {
        if ( iLastSkipDirection < 0 )
            {
            iPlaybackUtility->CommandL( EPbCmdPrevious );
            }
        else
            {
            iPlaybackUtility->CommandL( EPbCmdNext );
            }
        }

    // Display error message
    if ( displayError )
        {
        HandleAllTracksInvalidL();
        }

    MPX_DEBUG1("CMPXCommonPlaybackViewImp::SkipOnErrorL: Exiting");
    }

// ---------------------------------------------------------------------------
// Function to display information notes
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DisplayInfoNoteL( TInt aResourceId )
    {
    HBufC* text = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------------------------
// Function to display confirmation notes
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DisplayConfirmionNoteL( TInt aResourceId )
    {
    HBufC* text = StringLoader::LoadLC( aResourceId );
    CAknConfirmationNote* dlg = new ( ELeave ) CAknConfirmationNote( ETrue );
    dlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------------------------
// Set process priority.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::SetProcessPriority()
    {
    RProcess thisProcess;
    TProcessPriority  pPriority = thisProcess.Priority();

    // Check if playback state is not playing, set to background
    TMPXPlaybackState state( EPbStateNotInitialised );
    TRAP_IGNORE( state = iPlaybackUtility->StateL() );

    if ( EPbStatePlaying != state && pPriority == EPriorityHigh )
        {
        MPX_DEBUG1( "CMPXCommonPlaybackViewImp::SetProcessPriority -- setting process priority to EPriorityBackground" );
        thisProcess.SetPriority( EPriorityBackground );
        }
    }

// ---------------------------------------------------------------------------
// Launch equalizer dialog.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::LaunchEqualizerL()
    {

    // Activate Equalizer dialog via View Framework
    iViewUtility->ActivateViewL( TUid::Uid( KMPXPluginTypeEqualizerUid ) );

    iContainer->SetRect( ClientRect() );
    iContainer->DrawDeferred();
    iSwitchingView = EFalse;
    }


// ---------------------------------------------------------------------------
// Launch Music Settings view.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::LaunchMusicSettingsL()
    {
    // Activate Audio Effects view via View Framework
    iViewUtility->ActivateViewL(
        TUid::Uid( KMPXPluginTypeAudioEffectsUid ) );
    }

// ---------------------------------------------------------------------------
// Launch FM Transmitter application
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::LaunchFMTransmitterL()
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::LaunchFMTransmitterL" );

    if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
        {
        TApaTaskList tasList( iCoeEnv->WsSession() );
        TApaTask task = tasList.FindApp( KFmTxAppUid );
        if ( task.Exists() )
            {
            task.BringToForeground();
            }
        else
            {
            RProcess process;
            TApaAppInfo appInfo;
            RApaLsSession session;
            if ( KErrNone == session.Connect() )
                {
                CleanupClosePushL( session );
                TThreadId threadId;
                session.CreateDocument( KNullDesC, KFmTxAppUid, threadId );
                CleanupStack::PopAndDestroy(&session);
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Request for the media object
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::RequestMediaL( TBool aDrm /*=EFalse*/ )
    {
    MPX_DEBUG1("CMPXCommonPlaybackViewImp::RequestMediaL(): entering");

    MMPXSource* s = iPlaybackUtility->Source();
    if ( s )
        {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        attrs.Append( KMPXMediaGeneralBasic |
                      KMPXMediaGeneralUri |
                      KMPXMediaGeneralMimeType |
                      KMPXMediaGeneralDuration );
        attrs.Append( KMPXMediaMusicAlbumArtFileName |
                      KMPXMediaMusicArtist |
                      KMPXMediaMusicAlbum );
        if ( aDrm )
            {
            attrs.Append( KMPXMediaDrmAll );
            }
        else
            {
            // If not retrieving all drm attributes, then
            // minimal is retrieve the protected flag and
            // automated flag
            attrs.Append( KMPXMediaDrmProtected |
                          KMPXMediaDrmCanSetAutomated );
            }
        s->MediaL( attrs.Array(), *this );
        CleanupStack::PopAndDestroy( &attrs );
        }
    MPX_DEBUG1("CMPXCommonPlaybackViewImp::RequestMediaL(): exiting");
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::PrepareStatusPaneForPlaybackViewL
// -----------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewImp::PrepareStatusPaneForPlaybackViewL()
    {
    iPreviousStatusPaneLayout = StatusPane()->CurrentLayoutResId();
    TInt resId( KErrNone );

	// We need to restore the status pane if not visible.
	if (!StatusPane()->IsVisible())
		{
		StatusPane()->MakeVisible(ETrue);
		}

    if ( !Layout_Meta_Data::IsLandscapeOrientation() )
        {
        if ( iPreviousStatusPaneLayout != R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT )
            {
            resId = R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT;
            }
        }

    if ( KErrNone != resId )
        {
        CAknLocalScreenClearer* clearer = CAknLocalScreenClearer::NewLC( ETrue );
        StatusPane()->SwitchLayoutL( resId );
        CleanupStack::PopAndDestroy( clearer );
        }
    else if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::PrepareStatusPaneForExitingViewL
// -----------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewImp::PrepareStatusPaneForExitingViewL()
    {
    MPX_FUNC_EX("CMPXCommonPlaybackViewImp::PrepareStatusPaneForExitingViewL()");
    if ( StatusPane()->CurrentLayoutResId() !=
         iPreviousStatusPaneLayout)
        {
        StatusPane()->SwitchLayoutL( iPreviousStatusPaneLayout );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    MPX_DEBUG2("-->CMPXCommonPlaybackViewImp::HandlePlaybackMessage(): aError = %d", aError);
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    else if ( aError != KErrNone && iLastPBViewActivated )
        {
        TRAP_IGNORE( DoHandleErrorPlaybackMessageL( aError ) );
        }
    MPX_DEBUG2("<--CMPXCommonPlaybackViewImp::HandlePlaybackMessage(): aError = %d", aError);
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    TRAP_IGNORE( DoHandlePropertyL( aProperty, aValue, aError ) );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::HandleSubPlayerNamesL" );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media event.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::HandleMediaL" );
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ));
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionMediaObserver
// Handle extended media properties
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::HandleCollectionMediaL" );
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ));
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleCommandL( TInt aCommand )
    {
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::HandleCommandL(%d) entering", aCommand );

    switch (aCommand)
        {
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(
                iEikonEnv->WsSession(),
                AppUi()->AppHelpContextL() );
            break;
            }
        case EAknSoftkeyBack:
            {
             
            #ifdef BACKSTEPPING_INCLUDED
            // let Back Stepping Service handle the event
            TInt statusInfo( KMPXBackSteppingNotConsumed );
            if ( iViewUtility &&
                ( iViewUtility->ActiveViewType() != KNullUid ) )
                {
                TInt viewId( iViewUtility->ActiveViewType().iUid );
                TBuf8<KMVPrefixLen + KMaxIntLen> buf;
                buf.Copy( KMVPrefix );
                buf.AppendNum( viewId );
                statusInfo = iBackSteppingUtility->HandleBackCommandL( buf );
                MPX_DEBUG3("CMPXCommonPlaybackViewImp::HandleCommandL - viewid=0x%x, statusInfo=%d",
                     viewId, statusInfo );
                }
            if ( statusInfo == KMPXBackSteppingNotConsumed )
            #endif // BACKSTEPPING_INCLUDED
                {
                if ( !iBacking )
                    {
                    // event not consumed by Back Stepping utility, handle here

                    MMPXSource* source = iPlaybackUtility->Source();
                    if ( source )
                        {
                        CMPXCollectionPlaylist* playlist = source->PlaylistL();
                        if ( playlist )
                            {
                            CleanupStack::PushL( playlist );
    						
                            if ( playlist->Count() )
                                {
                                CMPXCollectionPath* pbPath = CMPXCollectionPath::NewL( playlist->Path() );
                                CleanupStack::PushL( pbPath );
                                pbPath->Back();
                                iViewUtility->PushDefaultHistoryL();
                                MPX_DEBUG_PATH(*pbPath);
                                iCollectionUtility->Collection().OpenL( *pbPath );
                                CleanupStack::PopAndDestroy( pbPath );
                                }
                            else
                                {
                                // TO-DO: check if collection is empty, activate
                                // music main menu? or change ui spec
                                AppUi()->HandleCommandL( EAknSoftkeyBack );
                                }
                            CleanupStack::PopAndDestroy( playlist );
                            }
                        else
                            {
                            AppUi()->HandleCommandL( EAknSoftkeyBack );
                            }
                        }
                    else
                        {
                        CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                        CleanupStack::PushL( cpath );
                        while ( cpath->Levels() > 2 )
                            {
                            cpath->Back();
                            }
                        iCollectionUtility->Collection().OpenL( *cpath );
                        CleanupStack::PopAndDestroy( cpath );
                        }
                    iBacking = ETrue;
                    }
                }
            break;
            }
        case EAknCmdExit:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        case EMPXPbvCmdGotoMusicMenu:
            {
            AppUi()->HandleCommandL( EMPXCmdGotoCollection );
            TInt val(0);
            CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
            repository->Get( KMPXMPLocalVariation, val );
            TBool startInAlbumMenu = val&KMPXStartMusicPlayerinArtistsandAlbums ? ETrue : EFalse;
            delete repository;

            if ( startInAlbumMenu )
                {
                CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
                CleanupStack::PushL( cpath );
                cpath->AppendL(3); // Albums
                MPX_DEBUG_PATH(*cpath);
                iCollectionUtility->Collection().OpenL( *cpath );
                CleanupStack::PopAndDestroy( cpath );
                }
            else
                {
                // Handle Goto Music menu
                CMPXCollectionPath* cpath = iCollectionUiHelper->MusicMenuPathL();
                CleanupStack::PushL( cpath );
                MPX_DEBUG_PATH (*cpath);
                iCollectionUtility->Collection().OpenL( *cpath );
                CleanupStack::PopAndDestroy( cpath );
                }
            // Make sure status pane layout is usual when going
            // directly to music menu
            // Status pane has to be modified before view gets deactivated
            break;
            }
        case EMPXPbvCmdPlay:
            {
            if ( iEmbedded )
                {
                iPlaybackUtility->CommandL( EPbCmdDisableEffect );
                }
            iPlaybackUtility->CommandL( EPbCmdPlay );
            break;
            }
        case EMPXPbvCmdStop:
            {
            iPlaybackUtility->CommandL( EPbCmdStop );
            break;
            }
        case EMPXPbvCmdPause:
            {
            MPX_PERF_CHECKPT("Pause command issued");
            iPlaybackUtility->CommandL( EPbCmdPause );
            break;
            }
        case EMPXPbvCmdPlayPause:
            {
            iPlaybackUtility->CommandL( EPbCmdPlayPause );
            break;
            }
        case EMPXPbvCmdSeekForward:
            {
            iSkipBtnPressed = ETrue;
            iCommandSender = ECsRenderer;
            if ( IsCommandSupportedL() )
                {
                iPlaybackUtility->CommandL( EPbCmdStartSeekForward );
                if ( !iKeySoundDisabled )
                    {
                    iAvkonViewAppUi->KeySounds()->PushContextL(
                        R_MPX_PBV_LEFT_RIGHT_KEYS_SILENT );
                    iKeySoundDisabled = ETrue;
                    }
                }
            iUnsupportedNoteDisabled = EFalse;
            iIsffButtonPressed = ETrue;
            break;
            }
        case EMPXPbvCmdSeekBackward:
            {
            iSkipBtnPressed = ETrue;
            iCommandSender = ECsRenderer;
            if ( IsCommandSupportedL() )
                {
                iPlaybackUtility->CommandL( EPbCmdStartSeekBackward );
                if ( !iKeySoundDisabled )
                    {
                    iAvkonViewAppUi->KeySounds()->PushContextL(
                        R_MPX_PBV_LEFT_RIGHT_KEYS_SILENT );
                    iKeySoundDisabled = ETrue;
                    }
                }
            iUnsupportedNoteDisabled = EFalse;
            iIsffButtonPressed = ETrue;
            break;
            }
        case EMPXPbvCmdEndSeek:
            {
            iSkipBtnPressed = EFalse;
            iIsffButtonPressed = EFalse;
			iUnsupportedNoteDisabled = EFalse;
            iPlaybackUtility->CommandL( EPbCmdStopSeeking );
            if ( iKeySoundDisabled )
                {
                iAvkonViewAppUi->KeySounds()->PopContext();
                iKeySoundDisabled = EFalse;
                }
            break;
            }
        case EMPXPbvCmdNextListItem:
            {
            // When user skips a track, reset the start playback index
            iStartPlaybackIndex = KErrNotFound;
            iDelayedErrorTimer->Cancel();

            iPlaybackUtility->CommandL( EPbCmdNext );
            break;
            }
        case EMPXPbvCmdPreviousListItem:
            {
            // When user skips a track, reset the start playback index
            iStartPlaybackIndex = KErrNotFound;
            iDelayedErrorTimer->Cancel();

            iPlaybackUtility->CommandL( EPbCmdPrevious );
            break;
            }
        case EMPXPbvCmdDecreaseVolume:
            {
            iPlaybackUtility->CommandL( EPbCmdDecreaseVolume );
            break;
            }
        case EMPXPbvCmdIncreaseVolume:
            {
            iPlaybackUtility->CommandL( EPbCmdIncreaseVolume );
            break;
            }
         case EMPXPbvCmdPosition:
            {
            iIsTapped = ETrue;
            iCommandSender = ECsRenderer;
            if ( IsCommandSupportedL() && ( iCurrentPlayerType != EPbRemote ) )
                {
            TInt newPosition = iContainer->GetNewSongPosition();
            iPlaybackUtility->SetL( EPbPropertyPosition, newPosition);
                }
            else
                {
                iUnsupportedNoteDisabled = EFalse;
                }
            break;
            }
        case EMPXPbvCmdRepeatOne:
            {
            iPlaybackUtility->SetL( EPbPropertyRepeatMode, EPbRepeatOne );
            break;
            }
        case EMPXPbvCmdRepeatAll:
            {
            iPlaybackUtility->SetL( EPbPropertyRepeatMode, EPbRepeatAll );
            break;
            }
        case EMPXPbvCmdRepeatOff:
            {
            iPlaybackUtility->SetL( EPbPropertyRepeatMode, EPbRepeatOff );
            break;
            }
        case EMPXPbvCmdRandomOn:
            {
            iPlaybackUtility->SetL( EPbPropertyRandomMode, ETrue );
            break;
            }
        case EMPXPbvCmdRandomOff:
            {
            iPlaybackUtility->SetL( EPbPropertyRandomMode, EFalse );
            break;
            }
        case EMPXCmdAfterSaveUseAsCascade:
            {
            CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
            MPX_DEBUG1( "Todd CMPXCommonPlaybackViewImp::HandleCommandL aCommand=EMPXCmdAfterSaveUseAsCascade" );
            FillAiwParametersL(paramList, ETrue);
            iServiceHandler->ExecuteMenuCmdL(EMPXPbvCmdUseAsCascade, paramList,
                iServiceHandler->OutParamListL());
            break;
            }
        // The clip was saved before setting clip as profile ringtone
        case EMPXCmdAfterSaveAiwCmdAssign:
            {
            CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
            MPX_DEBUG1( "Todd CMPXCommonPlaybackViewImp::HandleCommandL aCommand=EMPXCmdAfterSaveAiwCmdAssign" );
            FillAiwParametersL(paramList, ETrue);
            iServiceHandler->ExecuteMenuCmdL(EMPXPbvCmdAiwCmdAssign, paramList,
                iServiceHandler->OutParamListL());
            break;
            }
        case EMPXPbvCmdUseAsCascade:
        case EMPXPbvCmdAiwCmdAssign:
            {
            CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
            FillAiwParametersL(paramList, EFalse);
            iServiceHandler->ExecuteMenuCmdL(aCommand, paramList,
                iServiceHandler->OutParamListL());
            break;
            }
        case EMPXPbvCmdEqualizer:
            {
            LaunchEqualizerL();
            break;
            }
        case EMPXPbvCmdOpenMusicSettings:
            {
            LaunchMusicSettingsL();
            break;
            }

        case EMPXPbvCmdUpnpPlayViaLocal:
            {
            if ( iUpnpFrameworkSupport )
                {
                SelectNewPlayerL( aCommand );
                }
            break;
            }
        case EMPXPbvCmdFMTransmitter:
            {
            LaunchFMTransmitterL();
            break;
            }
        case EMPXPbvCmdAddToSavedPlaylist:
            {
            AddToSavedPlaylistL();
            break;
            }
        case EMPXPbvCmdAddToNewPlaylist:
            {
            AddToNewPlaylistL();
            break;
            }
        case EMPXCmdNext:
        case EMPXCmdPrevious:
            // When user skips a track, reset the start playback index
            iStartPlaybackIndex = KErrNotFound;
        case EMPXCmdPlay:
        case EMPXCmdPause:
        case EMPXCmdPlayPause:
        case EMPXCmdStop:
        case EMPXCmdSeekForward:
        case EMPXCmdSeekBackward:
        case EMPXCmdStopSeeking:
            {
            // check if current view is empty
            TInt count( 0 );
            MMPXSource* s = iPlaybackUtility->Source();
            if ( s )
                {
                CMPXCollectionPlaylist* playlist = s->PlaylistL();
                if ( playlist )
                    {
                    count = playlist->Count();
                    }
                delete playlist;
                }
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
                AppUi()->HandleCommandL( aCommand );
            count = count; // avoid compilation warning.
#else
            if ( count > 0 )
                {
                // only handle command if current playlist is not empty
                AppUi()->HandleCommandL( aCommand );
                }
#endif
            break;
            }
        case EMPXCmdVolumeUp:
        case EMPXCmdVolumeDown:
        case EMPXCmdVolumeMute:
        case EMPXCmdVolumeUnMute:
        case EMPXCmdVolumeChanged:
            {
            //Do not try to update volume if FMTx (transmission) is on.
            if (iFmTxActivity != EFmTxStateActive)
                {           
                AppUi()->HandleCommandL( aCommand );
                }
            break;
            }
        default:
            {
            if ( iUpnpFrameworkSupport )
                {
                if ( aCommand >= EMPXPbvCmdUpnpPlayViaRemotePlayer )
                    {
                    SelectNewPlayerL( aCommand );
                    }
                else
                    {
                    AppUi()->HandleCommandL( aCommand );
                    }
                }
            else
                {
                AppUi()->HandleCommandL( aCommand );
                }
            break;
            }
        }
    MPX_DEBUG1( "CMPXCommonPlaybackViewImp::HandleCommandL() exiting" );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Processes user commands.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::ProcessCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXCommonPlaybackViewImp::ProcessCommandL(%d): entering", aCommand);
    if ( aCommand == EAknCmdExit )
        {
        if ( MenuBar() )
            {
            MenuBar()->StopDisplayingMenuBar();
            }
        HandleCommandL( aCommand );
        }
    CAknView::ProcessCommandL( aCommand );
    MPX_DEBUG1("CMPXCommonPlaybackViewImp::ProcessCommandL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleStatusPaneSizeChange()
    {
    if ( iContainer && !iSwitchingView )
        {
        TUid activeView = iViewUtility->ActiveViewImplementationUid();
        if ( KMPXEqualizerViewImplementationId == activeView )
            {
            iContainer->AdjustOrdinalPosition( KMPXPlaybackViewWindowBackground );
            }
        else
            {
            iContainer->SetRect( ClientRect() );
            iContainer->DrawDeferred();
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid /* aCustomMessageId */,
    const TDesC8& /* aCustomMessage */ )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::DoActivateL()" );

    if( aPrevViewId.iAppUid == KAppUidMusicPlayerX && iContainer )
        {
        // record the begin state for the transition animation.
        iContainer->BeginTransition();
        }
            
    iSwitchingView = EFalse;
    iDatabaseNotReady = EFalse;
	iUnsupportedNoteDisabled = EFalse;
    delete iOldUri;
    iOldUri = NULL;
    CAknToolbar* toolbar = Toolbar();
    if ( toolbar )
        {
        AppUi()->AddToStackL(toolbar,
        		ECoeStackPriorityCba,
                ECoeStackFlagRefusesFocus
                | ECoeStackFlagRefusesAllKeys );
        toolbar->MakeVisible( ETrue );
        toolbar->HideItemsAndDrawOnlyBackground(EFalse);
        toolbar->SetToolbarVisibility(ETrue);
        }
    PrepareStatusPaneForPlaybackViewL();

    iRandomMode = KErrNotFound;
    iRepeatMode = KErrNotFound;

    iBacking = EFalse;
    // Update view
    // use idle call back since extracting album art binary data is slow
    if ( iIdle )
        {
        if ( iIdle->IsActive() )
            {
            MPX_DEBUG1( "CMPXCommonPlaybackViewImp::DoActivateL() iIdle active, canceling" );
            iIdle->Cancel();
            }
        delete iIdle;
        iIdle = NULL;
        }
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
    iIdle->Start( TCallBack( CMPXCommonPlaybackViewImp::DeferredAlbumArtExtractCallback, this ) );

    // Retrieve current repeat & random modes, fix for EJZU-7NZ9CD
    iPlaybackUtility->PropertyL( *this, EPbPropertyRandomMode );
    iPlaybackUtility->PropertyL( *this, EPbPropertyRepeatMode );

    // Set title
    // Ignore errors from updating title pane since if that is not
    // updated, still want to activate view
    TRAP_IGNORE(
        {
        UpdateTitlePaneL();
        CAknNavigationControlContainer* naviPane(
            static_cast<CAknNavigationControlContainer*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ))));
        if ( naviPane )
            {
            naviPane->PushDefaultL();
            }
        } );

    CEikButtonGroupContainer* cba = Cba();
    if ( cba )
        {

#ifdef __ENABLE_MSK
        iCommonUiHelper->SetMiddleSoftKeyLabelL(
            *cba,
            R_TEXT_SOFTKEY_EMPTY,
            EAknSoftkeyForwardKeyEvent );
#endif // __ENABLE_MSK
        }

    iPlaybackState = iPlaybackUtility->StateL();
    MPX_DEBUG2("CMPXCommonPlaybackViewImp::DoActivateL(): state = %d", iPlaybackState);
    if ( iPlaybackState == EPbStateInitialising )
        {
        delete iMedia;
        iMedia = NULL;
        UpdatePlaybackState( iPlaybackState );
        UpdateTrackInfoL( iMedia );
        }
    else
        {
        // reset iNewItemOpened when Playback view is activated when state is
        //  not initializing (e.g. user pressed Go to Now Playing view)
        MPX_DEBUG1("CMPXCommonPlaybackViewImp::DoActivateL() reset iNewItemOpened");
#ifndef CAMESE_IN_DRM_UTILITY
        if (iCameseSuperDistSupport)
            {
            iNewItemOpened = EFalse;
            }
#endif
        if ( iMedia == NULL )
            {
            MMPXSource* s = iPlaybackUtility->Source();
            if (  s )
                {
                CMPXCollectionPlaylist* playlist( s->PlaylistL() );
                if ( playlist )
                    {
                    CleanupStack::PushL( playlist );
                    if ( playlist->Count() > 0 )
                        {
                        RequestMediaL();
                        UpdatePlaybackState( iPlaybackState );
                        }
                    else
                        {
                        // playlist count is 0, no tracks
                        iPlaybackState = EPbStateNotInitialised;
                        }
                    CleanupStack::PopAndDestroy( playlist );
                    }
                else
                    {
                    // If empty playlist, then no tracks
                    iPlaybackState = EPbStateNotInitialised;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DoDeactivate()
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::DoDeactivate()" );
    iMPXUtility->CancelRequest();

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        iBacking = ETrue;
        }
    // Hide the toolbar when view is deactivated. Otherwise it may interfere
    // with other views (e.g. Antriksh based collection view) that doesn't have
    // toolbar to not draw correctly.
    CAknToolbar* toolbar = Toolbar();
    if ( toolbar )
        {
        toolbar->HideItemsAndDrawOnlyBackground(ETrue);
        toolbar->SetToolbarVisibility(EFalse);
        toolbar->MakeVisible(EFalse);
        AppUi()->RemoveFromStack( toolbar );
        }
    
    if ( iTNRequestTimer->IsActive())
        {
        iTNRequestTimer->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleForegroundEventL( TBool aForeground )
    {
    MPX_DEBUG2( "--->CMPXCommonPlaybackViewImp::HandleForegroundEventL(%d)", aForeground );

#ifdef BACKSTEPPING_INCLUDED
    TInt statusInfo( KMPXBackSteppingNotConsumed );
    MPX_DEBUG3("CMPXCommonPlaybackViewImp::HandleForegroundEventL - is in foreground=%d, this view=0x%x",
        aForeground, Id().iUid );
    if( aForeground &&
        iViewUtility &&
        ( iViewUtility->ActiveViewImplementationUid() != KNullUid ) )
        {
        TInt viewId( iViewUtility->ActiveViewType().iUid );
        TBuf8<KMVPrefixLen + KMaxIntLen> buf;
        buf.Copy( KMVPrefix );
        buf.AppendNum( viewId );

        TRAP_IGNORE(
            statusInfo =
                iBackSteppingUtility->ForwardActivationEventL( buf, ETrue );
            );
        MPX_DEBUG3("CMPXCommonPlaybackViewImp::HandleForegroundEventL - viewId=0x%x, statusInfo=%d", viewId, statusInfo );
        }
#endif // BACKSTEPPING_INCLUDED

        // Reset state when view's visibility is changed.
        iSwitchingView = EFalse;

        CAknView::HandleForegroundEventL( aForeground );
        if ( iContainer )
            {
            iContainer->HandleForegroundEventL( aForeground );
            if ( aForeground )
                {
                if ( iMedia )
                    {
                    MPX_DEBUG1( "CMPXCommonPlaybackViewImp::HandleForegroundEventL iMedia valid" );
                    MMPXSource* s = iPlaybackUtility->Source();
                    if ( s && iMedia->IsSupported( KMPXMediaGeneralId ) )
                        {
                        CMPXCollectionPlaylist* playlist = s->PlaylistL();
                        if ( playlist )
                            {
                            MPX_DEBUG1( "CMPXCommonPlaybackViewImp::HandleForegroundEventL playlist valid" );
                            const CMPXCollectionPath& path = playlist->Path();
                            TMPXItemId id( path.Id() );
                            delete playlist;
                            TMPXItemId cacheId(
                                iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                            if ( id != cacheId )
                                {
                                MPX_DEBUG1( "CMPXCommonPlaybackViewImp::HandleForegroundEventL requesting media" );
                                RequestMediaL();
                                UpdateTitlePaneL();
                                }
                            }
                        }
                    }

                iContainer->StartMarquee( ETextTrack );
                iContainer->StartMarquee( ETextArtist );
                iContainer->DrawNow();
                }
            }

    // to stop seek when music player goes to background.
    if ( iContainer && !aForeground && iIsffButtonPressed )
        {
        iIsffButtonPressed = EFalse;
        iContainer->RestoreButtons( iPlaybackState );
        iPlaybackUtility->CommandL( EPbCmdStopSeeking );
        }

    MPX_DEBUG1( "<---CMPXCommonPlaybackViewImp::HandleForegroundEventL()" );
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    TInt usbUnblockingStatus;
    RProperty::Get( KMPXViewPSUid,
                    KMPXUSBUnblockingPSStatus,
                    usbUnblockingStatus);

    switch ( aResourceId )
        {
        case R_MPX_PLAYBACK_VIEW_MENU:
            {
            if ( iIsffButtonPressed )
                {
                iIsffButtonPressed = EFalse;
                iContainer->RestoreButtons( iPlaybackState );
                iPlaybackUtility->CommandL( EPbCmdStopSeeking );
                }
            if ( !iMedia )
                {
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdRandom,
                    ETrue );
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdRepeat,
                    ETrue );
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdFMTransmitter,
                    ETrue );
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdUpnpPlayVia,
                    ETrue );
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdEqualizer,
                    ETrue );
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdOpenMusicSettings,
                    ETrue );
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdAddToPlaylist,
                    ETrue );
                aMenuPane->SetItemDimmed(
                    EMPXPbvCmdUseAsCascade,
                    ETrue );
                }
            else
                {
                if ( KErrNotFound == iRandomMode && iEmbedded )
                    {
                    aMenuPane->SetItemDimmed(
                        EMPXPbvCmdRandom,
                        ETrue );
                    }
                if ( KErrNotFound == iRepeatMode && iEmbedded )
                    {
                    aMenuPane->SetItemDimmed(
                        EMPXPbvCmdRepeat,
                        ETrue );
                    }

                TBool addToPlDimmed( ETrue );
                MMPXSource* s( iPlaybackUtility->Source() );
                if ( s )
                    {
                    CMPXCollectionPlaylist* playlist( s->PlaylistL() );
                    if ( playlist )
                        {
                        CleanupStack::PushL( playlist );
                        TInt count( playlist->Count() );
                        if ( count > 0 )
                            {
                            addToPlDimmed = EFalse;
                            }
                        CleanupStack::PopAndDestroy( playlist );
                        }
                    }
                if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
                    {
                    aMenuPane->SetItemDimmed( EMPXPbvCmdAddToPlaylist, ETrue );
                    }
                else
                    {
                    aMenuPane->SetItemDimmed( EMPXPbvCmdAddToPlaylist, addToPlDimmed );
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
                    aMenuPane->SetItemDimmed(
                        EMPXPbvCmdUseAsCascade,
                    !iMedia || isFormatNotSupported ||
                        iPlaybackState == EPbStateInitialising );

                // if help is not enabled, do not show "help"
                if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                    {
                    aMenuPane->SetItemDimmed(
                        EAknCmdHelp,
                        ETrue );
                    }

                // Check if FM Transmitter is supported
                if ( !FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
					{
					aMenuPane->SetItemDimmed(
						EMPXPbvCmdFMTransmitter,
						ETrue );
					}


                if ( iUpnpFrameworkSupport )
                    {
                    if ( !iMedia || !IsUpnpVisible() )
                        {
                        GetCurrentPlayerDetails();
                        if ( iCurrentPlayerType == EPbLocal )
                            {
                            aMenuPane->SetItemDimmed(
                                EMPXPbvCmdUpnpPlayVia,
                                ETrue);
                            }
                        }
                    }
                else
                    {
                    aMenuPane->SetItemDimmed( EMPXPbvCmdUpnpPlayVia, ETrue);
                    }
            aMenuPane->SetItemDimmed( EAknCmdExit, iExitOptionHidden );
                if (iServiceHandler->HandleSubmenuL(*aMenuPane))
                    {
                    return;
                    }
                }
            break;
            }
        case R_MPX_PLAYBACK_PLAY_VIA_SUB_MENU:
            {
            if ( iUpnpFrameworkSupport )
                {
                AddPlayersNamesToMenuL( *aMenuPane );
                }
            break;
            }
        case R_MPX_PLAYBACK_REPEAT_SUB_MENU:
            {
            TInt symbolOff = EEikMenuItemSymbolIndeterminate;
            TInt symbolAll = EEikMenuItemSymbolIndeterminate;
            TInt symbolOne = EEikMenuItemSymbolIndeterminate;

            if ( EPbRepeatOff == iRepeatMode )
                {
                symbolOff = EEikMenuItemSymbolOn;
                }
            else if ( EPbRepeatAll == iRepeatMode )
                {
                symbolAll = EEikMenuItemSymbolOn;
                }
            else
                {
                symbolOne = EEikMenuItemSymbolOn;
                }

            aMenuPane->SetItemButtonState(
                EMPXPbvCmdRepeatOff,
                symbolOff );
            aMenuPane->SetItemButtonState(
                EMPXPbvCmdRepeatAll,
                symbolAll );
            aMenuPane->SetItemButtonState(
                EMPXPbvCmdRepeatOne,
                symbolOne );
            break;
            }
        case R_MPX_PLAYBACK_RANDOM_SUB_MENU:
            {
            TInt symbolOff = EEikMenuItemSymbolOn;
            TInt symbolOn = EEikMenuItemSymbolIndeterminate;

            if ( iRandomMode )
                {
                symbolOff = EEikMenuItemSymbolIndeterminate;
                symbolOn = EEikMenuItemSymbolOn;
                }

            aMenuPane->SetItemButtonState(
                EMPXPbvCmdRandomOn,
                symbolOn );
            aMenuPane->SetItemButtonState(
                EMPXPbvCmdRandomOff,
                symbolOff );
            break;
            }
        case R_MPX_PBV_ADD_TO_PL_SUB_MENU:
            {
            TBool dim( ETrue );
            delete iUserPlaylists;
            iUserPlaylists = NULL;
            iUserPlaylists = iCommonUiHelper->FindPlaylistsL();
            if ( iUserPlaylists )
                {
                const CMPXMediaArray* mediaArray =
                    iUserPlaylists->Value<CMPXMediaArray>(
                    KMPXMediaArrayContents );
                User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));

                TInt entriesCount( mediaArray->Count() );
                MPX_DEBUG2( "CMPXCommonPlaybackViewImp::DynInitMenuPaneL Entry count = %d", entriesCount );
                if ( entriesCount > 0 )
                    {
                    dim = EFalse;
                    }
                }
            aMenuPane->SetItemDimmed( EMPXPbvCmdAddToSavedPlaylist, dim );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    if ( iServiceHandler->IsAiwMenu(aResourceId))
        {
        MPX_DEBUG1( "CMPXCollectionViewImp::DynInitMenuPaneL(): Aiw menu for assign" );
        CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
        FillAiwParametersL(paramList, EFalse);
        iServiceHandler->InitializeMenuPaneL(*aMenuPane,
                                            aResourceId,
                                            EMPXPbvCmdAiwCmdAssign,
                                            paramList);
        MPX_DEBUG1( "CMPXCollectionViewImp::DynInitMenuPaneL(): Aiw menu for assign end" );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXLayoutSwitchObserver
// Callback function when layout is changed.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleLayoutChange()
    {
    if (iContainer && !iSwitchingView)
        {
        if ( !Layout_Meta_Data::IsLandscapeOrientation() )
            {
            TUid activeView = iViewUtility->ActiveViewImplementationUid();

            // only switch to flat if current view is not equalizer
            // since it'll mess up equilizer's screen
            if ( StatusPane()->CurrentLayoutResId() !=
                 R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT &&
                 activeView != KMPXEqualizerViewImplementationId )
                {
                TRAP_IGNORE(
                    StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT ));
                }
            }
        else
            {
            if ( StatusPane()->CurrentLayoutResId() !=
                 R_AVKON_STATUS_PANE_LAYOUT_USUAL )
                {
                TRAP_IGNORE(
                    StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL ));
                }
            }

        iContainer->SetRect( ClientRect() );
        delete iOldUri;
        iOldUri = NULL;
        // Update album art
        TRAP_IGNORE( UpdateAlbumArtL( iMedia ));
        TRAP_IGNORE( UpdateTrackInfoL( iMedia ));
        iContainer->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// From MCoeViewDeactivationObserver
// Handles the activation of the view aNewlyActivatedViewId before
// the old view aViewIdToBeDeactivated is to be deactivated.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleViewActivation(
    const TVwsViewId& aNewlyActivatedViewId,
    const TVwsViewId& aViewIdToBeDeactivated )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::HandleViewActivation AV" );
    // Check if this view is going to be deactivated
    if ( aViewIdToBeDeactivated.iViewUid == Id() )
        {
        if ( aNewlyActivatedViewId.iViewUid != Id() )
            {
            iSwitchingView = ETrue;
            }
        }

    if ( iPBViewToBeActivated )
        {
        iLastPBViewActivated = ( aNewlyActivatedViewId.iViewUid == Id() );
        MPX_DEBUG2( "CMPXCommonPlaybackViewImp::HandleViewActivation iLastPBViewActivated %d", iLastPBViewActivated );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXViewActivationObserver
// Handle view activation.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleViewActivation(
    const TUid& aCurrentViewType,
    const TUid& /*aPreviousViewType*/ )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::HandleViewActivation VF" );
    iPBViewToBeActivated = ( KMPXPluginTypePlaybackUid == aCurrentViewType.iUid );
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::HandleViewActivation iPBViewToBeActivated %d", iPBViewToBeActivated );
    }
// ---------------------------------------------------------------------------
// From MMPXViewActivationObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleViewUpdate(
    TUid /* aViewUid */,
    MMPXViewActivationObserver::TViewUpdateEvent /* aUpdateEvent */,
    TBool /* aLoaded */,
    TInt /* aData */)
    {
    // Do nothing, this should be handled by the AppUI
    }

// ---------------------------------------------------------------------------
// From MMPXPSKeyObserver
// Handle PS event
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandlePSEvent(
    TUid aUid,
    TInt aKey )
    {
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::HandlePSEvent %d", aKey );

   	if ( aUid == KPSUidHWRMFmTx && aKey == KHWRMFmTxStatus )
       	{
       	// only responds to FM TX key value change
       	TRAP_IGNORE( UpdateFMTransmitterInfoL() );
       	}
    }

// ---------------------------------------------------------------------------
// From MMPXCenRepObserver
// Handle PS event
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleSettingChange(
    const TUid& aRepositoryUid,
    TUint32 aSettingId )
    {
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::HandleSettingChange %d", aSettingId );

	if ( aRepositoryUid == KCRUidFmTxCenRes &&
		 aSettingId == KFmTxCenResKeyFrequency )
		{
		// only responds to FM Tx Frequency value change
		TRAP_IGNORE( UpdateFMTransmitterInfoL() );
		}
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::IsUpnpVisible
// Taken from Gallery upnp support implementation
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonPlaybackViewImp::IsUpnpVisible()
    {
    MPX_FUNC( "CMPXCommonPlaybackViewImp::IsUpnpVisible" );
    TBool returnValue = EFalse;

#ifdef UPNP_INCLUDED
   	if ( iUpnpCopyCommand && iUpnpFrameworkSupport )
        {
        returnValue = iUpnpCopyCommand->IsAvailableL();
        }
#endif
    return returnValue;

    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::AddPlayersNamesToMenuL
// Add all available players names to the Play via submenu
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::AddPlayersNamesToMenuL(
    CEikMenuPane& aMenuPane )
    {
    if ( iUpnpFrameworkSupport )
        {
        RArray<TMPXPlaybackPlayerType> playerTypes;
        CleanupClosePushL(playerTypes);

        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        manager.GetPlayerTypesL(playerTypes);

        GetCurrentPlayerDetails();

        TInt countPlayersTypes( playerTypes.Count() );
        TInt countLocalType( 0 );
        TInt countRemoteType( 0 );

        if ( countPlayersTypes >= 1 )
            {
            iPlayersList.Close();

            for ( TInt i = 0; i < countPlayersTypes; i++)
                {
                MPX_DEBUG3( "playerTypes[%d]: %d", i, playerTypes[i] );
                switch (playerTypes[i])
                    {
                    case EPbLocal:
                        {
                        // we only show 1 specific type once in the menu
                        if ( countLocalType == 0 )
                            {
                            AddPlayerNameToMenuL( aMenuPane,
                                                EMPXPbvCmdUpnpPlayViaLocal,
                                                manager,
                                                playerTypes[i] );

                            countLocalType++;
                            }
                        break;
                        }
                    case EPbRemote:
                        {
                        // we only show 1 specific type once in the menu
                        if ( countRemoteType == 0 )
                            {
                            AddPlayerNameToMenuL( aMenuPane,
                                                EMPXPbvCmdUpnpPlayViaRemotePlayer,
                                                manager,
                                                playerTypes[i] );
                            countRemoteType++;
                            }
                        break;
                        }
                    default:
                        {
                        // default case is handled in the next loop, we want
                        // to add any "other" player at the end of the list
                        break;
                        }
                    }
                } // for loop

            for ( TInt i = 0; i < countPlayersTypes; i++)
                {
                MPX_DEBUG3( "playerTypes[%d]: %d", i, playerTypes[i] );
                // EPbLocal and EPbRemote were already added to the
                // submenu in the previous loop
                if ( playerTypes[i] != EPbLocal &&
                     playerTypes[i] != EPbRemote )
                    {
                    // EPbUnknown or else
                    TBool alreadyInTheList( EFalse );

                    for ( TInt j = 0; j < i; j++)
                        {
                        // if the new playertype is already in the list
                        if ( playerTypes[i] == playerTypes[j] )
                            {
                            alreadyInTheList = ETrue;
                            break;
                            }
                        }

                    if ( !alreadyInTheList )
                        {
                        HBufC* buf = manager.PlayerTypeDisplayNameL(playerTypes[i]);

                        if ( buf )
                            {
                            CleanupStack::PushL(buf);
                            AddPlayerNameToMenuL( aMenuPane,
                                                EMPXPbvCmdUpnpPlayViaRemotePlayer,
                                                manager,
                                                playerTypes[i],
                                                *buf );
                            CleanupStack::PopAndDestroy(buf);
                            }
                        // else we don't do anything. other player type is not
                        // currently supported. Implementation will be finalized
                        // when the requirement is defined.
                        }
                    }
                }
            }

        CleanupStack::PopAndDestroy(&playerTypes);

        if ( iPlayersList.Count() <= 1 )
            {
            aMenuPane.SetItemDimmed( EMPXPbvCmdUpnpPlayViaRemotePlayer,
                                      ETrue );
            }
        }
    else
        {
        // Remove compilation warnings
        (void)aMenuPane;
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::AddPlayerNameToMenuL
// Add 1 player name to the submenu
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::AddPlayerNameToMenuL(
    CEikMenuPane& aMenuPane,
    TInt aCommandId,
    MMPXPlayerManager& aPlayerManager,
    TMPXPlaybackPlayerType& aPlayerType,
    const TDesC& aMenuText )
    {
    if ( iUpnpFrameworkSupport )
        {
        RArray<TUid> players;
        CleanupClosePushL(players);
        aPlayerManager.GetPlayerListL(players, aPlayerType);

        // For now we only keep the first player we find
        // Local player is always the first one in the list
        // Ui spec limitation
        MPX_DEBUG2( "players[0]: %d", players[0].iUid );
        if ( aCommandId == EMPXPbvCmdUpnpPlayViaLocal )
            {
            iPlayersList.Insert(players[0], 0);
            }
        else if ( ( aCommandId == EMPXPbvCmdUpnpPlayViaRemotePlayer ) &&
                  (iPlayersList.Count() > 1) )
            {
            iPlayersList.Insert(players[0], 1);
            }
        else
            {
            iPlayersList.AppendL(players[0]);
            }
        CleanupStack::PopAndDestroy(&players);

        // if the player is of unknown type
        if ( ( aCommandId != EMPXPbvCmdUpnpPlayViaRemotePlayer ) &&
             ( aCommandId != EMPXPbvCmdUpnpPlayViaLocal ) )
            {
            CEikMenuPaneItem::SData menuItem;
            menuItem.iText.Copy( aMenuText );
            menuItem.iCascadeId = 0;
            menuItem.iFlags = EEikMenuItemRadioEnd;

            menuItem.iCommandId = aCommandId + (iPlayersList.Count() - 1);
            aCommandId = menuItem.iCommandId;

            aMenuPane.AddMenuItemL( menuItem );
            }

        if ( iCurrentPlayerType == aPlayerType )
            {
            aMenuPane.SetItemButtonState( aCommandId, EEikMenuItemSymbolOn );
            }
        }
    else
        {
        // Remove compilation warnings
        (void)aMenuPane;
        (void)aCommandId;
        (void)aPlayerManager;
        (void)aPlayerType;
        (void)aMenuText;
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::GetCurrentPlayerDetails
// Retrieves the current player name and type
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::GetCurrentPlayerDetails()
    {
    if ( iUpnpFrameworkSupport )
        {

        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        TUid currentlyUsedPlayer;
        TInt currentlyUsedSubPlayer;
        iCurrentPlayerType = EPbLocal;
        delete iSubPlayerName;
        iSubPlayerName = NULL;

        TRAP_IGNORE( manager.GetSelectionL( iCurrentPlayerType,
                                            currentlyUsedPlayer,
                                            currentlyUsedSubPlayer,
                                            iSubPlayerName) );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::SelectNewPlayerL
// Selects a new player for audio playback
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::SelectNewPlayerL(
    TInt aCommand )
    {
    if ( iUpnpFrameworkSupport )
        {
        MPX_FUNC_EX( "CMPXPlaybackViewImp::SelectNewPlayerL()" );
        MPX_DEBUG2( "CMPXPlaybackViewImp::SelectNewPlayerL(%d)", aCommand );

        TInt errorSelectPlayer( KErrCancel );

        MMPXSource* source = iPlaybackUtility->Source();
        CMPXCollectionPlaylist* playlist = NULL;
        if ( source )
            {
            playlist = source->PlaylistL();
            if ( playlist )
                {
                CleanupStack::PushL( playlist );
                iPlaybackUtility->CommandL(EPbCmdPreserveState);                
                }
            }

        switch (aCommand)
            {
            case EMPXPbvCmdUpnpPlayViaLocal:
                {
                // if Local is not already the current player, select it
                if ( iCurrentPlayerType != EPbLocal )
                    {
                    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
                    MPX_TRAP( errorSelectPlayer, manager.ClearSelectPlayersL() );
                    }
                break;
                }
            default:
                {
                if ( aCommand >= EMPXPbvCmdUpnpPlayViaRemotePlayer )
                    {
                    HBufC* buf = HBufC::NewLC( KMaxUidName );
                    buf->Des().AppendNum(
                         (iPlayersList[aCommand -
                                       EMPXPbvCmdUpnpPlayViaLocal]).iUid );

                    // ActivateViewL leaves with KErrCancel if the dialog is
                    // cancelled
                    // ActivateViewL leaves with KErrInUse if the selected player
                    // is already the active player
                    MPX_TRAP(errorSelectPlayer, iViewUtility->ActivateViewL(
                                    TUid::Uid( KMPXPluginTypeUPnPBrowseDialogUid ),
                                    buf ));
                    CleanupStack::PopAndDestroy( buf );
                    }
                break;
                }
            }

        // A new player was successfully selected, playback view changes
        if ( errorSelectPlayer == KErrNone )
            {
            if ( playlist )
                {
                iViewUtility->PushDefaultHistoryL();
                iPlaybackUtility->InitL( *playlist, ETrue );
                }
            }

        if (playlist)
            {
            CleanupStack::PopAndDestroy( playlist );
            }
        }
    else
        {
        // Remove compilation warnings
        (void)aCommand;
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::AddToSavedPlaylistL
// Adds current item to a saved playlist
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::AddToSavedPlaylistL()
    {
    MPX_DEBUG1("CMPXPlaybackViewImp::AddToSavedPlaylistL(): entering");

    ASSERT( iMedia );
    if ( !iUserPlaylists )
        {
        iUserPlaylists = iCommonUiHelper->FindPlaylistsL();
        }

    CMPXMedia* media = PrepareMediaForPlaylistLC( *iMedia );
    MPX_TRAPD( err, iCommonUiHelper->AddToSavedPlaylistL(
        *iUserPlaylists,
        *media,
        this,
        this ) );
    CleanupStack::PopAndDestroy( media );
    if ( KErrNone != err )
        {
        HandleErrorL( err );
        }
    delete iUserPlaylists;
    iUserPlaylists = NULL;

    MPX_DEBUG1("CMPXPlaybackViewImp::AddToSavedPlaylistL(): exiting");
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::AddToNewPlaylistL
// Adds current item to a new playlist
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::AddToNewPlaylistL()
    {
    MPX_DEBUG1("CMPXPlaybackViewImp::AddToNewPlaylistL(): entering");

    if ( iMedia )
        {
        CMPXMedia* media( PrepareMediaForPlaylistLC( *iMedia ));
        iCommonUiHelper->CreatePlaylistL(
            *media,
            this,
            this );
        CleanupStack::PopAndDestroy( media );
        }
    delete iUserPlaylists;
    iUserPlaylists = NULL;

    MPX_DEBUG1("CMPXPlaybackViewImp::AddToNewPlaylistL(): exiting");
    }

// ---------------------------------------------------------------------------
// PrepareMediaForPlaylistLC
// Prepares media for adding to a playlist
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXMedia* CMPXCommonPlaybackViewImp::PrepareMediaForPlaylistLC(
    const CMPXMedia& aMedia )
    {
    MPX_DEBUG1("CMPXPlaybackViewImp::PrepareMediaForPlaylistLC() entering");

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediaArray );
    CMPXMedia* entry = CMPXMedia::NewL( aMedia );
    CleanupStack::PushL( entry );
    mediaArray->AppendL( entry );
    CleanupStack::Pop( entry );
    media->SetCObjectValueL(
        KMPXMediaArrayContents,
        mediaArray );
    media->SetTObjectValueL(
        KMPXMediaArrayCount,
        mediaArray->Count() );
    CleanupStack::PopAndDestroy( mediaArray );
    MPX_DEBUG1("CMPXPlaybackViewImp::PrepareMediaForPlaylistLC() exiting");
    return media;
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::IsCommandSupportedL
// Check if the command is supported by the remote player
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMPXCommonPlaybackViewImp::IsCommandSupportedL()
    {
    TBool forwardCommand = ETrue;
    if ( iUpnpFrameworkSupport && IsUpnpVisible() )
        {
        // retrieve iCurrentPlayerType info
        GetCurrentPlayerDetails();

        if ( iCurrentPlayerType != EPbLocal)
            {
            // show error note, only in first key press
		    if( !iUnsupportedNoteDisabled )
			    {
			    iUnsupportedNoteDisabled = ETrue;
			    iOldPosition = iPosition;
			    if( iContainer && iIsTapped )
			        {
			        iContainer->SetNewSongPosition( iPosition * KMPXOneSecInMilliSecs );
			        }
			    HBufC* dialogText = NULL;
			    TBool setText = EFalse;
                switch ( iCommandSender )
                    {
                    case ECsMediaKey:
                        {
                        dialogText = StringLoader::LoadLC( R_MPX_MEDIA_KEYS_NOT_SUPPORTED );
                        setText = ETrue;
                        break;
                        }
                    case ECsRenderer:
                        {
                        dialogText = StringLoader::LoadLC( R_MPX_REQUEST_REJECTED );
                        setText = ETrue;
                        break;
                        }
                    default:
                        {
                        break;
                        }
                    }

                if( setText )
                    {
                CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                errNote->ExecuteLD( *dialogText );
                CleanupStack::PopAndDestroy( dialogText );
                forwardCommand = EFalse;
                    }
			    }
            }
        }
    return forwardCommand;
    }

// ---------------------------------------------------------------------------
// From MProgressDialogCallback
// Callback method. Get's called when a dialog is dismissed
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::DialogDismissedL( TInt aButtonId )
    {
    MPX_DEBUG2("--->CMPXCommonPlaybackViewImp::DialogDismissedL(%d)", aButtonId);
    switch (aButtonId)
        {
        case EAknSoftkeyCancel:
        case EAknSoftkeyDone:
            {
            break;
            }
        default:
            {
            break;
            }
        }
    MPX_DEBUG1("<---CMPXCommonPlaybackViewImp::DialogDismissedL()");
    }

// ---------------------------------------------------------------------------
// From MMPXCHelperObserver
// Callback method. Get's called when a dialog is dismissed
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleOperationCompleteL( TCHelperOperation /*aOperation*/,
                                                                   TInt aError,
                                                                   void* aArgument )
    {
    MPX_DEBUG2("CMPXPlaybackViewImp::HandleAddCompletedL(%d) entering", aError);

    CMPXMedia* playlist = (CMPXMedia*) aArgument;
    CleanupStack::PushL( playlist );

    iCommonUiHelper->DismissWaitNoteL();
    if ( KErrNone == aError )
        {
        const TDesC& title( playlist->ValueText( KMPXMediaGeneralTitle ) );
        HBufC* confirmTxt( StringLoader::LoadLC(
            R_MPX_QTN_MUS_NOTE_TRACK_ADDED_TO_PL,
            title ));
        CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote();

        TMPlayerSecondaryDisplayNote noteId( EMPlayerNoteAddSongToPlaylist );
        note->PublishDialogL(
            noteId,
            KMPlayerNoteCategory );
        CAknMediatorFacade* covercl( AknMediatorFacade( note ) );
        if ( covercl )
            {
            covercl->BufStream() << title;
            }

        note->ExecuteLD( *confirmTxt );
        CleanupStack::PopAndDestroy( confirmTxt );
        }
    else
        {
        HandleErrorL( aError );
        }
    CleanupStack::PopAndDestroy( playlist );
    MPX_DEBUG1("CMPXPlaybackViewImp::HandleAddCompletedL() exiting");
    }

// -----------------------------------------------------------------------------
// From MMPXCollectionObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleCollectionMessage(CMPXMessage* aMsg,
                                                                 TInt aErr )
    {
    TRAP_IGNORE( DoHandleCollectionMessageL( aMsg, aErr ) );
    }

// -----------------------------------------------------------------------------
// From MMPXCollectionObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleOpenL(const CMPXMedia& /*aEntries*/, TInt /*aIndex*/,
                                                     TBool /*aComplete*/, TInt /*aError*/)
    {
    // Do Nothing
    }

// -----------------------------------------------------------------------------
// From MMPXCollectionObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleOpenL(const CMPXCollectionPlaylist& /*aPlaylist*/,
                                                     TInt /*aError*/)
    {
    // Do Nothing
    }

// -----------------------------------------------------------------------------
// From MMPXCollectionObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::HandleCommandComplete(CMPXCommand* /*aCommandResult*/,
                                                               TInt /*aError*/)
    {
    // Do Nothing
    }

// ----------------------------------------------------------------------------
// void CMPXCommonPlaybackViewImp::DeferredAlbumArtExtractCallback
// call back function for deferred view updates.
// ----------------------------------------------------------------------------
//
TInt CMPXCommonPlaybackViewImp::DeferredAlbumArtExtractCallback( TAny* aPtr )
    {
    MPX_FUNC( "CMPXCommonPlaybackViewImp::DeferredAlbumArtExtractCallback" );
    CMPXCommonPlaybackViewImp* self =
        static_cast<CMPXCommonPlaybackViewImp*>( aPtr );
    TRAP_IGNORE( self->UpdateViewL() );
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::DelayedExit
// call back function for delayed view deactivation
//----------------------------------------------------------------------------
//
TInt CMPXCommonPlaybackViewImp::DelayedExit( TAny* aPtr )
    {
    MPX_FUNC( "CMPXCommonPlaybackViewImp::DelayedExit" );
    CMPXCommonPlaybackViewImp* self =
        static_cast<CMPXCommonPlaybackViewImp*>( aPtr );
    TRAP_IGNORE( self->HandleCommandL(EAknSoftkeyBack) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handle playback error message.
// ---------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewImp::DoHandleErrorPlaybackMessageL( TInt aError )
    {
    MPX_DEBUG2( "CMPXCommonPlaybackViewImp::DoHandleErrorPlaybackMessage(): Handle the error %d", aError);

    // Request media to handle error. We need to do that in
    // order to request DRM details.
    iDelayedError = aError;
    RequestMediaL( ETrue );
    }

// ---------------------------------------------------------------------------
// Handle collection message
// ---------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewImp::DoHandleCollectionMessageL(
    CMPXMessage* aMsg, TInt /*aErr*/ )
    {
    MPX_FUNC_EX( "CMPXCommonPlaybackViewImp::DoHandleCollectionMessageL" );
    if( aMsg )
        {
        TMPXMessageId id( aMsg->ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
        if ( KMPXMessageGeneral == id )
            {
            TInt event( aMsg->ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
            TInt op( aMsg->ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
            TInt data( aMsg->ValueTObjectL<TInt>( KMPXMessageGeneralData ) );

            if( event == TMPXCollectionMessage::EBroadcastEvent &&
                op == EMcMsgDiskRemoved &&
                iContainer &&
                iViewUtility->ActiveViewType() != TUid::Uid(KMPXPluginTypeWaitNoteDialogUid) )
                {
                TRAP_IGNORE( AknDialogShutter::ShutDialogsL( *CEikonEnv::Static() ) );
                StopDisplayingMenuBar();
                TRAP_IGNORE( UpdateTitlePaneL() );  // some dialogs change the title. ie equalizer
                }
            else if( event == TMPXCollectionMessage::EBroadcastEvent )
                {
                if( op == EMcMsgUSBMassStorageStart ||
                    op == EMcMsgUSBMTPStart ||
                    op == EMcMsgFormatStart )
                    {
                    iDatabaseNotReady = ETrue;
                    }
                else if( op == EMcMsgUSBMassStorageEnd ||
                         op == EMcMsgUSBMTPEnd ||
                         op == EMcMsgFormatEnd )
                    {
                    iDatabaseNotReady = EFalse;
                    }
                else if( op == EMcMsgDiskInserted )
                    {
                    // Need to reset the prev/next buttons state if they are pressed at the moment.
                    if (iSkipBtnPressed)
                        {
                        if ( AknLayoutUtils::PenEnabled() )
                            {
                            if ( Toolbar() )
                                {
                                CAknToolbar* toolbar = Toolbar();
                                CAknButton* skipControl;
                                skipControl = (CAknButton*)(toolbar->ComponentControl( 0 ));
                                skipControl->ResetState();
                                skipControl->DrawNow();

                                skipControl = (CAknButton*)(toolbar->ComponentControl( 2 ));
                                skipControl->ResetState();
                                skipControl->DrawNow();
                                }
                            }
                        iSkipBtnPressed = EFalse;
                        }
                    }
                }
            else if (event == TMPXCollectionMessage::EPathChanged &&
                     op == EMcPathChangedByOpen &&
                     data == EMcItemOpened)
                { // Open new entries
                MPX_DEBUG1("CMPXPlaybackViewImp::HandleCollectionMessage() New Item Opened");
                iNewItemOpened = ETrue;
                delete iMedia;
                iMedia = NULL;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewImp::FillAiwParameters
// -----------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewImp::FillAiwParametersL(
    CAiwGenericParamList& aParamList, TBool aSaved )
    {
    TFileName location(NULL);
    if ( aSaved )
        {
        location = MPXTlsHelper::FilePath();
        }

    if ( !location.Length())
        {
        location = iMedia->ValueText( KMPXMediaGeneralUri );
        }

    MPX_DEBUG1( "Todd CMPXCommonPlaybackViewImp::FillAiwParametersL location=" );
    #ifdef _DEBUG
        RDebug::RawPrint(location);
    #endif

    TPtrC mimeType(iMedia->ValueText( KMPXMediaGeneralMimeType ));
    MPX_DEBUG1( "Todd CMPXCommonPlaybackViewImp::FillAiwParametersL mime=" );
    #ifdef _DEBUG
        RDebug::RawPrint(mimeType);
    #endif
    TAiwVariant varMime(mimeType);
    if ( !mimeType.Length() && ( iMediaRecognizer ) )
        {
        varMime.Set(iMediaRecognizer->MimeTypeL(location));
        }
    TAiwGenericParam paramMime(EGenericParamMIMEType, varMime);
    aParamList.Reset();
    aParamList.AppendL(paramMime);
    TAiwVariant varFile(location);
    TAiwGenericParam paramFile(EGenericParamFile, varFile);
    aParamList.AppendL(paramFile);
    }

// ---------------------------------------------------------------------------
// Updates the middle softkey display
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateMiddleSoftKeyDisplayL()
    {
    MPX_FUNC("CMPXCommonPlaybackViewImp::UpdateMiddleSoftKeyDisplayL");

    if (iContainer)
            {
        CEikButtonGroupContainer* cba = Cba();
        if (cba)
            {
            MMPXSource* s = iPlaybackUtility->Source();
            if ( s )
                {
                CMPXCollectionPlaylist* playlist = s->PlaylistL();
                if ( playlist )
                    {
                    CleanupStack::PushL( playlist );
                    if ( playlist->Count() > 0 )
                        {
                        TMPXPlaybackState state = iPlaybackUtility->StateL();

                        if ((state == EPbStateInitialising) ||(state == EPbStatePlaying))
                            {
                            iCommonUiHelper->SetMiddleSoftKeyLabelL( *cba, R_QTN_MSK_PAUSE, EAknSoftkeyForwardKeyEvent);
                            }
                        else
                            {
                            iCommonUiHelper->SetMiddleSoftKeyLabelL( *cba, R_QTN_MSK_PLAY, EAknSoftkeyForwardKeyEvent);
                            }
                        }
                    CleanupStack::PopAndDestroy( playlist );
                    }
                }
            MPX_DEBUG1("CMPXCommonPlaybackViewImp::UpdateMiddleSoftKeyDisplayL - new label displayed");

            cba->DrawDeferred();
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates the middle toolbar button
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewImp::UpdateToolbar()
    {
    MPX_FUNC("CMPXCommonPlaybackViewImp::UpdateToolbar");

    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( Toolbar() )
            {
            CAknToolbar* toolbar = Toolbar();
            toolbar->SetDimmed( EFalse );
            CAknButton* rwControl;
            rwControl = (CAknButton*)(toolbar->ComponentControl( 0 ));
            CAknButton* pausePlayControl;
            pausePlayControl = (CAknButton*)(toolbar->ComponentControl( 1 ));
            CAknButton* fwControl;
            fwControl = (CAknButton*)(toolbar->ComponentControl( 2 ));
            MMPXSource* s = iPlaybackUtility->Source();
            if ( s )
                {
                CMPXCollectionPlaylist* playlist = s->PlaylistL();
                if ( playlist )
                    {
                    CleanupStack::PushL( playlist );
                    if ( playlist->Count() > 0 )
                        {
                        if( pausePlayControl )
                        	{
	                        TMPXPlaybackState state = iPlaybackUtility->StateL();
	                        if ((state == EPbStateInitialising) || (state == EPbStatePlaying))
	                           {
	                           pausePlayControl->SetCurrentState(0, ETrue);
	                           }
	                        else
	                           {
	                           pausePlayControl->SetCurrentState(1, ETrue);
	                           }
                        	}

						if( rwControl && fwControl )
							{
							rwControl->SetDimmed( EFalse );
							rwControl->DrawNow();
							fwControl->SetDimmed( EFalse );
							fwControl->DrawNow();
                    		}
                        }

                    CleanupStack::PopAndDestroy( playlist );
                    }
                else
                    {
                    if( pausePlayControl )
                    	{
	                    TMPXPlaybackState state = iPlaybackUtility->StateL();
	                    if ( ( state == EPbStateInitialising ) ||
	                           ( state == EPbStatePlaying ) )
	                        {
	                        pausePlayControl->SetCurrentState( 0, ETrue );
	                        }
	                    else
	                        {
	                        pausePlayControl->SetCurrentState( 1, ETrue );
	                        }
	                    }
                    }
                }
            else
                {
                toolbar->SetDimmed( ETrue );
                }
            }
        }
    }
// ----------------------------------------------------------------------------
// Callback function of timer to handle delayed error
// ----------------------------------------------------------------------------
//
TInt CMPXCommonPlaybackViewImp::HandleDelayedError( TAny* aPtr )
    {
    CMPXCommonPlaybackViewImp* pv = reinterpret_cast<CMPXCommonPlaybackViewImp*>( aPtr );
    pv->iDelayedErrorTimer->Cancel();

    // compare index
    if ( pv->iPlaybackUtility )
    	{
        MMPXSource* source( pv->iPlaybackUtility->Source() );
        if ( source )
    	    {
            CMPXCollectionPlaylist* pl( source->PlaylistL() );
        	if ( pl )
                {
                CleanupStack::PushL( pl );
                if ( pv->iErrIndex == pl->Index() )
                	{
                	pv->HandleErrorL( pv->iLastDelayedErr );
                	}
                CleanupStack::PopAndDestroy( pl );
                }
            }
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Updates track's album art.
// ---------------------------------------------------------------------------
//
TInt CMPXCommonPlaybackViewImp::HandleTNRequestForCustomSizeL( TAny* aPtr )
    {
    MPX_DEBUG1("CMPXCommonPlaybackViewImp::HandleTNRequestForCustomSizeL()");
    
    ASSERT( aPtr );
    CMPXCommonPlaybackViewImp* pv = reinterpret_cast<CMPXCommonPlaybackViewImp*>( aPtr );
    
    ASSERT( pv->iOldUri );   
    if ( pv->iContainer && !pv->iSwitchingView )
        {
        TInt err( KErrNone );      
        if ( pv->iMedia && pv->iMedia->IsSupported(KMPXMediaGeneralUri) )
            {
            const TDesC& album = pv->iMedia->ValueText( KMPXMediaGeneralUri );

            if ( pv->iOldUri->Compare( album ) == 0 )
                { 
                TRect albumArtRect(
                        pv->iLayout->IndicatorLayout(
                        pv->ClientRect(), EAlbumArtArea ) );
                
                // Request for custom TN size               
                MPX_TRAP( err,
                    pv->iMPXUtility->ExtractAlbumArtL(
                    *pv->iMedia,
                    *pv->iContainer,
                    albumArtRect.Size() ); );                
                }
            }

        if ( KErrNone != err )
            {
            // If error, show default album art
            MPX_DEBUG2("CMPXCommonPlaybackViewImp::HandleTNRequestForCustomSizeL(): err = %d", err);
            pv->iContainer->ExtractAlbumArtCompleted( NULL, KErrNone );
            }
        }
    
    // cancel timer
    if ( pv->iTNRequestTimer->IsActive())
        {
        pv->iTNRequestTimer->Cancel();
        }
        
    return KErrNone;
    }
    
//  End of File
