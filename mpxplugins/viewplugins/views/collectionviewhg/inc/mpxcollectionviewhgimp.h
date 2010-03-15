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
* Description:  MPX collection view implementation
*
*/


#ifndef C_CMPXCOLLECTIONVIEWHGIMP_H
#define C_CMPXCOLLECTIONVIEWHGIMP_H

// INCLUDES
#include <eikclb.h>
#include <aknwaitdialog.h>
#include <eikprogi.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionobserver.h>
#include <mpxplaybackobserver.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionuihelperobserver.h>
#include <mpxcollectionhelper.h>
#include <mpxcollectionpath.h>  // TMPXItemId
#include <mpxviewactivationobserver.h>
#include "mpxcbahandler.h"

#include "mpxcollectionviewhg.h"
#include "mpxplaybackframeworkdefs.h" // TMPXPlaybackPlayerType
#include "mpxwaitnotedefs.h"
#include "mpxselectedalbumhandler.h"

// FORWARD DECLARATIONS
class CAiwGenericParamList;
class CAiwServiceHandler;
class CAknNaviLabel;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknQueryDialog;
class CEikButtonGroupContainer;
class CMPXCollectionOpenUtility;
class CMPXCollectionPlaylist;
class CMPXCollectionViewHgContainer;
class CMPXCommonUiHelper;
class CMediaRecognizer;
class CMediatorCommandInitiator;
class CSendUi;
class CUpnpCopyCommand;

#ifdef BACKSTEPPING_INCLUDED
class MMPXBackSteppingUtility;
#endif // BACKSTEPPING_INCLUDED

class MMPXCollectionUtility;
class MMPXPlaybackUtility;
class MMPXPlayerManager;
class MMPXViewUtility;
class MProfileEngine;
// CLASS DECLARATION

/**
 *  MPX collection view.
 *
 *  @lib mpxcollectionview.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXCollectionViewHgImp ) : public CMPXCollectionViewHg,
                                             public MMPXCollectionObserver,
                                             public MMPXPlaybackObserver,
                                             public MMPXPlaybackCallback,
                                             public MEikListBoxObserver,
                                             public MProgressDialogCallback,
                                             public MMPXCollectionFindObserver,
                                             public MMPXCollectionHelperObserver,
                                             public MMPXCHelperObserver,
                                             public MMPXViewActivationObserver,
                                             public MCoeViewDeactivationObserver,
                                             public MCoeViewActivationObserver,
                                             public MMpxCbaHandler,
                                             public MMPXSelectedAlbumHandler
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXCollectionViewHgImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXCollectionViewHgImp* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXCollectionViewHgImp();

    /**
     * Launch music shop application.
     *
     * @ since 3.1
     */
    void LaunchMusicShopL();

    /**
     *  Saves selected album.
     *
     *  @param aMedia Album to be stored
     */
     void SaveSelectedAlbum (CMPXMedia &aMedia);

     /**
     *  Restores selected album.
     *
     *  @return media of restored album
     */
     const CMPXMedia* RestoreSelectedAlbum ();


