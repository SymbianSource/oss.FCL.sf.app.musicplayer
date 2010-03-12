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
* Description:  Implementation of Common Playback view's container.
*
*/



// INCLUDE FILES
#include <eiklabel.h>
#include <eikimage.h>
#include <eikprogi.h>
#include <eikmenub.h>
#include <eikappui.h>
#include <eikenv.h>
#include <aknview.h>
#include <AknsUtils.h>
#include <AknDef.h>
#include <AknsConstants.h>
#include <AknsSkinInstance.h>
#include <AknsControlContext.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <barsread.h>
#include <centralrepository.h>
#include <StringLoader.h>
#include <fontids.hrh>
#include <gulfont.h>
#include <applayout.cdl.h>
#include <AknIconArray.h>
#include <AknIconUtils.h>
#include <gulicon.h>
#include <featmgr.h>
#include <aknnotewrappers.h>

#include <data_caging_path_literals.hrh>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <avkon.mbg>
#include <mpxcommonplaybackview.rsg>
#include <mpxcommonplaybackview.mbg>
#include <mpxconstants.h>
#include <mpxfmtx.rsg> // For FF_FMTX
#include <mpxappui.hrh>
#include <mpxinternalcrkeys.h>
#include <touchfeedback.h>
#include <akntranseffect.h>                 // For transition effects
#include <gfxtranseffect\gfxtranseffect.h>  // For transition effects

#include "mpxcommonplaybackviewcontainer.h"
#include "mpxcommonplaybackviewlayout.h"
#include "mpxcommonplaybackview.hrh"
#include "mpxplaybackview.hlp.hrh"
#include "mpxplaybackviewinfolabel.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxlayoutswitchobserver.h"
#include "mpxcommonuihelper.h"
#include "mpxbuttonmanager.h"
#include "mpxlog.h"

// CONSTANTS
const TInt KLabelCount = ETextCount;
const TInt KMPXMinSecSeparatorIndex = 2;

_LIT(KMPXZeroDurationMark, "--");

const TInt KOneHourInSeconds = 3600;
const TInt KMPXTimeIndicatorLength = 16;

// S60 softkey positions in CBA
const TInt KMPXCbaLeftSoftkeyPos = 0;
const TInt KMPXCbaRightSoftkeyPos = 2;

// delay for actual seek operation
const TInt KSeekDelay = 100000; // 0.1s

const TInt KOneSecondInOneMilliSecond = 1000;
// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::ConstructL( const TRect& /*aRect*/ )
    {
    MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::ConstructL() entering" );

    CreateWindowL();

    TAknWindowLineLayout screenLayout =
        AknLayout::screen();
    TRect screenRect = screenLayout.Rect();
	iBackground = CAknsBasicBackgroundControlContext::NewL(
			KAknsIIDQsnBgAreaMain, screenRect, EFalse );

	iSeekTimer = CPeriodic::NewL( EPriorityLow );

    TInt flags( 0 );
    CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
    User::LeaveIfError( repository->Get( KMPXMPLocalVariation, flags ));
    delete repository;
    repository = NULL;

    iEnableMarquee = static_cast<TBool>( flags & KMPXMarqueeArtistName );

    iEnableButtons = static_cast<TBool>( flags & KMPXRockerMappingSupport );


    // Create labels
    CreateLabelsL();
    iLabels[ETextPlaylistCount]->MakeVisible( ETrue );

	if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
		{
    	// By default, FMTX label is displayed
    	iLabels[ETextFMFrequency]->MakeVisible( ETrue );
    	SetFMTXMode(ETrue);
		}

    // Create icons
    CreateIconsL();

    // Create buttons if enabled
    if ( iEnableButtons )
        {
        TRect rockerParentRect;
        if ( iCurrentLayout.Width())
        {
            rockerParentRect = iCurrentLayout;
        }
        else
        {
            rockerParentRect = iCurrentLayout;
        }

        iButtonManager = CMPXButtonManager::NewL(
            this,
            iLayout,
            *this);

        iButtonManager->SetContainerWindowL( *this );
        iButtonManager->SetMopParent( this );
        iButtonManager->SetRect(rockerParentRect);
        }

     // Read time format strings from AVKON resource
    iMinSecFormatString = StringLoader::LoadL(
        R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO );

    iShortFormatString =
        StringLoader::LoadL( R_QTN_TIME_DURAT_SHORT_WITH_ZERO );

    iCommonUiHelper = CMPXCommonUiHelper::NewL();

    // Refresh the progress bar
    UpdateProgressBarGraphics();

    iMode = EUninitialized;
    iLabels[ETextDownloadState]->MakeVisible( ETrue );
    iLabels[ETextArtist]->MakeVisible( EFalse );
    iLabels[ETextTrack]->MakeVisible( EFalse );
    iLabels[ETextElapsedTime]->MakeVisible( EFalse );
    iLabels[ETextRemainingTime]->MakeVisible( EFalse );
    iDisplayAlbumArt = EFalse;
    ActivateL();

    iTouchDown = EFalse;
    iActiveView = ETrue;
    iPrerollCompleted = EFalse;

    iLightStatus = CHWRMLight::ELightStatusUnknown;
    iIsForeground = EFalse ;
    iLight = CHWRMLight::NewL(this); 

	DrawableWindow()->SetPointerGrab( ETrue );
    EnableDragEvents();

    MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::ConstructL() exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXCommonPlaybackViewContainer::~CMPXCommonPlaybackViewContainer()
    {
    // Pointer events
    SetGloballyCapturing( EFalse );
    SetPointerCapture( EFalse );

    FreeIcons();
    delete iTrackAlbumArt;

    delete iBackground;
    iLabels.ResetAndDestroy();
    delete iMinSecFormatString;
    delete iShortFormatString;
    delete iCommonUiHelper;

    if ( iLight )
        {        
        delete iLight;
        iLight = NULL;
        }

    delete iButtonManager;

   MTouchFeedback* feedback = MTouchFeedback::Instance();
   if (feedback)
       {
       feedback->RemoveFeedbackForControl(this);
       }

    if ( iIdle )
        {
        iIdle->Cancel();
        delete iIdle;
        }

	if ( iSeekTimer )
		{
		iSeekTimer->Cancel();
		delete iSeekTimer;
		}
    }

// ---------------------------------------------------------------------------
// Updates time indicators.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateTimeIndicatorsL(
    TInt aPlaybackPosInSeconds,
    TInt aTotalLengthInSeconds )
    {
    iTrackTotalLengthInSeconds = aTotalLengthInSeconds;
    if (!iTouchDown)
        {
        if( iDragProgressBar )
          {
          TInt expectPlaybackPos = iNewSongPosition / KOneSecondInOneMilliSecond;
          if( expectPlaybackPos == aPlaybackPosInSeconds )
              {
              iDragProgressBar = EFalse;
              }
          else
              {
              return;
              }
          }
    if ( aTotalLengthInSeconds == 0 )
        {
        TLocale locale;
        TBuf<KMPXTimeIndicatorLength> pos;
        TChar separator = locale.TimeSeparator( KMPXMinSecSeparatorIndex );
        pos = KMPXZeroDurationMark;
        pos.Append( separator );
        pos += KMPXZeroDurationMark;

        UpdateLabelL( ETextElapsedTime, pos );
        UpdateLabelL( ETextRemainingTime, pos );
        }
    else
        {
        CMPXCommonUiHelper::TMPXDuratDisplayMode durationMode =
            CMPXCommonUiHelper::EMPXDuratAuto;
        if ( aTotalLengthInSeconds > KOneHourInSeconds )
            {
            durationMode = CMPXCommonUiHelper::EMPXDuratHMS;
            }

        // Convert ellapsed time to texts
        HBufC* position = iCommonUiHelper->DisplayableDurationL(
            aPlaybackPosInSeconds,
            durationMode );
        CleanupStack::PushL( position );

        TInt remainingTime = aTotalLengthInSeconds - aPlaybackPosInSeconds;
        if ( remainingTime < 0 )
            {
            // Convert all negative value to zero
            remainingTime = 0;
            }

        // Convert remaining time to texts
        HBufC* remaining = iCommonUiHelper->DisplayableDurationL(
            remainingTime,
            durationMode );
        CleanupStack::PushL( remaining );

        UpdateLabelL( ETextElapsedTime, *position );
        UpdateLabelL( ETextRemainingTime, *remaining );

        CleanupStack::PopAndDestroy( remaining );
        CleanupStack::PopAndDestroy( position );
        }

    if ( ( iMode != EUninitialized )
         || ( iMode == EPlayMode || iMode == EPauseMode ) )
        {
        if ( aTotalLengthInSeconds )
            {
            TReal t = aTotalLengthInSeconds;
            iPlaybackRatio = aPlaybackPosInSeconds / t;
            }
        RefreshProgressBar();
        }
        }
    }

