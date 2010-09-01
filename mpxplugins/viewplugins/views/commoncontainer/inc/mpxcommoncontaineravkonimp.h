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
* Description:  MPX common container definition for AVKON platform.
*
*/



#ifndef CMPXCOMMONCONTAINERAVKONIMP_H
#define CMPXCOMMONCONTAINERAVKONIMP_H


// INCLUDES
#include "mpxcommoncontainer.h"


// FORWARD DECLARATIONS
class CAknSearchField;
class CAknListBoxFilterItems;
class CMPXLbxExtendedFeatures;
class MEikCommandObserver;
class CEikMenuPane;
class CEikTextListBox;
class MEikListBoxObserver;
class CAknIconArray;
class CMPXCommonListBoxArrayBase;

// CLASS DECLARATION

/**
 *  Container class for AVKON common container.
 *  The view class should implement the following virtual function
 *  from MEikCommandObserver 
 *
 *  virtual void ProcessCommandL(TInt aCommandId);
 *
 *  common container uses this to pass commands back to the view class
 *  the commands supported are defined in mpxcommoncontainer.hrh
 *
 *  view container class owns this class to access List box commands, receive
 *  CoeControl events and GetHelpContext(). The view container class should
 *  implement the following virtual functions from MViewContainer
 *
 *  virtual void HandleKeyEvent();
 *  virtual void HandleHelpContent();
 *
 *  common container uses this to pass CoeControl events back to the view
 *  container class. 
 *
 *  view class should handle the following commands in ProcessCommandL
 *  from MEikCommandObserver: 
 *  
 *  EMPXCmdCommonDelete  // delete key pressed
 *  EMPXCmdCommonResourceChange  // resource change
 *
 *  @lib mpxcommoncontainer.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXCommonContainerAvkonImp )
    : public CCoeControl
    , public MMPXCommonContainer
    {
public:

    /**
     * default C++ constructor.
     */
    CMPXCommonContainerAvkonImp();
    
    /**
     * Destructor.
     */
    virtual ~CMPXCommonContainerAvkonImp();


