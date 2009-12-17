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
* Description:  MPX Screen Saver Plugin container
*
*/


#ifndef C_CMPXSCREENSAVERPLUGINCONTAINER_H
#define C_CMPXSCREENSAVERPLUGINCONTAINER_H

// INCLUDES
#include "mpxscreensaverplugin.hrh"

// CLASS DECLARATION
class CEikonEnv;
class MAknsSkinInstance;
class TAknsItemID;
class MScreensaverPluginHost;
class CEikLabel;

/**
*  Screen saver plugin container for music player now playing view
*  @lib mpxscreensaverplugin.lib
*  @since 3.0
*/

class CMPXScreenSaverPluginContainer: public CBase
{
public:
    /**
    * Default Constructor
    * @since 3.0
    */
    CMPXScreenSaverPluginContainer( MScreensaverPluginHost *aHost );

    /**
    * 2nd phase constructor
    * @since 3.0
    */
    void ConstructL();

    /**
    * Destructor.
    */
    virtual ~CMPXScreenSaverPluginContainer();

public:
    /**
    * @param aGc window GC for drawing
    * @param aPlaybackState current play state
    * @since 3.0
    */
    virtual TInt DrawL( CWindowGc& aGc, TMPXPlaybackState aPlaybackState );

    /**
    * Sets song properties
    * @since 3.0
    */
    void SetSongPropertiesL( const CMPXMedia& aMedia );

    /**
    * Set parameter
    * @param aType parameter type
    * @param aValue parameter value
    */
    void SetParam( MPXScreenSaverPluginParam aType, TInt aValue );

    /**
     * Set profile name
     *
     * @param aName profile name
     * @param aDisplay ETrue to display the profile name, otherwise EFalse
     */
    void SetProfileNameL( const TDesC& aName, TBool aDisplay );

    /**
    * Called to get display info
    * @since 3.0
    */
    void RefreshDisplayInfo();

private:

    /**
     * Creates the icons.
     *
     * @since 3.0
     * @return CGulIcon     Indicator graphic and mask. Ownership
     *                      transferred to caller.
     */
    void CreateIconsL();

    /**
     * Create a color icon.
     *
     * @param aID       skin id of the icon
     * @param aColorId  Item ID of the color table. if KAknsIIDNone is used, the icon
     *                  created will use the original color specified in the file
     * @param aColorIndex Index in the color table.
     * @param aBitmapFilename Filename to be used to construct the item.
     * @param aFileBitmapId ID of the bitmap in the file.
     * @param aFileMaskId ID of the mask in the file.
     * @param aIsIndicator ETrue if it's an indicator, it'll change the icon color
     *                     to match other indicator's color. EFalse will not change
     *                     icon color
     */
    CGulIcon* CreateIconL(
        const TAknsItemID& aID,
        const TAknsItemID& aColorId,
        TInt aColorIndex,
        const TDesC& aBitmapFilename,
        TInt aFileBitmapId,
        TInt aFileMaskId,
        TBool aIsIndicator );

    /**
     * Load the backgrounds, and resize them to screen size
     *
     */
    void LoadBackgroundsL();

    /**
     * Draws the clock
     * @since 3.0
     */
    void DrawClockL( CBitmapContext& aGc );

    /**
     * Draws background
     * @since 3.0
     */
    void DrawBackground( CBitmapContext& aGc, TInt aColorIndex );

    /**
     * Draws playback information (playback status, song title)
     * @since 3.0
     */
    void DrawPlaybackStatusL( CBitmapContext& aGc, TMPXPlaybackState aPlaybackState );

    /**
     * Draws indicators
     * @since 3.0
     */
    void DrawIndicatorsL( CBitmapContext& aGc );

    /**
     * Draws indicators on offscreenbitmap
     * @since 3.0
     */
    void DoDrawIndicatorsL( CBitmapContext& aGc,
        TInt aDisplayNumber, CGulIcon* aDisplayIcon,
        CFont* aFont, TRect aRect, TSize aSize );

    /**
     * Get layout data for an graphical indicator.
     *
     * @since 3.0
     * @param  aIndicator Enumeration value identifying the indicator
     * @return Layout data
     */
    TRect IndicatorLayout(
        MPXScreenSaverPluginControl aControl );

    /*
     * Get layout data for an label indicator.
     *
     * @since 3.0
     * @param  aIndicator Enumeration value identifying the indicator
     */
    void LabelLayoutL(
        MPXScreenSaverPluginControl aControl );

    /*
     * Get background height.
     *
     * @since 3.0
     * @return background height for current resolution
     */
    TInt BackgroundHeightL();

    /**
     * Get draw position and background color index, draw position is stored
     * in member variable iStartY
     *
     * @return background color index
     */
    TInt GetDrawPositionAndColorIndexL();

    /**
     * Resizes all the icons and rects to match current screen size
     */
    void UpdateLayoutL();

private:
    // Pointer to the screen saver host
    MScreensaverPluginHost*     iScreenSaverHost;

    CCoeControl*                iParentControl;
    CEikonEnv*                  iEikEnv;
    HBufC*                      iProfileName;
    TInt                        iStartY;
    TInt                        iStartX;
    TInt                        iNewMessage;
    TInt                        iMissedCall;
    TInt                        iNewInstantMessage;
    TInt                        iNewVoiceMessage;
    TInt                        iNewEmail;
    TInt                        iKeyguard;
    TBool                       iIsSilent;

    CGulIcon*                   iPlayIcon;  // owned
    CGulIcon*                   iPauseIcon; // owned
    CGulIcon*                   iMessageIcon;
    CGulIcon*                   iMissedCallIcon;
    CGulIcon*                   iNewInstantMessageIcon;
    CGulIcon*                   iNewVoiceMessageIcon;
    CGulIcon*                   iNewEmailIcon;
    CGulIcon*                   iKeyguardIcon;
    RPointerArray<CGulIcon>     iBackground;

    CEikLabel*                  iTitleLabel;
    CEikLabel*                  iClockLabel;
    CEikLabel*                  iAmPmLabel;
    CFont*                      iIndicatorFont;

    CFbsBitmap*                 iOffScreenBitmap; // owned
    CFbsBitGc*                  iOSBitmapContext; // owned
    CFbsBitmapDevice*           iOSBitmapDevice;  // owned

    TSize                       iScreenSize;
    TRect                       iRectIndicators;
    TRect                       iRectPlaybackStatus;
};

#endif // C_CMPXSCREENSAVERPLUGINCONTAINER_H

