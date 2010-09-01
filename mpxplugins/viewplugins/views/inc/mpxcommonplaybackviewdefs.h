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
* Description:  Common defines and data types for common playback view
*
*/



#ifndef MPXCOMMONPLAYBACKVIEWDEFS_H
#define MPXCOMMONPLAYBACKVIEWDEFS_H

_LIT(KMPXCommonPlaybackViewIconFile, "mpxcommonplaybackview.mbm");

/**
 * States for soft buttons in Playback View.
 * @since 3.0
 */
/**
 * Identifiers for soft buttons in Playback View.
 * @since 3.0
 */
enum TMPXPbvButton
    {
    EButtonRocker,
    EButtonLast         // Should always be the last value
    };

enum TMPXPbvAvkonButton
    {
    EButtonPlay,
    EButtonPause,
    EButtonFastForward,
    EButtonRewind,
    EButtonVolumeDown,
    EButtonVolumeUp,
    EButtonStop,
    };

/**
 * Identifiers for different graphical indicators visible in Playback View.
 * These constants are used by MPXPlaybackViewLayout when getting layout data
 * (MPXPlaybackViewLayout::IndicatorLayout()) and bitmaps
 * (MPXPlaybackViewLayout::GetIndicatorIconL()) for the indicators.
 * @since 3.0
 */
enum TMPXPbvIndicator
    {
    // Playback status indicators
    EIconPlay,
    EIconStop,
    EIconPause,
    EIconFastForward,
    EIconRewind,

    // Mode indicators
    EIconRepeatOne,
    EIconRepeatAll,
    EIconRandom,

    // Background
    EBackgroundGraphic, // Layout and graphic for skinned background

    // Progressive download
    ESliderPane,        // Parent pane for download/playback sliders
    ESliderBackground,  // Background graphic for slider
    EPlaybackSlider,    // Slider indicating current playback position
    EDownloadSlider,    // Slider indicating current download position

    EAlbumArtArea,       // Layout for album art, default graphic
    EIconRemotePlayer    // layout for upnp graphic

    // Playback type indicators
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    , EIconReal
#endif
    };

/**
 * Identifiers for different text fields visible in Playback View.
 * @since 3.0
 */
enum TMPXPbvTextIndicator
    {
    ETextArtist,
    ETextTrack,
    ETextPlaylistCount,

    ETextEmpty,
    ETextElapsedTime,
    ETextRemainingTime,
    ETextDownloadState,
    ETextFMFrequency,
    ETextCount
    };

/**
 * Playback states for Playback View.
 * @since 3.0
 */
enum TMPXPbvPlaybackMode
    {
    EInvalidMode = 0,
    EPlayMode,
    EStopMode,
    EPauseMode,
    ENoTracksMode,
    EUninitialized,
    EInitialising,
    EMediaKeySkipping,
    EBufferingMode
    };

enum TMPXPbvLayoutVariants
    {
    // Playback status indicators
    EPbvPortraitWithoutRocker,        // 0
    EPbvLandscapeWithoutRocker,       // 1
    EPbvPortraitTouchExpandedCntl,    // 2
    EPbvLandscapeTouchExpandedCntl,   // 3
    EPbvPortraitTouchCollapsedCntl,   // 4
    EPbvLandscapeTouchCollapsedCntl,  // 5
    EPbvPortraitWithRocker,           // 6
    EPbvLandscapeWithRocker,          // 7
    EPbvPortraitTouchFixedCntl,       // 8
    EPbvLandscapeTouchFixedCntl,      // 9
    EPbvPortraitNhdTouch,             // 10
    EPbvLandscapeNhdTouch,            // 11
    EPbvPortraitNhdTouchWithFM,       // 12
    EPbvLandscapeNhdTouchWithFM,	  // 13
	EPbvUndefinedVariant,			  // 14
    EPbvPortraitWithRockerFM,         // 15
    EPbvLandscapeWithRockerFM         // 16
    };
#endif  // MPXCOMMONPLAYBACKVIEWDEFS_H

// End of file
