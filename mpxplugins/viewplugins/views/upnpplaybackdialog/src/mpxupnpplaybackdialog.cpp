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
* Description:  Music Player upnp playback dialog implementation
*
*/



// INCLUDE FILES
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <akntitle.h>
#include <aknnotewrappers.h>
#include <aknnavide.h>
#include <aknnavilabel.h>
#include <coeutils.h>
#include <hlplch.h>
#include <bautils.h>
#include <centralrepository.h>
#include <bldvariant.hrh>
#include <caf/caferr.h>
#include <aknsoundsystem.h>
#include <apgcli.h>             // RApaLsSession
#include <ctsydomainpskeys.h>

#include <featmgr.h>
#include <hwrmfmtx.h> // TFmTxState, for FF_FMTX

#ifdef __SERIES60_31__
#include <upnpnetworkaccess.h>  // KCRUidUPnPApplication, KUPnPAppAccessPoint
#else
#include <upnpsettings.h>
#endif

#include <mpxcollectionmessage.h>
#include <mpxcollectionutility.h>
#include <mpxmediakeyhandler.h>

#include <data_caging_path_literals.hrh>
#include <layoutmetadata.cdl.h>
#include <mpxcommonplaybackview.rsg>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>   // KMPXMessageGeneralId
#include <mpxplaybackmessagedefs.h>  // KMPXMessagePbMediaChanged, KMPXMessagePbMedia
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
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
#include <mpxcommandgeneraldefs.h>


#include <mpxconstants.h>
#include <mpxappui.hrh>
#include "mpxcommonplaybackview.hrh"
#include "mpxalbumartutil.h"
#include "mpxcommonuihelper.h"
#include <mpxinternalcrkeys.h>
#include <mpxtlshelper.h>
#include <mpxlog.h>
#include <mpxuser.h>
#include <akntoolbar.h> 
#include <aknbutton.h> 

#include "mpxupnpplaybackdialog.hrh"
#include "mpxupnpplaybackdialogcustomcontrol.h"
#include <mpxupnpplaybackview.rsg>
#include "mpxupnpplaybackdialogcustomcontrolcontainer.h"
#include "mpxupnpplaybackdialog.h"
#include "mpxupnpplaybackdialoglayout.h"
#include <mpxupnpplaybackdialog.rsg>

// CONSTANTS
const TInt KMPXOneSecInMilliSecs( 1000 );

_LIT( KMPXUPnPPlaybackDialogRscPath, "mpxUpnpPlaybackdialog.rsc" );
_LIT( KMPXCommonPlaybackRscPath, "mpxcommonplaybackview.rsc" );
_LIT( KMPXUpnpPlaybackRscPath, "mpxupnpplaybackview.rsc" );

_LIT( KMPXFMTXRscPath, "mpxfmtx.rsc" ); // For FF_FMTX

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
_LIT(KMPXPnRealAudioMimeType, "audio/x-pn-realaudio");
_LIT(KMPXRealAudioMimeType, "audio/x-realaudio");
_LIT(KMPXRnRealAudioMimeType, "audio/vnd.rn-realaudio");
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXUPnPPlaybackDialog* CMPXUPnPPlaybackDialog::NewL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::NewL" );
    CMPXUPnPPlaybackDialog* self =
        new ( ELeave ) CMPXUPnPPlaybackDialog();
    CleanupStack::PushL( self );
    self->ConstructL( R_MPX_CUI_UPNP_PLAYBACK_DIALOG_MENUBAR );
    CleanupStack::Pop( self );  // this, it will be PushL-d by executeLD...
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::CMPXUPnPPlaybackDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialog::CMPXUPnPPlaybackDialog()
    {
    }

// ----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::SetParamL
// parameter passed in from viewframework
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXUPnPPlaybackDialog::SetParamL( const TDesC* /*aParam*/ )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::SetParamL" );
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::ConstructL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::ConstructL( TInt aResource )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::ConstructL" );
    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXUPnPPlaybackDialogRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    parse.Set( KMPXCommonPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName playbackResourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( playbackResourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), playbackResourceFile );
    iCommonPlaybackResourceOffset = coeEnv->AddResourceFileL( playbackResourceFile );

    parse.Set( KMPXUpnpPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName upnpResourceFile = parse.FullName();
    User::LeaveIfError( MPXUser::CompleteWithDllPath( upnpResourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), upnpResourceFile );
    iUpnpResourceOffset = coeEnv->AddResourceFileL( upnpResourceFile );

    // CommonPlaybackViewLayout uses this rsc file so it needs to be loaded
	if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
		{
		parse.Set( KMPXFMTXRscPath, &KDC_APP_RESOURCE_DIR, NULL );
		TFileName fmtxResourceFile( parse.FullName() );
		User::LeaveIfError( MPXUser::CompleteWithDllPath( fmtxResourceFile ) );
		BaflUtils::NearestLanguageFile( coeEnv->FsSession(), fmtxResourceFile );
		iFMTXResourceOffset = coeEnv->AddResourceFileL( fmtxResourceFile );
		}

    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );

    iMPXUtility = CMPXAlbumArtUtil::NewL();
    iCommonUiHelper = CMPXCommonUiHelper::NewL();

    iEmbedded = iEikonEnv->StartedAsServerApp();

    // Get an instance of view utility
    iViewUtility = MMPXViewUtility::UtilityL();

    TInt flags( 0 );
    CRepository* repository( CRepository::NewL( KCRUidMPXMPFeatures ));
    User::LeaveIfError( repository->Get( KMPXMPLocalVariation, flags ));
    delete repository;
    repository = NULL;

    iChangeRTForAllProfiles =
        static_cast<TBool>( flags & KMPXChangeRTForAll );

    iCollectionUtility = MMPXCollectionUtility::NewL();

    iStartPlaybackIndex = KErrNotFound;
    iLastSkipDirection = 1;

    if ( !iMediaKeyHandler )
        {
        // Handle media key in appui
        iMediaKeyHandler = MMPXMediaKeyHandler::NewL(
            EDisplayVolumePopup | EDisplayMediaPopup,
            this );
        }

    CAknDialog::ConstructL( aResource );
    if ( AknLayoutUtils::PenEnabled())
        {
        CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi());
        iToolbar = appUi->CurrentFixedToolbar();
        if ( iToolbar )
            {
            iBtnPreviousItem = 
                CAknButton::NewL( R_MPX_PBV_TOUCH_TOOLBAR_PREVIOUS_ITEM_BTN );
            iBtnNextItem = 
                CAknButton::NewL( R_MPX_PBV_TOUCH_TOOLBAR_NEXT_ITEM_BTN );
            iBtnPlayPause = 
                CAknButton::NewL( R_MPX_PBV_TOUCH_TOOLBAR_PLAYPAUSE_BTN );
            
            iToolbar->AddItemL( iBtnPreviousItem, EAknCtButton, 
                    EMPXPbvCmdPreviousListItem, KAknButtonReportOnLongPress );
            iToolbar->AddItemL( iBtnPlayPause, EAknCtButton, 
                    EMPXPbvCmdPlayPause, 0 );
            iToolbar->AddItemL( iBtnNextItem, EAknCtButton, 
                    EMPXPbvCmdNextListItem, KAknButtonReportOnLongPress );
            
            iToolbar->SetToolbarVisibility( ETrue );
            iToolbar->SetToolbarObserver( this );
            }
        }
    }
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXUPnPPlaybackDialog::~CMPXUPnPPlaybackDialog()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::~CMPXUPnPPlaybackDialog" );

    delete iMPXUtility;
    delete iMedia;
    delete iCommonUiHelper;

    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    if ( iViewUtility )
        {
        iViewUtility->Close();
        }

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }

    if ( iCommonPlaybackResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iCommonPlaybackResourceOffset );
        }

    iPlayersList.Close();
    delete iSubPlayerName;

    if ( iUpnpResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iUpnpResourceOffset );
        }

	if ( iFMTXResourceOffset )
		{
		iEikonEnv->DeleteResourceFile( iFMTXResourceOffset );
		}

    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    if ( iKeySoundDisabled )
        {
        iAvkonAppUi->KeySounds()->PopContext();
        }
    delete iOrigTitle;
    delete iPlaybackDialogLayout;

    delete iMediaKeyHandler;

    if ( iDelayedEventExit )
        {
        iDelayedEventExit->Cancel();
        }
    delete iDelayedEventExit;
    }

