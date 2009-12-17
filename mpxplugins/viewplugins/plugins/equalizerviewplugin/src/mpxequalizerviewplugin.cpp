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
* Description:  Implementation of Equalizer view plugin
*
*/



// INCLUDE FILES
#include <mpxcustomcommandobserver.h>
#include <mpxviewutilitycommanddef.h>
#include "mpxequalizerview.h"
#include "mpxequalizerviewplugin.h"
#include "mpxlog.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXEqualizerViewPlugin::CMPXEqualizerViewPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerViewPlugin::ConstructL()
    {
    MPX_FUNC("CMPXEqualizerViewPlugin::ConstructL");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEqualizerViewPlugin* CMPXEqualizerViewPlugin::NewL()
    {
    MPX_FUNC("CMPXEqualizerViewPlugin::NewL");
    CMPXEqualizerViewPlugin* self = new ( ELeave ) CMPXEqualizerViewPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXEqualizerViewPlugin::~CMPXEqualizerViewPlugin()
    {
    delete iView;
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to create view.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerViewPlugin::CreateViewL()
    {
    MPX_FUNC("CMPXEqualizerViewPlugin::CreateViewL");
    if ( !iView )
        {
        iView = CMPXEqualizerView::NewL();
        }
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to destroy view.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerViewPlugin::DestroyView()
    {
    MPX_FUNC("CMPXEqualizerViewPlugin::DestroyView");
    delete iView;
    iView = NULL;
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to activate view.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerViewPlugin::ActivateViewL(
    const TDesC* /* aParam */ )
    {
    MPX_FUNC("CMPXEqualizerViewPlugin::ActivateViewL");
    if ( !iView )
        {
        CreateViewL();
        }

    iView->LaunchEqualizerL();

    if ( iCommandObserver )
        {
        TPckgC<TInt> selfPtr( reinterpret_cast<TInt>( this ) );    
        iCommandObserver->HandleCustomCommand( 
            TUid::Uid( KMPXViewUtilityCustomCommand ), 
            EMPXViewUtilCmdViewDeActivated,
            selfPtr );
        }    
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to activate as default view, will not bring to foreground
// ---------------------------------------------------------------------------
//
void CMPXEqualizerViewPlugin::SetAsDefaultViewL()
    {
    MPX_FUNC("CMPXEqualizerViewPlugin::SetAsDefaultViewL");
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to deactivate view.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerViewPlugin::DeactivateView()
    {
    MPX_FUNC("CMPXEqualizerViewPlugin::DeactivateView");
    if ( iCommandObserver )
        {
        TPckgC<TInt> selfPtr( reinterpret_cast<TInt>( this ) );    
        iCommandObserver->HandleCustomCommand( 
            TUid::Uid( KMPXViewUtilityCustomCommand ), 
            EMPXViewUtilCmdViewDeActivated,
            selfPtr );
        }    
    }

//  End of File
