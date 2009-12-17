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
* Description:  Application UI class required by AVKON application architecture.
*
*/

#include <fbs.h>
#include <mpxlog.h>
#include "mpxcommoncontainertexturemanager2.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CMPXCommonContainerTextureManager2::CMPXCommonContainerTextureManager2()
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXCommonContainerTextureManager2::~CMPXCommonContainerTextureManager2()
    {
    ClearCache();
    }

// ----------------------------------------------------------------------------
// Creates texture from the specified bitmap
// ----------------------------------------------------------------------------
void CMPXCommonContainerTextureManager2::CreateTextureL(
        TInt aItemIndex, TMPXItemId aItemId, CFbsBitmap* aBitmap )
    {
    MPX_FUNC("CMPXCommonContainerTextureManager2::CreateTextureL()");
    if ( iTextureMap.Find(aItemIndex) )
        {
        CFbsBitmap* const* item  = iTextureMap.Find(aItemIndex);
        delete *item;
        }
    iTextureMap.InsertL( aItemIndex, aBitmap );
    iTextureIndexMap.InsertL( (TInt32)aItemId, aItemIndex );
    }

// ----------------------------------------------------------------------------
// Returns the texture id for the specified item
// ----------------------------------------------------------------------------
TBool CMPXCommonContainerTextureManager2::TextureExist( TMPXItemId aItemId ) const
    {
    MPX_FUNC("CMPXCommonContainerTextureManager2::TextureExist()");

    const TInt *ret = iTextureIndexMap.Find( (TInt32)aItemId );
    if ( ret )
        return ETrue;
    else
        return EFalse;
    }


// ----------------------------------------------------------------------------
// Clears the texture cache array
// ----------------------------------------------------------------------------
void CMPXCommonContainerTextureManager2::ClearCache()
    {
    MPX_FUNC("CMPXCommonContainerTextureManager2::ClearCache");

	THashMapIter<TInt, CFbsBitmap*> iter(iTextureMap);
	CFbsBitmap* const* bitmap2 = iter.CurrentValue();
    if (bitmap2)
        {
        delete bitmap2;
        }
	while ( CFbsBitmap* const* bitmap = iter.NextValue() )
		{
		delete *bitmap;
		iter.RemoveCurrent();
		}
   	iTextureMap.Close();
    iTextureIndexMap.Close();
    }

// ----------------------------------------------------------------------------
// Returns the bitmap
// ----------------------------------------------------------------------------
CFbsBitmap* CMPXCommonContainerTextureManager2::ProvideBitmapL( TInt aIndex )
    {
    MPX_FUNC("CMPXCommonContainerTextureManager2::ProvideBitmapL()");
    
    CFbsBitmap* const* item  = iTextureMap.Find(aIndex);
    
    if ( item )
        {
        return *item;
        }
    else
        {
        return NULL;
        }
    }

//End of file
