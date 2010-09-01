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
* Description:  This class is responsible for managing all of music collection
*                databases.
*
*/


// INCLUDE FILES
#include <mpxlog.h>
#include "mpxcollectiondbdef.h"
#include "mpxcollectiondbmanager.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXCollectionDbManager* CMPXCollectionDbManager::NewL(
    RFs& aFs)
    {
    MPX_FUNC("CMPXCollectionDbManager::NewL");

    CMPXCollectionDbManager* self = CMPXCollectionDbManager::NewLC(aFs);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXCollectionDbManager* CMPXCollectionDbManager::NewLC(
    RFs& aFs)
    {
    MPX_FUNC("CMPXCollectionDbManager::NewLC");

    CMPXCollectionDbManager* self = new (ELeave) CMPXCollectionDbManager(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXCollectionDbManager::~CMPXCollectionDbManager()
    {
    MPX_FUNC("CMPXCollectionDbManager::~CMPXCollectionDbManager");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXCollectionDbManager::CMPXCollectionDbManager(
    RFs& aFs) :
    CMPXDbManager(aFs)
    {
    MPX_FUNC("CMPXCollectionDbManager::CMPXCollectionDbManager");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXCollectionDbManager::ConstructL()
    {
    MPX_FUNC("CMPXCollectionDbManager::ConstructL");
    CMPXDbManager::ConstructL(TFileName(KMCDbFile));
    }

// End of File
