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
* Description:  Wait note view class
*
*/


#ifndef C_CMPXWAITNOTEPLUGIN_H
#define C_CMPXWAITNOTEPLUGIN_H

// Includes
#include <aknnotedialog.h>
#include <mpxviewplugin.h>
#include "mpxwaitnoteobserver.h"

// FOWARD DECLARATIONS
class CMPXWaitNoteDialog;
/**
 *  Wait note plugin
 *
 *  @lib mpxwaitnoteplugin.lib
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CMPXWaitNoteDialogPlugin ) : public CMPXViewPlugin,
                                                public MMPXWaitNoteObserver
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXWaitNoteDialogPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXWaitNoteDialogPlugin();

protected:
    
     /**
     * From CMPXViewPlugin
     * Interface method to create view.
     *
     * @since S60 v3.0
     */
    void CreateViewL();

    /**
     * From CMPXViewPlugin
     * Interface method to destroy view.
     *
     * @since S60 v3.0
     */
    void DestroyView();

    /**
     * From CMPXViewPlugin
     * Interface method to activate view.
     *
     * @since S60 v3.0
     * @param aParam Parameter required to activate the view.
     */
    void ActivateViewL( const TDesC* aParam = NULL );
    
    /**
     * From CMPXViewPlugin
     * Interface method to activate as default view, will not
     * bring to foreground.
     *
     * @since S60 v3.0
     */
    void SetAsDefaultViewL();

    /**
     * From CMPXViewPlugin
     * Interface method to deactivate view.
     *
     * @since S60 v3.0
     */
    void DeactivateView();

protected:
    
    /**
    * From MMPXWaitNoteObserver
    */
    void HandleWaitNoteDismissedL();
    
    /**
     * From MMPXWaitNoteOberserver
     * Get pointer to the note dialog. 
     * 
     * @param aNoteDialog Referrence of Pointer to the note dialog.
     * @param aDlgRsc The resource ID.
     * @param aTone Tone of the note dialog.
     */
    void NoteDialogL( CAknNoteDialog*& aNoteDialog,
            TInt aDlgRsc, CAknNoteDialog::TTone aTone );
private:

    /**
     * C++ default constructor.
     */
    CMPXWaitNoteDialogPlugin();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Destroy the previous activated wait note when it is 
     * ready to destroy
     */
    static TInt DestroyOldWaiteNote( TAny* aPtr );
    
private: // data
	CAknNoteDialog*		  iNoteDialog; // not Owned
    CMPXWaitNoteDialog*   iWaitNote;
    CMPXWaitNoteDialog*   iOldWaitNote; // Previous activated wait note
    CIdle*                iIdle;
    };

#endif // C_CMPXWAITNOTEPLUGIN_H
