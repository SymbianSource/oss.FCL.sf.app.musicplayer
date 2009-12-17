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
* Description:  Button state manager for common playback view
*
*/



#ifndef CMPXBUTTONMANAGER_H
#define CMPXBUTTONMANAGER_H

//  INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>

#include "mpxbutton.h"
#include "mpxplaybackframeworkdefs.h"

//  FORWARD DECLARATIONS
class CWindowGc;
class MAknsControlContext;
class MMPXPlaybackViewLayout;

class CAknButton;
class CAknViewAppUi;
class CGulIcon;
// CLASS DECLARATION

/**
*  Button state manager for playback view.
*
*  @lib mpxplaybackview.dll
*  @since S60 v3.0
*/
NONSHARABLE_CLASS( CMPXButtonManager ) :public CCoeControl,
                                        public MCoeControlObserver,
                                        public MCoeControlBackground
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * since 5.0
        * @param aObserver          Observer to receive commands from button
        *                           pushes.
        * @param aLayout            Layout for this view
        * @param aContainer			Container for this control
        */
        static CMPXButtonManager* NewL(
            MMPXButtonCmdObserver* aObserver,
            MMPXPlaybackViewLayout* aLayout,
            const CCoeControl &aContainer);
        /**
        * Destructor.
        */
        virtual ~CMPXButtonManager();

    public:

        /**
        * Update button state based on the state of the engine
        * @param aState State of the playback engine
        */
        void UpdateButtonStates(TMPXPlaybackState aState);

        /**
        * Handle a key event.
        * @see CCoeControl::OfferKeyEventL
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);

        /**
        * Set parent rectangle for the button layout
        * @since 3.0
        * @param aRect Parent rectangle
        */
        void SetParentRect(const TRect& aRect);

        /**
        * Sets to enable/display the button or not
        */
        void SetEnabled( TBool aEnabled );

        /**
         * Handles losing foreground
         *
         * @since S60 3.0
         */
        void HandleForegroundLostL();

	    /**
	     * From CCoeControl.
	     * Gets the number of controls contained in a compound control.
	     * since 5.0
	     *
	     * @return The number of component controls contained by this control.
	     */
	    TInt CountComponentControls() const;

	    /**
	     * From CCoeControl.
	     * Gets an indexed component of a compound control.
	     * since 5.0
	     *
	     * @param aIndex The index of the control.
	     * @return The component control with an index of aIndex.
	     */
	    CCoeControl* ComponentControl( TInt aIndex ) const;

		/**
		* From MCoeControlObserver.
		* To handle the button events.
		* since 5.0
		*/
		void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

	    /**
	     * From CoeControl.
	     * Handles a change to the control's resources.
		 * since 5.0
	     *
	     * @param aType A message UID value.
	     */
	    void HandleResourceChange( TInt aType );

	    void CMPXButtonManager::SizeChanged();

	    /**
	     * From MCoeControlBackground
		 * since 5.0
	     */
	    void Draw(CWindowGc& aGc, const CCoeControl& aControl, const TRect& aRect) const;
        
	    /**
        * restore button state in some special case
        * @since S60 3.0
        * @param aState State of the playback engine
        */
        void RestoreButtonStates( TMPXPlaybackState aState );

    private:
        /**
        * C++ default constructor.
        */
        CMPXButtonManager(
            MMPXButtonCmdObserver* aObserver,
            MMPXPlaybackViewLayout* aLayout);
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const CCoeControl &aContainer);

    private: // New functions

        /**
        * Create the rocker button using multiple avkon buttons
        */
        void CreateRockerAvkonButtonsL(const CCoeControl &aContainer);

        /**
        * Sets the avkon rocker button state and send observer commands
        */
        TKeyResponse SetAvkonButtonState( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Callback for long-press timer
        */
        static TInt TimerCallback(TAny* aPtr);

        /**
        * Handle long press event.
        */
        void HandleLongPress();

    private:    // Data
        TRect iParentRect;
        TInt iButton;
        MMPXButtonCmdObserver* iObserver;          // not owned

        TBool iIsEmbedded;
        TBool iProgDownloadMode;
        TBool iIsCDPreview;
        TBool iKeyDownEventReceived;
        MMPXPlaybackViewLayout* iLayout;    // not owned
        TBool iEnabled;
        TBool iKeyDownReceived;
        TBool iIsSeeking;
        RPointerArray<CAknButton> iAvkonButtons;        // owned
        TInt iRockerAvkonButtonCount;
        TBool iVolumeInRocker;
        TBool iIsTouchUi;
        TBool iStopInRocker;
        TBool iIsLongPress;
        CPeriodic* iLongKeyTimer;
        TInt iLongPressCommandId;
    };

#endif      // CMPXBUTTONMANAGER_H

// End of File
