/*
* Copyright ( c ) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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

#ifndef CMPMPXISOLATEDCOLLECTIONHELPER_H
#define CMPMPXISOLATEDCOLLECTIONHELPER_H

#include <mpxcollectionobserver.h>

class CMPXCollectionOpenUtility;
class CMPXCollectionPath;

/*!
    \class MpMpxIsolatedCollectionHelperObserver
    \brief Observer interface for class CMpMpxIsolatedCollectionHelper

    This is a helper class to open an insolated collection.
*/
class MMpMpxIsolatedCollectionHelperObserver
{
public:

    virtual void HandleIsolatedOpenL( const CMPXMedia& aEntries, 
            TInt aError ) = 0;
};

class CMpMpxIsolatedCollectionHelper : public CBase,
                                      public MMPXCollectionObserver
{
public:

    static CMpMpxIsolatedCollectionHelper* NewL( MMpMpxIsolatedCollectionHelperObserver* aObserver );
    static CMpMpxIsolatedCollectionHelper* NewLC( MMpMpxIsolatedCollectionHelperObserver* aObserver );
    virtual ~CMpMpxIsolatedCollectionHelper();
    void OpenCollectionL( CMPXCollectionPath& aPath );
    
private:
    CMpMpxIsolatedCollectionHelper( MMpMpxIsolatedCollectionHelperObserver* aObserver );
    void ConstructL();
    void HandleOpenL( const CMPXMedia& aEntries,
                     TInt aIndex,TBool aComplete,TInt aError );
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,TInt aError );
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

private:
    MMpMpxIsolatedCollectionHelperObserver* iObserver;
    CMPXCollectionOpenUtility*              iIncrementalOpenUtil; //owned
    TBool                                   iFirstIncrementalOpen;

};

#endif // CMPMPXISOLATEDCOLLECTIONHELPER_H
    
//EOF
