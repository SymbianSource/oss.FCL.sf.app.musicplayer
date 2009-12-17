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
* Description:  Wait note dialog for scanning
*
*/


#ifndef C_MPXSCANNINGNOTEDIALOG_H
#define C_MPXSCANNINGNOTEDIALOG_H

#include "mpxwaitnotedialog.h"

class MMPXHarvesterUtility;

/**
 *  Wait note class for the scanning cases
 *
 *  @lib mpxwaitnotedialog.dll
 *  @since S60 3.1
 */
class CMPXScanningWaitDialog : public CMPXWaitNoteDialog
                              
    {

public:

    /**
    * Two-phased constructor
    */
    static CMPXScanningWaitDialog* NewL( MMPXWaitNoteObserver* aObs,
                                         TWaitNoteType aType );

    /**
    * Virtual destructor
    */
    virtual ~CMPXScanningWaitDialog();

protected: // CMPXWaitNoteDialog
    
    /**
    * Initialize the note before executing
    * The inherited class should set CBA, text, icons etc here
    */
    void PreNoteDisplayHandleL();

    /**
    * Post note dissmissal handling
    * @arg aButtonId, button ID that the note was cancelled with
    */
    void PostNoteHandleL( TInt aButtonId );
    
    /**
    * From MMPXCollectionObserver
    */
    void HandleCollectionMessage(
        CMPXMessage* aMessage, TInt aError );

private: 

    /**
    * Handle DB Corruption, cleans up both harvester and collection
    */
    void HandleDatabaseCorruptionL();
    
private:

    /**
    * Private constructor
    */
    CMPXScanningWaitDialog( MMPXWaitNoteObserver* aObs,
                            TWaitNoteType aType );

    /**
    * Second phased constructor
    */
    void ConstructL();
    
    /**
     * Handle collection message
     * 
     * @param aMessage collection message
     */
    void DoHandleCollectionMessageL( const CMPXMessage& aMessage );

private: // data
    MMPXHarvesterUtility*  iHarvesterUtil;
    
    TInt                   iNumItemsAdded;
    TInt                   iScanningError;
    TInt                   iInitialMMCCount;
    TInt                   iTotalNewTracks;
    };


#endif // C_MPXSCANNINGNOTEDIALOG_H
