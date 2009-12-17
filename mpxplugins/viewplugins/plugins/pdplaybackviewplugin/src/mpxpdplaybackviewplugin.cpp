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
* Description:  Implementation of Progressive Download Playback view plugin
*
*/



// INCLUDE FILES
#include "mpxpdplaybackview.h"
#include "mpxpdplaybackviewplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewPlugin::CMPXPdPlaybackViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewPlugin* CMPXPdPlaybackViewPlugin::NewL()
    {
    CMPXPdPlaybackViewPlugin* self = new ( ELeave ) CMPXPdPlaybackViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewPlugin::~CMPXPdPlaybackViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CMPXPdPlaybackViewPlugin::ConstructViewLC()
    {
    return CMPXPdPlaybackView::NewLC();
    }

//  End of File
