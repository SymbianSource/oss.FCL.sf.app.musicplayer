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
* Description:  Implementation of Collection view plugin
*
*/



// INCLUDE FILES
#include "mpxcollectionview.h"
#include "mpxcollectionviewplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewPlugin::CMPXCollectionViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewPlugin* CMPXCollectionViewPlugin::NewL()
    {
    CMPXCollectionViewPlugin* self = 
        new ( ELeave ) CMPXCollectionViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCollectionViewPlugin::~CMPXCollectionViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CMPXCollectionViewPlugin::ConstructViewLC()
    {
    return CMPXCollectionView::NewLC();
    }

//  End of File
