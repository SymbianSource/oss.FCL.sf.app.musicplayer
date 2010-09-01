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
* Description:  Implementation of Podcast Playback view's container.
*
*/



// INCLUDE FILES

#include <AknDef.h>
#include <mpxconstants.h>

#include "mpxpodcastplaybackview.hlp.hrh"
#include "mpxpodcastplaybackviewcontainer.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxlog.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewContainer::CMPXPodcastPlaybackViewContainer(
    MEikCommandObserver* aCommandObserver,
    MMPXLayoutSwitchObserver* aLayoutObserver,
    MMPXPlaybackViewLayout* aLayout )
    {
    iCommandObserver = aCommandObserver;
    iLayoutObserver = aLayoutObserver;
    iMode = EUninitialized;
    iLayout = aLayout;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewContainer::ConstructL( const TRect& aRect )
    {
    MPX_DEBUG1( "CMPXPodcastPlaybackViewContainer::ConstructL() entering" );
    
    CMPXCommonPlaybackViewContainer::ConstructL( aRect );

    MPX_DEBUG1( "CMPXPodcastPlaybackViewContainer::ConstructL() exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewContainer::~CMPXPodcastPlaybackViewContainer()
    {
    }

// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewContainer::UpdateLayout()
    {
    MPX_DEBUG1( "CMPXPlaybackViewContainer::UpdateLayout() entering" );
    
    CMPXCommonPlaybackViewContainer::UpdateLayout();

    MPX_DEBUG1( "CMPXPlaybackViewContainer::UpdateLayout() exiting" );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXPodcastPlaybackViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXPlaybackViewContainer::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)", 
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );
        
    return CMPXCommonPlaybackViewContainer::OfferKeyEventL( aKeyEvent, aType );
    }
// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;  
    aContext.iContext = KMUS_HLP_PODCAST_PLAYBACK_VIEW; 
    }

//  End of File
