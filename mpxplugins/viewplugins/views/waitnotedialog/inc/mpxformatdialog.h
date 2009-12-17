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
* Description:  Wait note dialog for formatting
*
*/


#ifndef C_MPXFORMATTINGDIALOG_H
#define C_MPXFORMATTINGDIALOG_H

#include "mpxwaitnotedialog.h"

/**
 *  Wait note class for the formatting cases
 *
 *  Blocking dialog shown during formatting
 *
 *  @lib mpxwaitnotedialog.dll
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CMPXFormattingWaitDialog ) : public CMPXWaitNoteDialog                              
    {

public:

    /**
    * Two-phased constructor
    */
    static CMPXFormattingWaitDialog* NewL( MMPXWaitNoteObserver* aObs );

    /**
    * Virtual destructor
    */
    virtual ~CMPXFormattingWaitDialog();

protected:
    
    /**
    * From CMPXWaitNoteDialog
    * Initialize the note before executing
    * The inherited class should set CBA, text, icons etc here
    */
    void PreNoteDisplayHandleL();

    /**
    * From MMPXCollectionObserver
    */
    void HandleCollectionMessage(
            CMPXMessage* aMessage, TInt aError );
  
private:
    
    /**
    * Handles the collection message
    * @param aMessage message to handle
    * @param aError error code
    */
    void DoHandleCollectionMessageL(
             const CMPXMessage& aMessage, TInt aError );
               
private:

    /**
    * Private constructor
    */
    CMPXFormattingWaitDialog( MMPXWaitNoteObserver* aObs );

    /**
    * Second phased constructor
    */
    void ConstructL();

private: // data
    
    };

#endif // C_MPXFORMATTINGDIALOG_H
