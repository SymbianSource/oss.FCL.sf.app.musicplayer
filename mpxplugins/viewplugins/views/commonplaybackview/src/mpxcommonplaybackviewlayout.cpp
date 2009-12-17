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
* Description:  Layout and graphic factory for Common Playback View
*
*/



// INCLUDE FILES
#include <AknsUtils.h>
#include <AknUtils.h>
#include <barsread.h>
#include <gulicon.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <data_caging_path_literals.hrh>
#include <mpxcommonplaybackview.rsg>
#include <mpxcommonplaybackview.mbg>
#include <mpxfmtx.rsg> // For FF_FMTX
#include <featmgr.h>
#include <centralrepository.h>
#include <mpxinternalcrkeys.h>
#include <mpxlbxextendedfeatures.h>
#include <mpxlog.h>
#include <mpxuser.h>

#include "mpxcommonplaybackviewlayout.h"

using namespace AknLayoutScalable_Apps;


// CONSTANTS
_LIT(KMXPanicCategory, "CMPXCommonPlaybackViewLayout");
//_LIT(KMpxcommonplaybackviewMbmFile, "mpxcommonplaybackview.mbm");


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Get layout data for a button.
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CMPXCommonPlaybackViewLayout::ButtonLayout(
    const TRect& aParentRect )
    {
    TAknLayoutRect res;
    TAknWindowComponentLayout layoutItem;
    TBool isLandscape = Layout_Meta_Data::IsLandscapeOrientation();
    if(!isLandscape)
        {
        layoutItem = mup3_rocker_pane(0);
       }
    else
        {
        layoutItem = mup3_rocker_pane(1);
        }

    res.LayoutRect( aParentRect, layoutItem.LayoutLine() );
    return res.Rect();
    }

// ---------------------------------------------------------------------------
// Get layout data for an graphical indicator.
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CMPXCommonPlaybackViewLayout::IndicatorLayout(
    const TRect& aParentRect,
    TMPXPbvIndicator aIndicator )
    {
    TAknLayoutRect res;

    TMPXPbvLayoutVariants lafVariant(EPbvUndefinedVariant);
    lafVariant = GetIndicatorVariant(aIndicator);
    if (lafVariant == EPbvUndefinedVariant)
       {
       // unsupported
       ASSERT(0);
       }
    else
       {
       TAknWindowComponentLayout layoutItem;
       switch ( aIndicator )
          {
            case EIconPlay:
            case EIconPause:
            case EIconStop:
                {
                layoutItem = main_mup3_pane_g5(lafVariant);
                break;
                }
            case EIconRepeatAll:
            case EIconRepeatOne:
                {
                layoutItem = main_mup3_pane_g4(lafVariant);
                break;
                }
            case EIconRandom:
                {
                layoutItem = main_mup3_pane_g3(lafVariant);
                break;
                }
            case EAlbumArtArea:
                {
                layoutItem = main_mup3_pane_g2(lafVariant);
                break;
                }
            case ESliderPane:
                {
                layoutItem = mup3_progress_pane(lafVariant);
                break;
                }
            case ESliderBackground:
                {
                layoutItem = mup_progress_pane_cp03();
                break;
                }
            case EPlaybackSlider:
                {
                layoutItem = mup_progress_pane_cp03();
                break;
                }
            case EDownloadSlider:
                {
                layoutItem = mup_progress_pane_cp03();
                break;
                }
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
            case EIconReal:
                {
			    if( lafVariant == EPbvPortraitWithRocker )
					{
					layoutItem = main_mup3_pane_g8( 0 );
					}
				else if( lafVariant == EPbvLandscapeWithRocker )
					{	
					layoutItem = main_mup3_pane_g8( 15 );
				    }
				else
                    {
                    layoutItem = main_mup3_pane_g8( lafVariant );
                    }

                break;
                }
#endif
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                                    User::Panic(KMXPanicCategory, KErrArgument));
                }
          }

       res.LayoutRect(aParentRect, layoutItem.LayoutLine());
       }
    return res.Rect();
    }