// ---------------------------------------------------------------------------
// Updates playback view.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdateViewL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::UpdateViewL" );

    if ( GetUPnPPlaybackDialogCustomControl() && !iSwitchingView )
        {
        UpdatePlaybackState( iPlaybackState );
        UpdateTrackInfoL( iMedia );
        UpdateTrackPlaybackPositionL( iPosition, iDuration );
        UpdateTrackPosInPlaylistL();
        UpdateAlbumArtL( iMedia );
        UpdateDownloadStateLabelL();

        // Retrieve current repeat & random modes
        iPlaybackUtility->PropertyL( *this, EPbPropertyRandomMode );
        iPlaybackUtility->PropertyL( *this, EPbPropertyRepeatMode );
        iPlaybackUtility->PropertyL( *this, EPbPropertyVolume);
        }
    }

// ---------------------------------------------------------------------------
// Updates track info field.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdateTrackInfoL(
    const CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::UpdateTrackInfo" );

    if ( GetUPnPPlaybackDialogCustomControl() && !iSwitchingView )
        {
        if ( aMedia )
            {
            if ( aMedia->IsSupported( KMPXMediaGeneralTitle ) )
                {
                GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                    ETextTrack,
                    aMedia->ValueText( KMPXMediaGeneralTitle ) );
                }
            else if ( aMedia->IsSupported( KMPXMediaGeneralUri ) )
                {
                TParsePtrC filePath(
                    aMedia->ValueText( KMPXMediaGeneralUri ) );
                GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                    ETextTrack, filePath.Name() );
                }
            else
                {
                GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                    ETextTrack, KNullDesC );
                }

            if ( aMedia->IsSupported( KMPXMediaMusicArtist ) )
                {
                const TDesC& artist =
                    aMedia->ValueText( KMPXMediaMusicArtist );
                if ( artist != KNullDesC )
                    {
                    GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                        ETextArtist,
                        artist );
                    }
                else
                    {
                    if ( !iSkipping )
                        {
                        // Display unknown artist as artist
                        HBufC* unknownArtistText =
                            StringLoader::LoadLC( R_MPX_PBV_UNKNOWN_ARTIST_LABEL );
                        GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                            ETextArtist, *unknownArtistText );
                        CleanupStack::PopAndDestroy( unknownArtistText );
                        }
                    else
                        {
                        GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                            ETextArtist, KNullDesC );
                        }
                    }
                }
            else
                {
                GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                    ETextArtist, KNullDesC );
                }

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
            if ( aMedia->IsSupported( KMPXMediaGeneralMimeType ) )
                {
                // Get mime type
                const TDesC& mimeType = aMedia->ValueText(
                                                KMPXMediaGeneralMimeType );

                const TBool realAudioMode =
                    ( mimeType.Compare( KMPXPnRealAudioMimeType ) == 0 ) ||
                    ( mimeType.Compare( KMPXRealAudioMimeType ) == 0) ||
                    ( mimeType.Compare( KMPXRnRealAudioMimeType ) == 0 );

                MPX_DEBUG2("CMPXUPnPPlaybackDialog::UpdateTrackInfoL() mimeType: %S", &mimeType);
                // Set the real audio mode
                GetUPnPPlaybackDialogCustomControl()->SetRealAudioMode( realAudioMode );
                }
            else
                {
                GetUPnPPlaybackDialogCustomControl()->SetRealAudioMode( EFalse );
                }
#endif
            }
        else
            {
            // Display nothing if properties is NULL
            GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                ETextTrack, KNullDesC );
            GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                ETextArtist, KNullDesC );
            }
        }
    }

