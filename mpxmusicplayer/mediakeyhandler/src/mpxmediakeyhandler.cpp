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
* Description:  Implementation of Media key handler interface
*
*/



// INCLUDE FILES
#include <mpxmediakeyhandler.h>
#include "mpxmediakeyhandlerimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C MMPXMediaKeyHandler* MMPXMediaKeyHandler::NewL(
    TMPXMediaKeyPopupFlags aFlags,
    MMPXMediaKeyHandlerObserver* aObserver )
    {
    return CMPXMediaKeyHandlerImp::NewL( aFlags, aObserver );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
MMPXMediaKeyHandler::~MMPXMediaKeyHandler()
    {
    // Do nothing
    }

//  End of File
