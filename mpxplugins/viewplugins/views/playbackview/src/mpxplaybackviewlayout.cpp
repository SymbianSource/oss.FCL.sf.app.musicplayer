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
* Description:  Layout and graphic factory for Playback View
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
#include <mpxplaybackview.rsg>
#include <centralrepository.h>
#include <mpxinternalcrkeys.h>
#include <mpxlbxextendedfeatures.h>

#include "mpxplaybackviewlayout.h"
#include "mpxlog.h"

using namespace AknLayoutScalable_Apps;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewLayout* CMPXPlaybackViewLayout::NewL()
    {
    CMPXPlaybackViewLayout* self = CMPXPlaybackViewLayout::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewLayout* CMPXPlaybackViewLayout::NewLC()
    {
    CMPXPlaybackViewLayout* self = new ( ELeave ) CMPXPlaybackViewLayout();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewLayout::~CMPXPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewLayout::CMPXPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewLayout::ConstructL()
    {
    MPX_FUNC( "CMPXPlaybackViewLayout::ConstructL" );
    InitCommonLayoutL();
    }

//  End of File
