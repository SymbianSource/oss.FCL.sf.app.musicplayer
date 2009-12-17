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
* Description:  Implementation of Add tracks dialog plugin
*
*/



// INCLUDE FILES
#include <mpxaddtracksdialog.rsg>

#include "mpxaddtracksdialog.h"
#include "mpxaddtracksdialogplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXAddTracksDialogPlugin::CMPXAddTracksDialogPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialogPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXAddTracksDialogPlugin* CMPXAddTracksDialogPlugin::NewL()
    {
    CMPXAddTracksDialogPlugin* self = 
        new ( ELeave ) CMPXAddTracksDialogPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXAddTracksDialogPlugin::~CMPXAddTracksDialogPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Construct Avkon dialog.
// ---------------------------------------------------------------------------
//
CAknDialog* CMPXAddTracksDialogPlugin::ConstructDialogL()
    {
    return CMPXAddTracksDialog::NewL();
    }
    
// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Resource ID.
// ---------------------------------------------------------------------------
//
TInt CMPXAddTracksDialogPlugin::ResourceId()
    {
    return R_MPX_CUI_ADDTRACKS_DIALOG;
    }

//  End of File
