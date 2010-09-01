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
* Description:  MPX Collection view container implementation
*
*/



#ifndef CMPXCOLLECTIONVIEWCONTAINER_H
#define CMPXCOLLECTIONVIEWCONTAINER_H


// INCLUDES
#include <mpxcommonframeworkdefs.h> // TMPXItemId
#include "mpxviewcontainer.h"
#include "mpxcommoncontainer.h"

// FORWARD DECLARATION
class TAknsItemID;
class CAknContextPane;
class CMPXCommonContainerFactory;
class MMPXCommonContainer;

// CLASS DECLARATION

/**
 *  MPX Collection view container.
 *
 *  @lib mpxcollectionview.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXCollectionViewContainer )
    : public CBase
    , public MMPXViewContainer
    , public MMPXCommonListBoxArrayObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 5.1
     * @param aView Command observer.
     * @param aObserver MEikListBoxObserver
     * @param aIsEmbedded ETrue to indicate that view is embedded
     * @return Pointer to newly created object.
     */
    static CMPXCollectionViewContainer* NewL(
        MEikCommandObserver* aView,
        MEikListBoxObserver* aObserver,
        TBool aIsEmbedded );

    /**
     * Destructor.
     */
    virtual ~CMPXCollectionViewContainer();

    /**
     * Sets playback status with item id
     *
     * @param aId item id of the song
     * @param aStatus status of the song
     */
    void SetPlaybackStatusByIdL( TMPXItemId aId, TMPXPlaybackState aStatus, TInt aIndex = 0 );

    /**
     * Get current playback index
     *
     * @return Current playback index, KErrNotFound if nothing is playing
     */
    TInt PlaybackIndex();

    /**
     * Activates reorder mode
     *
     * @param aEnable ETrue to enable reorder mode, EFalse to disable
     */
    void ActivateReorderMode( TBool aEnable );

    /**
     * Set reorder mode
     *
     * @param aEnable ETrue to enable reorder mode, EFalse to disable reorder mode
     * @param aIndex current selected index
     */
    void SetReorderGrabbedMode( TBool aEnable, TInt aIndex = 0 );

    /**
     * Determines if it's currently in reorder mode
     *
     * @return ETrue if in reorder mode, EFalse otherwise
     */
    TBool IsInReorderMode();

    /**
     * Move current reorder index relatively
     *
     * @param aDirection +1 to move down, -1 to move up
     */
    void MoveReorderIndex( TInt aDirection );

    /**
     * Set current reorder index
     *
     * @param aIndex current reorder index
     */
    void SetReorderIndex( TInt aIndex );

    /**
     * Confirm reorder
     */
    void ConfirmReorderL();

    /**
     * Cancel reorder
     */
    void CancelReorder();

    /**
     * Gets original index of currently selected item in reorder mode
     */
    TInt GetOriginalIndex();

    /**
     * Update Titlepane icon to reorder image
     */
    void UpdateReorderTitleIconL();

    /**
     * Update titlepane to use original icon
     */
    void RestoreOriginalTitleIconL();

    /**
     * Content is ready
     */
    void ContentIsReady( TBool aReady );

    /**
     * Return common container
     *
     * @since 3.2
     */
    MMPXCommonContainer* Common();

// from base class MMPXViewContainer

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
     * Handles the retrieval of the view container's help context.
     *
     * @param aContext The control's help context.
     */
    void HandleHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * Returns the indicators for the specified item within the view container
     *
     * @param aIndex specified array index
     * @return Indicator icon indices
     */
    RArray<TInt> IndicatorsL( TInt aIndex );

private:

    /**
     * C++ constructor.
     */
    CMPXCollectionViewContainer(
        MEikCommandObserver* aView,
        MEikListBoxObserver* aObserver,
        TBool aIsEmbedded);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Creates listbox array
     */
    CMPXCommonListBoxArrayBase* CreateListBoxArrayL();

// from base class MMPXCommonListBoxArrayObserver

    /**
     * From MMPXCommonListBoxArrayObserver.
     * Handle listbox array events
     *
     *  @param aEvent list box event
     */
    void HandleListBoxArrayEventL(
        MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent );

private: // data member

    MMPXCommonContainer*            iCommonContainer; // Owned
    MEikCommandObserver*            iView; // not owned
    MEikListBoxObserver*            iListBoxObserver; // not owned
    CAknContextPane*                iContextPane;    // Not owned
    CMPXCollectionViewListBoxArray* iLbxArray; // Not owned
    CEikImage*                      iNewIcon;  // New context icon, owned
    CEikImage*                      iOrigIcon;  // New context icon, not owned
    TBool                           iReorder;
    TBool                           iReady;
    TBool                           iIsEmbedded;
    };

#endif  // CMPXCOLLECTIONVIEWCONTAINER_H

// End of File
