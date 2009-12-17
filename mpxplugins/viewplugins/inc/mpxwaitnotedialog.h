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
* Description:  Base class for all wait notes
*
*/


#ifndef C_MPXWAITNOTEDIALOG_H
#define C_MPXWAITNOTEDIALOG_H

#include <AknWaitDialog.h>
#include <AknProgressDialog.h>
#include <mpxcollectionobserver.h>
#include "mpxwaitnoteobserver.h"
#include "mpxwaitnotedefs.h"

// FOWARD DECLARATIONS
class MMPXCollectionUtility;
class CAknQueryDialog;
class CAknNoteDialog;
/*
* Simple structure to hold icon info
*/
class TNoteIconInfo
    {
public:
    TInt bitmapId;
    TInt maskId;
    TFileName bmpfile;
    
    inline TNoteIconInfo( TInt b, TInt m, TFileName f ) 
        {
        bitmapId=b; 
        maskId=m; 
        bmpfile=f;
        }
    };
 
/**
 *  MPXWaitNoteDialog
 *
 *  Base class for all wait dialogs in MPX
 *
 *  @lib mpxwaitnotedialog.lib
 *  @since S60 3.1
 */
class CMPXWaitNoteDialog : public CBase,
                           public MMPXCollectionObserver,
                           public MProgressDialogCallback
    {

public:

    /**
    * Virtual destructor
    */
    virtual ~CMPXWaitNoteDialog();

public: // New Functions
    
    /**
    * Execute the dialog
    */
    virtual void ExecuteLD();

    /**
    * Cancel the wait note
    */
    virtual void CancelNoteL();
    
    /**
    * Check the dialog destruction status
    */
    virtual TBool ReadyToDestroy();
    
protected: // New functions
    
    /**
    * Initialize the note before executing
    * The inherited class should set CBA, text, icons etc here
    */
    virtual void PreNoteDisplayHandleL() = 0;
    
    /**
    * Post note dissmissal handling
    * @arg aButtonId, button ID that the note was cancelled with
    */
    virtual void PostNoteHandleL( TInt aButtonId );
    
    /**
    * Set the CBA for the waitnote
    * @arg aCBAId, button group ID
    */
    void SetCBAL( TInt aCBAId );
    
    /**
    * Set the Text in the wait note
    * @arg aText, text to display
    */
    void SetTextL( const TDesC& aText );
    
    /**
    * Set the icon in the wait note 
    */
    void SetIconL( TNoteIconInfo& aIconInfo );
    
protected: // From Base classes

    /**
    * From MMPXCollectionObserver
    */
    void HandleCollectionMediaL(const CMPXMedia& aMedia, 
                                TInt aError);
    
    /**
    * From MMPXCollectionObserver
    */
    void HandleCollectionMessage(
        CMPXMessage* aMessage, TInt aError );
    
    /**
    * From MMPXCollectionObserver
    */
    void HandleOpenL(const CMPXMedia& aEntries,
                             TInt aIndex,TBool aComplete,TInt aError);
    
    /**
    * From MMPXCollectionObserver
    */                       
    void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist,TInt aError);                                    

    /**
    * Callback method when a dialog is dismissed.
    * @param aButtonId Button ID.
    */
    void DialogDismissedL( TInt aButtonId );
    
    /**
    * Display a confirmation dialog
    * @param aLabel, label for the popup note
    */
    void DisplayConfirmationNoteL( const TDesC& aLabel );
    
    /**
    * Display an information dialog
    * @param aLabel, label for the popup note
    */
    void DisplayInformationNoteL( const TDesC& aLabel );
    
    /**
    * Display a query dialog
    * @param aLabel, label for the query
    * @return button that was selected
    */
    TInt DisplayQueryNoteL( const TDesC& aQuery );
    
    /**
    * Dismiss the current query note, if any
    */
    void DismissQueryNoteL();
    
    /**
    * Display a generic CAknNoteDialog
    * @param aDlgRsc, dialog resource
    * @param aLabel, text 
    */
    void DisplayNoteDialogL( TInt aDlgRsc, const TDesC& aLabel, 
                             CAknNoteDialog::TTone aTone = CAknNoteDialog::ENoTone );
// Cover UI start
//#ifdef __COVER_DISPLAY
    /**
    * Display a generic CAknNoteDialog
    * @param aDlgRsc, dialog resource
    * @param aLabel, text 
    */
    void DisplayNoteDialogL( TInt aDlgRsc, TInt aTextRsc, 
                             TInt aItemsAdded, const TDesC& aLabel, 
                             CAknNoteDialog::TTone aTone = CAknNoteDialog::ENoTone );
//#endif // __COVER_DISPLAY    
// Cover UI end

protected:

    /**
    * Constructor
    */
    CMPXWaitNoteDialog( MMPXWaitNoteObserver* aObs, 
                        TWaitNoteType aType, 
                        TBool aVisibilityDelayOff = ETrue );

    /**
    * Two-phased constructor
    */
    void BaseConstructL();

protected: // data
    MMPXCollectionUtility* iCollection;
    
    MMPXWaitNoteObserver*  iObserver;
    TWaitNoteType          iWaitNoteType;
    TBool                  iVisDelayOff;    // VisibilityDelayOff
    CAknWaitDialog*        iWaitDialog;
    TInt                   iResourceOffset;
    CAknQueryDialog*       iQueryDialog; // Not Owned
    CAknNoteDialog* 	   iNoteDialog;  // Not Owned
    };

#endif // C_MPXWAITNOTEDIALOG_H
