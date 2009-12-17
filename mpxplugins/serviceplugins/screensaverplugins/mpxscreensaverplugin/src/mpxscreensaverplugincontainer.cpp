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
* Description:  Implementation of Screen Saver Plugin
*
*/


// INCLUDE FILES

#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <screensaverplugin.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <AknBidiTextUtils.h>
#include <eiklabel.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <StringLoader.h>
#include <bitmaptransforms.h>
#include <e32math.h>

#include <PUAcodes.hrh>
#include <barsread.h>

#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxscreensaverplugin.rsg>
#include <mpxscreensaverplugin.mbg>
#include <mpxplaybackframeworkdefs.h>
#include <mpxlog.h>
#include <mpxuser.h>

#include "mpxscreensaverplugincontainer.h"

// Constants
const TInt KMPXDurationDisplayResvLen = 10;
const TInt KMPXNumBackgrounds = 4;
#define KMPXIndicatorColor KRgbWhite

_LIT( KIconFileName, "mpxscreensaverplugin.mbm" );
_LIT( KMXPanicCategory, "CMPXScreenSaverPluginContainer" );

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::CMPXScreenSaverPluginContainer
// -----------------------------------------------------------------------------
//
CMPXScreenSaverPluginContainer::CMPXScreenSaverPluginContainer(
    MScreensaverPluginHost *aHost ):
    iScreenSaverHost( aHost ),
    iParentControl( NULL ),
    iBackground( KMPXNumBackgrounds )
    {
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::ConstructL
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::ConstructL()
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::ConstructL" );

    iEikEnv = CEikonEnv::Static();
    iTitleLabel = new ( ELeave ) CEikLabel();
    iClockLabel = new ( ELeave ) CEikLabel();
    iAmPmLabel = new ( ELeave ) CEikLabel();

    RefreshDisplayInfo();
    CreateIconsL();
    }

