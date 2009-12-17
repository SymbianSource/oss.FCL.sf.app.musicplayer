/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that implements Search Field feature.
*
*/


#ifndef CMPXCOMMONCONTAINERSEARCHFIELD_H
#define CMPXCOMMONCONTAINERSEARCHFIELD_H

// INCLUDE FILES
#include <coecntrl.h>
#include <eikedwin.h>
#include <Avkon.mbg>

// FORWARD DECLARATION
class CAknInputFrame;
class CMPXCommonContainerSearchField;
class CMPXCommonContainerSearchFieldWrapper;

// CLASS DECLARATION

/**
 * Class for handling key events. Has higher priority than FEP,
 * to be able to catch up/down key event and forward them to Search
 * Field's editor.
 */
NONSHARABLE_CLASS( CMPXCommonContainerSearchFieldKeyHandler ) :
                           public CCoeControl,
                           public MEikEdwinObserver
    {
    public:

        /**
         * public constructor.
         *
         * @param aSearchField Reference to Search Field that key events are controlled.
         */
        CMPXCommonContainerSearchFieldKeyHandler( CMPXCommonContainerSearchField& aSearchField );

        /**
         * Catches key events before FEP could do it.
         *
         * @param aKeyEvent Key event.
         * @param aType Key event type.
         * @return Response whether key was handled or not.
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * Catches EEventTextUpdate event.
         *
         * @param aEdwin Pointer to editor.
         * @param aEventType Editor event type.
         */
        void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );

        /**
         * Put control to control stack or remove it from there.
         *
         * @param aEnable ETrue puts control to the control stack with higher
         *        priority than FEP has. EFalse removes control from the stack.
         */
        void EnableL( TBool aEnable );

        /**
         * Resets key handling situation by setting iFirstKeyEvent to ETrue.
         */
        void Reset();

    /**
     * Get LeftRightEvent.
     */
    TBool IsLeftRightEvent();

    /**
     * SetFirstKey.
     */
    void SetFirstKeyEvent( TBool aFirstKeyEvent );

    protected:

        /**
         * Key event catching enable flag.
         */
        TBool iEnabled;

        /**
         * Only first key event is catched.
         */
        TBool iFirstKeyEvent;

        /**
         * Flag for LeftRightEvent.
         */
        TBool iLeftRightEvent;

        /**
         * Search Field control of which key events are catched.
         */
        CMPXCommonContainerSearchField& iSearchField;
    };

/**
 * Search Field class for component.
 *
 */
