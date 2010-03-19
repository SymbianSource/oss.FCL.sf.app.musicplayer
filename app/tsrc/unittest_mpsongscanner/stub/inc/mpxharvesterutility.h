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
* Description: mpxharvesterutility stub for testing mpsongscanner
*
*/


#ifndef MMPXHARVESTERUTILITY_H
#define MMPXHARVESTERUTILITY_H


#include <e32base.h>                    // CBase


class MMPXHarvesterUtility : public CBase
    {
    
public:
    
    static int getCount();
    static void setCheckForSystemEventsLLeave();
    static void CheckForSystemEventsLLeaveIfDesired();
    static void setScanLLeave();
    static void ScanLLeaveIfDesired();
    static void setCancelScanLLeave();
    static void CancelScanLLeaveIfDesired();
    
public:

    MMPXHarvesterUtility();
    virtual ~MMPXHarvesterUtility();

    void Close();
    void CheckForSystemEventsL();
    void ScanL();
    void CancelScanL();
    
    };


class CMPXHarvesterFactory : public CBase
    {
    
public:
    
    static void setNewLLeave();
    static void NewLLeaveIfDesiredL();
    
public:
    
    static MMPXHarvesterUtility* NewL();
    
    };

#endif      // MMPXHARVESTERUTILITY_H

// End of File
