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
* Description:  Implementation of Upnp Playback view
*
*/



// INCLUDE FILES
#include "mpxupnpplaybackview.h"
#include "mpxupnpplaybackviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXUPnPPlaybackView* CMPXUPnPPlaybackView::NewL()
    {
    return CMPXUPnPPlaybackViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXUPnPPlaybackView* CMPXUPnPPlaybackView::NewLC()
    {
    return CMPXUPnPPlaybackViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackView::~CMPXUPnPPlaybackView()
    {
    // Do nothing
    }

//  End of File
