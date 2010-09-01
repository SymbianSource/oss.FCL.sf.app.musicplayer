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
#include "mpxpdsbplaybackview.h"
#include "mpxpdsbplaybackviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXPdSbPlaybackView* CMPXPdSbPlaybackView::NewL()
    {
    return CMPXPdSbPlaybackViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXPdSbPlaybackView* CMPXPdSbPlaybackView::NewLC()
    {
    return CMPXPdSbPlaybackViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackView::~CMPXPdSbPlaybackView()
    {
    // Do nothing
    }

//  End of File
