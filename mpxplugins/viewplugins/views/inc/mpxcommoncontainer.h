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
* Description:  MPX common container definition.
*
*/



#ifndef MMPXCOMMONCONTAINER_H
#define MMPXCOMMONCONTAINER_H


// INCLUDES
#include <coecntrl.h>
#include "mpxcommonlistboxarrayobserver.h"
#include "mpxcommonlistboxarraybase.h"
#include "mpxviewcontainer.h"

// FORWARD DECLARATIONS
class CEikMenuPane;
class MEikCommandObserver;
class MEikListBoxObserver;
class MMPXCommonListBoxArrayObserver;

// CLASS DECLARATION

/**
 *  Abstract class for common container.
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
class MMPXCommonContainer
    {
public:

    enum TMPXCommonContainerCollectionType
        {
        EMPXCommonContainerMusic,
        EMPXCommonContainerPodcast
        };

    /**
     * Destructor.
     */
    virtual ~MMPXCommonContainer() {};

    /**
     * Return Coe Control
     *
     * @since 3.2
     */
    virtual CCoeControl* CoeControl() = 0;

    /**
     * Set View Container
     *
     * @since 3.2
     */
    virtual void SetViewContainer( MMPXViewContainer* aViewContainer ) = 0;

    /**
     * Set command observer for container
     *
     * @since 3.2
     */
    virtual void SetCommandObserver(
        MEikCommandObserver* aCommandObserver ) = 0;

    /**
     * Set list box observer for container
     *
     * @since 3.2
     */
    virtual void SetListBoxObserver( MEikListBoxObserver* aObserver ) = 0;

    /**
     * Set layout for all elements and set sizes for icons.
     *
     * @since 3.1
     */
    virtual void UpdateLayout() = 0;

    /**
     * Returns list box array, ownership not transfered
     *
     * @since 3.1
     * @return A pointer to the list box array
     */
    virtual CMPXCommonListBoxArrayBase* ListBoxArray() const = 0;

    /**
     * Sets list box array, ownership not transfered
     *
     * @since 3.1
     * @param A pointer to the list box array
     */
    virtual void SetListBoxArrayL(
        CMPXCommonListBoxArrayBase* aListBoxArray) = 0;

    /**
     * Return number of listbox items shown in the current listbox.
     *
     * @since 3.1
     * @return The number of items shown in the list box
     */
    virtual TInt CurrentListItemCount() const = 0;

    /**
     * Return total number of listbox items in the listbox.
     *
     * @since 3.1
     * @return Total items in the list box
     */
    virtual TInt TotalListItemCount() const = 0;

    /**
     * Gets top listbox item index (relative to the original listbox).
     * @return Top listbox item index
     *
     * @since 3.1
     */
    virtual TInt TopLbxItemIndex() const = 0;

    /**
     * Gets bottom listbox item index (relative to the original listbox).
     * @return Bottom listbox item index
     *
     * @since 3.1
     */
    virtual TInt BottomLbxItemIndex() const = 0;

    /**
     * Gets current listbox item index (relative to the original listbox).
     * @return Current listbox item index
     *
     * @since 3.1
     */
    virtual TInt CurrentLbxItemIndex() const = 0;

    /**
     * Get current selected listbox items indices (relative to the original
     * listbox). Ownership not transferred.
     *
     * @since 3.1
     * @return Current listbox item indices
     */
    virtual const CArrayFix<TInt>* CurrentSelectionIndicesL() const = 0;

    /**
     * Sewt current selected listbox items indices
     *
     * @since 3.1
     * @param aIndices Array of selection indices, ownership not transfered
     */
    virtual void SetCurrentSelectionIndicesL( CArrayFix<TInt>* aIndices ) const = 0;

    /**
     * Set top listbox item index.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    virtual void SetLbxTopItemIndex( TInt aIndex ) = 0;

    /**
     * Set current listbox item index.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    virtual void SetLbxCurrentItemIndex( TInt aIndex ) = 0;

    /**
     * Set current listbox item index and highlight it.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    virtual void SetLbxCurrentItemIndexAndDraw( TInt aIndex ) = 0;

    /**
     * Clear listbox selection.
     *
     * @since 3.1
     */
    virtual void ClearLbxSelection() = 0;

    /**
     * Sets list box backround text. This text is visible if the list box
     * has no items.
     *
     * @param aText The text for the empty list box background.
     * @since 3.2
     */
    virtual void SetLbxEmptyTextL( const TDesC& aText ) = 0;

    /**
     * Draws list box item index
     *
     * @since 3.2
     */
    virtual void DrawLbxItemL( TInt aIndex ) = 0;

    /**
     * Set Find box's focus.
     *
     * @since 3.1
     * @param aFocus ETrue if set focus on FindBox. Otherwise, EFalse.
     */
    virtual void SetFindBoxFocus( TBool aFocus ) = 0;

    /**
     * Handle listbox item addition.
     *
     * @since 3.1
     */
    virtual void HandleLbxItemAdditionL() = 0;

    /**
     * Handle listbox item addition, preserving the current display index.
     *
     * @since 3.1
     */
    virtual void HandleLbxItemAdditionPreserveIndexL() = 0;

    /**
     * Handle listbox item removal.
     *
     * @since 3.1
     */
    virtual void HandleLbxItemRemovalL() = 0;

    /**
     * Show/hide find box
     *
     * @since 3.1
     * @param aIsVisible ETrue to set findbox visible. Otherwise, EFalse
     */
    virtual void SetFindBoxVisibilityL( TBool aIsVisible ) = 0;

    /**
     * Determine find box visibility
     *
     * @since 3.1
     * @return ETrue if find box is currently visible, EFalse otherwise
     */
    virtual TBool FindBoxVisibility() = 0;

    /**
     * Restore the PopupCBA of find box
     */
    virtual void RestoreFindBoxPopupCBA() = 0;

    /**
     * Calculate the top index of the visible items
     *
     * @since 3.1
     * @param aBottomIndex the bottom index of the visible items
     * @return top index
     */
    virtual TInt CalculateTopIndex( TInt aBottomIndex ) = 0;

    /**
     * Creates the container
     *
     * @since 3.1
     * @param aCollectionType collection type
     */
    virtual void ConstructContainerL( TMPXCommonContainerCollectionType aCollectionType ) = 0;

    /**
     * Enable/disable find box
     *
     * @since 3.1
     * @aEnable ETrue to enable find box, EFalse to disable
     */
    virtual void EnableFindBox( TBool aEnable ) = 0;

    /**
     * Enable/disable marking
     *
     * @since 3.1
     * @aEnable ETrue to enable marking, EFalse to disable
     */
    virtual void EnableMarking( TBool aEnable ) = 0;

    /**
     * Custom handling of commands for markable lists.
     */
    virtual void HandleMarkableListProcessCommandL( TInt aCommand ) = 0;

    /**
     * Custom handling of menu pane for markable lists
     */
    virtual void HandleMarkableListDynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane ) = 0;


    /**
     * Handle listbox array events
     *
     *  @param aEvent list box event
     */
    virtual void HandleListBoxArrayEventL(
        MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent ) = 0;

    /**
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    virtual TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType ) = 0;

    /**
     * Activate the container
     */
    virtual void ActivateContainerL() = 0;
    };

/**
 *  Factory class to create the common container
 *
 *  @lib mpxcommoncontainer.lib
 *  @since S60 v3.2
 */
class CMPXCommonContainerFactory : CBase
    {
public:
    /**
    * Factory function to create the common container
    */
    IMPORT_C static MMPXCommonContainer* NewL();
    };

#endif  // MMPXCOMMONCONTAINER_H

// End of File
