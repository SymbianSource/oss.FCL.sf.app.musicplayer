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
* Description:  Implementation of UPnP browse dialog plugin
*
*/



// INCLUDE FILES
#include <StringLoader.h>
#include "mpxupnpbrowsedialog.h"
#include <mpxcustomcommandobserver.h>
#include <mpxviewutilitycommanddef.h>
#include <mpxmusicplayerviewplugin.hrh>

#include "mpxupnpbrowsedialogplugin.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXUPnPBrowseDialogPlugin::CMPXUPnPBrowseDialogPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXUPnPBrowseDialogPlugin::ConstructL()
    {
        // Get an instance of view utility
    iPlayerUid = 0;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXUPnPBrowseDialogPlugin* CMPXUPnPBrowseDialogPlugin::NewL()
    {
    CMPXUPnPBrowseDialogPlugin* self = 
        new ( ELeave ) CMPXUPnPBrowseDialogPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXUPnPBrowseDialogPlugin::~CMPXUPnPBrowseDialogPlugin()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Construct Avkon dialog.
// ---------------------------------------------------------------------------
//
CAknDialog* CMPXUPnPBrowseDialogPlugin::ConstructDialogL()
    {
    return CMPXUPnPBrowseDialog::NewL(0);
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to activate view.
// ---------------------------------------------------------------------------
//
void CMPXUPnPBrowseDialogPlugin::ActivateViewL(
    const TDesC* aParam )
    {
    if ( !iDialog )
        {
        CreateViewL();
        }

    // Translate the desc param into a Uid int
    HBufC* buf = HBufC::NewLC(aParam->Length());
    (buf->Des()).Copy(*aParam);

    TLex resDT( *buf );
    resDT.Val( iPlayerUid );
    CleanupStack::PopAndDestroy(buf);
    
    // Launch the dialog
    TInt selectedSubPlayer = iDialog->ExecuteLD(iPlayerUid);
    if ( iCommandObserver )
        {
        TPckgC<TInt> selfPtr( reinterpret_cast<TInt>( this ) );    
        iCommandObserver->HandleCustomCommand( 
            TUid::Uid( KMPXViewUtilityCustomCommand ), 
            EMPXViewUtilCmdViewDeActivated,
            selfPtr );
        }
    delete iDialog;
    iDialog = NULL;

    // if selectedSubPlayer is KErrCancel, dialog was cancelled
    if ( selectedSubPlayer >= 0 )
        {
        HBufC* buf = HBufC::NewLC( 20 ); // magic number
        buf->Des().AppendNum( selectedSubPlayer );
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {
        User::LeaveIfError(selectedSubPlayer);
        }
    }
    
// ---------------------------------------------------------------------------
// From CMPXAknDialogPlugin
// Resource ID.
// ---------------------------------------------------------------------------
//
TInt CMPXUPnPBrowseDialogPlugin::ResourceId()
    {
    return 0;
    }    

//  End of File