// ----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainerContainer::~CMPXScreenSaverPluginContainerContainer
// ----------------------------------------------------------------------------
//
CMPXScreenSaverPluginContainer::
    ~CMPXScreenSaverPluginContainer()
    {
    MPX_DEBUG1( "CMPXScreenSaverPluginContainer::~CMPXScreenSaverPluginContainer START" );

    delete iPlayIcon;
    delete iPauseIcon;
    delete iMessageIcon;
    delete iMissedCallIcon;
    delete iNewInstantMessageIcon;
    delete iNewVoiceMessageIcon;
    delete iNewEmailIcon;
    delete iKeyguardIcon;

    delete iTitleLabel;
    delete iClockLabel;
    delete iAmPmLabel;
    delete iProfileName;

    iBackground.ResetAndDestroy();
    iBackground.Close();

    if ( iIndicatorFont && iOSBitmapDevice )
        {
        iOSBitmapDevice->ReleaseFont( iIndicatorFont );
        }

    delete iOffScreenBitmap;
    delete iOSBitmapContext;
    delete iOSBitmapDevice;

    MPX_DEBUG1( "CMPXScreenSaverPluginContainer::~CMPXScreenSaverPluginContainer END" );
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::Draw
// -----------------------------------------------------------------------------
//
TInt CMPXScreenSaverPluginContainer::DrawL( CWindowGc& aGc,
    TMPXPlaybackState aPlaybackState )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::DrawL" );

    TInt colorIndex = GetDrawPositionAndColorIndexL();
    if ( !iOffScreenBitmap )
        {
        TInt backgroundHeight = BackgroundHeightL();
        // create an off-screen bitmap, device and context
        iOffScreenBitmap = new ( ELeave ) CFbsBitmap();
        User::LeaveIfError( iOffScreenBitmap->Create(
            TSize( iScreenSize.iWidth, backgroundHeight),
            aGc.Device()->DisplayMode() ) );
        iOSBitmapDevice = CFbsBitmapDevice::NewL( iOffScreenBitmap );
        User::LeaveIfError( iOSBitmapDevice->CreateContext( iOSBitmapContext ) );

        iOSBitmapContext->SetPenColor( KMPXIndicatorColor );
        // brush style has to be null in order for icon mask to work
        iOSBitmapContext->SetBrushColor( KRgbBlack );
        iOSBitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );
        iOSBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );

        // update the layouts
        UpdateLayoutL();

        if ( iIndicatorFont )
            {
            iOSBitmapDevice->ReleaseFont( iIndicatorFont );
            }
        const CFont* origFont = AknLayoutUtils::FontFromId( EAknLogicalFontSecondaryFont );
        TFontSpec fontSpecTitle = origFont->FontSpecInTwips();
        fontSpecTitle.iHeight = iRectIndicators.Height();  // Scale smaller
        iOSBitmapDevice->GetNearestFontToDesignHeightInPixels( iIndicatorFont, fontSpecTitle );

        // reload the backgrounds
        LoadBackgroundsL();
        }

    iOSBitmapContext->Clear();

    DrawBackground( *iOSBitmapContext, colorIndex );
    DrawClockL( *iOSBitmapContext );
    DrawPlaybackStatusL( *iOSBitmapContext, aPlaybackState );
    DrawIndicatorsL( *iOSBitmapContext );

    aGc.SetBrushColor( KRgbBlack );
    aGc.Clear();
    // activate partial mode
    // partial mode is not supported in landscape orientation?
    TScreensaverPartialMode partialMode;
    TRect drawArea = TRect( TPoint(0, iStartY),
        TPoint( iOffScreenBitmap->SizeInPixels().iWidth,
        iOffScreenBitmap->SizeInPixels().iHeight + iStartY) );
    partialMode.iBpp = 8;
    partialMode.iType = EPartialModeTypeMostPowerSaving;
    TInt status = iScreenSaverHost->SetActiveDisplayArea(
        drawArea, partialMode );
    MPX_DEBUG2( "CMPXScreenSaverPluginContainer::DrawL SetActiveDisplayArea status = %d", status );
    aGc.BitBlt( TPoint(0, iStartY), iOffScreenBitmap );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::SetSongPropertiesL
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::SetSongPropertiesL(
    const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::SetSongPropertiesL" );
    iTitleLabel->SetTextL(
        aMedia.ValueText( KMPXMediaGeneralTitle ) );
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::SetNewVoiceMessage
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::SetParam(
    MPXScreenSaverPluginParam aType, TInt aValue )
    {
    MPX_DEBUG3( "-->CMPXScreenSaverPluginContainer::SetParam type = %d, value = %d", aType, aValue );

    switch ( aType )
        {
        case MPXScreenSaverPluginParamMissedCall:
            {
            iMissedCall = aValue;
            break;
            }
        case MPXScreenSaverPluginParamNewMessage:
            {
            iNewMessage = aValue;
            break;
            }
        case MPXScreenSaverPluginParamInstantMessage:
            {
            iNewInstantMessage = aValue;
            break;
            }
        case MPXScreenSaverPluginParamEmail:
            {
            iNewEmail = aValue;
            break;
            }
        case MPXScreenSaverPluginParamVoiceMessage:
            {
            iNewVoiceMessage = aValue;
            break;
            }
        case MPXScreenSaverPluginParamKeyguard:
            {
            iKeyguard = aValue;
            break;
            }
        case MPXScreenSaverPluginParamIsSilent:
            {
            iIsSilent = aValue;
            break;
            }
        default:
            break;
        }
    MPX_DEBUG1( "<--CMPXScreenSaverPluginContainer::SetParam" );
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::SetProfileNameL
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::SetProfileNameL(
    const TDesC& aName, TBool aDisplay )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::SetProfileNameL" );
    delete iProfileName;
    iProfileName = NULL;
    if ( aDisplay )
        {
        iProfileName = aName.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::RefreshDisplayInfo
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::RefreshDisplayInfo()
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::RefreshDisplayInfo" );

    TScreensaverDisplayInfo displayInfo;
    displayInfo.iSize = sizeof( TScreensaverDisplayInfo );
    iScreenSaverHost->DisplayInfo( &displayInfo );
    iParentControl = displayInfo.iParent;

    // screen size from display info may not be full screen size
    iScreenSize = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
    MPX_DEBUG3( "CMPXScreenSaverPluginContainer::RefreshDisplayInfo Screen size: %d %d", iScreenSize.iWidth, iScreenSize.iHeight );

    // delete them so that they will be recreated next time
    delete iOffScreenBitmap;
    iOffScreenBitmap = NULL;
    delete iOSBitmapContext;
    iOSBitmapContext = NULL;
    delete iOSBitmapDevice;
    iOSBitmapDevice = NULL;
    }

// ---------------------------------------------------------------------------
// Create icons.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::CreateIconsL()
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::CreateIconsL" );

    TParse parse;
    parse.Set( KIconFileName, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    iPlayIcon = CreateIconL(
        KAknsIIDQgnIndiMupPlay,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG1,
        iconFile,
        EMbmMpxscreensaverpluginQgn_graf_mup_npv_icon_play,
        EMbmMpxscreensaverpluginQgn_graf_mup_npv_icon_play_mask,
        ETrue );

    iPauseIcon = CreateIconL(
        KAknsIIDQgnIndiMupPause,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG1,
        iconFile,
        EMbmMpxscreensaverpluginQgn_graf_mup_npv_icon_pause,
        EMbmMpxscreensaverpluginQgn_graf_mup_npv_icon_pause_mask,
        ETrue );

    iMessageIcon = CreateIconL(
        KAknsIIDQgnStatMessage,
        KAknsIIDQsnIconColors,
        EAknsMinorDefault,
        iconFile,
        EMbmMpxscreensaverpluginQgn_stat_message,
        EMbmMpxscreensaverpluginQgn_stat_message_mask,
        ETrue );

    iMissedCallIcon = CreateIconL(
        KAknsIIDQgnStatMissedCallsUni,
        KAknsIIDQsnIconColors,
        EAknsMinorDefault,
        iconFile,
        EMbmMpxscreensaverpluginQgn_stat_missed_calls_uni,
        EMbmMpxscreensaverpluginQgn_stat_missed_calls_uni_mask,
        ETrue );

    iNewInstantMessageIcon = CreateIconL(
        KAknsIIDQgnStatImUni,
        KAknsIIDQsnIconColors,
        EAknsMinorDefault,
        iconFile,
        EMbmMpxscreensaverpluginQgn_stat_im_uni,
        EMbmMpxscreensaverpluginQgn_stat_im_uni_mask,
        ETrue );

    iNewVoiceMessageIcon = CreateIconL(
        KAknsIIDQgnStatVm0,
        KAknsIIDQsnIconColors,
        EAknsMinorDefault,
        iconFile,
        EMbmMpxscreensaverpluginQgn_stat_vm0,
        EMbmMpxscreensaverpluginQgn_stat_vm0_mask,
        ETrue );

    iNewEmailIcon = CreateIconL(
        KAknsIIDQgnStatMessageMail,
        KAknsIIDQsnIconColors,
        EAknsMinorDefault,
        iconFile,
        EMbmMpxscreensaverpluginQgn_stat_message_mail,
        EMbmMpxscreensaverpluginQgn_stat_message_mail_mask,
        ETrue );

    iKeyguardIcon = CreateIconL(
        KAknsIIDQgnStatKeyguard,
        KAknsIIDQsnIconColors,
        EAknsMinorDefault,
        iconFile,
        EMbmMpxscreensaverpluginQgn_stat_keyguard,
        EMbmMpxscreensaverpluginQgn_stat_keyguard_mask,
        ETrue );

    LoadBackgroundsL();
    }

// ---------------------------------------------------------------------------
// Create a color icon.
// ---------------------------------------------------------------------------
//
CGulIcon* CMPXScreenSaverPluginContainer::CreateIconL(
    const TAknsItemID& aID,
    const TAknsItemID& aColorId,
    TInt aColorIndex,
    const TDesC& aBitmapFilename,
    TInt aFileBitmapId,
    TInt aFileMaskId,
    TBool aIsIndicator
    )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::CreateIconL" );

    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );

    CFbsBitmap* icon( NULL );
    CFbsBitmap* mask( NULL );

    if ( aColorId == KAknsIIDNone )
        {
        // do not use theme color, use the default color from the file
        AknsUtils::CreateIconLC( skin, aID,
            icon, mask, aBitmapFilename, aFileBitmapId, aFileMaskId );
        }
    else
        {
        // use theme color
        AknsUtils::CreateColorIconLC( skin, aID, aColorId, aColorIndex,
            icon, mask, aBitmapFilename, aFileBitmapId, aFileMaskId, KRgbBlack );
        }

    if ( aIsIndicator )
        {
        // should not be theme colored since everything else is not?
        AknIconUtils::SetIconColor( icon, KMPXIndicatorColor );
        }

    CGulIcon* res = CGulIcon::NewL( icon, mask );
    res->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2 ); // mask, icon
    return res;
    }