private:

    /**
     * C++ constructor.
     */
    CMPXCollectionViewHgImp();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Delete the selected items
     * @param aCommand the command that triggered the deletion
     *
     */
    void DeleteSelectedItemsL(TInt aCommand);
    
	/**
     * Delete the selected items in TBone view
     * @param aCommand the command that triggered the deletion
     *
     */
    void DeleteSelectedTBoneItemsL( TInt aCommand );

    /**
     * Updates list box
     *
     * @param aEntries collection entries opened
     * @param aIndex focused entry
     * @param aComplete ETrue no more entries. EFalse more entries
     *                   expected
     */
    void UpdateListBoxL(
        const CMPXMedia& aEntries,
        TInt aIndex,
        TBool aComplete);

    /**
     * Displays error notes.
     *
     * @param aError Error code to be handled.
     */
    void HandleError( TInt aError );

    /**
     * Updates the title pane
     */
    void UpdateTitlePaneL();

    /**
     * Update navi pane in reorder mode
     */
    void UpdateReorderNaviPaneL();

    /**
     * Updates playback status/indicator
     * Returns the current selection index of now playing song
     * if a match is found in current context
     *
     * @return Index of the song that is currently playing
     */
    TInt UpdatePlaybackStatusL();

    /**
     * Start either the refresh or scanning note
     */
    void StartWaitNoteL( TWaitNoteType aNoteType );

    /**
     * Start either the delete wait note
     *
     */
    void StartDeleteWaitNoteL();

    /**
     * Updates the progress note text and progress bar
     *
     * @param aProgressText Text to be displayed on the progress note
     */
    void UpdateProcessL( const TDesC& aProgressText );

    /**
    * Close a wait note
    */
    void CloseWaitNoteL();

    /**
     * Activates reorder mode
     */
    void ActivateReorderGrabbedModeL();

    /**
     * Deactivates reorder mode
     *
     * @param aExit ETrue to exit reorder mode, EFalse otherwise
     */
    void DeactivateReorderGrabbedModeL( TBool aExit );

    /**
     * Change the button group
     *
     * @param aResId resource ID
     */
    void SetNewCbaL( TInt aResId );

    /**
     * Display the details dialog
     *
     * @param aDataArray Array containing the data to display
     * @param aDialogResourceId Resource defination for the dialog
     * @param aTitleResourceId Resource defination for the title of the dialog
     */
    void DisplayDetailsDialogL( MDesC16Array& aDataArray,
        TInt aDialogResourceId, TInt aTitleResourceId );

    /**
     * Display collection details
     *
     * @param aMedia media object containing the library details
     */
    void DoShowCollectionDetailsL( const CMPXMedia& aMedia );

    /**
     * Display playlist details
     *
     * @param aMedia media object containing the playlist details
     */
    void DoShowPlaylistDetailsL( const CMPXMedia& aMedia );

    /**
     * Handles Upnp menus from DynInitMenuPaneL()
     *
     * @param aResourceId Resource id of the option menu
     * @param aMenuPane Reference to the menu pane
     */
    void HandleDynInitUpnpL( TInt aResourceId, CEikMenuPane& aMenuPane );

    /**
     * Checks if UPnP AP is defined
     *
     * @return ETrue if visible, EFalse otherwise
     */
    TBool IsUpnpVisible();

    /**
     * Displays the available players in the Play via submenu
     *
     * @param aMenuPane Menu pane to put the sub menu items
     */
    void AddPlayersNamesToMenuL( CEikMenuPane& aMenuPane );

    /**
     * Adds one player type to the Play via submenu
     *
     * @param aMenuPane menu pane to add player name to
     * @param aCommandId ID of the command
     * @param aPlayerManager player manager
     * @param aPlayerType type of the player
     * @param aMenuText optional menu text
     */
     void AddPlayerNameToMenuL( CEikMenuPane& aMenuPane,
                                TInt aCommandId,
                                MMPXPlayerManager& aPlayerManager,
                                TMPXPlaybackPlayerType& aPlayerType,
                                const TDesC& aMenuText = KNullDesC );

    /**
     * Retrieves the current player name and type
     */
    void GetCurrentPlayerDetails();

    /**
     * Selects a new player for audio playback
     *
     * @param aCommand Command Id to identify which player to use
     */
    void SelectNewPlayerL( TInt aCommand );

    /**
     * Copies selected file(s) to remote player
     */
    void CopySelectedItemsToRemoteL();

    /**
     * Handle call back from collectionframework for Copy to Remote command
     *
     * @param aMedia media
     * @param aComplete all selected medias have been found
     */
    void DoHandleCopyToRemoteL(
        const CMPXMedia& aMedia, TBool aComplete = ETrue );


    /**
     * Create and launch the search URL page to the Service
     * @param song name, artistname, album name
     */
    void DoFindInMusicShopL(const TDesC& aSongName,
                            const TDesC& aArtistName,
                            const TDesC& aAlbumName );

    /**
     * Parameter passed in from viewframework
     *
     * @param aParam Parameter pass in from viewframework.
     */
    void SetParamL( const TDesC* aParam );

    /**
     * Get media property for the current song
     */
    void DoGetSongMediaPropertyL();

    /**
     * Find playlists
     *
     * @return the number of playlist in the system
     */
    TInt FindPlaylistsL();

    /**
     * Save the changes to the playlist that is currently displaying
     */
    void SaveCurrentPlaylistL();

    /**
     * Handles rename operation complete
     */
    void HandleRenameOpCompleteL();

    /**
     * Checks if send option should be shown
     *
     * @return ETrue if send command should be hiden, EFalse if it should be shown
     */
    TBool SendOptionVisibilityL();

    /**
     * Checks if Set as ringtone option should be shown
     *
     * @return ETrue if the command should be hiden, EFalse if it should be shown
     */
    TBool SetAsRingToneOptionVisibilityL();

    /**
     * Checks if file details option should be shown
     *
     * @return ETrue if the command should be hiden, EFalse if it should be shown
     */
    TBool FileDetailsOptionVisibilityL();

    /**
     * Handle send command
     */
    void DoSendL();
   
   /**
     * Handle send command in TBone view
     */
    void DoSendTBoneListItemL( TMPXItemId aContainerId );

    /**
     * Handle send playlist command
     *
     * @param aItemId playlist id
     */
    void DoSendPlaylistL( TMPXItemId aItemId );

    /**
     * Handle call back from collectionframework for send command
     *
     * @param aMedia media
     */
    void DoHandleSendL( const CMPXMedia& aMedia );

    /**
     * Handle rename command
     */
    void DoHandleRenameL();

    /**
     * Prepares media object for selected items
     *
     * @param aMedia on return, populates the media object with a media array
     *        containing info for currently selected items used by playlist
     *        does not own this object
     */
    void PrepareMediaForSelectedItemsL( CMPXMedia& aMedia, TMPXGeneralCategory aContainerCategory, TMPXGeneralType aContainerType );

    /**
     * Populates media object with information needed for save to
     * existing playlist
     *
     * @param aMedia media object containing the entry
     * @param aCollectionId Id for collection containing the object
     * @return Media object needed for save to playlist operation
     */
    CMPXMedia* PopulateMediaLC( const CMPXMedia& aMedia, TMPXItemId aCollectionId );

    /**
     * Handle rename playlist command
     * @param aMedia media
     */
    void DoHandleRenamePlaylistL( const CMPXMedia& aMedia );

    /**
     * Set/clears the flags for item in database
     * @param aIndex index of the item in the list box
     * @param aMedia media object containing at least the following attribute:
     *        TMPXAttribute( KMPXMediaIdGeneral,EMPXMediaGeneralCollectionId )
     * @param aFlag flag to set/clear
     * @param aSet ETrue to set the flag, EFalse to clear the flag
     * @param aEnableInfoDialog ETrue to enable info dialog display, EFalse to disable
     */
    void UpdateDatabaseFlagL( TInt aIndex, const CMPXMedia& aMedia,
        TUint aFlag, TBool aSet, TBool aEnableInfoDialog = ETrue );

    /**
     * Get duration of current view
     */
    void GetDurationL();

    /**
     * Handles file check result
     *
     * @param aViewToActivate View to activate if the result is ok
     * @param aMedia Media object containing the information
     */
    void DoHandleFileCheckResultL( TUid aViewToActivate, const CMPXMedia& aMedia );

