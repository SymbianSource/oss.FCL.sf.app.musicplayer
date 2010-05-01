/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: isolated collection helper.
*
*/


#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionopenutility.h>

#include "mpmpxisolatedcollectionhelper.h"
#include "mpxlog.h"



const TInt KIncrementalDelayNone = 0;
const TInt KIncrementalDelayHalfSecond = 1000000;
const TInt KIncrementalFetchBlockSize = 20;
const TInt KIncrementalNullOffset = 0;


/*!
    \class CMpMpxIsolatedCollectionHelper
    \brief Helper class to open an isolated collection.

    This is a helper class to open an insolated collection.
*/

/*!
 \internal
 Two-phased constructor.
 */
CMpMpxIsolatedCollectionHelper* CMpMpxIsolatedCollectionHelper::NewL( 
        MMpMpxIsolatedCollectionHelperObserver* aObserver )
    {
    CMpMpxIsolatedCollectionHelper* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

/*!
 \internal
 Two-phased constructor.
 */
CMpMpxIsolatedCollectionHelper* CMpMpxIsolatedCollectionHelper::NewLC( 
        MMpMpxIsolatedCollectionHelperObserver* aObserver )
    {
    CMpMpxIsolatedCollectionHelper* self =
            new ( ELeave ) CMpMpxIsolatedCollectionHelper( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*!
 \internal
 Destructor
 */
CMpMpxIsolatedCollectionHelper::~CMpMpxIsolatedCollectionHelper()
    {
    delete iIncrementalOpenUtil;
    }
    

/*!
 \internal
 Opens an isolated collection with the /a path.
 */
void CMpMpxIsolatedCollectionHelper::OpenCollectionL( CMPXCollectionPath& aPath )
    {
    //Using incremental open to open the collection.
    
    // Cancel any reads
    iIncrementalOpenUtil->Stop();

    // Start the read
    iFirstIncrementalOpen = ETrue;
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    TArray<TMPXAttribute> ary = attrs.Array();
    iIncrementalOpenUtil->SetDelay( KIncrementalDelayNone );
    iIncrementalOpenUtil->StartL( aPath, ary, KIncrementalFetchBlockSize,
                                  KIncrementalNullOffset, CMPXCollectionOpenUtility::EFetchNormal );
    iIncrementalOpenUtil->SetDelay( KIncrementalDelayHalfSecond );
    CleanupStack::PopAndDestroy( &attrs );
    }

/*!
 \internal
 c++ Contructor
 */
CMpMpxIsolatedCollectionHelper::CMpMpxIsolatedCollectionHelper( MMpMpxIsolatedCollectionHelperObserver* aObserver )
    : iObserver( aObserver ),
      iIncrementalOpenUtil( 0 ),
      iFirstIncrementalOpen( EFalse )
    {
    }

/*!
 \internal
 Leaving constructor
 */
void CMpMpxIsolatedCollectionHelper::ConstructL()
    {
    iIncrementalOpenUtil = CMPXCollectionOpenUtility::NewL( this, KMcModeIsolated );

    }


/*!
 \internal
 reimp
 */
void CMpMpxIsolatedCollectionHelper::HandleOpenL( 
    const CMPXMedia& aEntries,
    TInt /*aIndex*/,
    TBool /*aComplete*/,
    TInt aError )
    {
    if ( iFirstIncrementalOpen )
        {
        iObserver->HandleIsolatedOpenL( aEntries, aError );
        iFirstIncrementalOpen = EFalse;
        }
    }

/*!
 \internal
  reimp
 */
void CMpMpxIsolatedCollectionHelper::HandleOpenL( 
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    }

/*!
 \internal
  reimp
 */
void CMpMpxIsolatedCollectionHelper::HandleCollectionMessage( 
    CMPXMessage* /*aMsg*/,
    TInt /*aErr*/ )
    {
    }

/*!
 \internal
  reimp
 */
void CMpMpxIsolatedCollectionHelper::HandleCollectionMediaL( 
    const CMPXMedia& /*aMedia*/,
    TInt /*aError*/ )
    {
    }
    
//EOF