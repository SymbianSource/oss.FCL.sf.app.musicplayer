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
* Description:  Controller for MPX Music player
*
*/


#ifndef C_MPXCONTROLLER_H
#define C_MPXCONTROLLER_H

#include <mpxcollectionobserver.h>
#include <coeaui.h>

// Foward declarations
class MMPXCollectionUtility;
class MMPXViewUtility;
class MMPXPlaybackUtility;
class CAknQueryDialog;

// Enumerations 
//@FIX ME
enum TWaitNoteType
    {
    EMPXScanningNote,
    EMPXRefreshingNote,
    EMPXMTPEventNote,
    EMPXUsbEventNote,
    EMPXOpeningNote,
    EMPXMediaNotAvailableNote,
    EMPXCorruptScanningNote,
    EMPXFormatScanningNote,
    EMPXNoteNotDefined
    };
    
/**
 *  Controller for handling system events and system startup
 *
 *  @lib mpx.exe
 *  @since S60 3.2
 */
class CMPXController : public CBase,
                       public MMPXCollectionObserver,
                       public MCoeViewActivationObserver
    {

public:

    /**
    * Two-Phased constructor
    */
    static CMPXController* NewL( TBool aDisablePodcasting );

    /**
    * virtual destructor
    */
    virtual ~CMPXController();

public: // New Functions
    
    /**
    * Idle callback function
    */
    static TInt IdleCallback( TAny* ptr );
    
    /**
    * Idle processing function
    */
    void HandleIdleEventL();

private: // Inherited function 

// From MMPXCollectionObserver
    
    /**
     * From MMPXCollectionObserver
     * Handle collection message.
     *
     * @param aMessage Collection message
     * @param aErr system error code.
     */
    void HandleCollectionMessage(
        CMPXMessage* aMessage, TInt aError );

    /**
     *  From MMPXCollectionObserver
     *  Handles the collection entries being opened. Typically called
     *  when client has Open()'d a folder
     *
     *  @param aEntries collection entries opened
     *  @param aIndex focused entry
     *  @param aComplete ETrue no more entries. EFalse more entries
     *                   expected
     *  @param aError error code   
     */
    void HandleOpenL(
        const CMPXMedia& aEntries,
        TInt aIndex,
        TBool aComplete,
        TInt aError );
    
    /**
     *  From MMPXCollectionObserver
     *  Handles the collection entries being opened. Typically called
     *  when client has Open()'d an item. Client typically responds by
     *  'playing' the item
     *
     *  @param aPlaylist collection path to item
     *  @param aError error code   
     */
    void HandleOpenL(
        const CMPXCollectionPlaylist& aPlaylist,
        TInt aError );        

    /**
     *  
     *  From MMPXCollectionObserver
     *  Handle media properties
     *  Notes: The client is responsible for delete the object of aProperties
     *
     *  @param aMedia media 
     *  @param aError error code    
     */
    void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
    * From MCoeViewActivationObserver
    * Wait for view activation
    */
    void HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId,
                               const TVwsViewId& aViewIdToBeDeactivated);

private: // New functions

    /**
     * Start either the refresh or scanning note
     */
    void StartWaitNoteL( TWaitNoteType aNoteType );
    
    /**
     * Close the waitnote dialog
     * @param aSkipCheckIfActive - flags if the method should check if waitnote 
     *                             is the current view 
     */
    void CloseWaitNoteL(TBool aSkipCheckIfActive = EFalse);
    
    /**
    * Calls MediaL to retrieve item details
    */ 
    void DoRetrieveDetailsL(TBool aSkipAttribute = EFalse);

    /**
     * Handle collection message
     * 
     * @param aMessage collection message
     */
    void DoHandleCollectionMessageL( const CMPXMessage& aMessage );
    
    /**
     * Handle a broadcast message event 
     * from the collection framework
     * @arg aEvent, event to handle
     */
    void DoHandleBroadcastMsgL( TInt aEvent );
    
    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    void HandleErrorL( TInt aError );
    
    /**
     * Handle a corrupt message sent from the collection server
     */
    void DoHandleCorruptMsgL();
    
private:
    
    /**
    * Constructor
    */
    CMPXController( TBool aDisablePodcasting );

    /**
    * 2nd phase constructor
    */
    void ConstructL();

private: // data

    // Member variables
    MMPXCollectionUtility*      iCollectionUtility;
    MMPXViewUtility*            iViewUtility;
    MMPXPlaybackUtility*        iPlaybackUtility;
    CIdle*                      iIdle;
    
    // States
    TBool                       iDelayedRefreshForUIReady;
    TBool                       iDelayedUsbRefresh;
    TBool                       iRefreshingCollection;
    TBool                       iInitDBNeeded;
    TBool                       iInitDBCorrupted;
    TBool                       iUIReady;
    CMPXCollectionPath*         iCurPath;
    
    // Handling system events
    CAknQueryDialog*            iQueryDialog;  // not owned, only for dismissal
    TInt                        iCurSystemEvent;
    TInt                        iCurPlugin;
    TBool                       iOutOfDisk;
    TBool                       iDiskDismountDuringFormat;
    TBool                       iRestartWaitDialog;
    TBool                       iDisablePodcasting;
    
    // Music and Podcast database ID
    TUid iMusicCollectionId;
    TUid iPodcastCollectionId;
    };

#endif // C_MPXCONTROLLER_H
