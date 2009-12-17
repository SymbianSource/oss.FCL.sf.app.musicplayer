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
* Description:  Implementation of Collection view
*
*/



// INCLUDE FILES
#include "mpxcollectionview.h"
#include "mpxcollectionviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXCollectionView* CMPXCollectionView::NewL()
    {
    return CMPXCollectionViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXCollectionView* CMPXCollectionView::NewLC()
    {
    return CMPXCollectionViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCollectionView::~CMPXCollectionView()
    {
    // Do nothing
    }

//  End of File