#ifdef __ENABLE_MSK
    /**
     * Updates the middle softkey display to a label or icon depending on
     *  whether there are marked indices
     * @param aMskId middle softkey command id
     */
    void UpdateMiddleSoftKeyDisplayL( TInt aMskId );
#endif // __ENABLE_MSK

    /**
     * Handles the completion of adding a playlist event.
     * @param aPlaylist, a media object representing the exported
     *        playlist. The content of this object is the same
     *        as what's supplied in AddPlaylistL, except that
     *        URI for the playlist file is added to the object
     *        upon successful processing of AddPlaylistL. Client
     *        should take over the ownership of this object. NULL
     *        if an error has occured while processing AddPlaylistL.
     * @param aError. the error code for AddPlaylistL processing
     *        error.
     */
    void HandleAddCompletedL( CMPXMedia* aPlaylist, TInt aError );

    /**
    * Handles the completion of setting a media event.
    * @param aMedia, a media object representing the updated media.
    * @param aError. the error code for SetL processing error.
    */
    void HandleSetCompletedL( CMPXMedia* aMedia, TInt aError );

    /**
     * Handle playback message
     *
     * @param aMessage playback message
     */
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );

    /**
     * Handle collection message
     *
     * @param aMessage collection message
     */
    void DoHandleCollectionMessageL( const CMPXMessage& aMessage );

    /**
    * Calls to Re-open the collection view
    * @param aShowWaitDlg show the "opening" dialog or not
    */
    void DoIncrementalOpenL( TBool aShowWaitDlg = ETrue );

    /**
     * Start wait note for delayed action due to the items not ready
     * (incremental openL change)
     *
     * @param aCommand the action to be executed when the items become ready
     * @param aNote text for wait note
     * @param aSkipDisplay flag use to determine if waitnote should be displayed or not
     */
    void StartDelayedActionL( TInt aCommand, TDesC& aNote, TBool aSkipDisplay = EFalse );

