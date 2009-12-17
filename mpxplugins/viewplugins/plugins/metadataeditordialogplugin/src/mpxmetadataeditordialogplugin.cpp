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
* Description:  Implementation of Metadata editor dialog plugin
*
*/



// INCLUDE FILES
#include <mpxmetadataeditordialog.rsg>
#include <mpxcustomcommandobserver.h>
#include <mpxviewutilitycommanddef.h>

#include "mpxmetadataeditordialog.h"
#include "mpxmetadataeditordialogplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXMetadataEditorDialogPlugin::CMPXMetadataEditorDialogPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialogPlugin::ConstructL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXMetadataEditorDialogPlugin* CMPXMetadataEditorDialogPlugin::NewL()
    {
    CMPXMetadataEditorDialogPlugin* self = 
        new ( ELeave ) CMPXMetadataEditorDialogPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMetadataEditorDialogPlugin::~CMPXMetadataEditorDialogPlugin()
    {
    // Do nothing
    }
    
// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to activate view.
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialogPlugin::ActivateViewL( const TDesC* aParam )
    {
    if ( !iDialog )
        {
        CreateViewL();
        }
    
    if ( aParam )
        {
        CMPXMetadataEditorDialog* pDialog = 
            static_cast<CMPXMetadataEditorDialog*>( iDialog );
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
CAknDialog* CMPXMetadataEditorDialogPlugin::ConstructDialogL()
    {
    return CMPXMetadataEditorDialog::NewL();
    }
    
// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Resource ID.
// ---------------------------------------------------------------------------
//
TInt CMPXMetadataEditorDialogPlugin::ResourceId()
    {
    return R_MPX_CUI_SONG_DETAILS_DIALOG;
    }

//  End of File
