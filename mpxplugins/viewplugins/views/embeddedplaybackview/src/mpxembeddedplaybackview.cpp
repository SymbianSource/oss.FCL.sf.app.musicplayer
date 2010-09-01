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
* Description:  Implementation of EmbeddedPlayback view
*
*/



// INCLUDE FILES
#include "mpxembeddedplaybackview.h"
#include "mpxembeddedplaybackviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXEmbeddedPlaybackView* CMPXEmbeddedPlaybackView::NewL()
    {
    return CMPXEmbeddedPlaybackViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXEmbeddedPlaybackView* CMPXEmbeddedPlaybackView::NewLC()
    {
    return CMPXEmbeddedPlaybackViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackView::~CMPXEmbeddedPlaybackView()
    {
    // Do nothing
    }

//  End of File