// ---------------------------------------------------------------------------
// Load the backgrounds, and resize them to screen size
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::LoadBackgroundsL()
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::LoadBackgroundsL" );

    TParse parse;
    parse.Set( KIconFileName, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile;
    iconFile.Append( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    iBackground.ResetAndDestroy();

    TInt width = iScreenSize.iWidth;
    TInt height = BackgroundHeightL();

    CGulIcon* icon = CreateIconL(
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_01,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_01_mask,
        EFalse );
    AknIconUtils::SetSize( icon->Bitmap(), TSize( width, height ),
        EAspectRatioNotPreserved );
    iBackground.Append( icon );
    icon = NULL;

    icon = CreateIconL(
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_02,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_02_mask,
        EFalse );
    AknIconUtils::SetSize( icon->Bitmap(), TSize( width, height ),
        EAspectRatioNotPreserved );
    iBackground.Append( icon );
    icon = NULL;

    icon = CreateIconL(
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_03,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_03_mask,
        EFalse );
    AknIconUtils::SetSize( icon->Bitmap(), TSize( width, height ),
        EAspectRatioNotPreserved );
    iBackground.Append( icon );
    icon = NULL;

    icon = CreateIconL(
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_04,
        EMbmMpxscreensaverpluginQgn_graf_mup_ss_bgnd_04_mask,
        EFalse );
    AknIconUtils::SetSize( icon->Bitmap(), TSize( width, height ),
        EAspectRatioNotPreserved );
    iBackground.Append( icon );
    icon = NULL;
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::DrawClock
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::DrawClockL( CBitmapContext& aGc )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::DrawClockL" );

    // Draw clock display
    TTime time;
    time.HomeTime();
    // R_QTN_TIME_USUAL_WITH_ZERO cannot be used because ZDigi does not have
    // alphabets
    HBufC* format = StringLoader::LoadLC(
        R_MPX_SCREENSAVER_TIME_USUAL_WITHOUT_AMPM );
    HBufC* clockTime = HBufC::NewLC(
        format->Length() + KMPXDurationDisplayResvLen );
    TPtr clockTimePtr = clockTime->Des();
    MPX_TRAPD( err, time.FormatL( clockTimePtr, *format ) );
    if (err != KErrNone)
        {
        clockTimePtr = KNullDesC;
        }
    // time digifont does not contain arabic / indic etc numbers.
    // If clock font is changed, or digifont starts supporting language-
    // specific numbers, the commented-out call below should be restored
    /*
    else
        {
        AknTextUtils::LanguageSpecificNumberConversion( clockTimePtr );
        }
    */

    // AM/PM text
    HBufC* ampmFormat = StringLoader::LoadLC( R_MPX_SCREENSAVER_AMPM );
    HBufC* ampm = HBufC::NewLC( ampmFormat->Length() + KMPXDurationDisplayResvLen );
    TPtr ampmPtr = ampm->Des();
    MPX_TRAP( err, time.FormatL( ampmPtr, *ampmFormat ) );
    if ( err != KErrNone )
        {
        ampmPtr = KNullDesC;
        }

    TRgb color( KMPXIndicatorColor );
    aGc.UseFont( iClockLabel->Font() );
    aGc.SetPenColor( color );
    TInt width = iClockLabel->Font()->TextWidthInPixels( clockTimePtr );
    // align right
    aGc.DrawText( clockTimePtr,
            TPoint( iClockLabel->Rect().iBr.iX - width, iClockLabel->Rect().iBr.iY ) );

    // draw AM/PM if time format is 12-hour
    TLocale locale;
    if ( locale.TimeFormat() == ETime12 )
        {
        aGc.DiscardFont();
        aGc.UseFont( iAmPmLabel->Font() );
        width = iAmPmLabel->Font()->TextWidthInPixels( ampmPtr );
        // align right
        aGc.DrawText( ampmPtr,
            TPoint( iAmPmLabel->Rect().iBr.iX - width, iAmPmLabel->Rect().iBr.iY ) );
        }

    aGc.DiscardFont();
    CleanupStack::PopAndDestroy( ampm );
    CleanupStack::PopAndDestroy( ampmFormat );
    CleanupStack::PopAndDestroy( clockTime );
    CleanupStack::PopAndDestroy( format );
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::DrawBackground
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::DrawBackground(
    CBitmapContext& aGc, TInt aColorIndex )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::DrawBackground" );
    aGc.BitBltMasked(
        TPoint(),
        iBackground[ aColorIndex ]->Bitmap(),
        TRect( aGc.Device()->SizeInPixels() ),
        iBackground[ aColorIndex ]->Mask(),
        EFalse );
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::DrawPlaybackStatus
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::DrawPlaybackStatusL(
    CBitmapContext& aGc, TMPXPlaybackState aPlaybackState )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::DrawPlaybackStatus" );

    TBool showTitle = EFalse;

    // Draw play status icon (if any)
    CGulIcon* icon = NULL;
    switch ( aPlaybackState )
        {
        case EPbStatePlaying:
            {
            icon = iPlayIcon;
            showTitle = ETrue;
            break;
            }
        case EPbStatePaused:
            {
            icon = iPauseIcon;
            showTitle = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    if ( icon )
        {
        aGc.BitBltMasked(
            iRectPlaybackStatus.iTl,
            icon->Bitmap(),
            TRect( iRectPlaybackStatus.Size() ),
            icon->Mask(),
            EFalse );
        }

    TRgb color( KMPXIndicatorColor );
    aGc.UseFont( iTitleLabel->Font() );
    aGc.SetPenColor( color );

    if ( iTitleLabel->Text() && showTitle )
        {
        HBufC* buf = iTitleLabel->Text()->AllocL();
        CleanupStack::PushL( buf );
        TPtr ptr( buf->Des() );
        AknBidiTextUtils::ConvertToVisualAndClipL(
            ptr,
            *iTitleLabel->Font(),
            iTitleLabel->Rect().Width(),
            iTitleLabel->Rect().Width() );
        aGc.DrawText( ptr,
            TPoint( iTitleLabel->Rect().iTl.iX, iTitleLabel->Rect().iBr.iY ) );
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {
        TTime time;
        time.HomeTime();
        HBufC* format = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
        HBufC* clockTime = HBufC::NewLC( format->Length() + KMPXDurationDisplayResvLen );
        TPtr clockTimePtr = clockTime->Des();
        time.FormatL( clockTimePtr, *format );
        AknTextUtils::LanguageSpecificNumberConversion( clockTimePtr );

        aGc.DrawText( clockTimePtr,
            TPoint( iTitleLabel->Rect().iTl.iX, iTitleLabel->Rect().iBr.iY ) );

        CleanupStack::PopAndDestroy( clockTime );
        CleanupStack::PopAndDestroy( format );
        }
    aGc.DiscardFont();
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::DrawIndicatorsL
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::DrawIndicatorsL(
     CBitmapContext& aGc )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::DrawIndicatorsL" );

    TSize indicatorSize = TSize( iRectIndicators.Height(),
        iRectIndicators.Height() );
    MPX_DEBUG2( "CMPXScreenSaverPluginContainer::DrawIndicatorsL indicator height %d", iRectIndicators.Height() );
    iStartX = iRectIndicators.iTl.iX;

    TRgb color( KMPXIndicatorColor );
    aGc.SetPenColor( color );
    aGc.UseFont( iIndicatorFont );

    if ( iKeyguard > 0 )
        {
        MPX_DEBUG3( "CMPXScreenSaverPluginContainer::DrawIndicatorsL drawing keyguard %d %d", iStartX, iRectIndicators.iTl.iY );
        DoDrawIndicatorsL( aGc, -1, iKeyguardIcon, iIndicatorFont,
            iRectIndicators, indicatorSize);
        }

    if ( iMissedCall > 0 )
        {
        MPX_DEBUG3( "CMPXScreenSaverPluginContainer::DrawIndicatorsL drawing new miss call %d %d", iStartX, iRectIndicators.iTl.iY );
        DoDrawIndicatorsL( aGc, iMissedCall, iMissedCallIcon, iIndicatorFont,
            iRectIndicators, indicatorSize);
        }

    if ( iNewMessage > 0 )
        {
        MPX_DEBUG3( "CMPXScreenSaverPluginContainer::DrawIndicatorsL drawing new message %d %d", iStartX, iRectIndicators.iTl.iY );
        DoDrawIndicatorsL( aGc, iNewMessage, iMessageIcon, iIndicatorFont,
            iRectIndicators, indicatorSize);
        }

    if ( iNewInstantMessage > 0 )
        {
        MPX_DEBUG3( "CMPXScreenSaverPluginContainer::DrawIndicatorsL drawing new instant message %d %d", iStartX, iRectIndicators.iTl.iY );
        DoDrawIndicatorsL( aGc, 0, iNewInstantMessageIcon, iIndicatorFont,
            iRectIndicators, indicatorSize);
        }

    if ( iNewEmail > 0 )
        {
        MPX_DEBUG3( "CMPXScreenSaverPluginContainer::DrawIndicatorsL drawing new email %d %d", iStartX, iRectIndicators.iTl.iY );
        DoDrawIndicatorsL( aGc, 0, iNewEmailIcon, iIndicatorFont,
            iRectIndicators, indicatorSize);
        }

    if ( iNewVoiceMessage > 0 )
        {
        MPX_DEBUG3( "CMPXScreenSaverPluginContainer::DrawIndicatorsL drawing new voice message %d %d", iStartX, iRectIndicators.iTl.iY );
        DoDrawIndicatorsL( aGc, 0, iNewVoiceMessageIcon,
            iIndicatorFont, iRectIndicators, indicatorSize);
        }

    if ( ( iProfileName ) || ( iIsSilent ) )
        {
        MPX_DEBUG3( "CMPXScreenSaverPluginContainer::DrawIndicatorsL drawing profile name %d %d", iStartX, iRectIndicators.iTl.iY );
        // clip text to fit screen
        HBufC* buf;
        if ( iProfileName )
            {
            buf = HBufC::NewLC( iProfileName->Length() +
                KMPXDurationDisplayResvLen );
            }
        else
            {
            buf = HBufC::NewLC( KMPXDurationDisplayResvLen );
            }
        TPtr ptr( buf->Des() );
        if ( iIsSilent )
            {
            ptr.Append( KPuaCodeSilentSymbol );
            }
        if ( iProfileName )
            {
            ptr.Append( iProfileName->Des() );
            AknBidiTextUtils::ConvertToVisualAndClipL(
                ptr,
                *iIndicatorFont,
                iRectIndicators.iBr.iX - iStartX,
                iRectIndicators.iBr.iX - iStartX );
            }
        aGc.DrawText(  *buf,
            TPoint( iStartX, iRectIndicators.iTl.iY + indicatorSize.iHeight ) );
        CleanupStack::PopAndDestroy( buf );
        }

    aGc.DiscardFont();
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPluginContainer::DrawIndicatorsL
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::DoDrawIndicatorsL(
     CBitmapContext& aGc, TInt aDisplayNumber, CGulIcon* aDisplayIcon,
     CFont* aFont, TRect aRect, TSize aSize )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::DoDrawIndicatorsL" );

    if ( aDisplayNumber > 0 )
        {
        HBufC* stringBuf = HBufC::NewLC( 5 );
        stringBuf->Des().AppendNum( aDisplayNumber );
        aGc.DrawText( stringBuf->Des(),
            TPoint( iStartX, aRect.iTl.iY + aSize.iHeight ) );
        iStartX = iStartX + aFont->TextWidthInPixels( stringBuf->Des() );
        CleanupStack::PopAndDestroy( stringBuf );
        }
    aGc.BitBltMasked(
        TPoint( iStartX, aRect.iTl.iY ),
        aDisplayIcon->Bitmap(),
        TRect( aSize ),
        aDisplayIcon->Mask(),
        EFalse );
    iStartX = iStartX + aSize.iWidth;
    }

// ---------------------------------------------------------------------------
// Get layout data for an graphical indicator.
// ---------------------------------------------------------------------------
//
TRect CMPXScreenSaverPluginContainer::IndicatorLayout(
    MPXScreenSaverPluginControl aControl )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::IndicatorLayout" );
    TInt resId = 0;
    if ( iScreenSize == TSize( 176, 208 ) )    //lowest portrait
        {
        switch ( aControl )
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_LOWRES_PORTRAIT;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_LOWRES_PORTRAIT;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 352, 416 ) ) //double portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_DOUBLE_PORTRAIT;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_DOUBLE_PORTRAIT;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 240, 320 ) ) //QVGA portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_QVGA_PORTRAIT;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_QVGA_PORTRAIT;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 208, 176 ) )    //lowest landscape
        {
        switch ( aControl )
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_LOWRES_LANDSCAPE;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_LOWRES_LANDSCAPE;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 416, 352 ) ) //double landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_DOUBLE_LANDSCAPE;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_DOUBLE_LANDSCAPE;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 320, 240 ) ) //QVGA landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_QVGA_LANDSCAPE;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_QVGA_LANDSCAPE;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 360, 640 ) ) // QHD portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_QHD_PORTRAIT;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_QHD_PORTRAIT;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 640, 360 ) ) // QHD landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_QHD_LANDSCAPE;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_QHD_LANDSCAPE;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 480, 640 ) ) // VGA portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_VGA_PORTRAIT;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_VGA_PORTRAIT;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 640, 480 ) ) // VGA landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginIndicatorPlaybackStatus:
                {
                resId = R_MPX_SCREENSAVER_PB_INDI_LAYOUT_VGA_LANDSCAPE;
                break;
                }
            case MPXScreenSaverPluginIndicatorOther:
                {
                resId = R_MPX_SCREENSAVER_INDI_LAYOUT_VGA_LANDSCAPE;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
	else
        {
        User::Panic( KMXPanicCategory, KErrNotSupported );
        }
    TAknLayoutRect res;
    res.LayoutRect( TRect( TPoint( 0, 0),
        TPoint( iOffScreenBitmap->SizeInPixels().iWidth,
        iOffScreenBitmap->SizeInPixels().iHeight) ), resId );
    return res.Rect();
    }

// ---------------------------------------------------------------------------
// Get layout data for an graphical indicator.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::LabelLayoutL(
    MPXScreenSaverPluginControl aControl )
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::LabelLayoutL" );
    TInt resId = 0;
    CEikLabel* label = NULL;
    if ( iScreenSize == TSize( 176, 208 ) )    //lowest portrait
        {
        switch ( aControl )
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_LOWRES_PORTRAIT;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_LOWRES_PORTRAIT_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_LOWRES_PORTRAIT;
                    }
                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_LOWRES_PORTRAIT;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 352, 416 ) ) //double portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_DOUBLE_PORTRAIT;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_DOUBLE_PORTRAIT_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_DOUBLE_PORTRAIT;
                    }

                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_DOUBLE_PORTRAIT;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 240, 320 ) ) //QVGA portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_QVGA_PORTRAIT;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QVGA_PORTRAIT_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QVGA_PORTRAIT;
                    }
                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_QVGA_PORTRAIT;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 208, 176 ) )    //lowest landscape
        {
        switch ( aControl )
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_LOWRES_LANDSCAPE;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_LOWRES_LANDSCAPE_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_LOWRES_LANDSCAPE;
                    }

                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_LOWRES_LANDSCAPE;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 416, 352 ) ) //double landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_DOUBLE_LANDSCAPE;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_DOUBLE_LANDSCAPE_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_DOUBLE_LANDSCAPE;
                    }
                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_DOUBLE_LANDSCAPE;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 320, 240 ) ) //QVGA landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_QVGA_LANDSCAPE;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QVGA_LANDSCAPE_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QVGA_LANDSCAPE;
                    }

                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_QVGA_LANDSCAPE;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 360, 640 ) ) // QHD portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_QHD_PORTRAIT;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QHD_PORTRAIT_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QHD_PORTRAIT;
                    }

                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_QHD_PORTRAIT;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 640, 360 ) ) // QHD landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_QHD_LANDSCAPE;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QHD_LANDSCAPE_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_QHD_LANDSCAPE;
                    }

                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_QHD_LANDSCAPE;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 480, 640 ) ) // VGA portrait
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_VGA_PORTRAIT;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_VGA_PORTRAIT_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_VGA_PORTRAIT;
                    }

                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_VGA_PORTRAIT;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
    else if ( iScreenSize == TSize( 640, 480 ) ) // VGA landscape
        {
        switch (aControl)
            {
            case MPXScreenSaverPluginLabelTitle:
                {
                resId = R_MPX_SCREENSAVER_TITLE_LAYOUT_VGA_LANDSCAPE;
                label = iTitleLabel;
                break;
                }
            case MPXScreenSaverPluginLabelClock:
                {
                TLanguage uiLanguage = User::Language();
                if( uiLanguage == ELangArabic ||
                    uiLanguage == ELangUrdu ||
                    uiLanguage == ELangFarsi )
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_VGA_LANDSCAPE_ARABIC;
                    }
                else
                    {
                    resId = R_MPX_SCREENSAVER_CLOCK_LAYOUT_VGA_LANDSCAPE;
                    }

                label = iClockLabel;
                break;
                }
            case MPXScreenSaverPluginLabelAmPm:
                {
                resId = R_MPX_SCREENSAVER_AMPM_LAYOUT_VGA_LANDSCAPE;
                label = iAmPmLabel;
                break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                    User::Panic( KMXPanicCategory, KErrArgument ) );
                break;
                }
            }
        }
	else
        {
        User::Panic( KMXPanicCategory, KErrNotSupported );
        }
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, resId );

    // Read the layout from resource file
    TAknTextComponentLayout textLayout;
    textLayout.SetFont( reader.ReadInt16() );
    textLayout.SetC( reader.ReadInt16() );
    textLayout.Setl( reader.ReadInt16() );
    textLayout.Setr( reader.ReadInt16() );
    textLayout.Setb( reader.ReadInt16() );
    textLayout.SetW( reader.ReadInt16() );
    textLayout.SetJ( reader.ReadInt16() );
    CleanupStack::PopAndDestroy();  // reader

    AknLayoutUtils::LayoutLabel(
        label, TRect( TPoint( 0, 0),
        TPoint( iOffScreenBitmap->SizeInPixels().iWidth,
        iOffScreenBitmap->SizeInPixels().iHeight) ),
        textLayout.LayoutLine() );
    }