// ---------------------------------------------------------------------------
// Set layout data for a text label.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewLayout::LayoutLabel(
    CEikLabel* aLabel,
    const TRect& aParentRect,
    TMPXPbvTextIndicator aText,
    TInt /*aOffsetx*/, /* =0 */
    TInt /*aOffsety*/ /* =0 */ )
    {
    TMPXPbvLayoutVariants lafVariant(EPbvUndefinedVariant);
    lafVariant = GetTextVariant(aText);
    if (lafVariant == EPbvUndefinedVariant)
        {
        ASSERT(0);
        }
    else
        {
        TAknTextComponentLayout layoutItem;
        switch ( aText )
            {
            case ETextTrack:
                {
                layoutItem = main_mup3_pane_t1(lafVariant);
                break;
                }
            case ETextArtist:
                {
                layoutItem = main_mup3_pane_t2(lafVariant);
                break;
                }
            case ETextPlaylistCount:
                {
				if( lafVariant == EPbvLandscapeWithRocker )
					{
					lafVariant = (TMPXPbvLayoutVariants)15;
					}
                layoutItem = main_mup3_pane_t5(lafVariant);
                break;
                }
            case ETextEmpty:
                {
                layoutItem = main_mup3_pane_t6(lafVariant);
                break;
                }
            case ETextElapsedTime:
                {
                layoutItem = mup3_progress_pane_t1();
                break;
                }
            case ETextRemainingTime:
                {
                layoutItem = mup3_progress_pane_t2();
                break;
                }
            case ETextDownloadState:
                {
                layoutItem = mup3_progress_pane_t3();
                break;
                }
            case ETextFMFrequency:
                {
                if( lafVariant == EPbvPortraitWithRocker )
                    {
                    layoutItem = main_mup3_pane_t7( 15 );
                    }
                else if( lafVariant == EPbvLandscapeWithRocker )
                    {	
                    layoutItem = main_mup3_pane_t7( 16 );
                    }
				else
					{
					layoutItem = main_mup3_pane_t7(lafVariant);
					}					
				break;
                }
            default:
                {
                __ASSERT_ALWAYS(EFalse,
                                    User::Panic(KMXPanicCategory, KErrArgument));
                }
            }

        AknLayoutUtils::LayoutLabel(aLabel, aParentRect, layoutItem.LayoutLine());

        }
    }

// ---------------------------------------------------------------------------
// Get the bitmap and mask for a soft button in a given state.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewLayout::GetButtonBitmapsL(
    TMPXPbvButton /*aButton*/,
    TInt /*aButtonState*/,
    CFbsBitmap*& /*aBitmap*/,
    CFbsBitmap*& /*aMask*/ )
    {
// do nothing
    }

// ---------------------------------------------------------------------------
// Get the bitmap and mask for a given indicator.
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CMPXCommonPlaybackViewLayout::GetIndicatorIconMaskL(
    TMPXPbvIndicator aIndicator )
    {
    TParse parse;
    parse.Set( KMPXCommonPlaybackViewIconFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    switch ( aIndicator )
        {
        case EIconPlay:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnIndiMupPlay,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_indi_mup_play,
                EMbmMpxcommonplaybackviewQgn_indi_mup_play_mask);
            }
        case EIconPause:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnIndiMupPause,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_indi_mup_pause,
                EMbmMpxcommonplaybackviewQgn_indi_mup_pause_mask);
            }
        case EIconStop:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnIndiMupStop,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_indi_mup_stop,
                EMbmMpxcommonplaybackviewQgn_indi_mup_stop_mask);
            }
        case EIconRepeatAll:
            {
            return CreateColorIconL(
                skin,
                KAknsIIDQgnIndiMupRepeat,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG26,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_icon_repeat,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_icon_repeat_mask );
            }
        case EIconRepeatOne:
            {
            return CreateColorIconL(
                skin,
                KAknsIIDQgnIndiMupRepeatOne,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG26,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_icon_repeatone,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_icon_repeatone_mask );
            }
        case EIconRandom:
            {
            return CreateColorIconL(
                skin,
                KAknsIIDQgnIndiMupRandom,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG26,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_icon_shuffle,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_icon_shuffle_mask );
            }
        case ESliderBackground:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnGrafMup2BarFrame,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_prbar_bgd,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_prbar_bgd_mask );
            }
        case EPlaybackSlider:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnGrafMup2BarProgress2,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_prbar_top_play,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_prbar_top_play_mask );
            }
        case EDownloadSlider:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnGrafMup2BarProgress,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_prbar_mid_download,
                EMbmMpxcommonplaybackviewQgn_graf_mup_npv_prbar_mid_download_mask );
            }
        case EAlbumArtArea:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnIndiMupDefaultAlbum,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_indi_mup_default_album,
                EMbmMpxcommonplaybackviewQgn_indi_mup_default_album_mask );
            }
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
        case EIconReal:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnIndiMupRp,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_indi_mup_rp,
                EMbmMpxcommonplaybackviewQgn_indi_mup_rp_mask );
            }
