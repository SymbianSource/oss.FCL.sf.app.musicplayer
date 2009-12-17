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
* Description:  Implementation of Playback view's container.
*
*/



// INCLUDE FILES

#include <AknDef.h>
#include <mpxconstants.h>

#include "mpxplaybackview.hlp.hrh"
#include "mpxplaybackviewcontainer.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxlog.h"


// CONSTANTS


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewContainer::CMPXPlaybackViewContainer(
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
void CMPXPlaybackViewContainer::ConstructL( const TRect& aRect )
    {
    MPX_DEBUG1( "CMPXPlaybackViewContainer::ConstructL() entering" );
    
    CMPXCommonPlaybackViewContainer::ConstructL( aRect );

    MPX_DEBUG1( "CMPXPlaybackViewContainer::ConstructL() exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewContainer::~CMPXPlaybackViewContainer()
    {
    }


// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewContainer::UpdateLayout()
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
TKeyResponse CMPXPlaybackViewContainer::OfferKeyEventL(
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
void CMPXPlaybackViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;  
    aContext.iContext = KMUS_HLP_PLAYBACK_VIEW; 
    }


//  End of File
