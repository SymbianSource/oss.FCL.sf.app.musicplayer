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
* Description:  Class for providing list filtering.
*
*/


#ifndef CMPXCOMMONCONTAINERFILTEREDLIST_H
#define CMPXCOMMONCONTAINERFILTEREDLIST_H

// INCLUDES

#include <coemain.h>

// FORWARD DECLARATIONS

class CMPXMedia;
class CMPXMediaArray;
class TMPXItemId;

// CLASS DECLARATION

/**
 * Class for providing list filtering.
 *
 */
NONSHARABLE_CLASS( CMPXCommonContainerFilteredList ) :
        public CBase
    {
public: // Constructing and destructing.

    /**
     * Two-phased constructor.
     *
     * @return New object.
     */
    static CMPXCommonContainerFilteredList* NewL();

    /**
     * Two-phased constructor.
     *
     * @return New object.
     */
    static CMPXCommonContainerFilteredList* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXCommonContainerFilteredList();

public: // Data changes.

    /**
     * Replaces media list with new one.
     *
     * @param aMediaList New media list.
     */
    void ReplaceMediaListL( const CMPXMediaArray& aMediaList );

    /**
     * Call when filtering word changes. Causes filtered list to be updated.
     * Calling with zero-length filtering word stops filtering.
     *
     * @param aWord New filtering word.
     */
    void SetFilteringWordL( const TDesC& aWord );

    /**
     * Checks if full or filtered data is currently shown.
     *
     * @return ETrue if filtered data is shown.
     */
    TBool IsFiltering();

public: // Data fetching and status information.

    /**
     * Checks if media list exists.
     *
     * @return ETrue if media list exists.
     */
    TBool HasMediaList();

    /**
     * Returns count of filtered media items.
     *
     * @return Count of filtered items.
     */
    TInt FilteredCount();

    /**
     * Checks that given filtered index is valid.
     *
     * @return ETrue if filtered index is valid.
     */
    TBool IsFilteredIndexValid( TInt aFilteredIndex );

    /**
     * Translates filtered index to real index of media list.
     *
     * @param aFilteredIndex Filtered index.
     * @return Real index.
     */
    TInt FilteredIndexToRealIndex( TInt aFilteredIndex );

private:

    /**
     * Constructor.
     */
    CMPXCommonContainerFilteredList();

    /**
     * Updates internal filtering based on new filtering word.
     *
     * @param aWord New filtering word.
     */
    void ReAdjustFilteringL( const TDesC& aWord );

private:

    /**
     * Array of medias.
     * Own.
     */
    CMPXMediaArray* iMediaList;

    /**
     * Array of real indexes that together are the filtered list.
     * Own.
     */
    RArray<TInt> iFilteredMediaList;

    /**
     * Internal flag for indicating if data is being filtered.
     */
    TBool iFiltering;

    /**
     * Filtering word used for current filtering.
     * Own.
     */
    HBufC* iFilteringWord;
    };

#endif // CMPXCOMMONCONTAINERFILTEREDLIST_H
