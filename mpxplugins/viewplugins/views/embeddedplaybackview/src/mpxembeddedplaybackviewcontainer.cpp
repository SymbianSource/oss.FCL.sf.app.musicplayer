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
* Description:  Implementation of Embedded Playback view's container.
*
*/



// INCLUDE FILES

#include <AknDef.h>
#include <mpxconstants.h>

#include "mpxembeddedplaybackview.hlp.hrh"
#include "mpxembeddedplaybackviewcontainer.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxcommonuihelper.h"
#include "mpxlog.h"


// CONSTANTS


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewContainer::CMPXEmbeddedPlaybackViewContainer(
    MEikCommandObserver* aCommandObserver,
    MMPXLayoutSwitchObserver* aLayoutObserver,
    MMPXPlaybackViewLayout* aLayout,
    CMPXCommonUiHelper* aCommonUiHelper )
    {
    iCommandObserver = aCommandObserver;
    iLayoutObserver = aLayoutObserver;
    iMode = EUninitialized;
    iLayout = aLayout;
    iCommonUiHelper = aCommonUiHelper;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewContainer::ConstructL( const TRect& aRect )
    {
    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewContainer::ConstructL() entering" );

    CMPXCommonPlaybackViewContainer::ConstructL( aRect );

    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewContainer::ConstructL() exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewContainer::~CMPXEmbeddedPlaybackViewContainer()
    {
    }


// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewContainer::UpdateLayout()
    {
    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewContainer::UpdateLayout() entering" );

    CMPXCommonPlaybackViewContainer::UpdateLayout();

    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewContainer::UpdateLayout() exiting" );
    }

// ---------------------------------------------------------------------------
// sets display mode
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewContainer::SetMode( TMPXPbvPlaybackMode aMode )
    {
    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewContainer::SetMode() entering" );

    if ( aMode != ENoTracksMode )
        {
        CMPXCommonPlaybackViewContainer::SetMode( aMode );
        }
    else
        {
        // the only chance this will be ENoTracksMode in embedded view
        // is when the file is saving (playback engine state change to not initialized)
        // when that happens, ignore the event so that the view
        // doesn't go blank
        MPX_DEBUG1( "CMPXEmbeddedPlaybackViewContainer::SetMode no tracks mode, ignoring" );
        }

    MPX_DEBUG1( "CMPXEmbeddedPlaybackViewContainer::SetMode() exiting" );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXEmbeddedPlaybackViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXEmbeddedPlaybackViewContainer::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    return CMPXCommonPlaybackViewContainer::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;
    TRAP_IGNORE(
        {
        if ( iCommonUiHelper->IsHostBrowserL() )
            {
            aContext.iContext = KMUS_HLP_EMBED_BROWSER;
            }
        else if ( iCommonUiHelper->IsHostMessagingL() )
            {
            aContext.iContext = KMUS_HLP_EMBED_MESSAGING;
            }
        else
            {
            aContext.iContext = KMUS_HLP_EMBED_GENERAL;
            }
        } );
    }


//  End of File