// ---------------------------------------------------------------------------
// Update current playback state.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdatePlaybackState(
    TMPXPlaybackState aPlaybackState )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::UpdatePlaybackState()" );
    MPX_DEBUG2("CMPXUPnPPlaybackDialog::UpdatePlaybackState(%d): Entering", aPlaybackState);

    if ( GetUPnPPlaybackDialogCustomControl() && !iSwitchingView )
        {
        TMPXPbvPlaybackMode mode( EInvalidMode );

        switch ( aPlaybackState )
            {
            case EPbStateNotInitialised:
                {
                mode = ENoTracksMode;
                break;
                }
            case EPbStateInitialising:
            case EPbStateDownloading:
                {
                mode = EInitialising;
                break;
                }
            case EPbStatePlaying:
                {
                mode = EPlayMode;
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
            default:
                {
                // Pass
                break;
                }
            }

        if ( EInvalidMode != mode )
            {
            iPlaybackState = aPlaybackState;
            GetUPnPPlaybackDialogCustomControl()->SetMode( mode );
            }

        GetUPnPPlaybackDialogCustomControl()->UpdateButtons( aPlaybackState );
        UpdateToolbar();
        }
    else
        {
        // If no container, just need to update state
        switch ( aPlaybackState )
            {
            case EPbStateNotInitialised:
            case EPbStateInitialising:
            case EPbStateDownloading:
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
    }

// ---------------------------------------------------------------------------
// Updates track's playback position.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdateTrackPlaybackPositionL(
    TInt aPos,
    TInt aDuration )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::UpdateTrackPlaybackPosition" );

    if ( GetUPnPPlaybackDialogCustomControl() && !iSwitchingView )
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
                GetUPnPPlaybackDialogCustomControl()->UpdateTimeIndicatorsL( aPos, aDuration );
                break;
                }
            case EPbStateStopped:
                {
                GetUPnPPlaybackDialogCustomControl()->UpdateTimeIndicatorsL( aPos, aDuration );
                } // falls through
            case EPbStateNotInitialised:
                {
                GetUPnPPlaybackDialogCustomControl()->UpdateDurationLabelL( aDuration );
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
void CMPXUPnPPlaybackDialog::UpdateAlbumArtL(
    const CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::UpdateAlbumArt" );

    if ( GetUPnPPlaybackDialogCustomControl() && !iSwitchingView )
        {
        TInt err( KErrNone );
        if ( aMedia )
            {
            TRect albumArtRect(
                iPlaybackDialogLayout->IndicatorLayout(
                    iModRect, EAlbumArtArea ) );

            MPX_TRAP( err,
                iMPXUtility->ExtractAlbumArtL(
                    *aMedia,
                    *GetUPnPPlaybackDialogCustomControl(),
                    albumArtRect.Size() ); );
            }

        if ( !aMedia || KErrNone != err )
            {
            // If error, show default album art
            MPX_DEBUG2("CMPXUPnPPlaybackDialog::UpdateAlbumArt(): err = %d", err);
            GetUPnPPlaybackDialogCustomControl()->ExtractAlbumArtCompleted( NULL, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates track position in playlist field.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdateTrackPosInPlaylistL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::UpdateTrackPosInPlaylist" );

    if ( GetUPnPPlaybackDialogCustomControl() && !iSwitchingView )
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
                index = playlist->Index();
                delete playlist;
                playlist = NULL;
                }
            }
        MPX_DEBUG3("CMPXUPnPPlaybackDialog::UpdateTrackPosInPlaylistL: index = %d, count = %d", index, count );

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
                GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                    ETextPlaylistCount,
                    *plcounter );
                CleanupStack::PopAndDestroy( plcounter );
                CleanupStack::PopAndDestroy( params );
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
            HBufC* noTracksText =
                StringLoader::LoadLC( R_MPX_PBV_NO_TRACKS_LABEL );
            GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                ETextEmpty, *noTracksText );
            CleanupStack::PopAndDestroy( noTracksText );
            }

        if ( emptyCount )
            {
            // Empty string if playlist is empty
            GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                ETextPlaylistCount,
                KNullDesC );
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates download state label.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdateDownloadStateLabelL()
    {
    MPX_FUNC( "CMPXCommonPlaybackViewImp::UpdateDownloadStateLabelL" );

    if ( GetUPnPPlaybackDialogCustomControl() && !iSwitchingView )
        {
        switch ( iPlaybackState )
            {
            case EPbStateInitialising:
            case EPbStateDownloading:
                {
                HBufC* labelText(
                    StringLoader::LoadLC( R_MPX_PBV_SONG_OPENING_LABEL ));
                GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                    ETextDownloadState, *labelText );
                CleanupStack::PopAndDestroy( labelText );
                break;
                }
            default:
                {
                GetUPnPPlaybackDialogCustomControl()->UpdateLabelL(
                    ETextDownloadState, KNullDesC );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates Title Pane.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdateTitlePaneL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::UpdateTitlePaneL" );

    CAknTitlePane* title( NULL );

    TRAP_IGNORE(
        {
        title = static_cast<CAknTitlePane*>
            ( iAvkonAppUi->StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        } );

    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
    TUid currentlyUsedPlayer;
    TInt currentlyUsedSubPlayer;
    delete iSubPlayerName;
    iSubPlayerName = NULL;
    iCurrentPlayerType = EPbLocal;
    TRAP_IGNORE( manager.GetSelectionL( iCurrentPlayerType,
                                        currentlyUsedPlayer,
                                        currentlyUsedSubPlayer,
                                        iSubPlayerName ) );

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
			if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
				{
				// We need to switch off FMtx so playback can be local
				CHWRMFmTx* fmtx = CHWRMFmTx::NewLC();
				TRAP_IGNORE( fmtx->DisableL());
				CleanupStack::PopAndDestroy( fmtx );
				}

            RApaLsSession lsSession;
            TInt ret = lsSession.Connect();
            if ( ret == KErrNone )
                {
                CleanupClosePushL( lsSession );
                TApaAppInfo appInfo;
                RProcess registeredProcess;
                CleanupClosePushL( registeredProcess );

                ret = lsSession.GetAppInfo( appInfo, registeredProcess.Identity() );
                MPX_DEBUG2("ProcessUID=0x%x", registeredProcess.Identity().iUid);

                CleanupStack::PopAndDestroy( &registeredProcess );

                if ( ret == KErrNone )
                    {
                    MPX_DEBUG2("calling app name = %S", &appInfo.iCaption);
                    title->SetTextL(appInfo.iCaption);
                    }
                CleanupStack::PopAndDestroy( &lsSession ); // close lsSession
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL" );

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ));
    if ( KMPXMessagePbMediaChanged == id )
        {
        if ( aMessage.IsSupported( KMPXMessagePbMedia ))
            {
            CMPXMedia* media( aMessage.Value<CMPXMedia>( KMPXMessagePbMedia ));
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
                MPX_DEBUG2( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL - EPropertyChanged(%d)", type );
                TMPXPlaybackProperty property(
                    static_cast<TMPXPlaybackProperty>( type ) );
                TInt error( KErrNone );

                DoHandlePropertyL( property, data, error );
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL - EStateChanged(%d)", type );

                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );
                if ( ( data == KErrNotSupported ) &&
                     ( state == EPbStatePaused ) )
                     {
                     CommandNotSupportedL();
                     }
                 else if ( ( data == KErrAccessDenied ) &&
                           ( state == EPbStatePaused ))
                    {
                    if ( MPXUser::IsCallOngoing( EPSCTsyCallTypeH324Multimedia ))
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
            case TMPXPlaybackMessage::EDownloadStarted:
                {
                MPX_DEBUG2( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL - EDownloadStarted(%d)", type );
                TMPXPlaybackState state = EPbStateDownloading;
                DoHandleStateChangedL( state, data );
                break;
                }
            case TMPXPlaybackMessage::ECommandReceived:
                {
                MPX_DEBUG2( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL - ECommandReceived(%d)", type );
                break;
                }
            case TMPXPlaybackMessage::ESkipping:
                {
                MPX_DEBUG2( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL - ESkipping(%d)", data );
                iMPXUtility->CancelRequest();
                if ( GetUPnPPlaybackDialogCustomControl() )
                    {
                    GetUPnPPlaybackDialogCustomControl()->SetMode( EMediaKeySkipping );
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
                MPX_DEBUG1( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL - ESkipEnd()");
                iSkipping = EFalse;
                break;
                }
            case TMPXPlaybackMessage::EPlaylistUpdated:
                {
                MPX_DEBUG1( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL - EPlaylistUpdated()");
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
                                if ( !GetUPnPPlaybackDialogCustomControl() )
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
                if (iCommonUiHelper)
                    {
                    iCommonUiHelper->DismissWaitNoteL();
                    }
                break;
                }
            case TMPXPlaybackMessage::EPlayerUnavailable:
                {
                TUid activeView = iViewUtility->ActiveViewType();
                if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
                    {
                    iPlaybackUtility->CommandL(EPbCmdClose);
                    if ( data == KErrNotFound )
                        {
                        if ( iSubPlayerName )
                            {
                            HBufC* dialogText = StringLoader::LoadLC( R_MPX_NOTE_REMOTE_CONNECTION_FAILED,
                                                                      *iSubPlayerName );
                            CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                            errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                            errNote->ExecuteLD( *dialogText );
                            CleanupStack::PopAndDestroy( dialogText );
                            }
                        }

                    if ( !iDelayedEventExit )
                        {
                        iDelayedEventExit = CIdle::NewL(CActive::EPriorityStandard);
                        }

                    if ( !iDelayedEventExit->IsActive())
                        {
                        TCallBack callBack( CMPXUPnPPlaybackDialog::DelayedExit,
                                            this );
                        iDelayedEventExit->Start(callBack);
                        }
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError,
    TBool aInit)
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::DoHandlePropertyL" );
    MPX_DEBUG4( "HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    if ( KErrNone == aError )
        {
        switch ( aProperty  )
            {
            case EPbPropertyPosition:
                {
                iPosition = aValue / KMPXOneSecInMilliSecs;
                UpdateTrackPlaybackPositionL(
                    iPosition, iDuration );
                break;
                }
            case EPbPropertyDuration:
                {
                iDuration = aValue / KMPXOneSecInMilliSecs;
                if ( ( ( iPlaybackState == EPbStateStopped ) ||
                      ( iPlaybackState == EPbStateInitialising ) ) &&
                     iDuration > 0 )
                    {
                    UpdateTrackPlaybackPositionL( iPosition, iDuration );
                    }
                break;
                }
            case EPbPropertyRandomMode:
                {
                iRandomMode = aValue;
                if ( GetUPnPPlaybackDialogCustomControl() )
                    {
                    GetUPnPPlaybackDialogCustomControl()->SetRandomMode( aValue );
                    }
                break;
                }
            case EPbPropertyRepeatMode:
                {
                iRepeatMode = aValue;
                if ( GetUPnPPlaybackDialogCustomControl() )
                    {
                    GetUPnPPlaybackDialogCustomControl()->SetRepeatMode(
                        EPbRepeatAll == aValue, EPbRepeatOne == aValue );
                    }
                break;
                }
            case EPbPropertyVolume:
                {
                iCurrentVolume = aValue;
                MPX_DEBUG2( "CMPXUPnPPlaybackDialog::DoHandlePlaybackMessageL iCurrentVolume = %d", iCurrentVolume );
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
void CMPXUPnPPlaybackDialog::DoHandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::DoHandleMediaL" );
    MPX_DEBUG2("CMPXUPnPPlaybackDialog::DoHandleMediaL(): aError = %d", aError);

    delete iMedia;
    iMedia = NULL;
    if ( KErrNone == aError )
        {
        iMedia = CMPXMedia::NewL( aMedia );

        UpdateTrackPosInPlaylistL();
        UpdateTrackInfoL( iMedia );
        // If media key skipping, do not update album art
        if ( !iSkipping )
            {
            UpdateAlbumArtL( iMedia );
            }

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
            // Copy error code and reset iDelayedError, this is becuase
            // a media event may be called while handling this error
            TInt err( iDelayedError );
            iDelayedError = KErrNone;
            HandleErrorL( err );
            }
        }
    else
        {
        HandleErrorL( aError );
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
void CMPXUPnPPlaybackDialog::DoHandleStateChangedL(
    TMPXPlaybackState aState,
    TInt aData )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::DoHandleStateChangedL" );

    // If KErrDisconnected, it's a fatal error and we close the dialog
    if ( ( aState == EPbStatePaused ) &&
         ( aData == KErrDisconnected ) )
        {
        TUid activeView = iViewUtility->ActiveViewType();
        if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
            {
            iPlaybackUtility->CommandL(EPbCmdStop);

            if ( !iDelayedEventExit )
                {
                iDelayedEventExit = CIdle::NewL(CActive::EPriorityStandard);
                }

            if ( !iDelayedEventExit->IsActive())
                {
                TCallBack callBack( CMPXUPnPPlaybackDialog::DelayedExit,
                                    this );
                iDelayedEventExit->Start(callBack);
                }
            }
        }
    else
        {
        UpdatePlaybackState( aState );

        switch ( aState )
            {
            case EPbStateStopped:
                {
                iStartPlaybackIndex = KErrNotFound;
                }
            case EPbStateNotInitialised:
                {
                UpdateTrackPlaybackPositionL( 0, iDuration );
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

                // Fall through on purpose
                }
            case EPbStateDownloading:
                {
                UpdateDownloadStateLabelL();
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
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleErrorL( TInt aError )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleErrorL()" );
    MPX_DEBUG2( "CMPXUPnPPlaybackDialog::HandleErrorL(%d)", aError );
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
        if ( KErrNotFound != nextIndex &&
            ( ( KErrArgument != aError && KErrCompletion != aError ) ||
              iPlaybackState != EPbStatePlaying ))
            {
            // Check if all tracks are invalid, or if there's only
            // 1 track in the playlist
            if ( nextIndex == iStartPlaybackIndex ||
                 plCount == 1 )
                {
                aError = KMPXAllTracksInvalid;
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
                             currentIndex == iStartPlaybackIndex )
                            {
                            iCommonUiHelper->HandleErrorL( aError, iMedia );
                            }
                        }
                    }
                SkipOnErrorL( currentIndex, plCount );
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
                {
                SkipOnErrorL( currentIndex, plCount );
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
            // Cannot be recovered, exit the dialog.
            case KErrDisconnected:
                {
                TUid activeView = iViewUtility->ActiveViewType();
                if ( activeView == TUid::Uid( KMPXPluginTypePlaybackUid ) )
                    {
                    iPlaybackUtility->CommandL(EPbCmdStop);

                    if ( !iDelayedEventExit )
                        {
                        iDelayedEventExit = CIdle::NewL(CActive::EPriorityStandard);
                        }

                    if ( !iDelayedEventExit->IsActive())
                        {
                        TCallBack callBack( CMPXUPnPPlaybackDialog::DelayedExit,
                                            this );
                        iDelayedEventExit->Start(callBack);
                        }
                    }
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
    }

// ---------------------------------------------------------------------------
// Displays error note for when all tracks are invalid,
// and goes back to collection list view
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleAllTracksInvalidL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleAllTracksInvalidL()" );

    iPlaybackUtility->CommandL( EPbCmdStop );
    UpdatePlaybackState( EPbStateStopped );

    // Do not display error if view is not active
    if ( GetUPnPPlaybackDialogCustomControl() )
        {
        iCommonUiHelper->HandleErrorL( KMPXAllTracksInvalid, iMedia );
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

// ---------------------------------------------------------------------------
// Checks whether or not to skip to next track on an error
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::SkipOnErrorL(
    TInt aCurrentIndex,
    TInt aCount )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::SkipOnErrorL()" );
    MPX_DEBUG2("CMPXUPnPPlaybackDialog::SkipOnErrorL(%d)", aCurrentIndex);

    TBool skip( ETrue );
    TBool displayError( EFalse );

    // UI specs say theres two cases where it stops and
    // does not skip to the next song.
    // First case is if not repeat all and playback has
    // reached the end of the list and gone to the first song.
    // Second case is if not repeat all and the user is skipping
    // backwards and reached the first song
    if ( EPbRepeatAll != iRepeatMode )
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

    MPX_DEBUG1("CMPXUPnPPlaybackDialog::SkipOnErrorL: Exiting");
    }

// ---------------------------------------------------------------------------
// Request for the media object
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::RequestMediaL( TBool aDrm /*=EFalse*/ )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::RequestMediaL()" );

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
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::PrepareStatusPaneForPlaybackViewL
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::PrepareStatusPaneForPlaybackViewL()
    {
    iPreviousLayoutId = iAvkonAppUi->StatusPane()->CurrentLayoutResId();

    if ( !Layout_Meta_Data::IsLandscapeOrientation() )
        {
        if ( iPreviousLayoutId !=
             R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT )
            {
            TRAP_IGNORE(
                iAvkonAppUi->StatusPane()->SwitchLayoutL(
                R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT ));
            }
        }
    else
        {
        if ( iPreviousLayoutId !=
             R_AVKON_STATUS_PANE_LAYOUT_USUAL )
            {
            TRAP_IGNORE(
                iAvkonAppUi->StatusPane()->SwitchLayoutL(
                R_AVKON_STATUS_PANE_LAYOUT_USUAL ));
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::PrepareStatusPaneForExitingViewL
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::PrepareStatusPaneForExitingViewL()
    {
    if ( iAvkonAppUi->StatusPane()->CurrentLayoutResId() !=
         R_AVKON_STATUS_PANE_LAYOUT_USUAL)
        {
        iAvkonAppUi->StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandlePlaybackMessage" );
    MPX_DEBUG2("CMPXUPnPPlaybackDialog::HandlePlaybackMessage(%d)", aError);
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    else if ( aError != KErrNone )
        {
        MPX_DEBUG2( "CMPXUPnPPlaybackDialog::HandlePlaybackMessage - EError(%d)", aError );
        // Check if it's a DRM song first, only get media for DRM songs
        TBool drmSong( EFalse );
        if ( iMedia )
            {
            if ( iMedia->IsSupported( KMPXMediaDrmProtected ))
                {
                drmSong = iMedia->ValueTObjectL<TBool>( KMPXMediaDrmProtected );
                }
            }
        if ( drmSong )
            {
            iDelayedError = aError;
            TRAP_IGNORE( RequestMediaL( ETrue ));
            }
        else
            {
            iDelayedError = KErrNone;
            TRAP_IGNORE( HandleErrorL( aError ));
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    DoHandlePropertyL( aProperty, aValue, aError, ETrue );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleSubPlayerNamesL" );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media event.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleMediaL" );
    DoHandleMediaL( aMedia, aError );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionMediaObserver
// Handle extended media properties
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleCollectionMediaL" );
    DoHandleMediaL( aMedia, aError );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleCommandL( TInt aCommand )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleCommandL()" );
    MPX_DEBUG2( "CMPXUPnPPlaybackDialog::HandleCommandL(%d) entering", aCommand );

    switch (aCommand)
        {
        case EMPXPbvCmdPlay:
            {
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
        case EMPXPbvCmdSeekBackward:
            {
            if ( iCurrentPlayerType == EPbLocal)
                {
                if ( aCommand == EMPXPbvCmdSeekForward )
                    {
                    iPlaybackUtility->CommandL( EPbCmdStartSeekForward );
                    }
                else
                    {
                    iPlaybackUtility->CommandL( EPbCmdStartSeekBackward );
                    }

                if ( !iKeySoundDisabled )
                    {
                    iAvkonAppUi->KeySounds()->PushContextL(
                        R_MPX_PBV_LEFT_RIGHT_KEYS_SILENT );
                    iKeySoundDisabled = ETrue;
                    }
                }
            else
                {
                // Not Supported in UPnP
                if ( iSeekForward )
                    {
                    break;
                    }
                iSeekForward = ETrue;
                CommandNotSupportedL();

                if ( iLastKeyType != EEventKeyUp )
                    {
                    iLastKeyType = EEventKeyUp;
                    if ( GetUPnPPlaybackDialogCustomControl() )
                        {
                        GetUPnPPlaybackDialogCustomControl()->OfferKeyEventL(
                                                                    iLastKeyEvent,
                                                                    EEventKeyUp );
                        }
                    }
                iSeekForward = EFalse;
                }
            break;
            }
        case EMPXPbvCmdEndSeek:
            {
            if ( iCurrentPlayerType == EPbLocal)
                {
                iPlaybackUtility->CommandL( EPbCmdStopSeeking );
                if ( iKeySoundDisabled )
                    {
                    iAvkonAppUi->KeySounds()->PopContext();
                    iKeySoundDisabled = EFalse;
                    }
                }
            // else Not supported
            break;
            }
        case EMPXPbvCmdNextListItem:
            {
            // When user skips a track, reset the start playback index
            iStartPlaybackIndex = KErrNotFound;

            iPlaybackUtility->CommandL( EPbCmdNext );
            break;
            }
        case EMPXPbvCmdPreviousListItem:
            {
            // When user skips a track, reset the start playback index
            iStartPlaybackIndex = KErrNotFound;

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
        case EMPXCmdSeekForward:
        case EMPXCmdSeekBackward:
            {
            if ( iCurrentPlayerType == EPbLocal)
                {
                iAvkonAppUi->HandleCommandL( aCommand );
                }
            else
                {
                CommandNotSupportedL();
                }
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
        case EMPXCmdStopSeeking:
        case EMPXCmdVolumeUp:
        case EMPXCmdVolumeDown:
            {
            iAvkonAppUi->HandleCommandL( aCommand );
            break;
            }
        default:
            {
            iAvkonAppUi->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::ProcessCommandL( TInt aCommandId )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::ProcessCommandL" );
    CAknDialog::ProcessCommandL( aCommandId );
    HandleCommandL( aCommandId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Event handler for status pane size changes.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleStatusPaneSizeChange()
    {
    CMPXUPnPPlaybackDialogCustomControlContainer *control =
        ( CMPXUPnPPlaybackDialogCustomControlContainer* )
        ControlOrNull ( EMPXUPnPPlaybackDlgCtrlId );
    if ( control )
        {
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, iModRect );

        if ( iToolbar && iToolbar->IsVisible() 
                && !Layout_Meta_Data::IsLandscapeOrientation())
            {
            iModRect.iBr.iY -= iToolbar->Rect().Height();
            }
        control->SetRect( iModRect );
        control->iDialogCustomControl->SetRect( iModRect );
        control->iDialogCustomControl->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::GetUPnPPlaybackDialogCustomControl
// Returns the Custom Control Container.
// -----------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogCustomControl *CMPXUPnPPlaybackDialog::
    GetUPnPPlaybackDialogCustomControl() const
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::GetUPnPPlaybackDialogCustomControl" );
    CMPXUPnPPlaybackDialogCustomControlContainer *control =
        ( CMPXUPnPPlaybackDialogCustomControlContainer* )
        ControlOrNull ( EMPXUPnPPlaybackDlgCtrlId );
    if ( control )
        {
        return control->iDialogCustomControl;
        }
    return NULL;
    }


// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::CreateCustomControlL
// Creates a Custom Control for the dialog.
// -----------------------------------------------------------------------------
//
SEikControlInfo CMPXUPnPPlaybackDialog::CreateCustomControlL(
    TInt aControlType )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::CreateCustomControlL" );
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;

    switch (aControlType)
        {
        case EMPXUPnPPlaybackDlg:
            {
            controlInfo.iControl = new ( ELeave )
                CMPXUPnPPlaybackDialogCustomControlContainer;
            break;
            }
        default:
            {
            break;
            }
        }
    return controlInfo;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::PreLayoutDynInitL
// Initialises the dialog's controls before the dialog is sized and layed out.
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::PreLayoutDynInitL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::PreLayoutDynInitL" );

    iSwitchingView = EFalse;

    PrepareStatusPaneForPlaybackViewL();

    iRandomMode = KErrNotFound;
    iRepeatMode = KErrNotFound;

    BackupPreviousStatusPaneL();

    // Set title
    // Ignore errors from updating title pane since if that is not
    // updated, still want to activate view
    TRAP_IGNORE(
        {
        UpdateTitlePaneL();
        CAknNavigationControlContainer* naviPane(
            static_cast<CAknNavigationControlContainer*>
            ( iAvkonAppUi->StatusPane()->ControlL(
                TUid::Uid( EEikStatusPaneUidNavi ) ) ));
        if ( naviPane )
            {
            naviPane->PushDefaultL();
            }
        } );

    if ( ButtonGroupContainer().ButtonGroup() )
        {
        CCoeControl* control = ButtonGroupContainer().ButtonGroup()->AsControl();
        static_cast<CEikCba*>( control )->
            SetSkinBackgroundId( KAknsIIDQsnBgAreaControlMp );
        }

    AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, iModRect );

    if ( iToolbar && iToolbar->IsVisible() 
            && !Layout_Meta_Data::IsLandscapeOrientation())
        {
        iModRect.iBr.iY -= iToolbar->Rect().Height();
        }
    CMPXUPnPPlaybackDialogCustomControlContainer *control1 =
        ( CMPXUPnPPlaybackDialogCustomControlContainer* ) ControlOrNull (
            EMPXUPnPPlaybackDlgCtrlId );
    CMPXUPnPPlaybackDialogCustomControl *control = NULL;

    if ( control1 )
        {
        if ( !iPlaybackDialogLayout )
            {
            iPlaybackDialogLayout = CMPXUPnPPlaybackDialogLayout::NewL();
            }

        control = CMPXUPnPPlaybackDialogCustomControl::NewL(
            *control1, iModRect, this, this, iPlaybackDialogLayout, iCurrentPlayerType );
        control1->iDialogCustomControl = control;
        GetUPnPPlaybackDialogCustomControl()->MakeVisible( ETrue );
        MakeVisible(EFalse);
        }

    // Update view
    // Ignore errors from updating labels in view since if labels
    // are empty, still want to activate view
    iPlaybackState = iPlaybackUtility->StateL();
    TRAP_IGNORE( UpdateViewL() );

    if ( ( iPlaybackState == EPbStateInitialising ) ||
         ( iPlaybackState == EPbStateDownloading ) )
        {
        delete iMedia;
        iMedia = NULL;
        UpdatePlaybackState( iPlaybackState );
        UpdateTrackInfoL( iMedia );
        }
    else if ( iMedia == NULL )
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

    CAknDialog::PreLayoutDynInitL();
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::OkToExitL
// This function returns whether it's ok to exit or not
// -----------------------------------------------------------------------------
//
TBool CMPXUPnPPlaybackDialog::OkToExitL( TInt aButtonId )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::OkToExitL" );
    TBool retVal = EFalse;
    switch (aButtonId)
        {
        case EAknSoftkeyClose:
            {
            if ( iToolbar )
                {
                iToolbar->SetToolbarVisibility( EFalse );
                iToolbar->SetToolbarObserver( NULL );
                iToolbar->RemoveItem( EMPXPbvCmdPreviousListItem );
                iToolbar->RemoveItem( EMPXPbvCmdPlayPause );
                iToolbar->RemoveItem( EMPXPbvCmdNextListItem );
                iToolbar = NULL;
                }
            if ( !Layout_Meta_Data::IsLandscapeOrientation() )
                {
                PrepareStatusPaneForExitingViewL();
                }

            RestorePreviousStatusPaneL();
            iPlaybackUtility->CommandL( EPbCmdStop );
            iPlaybackUtility->CommandL( EPbCmdClose );
            retVal = ETrue;
            break;
            }
        default:
            {
            retVal = CAknDialog::OkToExitL( aButtonId );
            break;
            }
        }
    if ( retVal )
        {
        iMPXUtility->CancelRequest();

        // We need to unload the current plugin to release WLAN connection
        iPlaybackUtility->PlayerManager().ClearSelectPlayersL();
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::BackupPreviousStatusPaneL
// Backs up Status Pane information.
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::BackupPreviousStatusPaneL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::BackupPreviousStatusPaneL" );
    CEikStatusPane* sp = iAvkonAppUi->StatusPane();
    // Backup navi pane
    iNaviPane = static_cast< CAknNavigationControlContainer* >
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    // Backup title pane
    iTitlePane = static_cast< CAknTitlePane* >
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    iOrigTitle = iTitlePane->Text()->AllocL();
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::RestorePreviousStatusPaneL()
// Restores previous status pane.
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::RestorePreviousStatusPaneL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::RestorePreviousStatusPaneL" );
    if ( iOrigTitle && iTitlePane )
        {
        // Set original title pane
        iTitlePane->SetTextL( *iOrigTitle );
        }
    // Set original navi pane
    if ( iNaviPane )
        {
        iNaviPane->Pop();
        }
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::DynInitMenuPaneL" );
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::CountComponentControls
// Counts the number of components.
// -----------------------------------------------------------------------------
//
TInt CMPXUPnPPlaybackDialog::CountComponentControls() const
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::CountComponentControls" );
    TInt newitems = 0;
    if ( GetUPnPPlaybackDialogCustomControl() ) newitems++;
    return CAknDialog::CountComponentControls() + newitems;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::ComponentControl
// Returns component control.
// -----------------------------------------------------------------------------
//
CCoeControl* CMPXUPnPPlaybackDialog::ComponentControl( TInt aIndex ) const
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::ComponentControl" );
    if (aIndex == CAknDialog::CountComponentControls() )
        {
        return GetUPnPPlaybackDialogCustomControl();
        }
    return CAknDialog::ComponentControl( aIndex );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXUPnPPlaybackDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::OfferKeyEventL()" );
    MPX_DEBUG4( "CMPXUPnPPlaybackDialog::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    TKeyResponse keyResponse( EKeyWasNotConsumed );

    if ( ( iLastKeyEvent.iScanCode == aKeyEvent.iScanCode ) &&
         ( iLastKeyType == aType ) &&
         ( iLastKeyType == EEventKeyUp ) )
        {
        keyResponse = EKeyWasConsumed;
        }
    else
        {
        iLastKeyEvent = aKeyEvent;
        iLastKeyType = aType;

        // End key was pressed and dialog needs to close
        if ( aKeyEvent.iCode == EKeyEscape )
            {
            TryExitL(EAknSoftkeyClose);
            keyResponse = EKeyWasConsumed;
            }
        else
            {
            if ( GetUPnPPlaybackDialogCustomControl() )
                {
                keyResponse = GetUPnPPlaybackDialogCustomControl()->OfferKeyEventL(
                                                                    aKeyEvent,
                                                                    aType );
                }
            }
        }
    return keyResponse;
    }

// ---------------------------------------------------------------------------
// From MMPXCustomCommandObserver.
// Handle custom commands.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleMediaKeyCommand(
    const TMPXPlaybackCommand aCommand,
    const TInt aData )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleMediaKeyCommand()" );

    switch ( aCommand )
        {
        case EPbCmdSetVolume:
            {
    		TMPXPlaybackState playerState( iPlaybackUtility->StateL() );
            if ( iAvkonAppUi->IsForeground() || playerState == EPbStatePlaying )
                {
				if ( aData != iCurrentVolume )
					{
					SetVolume( aData );
					}
				// popup volume control even if volume didn't change
				HandlePopupL( EPbCmdSetVolume );
				}
            break;
            }
        case EPbCmdMuteVolume:
            {
    		TMPXPlaybackState playerState( iPlaybackUtility->StateL() );
            if ( iAvkonAppUi->IsForeground() || playerState == EPbStatePlaying )
                {
				MuteVolume();
				HandlePopupL( EPbCmdMuteVolume );
				}
            break;
            }
        case EPbCmdUnMuteVolume:
            {
    		TMPXPlaybackState playerState( iPlaybackUtility->StateL() );
            if ( iAvkonAppUi->IsForeground() || playerState == EPbStatePlaying )
                {
				UnMuteVolume();
				HandlePopupL( EPbCmdUnMuteVolume );
				}
            break;
            }
        default:
            {
            if ( iAvkonAppUi->IsForeground() )
                {
                // Forward the command to handle when
                // music is foreground app and not displaying
                // dialogs
                MPX_DEBUG2( "CMPXUPnPPlaybackDialog::HandleCustomCommand sending command %d to view", aCommand );
                TRAP_IGNORE( DoHandleMediaKeyCommandL( aCommand ) );
                }
            else
                {
                TMPXPlaybackState playerState( EPbStateNotInitialised );
                TRAP_IGNORE( playerState = iPlaybackUtility->StateL() );
                // displaying not foreground, pass to view to handle only play command
                if ( playerState == EPbStatePlaying ||
                     playerState == EPbStatePaused ||
                     playerState == EPbStateSeekingForward ||
                     playerState == EPbStateSeekingBackward )
                    {
                    MPX_DEBUG2( "CMPXUPnPPlaybackDialog::HandleCustomCommand not foreground, sending command %d to HandleMediaKeyCommandL", aCommand );
                    TRAP_IGNORE( DoHandleMediaKeyCommandL( aCommand ) );
                    }
                else if ( aCommand == EPbCmdPlay || aCommand == EPbCmdPlayPause )
                    {
                    MPX_DEBUG2( "CMPXUPnPPlaybackDialog::HandleCustomCommand not foreground, active idle foreground, sending command %d to HandleMediaKeyCommandL", aCommand );
                    // not foreground, play without loading playback view
                    TRAP_IGNORE( DoHandleMediaKeyCommandL( aCommand ) );
                    }
                else
                    {
                    MPX_DEBUG2( "CMPXUPnPPlaybackDialog::HandleCustomCommand not foreground, inactive state, command %d ignored", aCommand );
                    }
                }
			}
        }
    }

// ---------------------------------------------------------------------------
// Additional handling of media key commands.
// Determine if the command will be passed to the playback engine.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::DoHandleMediaKeyCommandL(
	TMPXPlaybackCommand aCommand )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::DoHandleMediaKeyCommandL()" );

    TBool handleCommand(EFalse);
    TMPXPlaybackState playerState( iPlaybackUtility->StateL() );

    switch ( aCommand )
        {
        case EPbCmdPlay:
        case EPbCmdPause:
        case EPbCmdPlayPause:
        case EPbCmdStop:
        case EPbCmdNext:
        case EPbCmdPrevious:
        	handleCommand = ETrue;
        	break;
        case EPbCmdStartSeekForward:
            {
            if ( iCurrentPlayerType == EPbLocal  &&
                 ( EPbStatePlaying == playerState ||
                   EPbStatePaused == playerState ||
                   EPbStateSeekingBackward == playerState ) )
                {
        		handleCommand = ETrue;
                }
            break;
            }
        case EPbCmdStartSeekBackward:
            {
            if ( iCurrentPlayerType == EPbLocal  &&
                 ( EPbStatePlaying == playerState ||
                   EPbStatePaused == playerState ||
                   EPbStateSeekingForward == playerState ) )
                {
        		handleCommand = ETrue;
                }
            break;
            }
        case EPbCmdStopSeeking:
            {
            if ( iCurrentPlayerType == EPbLocal  &&
                 ( EPbStateSeekingForward == playerState ||
                   EPbStateSeekingBackward == playerState ) )
                {
        		handleCommand = ETrue;
                }
            // Do nothing, not supported by UPnP
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if ( handleCommand )
        {
        iPlaybackUtility->CommandL( aCommand );

        // Control popup behaviour
        HandlePopupL( aCommand );
        }
    }

// -----------------------------------------------------------------------------
// Control popup behaviour.
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandlePopupL(
    TMPXPlaybackCommand aCommand )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandlePopupL" );

    switch ( aCommand )
        {
        case EPbCmdStop:
            {
            iMediaKeyHandler->DismissNotifier( EDisplayMediaPopup );
            break;
            }
        case EPbCmdSetVolume:
        case EPbCmdMuteVolume:
        case EPbCmdUnMuteVolume:
            {
            if ( iAvkonAppUi->IsForeground() )
                {
				// display pop up volume control
				iMediaKeyHandler->ShowVolumePopupL();
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
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleResourceChange( TInt aType )
    {
    if ( GetUPnPPlaybackDialogCustomControl() )
        {
        GetUPnPPlaybackDialogCustomControl()->HandleResourceChange( aType );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXLayoutSwitchObserver
// Callback function when layout is changed.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::HandleLayoutChange()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialog::HandleLayoutChange()" );

    if (GetUPnPPlaybackDialogCustomControl() && !iSwitchingView)
        {
        if ( !Layout_Meta_Data::IsLandscapeOrientation() )
            {
            if ( iAvkonAppUi->StatusPane()->CurrentLayoutResId() !=
                 R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT )
                {
                TRAP_IGNORE(
                    iAvkonAppUi->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT ));
                }
            }
        else
            {
            if ( iAvkonAppUi->StatusPane()->CurrentLayoutResId() !=
                 R_AVKON_STATUS_PANE_LAYOUT_USUAL )
                {
                TRAP_IGNORE(
                    iAvkonAppUi->StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL ));
                }
            }

        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, iModRect );

        if ( iToolbar && iToolbar->IsVisible() 
                && !Layout_Meta_Data::IsLandscapeOrientation())
            {
            iModRect.iBr.iY -= iToolbar->Rect().Height();
            }
        GetUPnPPlaybackDialogCustomControl()->SetRect( iModRect );

        // Update album art
        TRAP_IGNORE( UpdateAlbumArtL( iMedia ));

        GetUPnPPlaybackDialogCustomControl()->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::CommandNotSupportedL
// Shows a Not supported error note
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::CommandNotSupportedL()
    {
    HBufC* dialogText = StringLoader::LoadLC(R_MPX_MEDIA_KEYS_NOT_SUPPORTED);
    CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
    errNote->ExecuteLD( *dialogText );
    CleanupStack::PopAndDestroy( dialogText );
    }

// -----------------------------------------------------------------------------
// Exits the dialog when callback from CIdle
// -----------------------------------------------------------------------------
//
TInt CMPXUPnPPlaybackDialog::DelayedExit( TAny* aPtr )
    {
    CMPXUPnPPlaybackDialog* upnpPlaybackDialog =
        static_cast<CMPXUPnPPlaybackDialog*>(aPtr);

    TRAP_IGNORE(upnpPlaybackDialog->TryExitL(EAknSoftkeyClose));
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::SetVolume
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::SetVolume( const TInt aVolume )
    {
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdSetVolume );
    cmd->SetTObjectValueL<TUint>( KMPXCommandPlaybackGeneralData, aVolume );
    iPlaybackUtility->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::MuteVolume
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::MuteVolume()
    {
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdMuteVolume );
    iPlaybackUtility->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialog::UnMuteVolume
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UnMuteVolume()
    {
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdUnMuteVolume );
    iPlaybackUtility->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }

// ---------------------------------------------------------------------------
// Updates the middle toolbar button
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::UpdateToolbar()
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        MPX_DEBUG1("AknLayoutUtils::PenEnabled() : true");
        if ( iToolbar && iToolbar->IsVisible())
            {
            MPX_DEBUG1("Toolbar() : true");
            iToolbar->SetDimmed( EFalse );
            CAknButton* pausePlayControl;
            CAknButton* skipControl;
            pausePlayControl = (CAknButton*)(iToolbar->ComponentControl( 1 ));
            MMPXSource* s = iPlaybackUtility->Source();
            if ( s )
                {
                CMPXCollectionPlaylist* playlist = s->PlaylistL();
                if ( playlist )
                    {
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
                        }

                    delete playlist;
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
                skipControl = (CAknButton*)(iToolbar->ComponentControl( 0 ));
                if( skipControl )
                    {
                    skipControl->SetCurrentState(1,ETrue);
                    }

                skipControl = (CAknButton*)(iToolbar->ComponentControl( 2 ));
                if( skipControl )
                    {
                    skipControl->SetCurrentState(1,ETrue);
                    }
                }
            else
                {
                iToolbar->SetDimmed( ETrue );
                }
            }
        else
            {
            MPX_DEBUG1("Toolbar() : false");
            }
        }
    }
// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Used to change toolbar settings before toolbar becomes visible
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialog::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
    {
    CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi());
    CAknToolbar* toolbar = appUi->CurrentFixedToolbar();
    if ( toolbar == aToolbar )
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
void CMPXUPnPPlaybackDialog::OfferToolbarEventL( TInt aCommandId )
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

    HandleCommandL( commandId );
    }
//  End of File