// from base class MProgressDialogCallback

    /**
     * From MProgressDialogCallback
     * Callback method
     * Get's called when a dialog is dismissed.
     *
     * @param aButtonId Button pressed when the dialog is dismissed
     */
    void DialogDismissedL( TInt aButtonId );

// from base class MMPXCollectionObserver

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
     *  From MMPXCollectionObserver
     *  Handle media properties
     *
     *  @param aMedia media
     *  @param aError error code
     */
    void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     * From MMPXCollectionObserver
     */
    void HandleCommandComplete(CMPXCommand* aCommandResult,
                               TInt aError);

// from base class MMPXCollectionHelperObserver

    /**
     * From MMPXCollectionHelperObserver
     * Callback function to signal that adding a file is complete
     *
     * @param aError, error of the operation
     */
    void HandleAddFileCompleteL( TInt aErr );

// from base class MMPXCHelperObserver

    /**
     * From MMPXCHelperObserver
     * Handles the completion of any collection helper event.
     *
     * @param aOperation, operation completed
     * @param aErr. the error code
     * @param aArgument Argument returned from the operation
     */
    void HandleOperationCompleteL( TCHelperOperation aOperation,
                                   TInt aErr,
                                   void* aArgument );

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
     * @param aPlayer UID of the player
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
     *
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
     * @return Views Uid
     */
    TUid Id() const;

    /**
     * From CAknView
     * Command handling function.
     *
     * @param aCommand Command which is handled
     */
    void HandleCommandL( TInt aCommand );

	/**
	 * From CAknView
	 * Called when status pane layout has changed
	 */
	void HandleStatusPaneSizeChange();

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
     * From MEikListBoxObserver
     * Handles listbox events.
     *
     * @param aListBox Listbox where the event occurred.
     * @param aEventType Event type.
     */
     void HandleListBoxEventL(
        CEikListBox* aListBox,
        TListBoxEvent aEventType );

// from MEikCommandObserver

    /**
     * From MEikCommandObserver
     * Processes user commands.
     *
     * @param aCommandId ID of the command to respond to.
     */
    virtual void ProcessCommandL(TInt aCommandId);

// from MMPXCollectionFindObserver

    /**
     * From MMPXCollectionFindObserver
     * Handle callback for "find" operation
     * @param aEntries, CMPXMedia to be returned
     * @param aComplete ETrue no more entries. EFalse more entries
     *                  expected
     * @param aError error code
     */
    virtual void HandleFindAllL(const CMPXMedia& aResults,
                                TBool aComplete,TInt aError);

