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
* Description: mpxcollectionutility stub for testing mpmpxframeworkwrapper
*
*/


#ifndef MMPXCOLLECTIONUTILITY_H
#define MMPXCOLLECTIONUTILITY_H

#include <mpxcollectionframeworkdefs.h>
#include <mpxcommonframeworkdefs.h>
#include <mpxcollectionobserver.h>
#include <mpxattribute.h>
#include <mpxsearchcriteria.h>
#include <mpxattributespecs.h>

#include <badesca.h>


//Forward declarations
class CMPXCollectionPath;

class MMPXCollection
{
public:
    
    // Stub functions interface
    virtual void CommandL(CMPXCommand& aCmd) = 0;
   
};

class MMPXCollectionUtility : public MMPXCollection
{
public:

    // Stub functions
    static MMPXCollectionUtility* NewL( MMPXCollectionObserver* aObs = NULL,
                                        const TUid& aModeId = KMcModeDefault);
    MMPXCollectionUtility();
    ~MMPXCollectionUtility();
    MMPXCollection& Collection();
    TUid CollectionIDL(const TArray<TUid>& aUids);
    void Close();
    
    void CommandL(CMPXCommand& aCmd);
    
public:
 
    CMPXCommand *iCmd;
};





#endif      // MMPXCOLLECTIONUTILITY_H