// ---------------------------------------------------------------------------
// Get drawable background height.
// ---------------------------------------------------------------------------
//
TInt CMPXScreenSaverPluginContainer::BackgroundHeightL()
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::BackgroundHeightL" );
    TInt resId = 0;
    if ( iScreenSize == TSize( 176, 208 ) )    //lowest portrait
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_LOWRES_PORTRAIT;
        }
    else if ( iScreenSize == TSize( 352, 416 ) ) //double portrait
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_DOUBLE_PORTRAIT;
        }
    else if ( iScreenSize == TSize( 240, 320 ) ) //QVGA portrait
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_QVGA_PORTRAIT;
        }
    else if ( iScreenSize == TSize( 208, 176 ) )    //lowest landscape
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_LOWRES_LANDSCAPE;
        }
    else if ( iScreenSize == TSize( 416, 352 ) ) //double landscape
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_DOUBLE_LANDSCAPE;
        }
    else if ( iScreenSize == TSize( 320,240 ) ) // QVGA landscape
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_QVGA_LANDSCAPE;
        }
    else if ( iScreenSize == TSize( 360, 640 ) ) // QHD
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_QHD_PORTRAIT;
        }
    else if ( iScreenSize == TSize( 640, 360 ) ) // QHD
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_QHD_LANDSCAPE;
        }
    else if ( iScreenSize == TSize( 480, 640 ) ) // VGA portrait
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_QHD_PORTRAIT;
        }
    else if ( iScreenSize == TSize( 640, 480 ) ) // VGA landscape
        {
        resId = R_MPX_SCREENSAVER_BACKGROUND_HEIGHT_QHD_LANDSCAPE;
        }
	else
        {
        User::Panic( KMXPanicCategory, KErrNotSupported );
        }
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, resId );
    TInt res = reader.ReadInt16();
    MPX_DEBUG2( "CMPXScreenSaverPluginContainer::BackgroundHeightL %d", res );
    CleanupStack::PopAndDestroy();  // reader
    return res;
    }

