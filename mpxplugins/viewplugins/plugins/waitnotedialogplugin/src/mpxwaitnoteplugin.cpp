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
* Description:  Wait Note Plugin
*
*/

#include <eikdialg.h>
#include <aknnotedialog.h>
#include <mpxcustomcommandobserver.h>
#include <mpxviewutilitycommanddef.h>
#include <mpxlog.h>
#include "mpxwaitnotedialog.h"
#include "mpxwaitnoteconstructor.h"
#include "mpxwaitnoteplugin.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default Constructor
// ---------------------------------------------------------------------------
//
CMPXWaitNoteDialogPlugin::CMPXWaitNoteDialogPlugin()
    {
    iWaitNote = NULL;
    iOldWaitNote = NULL;
    iNoteDialog = NULL;
    }

// ---------------------------------------------------------------------------
// Virtual destructor
// ---------------------------------------------------------------------------
//
CMPXWaitNoteDialogPlugin::~CMPXWaitNoteDialogPlugin()
    {
    if (iIdle)
        {
        iIdle->Cancel();
        delete iIdle;
        iIdle = NULL;
        }
    if( iOldWaitNote )
        {
        delete iOldWaitNote;
        iOldWaitNote = NULL;
        }
    if( iWaitNote )
        {
        delete iWaitNote;
        iWaitNote = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Second Phased Constructor
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::ConstructL()
    {
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    }

// ---------------------------------------------------------------------------
// Destroy previous wait note when callback from CIdle
// ---------------------------------------------------------------------------
//
TInt CMPXWaitNoteDialogPlugin::DestroyOldWaiteNote( TAny* aPtr )
    {
    MPX_DEBUG1("CMPXWaitNoteDialogPlugin::DestroyOldWaiteNote()");
    TBool cont = EFalse;

    CMPXWaitNoteDialogPlugin* waitNotePlugin =
            static_cast<CMPXWaitNoteDialogPlugin*>(aPtr);

    if( waitNotePlugin->iOldWaitNote->ReadyToDestroy() )
        {
        TRAP_IGNORE( waitNotePlugin->iOldWaitNote->CancelNoteL() );
        delete waitNotePlugin->iOldWaitNote;
        waitNotePlugin->iOldWaitNote = NULL;
        cont = EFalse;
        }
    else
        {
        cont = ETrue;
        }
    return cont;
    }

// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXWaitNoteDialogPlugin* CMPXWaitNoteDialogPlugin::NewL()
    {
    CMPXWaitNoteDialogPlugin* self = new( ELeave ) CMPXWaitNoteDialogPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
// ---------------------------------------------------------------------------
// Get NoteDialog resource
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::NoteDialogL( CAknNoteDialog*& aNoteDialog,
                             TInt aDlgRsc, CAknNoteDialog::TTone aTone  )
    {
    MPX_DEBUG1("CMPXWaitNoteDialogPlugin::NoteDialogL()");

    if ( !iNoteDialog )
        {
        iNoteDialog = new ( ELeave ) CAknNoteDialog(
                (CEikDialog**)( &iNoteDialog ),
                CAknNoteDialog::ENoTone,
                CAknNoteDialog::ENoTimeout );
        iNoteDialog->PrepareLC( aDlgRsc );
        iNoteDialog->SetTone( aTone );
        iNoteDialog->RunLD();
        }
    aNoteDialog = iNoteDialog;
    }

// ---------------------------------------------------------------------------
// Create the view
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::CreateViewL()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Destroy the view
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::DestroyView()
    {
    MPX_DEBUG1("CMPXWaitNoteDialogPlugin::DestroyView()");
    if( iWaitNote )
        {
        TRAP_IGNORE( iWaitNote->CancelNoteL() );
        }
    }

// ---------------------------------------------------------------------------
// Activate the view
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::ActivateViewL( const TDesC* aParam )
    {
    MPX_DEBUG1("CMPXWaitNoteDialogPlugin::ActivateViewL()");
    // Delete old instance
    if( iWaitNote )
        {
        // delete old will callback and destroy second time, need to use a
        // old variable, and avoid using old address
        MPX_DEBUG1("CMPXWaitNoteDialogPlugin::ActivateViewL()");
        if( iOldWaitNote )
	        {
			TRAP_IGNORE( iOldWaitNote->CancelNoteL() );
			delete iOldWaitNote;
			iOldWaitNote = NULL;
	        }
        iOldWaitNote = iWaitNote;
        iWaitNote = NULL;
        }

    // Start the note
    iWaitNote = CMPXWaitNoteConstructor::ConstructWaitNoteL( *aParam, this );
    iWaitNote->ExecuteLD();
    if( iOldWaitNote )
        {
		iOldWaitNote->CancelNoteL();
        TCallBack callBack( CMPXWaitNoteDialogPlugin::DestroyOldWaiteNote,
                            this );
        if( iIdle->IsActive() )
            {
            iIdle->Cancel();
            }
        iIdle->Start(callBack);
        }
    }

// ---------------------------------------------------------------------------
// From CMPXViewPlugin
// Interface method to activate as default view, will not bring to foreground
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::SetAsDefaultViewL()
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// Deactivate the view
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::DeactivateView()
    {
    // Cancel the note if we deactivate the view.
    if (iWaitNote)  // Fix for ESLU-7CFEPF
        {
        TRAP_IGNORE( iWaitNote->CancelNoteL() );
        }

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
// Handling when the wait note is dismissed
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialogPlugin::HandleWaitNoteDismissedL()
    {
    // Tell view framework that progress note has been dismissed
    //
    MPX_DEBUG1("CMPXWaitNoteDialogPlugin::HandleWaitNoteDismissedL()");
    if ( iCommandObserver )
        {
        TPckgC<TInt> selfPtr( reinterpret_cast<TInt>( this ) );
        iCommandObserver->HandleCustomCommand(
            TUid::Uid( KMPXViewUtilityCustomCommand ),
            EMPXViewUtilCmdViewDeActivated,
            selfPtr );
        }
    }
