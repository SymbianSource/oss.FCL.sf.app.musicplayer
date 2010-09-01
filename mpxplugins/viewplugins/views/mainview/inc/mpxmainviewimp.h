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
* Description:  MPX main view implementation
*
*/



#ifndef C_CMPXMAINVIEWIMP_H
#define C_CMPXMAINVIEWIMP_H


// INCLUDES
#include <eikspmod.h>
#include <eikclb.h>
#include <mpxplaybackobserver.h>
#include <mpxcollectionobserver.h>
#include "mpxwaitnotedefs.h"

#include "mpxmainview.h"
#include "mpxalbumartutilobserver.h"

// FORWARD DECLARATIONS
class CMPXMainViewContainer;
class MMPXViewUtility;
class MMPXPlaybackUtility;
class MMPXCollectionUtility;
class CMPXCommonUiHelper;
class CMPXAlbumArtUtil;
class CMPXCollectionPath;
class CMPXMediaArray;
class CAknQueryDialog;

// CLASS DECLARATION

/**
 *  MPX main view.
 *
 *  @lib mpxmainview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXMainViewImp ) : public CMPXMainView,
                                       public MMPXCollectionObserver,
                                       public MMPXPlaybackObserver,
                                       public MMPXPlaybackCallback,
                                       public MEikListBoxObserver,
                                       public MMPXAlbumArtUtilObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXMainViewImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXMainViewImp* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXMainViewImp();

public: // New Functions

    /**
    * Idle callback function
    */
    static TInt IdleCallback( TAny* ptr );

    /**
    * Idle processing function
    */
    void HandleIdleEventL();

private:

    /**
     * C++ default constructor.
     */
    CMPXMainViewImp();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    void HandleErrorL( TInt aError );

    /**
     * Function to display information notes
     *
     * @param aResourceId Resource ID of the text string.
     */
    void DisplayInfoNoteL( TInt aResourceId );

    /**
     * Function to display confirmation notes
     *
     * @param aResourceId Resource ID of the text string.
     */
    void DisplayConfirmionNoteL( TInt aResourceId );

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

// from base class MMPXPlaybackObserver

    /**
     * From MMPXPlaybackObserver
     * Handle playback message
     *
     * @param aMessage playback message
     * @param aErr system error code.
     */
    void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

// from base class MMPXPlaybackCallback

    /**
     * From MMPXPlaybackCallback
     * Handle playback property
     *
     * @param aProperty the property
     * @param aValue the value of the property
     * @param aError error code
     */
    void HandlePropertyL(
        TMPXPlaybackProperty aProperty,
        TInt aValue,
        TInt aError );

    /**
     * From MMPXPlaybackCallback
     * Method is called continously until aComplete=ETrue, signifying that
     * it is done and there will be no more callbacks
     * Only new items are passed each time
     *
     * @param aPlayer UID of the subplayer
     * @param aSubPlayers a list of sub players
     * @param aComplete ETrue no more sub players. EFalse more subplayer
     *                  expected
     * @param aError error code
     */
    void HandleSubPlayerNamesL(
        TUid aPlayer,
        const MDesCArray* aSubPlayers,
        TBool aComplete,
        TInt aError );

    /**
     * From MMPXPlaybackCallback
     * Handle media properties
     * Notes: The client is responsible for delete the object of aMedia
     *
     * @since 3.0
     * @param aMedia media
     * @param aError error code
     */
    void HandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

// from base class CAknView

    /**
     * From CAknView
     * Returns views id.
     *
     * @since 3.0
     * @return Views Uid
     */
    TUid Id() const;

    /**
     * From CAknView
     * Command handling function.
     *
     * @since 3.0
     * @param aCommand Command which is handled
     */
    void HandleCommandL( TInt aCommand );

    /**
     * From CAknView
     * Handles a view activation.
     *
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
     */
    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );

    /**
     * From CAknView
     * View deactivation function.
     */
    void DoDeactivate();

    /**
     * From CAknView
     * Foreground event handling function.
     *
     * @param aForeground Indicates the required focus state of the control.
     */
    void HandleForegroundEventL( TBool aForeground );

// from base class MEikMenuObserver

    /**
     * From MEikMenuObserver
     * Dynamically initialises a menu pane.
     *
     * @param aResourceId The ID of the menu pane.
     * @param aMenuPane The menu pane itself.
     */
    void DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane );

// from MEikListBoxObserver

    /**
     * Handles listbox events.
     *
     * @param aListBox Listbox where the event occurred.
     * @param aEventType Event type.
     */
     void HandleListBoxEventL(
        CEikListBox* aListBox,
        TListBoxEvent aEventType );

