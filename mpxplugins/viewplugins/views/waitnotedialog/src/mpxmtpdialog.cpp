/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wait note dialog for MTP connection
*
*/


#include <e32base.h>
#include <avkon.rsg>
#include <avkon.mbg>
#include <StringLoader.h>
#include <StringLoader.h>
#include <aknconsts.h>
#include <mpxcollectionmessage.h>
#include <mpxwaitnotedialog.rsg>
#include <mpxcollectionframeworkdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxlog.h>
#include "mpxmtpdialog.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXMtpWaitDialog::CMPXMtpWaitDialog( MMPXWaitNoteObserver* aObs ) :
                                      CMPXWaitNoteDialog( aObs, EMPXMTPEventNote )
    {

    }


// ---------------------------------------------------------------------------
// 2nd Phased constructor
// ---------------------------------------------------------------------------
//
void CMPXMtpWaitDialog::ConstructL()
    {
    BaseConstructL();
    }


// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CMPXMtpWaitDialog* CMPXMtpWaitDialog::NewL( MMPXWaitNoteObserver* aObs )
    {
    CMPXMtpWaitDialog* self = new(ELeave) CMPXMtpWaitDialog( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMtpWaitDialog::~CMPXMtpWaitDialog()
    {
    }

// ---------------------------------------------------------------------------
// Initialize the wait note
// ---------------------------------------------------------------------------
//
void CMPXMtpWaitDialog::PreNoteDisplayHandleL()
    {
    // CBA
    SetCBAL( R_AVKON_SOFTKEYS_EMPTY );

    // Text
    HBufC* text = StringLoader::LoadLC( R_MPX_MTP_TRANSFER_IN_PROGRESS );
    SetTextL( *text );
    CleanupStack::PopAndDestroy( text );

    // Icon
    TNoteIconInfo icon( (TInt)EMbmAvkonQgn_note_progress,
                        (TInt)EMbmAvkonQgn_note_progress_mask,
                        TFileName(KAvkonBitmapFile) );
    SetIconL( icon );
    }

// End of File