// ---------------------------------------------------------------------------
// Get draw position and background color index, draw position is stored
// in member variable iStartY
// ---------------------------------------------------------------------------
//
TInt CMPXScreenSaverPluginContainer::GetDrawPositionAndColorIndexL()
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::GetDrawPositionAndColorIndexL" );
    TTime time;
    time.HomeTime();

    TInt colorIndex = 0;
    TInt64 currentTime = time.Int64();
    colorIndex = currentTime / 10000000; // not a typo, to get it to the closest 10 seconds
    colorIndex = colorIndex % KMPXNumBackgrounds;

    TReal yReal = Math::FRand( currentTime );
    TInt randomRangeY = iScreenSize.iHeight - BackgroundHeightL();
    iStartY = ( TInt( yReal * randomRangeY ) );

    MPX_DEBUG3( "CMPXScreenSaverPluginContainer::GetDrawPositionAndColorIndexL index = %d, height = %d", colorIndex, iStartY );

    return colorIndex;
    }

// ---------------------------------------------------------------------------
// Resizes all the icons and rects to match current screen size
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPluginContainer::UpdateLayoutL()
    {
    MPX_FUNC( "CMPXScreenSaverPluginContainer::UpdateLayoutL" );

    iRectIndicators = IndicatorLayout( MPXScreenSaverPluginIndicatorOther );
    AknIconUtils::SetSize( iKeyguardIcon->Bitmap(),
        TSize( iRectIndicators.Height(), iRectIndicators.Height() ),
        EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iMissedCallIcon->Bitmap(),
        TSize( iRectIndicators.Height(), iRectIndicators.Height() ),
        EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iMessageIcon->Bitmap(),
        TSize( iRectIndicators.Height(), iRectIndicators.Height() ),
        EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iNewInstantMessageIcon->Bitmap(),
        TSize( iRectIndicators.Height(), iRectIndicators.Height() ),
        EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iNewVoiceMessageIcon->Bitmap(),
        TSize( iRectIndicators.Height(), iRectIndicators.Height() ),
        EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iNewEmailIcon->Bitmap(),
        TSize( iRectIndicators.Height(), iRectIndicators.Height() ),
        EAspectRatioNotPreserved );

    iRectPlaybackStatus =
        IndicatorLayout( MPXScreenSaverPluginIndicatorPlaybackStatus );
    AknIconUtils::SetSize( iPlayIcon->Bitmap(), iRectPlaybackStatus.Size(),
        EAspectRatioNotPreserved );
    AknIconUtils::SetSize( iPauseIcon->Bitmap(), iRectPlaybackStatus.Size(),
        EAspectRatioNotPreserved );

    LabelLayoutL( MPXScreenSaverPluginLabelTitle );
    LabelLayoutL( MPXScreenSaverPluginLabelClock );
    LabelLayoutL( MPXScreenSaverPluginLabelAmPm );
    }


// End Of file.