NONSHARABLE_CLASS( CMPXCommonContainerSearchField ) : public CCoeControl,
                                         public MCoeControlObserver,
                                         public MCoeFepObserver
    {
    public:

        /**
         * Two-phased constructor. Leaves on failure.
         *
         * @param aParent Parent control.
         * @param aFindKeywordMode ETrue if the pane is a find keyword pane.
         * @return The constructed Search Field control.
         */
        static CMPXCommonContainerSearchField* NewL( CMPXCommonContainerSearchFieldWrapper* aParent,
                                        TBool aFindKeywordMode = ETrue );

        /**
         * Destructor.
         */
        virtual ~CMPXCommonContainerSearchField();

        /**
         * Shows or hides to Search Field.
         *
         * @param ETrue to set visible
         */
        void MakeVisibleL( TBool aVisible );

        /**
         * Non-leaving version of MakeVisibleL.
         *
         * @param ETrue to set visible
         */
		void MakeVisible( TBool aVisible );

        /**
         * Called to handle change in Search Field size.
         */
        void HandleFindSizeChanged();

        /**
         * Enable key event handler.
         *
         * @param aEnable ETrue to enable.
         */
        void EnableKeyEventHandlerL( TBool aEnable );

    public:	// From MCoeControlObserver

        /**
         * Handle control event.
         *
         * @param aControl The control.
         * @param aEventType Event type.
         */
	    void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

    public: // From CCoeControl

        /**
         * Handles key event.
         *
         * @param aKeyEvent The key event.
         * @param aType Key event type.
         * @return Response whether event was consumed.
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * HandlePointerEventL, from CCoeControl.
         *
         * @param aPointerEvent Pointer event.
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
         * Count component controls.
         *
         * @return Number of component controls
         */
        TInt CountComponentControls() const;

        /**
         * Get a component control by index.
         *
         * @param aIndex Index of component control to be returned
         * @return Component control or NULL
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         * Sets the OrdinalPosition of Search Field.
         *
         * @param aPos The OrdinalPosition
         */
        void SetOrdinalPosition( TInt aPos );

    public: // Text manipulation.

        /**
         * Lengt of the text in the editor.
         *
         * @return The text length
         */
        TInt TextLength() const;

        /**
         * Get text from the editor.
         *
         * @return text from the editor
         */
        HBufC* GetTextLC() const;

        /**
         * Set text.
         *
         * @param aTxt Text to set, appended if needed.
         */
        void SetTextL( const TDesC& aTxt );

        /**
         * Selects all text in the editor, see CEikEdwin's SelectAllL().
         */
        void SelectAllL();

        /**
         * Set info text.
         *
         * @param aText Text to be displayed as info
         */
        void SetInfoTextL( const TDesC& aText );

        /**
         * Perform clipboard functionality.
         *
         * @param aClipboardFunc Function to perform.
         */
        void ClipboardL( CEikEdwin::TClipboardFunc aClipboardFunc );

        /**
         * Returns editor control of Search Field.
         *
         * @return Editor control.
         */
        CEikEdwin* Editor() const;

        /**
         * Checks if client's keyword needs updating, and updates if needed.
         */
        void UpdateFindKeywordL();

    public: // Observer support.

        /**
         * Handles resource change.
         *
         * @param aType Type of resource change.
         */
		void HandleResourceChange( TInt aType );

        /**
         * Handles the completion of a FEP transaction.
         */
		void HandleCompletionOfTransactionL();

        /**
         * Handles the start of a FEP transaction.
         */
		void HandleStartOfTransactionL();

    protected: // Construction.

        /**
         * Constructor.
         *
         * @param aFindKeywordMode ETrue if the pane is a find keyword pane.
         */
        CMPXCommonContainerSearchField( TBool aFindKeywordMode );

        /**
         * Second-phase constructor. Leaves on failure.
         *
         * @param aParent Parent control.
         */
        void ConstructL( CMPXCommonContainerSearchFieldWrapper* aParent );

    protected: // From CCoeControl.

        /**
         * Handle size change, layout and draw components.
         */
        void SizeChanged();

        /**
         * Handle focus change.
         *
         * @param aDrawNow ETrue if immediate draw is needed.
         */
        void FocusChanged( TDrawNow aDrawNow );

    private:

        /**
        * Set the text and redraw.
        *
        * @param aTxt Text to set.
        */
        void DoSetTextL( const TDesC& aTxt );

        /**
         * Activates virtual keyboard.
         */
        void ActivateVkbL();

    protected: // Data.

        /**
         * Pointer to parent (control).
         * Not own.
         */
        CMPXCommonContainerSearchFieldWrapper* iParent;

        /**
         * If the control is in find keyword pane mode.
         */
		TBool iFindKeywordMode;

		/**
		 * HandleFEPFind.
		 */
		TBool iHandleFEPFind;

		/**
		 * The previously typed text into iEditor.
		 */
		HBufC* iPrevKeyword;

		/**
		 * Editor.
		 * Own.
		 */
        CEikEdwin* iEditor;

        /**
         * Input frame.
         * Own.
         */
        CAknInputFrame* iInputFrame;

        /**
         * Key handler.
         * Own.
         */
        CMPXCommonContainerSearchFieldKeyHandler *iSearchFieldKeyHandler;
    };

#endif // CMPXCOMMONCONTAINERSEARCHFIELD_H