// from base class MMPXCommonContainer

    /**
     * Return Coe Control
     *
     * @since 3.2
     */
    CCoeControl* CoeControl();

    /**
     * Set View Container
     *
     * @since 3.2
     */
    void SetViewContainer( MMPXViewContainer* aViewContainer );

    /**
     * Set command observer for container
     *
     * @since 3.2
     */
    void SetCommandObserver( MEikCommandObserver* aCommandObserver );

    /**
     * Set list box observer for container
     *
     * @since 3.2
     */
    void SetListBoxObserver( MEikListBoxObserver* aObserver );

    /**
     * Set layout for all elements and set sizes for icons.
     *
     * @since 3.1
     */
    void UpdateLayout();

    /**
     * Returns list box array, ownership not transfered
     *
     * @since 3.1
     * @return A pointer to the list box array
     */
    CMPXCommonListBoxArrayBase* ListBoxArray() const;
    
    /**
     * Sets up the listbox array, ownership not transferred
     */
    void SetListBoxArrayL( CMPXCommonListBoxArrayBase* aListBoxArray );

    /**
     * Return number of listbox items shown in the current listbox.
     *
     * @since 3.1
     * @return The number of items shown in the list box
     */
    TInt CurrentListItemCount() const;

    /**
     * Return total number of listbox items in the listbox.
     *
     * @since 3.1
     * @return Total items in the list box
     */
    TInt TotalListItemCount() const;

    /**
     * Gets top listbox item index (relative to the original listbox).
     * @return Top listbox item index
     *
     * @since 3.1
     */
    TInt TopLbxItemIndex() const;
    
    /**
     * Gets bottom listbox item index (relative to the original listbox).
     * @return Bottom listbox item index
     *
     * @since 3.1
     */
    TInt BottomLbxItemIndex() const;

    /**
     * Gets current listbox item index (relative to the original listbox).
     * @return Current listbox item index
     *
     * @since 3.1
     */
    TInt CurrentLbxItemIndex() const;

    /**
     * Get current selected listbox items indices (relative to the original
     * listbox). Ownership not transferred.
     *
     * @since 3.1
     * @return Current listbox item indices
     */
    const CArrayFix<TInt>* CurrentSelectionIndicesL() const;
    
    /**
     * Sewt current selected listbox items indices
     *
     * @since 3.1
     * @param aIndices Array of selection indices, ownership not transfered
     */
    void SetCurrentSelectionIndicesL( CArrayFix<TInt>* aIndices ) const;

    /**
     * Set top listbox item index.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    void SetLbxTopItemIndex( TInt aIndex );

    /**
     * Set current listbox item index.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    void SetLbxCurrentItemIndex( TInt aIndex );

    /**
     * Set current listbox item index and highlight it.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    void SetLbxCurrentItemIndexAndDraw( TInt aIndex );

    /**
     * Clear listbox selection.
     *
     * @since 3.1
     */
    void ClearLbxSelection();

    /**
     * Sets list box backround text. This text is visible if the list box 
     * has no items.
     *
     * @param aText The text for the empty list box background.
     * @since 3.2
     */
	void SetLbxEmptyTextL( const TDesC& aText );

    /**
     * Draws list box item index
     *
     * @since 3.2
     */
    void DrawLbxItemL( TInt aIndex );
    
    /**
     * Set Find box's focus.
     *
     * @since 3.1
     * @param aFocus ETrue if set focus on FindBox. Otherwise, EFalse.
     */
    void SetFindBoxFocus( TBool aFocus );

    /**
     * Handle listbox item addition.
     *
     * @since 3.1
     */
    void HandleLbxItemAdditionL();
    
    /**
     * Handle listbox item addition, preserving the current display index.
     *
     * @since 3.1
     */
    void HandleLbxItemAdditionPreserveIndexL();
    
    /**
     * Handle listbox item removal.
     *
     * @since 3.1
     */
    void HandleLbxItemRemovalL();

    /**
     * Show/hide find box
     *
     * @since 3.1
     * @param aIsVisible ETrue to set findbox visible. Otherwise, EFalse
     */
    void SetFindBoxVisibilityL( TBool aIsVisible );

    /**
     * Determine find box visibility
     *
     * @since 3.1
     * @return ETrue if find box is currently visible, EFalse otherwise
     */
    TBool FindBoxVisibility();

    /**
     * Restore the PopupCBA of find box
     */
    void RestoreFindBoxPopupCBA();
    
    /**
     * Calculate the top index of the visible items
     *
     * @since 3.1
     * @param aBottomIndex the bottom index of the visible items
     * @return top index
     */
    TInt CalculateTopIndex( TInt aBottomIndex );

    /**
     * Creates the container
     *
     * @since 3.1
     * @param aCollectionType collection type
     */
    void ConstructContainerL( TMPXCommonContainerCollectionType aCollectionType );

    /**
     * Enable/disable find box     
     * 
     * @since 3.1
     * @aEnable ETrue to enable find box, EFalse to disable
     */
    void EnableFindBox( TBool aEnable );
    
    /**
     * Enable/disable marking     
     * 
     * @since 3.1
     * @aEnable ETrue to enable marking, EFalse to disable
     */
    void EnableMarking( TBool aEnable );

    /**
     * Custom handling of commands for markable lists.
     */
    void HandleMarkableListProcessCommandL( TInt aCommand );

    /**
     * Custom handling of menu pane for markable lists
     */
    void HandleMarkableListDynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane );
        
    /**
     * Handle listbox array events
     *
     *  @param aEvent list box event
     */
    void HandleListBoxArrayEventL( 
        MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent );

    /**
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );
    
    /**
     * Activate the container
     */
    void ActivateContainerL();

    /**
     * Deactivate the container
     */
    void DeactivateContainerL();

    /**
     * Checks whether the container is active
     */
    TBool IsContainerActive();

// from base class CCoeControl

    /**
     * From CCoeControl.
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );
    
    /**
     * From CCoeControl.
     * Gets the control's help context.
     *
     * @param aContext The control's help context.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    
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

    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );
    
private:

    /**
     * Create list box 
     */
    void CreateListBoxL();
    
    /**
     * Constructs Find box.
     *
     * @param aIsFindPopup ETrue if findbox is a popup type.
     *                     EFalse if finsbox is a fixed type.
     */
    void ConstructFindBoxL( TBool aIsFindPopup );

    /**
     * Creates icon array, populates iIconArray to be used by listbox
     */   
    void CreateIconArrayL();
    
protected: // data

    CAknIconArray*              iIconArray;  

    MMPXViewContainer*          iViewContainer;    // not owned
    MEikCommandObserver*        iView;          // not owned
    MEikListBoxObserver*        iListBoxObserver; // not owned
    CEikTextListBox*            iListBox;       // owned
    CMPXLbxExtendedFeatures*    iLbxExtFeat;    // owned    
    CMPXCommonListBoxArrayBase* iListBoxArray;  // not owned
    
    CAknSearchField*            iFindBox;  // owned
    CAknListBoxFilterItems*     iFindBoxFilterItems; // not owned
    TBool                       iIsFindPopup;    
    TBool                       iIsFindEnabled;
    TBool                       iIsMarkingEnabled;
    TInt                        iResourceOffset; // must be freed
    TBool                       iContainerActive;
    };

#endif  // CMPXCOMMONCONTAINERAVKONIMP_H

// End of File
