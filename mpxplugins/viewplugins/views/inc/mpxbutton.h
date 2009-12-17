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
* Description:  Push-button in Common Playback View.
*
*/



#ifndef CMPXBUTTON_H
#define CMPXBUTTON_H


// INCLUDES
#include "mpxcommonplaybackviewdefs.h"

// FORWARD DECLARATIONS
class CGulIcon;
class CWindowGc;
class CAknKeySoundSystem;
class MAknsControlContext;
class MMPXPlaybackViewLayout;

NONSHARABLE_STRUCT( TMPXButtonInfo )
    {
    TInt iState;
    CGulIcon* iBitmap;
    TInt iEnterCommand;
    TInt iExitCommand;
    TInt iLongEnterCommand;
    TInt iLongExitCommand;
    };

// CLASS DECLARATION

/**
 * Observer interface to receive commands from button presses.
 *
 *  @lib mpxplaybackview.dll
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( MMPXButtonCmdObserver )
    {
    public:
        /**
         * Handle a button command.
         * @param aCommand Command id, as specified in the button constructor.
         */
        virtual void HandleButtonCommandL( TInt aCommand ) = 0;

    };


/**
*  Push-button in Playback View. This class encapsulates the graphics,
*  state (pushed, dimmed, highlighted), and layout information of the button.
*  The button calls the HandleButtonCommandL() function for the observer
*  object when it is pushed.
*
*  @lib MusicPlayer.app
*  @since 3.0
*/
NONSHARABLE_CLASS( CMPXButton ) : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @since 3.0
        * @param aButtonId   Id for the button (determines layout and graphics)
        * @param aObserver   Pointer to an object implementing the
        *                    MMPlayerButtonCmdObserver interface.
        * @param aLayout     Layout for this view
        * @return New CMPlayerButton instance.
        */
        static CMPXButton* NewL(
            TMPXPbvButton aButtonId,
            MMPXButtonCmdObserver* aObserver,
            MMPXPlaybackViewLayout* aLayout );

        /**
        * Two-phased constructor.
        * @since 3.0
        * @param aButtonId   Id for the button (determines layout and graphics)
        * @param aObserver   Pointer to an object implementing the
        *                    MMPlayerButtonCmdObserver interface.
        * @param aLayout     Layout for this view
        * @return New CMPlayerButton instance.
        */
        static CMPXButton* NewLC(
            TMPXPbvButton aButtonId,
            MMPXButtonCmdObserver* aObserver,
            MMPXPlaybackViewLayout* aLayout );

        /**
        * Destructor.
        */
        virtual ~CMPXButton();

    public: // New functions
        
        /**
        * Adds a new state to this button
        * @since 3.0
        * @param aState         ID of the state
        * @param aEnterCommand  Command ID to execute when it enters this state
        * @param aExitCommand   Command ID to execute whne it exits this state
        * @param aLongEnterCommand  Command ID executes on a long press
        * @param aLongExitCommand   Command ID to execute after a long press
        */
        void AddStateL( 
            TInt aState,
            TInt aEnterCommand,
            TInt aExitCommand,
            TInt aLongEnterCommand,
            TInt aLongExitCommand );
        
        /**
        * Get the id of the button.
        * @since 3.0
        * @return Button id (as specified in NewL())
        */
        TMPXPbvButton Id() const;

        /**
        * Hide/show this button
        * @since 3.0
        * @param aVisible  ETrue if this button is to be visible
        */
        void MakeVisible( TBool aVisible );

        /**
        * Get the visibility status of the button.
        * @since 3.0
        * @return ETrue if this button is visible
        */
        TBool IsVisible() const;
        
        /**
        * Set the state of this button
        * @since 3.0
        * @param aState     New state to set to
        * @param aNotifyExit Whether or not to notify observer of the
        *                    exit command of previous state
        * @param aNotifyEnter Whether or not to notify observer of the
        *                     enter command of the new state
        */
        void SetButtonState( TInt aState,   
                             TBool aNotifyExit=EFalse,
                             TBool aNotifyEnter=EFalse );
                
        /**
        * Get the current state of the button
        * @since 3.0
        * @return State of the button
        */
        TInt CurrentState() const;
        
        /**
        * Draw the button according to its current state.
        * @since 3.0
        * @param aGc   Graphics context to draw to
        * @param aRect Dirty rectangle. Drawing can be clipped inside this
        *              rectangle.
        */
        void RedrawRect(CBitmapContext& aGc, const TRect& aRect) const;

        /**
        * Reload graphics.
        * @since 3.0
        * @param aParentRect New parent rectangle
        */
        void ReloadIconsL(const TRect& aParentRect);

        /**
        * Set background bitmap
        * @since 3.0
        * @param aBackground       Pointer to the skin background control
        *                          context. Ownership not transferred.
        */
        void SetBackground(MAknsControlContext* aBackground);

        /**
        * Get the redraw flag.
        * @since 3.0
        * @return ETrue if this button needs to be redrawn
        */
        TBool NeedRedraw() const;

        /**
        * Clear the redraw flag.
        * @since 3.0
        */
        void ClearRedrawFlag();

    private: // New functions
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * C++ constructor
        */
        CMPXButton(
            TMPXPbvButton aButtonId,
            MMPXButtonCmdObserver* aObserver,
            MMPXPlaybackViewLayout* aLayout );

        /**
        * Load the bitmap and mask for given state.
        * @return CGulIcon containing bitmap and mask
        */
        CGulIcon* LoadIconL( TInt aState );

        /**
        * Free all icons
        */
        void FreeIcons();

        /**
        * Create a more translucent version of the mask to be used
        * when the button is dimmed.
        */
        void CreateTmpDimmedMaskL();

        /**
        * Handle long press event.
        */
        void HandleLongPress();

        /**
        * Callback for long-press timer
        */
        static TInt TimerCallback(TAny* aPtr);

    private:     // Data
        TMPXPbvButton iButtonId;
        TInt iCurrentStateIndex;
        RPointerArray<TMPXButtonInfo> iButtonArray; // owned
        MMPXButtonCmdObserver* iObserver; // not owned
        
        TRect iParentRect;
        TRect iRect;

        TBool iVisible;
        TBool iIsLongPress;

        CPeriodic* iTimer; // owned

        MAknsControlContext* iBackground; // not owned

        TBool iNeedRedraw;

        CAknKeySoundSystem* iKeySoundSystem;  // not owned
        TBool iSelectionKeySilent;
        MMPXPlaybackViewLayout* iLayout;    // not owned
    };

#endif      // CMPXBUTTON_H

// End of File
