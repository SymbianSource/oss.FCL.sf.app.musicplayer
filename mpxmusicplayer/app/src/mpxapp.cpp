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
* Description:  Application class required by AVKON application framework.
*
*/


// INCLUDE FILES
#include <eikstart.h>

#include "mpxapp.h"
#include "mpxdocument.h"
#include <mpxconstants.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Returns application UID
// ---------------------------------------------------------------------------
//
TUid CMPXApp::AppDllUid() const
    {
    return KAppUidMusicPlayerX;
    }
   
// ---------------------------------------------------------------------------
// Creates CMPXDocument object
// ---------------------------------------------------------------------------
//
CApaDocument* CMPXApp::CreateDocumentL()
    {
    return CMPXDocument::NewL( *this );
    }


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CMPXApp;
    }


// ---------------------------------------------------------------------------
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File  
