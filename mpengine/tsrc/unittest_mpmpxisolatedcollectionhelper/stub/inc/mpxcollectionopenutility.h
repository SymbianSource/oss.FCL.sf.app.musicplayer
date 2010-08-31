/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mpxcollectionopenutility stub for testing CMpMpxIsolatedCollectionHelper
*
*/


#ifndef C_CMPXCOLLECTIONOPENUTILITY_H
#define C_CMPXCOLLECTIONOPENUTILITY_H

#include <mpxcollectionobserver.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionopenlresultdef.h>

class CMPXCollectionPath;

class CMPXCollectionOpenUtility  
{
public:
    
    // Stub functions
    enum TDirection
        {
        EFetchNormal,
        EFetchUp,
        EFetchDown
        };
     static CMPXCollectionOpenUtility* NewL( MMPXCollectionObserver* aObs, TUid aMode = KMcModeDefault );
     CMPXCollectionOpenUtility();
     ~CMPXCollectionOpenUtility();
     void StartL( const CMPXCollectionPath& aPath,
                              TArray<TMPXAttribute> aAttrs,
                              TInt aChunkSize,
                              TInt aOffset = 0,
                              TDirection aDirection = EFetchNormal,
                              TMPXAttribute aKeyAttribute = KMPXMediaNullAttribute );
     void Stop();
     void SetDelay( TInt aDelay );
     CMPXCollectionPath* PathL();
public:
     TInt                      iDelay;
     TInt                      iChunkSize;
     TBool                     iStop;
     TBool                     iStart;
     TDirection                iDirection;
     const CMPXCollectionPath *iPath; //not own
     TUid                      iMode;
    };

#endif // C_CMPXCOLLECTIONOPENUTILITY_H
