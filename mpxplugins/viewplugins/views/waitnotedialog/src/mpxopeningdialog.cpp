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
* Description:  Wait note dialog for Opening
*
*/


#include <e32base.h>
#include <StringLoader.h>
#include <avkon.rsg>
#include <avkon.mbg>
#include <aknconsts.h>
#include <mpxcollectionmessage.h>
#include <mpxwaitnotedialog.rsg>
#include <mpxcollectionframeworkdefs.h>
#include <mpxlog.h>
#include <mpxcollectionutility.h>
#include "mpxopeningdialog.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXOpeningWaitDialog::CMPXOpeningWaitDialog( MMPXWaitNoteObserver* aObs, TBool aVisibilityDelayOff )
                                     : CMPXWaitNoteDialog ( aObs,
                                                           EMPXOpeningNote,
                                                           aVisibilityDelayOff )
    {
	// CMPXWaitNoteDialog constructor should have 3rd parameter (=aVisibilityDelayOff) set to ETrue 
	// when the length of the process is AWLAYS over 1.5 seconds. (e.g. opening all songs)
    }


// ---------------------------------------------------------------------------
// 2nd Phased constructor
// ---------------------------------------------------------------------------
//
void CMPXOpeningWaitDialog::ConstructL()
    {
    BaseConstructL();
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXOpeningWaitDialog* CMPXOpeningWaitDialog::NewL( MMPXWaitNoteObserver* aObs, TBool aVisibilityDelayOff )
    {
    CMPXOpeningWaitDialog* self = new(ELeave) CMPXOpeningWaitDialog( aObs, aVisibilityDelayOff );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXOpeningWaitDialog::~CMPXOpeningWaitDialog()
    {
    }


// ---------------------------------------------------------------------------
// Setup Wait Notes
// ---------------------------------------------------------------------------
//
void CMPXOpeningWaitDialog::PreNoteDisplayHandleL()
    {
    // CBA
    SetCBAL( R_AVKON_SOFTKEYS_EMPTY );

    // Text
    HBufC* text = StringLoader::LoadLC( R_MPX_OPENING_TXT );
    SetTextL( *text );
    CleanupStack::PopAndDestroy( text );

    // Icon
    TNoteIconInfo icon( (TInt)EMbmAvkonQgn_note_progress,
                        (TInt)EMbmAvkonQgn_note_progress_mask,
                        TFileName(KAvkonBitmapFile) );
    SetIconL( icon );

    //HandleDatabaseOpenL();
    }


// End of File
