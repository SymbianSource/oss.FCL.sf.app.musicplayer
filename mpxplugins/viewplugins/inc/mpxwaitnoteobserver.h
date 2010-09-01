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
* Description:  Wait note observer
*
*/



#ifndef M_MPXWAITNOTEOBSERVER_H
#define M_MPXWAITNOTEOBSERVER_H

#include <aknnotedialog.h>
/**
 *  MMPXWaitNoteObserver
 *
 *  Wait Note Observer
 *
 *  @lib mpxwaitnotedialog.lib
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( MMPXWaitNoteObserver )
    {

public:
    /**
    * Handles that a wait note has been dismissed
    * This function should call view utility mark view as closed
    */
    virtual void HandleWaitNoteDismissedL() = 0;
    /**
     * Get pointer to the note dialog 
     * 
     * @param aNoteDialog Referrence of Pointer to the note dialog.
     * @param aDlgRsc The resource ID.
     * @param aTone Tone of the note dialog.
     */
    virtual void NoteDialogL( CAknNoteDialog*& aNoteDialog, 
    		                   TInt aDlgRsc, 
    		                   CAknNoteDialog::TTone aTone ) = 0;
    };


#endif // M_MPXWAITNOTEOBSERVER_H
