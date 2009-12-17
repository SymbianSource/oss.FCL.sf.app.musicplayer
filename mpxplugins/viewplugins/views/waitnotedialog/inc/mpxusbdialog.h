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
* Description:  Wait note during USB transfer
*
*/


#ifndef C_MPX_USB_WAIT_DIALOG_H
#define C_MPX_USB_WAIT_DIALOG_H

#include "mpxwaitnotedialog.h"

/**
 *  MTP wait note dialog
 *
 *  Blocking dialog shown during an MTP transfer
 *
 *  @lib mpxwaitnotedialog.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXUsbWaitDialog ) : public CMPXWaitNoteDialog
    {

public:

    static CMPXUsbWaitDialog* NewL( MMPXWaitNoteObserver* aObs );

    virtual ~CMPXUsbWaitDialog();

protected:

    /**
    * From CMPXWaitNoteDialog
    * Initialize the note before executing
    * The inherited class should set CBA, text, icons etc here
    */
    void PreNoteDisplayHandleL();


private:

    CMPXUsbWaitDialog( MMPXWaitNoteObserver* aObs );

    void ConstructL();

private: // data

    };

#endif // C_MPX_MTP_WAIT_DIALOG_H
