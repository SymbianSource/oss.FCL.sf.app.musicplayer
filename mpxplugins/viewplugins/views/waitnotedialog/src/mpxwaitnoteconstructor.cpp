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
* Description:  Constructor class
*
*/



#include <e32base.h>
#include <mpxuser.h>
#include "mpxwaitnoteobserver.h"
#include "mpxwaitnoteconstructor.h"
#include "mpxwaitnotedialog.h"
#include "mpxscanningdialog.h"
#include "mpxwaitnotedefs.h"
#include "mpxusbdialog.h"
#include "mpxmtpdialog.h"
#include "mpxopeningdialog.h"
#include "mpxformatdialog.h"

// ---------------------------------------------------------------------------
// Generic Constructor function
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXWaitNoteDialog* CMPXWaitNoteConstructor::ConstructWaitNoteL(const TDesC& aArg,
                                                           MMPXWaitNoteObserver* aObs )
    {
    // Make sure argument is provided
    //
    if( &aArg == NULL )
        {
        User::Leave(KErrArgument);
        }

    // Deserialize the data
    //
    HBufC8* arg = MPXUser::Alloc8L( aArg );
    TPckg<TWaitNoteType> buf(EMPXNoteNotDefined);
    buf.Copy(*arg);
    TWaitNoteType type = buf();
    delete arg;  // no leaving funcs

    // Construct the wait note according to the type
    //
    CMPXWaitNoteDialog* note = (NULL);
    switch( type )
        {
        case EMPXScanningNote:
        case EMPXRefreshingNote:
        case EMPXCorruptScanningNote:
            {
            note = CMPXScanningWaitDialog::NewL( aObs, type );
            break;
            }
        case EMPXMTPEventNote:
            {
            note = CMPXMtpWaitDialog::NewL( aObs );
            break;
            }
        case EMPXUsbEventNote:
            {
            note = CMPXUsbWaitDialog::NewL( aObs );
            break;
            }
        case EMPXOpeningNote:
            {
            note = CMPXOpeningWaitDialog::NewL( aObs, EFalse );
            break;
            }
        case EMPXFormatScanningNote:
            {
            note = CMPXFormattingWaitDialog::NewL( aObs );
            break;
            }
		case EMPXImmediatelyOpeningNote:
			{
			note = CMPXOpeningWaitDialog::NewL( aObs );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    return note;
    }

