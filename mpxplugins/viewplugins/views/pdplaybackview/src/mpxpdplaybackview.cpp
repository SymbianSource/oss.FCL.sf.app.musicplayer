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
* Description:  Implementation of Progressive Download Playback view
*
*/



// INCLUDE FILES
#include "mpxpdplaybackview.h"
#include "mpxpdplaybackviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXPdPlaybackView* CMPXPdPlaybackView::NewL()
    {
    return CMPXPdPlaybackViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXPdPlaybackView* CMPXPdPlaybackView::NewLC()
    {
    return CMPXPdPlaybackViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackView::~CMPXPdPlaybackView()
    {
    // Do nothing
    }

//  End of File
