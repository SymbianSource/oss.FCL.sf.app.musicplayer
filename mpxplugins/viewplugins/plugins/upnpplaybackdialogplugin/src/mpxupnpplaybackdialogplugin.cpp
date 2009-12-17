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
* Description:  Implementation of upnpplayback dialog plugin
*
*/


// INCLUDE FILES
#include <mpxupnpplaybackdialog.rsg>
#include <mpxcustomcommandobserver.h>
#include <mpxviewutilitycommanddef.h>

#include "mpxupnpplaybackdialog.h"
#include "mpxupnpplaybackdialogplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogPlugin::CMPXUPnPPlaybackDialogPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogPlugin* CMPXUPnPPlaybackDialogPlugin::NewL()
    {
    CMPXUPnPPlaybackDialogPlugin* self = 
        new ( ELeave ) CMPXUPnPPlaybackDialogPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogPlugin::~CMPXUPnPPlaybackDialogPlugin()
    {
    // Do nothing
    }
    
// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to activate view.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogPlugin::ActivateViewL( const TDesC* aParam )
    {
    if ( !iDialog )
        {
        CreateViewL();        
        }
    
    if ( aParam )
        {
        CMPXUPnPPlaybackDialog* pDialog = 
            static_cast<CMPXUPnPPlaybackDialog*>( iDialog );
        pDialog->SetParamL( aParam );
        }
    
    iDialog->ExecuteLD( ResourceId() );
    if ( iCommandObserver )
        {
        TPckgC<TInt> selfPtr( reinterpret_cast<TInt>( this ) );    
        iCommandObserver->HandleCustomCommand( 
            TUid::Uid( KMPXViewUtilityCustomCommand ), 
            EMPXViewUtilCmdViewDeActivated,
            selfPtr );
        }    
    iDialog = NULL;
    }    

// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Construct Avkon dialog.
// ---------------------------------------------------------------------------
//
CAknDialog* CMPXUPnPPlaybackDialogPlugin::ConstructDialogL()
    {
    return CMPXUPnPPlaybackDialog::NewL();
    }
    
// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Resource ID.
// ---------------------------------------------------------------------------
//
TInt CMPXUPnPPlaybackDialogPlugin::ResourceId()
    {
    return R_MPX_CUI_UPNP_PLAYBACK_DIALOG;
    }

//  End of File
