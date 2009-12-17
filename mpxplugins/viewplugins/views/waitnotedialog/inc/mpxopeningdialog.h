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


#ifndef C_MPXOPENINGNOTEDIALOG_H
#define C_MPXOPENINGNOTEDIALOG_H

#include "mpxwaitnotedialog.h"

/**
 *  Wait note class for the opening cases
 *
 *  Blocking dialog shown during opening
 *
 *  @lib mpxwaitnotedialog.dll
 *  @since S60 3.1
 */
class CMPXOpeningWaitDialog : public CMPXWaitNoteDialog                              
    {

public:

    /**
    * Two-phased constructor
    */
    static CMPXOpeningWaitDialog* NewL( MMPXWaitNoteObserver* aObs, TBool aVisibilityDelayOff = ETrue );

    /**
    * Virtual destructor
    */
    virtual ~CMPXOpeningWaitDialog();

protected:
    
    /**
    * From CMPXWaitNoteDialog
    * Initialize the note before executing
    * The inherited class should set CBA, text, icons etc here
    */
    void PreNoteDisplayHandleL();

private:

    /**
    * Private constructor
    */
    CMPXOpeningWaitDialog( MMPXWaitNoteObserver* aObs, TBool aVisibilityDelayOff );

    /**
    * Second phased constructor
    */
    void ConstructL();

private: // data
    
    };

#endif // C_MPXOPENINGNOTEDIALOG_H
