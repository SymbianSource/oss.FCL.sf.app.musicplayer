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


#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include "mpxcommoncontainerfilteredlist.h"

// CONSTANTS

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMPXCommonContainerFilteredList* CMPXCommonContainerFilteredList::NewL()
    {
    CMPXCommonContainerFilteredList* self =
        CMPXCommonContainerFilteredList::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMPXCommonContainerFilteredList* CMPXCommonContainerFilteredList::NewLC()
    {
    CMPXCommonContainerFilteredList* self =
        new (ELeave) CMPXCommonContainerFilteredList();
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMPXCommonContainerFilteredList::CMPXCommonContainerFilteredList()
 :  iFiltering( EFalse )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMPXCommonContainerFilteredList::~CMPXCommonContainerFilteredList()
    {
    iFilteredMediaList.Reset();
    delete iFilteringWord;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerFilteredList::ReplaceMediaListL( const CMPXMediaArray& aMediaList )
    {
    iMediaList = const_cast<CMPXMediaArray*>(&aMediaList);

    if ( iFiltering )
        {
        HBufC* tempWord = iFilteringWord;
        CleanupStack::PushL( tempWord );
        iFilteringWord = NULL;

        ReAdjustFilteringL( *tempWord );

        CleanupStack::PopAndDestroy( tempWord );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerFilteredList::SetFilteringWordL( const TDesC& aWord )
    {
    if ( aWord.Length() > 0 )
        {
        if ( HasMediaList() )
            {
            ReAdjustFilteringL( aWord );
            iFiltering = ETrue;
            }
        }
    else
        {
        iFilteredMediaList.Reset();
        delete iFilteringWord;
        iFilteringWord = NULL;
        iFiltering = EFalse;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMPXCommonContainerFilteredList::IsFiltering()
    {
    return iFiltering;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMPXCommonContainerFilteredList::HasMediaList()
    {
    return iMediaList != NULL;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerFilteredList::FilteredCount()
    {
    if ( HasMediaList() )
        {
        if ( iFiltering )
            {
            return iFilteredMediaList.Count();
            }
        else
            {
            return iMediaList->Count();
            }
        }

    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMPXCommonContainerFilteredList::IsFilteredIndexValid( TInt aFilteredIndex )
    {
    if ( HasMediaList() && aFilteredIndex >= 0 )
        {
        if ( iFiltering )
            {
            if ( aFilteredIndex < iFilteredMediaList.Count() )
                {
                return ETrue;
                }
            }
        else
            {
            if ( aFilteredIndex < iMediaList->Count() )
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerFilteredList::FilteredIndexToRealIndex( TInt aFilteredIndex )
    {
    TInt realIndex( KErrNotFound );

    if ( HasMediaList() )
        {
        if ( iFiltering )
            {
            if ( aFilteredIndex < iFilteredMediaList.Count() )
                {
                realIndex = iFilteredMediaList[aFilteredIndex];
                }
            }
        else
            {
            if ( aFilteredIndex < iMediaList->Count() )
                {
                realIndex = aFilteredIndex;
                }
            }
        }

    return realIndex;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerFilteredList::ReAdjustFilteringL( const TDesC& aWord )
    {
    CMPXMedia* media = NULL;

    // If user added more letters to filtering, update from existing set.
    if ( iFilteringWord &&
         iFilteringWord->Length() < aWord.Length() )
        {
        TInt ri( KErrNotFound );

        for ( TInt fi = 0; fi < iFilteredMediaList.Count(); fi++ )
            {
            ri = iFilteredMediaList[fi];
            if ( ri < iMediaList->Count() )
                {
                media = (*iMediaList)[ri];

                if ( media->IsSupported( KMPXMediaGeneralTitle ) )
                    {
                    if ( media->ValueText( KMPXMediaGeneralTitle ).FindF( aWord ) == KErrNotFound )
                        {
                        iFilteredMediaList.Remove( fi );
                        fi--;
                        }
                    }
                }
            }
        }
    // If there is no filtering, or user removed letters, create a new filtering set.
    else
        {
        iFilteredMediaList.Reset();

        for ( TInt ri = 0; ri < iMediaList->Count(); ri++ )
            {
            media = (*iMediaList)[ri];

            if ( media->IsSupported( KMPXMediaGeneralTitle ) )
                {
                if ( media->ValueText( KMPXMediaGeneralTitle ).FindF( aWord ) != KErrNotFound )
                    {
                    iFilteredMediaList.AppendL( ri );
                    }
                }
            }
        }

    delete iFilteringWord;
    iFilteringWord = NULL;
    iFilteringWord = aWord.AllocL();
    }
