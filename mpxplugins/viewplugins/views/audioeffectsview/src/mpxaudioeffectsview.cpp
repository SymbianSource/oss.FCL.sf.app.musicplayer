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
* Description:  Implementation of Audio effects view
*
*/



// INCLUDE FILES
#include "mpxaudioeffectsview.h"
#include "mpxaudioeffectsviewimp.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXAudioEffectsView* CMPXAudioEffectsView::NewL()
    {
    return CMPXAudioEffectsViewImp::NewL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXAudioEffectsView* CMPXAudioEffectsView::NewLC()
    {
    return CMPXAudioEffectsViewImp::NewLC();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXAudioEffectsView::~CMPXAudioEffectsView()
    {
    // Do nothing
    }

//  End of File