// ---------------------------------------------------------------------------
// Updates label
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateLabelL(
    TMPXPbvTextIndicator aLabel,
    const TDesC& aText )
    {
    ASSERT( aLabel >= 0 && aLabel < iLabels.Count() );
    CMPXPlaybackViewInfoLabel* label = iLabels[aLabel];
    if(aLabel == ETextDownloadState)
        {
        iSliderPaneRect = iLayout->IndicatorLayout( Rect(), ESliderPane );
        iLayout->LayoutLabel(
        iLabels[ETextDownloadState], iSliderPaneRect, ETextDownloadState );

        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb color = KRgbBlack;

        AknsUtils::GetCachedColor(
            skin,
            color,
            KAknsIIDQsnTextColors,
            EAknsCIQsnTextColorsCG50 );

        AknLayoutUtils::OverrideControlColorL(
            *iLabels[ETextDownloadState],
            EColorLabelText,
            color );
        }
    if ( label )
        {
        TBool needRedraw = EFalse;

        if ( label->FullText() != aText )
            {
            needRedraw = ETrue;
            label->SetLabelTextL( aText);
            }

        if ( needRedraw && label->IsVisible() )
            {
            label->StartMarquee();
            label->DrawDeferred();
            }
        }
    }

// ---------------------------------------------------------------------------
// Sets main pane mode.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::SetMode( TMPXPbvPlaybackMode aMode )
    {
    iMode = aMode;
    iDisplayAlbumArt = ETrue;

    switch ( iMode )
        {
	    case EUninitialized:
	    	{
	    	// Set label visibility
	        iLabels[ETextDownloadState]->MakeVisible( ETrue );
	        iLabels[ETextArtist]->MakeVisible( ETrue );
	        iLabels[ETextTrack]->MakeVisible( ETrue );
	        //iLabels[ETextEmpty]->MakeVisible( EFalse );
	        iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
	        iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);
        	break;
        	}
        case ENoTracksMode:
            {
            // Set label visibility
            iLabels[ETextDownloadState]->MakeVisible( ETrue );
            iLabels[ETextArtist]->MakeVisible( EFalse );
            iLabels[ETextTrack]->MakeVisible( EFalse );
            //iLabels[ETextEmpty]->MakeVisible( ETrue );
            iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
            iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);
            iDisplayAlbumArt = EFalse;
            break;
            }
        case EStopMode:
            {
            // Fixed the layout for Download label and time label are overlaped in position
            iLabels[ETextDownloadState]->MakeVisible( EFalse );
            iLabels[ETextTrack]->MakeVisible( ETrue );
            iLabels[ETextArtist]->MakeVisible( ETrue );
            //iLabels[ETextEmpty]->MakeVisible( EFalse );
            iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
            iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);
            break;
            }
        case EInitialising:
            {
            iLabels[ETextDownloadState]->MakeVisible( ETrue );
            iLabels[ETextTrack]->MakeVisible( ETrue );
            iLabels[ETextArtist]->MakeVisible( ETrue );
            //iLabels[ETextEmpty]->MakeVisible( EFalse );
            iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
            iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);
            break;
            }
        case EMediaKeySkipping:
            {
            iLabels[ETextDownloadState]->MakeVisible( ETrue );
            iLabels[ETextTrack]->MakeVisible( ETrue );
            iLabels[ETextArtist]->MakeVisible( ETrue );
            //iLabels[ETextEmpty]->MakeVisible( EFalse );
            iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
            iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);
            break;
            }
        case EInvalidMode:
            {
            iLabels[ETextDownloadState]->MakeVisible( EFalse );
            iLabels[ETextTrack]->MakeVisible( EFalse );
            iLabels[ETextArtist]->MakeVisible( EFalse );
            //iLabels[ETextEmpty]->MakeVisible( EFalse );
            iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
            iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);

            break;
            }
        case EBufferingMode:
            {
            // Set label visibility
            iLabels[ETextDownloadState]->MakeVisible( EFalse );
            iLabels[ETextArtist]->MakeVisible( ETrue );
            iLabels[ETextTrack]->MakeVisible( ETrue );
            //iLabels[ETextEmpty]->MakeVisible( EFalse );
            iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
            iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);
            break;
            }
        default:
            {
            // Set label visibility
            iLabels[ETextDownloadState]->MakeVisible( EFalse );
            iLabels[ETextArtist]->MakeVisible( ETrue );
            iLabels[ETextTrack]->MakeVisible( ETrue );
            //iLabels[ETextEmpty]->MakeVisible( EFalse );
            iLabels[ETextElapsedTime]->MakeVisible( ETrue && iPrerollCompleted);
            iLabels[ETextRemainingTime]->MakeVisible( ETrue && iPrerollCompleted);
            break;
            }
        }

    if ( iMode == EPlayMode )
        {
        iLabels[ETextArtist]->StartMarquee();
        iLabels[ETextTrack]->StartMarquee();
        }
    else
        {
        iLabels[ETextArtist]->StopMarquee();
        iLabels[ETextTrack]->StopMarquee();
        }
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// Set random mode.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::SetRandomMode( TBool aRandomMode )
    {
    iRandomMode = aRandomMode;
    Window().Invalidate( iRandomIconRect );
    }

