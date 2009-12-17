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
* Description:  Implementation of main view
*
*/



// INCLUDE FILES
#include "mpxmainview.h"
#include "mpxmainviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXMainView* CMPXMainView::NewL()
    {
    return CMPXMainViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXMainView* CMPXMainView::NewLC()
    {
    return CMPXMainViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMainView::~CMPXMainView()
    {
    // Do nothing
    }

//  End of File
