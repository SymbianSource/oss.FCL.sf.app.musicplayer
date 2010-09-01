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
#include <mpxpdplaybackview.rsg>
#include <centralrepository.h>
#include <mpxinternalcrkeys.h>
#include <mpxlbxextendedfeatures.h>

#include "mpxpdplaybackviewlayout.h"
#include "mpxlog.h"

using namespace AknLayoutScalable_Apps;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewLayout* CMPXPdPlaybackViewLayout::NewL()
    {
    CMPXPdPlaybackViewLayout* self = CMPXPdPlaybackViewLayout::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewLayout* CMPXPdPlaybackViewLayout::NewLC()
    {
    CMPXPdPlaybackViewLayout* self = new ( ELeave ) CMPXPdPlaybackViewLayout();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewLayout::~CMPXPdPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewLayout::CMPXPdPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewLayout::ConstructL()
    {
    MPX_FUNC( "CMPXPdPlaybackViewLayout::ConstructL" );
    InitCommonLayoutL();
    }

//  End of File