// from MMPXAlbumArtUtilObserver

    /**
     * From MMPXAlbumArtUtilObserver.
     * Notify that extraction of album art started.
     */
    void ExtractAlbumArtStarted();

    /**
     * From MMPXAlbumArtUtilObserver.
     * Album art is extracted from file and ready to use.
     *
     * @param aBitmap a converted image. Ownership is transferred.
     * @param aErr error code
     */
    void ExtractAlbumArtCompleted(
        CFbsBitmap* aBitmap,
        TInt aErr );

// New function

    /**
     *  Handle playback message
     *
     *  @param aMessage playback message
     */
    void DoHandlePlaybackMessageL(
        const CMPXMessage& aMessage );

    /**
     *  Handle playback property
     *
     *  @param aProperty the property
     *  @param aValue the value of the property
     *  @param aError error code
     */
    void DoHandlePropertyL(
        TMPXPlaybackProperty aProperty,
        TInt aValue,
        TInt aError );

    /**
     * Handle collection message
     *
     * @param aMessage collection message
     */
    void DoHandleCollectionMessageL( const CMPXMessage& aMessage );

    /**
     *  Handle media properties.
     *  Notes: The client is responsible for delete the object of aMedia
     *
     *  @param aMedia media properties
     *  @param aError error code
     */
    void DoHandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     */
    void DoHandleStateChangedL(
        TMPXPlaybackState aState );

    /**
     * Updates list box
     *
     * @param aEntries collection entries opened
     * @param aIndex focused entry
     */
    void UpdateListBoxL(
        const CMPXMedia& aEntries,
        TInt aIndex );

    /**
    * Update the total item count text
    * @param aMedia, media object containing the information
    */
    void UpdateItemCountTotalL( const CMPXMedia& aMedia );

    /**
     * Updates playback view status
     *
     */
    void UpdatePlaybackViewStatusL();

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
     * Handle a broadcast message event
     * from the collection framework
     * @arg aEvent, event to handle
     */
    void DoHandleBroadcastMsgL( TInt aEvent );

    /**
     * Handle a corrupt message sent from the collection server
     */
    void DoHandleCorruptMsgL();

    /**
    * Starts the MediaL() calls to update plugin info
    */
    void UpdatePluginInfo();

    /**
    * Send resynchronize collection command to specific collection(s)
    */
    void ResynchronizeCollectionL();

    /**
    * Calls MediaL to retrieve item details
    */
    void DoRetrieveDetailsL(TBool aSkipAttribute = EFalse);

    /**
     * Album art is extracted from file and ready to use.
     *
     * @param aBitmap a converted image. Ownership is transferred.
     * @param aErr error code
     */
    void DoExtractAlbumArtCompletedL( CFbsBitmap* aBitmap,
                                     TInt aErr );

    /**
    * Sets the default focus
    * Sets a KMPXCommandIdCollectionSelect command with
    * selection index of 0
    */
    void DoSetDefaultFocusL();

private:    // Data

    CMPXMainViewContainer*      iContainer;  // own
    MMPXPlaybackUtility*        iPlaybackUtility;
    MMPXViewUtility*            iViewUtility;
    MMPXCollectionUtility*      iCollectionUtility;
    CMPXAlbumArtUtil*           iMPXUtility;
    CMPXCommonUiHelper*         iCommonUiHelper;
    TInt                        iResourceOffset;       // must be freed
    TBool                       iColInitialized;

    // Incremental update to listbox
    CMPXCollectionPath*         iCurPath;
    CMPXMediaArray*             iMediaArray;
    TInt                        iCurPlugin;
    TBool                       iInitDBNeeded;
    TBool                       iInitDBCorrupted;
    TBool                       iDelayedUsbRefresh;
    TBool                       iOutOfDisk;

    // playback view status
    HBufC*                      iTitle;
    TInt                        iDuration;
    TInt                        iPosition;
    TInt                        iPlaybackState;
    TInt                        iAlbumArtIndex;

    // Handlng system events
    CAknQueryDialog*            iQueryDialog;  // not owned, only for dismissal
    TInt                        iCurSystemEvent;

    TBool                       iShowPlaybackIndicator;
    TBool                       iRefreshingCollection;
    TBool                       iExitOptionHidden;
    TBool                       iDelayPluginInfoUpdate;
    TInt                        iCurPlPlayPosition;
    TInt                        iCurPlTotalCount;
    CIdle*                      iIdle;
    TBool                       iDiskDismountDuringFormat;
    TBool                       iPathCorrupted;
    };

#endif  // C_CMPXMAINVIEWIMP_H

// End of File
