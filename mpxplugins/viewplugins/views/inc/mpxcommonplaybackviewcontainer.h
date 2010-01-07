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
* Description:  MPX Common playback view's container implementation.
*
*/



#ifndef CMPXCOMMONPLAYBACKVIEWCONTAINER_H
#define CMPXCOMMONPLAYBACKVIEWCONTAINER_H


// INCLUDES
#include <coecntrl.h>

#include "mpxcommonplaybackviewdefs.h"
#include "mpxalbumartutilobserver.h"
#include "mpxbutton.h"
#include "mpxplaybackframeworkdefs.h"


// FORWARD DECLARATIONS
class MEikCommandObserver;
class MAknsControlContext;
class CAknIconArray;
class CMPXPlaybackViewInfoLabel;
class MMPXLayoutSwitchObserver;
class CMPXCommonUiHelper;
class CMPXButtonManager;
class MMPXPlaybackViewLayout;

// CLASS DECLARATION

/**
 *  Container class for audio view.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
class CMPXCommonPlaybackViewContainer : public CCoeControl,
                                                 public MMPXAlbumArtUtilObserver,
                                                 public MMPXButtonCmdObserver
    {
public:

    /**
     * Symbian 2nd phase constructor.
     *
     * @param aRect Frame rectangle for container.
     */
    IMPORT_C virtual void ConstructL( const TRect& aRect );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CMPXCommonPlaybackViewContainer();

    /**
     * Updates time indicators.
     *
     * @since S60 3.0
     * @param  aPlaybackPosInSeconds, New playback postions
     * @param  aTotalLengthInSeconds, Clips duration
     */
    IMPORT_C virtual void UpdateTimeIndicatorsL(
        TInt aPlaybackPosInSeconds,
        TInt aTotalLengthInSeconds );

    /**
     * Updates label
     *
     * @since S60 3.0
     * @param aLabel Label which is updated
     * @param aText New text value
     */
    IMPORT_C virtual void UpdateLabelL(
        TMPXPbvTextIndicator aLabel,
        const TDesC& aText );

    /**
     * Sets main pane mode.
     *
     * @since S60 3.0
     * @param aMode Playback mode.
     */
    IMPORT_C virtual void SetMode( TMPXPbvPlaybackMode aMode );

    /**
     * Set random mode.
     *
     * @since S60 3.0
     * @param aRandomMode ETrue if random is on.
     */
    IMPORT_C virtual void SetRandomMode( TBool aRandomMode );

    /**
     * Set repeat mode.
     *
     * @since S60 3.0
     * @param aRepeatAll ETrue if repeat all is on.
     * @param aRepeatOne ETrue if repeat one is on.
     */
    IMPORT_C virtual void SetRepeatMode(
        TBool aRepeatAll,
        TBool aRepeatOne );

    /**
     * Set Real Audio mode.
     *
     * @since S60 3.2
     * @param aRealAudioMode ETrue if a Real Audio file is being played.
     */
    IMPORT_C virtual void SetRealAudioMode( TBool aRealAudioMode );

    /**
    * Update buttons according to current state.
    *
    * @since 3.0
    */
    IMPORT_C virtual void UpdateButtons( TMPXPlaybackState aState );

    /**
     * Set layout for all elements and set sizes for icons.
     *
     * @since S60 3.0
     */
    IMPORT_C virtual void UpdateLayout();

    /**
     * Updates duration label.
     *
     * @since S60 3.0
     * @param  aTotalLengthInSeconds, Clips duration
     */
    IMPORT_C virtual void UpdateDurationLabelL( TInt aTotalLengthInSeconds );

    /**
     * Force softkey label colors to what is specified in skin
     * for Music Player soft keys.
     *
     * @since S60 3.0
     */
    IMPORT_C virtual void UpdateSoftkeyColors();

    /**
     * Handles foreground events
     *
     * @since S60 3.0
     * @param aForeground ETrue if app is going to foreground, EFalse otherwise
     */
    IMPORT_C virtual void HandleForegroundEventL( TBool aForeground );

