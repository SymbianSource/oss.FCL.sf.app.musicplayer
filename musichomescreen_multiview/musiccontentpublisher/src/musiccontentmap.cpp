/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages MCP plugins, and content publishing.
*
*/

#include "musiccontentmap.h"

#include <mpxlog.h>

// ======== TRACING MACROS ========

// These extend macros in <mpxlog.h>

#ifdef _DEBUG
#define MPX_DEBUG2_8(cstr, p) RDebug::Printf(cstr, p)
#else
#define MPX_DEBUG2_8(cstr, p)
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
RMusicContentVariant::RMusicContentVariant():iType(ETypeNull),iModified(EFalse){}

// ---------------------------------------------------------------------------
// Reset value to null
// ---------------------------------------------------------------------------
//
void RMusicContentVariant::Reset()
    {
    switch (iType)
        {
        case ETypeNull:
        	  return;
        case ETypeInt:
            break;
        case ETypeBufC:
            delete iValue.iBufC;
            iValue.iBufC = 0;
            break;
        case ETypeMap:
      	    if ( iValue.iMap )
      	        {
      	        iValue.iMap->Close();
                iValue.iMap = 0;
      	        }
      	    break;
        }
    iType = ETypeNull;
    iModified = ETrue;
    };
    
// ---------------------------------------------------------------------------
// Set value
// ---------------------------------------------------------------------------
//
TBool RMusicContentVariant::SetL( TInt aInt)
    {
    if (iType != ETypeInt || iValue.iInt != aInt)
        {
        Reset();
        iValue.iInt = aInt;
        iType = ETypeInt;
        iModified = ETrue;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
    
// ---------------------------------------------------------------------------
// Set value
// ---------------------------------------------------------------------------
//
TBool RMusicContentVariant::SetL( const TDesC& aDesc )
    {
    if (iType != ETypeBufC || iValue.iBufC->Compare(aDesc) )
        {
        Reset();
        iValue.iBufC = aDesc.AllocL();
        iType = ETypeBufC;
        iModified = ETrue;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Set value
// ---------------------------------------------------------------------------
//
TBool RMusicContentVariant::SetL( CLiwMap * aMap )
    {
    if (iType != ETypeMap || ! ( *iValue.iMap == *aMap ) )
        {
        Reset();
        TLiwVariant variant;
        variant.SetL ( TLiwVariant (aMap) ); // increase CLiwMap reference counter
        iValue.iMap = aMap;
        iType = ETypeMap;
        iModified = ETrue;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Return whether value has been modified
// ---------------------------------------------------------------------------
//
TBool RMusicContentVariant::IsModified() const
    {
    return iModified;
    }
    
// ---------------------------------------------------------------------------
// Reset modified flag
// ---------------------------------------------------------------------------
//
void RMusicContentVariant::ResetModified()
    {
    iModified = EFalse;
    }
    
// ---------------------------------------------------------------------------
// Return whether value is null
// ---------------------------------------------------------------------------
//
TBool RMusicContentVariant::IsNull() const
    {
    return iType == ETypeNull;
    }

// ---------------------------------------------------------------------------
// Return variant value
// ---------------------------------------------------------------------------
//
TLiwVariant RMusicContentVariant::LiwVariant() const
    {
    switch (iType)
        {
        case ETypeNull:
            return TLiwVariant();
        case ETypeInt:
            return TLiwVariant( iValue.iInt );
        case ETypeBufC:
            return TLiwVariant( iValue.iBufC );
        case ETypeMap:
            return TLiwVariant( iValue.iMap );
        }
    return TLiwVariant();
    }

// ---------------------------------------------------------------------------
// Hash function for the map
// ---------------------------------------------------------------------------
//
static inline TUint32 HashFunction(const TPtrC8& aKey)
   {
   return DefaultHash::Des8(aKey);
   }

// ---------------------------------------------------------------------------
// Identity relation for the map
// ---------------------------------------------------------------------------
//
static inline TBool IdentityRelation(const TPtrC8& aLeft, const TPtrC8& aRight)
   {
   return DefaultIdentity::Des8(aLeft, aRight);
   }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
RMusicContentMap::RMusicContentMap():
    iMap(
        THashFunction32< TPtrC8 >(&HashFunction), 
        TIdentityRelation< TPtrC8 >(&IdentityRelation) ),
    iModified( EFalse )
    {}

// ---------------------------------------------------------------------------
// Reset a value in the map
// ---------------------------------------------------------------------------
//
void RMusicContentMap::Reset( TPtrC8 aKey )
    {
    MPX_DEBUG2_8("RMusicContentMap::Reset %S", &aKey);
     RMusicContentVariant * value = iMap.Find( aKey );
    if ( value  )
        {
        iModified |= !value->IsNull();
        value->Reset();
        }
    }
    
// ---------------------------------------------------------------------------
// Set a value in the map
// ---------------------------------------------------------------------------
//
void RMusicContentMap::SetL( TPtrC8 aKey, const TDesC& aValue )
    {
    MPX_DEBUG2_8("RMusicContentMap::SetL %S", &aKey);
    iModified |= AtL( aKey ).SetL( aValue );
    }
    
// ---------------------------------------------------------------------------
// Set a value in the map
// ---------------------------------------------------------------------------
//
void RMusicContentMap::SetL( TPtrC8 aKey, const TInt& aValue )
    {
    MPX_DEBUG2_8("RMusicContentMap::SetL %S", &aKey);
    iModified |= AtL( aKey ).SetL( aValue );
    }
    
// ---------------------------------------------------------------------------
// Set a value in the map
// ---------------------------------------------------------------------------
//
void RMusicContentMap::SetL( TPtrC8 aKey, CLiwMap * aMap )
    {
    MPX_DEBUG2_8("RMusicContentMap::SetL %S", &aKey);
    iModified |= AtL( aKey ).SetL( aMap );
    }
    
// ---------------------------------------------------------------------------
// Return whether map has been modified
// ---------------------------------------------------------------------------
//
TBool RMusicContentMap::IsModified() const
    {
    return iModified;
    }
    
// ---------------------------------------------------------------------------
// Get modified values
// ---------------------------------------------------------------------------
//
CLiwMap * RMusicContentMap::GetModifiedLC()
    {
    MPX_FUNC("RMusicContentMap::GetModifiedLC");
    CLiwMap * result = NULL;
    if (iModified)
        {
        result = CLiwDefaultMap::NewLC();
        THashMapIter<TPtrC8, RMusicContentVariant> iter( iMap );
        while( const RMusicContentVariant * ptr = iter.NextValue() )
            {
            if ( ! ptr->IsModified() ) continue;
/*            	
            if ( ptr->IsNull() )
                {
                MPX_DEBUG2_8("RMusicContentMap::GetModifiedL skipping %S (is null)", iter.CurrentKey());
                }
            else
*/
                {
                MPX_DEBUG2_8("RMusicContentMap::GetModifiedL publishing %S", iter.CurrentKey());
                result->InsertL( *iter.CurrentKey(), ptr->LiwVariant() );
                }
            iter.CurrentValue()->ResetModified();
            }
        iModified = EFalse;
        }
    return result;
    }
    
// ---------------------------------------------------------------------------
// Get all values
// ---------------------------------------------------------------------------
//
CLiwMap * RMusicContentMap::GetAllLC()
    {
    MPX_FUNC("RMusicContentMap::GetAllL");
    CLiwMap * result = NULL;
    result = CLiwDefaultMap::NewLC();
    THashMapIter<TPtrC8, RMusicContentVariant> iter( iMap );
    while( const RMusicContentVariant * ptr = iter.NextValue() )
        {
/*        if ( ptr->IsNull() )
            {
            MPX_DEBUG2_8("RMusicContentMap::GetAllL skipping %S (is null)", iter.CurrentKey());
            }
        else
*/            {
            MPX_DEBUG2_8("RMusicContentMap::GetAllL publishing %S", iter.CurrentKey());
            result->InsertL( *iter.CurrentKey(), ptr->LiwVariant() );
            }
        iter.CurrentValue()->ResetModified();
        }
    iModified = EFalse;
    return result;
    }
    
// ---------------------------------------------------------------------------
// Close map
// ---------------------------------------------------------------------------
//
void RMusicContentMap::Close()
    {
    MPX_FUNC("RMusicContentMap::Close");
    THashMapIter<TPtrC8, RMusicContentVariant> iter( iMap );
    while( iter.NextValue() )
        {
        iter.CurrentValue()->Reset();
        }
    iMap.Close();
    }

RMusicContentVariant& RMusicContentMap::AtL (TPtrC8 aKey)
    {
    RMusicContentVariant * value = iMap.Find( aKey );
    if (! value )
        {
        iMap.InsertL ( aKey, RMusicContentVariant() );
        value = iMap.Find( aKey );
        }
    return *value;
    }
