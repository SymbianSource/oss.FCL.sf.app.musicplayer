/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Metadata handler interface
*
*/



// INCLUDE FILES
#include "mpxmetadatahandler.h" 
#include "mpxmetadatahandlerimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C MMPXMetaDataHandler* MMPXMetaDataHandler::NewL(CRemConInterfaceSelector &aInterfaceSelector)
    {
    return CMPXMetaDataHandlerImp::NewL(aInterfaceSelector);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
MMPXMetaDataHandler::~MMPXMetaDataHandler()
    {
    // Do nothing
    }

//  End of File
