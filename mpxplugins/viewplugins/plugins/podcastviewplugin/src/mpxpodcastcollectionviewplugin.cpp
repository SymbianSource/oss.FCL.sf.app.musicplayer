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
* Description:  Implementation of Podcast Collection view plugin
*
*/



// INCLUDE FILES
#include "mpxpodcastcollectionview.h"
#include "mpxpodcastcollectionviewplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewPlugin::CMPXPodcastCollectionViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewPlugin* CMPXPodcastCollectionViewPlugin::NewL()
    {
    CMPXPodcastCollectionViewPlugin* self = 
        new ( ELeave ) CMPXPodcastCollectionViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewPlugin::~CMPXPodcastCollectionViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CMPXPodcastCollectionViewPlugin::ConstructViewLC()
    {
    return CMPXPodcastCollectionView::NewLC();
    }

//  End of File
