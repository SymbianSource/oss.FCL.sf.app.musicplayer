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
* Description:  Listbox array for podcast collection view.
*
*/


#ifndef C_CMPXPODCASTCOLLECTIONVIEWLISTBOXARRAY_H
#define C_CMPXPODCASTCOLLECTIONVIEWLISTBOXARRAY_H

#include "mpxcommonlistboxarraybase.h"

// FORWARD DECLARATIONS
class TAknsItemID;

// CLASS DECLARATION

/**
*  Listbox array for collection view container.
*  Creates item text descriptors according to the list model.
*/
NONSHARABLE_CLASS( CMPXPodcastCollectionViewListBoxArray ): public CMPXCommonListBoxArrayBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastCollectionViewListBoxArray* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXPodcastCollectionViewListBoxArray();

    /**
     * Sets playback status with item id
     *
     * @since 3.1
     * @param aId item id of the song
     * @param aStatus status of the song
     * @return The index of episode
     */
    TInt SetPlaybackStatusByIdL( TMPXItemId aId, TMPXPlaybackState aStatus );

     /**
     * Gets the index for the song that's currently playing
     *
     * @return The index for the song that's currently playing
     */
    TInt GetPlaybackIndex() const;

    /**
     * Get array of indicator icon indices at the specified index
     */
    RArray<TInt> IndicatorIconIndicesL( TInt aIndex );

public: // Functions from base classes

    /**
     * From MDesCArray, indexes into a descriptor array.
     * @param aIndex The position of the descriptor element within a descriptor array.
     * @return Descriptor element located at position aIndex within a descriptor array
     */
    TPtrC MdcaPoint( TInt aIndex ) const;

    /**
     * Creates icon array
     *
     */
    CAknIconArray* CreateIconArrayL();

    /**
     * Appends media array to the current array
     *
     * @param aMedia media array to append
     */
    void AppendMediaL( const CMPXMedia& aMedia );

public: // new functions

    /**
     * Checks if the item is a broken link, ETrue if broken
     *
     * @param aIndex index of the item
     */
    TBool IsItemBrokenLinkL( TInt aIndex );

    /**
     * Checks if the item is corrupted, ETrue if corrupted
     *
     * @param aIndex index of the item
     */
    TBool IsItemCorruptedL( TInt aIndex );

private: // constructors

    /**
     * C++ default constructor.
     */
    CMPXPodcastCollectionViewListBoxArray();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Handle MdcaPoint
     * @param aIndex The position of the descriptor element within a descriptor array.
     * @return Descriptor element located at position aIndex within a descriptor array
     */
    TPtrC DoMdcaPointL( TInt aIndex ) const;

private: // new functions

    /**
     * Load an icon and append it to an icon array.
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
     */
    TInt MapListBoxIndexToArrayIndex( TInt aIndex ) const;

    /**
     * Sets playback status
     *
     * @param aIndex index of the song to set
     * @param aStatus status of the song
     * @return The index of episode
     */
    TInt SetPlaybackStatusByIndex( TInt aIndex, TMPXPlaybackState aStatus );

private: // data
    TBool                   iReorder;
    TInt                    iOriginalIndex;
    TInt                    iMovedIndex;
    TInt                    iMMCDrive;
    };

#endif  // C_CMPXPODCASTCOLLECTIONVIEWLISTBOXARRAY_H

// End of File
