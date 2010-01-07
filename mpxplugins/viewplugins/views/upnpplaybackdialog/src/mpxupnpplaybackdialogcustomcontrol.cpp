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
* Description:  Music Player UPnP Playback dialog implementation
*
*/



// INCLUDE FILES
#include <AknUtils.h>
#include <eiklabel.h>
#include <StringLoader.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsUtils.h>
#include <gulicon.h>

#include <mpxupnpplaybackdialog.rsg>

#include "mpxupnpplaybackdialogcustomcontrol.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxlog.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialogCustomControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogCustomControl*
    CMPXUPnPPlaybackDialogCustomControl::NewLC(
    const CCoeControl& aParent,
    const TRect &aRect,
    MEikCommandObserver* aCommandObserver,
    MMPXLayoutSwitchObserver* aLayoutObserver,
    MMPXPlaybackViewLayout* aLayout,
    TMPXPlaybackPlayerType aCurrentPlayerType )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialogCustomControl::NewLC" );
    CMPXUPnPPlaybackDialogCustomControl* self =
        new ( ELeave ) CMPXUPnPPlaybackDialogCustomControl( aCommandObserver,
                                                            aLayoutObserver,
                                                            aLayout);
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aRect, aCurrentPlayerType );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialogCustomControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogCustomControl*
    CMPXUPnPPlaybackDialogCustomControl::NewL(
    const CCoeControl& aParent,
    const TRect &aRect,
    MEikCommandObserver* aCommandObserver,
    MMPXLayoutSwitchObserver* aLayoutObserver,
    MMPXPlaybackViewLayout* aLayout,
    TMPXPlaybackPlayerType aCurrentPlayerType)
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialogCustomControl::NewL" );
    CMPXUPnPPlaybackDialogCustomControl* self = NewLC(
        aParent, aRect, aCommandObserver, aLayoutObserver, aLayout, aCurrentPlayerType );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CMPXUPnPPlaybackDialogCustomControl::~CMPXUPnPPlaybackDialogCustomControl()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialogCustomControl::~CMPXUPnPPlaybackDialogCustomControl" );
    delete iBackground;

    delete iRemotePlayerIcon;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialogCustomControl::CMPXUPnPPlaybackDialogCustomControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogCustomControl::CMPXUPnPPlaybackDialogCustomControl(
    MEikCommandObserver* aCommandObserver,
    MMPXLayoutSwitchObserver* aLayoutObserver,
    MMPXPlaybackViewLayout* aLayout )
    {
    iCommandObserver = aCommandObserver;
    iLayoutObserver = aLayoutObserver;
    iMode = EUninitialized;
    iLayout = aLayout;
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialogCustomControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogCustomControl::ConstructL(
    const CCoeControl& aParent,
    const TRect &aRect,
    TMPXPlaybackPlayerType aCurrentPlayerType )
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialogCustomControl::ConstructL" );

    CMPXCommonPlaybackViewContainer::ConstructL( aRect );

    if ( aCurrentPlayerType != EPbLocal )
        {
        iRemotePlayerUsed = ETrue;
        }

    // Disables FMTX label
    SetFMTXMode(EFalse);

    SetMopParent( const_cast < CCoeControl* >( &aParent ) );


    SetRect( aRect );
    ActivateL();
    MakeVisible( ETrue );
    }

// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogCustomControl::UpdateLayout()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialogCustomControl::UpdateLayout()" );

    CMPXCommonPlaybackViewContainer::UpdateLayout();

    iRemotePlayerIconRect = iLayout->IndicatorLayout( Rect(), EIconRemotePlayer );

    if ( iRemotePlayerIcon )
        {
        AknIconUtils::SetSize(
            iRemotePlayerIcon->Bitmap(), iRemotePlayerIconRect.Size() );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXUPnPPlaybackDialogCustomControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXUPnPPlaybackDialogCustomControl::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    return CMPXCommonPlaybackViewContainer::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// Creates icons.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogCustomControl::CreateIconsL()
    {
    CMPXCommonPlaybackViewContainer::CreateIconsL();

    // Remote Player
    delete iRemotePlayerIcon;
    iRemotePlayerIcon = NULL;
    iRemotePlayerIcon = iLayout->GetIndicatorIconMaskL( EIconRemotePlayer );
    }

// ---------------------------------------------------------------------------
// Redraw part of the screen.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogCustomControl::RedrawRect(
  const TRect& aRect) const
    {

    CMPXCommonPlaybackViewContainer::RedrawRect( aRect );

    if ( iRemotePlayerUsed )
        {
        CWindowGc& gc = SystemGc();
        DrawIndicator( gc, aRect, iRemotePlayerIconRect, iRemotePlayerIcon );
        }
    }

//  End of File
