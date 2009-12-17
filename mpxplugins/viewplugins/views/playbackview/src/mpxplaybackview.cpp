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
* Description:  Implementation of Playback view
*
*/



// INCLUDE FILES
#include "mpxplaybackview.h"
#include "mpxplaybackviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXPlaybackView* CMPXPlaybackView::NewL()
    {
    return CMPXPlaybackViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXPlaybackView* CMPXPlaybackView::NewLC()
    {
    return CMPXPlaybackViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPlaybackView::~CMPXPlaybackView()
    {
    // Do nothing
    }

//  End of File
