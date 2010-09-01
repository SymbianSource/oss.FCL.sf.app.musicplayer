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
* Description:  Implementation of albumart editor dialog plugin
*
*/


// INCLUDE FILES
#include <mpxalbumarteditordialog.rsg>
#include <mpxcustomcommandobserver.h>
#include <mpxviewutilitycommanddef.h>

#include "mpxalbumarteditordialog.h"
#include "mpxalbumarteditordialogplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogPlugin::CMPXAlbumArtEditorDialogPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogPlugin* CMPXAlbumArtEditorDialogPlugin::NewL()
    {
    CMPXAlbumArtEditorDialogPlugin* self = 
        new ( ELeave ) CMPXAlbumArtEditorDialogPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogPlugin::~CMPXAlbumArtEditorDialogPlugin()
    {
    // Do nothing
    }
    
// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to activate view.
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogPlugin::ActivateViewL( const TDesC* aParam )
    {
    if ( !iDialog )
        {
        CreateViewL();        
        }
    
    if ( aParam )
        {
        CMPXAlbumArtEditorDialog* pDialog = 
            static_cast<CMPXAlbumArtEditorDialog*>( iDialog );
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
CAknDialog* CMPXAlbumArtEditorDialogPlugin::ConstructDialogL()
    {
    return CMPXAlbumArtEditorDialog::NewL();
    }
    
// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Resource ID.
// ---------------------------------------------------------------------------
//
TInt CMPXAlbumArtEditorDialogPlugin::ResourceId()
    {
    return R_MPX_CUI_ALBUM_ART_DIALOG;
    }

//  End of File