// from MMPXViewActivationObserver

    /**
     * From MMPXViewActivationObserver
     * Handle view activation.
     *
     * @param aCurrentViewType Current view type Uid.
     * @param aPreviousViewType Previous view type Uid.
     */
    void HandleViewActivation(
        const TUid& aCurrentViewType,
        const TUid& aPreviousViewType );
    /**
     * @see MMPXViewActivationObserver
     */
    void HandleViewUpdate(
        TUid aViewUid,
        MMPXViewActivationObserver::TViewUpdateEvent aUpdateEvent,
        TBool aLoaded,
        TInt aData);

// from MCoeViewActivationObserver

    /**
     * From MCoeViewActivationObserver
     * Handle view activation.
     *
     * @param aNewlyActivatedViewId newly activated view id.
     * @param aViewIdToBeDeactivated deactivated view id.
     */
    void HandleViewActivation(
        const TVwsViewId& aNewlyActivatedViewId,
        const TVwsViewId& aViewIdToBeDeactivated );

// from MCoeViewDeactivationObserver
    /**
    * Handles view deactivation notification from view server
    */
    void HandleViewDeactivation(const TVwsViewId& aViewIdToBeDeactivated,
                                const TVwsViewId& aNewlyActivatedViewId);

    void InitiateWaitDialogL();
    void FillAiwParametersL( CAiwGenericParamList& aParamList );

    void GetUint32Presentation( TUint32& aResult, const TDesC8& aBuf, TInt aOffset );
    /**
     * Launches Java Midlet Music Store
     *
     * @param aUid Uid of the Java app generated dynamically
     */
    void LaunchOperatorJavaMusicShopL(TUid aUid) ;

    /**
     * Launches Native Music Shop
     *
     * @param aUid Uid of the native app for music Shp
     */
 	void LaunchOperatorNativeMusicShopL();
 		/**
      * Launches Native URL Music Shop
      *
      * @param URl of the native app for music Shp
      */
 	void LaunchOperatorURLMusicShopL();

 	/*
 	 * Stores the current list box item index.
 	 */
 	void StoreListboxItemIndexL();
 	
 	void ShowAddedItemsDialogL();

 	/*
 	 * Internal function for registering to BackStepping service.
 	 */
 	void HandleBacksteppingActivation();

 	void ChangeCbaVisibility( TBool aVisible );

 	void UpdateCba();

	TBool NowPlayingOptionVisibilityL();
    void HandleInitMusicMenuPaneL(CEikMenuPane* aMenuPane );
    void DynInitMenuPaneAlbumL(TInt aResourceId, CEikMenuPane* aMenuPane );
    void DynInitMenuPanePlaylistL(TInt aResourceId, CEikMenuPane* aMenuPane );
    void DynInitMenuPaneGenreL(TInt aResourceId, CEikMenuPane* aMenuPane );
    void DynInitMenuPaneSongsL(TInt aResourceId, CEikMenuPane* aMenuPane );
    void DynInitMenuPanePlaylistSongsL(TInt aResourceId, CEikMenuPane* aMenuPane );

    void OpenAllSongsL();
    void OpenArtistAlbumsL();
    void OpenPlaylistsL();
    void OpenGenreL();
    void OpenPodcastsL();


