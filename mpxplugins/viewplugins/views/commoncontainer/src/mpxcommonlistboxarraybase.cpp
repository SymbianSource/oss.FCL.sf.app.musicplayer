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
* Description:  Listbox array for common container
*
*/


// INCLUDE FILES
#include <mpxmedia.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>
#include <mpxlog.h>

#include <mpxcollectionpath.h>  // TMPXItemId
#include <mpxmediageneraldefs.h>

#include "mpxcommonlistboxarrayobserver.h"
#include "mpxcommonlistboxarraybase.h"

// CONSTANTS
const TInt KMPXMaxStringSize = 300;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::CMPXCommonListBoxArrayBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXCommonListBoxArrayBase::CMPXCommonListBoxArrayBase() :
    iPbState( EPbStateNotInitialised ),
    iIndex( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXCommonListBoxArrayBase::~CMPXCommonListBoxArrayBase()
    {
    MPX_DEBUG1( "CMPXCommonListBoxArrayBase::~CMPXCommonListBoxArrayBase START" );
    delete iItem;
    delete iContainerMedia;
    MPX_DEBUG1( "CMPXCommonListBoxArrayBase::~CMPXCommonListBoxArrayBase END" );
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::ConstructListBoxArrayL
// Construct the array
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonListBoxArrayBase::ConstructListBoxArrayL()
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::ConstructListBoxArrayL" );
    delete iItem;
    iItem = NULL;
    iItem = HBufC::NewL( KMPXMaxStringSize );
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::AppendMediaL
// Appends media array to the current array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonListBoxArrayBase::AppendMediaL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::AppendMediaL" );
    delete iContainerMedia;
    iContainerMedia=NULL;
    iContainerMedia=CMPXMedia::NewL(aMedia);
    iMedia=const_cast<CMPXMediaArray*>(iContainerMedia->Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    MPX_DEBUG2( "CMPXCommonListBoxArrayBase::AppendMediaL media count %d", iMedia->Count() );
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::ResetMediaArray
// Resets media array.
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonListBoxArrayBase::ResetMediaArrayL()
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::ResetMediaArray" );
    delete iContainerMedia;
    iContainerMedia = NULL;
    iMedia = NULL;
    iIndex = KErrNotFound;
    iPbState = EPbStateNotInitialised;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::MediaArray
// Gets media array.
// -----------------------------------------------------------------------------
//
EXPORT_C const CMPXMediaArray& CMPXCommonListBoxArrayBase::MediaArray()
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::MediaArray" );
    ASSERT( iMedia );
    return *iMedia;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::MediaL
// Get the media at index.
// -----------------------------------------------------------------------------
//
EXPORT_C const CMPXMedia& CMPXCommonListBoxArrayBase::MediaL( TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::MediaL" );
    ASSERT( iMedia );
    if ( ( iMedia->Count() < aIndex + 1 ) || ( aIndex  < 0 ) )
        {
        MPX_DEBUG3( "CMPXCommonListBoxArrayBase::MediaL KErrArgument aIndex = %d, iMedia->Count() = %d", aIndex, iMedia->Count() );
        User::Leave( KErrArgument );
        }
    return *( iMedia->AtL( aIndex ) );
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::ContainerMedia
// Get the container media.
// -----------------------------------------------------------------------------
//
EXPORT_C const CMPXMedia& CMPXCommonListBoxArrayBase::ContainerMedia()
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::ContainerMedia" );
    return *iContainerMedia;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::SetObserver
// Set observer
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonListBoxArrayBase::SetObserver(
    MMPXCommonListBoxArrayObserver* aObserver)
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::SetObserver" );
    ASSERT( !iObserver );
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::RemoveObserver
// Remove observer
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXCommonListBoxArrayBase::RemoveObserver()
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::RemoveObserver" );
    iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::CreateIconArrayL
// creates icon array.
// -----------------------------------------------------------------------------
//
EXPORT_C CAknIconArray* CMPXCommonListBoxArrayBase::CreateIconArrayL()
    {
    MPX_FUNC( "CMPXCommonListBoxArrayBase::CreateIconArrayL" );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::MdcaCount
// Returns the number of descriptor elements in a descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXCommonListBoxArrayBase::MdcaCount() const
    {
    MPX_DEBUG2( "CMPXCommonListBoxArrayBase::MdcaCount count = %d", iMedia ? iMedia->Count() : 0 );
    return ( iMedia ? iMedia->Count() : 0 );
    }

//  End of File
