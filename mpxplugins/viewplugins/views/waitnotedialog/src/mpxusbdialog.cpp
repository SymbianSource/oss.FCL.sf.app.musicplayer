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
* Description:  Wait note dialog for USB transfer
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
#include <mpxmessagegeneraldefs.h>
#include <mpxlog.h>
#include "mpxusbdialog.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXUsbWaitDialog::CMPXUsbWaitDialog( MMPXWaitNoteObserver* aObs )
                                     : CMPXWaitNoteDialog( aObs, EMPXUsbEventNote )
    {

    }


// ---------------------------------------------------------------------------
// 2nd Phased constructor
// ---------------------------------------------------------------------------
//
void CMPXUsbWaitDialog::ConstructL()
    {
    BaseConstructL();
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXUsbWaitDialog* CMPXUsbWaitDialog::NewL( MMPXWaitNoteObserver* aObs )
    {
    CMPXUsbWaitDialog* self = new(ELeave) CMPXUsbWaitDialog( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXUsbWaitDialog::~CMPXUsbWaitDialog()
    {
    }


// ---------------------------------------------------------------------------
// Setup Wait Notes
// ---------------------------------------------------------------------------
//
void CMPXUsbWaitDialog::PreNoteDisplayHandleL()
    {
    // CBA
    SetCBAL( R_AVKON_SOFTKEYS_EMPTY );

    // Text
    HBufC* text = StringLoader::LoadLC( R_MPX_USB_CONNECTION_IN_PROGRESS );
    SetTextL( *text );
    CleanupStack::PopAndDestroy( text );

    // Icon
    TNoteIconInfo icon( (TInt)EMbmAvkonQgn_note_progress,
                        (TInt)EMbmAvkonQgn_note_progress_mask,
                        TFileName(KAvkonBitmapFile) );
    SetIconL( icon );
    }


// End of File
