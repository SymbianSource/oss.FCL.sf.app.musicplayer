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
* Description:  Listbox array for collection view.
*
*/


#ifndef C_CMPXCOLLECTIONVIEWHGLISTBOXARRAY_H
#define C_CMPXCOLLECTIONVIEWHGLISTBOXARRAY_H

#include "mpxcommonlistboxarraybase.h"

// FORWARD DECLARATIONS
class TAknsItemID;

// CLASS DECLARATION

/**
 *  Listbox array for collection view container.
 *  Creates item text descriptors according to the list model.
 *
 *  @lib mpxcollectionview.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXCollectionViewListBoxArray ): public CMPXCommonListBoxArrayBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 3.1
     * @return Pointer to newly created object.
     */
    static CMPXCollectionViewListBoxArray* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXCollectionViewListBoxArray();

    /**
     * Sets playback status with item id
     *
     * @since 3.1
     * @param aId item id of the song
     * @param aStatus status of the song
     * @return The index of previous playing song
     */
    TInt SetPlaybackStatusByIdL( TMPXItemId aId, TMPXPlaybackState aStatus, TInt aIndex );

    /**
     * Activates reorder mode
     *
     * @since 3.1
     * @param aEnable ETrue to enable reorder mode, EFalse to disable
     */
    void ActivateReorderMode( TBool aEnable );

    /**
     * Set reorder mode
     *
     * @since 3.1
     * @param aEnable ETrue to enable reorder mode, EFalse to disable reorder mode
     * @param aIndex current selected index
     */
    void SetReorderGrabbedMode( TBool aEnable, TInt aIndex = 0 );

    /**
     * Move current reorder index
     *
     * @since 3.1
     * @param aDirection +1 to move down, -1 to move up
     */
    void MoveReorderIndex( TInt aDirection );

    /**
     * Set current reorder index
     *
     * @since 3.1
     * @param aIndex current reorder index
     */
    void SetReorderIndex( TInt aIndex );

    /**
     * Confirm reorder
     *
     * @since 3.1
     */
    void ConfirmReorderL();

    /**
     * Cancel reorder
     *
     * @since 3.1
     */
    void CancelReorder();

    /**
     * Gets original index of currently selected item in reorder mode
     *
     * @since 3.1
     * @return The original index of the current item in reorder mode
     */
    TInt GetOriginalIndex() const;

    /**
     * Gets the index for the song that's currently playing
     *
     * @return The index for the song that's currently playing
     */
    TInt GetPlaybackIndex() const;

    /**
     * Indicates if Unknown entry exists in this view
     *
     * @since 3.1
     * @return ETrue if "unknown" category exists, EFalse otherwise
     */
    TBool IsCategoryUnknownExist() const ;

    /**
     * Checks if the item is a broken link, ETrue if broken
     *
     * @since 3.1
     * @param aIndex index of the item
     * @return ETrue if the item is a broken link, EFalse otherwise
     */
    TBool IsItemBrokenLinkL( TInt aIndex ) const ;

    /**
     * Checks if the item is corrupted, ETrue if corrupted
     *
     * @since 3.1
     * @param aIndex index of the item
     * @return ETrue if the item is corrupted link, EFalse otherwise
     */
    TBool IsItemCorruptedL( TInt aIndex ) const;

    /**
     * Get array of indicator icon indices at the specified index
     */
    RArray<TInt> IndicatorIconIndicesL( TInt aIndex);

public: // Functions from base classes

    /**
     * From MDesCArray, indexes into a descriptor array.
     *
     * @param aIndex The position of the descriptor element within a descriptor array.
     * @return Descriptor element located at position aIndex within a descriptor array
     */
    TPtrC MdcaPoint( TInt aIndex ) const;

    /**
     * From CMPXCommonListBoxArrayBase
     * Creates icon array
     *
     * @return Pointer to the created icon array
     */
    CAknIconArray* CreateIconArrayL();

    /**
     * From CMPXCommonListBoxArrayBase
     * Appends media array to the current array
     *
     * @param aMedia media array to append
     */
    void AppendMediaL( const CMPXMedia& aMedia );

private: // constructors

    /**
     * C++ constructor.
     */
    CMPXCollectionViewListBoxArray();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // new functions

    /**
     * Load an icon and append it to an icon array.
     *
     * @param aArray    pointer to the icon array
     * @param aID       skin id of the icon
     * @param aMbmFile  path to the mbm/mif file containing the icon
     * @param aBitmapId mbm id of the bitmap
     * @param aMaskId   mbm id of the mask
     */
    void AppendIconToArrayL(
        CAknIconArray* aArray,
        const TAknsItemID& aID,
        const TDesC& aMbmFile,
        TInt aBitmapId,
        TInt aMaskId );

    /**
     * Load an icon and append it to an icon array.
     *
     * @param aArray    pointer to the icon array
     * @param aID       skin id of the icon
     * @param aColorId  Item ID of the color table.
     * @param aColorIndex Index in the color table.
     * @param aMbmFile  path to the mbm/mif file containing the icon
     * @param aBitmapId mbm id of the bitmap
     * @param aMaskId   mbm id of the mask
     */
    void AppendColorIconToArrayL(
        CAknIconArray* aArray,
        const TAknsItemID& aID,
        const TAknsItemID& aColorId,
        TInt aColorIndex,
        const TDesC& aMbmFile,
        TInt aBitmapId,
        TInt aMaskId );

    /**
     * Maps list box index to array index in reorder mode
     *
     * @aIndex listbox index
     * @return The mapped index
     */
    TInt GetArrayIndexForListBoxIndex( TInt aIndex ) const;

    /**
     * Get reorder icon
     *
     * @return The index of the reorder icon
     */
    TInt GetReorderIcon() const;

    /**
     * Retrieve indexes into a descriptor array.
     *
     * @param aIndex The position of the descriptor element within a descriptor array.
     * @return Descriptor element located at position aIndex within a descriptor array
     */
    TPtrC GetMdcaPointL( TInt aIndex ) const;

    /**
     * Sets playback status
     *
     * @param aIndex index of the song to set
     * @param aStatus status of the song
     * @return The index of previous playing song
     */
    TInt SetPlaybackStatusByIndex( TInt aIndex, TMPXPlaybackState aStatus );

private: // data
    CMPXMediaArray*         iMediaOwned;

    TBool                   iReorderActivated;
    TBool                   iReorder;
    TInt                    iOriginalIndex;
    TInt                    iMovedIndex;
    TBool                   iMoveNowPlaying;

    TBool                   iIsUnknownExist;
    HBufC*                  iUnknown;
    TInt                    iMMCDrive;
    };

#endif  // C_CMPXCOLLECTIONVIEWHGLISTBOXARRAY_H

// End of File
