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
* Description: Wrapper for mpx framework utilities - private implementation.
*
*/

#ifndef MPMPXFRAMEWORKWRAPPER_P_H
#define MPMPXFRAMEWORKWRAPPER_P_H

#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelperobserver.h>

class MMPXCollectionUtility;
class MMPXHarvesterUtility;
class MpMpxFrameworkWrapper;
class QStringList;


class MpMpxFrameworkWrapperPrivate : public MMPXCollectionObserver
{
public:

    explicit MpMpxFrameworkWrapperPrivate( MpMpxFrameworkWrapper *wrapper );
    virtual ~MpMpxFrameworkWrapperPrivate();

    void init();
    void scan();
    void cancelScan();

private:

    void HandleOpenL( const CMPXMedia& aEntries,
                     TInt aIndex,TBool aComplete,TInt aError );
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,TInt aError );
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );

    void DoInitL();
    void DoScanL();
    void DoCancelScanL();
    void DoHandleCollectionMessageL( const CMPXMessage& aMsg );
    TBool BroadcastEventFilter( TInt aMsg );

private:

    MpMpxFrameworkWrapper           *q_ptr;

    MMPXCollectionUtility           *iCollectionUtility;
    MMPXHarvesterUtility            *iHarvesterUtility;
    TInt                            iNumItemsAdded;
    TBool                           iScanning;
    TInt                            iPreviousBroadCastMsg;

};

#endif // MPMPXFRAMEWORKWRAPPER_P_H