#endif
        default:
            {
            __ASSERT_ALWAYS(
                EFalse,
                User::Panic( KMXPanicCategory, KErrArgument ) );
            break;
            }
        }
    return NULL; // Not reached
    }

// ---------------------------------------------------------------------------
// Create a color-skinned icon.
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CMPXCommonPlaybackViewLayout::CreateColorIconL(
    MAknsSkinInstance* aSkin,
    const TAknsItemID& aIconSkinId,
    const TAknsItemID& aColorId,
    TInt aColorIndex,
    const TDesC& aBitmapFilename,
    TInt aFileBitmapId,
    TInt aFileMaskId )
    {
    CFbsBitmap* icon = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateColorIconLC(
        aSkin,           // aInstance
        aIconSkinId,     // aID
        aColorId,        // aColorID
        aColorIndex,     // aColorIndex
        icon,            // aBitmap
        mask,            // aMask
        aBitmapFilename, // aFilename
        aFileBitmapId,   // aFileBitmapId
        aFileMaskId,     // aFileBitmapId
        KRgbBlack );     // aDefaultColor

    CGulIcon* res = CGulIcon::NewL( icon, mask );
    res->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop(2); // mask, icon

    return res;
    }

// ---------------------------------------------------------------------------
// Layouts the label by reading in the resource values
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonPlaybackViewLayout::DoLayoutLabelL(
    CEikLabel* aLabel,
    const TRect& aParentRect,
    TInt aResId,
    TInt aOffsetx,
    TInt aOffsety )
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResId );

    // Read the layout from resource file
    TAknTextComponentLayout textLayout;
    textLayout.SetFont( reader.ReadInt16() );
    textLayout.SetC( reader.ReadInt16() );
    textLayout.Setl( reader.ReadInt16() + aOffsetx );
    textLayout.Setr( reader.ReadInt16() - aOffsetx );
    textLayout.Setb( reader.ReadInt16() + aOffsety );
    textLayout.SetW( reader.ReadInt16() );
    textLayout.SetJ( reader.ReadInt16() );
    CleanupStack::PopAndDestroy();  // reader

    AknLayoutUtils::LayoutLabel(
        aLabel, aParentRect, textLayout.LayoutLine() );
    }

// ---------------------------------------------------------------------------
// Get layout data for a button.
// ---------------------------------------------------------------------------
//
EXPORT_C TRect CMPXCommonPlaybackViewLayout::ButtonLayout(const TRect& aParentRect,
                                      TInt aButton)
    {


    TAknLayoutRect res;
    TAknWindowComponentLayout layoutItem;
    TInt buttonVariant = 0;

    switch (aButton)
        {
        case EButtonPlay:
        case EButtonPause:
            {
            layoutItem = mup3_control_keys_pane_g3(buttonVariant);
            break;
            }
        case EButtonFastForward:
            {
            layoutItem = mup3_control_keys_pane_g2(buttonVariant);
            break;
            }
        case EButtonRewind:
            {
            layoutItem = mup3_control_keys_pane_g1(buttonVariant);
            break;
            }
        case EButtonStop:
        case EButtonVolumeDown:
            {
            layoutItem = mup3_control_keys_pane_g4(buttonVariant);
            break;
            }
        case EButtonVolumeUp:
            {
            layoutItem = mup3_control_keys_pane_g4_copy1(buttonVariant);
            break;
            }
        default:
            {
            ASSERT(0);
            break;
            }
        }

    res.LayoutRect( aParentRect, layoutItem.LayoutLine() );

    return res.Rect();
    }


