/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*                for PDL init from stand-alone browser
*
*/



// INCLUDE FILES
#include "mpxpdsbplaybackview.h"
#include "mpxpdsbplaybackviewplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewPlugin::CMPXPdSbPlaybackViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewPlugin* CMPXPdSbPlaybackViewPlugin::NewL()
    {
    CMPXPdSbPlaybackViewPlugin* self = new ( ELeave ) CMPXPdSbPlaybackViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewPlugin::~CMPXPdSbPlaybackViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CMPXPdSbPlaybackViewPlugin::ConstructViewLC()
    {
    return CMPXPdSbPlaybackView::NewLC();
    }

//  End of File
