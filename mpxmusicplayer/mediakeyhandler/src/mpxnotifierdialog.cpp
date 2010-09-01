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
* Description:  Implementation of media key handler
*
*/



// INCLUDE FILES
#include <eikenv.h>
#include <apgwgnam.h>
#include <eikimage.h>
#include <eikappui.h>
#include <gulfont.h>
#include <fontids.hrh>
#include <AknUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <AknsDrawUtils.h>
#include <avkon.rsg>
#include <eiklabel.h>
#include <data_caging_path_literals.hrh>
#include <aknlayoutscalable_apps.cdl.h>
#include <skinlayout.cdl.h>
#include <mpxmediakeyhandler.mbg>
#include <mpxmediakeyhandler.rsg>
#include <StringLoader.h>
#include <mpxuser.h>
#include <activeidle2domainpskeys.h>
#include <mpxpskeywatcher.h>

#include "mpxlog.h"
#include "mpxnotifierdialog.h"

using namespace AknLayoutScalable_Apps;


// CONSTANTS
const TInt KMPXComponentsCount = CMPXNotifierDialog::EMPXComponentsCount;

const TInt KMPXOneSecInMicroSecs( 1000000 );
const TInt KMPXOneHourInSeconds( 3600 );
const TInt KMPXOneHundredHoursInSecs( 360000 );
const TInt KMPXDurationDisplayResvLen( 10 );

const TInt KMPXWindowPosition = 10;             // window group's priority
const TInt KMPXInactivityTime = 4000000;        // 4 seconds - close window after this
_LIT( KMPXInfoPopupWgName, "MPXInfoPopup" );    // window groups name

