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
* Description:  Layout and graphic factory for Progressive Download Playback View
*
*/



// INCLUDE FILES
#include <AknsUtils.h>
#include <AknUtils.h>
#include <barsread.h>
#include <gulicon.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <data_caging_path_literals.hrh>
#include <mpxpdsbplaybackview.rsg>
#include <centralrepository.h>
#include <mpxinternalcrkeys.h>
#include <mpxlbxextendedfeatures.h>

#include "mpxpdsbplaybackviewlayout.h"
#include "mpxlog.h"

using namespace AknLayoutScalable_Apps;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewLayout* CMPXPdSbPlaybackViewLayout::NewL()
    {
    CMPXPdSbPlaybackViewLayout* self = CMPXPdSbPlaybackViewLayout::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewLayout* CMPXPdSbPlaybackViewLayout::NewLC()
    {
    CMPXPdSbPlaybackViewLayout* self = new ( ELeave ) CMPXPdSbPlaybackViewLayout();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewLayout::~CMPXPdSbPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPdSbPlaybackViewLayout::CMPXPdSbPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPdSbPlaybackViewLayout::ConstructL()
    {
    MPX_FUNC( "CMPXPdSbPlaybackViewLayout::ConstructL" );
    InitCommonLayoutL();
    }

//  End of File