// ---------------------------------------------------------------------------
//  Determine the layout (touch, non-touch, landscape, portrait, rocker, etc...)
// ---------------------------------------------------------------------------
//
EXPORT_C TMPXPbvLayoutVariants CMPXCommonPlaybackViewLayout::DetermineLayoutVariant()
    {
    TMPXPbvLayoutVariants variant(EPbvUndefinedVariant);

    TInt flags( 0 );
    CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
    User::LeaveIfError( repository->Get( KMPXMPLocalVariation, flags ));
    delete repository;
    repository = NULL;
    TBool showRockerGraphics = static_cast<TBool>( flags & KMPXRockerMappingSupport );

    TBool isLandscape = Layout_Meta_Data::IsLandscapeOrientation();

    TBool isTouch = AknLayoutUtils::PenEnabled();

    if (isTouch && !isLandscape )
        {
        variant = EPbvPortraitNhdTouchWithFM;
        }
    else if (isTouch && isLandscape)
        {
        variant = EPbvLandscapeNhdTouchWithFM;
        }
    else if (!showRockerGraphics && !isLandscape )
        {
        variant = EPbvPortraitWithoutRocker;
        }
    else if (!showRockerGraphics && isLandscape)
        {
        variant = EPbvLandscapeWithoutRocker;
        }
    else if (showRockerGraphics && !isLandscape)
        {
        variant = EPbvPortraitWithRocker;
        }
    else if (showRockerGraphics && isLandscape)
        {
        variant = EPbvLandscapeWithRocker;
        }
    

    return(variant);
    }

// ---------------------------------------------------------------------------
//  Determine the layout variant for a text item
// ---------------------------------------------------------------------------
//
EXPORT_C TMPXPbvLayoutVariants CMPXCommonPlaybackViewLayout::GetTextVariant(TMPXPbvTextIndicator aText)
    {
    TMPXPbvLayoutVariants layoutVariant = DetermineLayoutVariant();
    TMPXPbvLayoutVariants textVariant;
    switch ( aText )
        {
        case ETextTrack:
        case ETextArtist:
        case ETextPlaylistCount:
        case ETextFMFrequency:
            {
            textVariant = layoutVariant;
            break;
            }
        case ETextEmpty:
        case ETextElapsedTime:
        case ETextRemainingTime:
        case ETextDownloadState:
            {
            textVariant = static_cast<TMPXPbvLayoutVariants> (0);    // always 0 regardless of layout variant
            break;
            }
        default:
            {
            textVariant = EPbvUndefinedVariant;
            }
        }

    return(textVariant);
    }

// ---------------------------------------------------------------------------
// Determine the layout variant type for an indicator item
// ---------------------------------------------------------------------------
//
EXPORT_C TMPXPbvLayoutVariants CMPXCommonPlaybackViewLayout::GetIndicatorVariant(TMPXPbvIndicator aIndicator)
    {
    TMPXPbvLayoutVariants layoutVariant = DetermineLayoutVariant();
    TMPXPbvLayoutVariants indicatorVariant;
    switch ( aIndicator )
        {
        case EAlbumArtArea:
        case EIconRepeatOne:
        case EIconRepeatAll:
        case EIconRandom:
        case EIconPlay:
        case EIconStop:
        case EIconPause:
        case ESliderPane:
        case ESliderBackground:
        case EPlaybackSlider:
        case EDownloadSlider:
        case EIconReal:
            {
            indicatorVariant = layoutVariant;
            break;
            }
        case EIconFastForward:
        case EIconRewind:
        case EIconRemotePlayer:
        case EBackgroundGraphic:
            {
            indicatorVariant = EPbvUndefinedVariant;
            break;
            }

        default:
            {
            indicatorVariant = EPbvUndefinedVariant;
            }
        }

    return(indicatorVariant);
    }

//  End of File
