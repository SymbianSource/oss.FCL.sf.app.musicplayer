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



#ifndef C_CMPXNOTIFIERDIALOG_H
#define C_CMPXNOTIFIERDIALOG_H


// INCLUDES
#include <coecntrl.h>
#include <mpxpskeyobserver.h>   // MMPXPSKeyObserver
#include <mpxmediakeyhandlerdefs.h>


// FORWARD DECLARATIONS
class MAknsSkinInstance;
class TAknsItemID;
class CEikLabel;
class CEikImage;
class CMPXPSKeyWatcher;

// CLASS DECLARATION

/**
 *  Music Player playback info popup.
 *
 *  @lib mpxmediakeyhandler.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXNotifierDialog ) : public CCoeControl,
										  public MMPXPSKeyObserver
    {
public:

    enum TMPXPlaybackPopupIcons
        {
        EMPXPopupNoneIcon = 0,
        EMPXPopupPlayIcon,
        EMPXPopupPauseIcon,
        EMPXPopupFFIcon,
        EMPXPopupFRIcon
        };

    enum TMPXPlaybackPopupComponents
        {
        EMPXPlayIcon = 0,
        EMPXPauseIcon,
        EMPXFFIcon,
        EMPXFRIcon,
        EMPXTrackLabel,
        EMPXTimeLabel,
        EMPXComponentsCount
        };

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXNotifierDialog* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXNotifierDialog();

    /**
     * Shows pop-up dialog.
     *
     * @since 3.0
     * @param aIcon used icon
     * @param aTitle song tittle
     * @param aTime playback time in seconds
     * @param aMode Mode to determine the popup's behaviour
     */
    void ShowInfoPopupL(
        TMPXPlaybackPopupIcons aIcon,
        const TDesC& aTitle,
        TInt aTime,
        TMPXPlaybackPopupModes aMode );

    /**
     * Removes pop-up from screen.
     *
     * @since 3.0
     */
    void CancelInfoPopup();

    /**
     * Gets called when orientation change begins
     */        
    void NotifyOrientationChangeBegin();
    
private:

    /**
     * C++ default constructor.
     */
    CMPXNotifierDialog();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Updates pop-up dialog contents.
     *
     * @param aIcon used icon
     * @param aTitle song tittle
     * @param aTime playback time in seconds
     */
    void UpdateInfoPopupL(
        TMPXPlaybackPopupIcons aIcon,
        const TDesC& aTitle,
        TInt aTime );

    /**
     * Create icons.
     */
    void CreateIconsL();

    /**
     * Constructs skinnable icon.
     *
     * @param aImage Pointer to the skinnable icon on return.
     * @param aSkin Pointer to the current skin instance.
     * @param aId Item ID of the masked bitmap to be created.
     * @param aColorId Item ID of the color table.
     * @param aColorIndex Index in the color table.
     * @param aBitmapId mbm id of the bitmap
     * @param aMaskId   mbm id of the mask
     */
    void ConstructImageL(
        CEikImage* aImage,
        MAknsSkinInstance* aSkin,
        const TAknsItemID& aId,
        const TAknsItemID& aColorId,
        TInt aColorIndex,
        TInt aBitmapId,
        TInt aMaskId );

    /**
     * Resize info popup.
     */
    void ResizeInfoPopup();

    /**
     * Sets pop-up layouts.
     */
    void SetLayoutsL( TBool aLong = EFalse );

    /**
     * Show playback icon.
     *
     * @param aIcon Icon to be displayed.
     * @return ETrue if it's ok to displayed. Otherwise, EFalse.
     */
    void ShowIcon(
        TMPXPlaybackPopupIcons aIcon );

    /**
     * Callback for timer
     *
     * @param aPtr Pointer pass to this callback function.
     * @return Zero if callback function doesn't need to be called again.
     *         Otherwise, non-zero.
     */
    static TInt TimerCallback( TAny* aPtr );

    /**
     * Convert to displayable duration
     *
     * @since S60 v3.0
     * @param aduration Duration in seconds
     * @return A heap descriptor that contains displayable duration
     *         (ownership transferred). Caller must destroy this object
     *         after use.
     */
    HBufC* DisplayableDurationL(
        TInt aDuration );

// from base class CoeControl

    /**
     * From CCoeControl.
     * Draws the control.
     *
     * @param aRect The rectangular region of the control to be drawn.
     */
    void Draw( const TRect& aRect ) const;

    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );

    /**
     * From CCoeControl.
     * Responds to changes to the size and position of the contents of this
     * control.
     */
    void SizeChanged();

    /**
     * From CCoeControl.
     * Gets the number of controls contained in a compound control.
     *
     * @return The number of component controls contained by this control.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     * Gets an indexed component of a compound control.
     *
     * @param aIndex The index of the control.
     * @return The component control with an index of aIndex.
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;
protected:    // from MMPXPSKeyObserver

	/**
	* From MMPXPSKeyObserver
	* Handle PS event
	*
	* @param aUid The UID that identifies the property category
	* @param aKey The property sub-key
	*/
	void HandlePSEvent( TUid aUid, TInt aKey );
    
	
private:    // Data

    CEikLabel* iSongLabel;  // own
    CEikLabel* iTimeLabel;  // own
    CEikImage* iPlayIcon;   // own
    CEikImage* iPauseIcon;  // own
    CEikImage* iFFIcon;   // own
    CEikImage* iFRIcon;  // own
    CPeriodic* iTimer;      // own
    RWindowGroup iWindowGroup;  // own
    CMPXPSKeyWatcher* iPSKeyWatcher;    // own

    TMPXPlaybackPopupIcons iIconShown;
    
    TBool iReopenDialog;
    };

#endif  // C_CMPXNOTIFIERDIALOG_H

// End of File