// from base class MMPXAlbumArtUtilObserver

    /**
     * From MMPXAlbumArtUtilObserver.
     * Notify that extraction of album art started.
     */
    IMPORT_C virtual void ExtractAlbumArtStarted();

    /**
     * From MMPXAlbumArtUtilObserver.
     * Album art is extracted from file and ready to use.
     *
     * @param aBitmap a converted image. Ownership is transferred.
     * @param aErr error code
     */
    IMPORT_C virtual void ExtractAlbumArtCompleted(
        CFbsBitmap* aBitmap,
        TInt aErr );

// from base class CoeControl

    /**
     * From CoeControl.
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    IMPORT_C virtual TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    IMPORT_C virtual void HandleResourceChange( TInt aType );

    /**
     * From CoeControl.
     * Gets the control's help context.
     *
     * @param aContext The control's help context.
     */
    IMPORT_C virtual void GetHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * Starts marquee for a playback label
     *
     * @param aLabel Label to start marquee
     */
    IMPORT_C virtual void StartMarquee( TMPXPbvTextIndicator aLabel );

protected:






    /**
     * Creates labels.
     */
    IMPORT_C virtual void CreateLabelsL();

    /**
     * Creates icons.
     * Any subclass which overrides this must call this base class method
     * first or else the common icons will not be created.
     */
    IMPORT_C virtual void CreateIconsL();

    /**
     * Deletes all icon objects.
     */
    IMPORT_C void FreeIcons();

    /**
     * Draw an icon
     *
     * @since 3.0
     * @param aGc            Graphics context to draw to
     * @param aDirtyRect     Part of the screen that needs redrawing
     * @param aIndicatorRect Position for the icon
     * @param aIcon          Icon and mask
     * @param aInvertMask    Invert mask (see CBitmapContext::BitBltMasked)
     */
    IMPORT_C virtual void DrawIndicator(
        CWindowGc& aGc ,
        const TRect& aDirtyRect,
        const TRect& aIndicatorRect,
        const CGulIcon* aIcon,
        TBool aInvertMask = ETrue ) const;

    /**
     * Redraw part of the screen to the bitmap context.
     *
     * @param aRect Area to be redrawn.
     * 
     */
  IMPORT_C virtual void RedrawRect(
         const TRect& aRect) const;

    /**
     * Update progress bar graphics based on current playback
     * and download position.
     */
    IMPORT_C virtual void UpdateProgressBarGraphics();

    /**
     * Update progress bar graphics and redraw.
     */
    IMPORT_C virtual void RefreshProgressBar();

    /**
     * Update label text color according to skin
     */
    IMPORT_C virtual void UpdateLabelColorsL();

    /**
     * Update skin control context for background
     *
     * @param aRect Area to be updated.
     */
    IMPORT_C virtual void UpdateBackgroundSkinControlContext( const TRect& aRect );

    /**
     * Set FMTX mode.
     *
     * @since S60 3.2
     * @param aFMTXMode ETrue if the view supports FMTX label.
     */
    IMPORT_C virtual void SetFMTXMode( TBool aFMTXMode );

// from base class CoeControl

    /**
     * From CCoeControl.
     * Responds to changes to the size and position of the contents of this
     * control.
     */
    IMPORT_C virtual void SizeChanged();

    /**
     * From CCoeControl.
     * Gets the number of controls contained in a compound control.
     *
     * @return The number of component controls contained by this control.
     */
    IMPORT_C virtual TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     * Gets an indexed component of a compound control.
     *
     * @param aIndex The index of the control.
     * @return The component control with an index of aIndex.
     */
    IMPORT_C virtual CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * From CCoeControl.
     * Draws the control.
     *
     * @param aRect The rectangular region of the control to be drawn.
     */
    IMPORT_C virtual void Draw( const TRect& aRect ) const;

