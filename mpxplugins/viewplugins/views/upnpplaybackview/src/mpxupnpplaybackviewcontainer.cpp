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
* Description:  Implementation of Upnp Playback view's container.
*
*/



// INCLUDE FILES

#include <AknDef.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <mpxconstants.h>
#include <aknnotewrappers.h>  // error note
#include <StringLoader.h>
#include <mpxcommonplaybackview.rsg>
#include "mpxupnpplaybackview.hlp.hrh"
#include "mpxupnpplaybackviewcontainer.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxcommonplaybackview.hrh" // playback command
#include "mpxlog.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackViewContainer::CMPXUPnPPlaybackViewContainer(
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
void CMPXUPnPPlaybackViewContainer::ConstructL( const TRect& aRect )
    {
    MPX_DEBUG1( "CMPXUPnPPlaybackViewContainer::ConstructL() entering" );

    CMPXCommonPlaybackViewContainer::ConstructL( aRect );

    // TEMPORARY
    iRemotePlayerUsed = ETrue;

    // Disables FMTX label
    SetFMTXMode(EFalse);

    MPX_DEBUG1( "CMPXUPnPPlaybackViewContainer::ConstructL() exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackViewContainer::~CMPXUPnPPlaybackViewContainer()
    {
    delete iRemotePlayerIcon;
    }


// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewContainer::UpdateLayout()
    {
    MPX_DEBUG1( "CMPXUPnPPlaybackViewContainer::UpdateLayout() entering" );

    CMPXCommonPlaybackViewContainer::UpdateLayout();

    iRemotePlayerIconRect = iLayout->IndicatorLayout( Rect(), EIconRemotePlayer );

    if ( iRemotePlayerIcon )
        {
        AknIconUtils::SetSize(
            iRemotePlayerIcon->Bitmap(), iRemotePlayerIconRect.Size() );
        }

    MPX_DEBUG1( "CMPXUPnPPlaybackViewContainer::UpdateLayout() exiting" );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXUPnPPlaybackViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXUPnPPlaybackViewContainer::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    if ( ( iLastKeyEvent.iScanCode == aKeyEvent.iScanCode ) &&
         ( iLastKeyType == aType ) &&
         ( iLastKeyType == EEventKeyUp ) )
        {
        return EKeyWasConsumed;
        }
    else
        {
        iLastKeyEvent = aKeyEvent;
        iLastKeyType = aType;

        return CMPXCommonPlaybackViewContainer::OfferKeyEventL( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;
    aContext.iContext = KMUS_HLP_UPNP_PLAYBACK_VIEW;
    }

// ---------------------------------------------------------------------------
// Creates icons.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewContainer::CreateIconsL()
    {
    CMPXCommonPlaybackViewContainer::CreateIconsL();

    delete iRemotePlayerIcon;
    iRemotePlayerIcon = NULL;

    // Remote Player
    iRemotePlayerIcon = iLayout->GetIndicatorIconMaskL( EIconRemotePlayer );
    }

// ---------------------------------------------------------------------------
// Redraw part of the screen.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewContainer::RedrawRect(
    const TRect& aRect,
    CBitmapContext& aGc) const
    {
    CMPXCommonPlaybackViewContainer::RedrawRect( aRect, aGc );

    if ( iRemotePlayerUsed )
        {
        DrawIndicator( aGc, aRect, iRemotePlayerIconRect, iRemotePlayerIcon );
        }
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackViewContainer::HandleButtonCommandL
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackViewContainer::HandleButtonCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXUPnPPlaybackViewContainer::HandleButtonCommandL(%d) entering", aCommand);

    switch (aCommand)
        {
        case EMPXPbvCmdSeekForward:
        case EMPXPbvCmdSeekBackward:
            {
            HBufC* dialogText = StringLoader::LoadLC(R_MPX_MEDIA_KEYS_NOT_SUPPORTED);
            CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
            errNote->ExecuteLD( *dialogText );
            CleanupStack::PopAndDestroy( dialogText );

            // The error note consumed the up key event so it needs
            //to be sent again
            if ( iLastKeyType != EEventKeyUp )
                {
                iLastKeyType = EEventKeyUp;
                CMPXCommonPlaybackViewContainer::OfferKeyEventL( iLastKeyEvent,
                                                                   EEventKeyUp );
                }
            break;
            }
        case EMPXPbvCmdEndSeek:
            {
            // not used/forwarded
            break;
            }
        default:
            {
            CMPXCommonPlaybackViewContainer::HandleButtonCommandL( aCommand );
            break;
            }
        }
    MPX_DEBUG1("CMPXUPnPPlaybackViewContainer::HandleButtonCommandL() exiting");
    }

//  End of File