// ---------------------------------------------------------------------------
// Set repeat mode.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::SetRepeatMode(
    TBool aRepeatAll,
    TBool aRepeatOne )
    {
    iRepeatAllMode = aRepeatAll;
    iRepeatOneMode = aRepeatOne;
    Window().Invalidate( iRepeatIconRect );
    }

// ---------------------------------------------------------------------------
// Set RealPlayer mode.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::SetRealAudioMode(
    TBool aRealAudioMode )
    {
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    iRealAudioMode = aRealAudioMode;
    Window().Invalidate( iRealIconRect );
#else
    (void) aRealAudioMode;
    ASSERT( 0 );
#endif
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::UpdateButtons
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateButtons(
    TMPXPlaybackState aState )
    {
    if ( iEnableButtons )
        {
        iButtonManager->UpdateButtonStates( aState );
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateLayout()
    {
    MPX_FUNC( "CMPXCommonPlaybackViewContainer::UpdateLayout" );
    TRAP_IGNORE( DoUpdateLayoutL() );
    }

// ---------------------------------------------------------------------------
// Updates duration label.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateDurationLabelL(
    TInt aTotalLengthInSeconds )
    {
    HBufC* labelText( NULL );
    HBufC* duration( NULL );

    if ( aTotalLengthInSeconds == 0 )
        {
        TLocale locale;
        TChar separator = locale.TimeSeparator( KMPXMinSecSeparatorIndex );

        duration = HBufC::NewLC( KMPXTimeIndicatorLength );
        TPtr durationTPtr( duration->Des() );

        durationTPtr = KMPXZeroDurationMark;
        durationTPtr.Append( separator );
        durationTPtr += KMPXZeroDurationMark;
        }
    else
        {
        // Convert track duration to text
        duration = iCommonUiHelper->DisplayableDurationL(
            aTotalLengthInSeconds );
        CleanupStack::PushL( duration );
        }

    labelText = StringLoader::LoadLC(
        R_MPX_PBV_TRACK_DURATION_FORMAT,
        *duration );

    CleanupStack::PopAndDestroy( labelText );
    CleanupStack::PopAndDestroy( duration );
    }

// ---------------------------------------------------------------------------
// Force softkey label colors to what is specified in skin
// for Music Player soft keys.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateSoftkeyColors()
    {
    CEikButtonGroupContainer* cbgc = CEikButtonGroupContainer::Current();
    MEikButtonGroup* bg = NULL;
    if ( cbgc )
        {
        bg = cbgc->ButtonGroup();
        }

    if ( bg )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();

        TRgb color = KRgbBlack;
        CCoeControl* lsk = cbgc->ControlOrNull(
            bg->CommandId( KMPXCbaLeftSoftkeyPos ) );
        if ( lsk )
            {
            // Color is not updated if it not found from the skin
            if (!AknsUtils::GetCachedColor(
                skin, color, KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG47 ) )
                {
                TRAP_IGNORE(
                    AknLayoutUtils::OverrideControlColorL( *lsk,
                    EColorLabelText,
                    color ) );
                }
            }

        CCoeControl* rsk = cbgc->ControlOrNull(
            bg->CommandId( KMPXCbaRightSoftkeyPos ) );
        if ( rsk )
            {
            // Color is not updated if it not found from the skin
            if ( !AknsUtils::GetCachedColor(
                skin, color, KAknsIIDQsnTextColors,
                EAknsCIQsnTextColorsCG46 ) )
                {
                TRAP_IGNORE(
                    AknLayoutUtils::OverrideControlColorL( *rsk,
                    EColorLabelText,
                    color ) );
                }
            }
        cbgc->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// Handles foreground events
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::HandleForegroundEventL(
    TBool aForeground )
    {
    MPX_FUNC( "CMPXCommonPlaybackViewContainer::HandleForegroundEventL" );

    iIsForeground = aForeground;

    if ( !aForeground && iEnableButtons )
        {
        // if losing foreground and buttons are enabled
        iButtonManager->HandleForegroundLostL();
        }

    if ( !aForeground && iTouchDown )
        {
        // Pointer events
        SetGloballyCapturing( EFalse );
        SetPointerCapture( EFalse );

        iTouchDown = EFalse;
        }

	// Special case: if going to background but still partially visible
	if( iActiveView && !aForeground )
		{
		CAknAppUi* appUi = (CAknAppUi*)CEikonEnv::Static()->AppUi();
		if( !appUi->IsFaded() )
		    {
		    iActiveView = EFalse;
		    }
		}
	else
		{
		iActiveView = aForeground;
		}

    RefreshProgressBar();
    }

// ---------------------------------------------------------------------------
// From MMPXAlbumArtUtilObserver
// Notify that extraction of album art started.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::ExtractAlbumArtStarted()
    {
    MPX_DEBUG1("CMPXCommonPlaybackViewContainer::ExtractAlbumArtStarted(): Entering");

    MPX_DEBUG1("CMPXCommonPlaybackViewContainer::ExtractAlbumArtStarted(): Exiting");
    }


// ---------------------------------------------------------------------------
// From MMPXAlbumArtUtilObserver
// Album art is extracted from file and ready to use.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::ExtractAlbumArtCompleted(
    CFbsBitmap* aBitmap,
    TInt aErr )
    {
    // Here we are making the assumption that if we get an update on album
    // art, it is Ok to Start displaying the items.
    iLabels[ETextTrack]->MakeVisible( ETrue );
    iLabels[ETextArtist]->MakeVisible( ETrue );
    iLabels[ETextElapsedTime]->MakeVisible( ETrue);
    iLabels[ETextRemainingTime]->MakeVisible( ETrue);
    iPrerollCompleted = ETrue;

    MPX_DEBUG3( "CMPXCommonPlaybackViewContainer::ExtractAlbumArtCompleted(bitmap:0x%x, err:%d)",
        aBitmap, aErr );

    delete iTrackAlbumArt;
    iTrackAlbumArt = NULL;

    if ( aBitmap && KErrNone == aErr )
        {
        iTrackAlbumArt = aBitmap;
        }

    DrawDeferred(); 
    //Window().Invalidate( iAlbumArtRect );
    //Window().Invalidate( iSliderPaneRect );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CMPXCommonPlaybackViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXCommonPlaybackViewContainer::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    TKeyResponse response = EKeyWasNotConsumed;

    if ( iEnableButtons )
        {
        response = iButtonManager->OfferKeyEventL( aKeyEvent, aType );
        if ( response == EKeyWasConsumed )
            {
            CWindowGc& gc = SystemGc();
            gc.Activate( *DrawableWindow() );
//            iButtonManager->RedrawIfNeeded(gc);
            gc.Deactivate();
            }
        }

    if ( response == EKeyWasNotConsumed )
        {
// TODO
/*
        if ( aKeyEvent.iCode == EKeyLeftArrow && !FeatureManager::FeatureSupported(
                    KFeatureIdSideVolumeKeys ))
            {
            iCommandObserver->ProcessCommandL( EMPlayerCmdDecreaseVolume) ;
            response = EKeyWasConsumed;
            }
        else if ( aKeyEvent.iCode == EKeyRightArrow && !FeatureManager::FeatureSupported(
                    KFeatureIdSideVolumeKeys ))
            {
            iCommandObserver->ProcessCommandL( EMPlayerCmdIncreaseVolume );
            response = EKeyWasConsumed;
            }
        else
            {
            // Do nothing
            }
*/
        }

    return response;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange )
        {
        // Reload icons
        TRAP_IGNORE(
            {
            CreateIconsL();
            } );

        // Make sure execute layout again
        iCurrentLayout = TRect();
        UpdateLayout(); // To make sure AknIconUtils::SetSize() gets called
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iLayoutObserver->HandleLayoutChange();
        UpdateBackgroundSkinControlContext( Rect() );

        }
    else
        {
        // pass
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;
/*
    if ( iIsEmbedded )
        {
        // TODO: Handle for embedded mode
        aContext.iContext = KMUS_HLP_PLAYBACK_VIEW;
        }
    else
        {
        aContext.iContext = KMUS_HLP_PLAYBACK_VIEW;
        }
*/
    }

// ---------------------------------------------------------------------------
// Starts marquee for a playback label
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::StartMarquee(
    TMPXPbvTextIndicator aLabel )
    {
    MPX_FUNC( "CMPXCommonPlaybackViewContainer::StartMarquee" );
    ASSERT( aLabel >= 0 && aLabel < iLabels.Count() );
    CMPXPlaybackViewInfoLabel* label = iLabels[aLabel];
    if ( label )
        {
        if ( label->IsVisible() )
            {
            label->StartMarquee();
            label->DrawDeferred();
            }
        }
    }

// ---------------------------------------------------------------------------
// Creates labels.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::CreateLabelsL()
    {
    CMPXPlaybackViewInfoLabel* label = NULL;

    for ( TInt index = 0; index < KLabelCount; ++index )
        {
        label = new ( ELeave ) CMPXPlaybackViewInfoLabel();
        CleanupStack::PushL( label );
        label->SetContainerWindowL( *this );
        label->SetTextL( KNullDesC );
        label->ConstructL(
            iEnableMarquee && ( index == ETextTrack ) );

        label->SetBackground( iBackground );
        User::LeaveIfError( iLabels.Append( label ) );
        CleanupStack::Pop( label );
        }
    }

// ---------------------------------------------------------------------------
// Creates icons.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::CreateIconsL()
    {
    FreeIcons();

    // Play/stop/pause icons
    iPlayIcon =
        iLayout->GetIndicatorIconMaskL( EIconPlay );
    iPauseIcon =
        iLayout->GetIndicatorIconMaskL(EIconPause );

    // Repeat, random, etc
    iRepeatAllIcon =
        iLayout->GetIndicatorIconMaskL( EIconRepeatAll );
    iRepeatOneIcon =
        iLayout->GetIndicatorIconMaskL( EIconRepeatOne );
    iRandomIcon =
        iLayout->GetIndicatorIconMaskL( EIconRandom );
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    iRealIcon =
        iLayout->GetIndicatorIconMaskL( EIconReal );
#endif

    // Download slider
    iDownloadSlider = iLayout->GetIndicatorIconMaskL(
        EDownloadSlider );
    iPlaybackSlider = iLayout->GetIndicatorIconMaskL(
        EPlaybackSlider );
    iSliderBackground = iLayout->GetIndicatorIconMaskL(
        ESliderBackground );

    // Default album art
    iDefaultAlbumArt = iLayout->GetIndicatorIconMaskL(
        EAlbumArtArea );
    }

// ---------------------------------------------------------------------------
// Deletes all icon objects.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::FreeIcons()
    {
    delete iPauseIcon;
    iPauseIcon = NULL;
    delete iPlayIcon;
    iPlayIcon = NULL;
    delete iRandomIcon;
    iRandomIcon = NULL;
    delete iRepeatOneIcon;
    iRepeatOneIcon = NULL;
    delete iRepeatAllIcon;
    iRepeatAllIcon = NULL;
    delete iDownloadSlider;
    iDownloadSlider = NULL;
    delete iPlaybackSlider;
    iPlaybackSlider = NULL;
    delete iSliderBackground;
    iSliderBackground = NULL;
    delete iDefaultAlbumArt;
    iDefaultAlbumArt = NULL;
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    delete iRealIcon;
    iRealIcon = NULL;
#endif
    }

// ---------------------------------------------------------------------------
// Draw an icon
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::DrawIndicator(
    CWindowGc& aGc ,
    const TRect& aDirtyRect,
    const TRect& aIndicatorRect,
    const CGulIcon* aIcon,
    TBool aInvertMask ) const
    {
    if ( aIcon && aIndicatorRect.Intersects( aDirtyRect ) )
        {
        aGc.BitBltMasked(
            aIndicatorRect.iTl,
            aIcon->Bitmap(),
            TRect( aIndicatorRect.Size() ),
            aIcon->Mask(),
            aInvertMask );
        }
    }

// ---------------------------------------------------------------------------
// Redraw part of the screen.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::RedrawRect(
    const TRect& aRect) const
    {
	CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

	AknsDrawUtils::Background( skin, iBackground,
			this, gc, aRect );

    TBool embedded = iEikonEnv->StartedAsServerApp();

    if ( iPrerollCompleted )
        {
        if ( iRepeatAllMode && !embedded )
            {
            DrawIndicator( gc, aRect, iRepeatIconRect, iRepeatAllIcon );
            }

        if ( iRepeatOneMode && !embedded )
            {
            DrawIndicator(gc, aRect, iRepeatIconRect, iRepeatOneIcon);
            }

        if ( iRandomMode && !embedded )
            {
            DrawIndicator(gc, aRect, iRandomIconRect, iRandomIcon);
            }

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
        if ( iRealAudioMode )
            {
            DrawIndicator( gc, aRect, iRealIconRect, iRealIcon );
            }
#endif

        DrawIndicator( gc, aRect, iSliderBackgroundRect, iSliderBackground );
        if ( iMode == EPlayMode || iMode == EPauseMode )
            {

            DrawIndicator( gc, aRect, TRect(iDownloadSliderRect.iTl,
                    iDownloadSlider->Bitmap()->SizeInPixels()),
                    iDownloadSlider );
            DrawIndicator(gc, aRect, TRect(iPlaybackSliderRect.iTl,
                    iPlaybackSlider->Bitmap()->SizeInPixels() ),
                    iPlaybackSlider );
            }

        if ( iDisplayAlbumArt && aRect.Intersects( iAlbumArtRect ) )
            {
            if ( iTrackAlbumArt )
                {
                // Calculating the CenterPoint for Drawing the albumart image 
                TSize bmpSizeInPixels = iTrackAlbumArt->SizeInPixels();
                TInt xPos = ( iAlbumArtRect.Width() - bmpSizeInPixels.iWidth )
                        / 2;
                TInt yPos =
                        ( iAlbumArtRect.Height() - bmpSizeInPixels.iHeight )
                                / 2;
                TPoint Centerpos = TPoint(xPos, yPos );
                // bitmap top left corner position
                Centerpos += iAlbumArtRect.iTl;

                // Draw album art and frame
                gc.BitBlt( Centerpos, iTrackAlbumArt);
                TRgb color = KRgbBlack;
                AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                        EAknsCIQsnTextColorsCG50 );
                TRect imageRect( Centerpos, iTrackAlbumArt->SizeInPixels() );
                gc.SetPenStyle( CGraphicsContext::ESolidPen );
                gc.SetBrushStyle( CGraphicsContext::ENullBrush );
                gc.SetPenColor( color );
                gc.DrawRect( imageRect );
                }
            else
                {
                DrawIndicator( gc, aRect, iAlbumArtRect, iDefaultAlbumArt );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Update progress bar graphics based on current playback
// and download position.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateProgressBarGraphics()
    {
    MPX_FUNC("CMPXCommonPlaybackViewContainer::UpdateProgressBarGraphics()");
    TSize downloadBarSize = iDownloadSliderRect.Size();
    TSize playbackBarSize = iPlaybackSliderRect.Size();

    downloadBarSize.iWidth *= iDownloadRatio;
    playbackBarSize.iWidth *= iPlaybackRatio;

    AknIconUtils::SetSize(
        iDownloadSlider->Bitmap(),
        downloadBarSize,
        EAspectRatioNotPreserved );
    AknIconUtils::SetSize(
        iPlaybackSlider->Bitmap(),
        playbackBarSize,
        EAspectRatioNotPreserved );
    }

// ---------------------------------------------------------------------------
// Update progress bar graphics and redraw.
// Refresh happens only when backlight is ON and
// the UI is in foreground. 
// Note: Some display types may not need backlight. In that case
// code may need to be adjusted accordingly.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::RefreshProgressBar()
    {
    MPX_FUNC( "CMPXCommonPlaybackViewContainer::RefreshProgressBar" );

    MPX_DEBUG2(" iIsForeground : (%d)", iIsForeground);
    MPX_DEBUG2(" iLightStatus : (%d)", iLightStatus);

    if ( iIsForeground && (iLightStatus == CHWRMLight::ELightOn) )
		{
		UpdateProgressBarGraphics();
		Window().Invalidate( iSliderPaneRect );
		}
    }

// ---------------------------------------------------------------------------
// Update label text color according to skin
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateLabelColorsL()
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb color = KRgbBlack;
    // Color is not updated if it not found from the skin
    AknsUtils::GetCachedColor(
        skin,
        color,
        KAknsIIDQsnTextColors,
        EAknsCIQsnTextColorsCG6 );

    for ( TInt i = 0; i < iLabels.Count(); i++ )
        {
        AknLayoutUtils::OverrideControlColorL(
            *iLabels[i],
            EColorLabelText,
            color );
        }
    }

// ---------------------------------------------------------------------------
// Update skin control context for background
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::UpdateBackgroundSkinControlContext(
    const TRect& aRect )
    {
    CAknsBasicBackgroundControlContext* background =
        static_cast<CAknsBasicBackgroundControlContext*>( iBackground );
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        TAknWindowLineLayout screenLayout =
            AknLayout::screen();
        TRect screenRect = screenLayout.Rect();
        background->SetBitmap( KAknsIIDQsnBgAreaMain );
        background->SetRect( screenRect );

        }
    else
        {
        background->SetBitmap( KAknsIIDQsnBgAreaMain );
        background->SetRect( aRect );        
        }

    }

// ---------------------------------------------------------------------------
// Set FMTX mode.
// FMTX label shouldn't appear in some playback views
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::SetFMTXMode(
    TBool aFMTXMode )
    {
	if ( FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
		{
	    iFMTXMode = aFMTXMode;
		}
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Responds to changes to the size and position of the contents of this
// control.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::SizeChanged()
    {
    MPX_FUNC("CMPXCommonPlaybackViewContainer::SizeChanged()");
    UpdateLayout();
    UpdateSoftkeyColors();
    UpdateBackgroundSkinControlContext( Rect() );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMPXCommonPlaybackViewContainer::CountComponentControls() const
    {
    if(iPrerollCompleted)
        {
        return KLabelCount + 1; // ButtonManager
        }
    else
        {
        return 0;
        }
   } 
// ---------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CMPXCommonPlaybackViewContainer::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = NULL;
    if(iPrerollCompleted)
        {
        if (aIndex < KLabelCount)
            {
            control = iLabels[aIndex];
            }
        else
            {
            control = iButtonManager;
            }
        }

    return control;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Draws the control.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::Draw( const TRect& aRect ) const
    {
    RedrawRect(aRect);
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::HandleButtonCommandL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::HandleButtonCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXCommonPlaybackViewContainer::HandleButtonCommandL(%d) entering", aCommand);
    if ( iCommandObserver )
        {
        iCommandObserver->ProcessCommandL( aCommand );
        }
    MPX_DEBUG1("CMPXCommonPlaybackViewContainer::HandleButtonCommandL() exiting");
    }

// ---------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::HandlePointerEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::HandlePointerEventL" );
    if(AknLayoutUtils::PenEnabled())
        {
        TSize playbackBarSize = iPlaybackSliderRect.Size();
        TReal width;
        switch( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Down:
                {
                if ( iSliderPaneRect.Contains(aPointerEvent.iPosition) && 
                        ( iMode == EPlayMode || iMode == EPauseMode ))
                    {
                    // Pointer events
                    SetGloballyCapturing( ETrue );
                    SetPointerCapture( ETrue );

                    MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::HandlePointerEventL btnDOWN" );
                    iTouchDown = ETrue;
                    width = aPointerEvent.iPosition.iX - iSliderBackgroundRect.iTl.iX;
                    playbackBarSize.iWidth = width;
                    AknIconUtils::SetSize(
                        iPlaybackSlider->Bitmap(),
                        playbackBarSize,
                        EAspectRatioNotPreserved );
                    }
                if ( iAlbumArtRect.Contains(aPointerEvent.iPosition ) )
                    {                    
                    MTouchFeedback* feedback = MTouchFeedback::Instance();
                    if (feedback)
                        {
                        feedback->InstantFeedback(ETouchFeedbackBasic);
                        }                
                 
                    if ( iCommandObserver )
        								{
                        iCommandObserver->ProcessCommandL( EMPXCmdVolumeChanged );
                        }
                    }
                break;
                }

            case TPointerEvent::EDrag:
                {
                if(iTouchDown && ( iMode == EPlayMode || iMode == EPauseMode ) )
                    {
                    MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::HandlePointerEventL ****btnDrag****" );

                    MTouchFeedback* feedback = MTouchFeedback::Instance();
                    if (feedback)
                        {
                        feedback->InstantFeedback(ETouchFeedbackSensitive);
                        }

                    if (aPointerEvent.iPosition.iX <= iSliderBackgroundRect.iTl.iX)
                        {
                        width = 0;
                        }
                    else if (aPointerEvent.iPosition.iX >= iSliderBackgroundRect.iBr.iX)
                        {
                        width = iSliderBackgroundRect.iBr.iX - iSliderBackgroundRect.iTl.iX;
                        }
                    else
                        {
                        width = aPointerEvent.iPosition.iX - iSliderBackgroundRect.iTl.iX;
                        }

                    playbackBarSize.iWidth = width;
                    AknIconUtils::SetSize(
                        iPlaybackSlider->Bitmap(),
                        playbackBarSize,
                        EAspectRatioNotPreserved );

                    // Update the ellapsed and remaining time texts for user feedback
                    TReal totalWidth = iPlaybackSliderRect.Width();
                    TReal ratio = width/totalWidth;
                    TInt newSongPositionInSec = ratio * iTrackTotalLengthInSeconds;
                                CMPXCommonUiHelper::TMPXDuratDisplayMode durationMode =
                    CMPXCommonUiHelper::EMPXDuratAuto;
                    if ( iTrackTotalLengthInSeconds > KOneHourInSeconds )
                        {
                        durationMode = CMPXCommonUiHelper::EMPXDuratHMS;
                        }

                    // Convert ellapsed time to text
                    HBufC* position = iCommonUiHelper->DisplayableDurationL(
                        newSongPositionInSec,
                        durationMode );
                    CleanupStack::PushL( position );

                    TInt remainingTime = iTrackTotalLengthInSeconds - newSongPositionInSec;
                    if ( remainingTime < 0 )
                        {
                        remainingTime = 0;
                        }

                    // Convert remaining time to text
                    HBufC* remaining = iCommonUiHelper->DisplayableDurationL(
                        remainingTime,
                        durationMode );
                    CleanupStack::PushL( remaining );

                    UpdateLabelL( ETextElapsedTime, *position );
                    UpdateLabelL( ETextRemainingTime, *remaining );
                    CleanupStack::PopAndDestroy( remaining );
                    CleanupStack::PopAndDestroy( position );

                    // Invalidate rect so it will be redrawn immediately
                    Window().Invalidate( iSliderPaneRect );
                    }
                else
                    {
                    SetGloballyCapturing( EFalse );
                    SetPointerCapture( EFalse );
                    iTouchDown = EFalse;
                    }
                break;
                }

            case TPointerEvent::EButton1Up:
                {
                MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::HandlePointerEventL btnUP" );
                if(iTouchDown)
                    {
                    // Pointer events
                    SetGloballyCapturing( EFalse );
                    SetPointerCapture( EFalse );
                    if ( ( iMode == EPlayMode || iMode == EPauseMode ) )
                        {
                        iTouchDown = EFalse;
                        if ( aPointerEvent.iPosition.iX
                                <= iSliderBackgroundRect.iTl.iX )
                            {
                            width = 0;
                            }
                        else if ( aPointerEvent.iPosition.iX
                                >= iSliderBackgroundRect.iBr.iX )
                            {
                            width = iSliderBackgroundRect.iBr.iX
                                    - iSliderBackgroundRect.iTl.iX;
                            }
                        else
                            {
                            width = aPointerEvent.iPosition.iX
                                    - iSliderBackgroundRect.iTl.iX;
                            }
                        playbackBarSize.iWidth = width;
                        AknIconUtils::SetSize( iPlaybackSlider->Bitmap(),
                                playbackBarSize, EAspectRatioNotPreserved );

                        // Set new song position based upon progress bar location
                        TReal totalWidth = iPlaybackSliderRect.Width();
                        TReal ratio = width / totalWidth;
                        iNewSongPosition = (ratio
                                * iTrackTotalLengthInSeconds) * 1000;

                        // Invalidate rect so it will be redrawn immediately
					    Window().Invalidate( iSliderPaneRect );
					    iSeekTimer->Cancel();
					    iSeekTimer->Start( KSeekDelay, KSeekDelay, TCallBack( CallSetPosition, this ) );
					    iDragProgressBar = ETrue;
                        }
                    }
                break;
                }

            default:
                {
                MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::HandlePointerEventL default" );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::GetNewSongPosition
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMPXCommonPlaybackViewContainer::GetNewSongPosition()
    {
    return iNewSongPosition;
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::LightStatusChanged
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewContainer::LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus )
    {
    MPX_FUNC( "CMPXCommonPlaybackViewContainer::LightStatusChanged" );
    if ( aTarget == CHWRMLight::EPrimaryDisplay )
        {
		MPX_DEBUG2(" LightStatusChanged: (%d)", aStatus);
		if ( ( aStatus == CHWRMLight::ELightOn || aStatus == CHWRMLight::ELightOff ) && aStatus != iLightStatus )
	    	{	
			iLightStatus = aStatus;
			}
        }
    }

// ---------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::SetNewSongPosition
// ---------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewContainer::SetNewSongPosition( const TInt64& aPositon )
    {
    iNewSongPosition = aPositon;
    }

// ---------------------------------------------------------------------------
// Helper function to set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewContainer::DoUpdateLayoutL()
    {
    TRect myRect = Rect();
    if ( myRect == iCurrentLayout )
        {
        // Layout has not changed, do nothing
        return;
        }


    MPX_DEBUG5( "CMPXCommonPlaybackViewContainer::DoUpdateLayoutL() -- new layout: top-left (%d, %d) size (%d, %d)",
        myRect.iTl.iX, myRect.iTl.iY, myRect.Size().iWidth, myRect.Size().iHeight );

    // Get layout data
    iSliderPaneRect = iLayout->IndicatorLayout(
        myRect, ESliderPane );
    iLayout->LayoutLabel(
        iLabels[ETextElapsedTime], iSliderPaneRect, ETextElapsedTime );
    iLayout->LayoutLabel(
        iLabels[ETextRemainingTime], iSliderPaneRect, ETextRemainingTime );
    iLayout->LayoutLabel(
        iLabels[ETextArtist], myRect, ETextArtist );
    iLayout->LayoutLabel(
        iLabels[ETextTrack], myRect, ETextTrack );
    iLayout->LayoutLabel(
        iLabels[ETextPlaylistCount], myRect, ETextPlaylistCount );
    //iLayout->LayoutLabel(
    //    iLabels[ETextEmpty], myRect, ETextEmpty );
    iLayout->LayoutLabel(
        iLabels[ETextDownloadState], iSliderPaneRect, ETextDownloadState );

   	if ( iFMTXMode )
       	{
       	iLayout->LayoutLabel(
           	iLabels[ETextFMFrequency], myRect, ETextFMFrequency );
       	}

    // Check if the artist and title rects intersect, if so move them
    TRect titleRect = iLabels[ETextTrack]->Rect();
    TRect artistRect = iLabels[ETextArtist]->Rect();
    if ( titleRect.Intersects( artistRect ))
        {
        TRect intersection = titleRect;
        intersection.Intersection( artistRect );
        TInt diff = -(intersection.Height());
        iLayout->LayoutLabel(
            iLabels[ETextTrack], myRect, ETextTrack, 0, diff );
        }

    iIconLayout = iLayout->IndicatorLayout( myRect, EIconPlay );
    iRandomIconRect = iLayout->IndicatorLayout( myRect, EIconRandom );
    iRepeatIconRect = iLayout->IndicatorLayout( myRect, EIconRepeatAll );
    iBackgroundLayout = myRect;

    iDownloadSliderRect = iLayout->IndicatorLayout(
        iSliderPaneRect, EDownloadSlider );
    iPlaybackSliderRect = iLayout->IndicatorLayout(
        iSliderPaneRect, EPlaybackSlider );
    iSliderBackgroundRect = iLayout->IndicatorLayout(
        iSliderPaneRect, ESliderBackground );

    iAlbumArtRect = iLayout->IndicatorLayout(
        myRect, EAlbumArtArea );

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    iRealIconRect = iLayout->IndicatorLayout( myRect, EIconReal );
#endif

    // Scale icons
    if ( iPauseIcon )
        {
        AknIconUtils::SetSize(
            iPauseIcon->Bitmap(), iIconLayout.Size() );
        }
    if ( iPlayIcon )
        {
        AknIconUtils::SetSize(
            iPlayIcon->Bitmap(), iIconLayout.Size() );
        }
    if ( iRandomIcon )
        {
        AknIconUtils::SetSize(
            iRandomIcon->Bitmap(), iRandomIconRect.Size() );
        }
    if ( iRepeatAllIcon )
        {
        AknIconUtils::SetSize(
            iRepeatAllIcon->Bitmap(), iRepeatIconRect.Size() );
        }
    if ( iRepeatOneIcon )
        {
        AknIconUtils::SetSize(
            iRepeatOneIcon->Bitmap(), iRepeatIconRect.Size() );
        }

    if ( iSliderBackground )
        {
        AknIconUtils::SetSize(
            iSliderBackground->Bitmap(),
            iSliderBackgroundRect.Size(),
            EAspectRatioNotPreserved );
        UpdateProgressBarGraphics();
        if ( iMode == EPlayMode || iMode == EPauseMode )
           {
            MTouchFeedback* feedback = MTouchFeedback::Instance();
            if ( feedback )
                {
                feedback->SetFeedbackArea( this, 0, iSliderPaneRect,
                    ETouchFeedbackBasic, ETouchEventStylusDown);
                }
           }
        }

    if ( iDefaultAlbumArt )
        {
        AknIconUtils::SetSize(
            iDefaultAlbumArt->Bitmap(), iAlbumArtRect.Size() );
        }

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    if (iRealIcon)
        {
        AknIconUtils::SetSize(
             iRealIcon->Bitmap(), iRealIconRect.Size() );
        }
#endif

    iCurrentLayout = myRect;

    if ( iEnableButtons )
        {
        if (iButtonManager)
            {
            iButtonManager->SetParentRect( myRect );
            iButtonManager->SetRect(myRect);
            }
        }

    // Update label colors according to skin (ignore the color in layout data)
    UpdateLabelColorsL();

    // update artist/title labels to use the full length of control
    // other controls should not have been clipped in the first place
    if ( iLabels[ ETextTrack ]->FullText().Length() > 0 )
        {
        HBufC* title = iLabels[ ETextTrack ]->FullText().AllocLC();
        iLabels[ ETextTrack ]->SetLabelTextL( *title );
        CleanupStack::PopAndDestroy( title );
        }
    if ( iLabels[ ETextArtist ]->FullText().Length() > 0 )
        {
        HBufC* title = iLabels[ ETextArtist ]->FullText().AllocLC();
        iLabels[ ETextArtist ]->SetLabelTextL( *title );
        CleanupStack::PopAndDestroy( title );
        }

    MPX_DEBUG1( "CMPXCommonPlaybackViewContainer::DoUpdateLayoutL() exiting" );
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::ReStoreButtons
// -----------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewContainer::RestoreButtons( 
    TMPXPlaybackState aState )
    {
    if ( iEnableButtons )
        {
        iButtonManager->RestoreButtonStates( aState );
        DrawDeferred();
        }
    }

TInt CMPXCommonPlaybackViewContainer::CallSetPosition()
	{
	TRAP_IGNORE( HandleButtonCommandL( EMPXPbvCmdPosition ) );
	iSeekTimer->Cancel();
	return 0;
	}

TInt CMPXCommonPlaybackViewContainer::CallSetPosition( TAny* aAny )
	{
	return ((CMPXCommonPlaybackViewContainer*)aAny)->CallSetPosition();
	}

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::AdjustOrdinalPosition
// -----------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewContainer::AdjustOrdinalPosition( TInt aNewOrdinalPosition )
    {
    return Window().SetOrdinalPosition( aNewOrdinalPosition );
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::BeginTransition
// -----------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewContainer::BeginTransition()
    {
    iTransitionType = EMPXTranstionToRight;
    const TInt flags = AknTransEffect::TParameter::EActivateExplicitCancel;
    TRect appRect = ((CAknAppUi*)iCoeEnv->AppUi())->ApplicationRect();
    GfxTransEffect::BeginFullScreen( iTransitionType, appRect,
            AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(KAppUidMusicPlayerX, flags));

    // start transition immediately. Other option would be to start it when the album thumb has
    // been received.
    EndTransition();
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::EndTransition
// -----------------------------------------------------------------------------
//
void CMPXCommonPlaybackViewContainer::EndTransition()
    {
    if( iTransitionType != EMPXTranstionNotDefined )
        {
        GfxTransEffect::EndFullScreen();
        iTransitionType = EMPXTranstionNotDefined;
        }
    }

//  End of File