// from base class MMPXButtonCmdObserver

    /**
     * From MMPXButtonCmdObserver
     * Handle a button command.
     * @param aCommand Command id, as specified in the button constructor.
     */
   IMPORT_C virtual void HandleButtonCommandL( TInt aCommand );

    /**
     * From CCoeControl.
     * Handle events from the touch screen
     * since 5.0
     */
    IMPORT_C virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

public:

    /**
     * Gets song position (from touch progress bar)
     * since 5.0
     */
    IMPORT_C virtual TInt GetNewSongPosition();
    
    /**
     * Sets song position (from currently playback position)
     * since 5.0
     */
    void SetNewSongPosition( const TInt64& aPositon );

    /**
     * Restore Buttons' state in some special case
     *
     * @since S60 3.0
     */
    void RestoreButtons( TMPXPlaybackState aState );

    TInt CallSetPosition();

    static TInt CallSetPosition( TAny* aAny );
    
    /**
     * Sets the ordinal position of a window.
     *
     * @param aNewOrdinalPosition The window's new ordinal position
     * 
     * @since S60 3.0
     */
    void AdjustOrdinalPosition( TInt aNewOrdinalPosition );

private:

    /**
     * Set layout for all elements and set sizes for icons.
     */
    void DoUpdateLayoutL();

 protected:   // data

    MEikCommandObserver* iCommandObserver;  // not owned
    MMPXLayoutSwitchObserver*    iLayoutObserver;   // not owned
    RPointerArray<CMPXPlaybackViewInfoLabel> iLabels;   // owned


 


    HBufC* iMinSecFormatString;         // owned
    HBufC* iShortFormatString;          // owned

    CGulIcon* iPlayIcon;                // owned
    CGulIcon* iPauseIcon;               // owned
    CGulIcon* iRandomIcon;              // owned
    CGulIcon* iRepeatOneIcon;           // owned
    CGulIcon* iRepeatAllIcon;           // owned

    CGulIcon* iDownloadSlider;          // owned
    CGulIcon* iPlaybackSlider;          // owned
    CGulIcon* iSliderBackground;        // owned

    CGulIcon* iDefaultAlbumArt;         // owned
    CFbsBitmap* iTrackAlbumArt;         // owned

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    CGulIcon* iRealIcon;                // owned
#endif

    MAknsControlContext* iBackground;   // owned

    CMPXCommonUiHelper* iCommonUiHelper;    // owned

    TRect iBackgroundLayout;
    TRect iIconLayout;
    TRect iRandomIconRect;
    TRect iRepeatIconRect;
    TRect iSliderPaneRect;
    TRect iSliderBackgroundRect;
    TRect iPlaybackSliderRect;
    TRect iDownloadSliderRect;
    TRect iAlbumArtRect;
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    TRect iRealIconRect;
#endif

    TMPXPbvPlaybackMode iMode;

    TBool iRepeatAllMode;
    TBool iRepeatOneMode;
    TBool iRandomMode;
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    TBool iRealAudioMode;
#endif
    TBool iFMTXMode;

    TRect iCurrentLayout;

    // Progress bar position - download
    TReal iDownloadRatio;

    // Progress bar position - playback
    TReal iPlaybackRatio;

    TBool iEqualizerEnabled;
    TBool iEnableButtons;
    TBool iEnableVolumeIndicator;
    TBool iEnableMarquee;
    TBool iDisplayAlbumArt;
    TBool iPrerollCompleted;

    CMPXButtonManager*  iButtonManager; // owned
    CIdle* iIdle;
    TInt iIdleTaskCompleted;
    MMPXPlaybackViewLayout* iLayout;    // not owned
    CPeriodic* iSeekTimer;

    TInt iTrackTotalLengthInSeconds;
    TInt64 iNewSongPosition; // Final position from touch progress bar
    TBool iTouchDown;        // Flag indicating touch progress bar is in use
    TBool iActiveView;



    TBool iDragProgressBar; //Flag drag progress bar
    };

#endif  // CMPXCOMMONPLAYBACKVIEWCONTAINER_H

// End of File
