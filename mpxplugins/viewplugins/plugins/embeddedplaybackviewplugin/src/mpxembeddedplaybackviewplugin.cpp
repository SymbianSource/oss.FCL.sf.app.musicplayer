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
* Description:  Implementation of Embedded Playback view plugin
*
*/



// INCLUDE FILES
#include "mpxembeddedplaybackview.h"
#include "mpxembeddedplaybackviewplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewPlugin::CMPXEmbeddedPlaybackViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewPlugin* CMPXEmbeddedPlaybackViewPlugin::NewL()
    {
    CMPXEmbeddedPlaybackViewPlugin* self = new ( ELeave ) CMPXEmbeddedPlaybackViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewPlugin::~CMPXEmbeddedPlaybackViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CMPXEmbeddedPlaybackViewPlugin::ConstructViewLC()
    {
    return CMPXEmbeddedPlaybackView::NewLC();
    }

//  End of File
