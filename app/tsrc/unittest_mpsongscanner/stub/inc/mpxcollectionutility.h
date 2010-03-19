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
* Description: mmpxcollectionutility stub for testing mpsongscanner
*
*/


#ifndef MMPXCOLLECTIONUTILITY_H
#define MMPXCOLLECTIONUTILITY_H


#include <mpxcommonframeworkdefs.h>     // TMPXOpenMode
#include <mpxattribute.h>               // TMPXAttribute
#include <e32base.h>                    // CBase


class MpSongScannerPrivate;
class CMPXCollectionPath;
class CMPXAttributeSpecs;
class CMPXFilter;


class MMPXCollection : public CBase
    {
    
public:
    
    static void setOpenLLeave();
    static void OpenLLeaveIfDesiredL();
    static void setMediaLLeave();
    static void MediaLLeaveIfDesiredL();
    
public:
    
    MMPXCollection();
    virtual ~MMPXCollection();
    
    void OpenL(TMPXOpenMode aMode = EMPXOpenDefault);
    void MediaL(const CMPXCollectionPath& aPath, const TArray<TMPXAttribute>& aAttrs, CMPXAttributeSpecs* aSpecs=NULL, CMPXFilter* aFilter=NULL);
    
    };

class MMPXCollectionUtility : public CBase
    {
    
public:
    
    static int getCount();
    static void setNewLLeave();
    static void NewLLeaveIfDesiredL();
    static void setCollectionIDLLeave();
    static void CollectionIDLLeaveIfDesiredL();

public:

    static MMPXCollectionUtility* NewL(MpSongScannerPrivate* aObs);
    virtual ~MMPXCollectionUtility();

    void Close();
    TUid CollectionIDL(const TArray<TUid>& aUids);
    MMPXCollection& Collection();
    
// test function
public:
    
    TInt SendRefreshStartMessage();
    TInt SendRefreshEndMessage();
    TInt SendDiskInsertedMessage();
    TInt SendItemChangedMessage();
    
    TInt SendDbCreatedMedia();
    TInt SendNotDbCreatedMedia();
    
private:
    
    MMPXCollectionUtility();

// workaround so that iObserver can be set everytime on NewL 
public:
    
    MpSongScannerPrivate*   iObserver;
    
private:
    
    MMPXCollection          iCollection;

    };

#endif      // MMPXCOLLECTIONUTILITY_H

// End of File