private:    // Data

    MMPXCollectionUtility*      iCollectionUtility;
    CMPXCollectionViewHgContainer* iContainer;
    MMPXPlaybackUtility*        iPlaybackUtility;
    MMPXViewUtility*            iViewUtility;
    CMPXCommonUiHelper*         iCommonUiHelper;
    MMPXCollectionUiHelper*     iCollectionUiHelper;
    MMPXCollectionHelper*       iCollectionHelper;
    CMPXMedia*                  iUserPlaylists;

    CSendUi*                    iSendUi;
    HBufC*                      iTitle;
    HBufC*                      iDuration;
    HBufC*                      iOriginalTitle;
    HBufC*                      iOriginalDuration;
    HBufC*                      iNewName;
    CArrayFix<TInt>*            iBottomIndex;

    TBool                       iBackOneLevel;
    TBool                       iPossibleJump;
    TBool                       iIsGrabbed;
    TBool                       iIsEmbedded;
    TBool                       iAddingSong;
    TBool                       iIsDeleting;
    TBool                       iIsSavingReorderPlaylist;
    TBool                       iIsWaitNoteCanceled;
    TBool                       iInvalidFileExist;
    TBool                       iHandlingKeyEvent;
    TBool                       iAddingToNewPlaylist;
    TBool                       iExitOptionHidden;
    TBool                       iGoToMusicShopOptionHidden;
    TBool                       iIgnoreNextFocusChangedMessage;
    TBool                       iCollectionReady;  // for incremental open
    TBool                       iCollectionCacheReady; //cache ready
    
    TInt                        iCurrentPlaylistOp;
    TInt                        iNumSongAddedToPlaylist;
    TMPXItemId                  iPlaylistId;

    TInt                        iCurrentMediaLOp;   // current mediaL operation
    TInt                        iCurrentFindAllLOp; // current FindAllL operation
    TInt                        iLastDepth;
    TInt                        iPlayIndex;
    TInt                        iResourceOffset;       // must be freed
    TInt                        iSetMediaLCount;

    TInt                        iCurrentHighlightedIndex;

    TInt                        iCachedCommand; // for incremental open
    CListBoxView::CSelectionIndexArray* iCachedSelectionIndex;

    CEikButtonGroupContainer*       iCurrentCba;

    CAknQueryDialog*            iConfirmationDlg;

    const CArrayFix<TInt>*      iSelectionIndexCache; // not owned
#ifdef __ENABLE_MSK
    TInt                            iCurrentMskId;
    TBool                           iShowContextMenu;
#endif // __ENABLE_MSK

    MProfileEngine*             iProfileEngine;

    RArray<TUid> iPlayersList;
    CAiwServiceHandler* iServiceHandler;
    TInt iErrorAttachCopyMenu;
    TMPXPlaybackPlayerType iCurrentPlayerType;
    HBufC* iSubPlayerName;

    HBufC* iCurrentSelectedIndex;   // current selected item index
    TBool   iUsingNokiaService;

    CMPXCollectionOpenUtility* iIncrementalOpenUtil;

    TInt iUpCount;
    TInt iDownCount;

    CMediatorCommandInitiator* iCommandInitiator;
    TPtrC iItemTitle;
    TInt iWaitNoteId;
    TBool iCoverDisplay;

#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
    TBool iPodcast;
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU

#ifdef BACKSTEPPING_INCLUDED
    MMPXBackSteppingUtility* iBackSteppingUtility;
    TBool iActivateBackStepping;
#endif // BACKSTEPPING_INCLUDED

    TBool iUSBOnGoing;
    TBool iInAlbumArtDialog;
    TBool iDisablePodcasting;
    TProcessPriority iPriority;
    TBool iInSongDetails;

    TBool iUpnpFrameworkSupport;
    CMediaRecognizer* iMediaRecognizer;
    TInt iErrorAttachAssignMenu;
    TWaitNoteType iNoteType;

    // Music Store
    TInt iOperatorMusicStore;
    TUint32 iMusicStoreUID;
	TUint32 iOperatorNativeMusicStoreUID;
    TUid iOperatorMusicStoreUID;
    HBufC16* iOperatorMusicStoreName;
    TInt iOperatorMusicStoreType;
    TInt iMusicStoreWebPage;
    HBufC16* iOperatorMusicStoreURI;

    // Wait note for delete
    CAknWaitDialog*	iWaitDialog;
    TBool iIsAddingToPlaylist;  // flag used to handle the return by adding songs incrementally
    CUpnpCopyCommand* iUpnpCopyCommand;

    TBool iOpeningNote; // Waitnote is opening
	TBool iMarkedAll;
	TBool iFirstIncrementalBatch;
    CMPXMedia *iStoredAlbum; // owned
    
    TBool                       iDialogDismissed;
    HBufC*                      iTitleWait; // Owned
    };

#endif  // C_CMPXCOLLECTIONVIEWHGIMP_H

// End of File
