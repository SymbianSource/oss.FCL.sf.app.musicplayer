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
* Description:  Implementation of Playback view
*
*/



// INCLUDE FILES
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <mpxcommonplaybackview.rsg>
#include <mpxplaybackview.rsg>

#include "mpxplaybackview.hrh"
#include "mpxplaybackviewimp.h"
#include "mpxplaybackviewcontainer.h"
#include "mpxplaybackviewlayout.h"
#include "mpxlog.h"

#include <mpxplaybackutility.h>
#include <akntoolbar.h>
#include <aknbutton.h>
#include "mpxcommonplaybackview.hrh"
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewImp::CMPXPlaybackViewImp()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXPlaybackViewImp::ConstructL" );

    CMPXCommonPlaybackViewImp::ConstructL();

    BaseConstructL( R_MPX_PLAYBACK_VIEW );
    if(AknLayoutUtils::PenEnabled())
    {
    if ( Toolbar() )
        {
        iToolbar = Toolbar();
        iToolbar->SetToolbarObserver( this );
        }
    }

    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewImp* CMPXPlaybackViewImp::NewL()
    {
    CMPXPlaybackViewImp* self = CMPXPlaybackViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewImp* CMPXPlaybackViewImp::NewLC()
    {
    CMPXPlaybackViewImp* self = new ( ELeave ) CMPXPlaybackViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewImp::~CMPXPlaybackViewImp()
    {
    MPX_DEBUG1( "CMPXPlaybackViewImp::~CMPXPlaybackViewImp entering" );

    delete iPlaybackViewLayout;

    MPX_DEBUG1( "CMPXPlaybackViewImp::~CMPXPlaybackViewImp exiting" );
    }


// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    CMPXCommonPlaybackViewImp::DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXPlaybackViewImp::Id() const
    {
    return TUid::Uid( KMPXPlaybackViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewImp::ProcessCommandL( TInt aCommand )
    {
    MPX_DEBUG2("CMPXPlaybackViewImp::ProcessCommandL(%d): entering", aCommand);
    if ( aCommand == EAknCmdExit )
        {
        if ( MenuBar() )
            {
            MenuBar()->StopDisplayingMenuBar();
            }
		HandleCommandL( aCommand );
        }
    CAknView::ProcessCommandL( aCommand );
    MPX_DEBUG1("CMPXPlaybackViewImp::ProcessCommandL(): exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    MPX_FUNC( "CMPXPlaybackViewImp::DoActivateL()" );
MPX_PERF_START(CWM_CMPXPlaybackViewImp_DoActivateL);
    if ( !iContainer )
        {
        if ( !iPlaybackViewLayout )
            {
            iPlaybackViewLayout = CMPXPlaybackViewLayout::NewL();
            }
        iContainer = new ( ELeave ) CMPXPlaybackViewContainer(
            this,
            this,
            iPlaybackViewLayout );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }
    else
        {
        AppUi()->AddToStackL( *this, iContainer );
        }

    CMPXCommonPlaybackViewImp::DoActivateL( aPrevViewId , aCustomMessageId, aCustomMessage );
    UpdateMiddleSoftKeyDisplayL();
    MPX_PERF_CHECKPT("Playback view displayed");
MPX_PERF_END(CWM_CMPXPlaybackViewImp_DoActivateL);
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_DEBUG1( "CMPXPlaybackViewImp::DoHandlePlaybackMessageL");

    TMPXMessageId id( *aMessage.Value<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( *aMessage.Value<TInt>( KMPXMessageGeneralType ) );
        TInt data( *aMessage.Value<TInt>( KMPXMessageGeneralData ) );
        switch ( *aMessage.Value<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EStateChanged:
                UpdateMiddleSoftKeyDisplayL();
                UpdateToolbarL();
                break;

            default:
                break;
            }
        }
    CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL (aMessage);
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Used to change toolbar settings before toolbar becomes visible
// -----------------------------------------------------------------------------
//
void CMPXPlaybackViewImp::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
    {
    if (aResourceId == R_MPX_PBV_TOUCH_TOOLBAR)
        {
        // Put initializations here to take effect before toolbar is shown
        UpdateToolbarL();
    	aToolbar->UpdateBackground();
       }
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Handles toolbar events
// -----------------------------------------------------------------------------
//
void CMPXPlaybackViewImp::OfferToolbarEventL( TInt aCommandId )
    {
    TInt commandId = aCommandId;
    TInt eventModifiers = iToolbar->EventModifiers();
    if ( eventModifiers == CAknToolbar::ELongPress )
    	{
    	if ( aCommandId == EMPXPbvCmdNextListItem )
    		{
    		commandId = EMPXPbvCmdSeekForward;
    		}
    	else if ( aCommandId == EMPXPbvCmdPreviousListItem )
    		{
    		commandId = EMPXPbvCmdSeekBackward;
    		}
    	}
    else if ( eventModifiers == CAknToolbar::ELongPressEnded )
    	{
    	if ( aCommandId == EMPXPbvCmdNextListItem || aCommandId == EMPXPbvCmdPreviousListItem )
    	    {
    	    commandId = EMPXPbvCmdEndSeek;
    	    }
    	}

    CAknView::ProcessCommandL( commandId );
    }

//  End of File