_LIT( KMPXMediaKeyHandlerMbmFile, "mpxmediakeyhandler.mbm" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXNotifierDialog::CMPXNotifierDialog()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::ConstructL()
    {
    // create a window group for popup
    // needed to prevent apps noticing this window
    RWsSession& wsSession = iCoeEnv->WsSession();
    iWindowGroup = RWindowGroup( wsSession );
    User::LeaveIfError( iWindowGroup.Construct( ( TUint32 )&iWindowGroup, EFalse ) );
    iWindowGroup.AutoForeground( EFalse );
    iEikonEnv->EikAppUi()->AddToStackL( this );

    CApaWindowGroupName* name = CApaWindowGroupName::NewLC(
        wsSession, ( TUint32 )&iWindowGroup );
    name->SetHidden( ETrue );
    name->SetCaptionL( KMPXInfoPopupWgName ); // Available to OOM watcher so this won't be closed
    User::LeaveIfError( name->SetWindowGroupName( iWindowGroup ) );
    CleanupStack::PopAndDestroy( name );

    iWindowGroup.SetOrdinalPosition( 0, ECoeWinPriorityNeverAtFront );

    // create control's window to own window group
    CreateWindowL( &iWindowGroup );

    // create icon & label controls
    iSongLabel = new ( ELeave ) CEikLabel();
    iSongLabel->SetContainerWindowL( *this );
    iSongLabel->SetTextL( KNullDesC );

    iTimeLabel = new ( ELeave ) CEikLabel();
    iTimeLabel->SetContainerWindowL( *this );
    iTimeLabel->SetTextL( KNullDesC );

    CreateIconsL();

    // Timer
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    // for watching when we go active idle
    iPSKeyWatcher = CMPXPSKeyWatcher::NewL( KPSUidAiInformation, KActiveIdleState, this );

    MakeVisible( EFalse );
    ResizeInfoPopup();
    ActivateL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXNotifierDialog* CMPXNotifierDialog::NewL()
    {
    CMPXNotifierDialog* self = new( ELeave ) CMPXNotifierDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXNotifierDialog::~CMPXNotifierDialog()
    {
    iEikonEnv->EikAppUi()->RemoveFromStack( this );
    delete iPSKeyWatcher;
    iWindowGroup.Close();
    if (iTimer)
        iTimer->Cancel();
    delete iTimer;
    delete iSongLabel;
    delete iTimeLabel;
    delete iPlayIcon;
    delete iPauseIcon;
    delete iFFIcon;
    delete iFRIcon;
    }

// ---------------------------------------------------------------------------
// Shows pop-up dialog.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::ShowInfoPopupL(
    TMPXPlaybackPopupIcons aIcon,
    const TDesC& aTitle,
    TInt aTime,
    TMPXPlaybackPopupModes aMode )
    {
    MPX_DEBUG2( "CMPXNotifierDialog::ShowInfoPopupL aMode = %d", aMode );
    if ( ( EMPXPopupShowIfVisible == aMode && !IsVisible() ) ||
        ( EMPXPopupShowIfVisibleRestartTimer == aMode && !IsVisible() ) )
        {
        MPX_DEBUG1( "CMPXNotifierDialog::ShowInfoPopupL do nothing" );
        return;
        }

    UpdateInfoPopupL( aIcon, aTitle, aTime );

    if ( EMPXPopupTimeout == aMode ||
        EMPXPopupShowIfVisibleRestartTimer == aMode )
        {
        MPX_DEBUG1( "CMPXNotifierDialog::ShowInfoPopupL starting timer" );
        iTimer->Cancel();
        iTimer->Start(
            KMPXInactivityTime,
            KMPXInactivityTime,
            TCallBack( TimerCallback, this ) );
        }
    else if ( EMPXPopupNoTimeout == aMode )
        {
        MPX_DEBUG1( "CMPXNotifierDialog::ShowInfoPopupL canceling timer" );
        iTimer->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// Removes pop-up from screen.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::CancelInfoPopup()
    {
    MPX_DEBUG1( "CMPXNotifierDialog::CancelInfoPopup" );
    iTimer->Cancel();
    if ( IsVisible() )
        {
        iWindowGroup.SetOrdinalPosition( 0, ECoeWinPriorityNeverAtFront );
        MakeVisible( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// Gets notified when display orientation hange is started
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::NotifyOrientationChangeBegin()
	{
	// ::HandeResourceChange() will unhide the dialog as soon as the
	// orientation change has been completed. KEikDynamicLayoutVariantSwitch comes
	// too late so the layout is not correct from the beginning 
	if( IsVisible() )
		{
		MakeVisible( EFalse );
		iReopenDialog = ETrue;
		}
	}

// Updates pop-up dialog contents.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::UpdateInfoPopupL(
    TMPXPlaybackPopupIcons aIcon,
    const TDesC& aTitle,
    TInt aTime )
    {
    ShowIcon( aIcon );

    // Set label text
    // magic: allocate 3 chars more (make sure "..." fits).
    HBufC* buf = HBufC::NewLC( aTitle.Length() + 3 );
    *buf = aTitle;
    TPtr ptr( buf->Des() );
    AknTextUtils::ClipToFit(
        ptr,
        *iSongLabel->Font(),
        iSongLabel->Rect().Width() );
    iSongLabel->SetTextL( *buf );
    CleanupStack::PopAndDestroy( buf );

    // Set duration label
    if ( aTime != KErrNotFound )
        {
        HBufC* duration = DisplayableDurationL( aTime );
        CleanupStack::PushL( duration );
        iTimeLabel->SetTextL( *duration );
        CleanupStack::PopAndDestroy( duration );
        }
    else
        {
        iTimeLabel->SetTextL( KNullDesC );
        }

    if ( !IsVisible() )
        {
        // make this window visible
        iWindowGroup.SetOrdinalPosition( 0, KMPXWindowPosition );
        MakeVisible( ETrue );
        }
    DrawDeferred();
    }

// ---------------------------------------------------------------------------
// Create icons.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::CreateIconsL()
    {
    delete iPlayIcon;
    iPlayIcon = NULL;
    iPlayIcon = new ( ELeave ) CEikImage();
    iPlayIcon->SetContainerWindowL( *this );
    ConstructImageL(
        iPlayIcon,
        AknsUtils::SkinInstance(),
        KAknsIIDQgnIndiMupPlay,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        EMbmMpxmediakeyhandlerQgn_indi_mup_play,
        EMbmMpxmediakeyhandlerQgn_indi_mup_play_mask );

    delete iPauseIcon;
    iPauseIcon = NULL;
    iPauseIcon = new ( ELeave ) CEikImage();
    iPauseIcon->SetContainerWindowL( *this );
    ConstructImageL(
        iPauseIcon,
        AknsUtils::SkinInstance(),
        KAknsIIDQgnIndiMupPause,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        EMbmMpxmediakeyhandlerQgn_indi_mup_pause,
        EMbmMpxmediakeyhandlerQgn_indi_mup_pause_mask );
    delete iFFIcon;
    iFFIcon = NULL;
    iFFIcon = new ( ELeave ) CEikImage();
    iFFIcon->SetContainerWindowL( *this );
    ConstructImageL(
        iFFIcon,
        AknsUtils::SkinInstance(),
        KAknsIIDQgnIndiMupForw,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        EMbmMpxmediakeyhandlerQgn_indi_mup_forw,
        EMbmMpxmediakeyhandlerQgn_indi_mup_forw_mask );

    delete iFRIcon;
    iFRIcon = NULL;
    iFRIcon = new ( ELeave ) CEikImage();
    iFRIcon->SetContainerWindowL( *this );
    ConstructImageL(
        iFRIcon,
        AknsUtils::SkinInstance(),
        KAknsIIDQgnIndiMupRew,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        EMbmMpxmediakeyhandlerQgn_indi_mup_rew,
        EMbmMpxmediakeyhandlerQgn_indi_mup_rew_mask );
    }

// ---------------------------------------------------------------------------
// Constructs skinnable icon.
// ---------------------------------------------------------------------------
//
void  CMPXNotifierDialog::ConstructImageL(
    CEikImage* aImage,
    MAknsSkinInstance* aSkin,
    const TAknsItemID& aId,
    const TAknsItemID& aColorId,
    TInt aColorIndex,
    TInt aBitmapId,
    TInt aMaskId )
    {
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    TParse parse;
    parse.Set( KMPXMediaKeyHandlerMbmFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    AknsUtils::CreateColorIconL(
        aSkin,
        aId,
        aColorId,
        aColorIndex,
        bitmap,
        mask,
        iconFile,
        aBitmapId,
        aMaskId,
        KRgbBlack );

    aImage->SetPicture( bitmap, mask ); // Owership transferred to CEikImage
    }

// ---------------------------------------------------------------------------
// Resize info popup.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::ResizeInfoPopup()
    {
    TRect appRect = iEikonEnv->EikAppUi()->ApplicationRect();

    // get parent rectangle (status pane)
    TAknWindowLineLayout statusPane = AknLayout::status_pane( appRect, 0 );
    TAknLayoutRect res;
    res.LayoutRect( appRect, statusPane );
    TRect parentRect = res.Rect();

    // set the rectangle
    TAknWindowComponentLayout l = popup_mup_playback_window();
    res.LayoutRect( parentRect, l.LayoutLine() );
    SetRect( res.Rect() );
    DrawNow();
    }

// ---------------------------------------------------------------------------
// Sets pop-up layouts.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::SetLayoutsL( TBool aLong )
    {
    TAknWindowComponentLayout l;
    // play icon
    l = popup_mup_playback_window_g1();
    AknLayoutUtils::LayoutImage( iPlayIcon, Rect(), l.LayoutLine() );
    // pause icon
    l = popup_mup_playback_window_g1();
    AknLayoutUtils::LayoutImage( iPauseIcon, Rect(), l.LayoutLine() );
    // FF icon
    l = popup_mup_playback_window_g1();
    AknLayoutUtils::LayoutImage( iFFIcon, Rect(), l.LayoutLine() );
    // FR icon
    l = popup_mup_playback_window_g1();
    AknLayoutUtils::LayoutImage( iFRIcon, Rect(), l.LayoutLine() );
    // song label
    TAknTextComponentLayout t = popup_mup_playback_window_t1();
    AknLayoutUtils::LayoutLabel( iSongLabel, Rect(), t.LayoutLine() );
    // time label
    if( aLong )
        {
        t = popup_mup_playback_window_t2( 1 );
        }
    else
        {
        t = popup_mup_playback_window_t2();
        }

    AknLayoutUtils::LayoutLabel( iTimeLabel, Rect(), t.LayoutLine() );

    // Set label color
    TRgb color;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TInt error = AknsUtils::GetCachedColor(
        skin,
        color,
        KAknsIIDQsnTextColors,
        EAknsCIQsnTextColorsCG19 );
    if ( !error )
        {
        AknLayoutUtils::OverrideControlColorL(
            *iSongLabel,
            EColorLabelText,
            color );

        AknLayoutUtils::OverrideControlColorL(
            *iTimeLabel,
            EColorLabelText,
            color );
        }
    }

// ---------------------------------------------------------------------------
// Show playback icon.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::ShowIcon(
    TMPXPlaybackPopupIcons aIcon )
    {

    switch ( aIcon )
        {
        case EMPXPopupPlayIcon:
            {
            iIconShown = EMPXPopupPlayIcon;
            iPlayIcon->MakeVisible( ETrue );
            iPauseIcon->MakeVisible( EFalse );
            iFFIcon->MakeVisible( EFalse );
            iFRIcon->MakeVisible( EFalse );
            break;
            }
        case EMPXPopupPauseIcon:
            {
            iIconShown = EMPXPopupPauseIcon;
            iPlayIcon->MakeVisible( EFalse );
            iPauseIcon->MakeVisible( ETrue );
            iFFIcon->MakeVisible( EFalse );
            iFRIcon->MakeVisible( EFalse );
            break;
            }
        case EMPXPopupFFIcon:
            {
            iIconShown = EMPXPopupFFIcon;
            iFFIcon->MakeVisible( ETrue );
            iPlayIcon->MakeVisible( EFalse );
            iPauseIcon->MakeVisible( EFalse );
            iFRIcon->MakeVisible( EFalse );
            break;
            }
        case EMPXPopupFRIcon:
            {
            iIconShown = EMPXPopupFFIcon;
            iPlayIcon->MakeVisible( EFalse );
            iPauseIcon->MakeVisible( EFalse );
            iFFIcon->MakeVisible( EFalse );
            iFRIcon->MakeVisible( ETrue );
            break;
            }
        default:
            {
            iIconShown = EMPXPopupNoneIcon;
            iPlayIcon->MakeVisible( EFalse );
            iPauseIcon->MakeVisible( EFalse );
            iFFIcon->MakeVisible( EFalse );
            iFRIcon->MakeVisible( EFalse );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Callback for timer
// ---------------------------------------------------------------------------
//
TInt CMPXNotifierDialog::TimerCallback( TAny* aPtr )
    {
    MPX_DEBUG1( "CMPXNotifierDialog::TimerCallback" );
    static_cast<CMPXNotifierDialog*>( aPtr )->CancelInfoPopup();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Convert to displayable duration.
// ---------------------------------------------------------------------------
//
HBufC* CMPXNotifierDialog::DisplayableDurationL(
    TInt aDuration )
    {
    TInt resId;
    HBufC* format = NULL;
    if ( aDuration >= KMPXOneHundredHoursInSecs )
        {
        resId = R_MPX_TIME_DURAT_HHHMMSS_WITH_ZERO;
        format = StringLoader::LoadLC(
            resId, aDuration / KMPXOneHourInSeconds );
        }
    else
        {
        resId = R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO;
        if ( aDuration >= KMPXOneHourInSeconds )
            {
            resId = R_QTN_TIME_DURAT_LONG_WITH_ZERO;
            }
        format = StringLoader::LoadLC( resId );
        }

    SetLayoutsL( aDuration > KMPXOneHourInSeconds? ETrue: EFalse);

    // Convert total playing time to texts.
    TTime durTime( (TInt64)aDuration * KMPXOneSecInMicroSecs );

    HBufC* buf =
        HBufC::NewLC( format->Length() + KMPXDurationDisplayResvLen );
    TPtr bufTPtr( buf->Des() );

    durTime.FormatL( bufTPtr, *format );
    AknTextUtils::LanguageSpecificNumberConversion( bufTPtr );
    CleanupStack::Pop( buf );
    CleanupStack::PopAndDestroy( format );

    return buf;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Draws the control.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::Draw( const TRect& /* aRect */ ) const
    {
    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );

    // background
    AknsDrawUtils::Background( skin, cc, gc, Rect() );

    // skin stuff - find out size of the corner piece
    TAknLayoutRect cornerRect;
    // skinned draw uses submenu popup window skin (skinned border)
    cornerRect.LayoutRect(
        Rect(),
        SkinLayout::Submenu_skin_placing_Line_2() );

    TRect innerRect( Rect() );
    innerRect.Shrink( cornerRect.Rect().Width(), cornerRect.Rect().Height() );

    if ( !AknsDrawUtils::DrawFrame(
            skin,
            gc,
            Rect(),
            innerRect,
            KAknsIIDQsnFrPopupSub,
            KAknsIIDQsnFrPopupCenterSubmenu ) )
        {
        // skinned border failed -> black border
        gc.SetPenStyle( CGraphicsContext::ESolidPen );
        gc.SetBrushColor( KRgbBlack );
        gc.DrawRect( Rect() );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange )
        {
        // Reload icons & layout
        TRAP_IGNORE(
            {
            CreateIconsL();
            ShowIcon( iIconShown );
            SetLayoutsL();
            } );
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        ResizeInfoPopup();
        if( iReopenDialog )
          {
          MakeVisible( ETrue );
          iReopenDialog = EFalse;
          }
        }
    else
        {
        // pass
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Responds to changes to the size and position of the contents of this
// control.
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::SizeChanged()
    {
    TRAP_IGNORE( SetLayoutsL() );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// ---------------------------------------------------------------------------
//
TInt CMPXNotifierDialog::CountComponentControls() const
    {
    return KMPXComponentsCount;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXNotifierDialog::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = NULL;

    switch ( aIndex )
        {
        case EMPXTrackLabel:
            {
            control = iSongLabel;
            break;
            }
        case EMPXTimeLabel:
            {
            control = iTimeLabel;
            break;
            }
        case EMPXPlayIcon:
            {
            control = iPlayIcon;
            break;
            }
        case EMPXPauseIcon:
            {
            control = iPauseIcon;
            break;
            }
        case EMPXFFIcon:
            {
            control = iFFIcon;
            break;
            }
        case EMPXFRIcon:
            {
            control = iFRIcon;
            break;
            }
        default:
            {
            // Pass
            break;
            }
        }

    return control;
    }

// ---------------------------------------------------------------------------
// From MMPXPSKeyObserver
// Handle PS event
// ---------------------------------------------------------------------------
//
void CMPXNotifierDialog::HandlePSEvent( TUid aUid, TInt aKey )
    {
    if ( ( aUid == KPSUidAiInformation ) && ( aKey == KActiveIdleState ) )
        {
        //we want to cancel popup when going to active idle
        CancelInfoPopup();
        }
    }
//  End of File
