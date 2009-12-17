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
* Description:  MPX podcast collection view implementation
*
*/



#ifndef C_CMPXPODCASTCOLLECTIONVIEWIMP_H
#define C_CMPXPODCASTCOLLECTIONVIEWIMP_H


// INCLUDES
#include <eikspmod.h>
#include <eikclb.h>
#include <AknProgressDialog.h>  // MProgressDialogCallback
#include <eikprogi.h>
#include <AknWaitDialog.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionobserver.h>
#include <mpxplaybackobserver.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionuihelperobserver.h>
#include <mpxcollectionhelper.h>
#include <mpxcollectionpath.h>  // TMPXItemId
#include <mpxviewactivationobserver.h>
#include "mpxpodcastcollectionview.h"

#include "mpxplaybackframeworkdefs.h" // TMPXPlaybackPlayerType

// FORWARD DECLARATIONS
class CMPXPodcastCollectionViewContainer;
class MMPXCollectionUtility;
class CMPXCollectionPlaylist;
class MMPXPlaybackUtility;
class CMPXCommonUiHelper;
class MMPXViewUtility;
class CSendUi;
class CAknNavigationDecorator;
class CAknNaviLabel;
class CAknNavigationControlContainer;
class CEikButtonGroupContainer;

class MMPXPlayerManager;
class CAiwServiceHandler;
#ifdef UPNP_INCLUDED
class CUpnpCopyCommand;
#endif
// CLASS DECLARATION

/**
 *  MPX Podcast Collection view.
 *
 *  @lib mpxpodcastcollectionview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPodcastCollectionViewImp ) : public CMPXPodcastCollectionView,
                                             public MMPXCollectionObserver,
                                             public MMPXPlaybackObserver,
                                             public MMPXPlaybackCallback,
                                             public MEikListBoxObserver,
                                             public MProgressDialogCallback,
                                             public MMPXCollectionFindObserver,
                                             public MMPXCHelperObserver,
                                             public MMPXViewActivationObserver,
                                             public MCoeViewDeactivationObserver,
                                             public MCoeViewActivationObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastCollectionViewImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastCollectionViewImp* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXPodcastCollectionViewImp();

    /**
     * Delete the selected items
     *
     * @ since 3.0
     */
    void DeleteSelectedItemsL();

private:

    /**
     * C++ default constructor.
     */
    CMPXPodcastCollectionViewImp();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Updates list box
     *
     * @param aEntries Podcast collection entries opened. Method may
     *                 update the entries array by removing the playing item
     *                 if currently within Never Played list view.
     * @param aIndex focused entry
     */
    void UpdateListBoxL(CMPXMedia& aEntries,
                        TInt aIndex );

    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    void HandleErrorL( TInt aError );

    /**
     * Updates the navi pane
     */
    void UpdateNaviPaneL();

    /**
     * Updates the title pane
     */
    void UpdateTitlePaneL();

    /**
     * Updates the option menu
     *
     */
    void UpdateOptionMenuL();

    /**
     * Start either the delete progress note
     */
    void StartProgressNoteL();

    /**
     * Updates the progress note text and progress bar
     * 
     * @param	aProgress 		Indicates the portion of the process completed
     * @param 	aProgressText	Text to be displayed on the progress note  	
     */
    void UpdateProcessL(TInt aProgress,const TDesC& aProgressText);

    /**
     * Updates the copy to remote menu attachment
     *
     */
    void UpdateCopyToRemoteMenu(TInt aMenuPane);

    /**
     * Updates playback status/indicatior
     * @param aStatusChangeFlag flag to indicate if a status change
     *        has indeed been detected
     * Returns the current selection index if the paths match
     */
    TInt UpdatePlaybackStatusL(TBool aStatusChangeFlag);

    /**
     * Change the button group
     *
     * @aResId resource ID
     */
    void SetNewCbaL( TInt aResId );

    /**
     * Display collection details
     *
     * @param aMedia media object containing the library details
     */
    void ShowCollectionDetailsL( const CMPXMedia& aMedia );

    /**
     * Handles Upnp menus from DynInitMenuPaneL()
     *
     * @param aResourceId the id of the resource being initialized
     * @param aMenuPane the menu pane the resource belongs to
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
    void DoHandleCopyToRemoteL(const CMPXMedia& aMedia,
                               TBool aComplete = ETrue );


    /**
     * checks if send option should be shown the option is selected
     *
     * @return ETrue if send command should be hided, EFalse if it should be shown
     */
    TBool SendOptionVisibilityL();

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
     * Handle call back from collectionframework for send command
     *
     * @param aMedia media
     * @param aError error code
     */
    void DoHandleSendL(const CMPXMedia& aMedia, TInt aError );
    /**
     * Prepares media object for selected items
     *
     * @param aMedia on return, populates the media object with a media array
     *        containing info for currently selected items used by playlist
     *        does not own this object
     *
     * @return error code
     */
    TInt PrepareMediaForSelectedItemsL( CMPXMedia* aMedia );

    /**
     * Set/clears the flags for item in database
     * @param aIndex index of the item in the list box
     * @param aMedia media object containing at least the following attribute:
     *        TMPXAttribute( KMPXMediaIdGeneral,EMPXMediaGeneralCollectionId )
     * @param aFlag flag to set/clear
     * @param aSet ETrue to set the flag, EFalse to clear the flag
     * @param aEnableInfoDialog ETrue to enable info dialog display, EFalse to disable
     */
    void UpdateDatabaseFlagL( TInt aIndex,
                              const CMPXMedia& aMedia,
                              TUint aFlag,
                              TBool aSet,
                              TBool aEnableInfoDialog = ETrue );

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
     * Updates a media to the collection
     * @param aMedia media to update
     * @param aSync synchronous operation or not
     */
    void DoSetCollectionL( CMPXMedia* aMedia, TBool aSync );

#ifdef __ENABLE_MSK
    /**
     * Updates the middle softkey display to a label or icon depending on whether
     * there are marked indices
     * @param aMskId middle softkey command id
     */
    void UpdateMiddleSoftKeyDisplayL( TInt aMskId );
#endif // __ENABLE_MSK

    /**
     * Changes the selected episode(s) status to Played /unplayed
     * @param aPlayed played status to set (EFalse: unplayed)
     */
    void SetPlayedStatusL( TBool aPlayed );

    /**
     * From MProgressDialogCallback
     * Callback method
     * Get's called when a dialog is dismissed.
     */
    void DialogDismissedL( TInt aButtonId );

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
    void HandleOpenL(const CMPXMedia& aEntries,
                     TInt aIndex,
                     TBool aComplete,
                     TInt aError );

    /**
     *  From MMPXCollectionObserver
     *  Handles the collection entries being opened. Typically called
     *  when client has Open()'d an item. Client typically responds by
     *  'playing' the item
     *
     *  @param aPlaylist Podcast collection path to item
     *  @param aError error code
     */
    void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist,
                     TInt aError );

    /**
     *
     *  From MMPXCollectionObserver
     *  Handle media properties
     *
     *  @param aMedia media
     *  @param aError error code
     */
    void HandleCollectionMediaL(const CMPXMedia& aMedia,
                                TInt aError );

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
     * @since 3.0
     * @param aMessage playback message
     * @param aErr system error code.
     */
    void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

// from base class MMPXPlaybackCallback

    /**
     * From MMPXPlaybackCallback
     * Handle playback property
     *
     * @since 3.0
     * @param aProperty the property
     * @param aValue the value of the property
     * @param aError error code
     */
    void HandlePropertyL(TMPXPlaybackProperty aProperty,
                         TInt aValue,
                         TInt aError );

    /**
     * From MMPXPlaybackCallback
     * Method is called continously until aComplete=ETrue, signifying that
     * it is done and there will be no more callbacks
     * Only new items are passed each time
     *
     * @since 3.0
     * @param aPlayer UID of the subplayer
     * @param aSubPlayers a list of sub players
     * @param aComplete ETrue no more sub players. EFalse more subplayer
     *                  expected
     * @param aError error code
     */
    void HandleSubPlayerNamesL(TUid aPlayer,
                               const MDesCArray* aSubPlayers,
                               TBool aComplete,
                               TInt aError );

    /**
     * From MMPXPlaybackCallback
     * Handle media properties
     *
     * @since 3.0
     * @param aMedia media
     * @param aError error code
     */
    void HandleMediaL(const CMPXMedia& aMedia,
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
    void DoActivateL(const TVwsViewId& aPrevViewId,
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
    void DynInitMenuPaneL(TInt aResourceId,
                          CEikMenuPane* aMenuPane );

// from MEikListBoxObserver

    /**
     * Handles listbox events.
     *
     * @param aListBox Listbox where the event occurred.
     * @param aEventType Event type.
     */
     void HandleListBoxEventL(CEikListBox* aListBox,
                              TListBoxEvent aEventType );

// from MEikCommandObserver

    /**
     * Processes user commands.
     *
     * @param aCommandId ID of the command to respond to.
     */
    virtual void ProcessCommandL(TInt aCommandId);

// from MMPXCollectionFindObserver

    /**
    *  Handle callback for "find" operation
    *  @param aEntries, CMPXMedia to be returned
    *  @param aComplete ETrue no more entries. EFalse more entries
    *                   expected
    *  @param aError error code
    */
    virtual void HandleFindAllL(const CMPXMedia& aResults,
                                TBool aComplete,TInt aError);

    /**
     * Launch Nokia Podcasting Application
     *
     */
    void LaunchPodcastAppL();

    /**
     * Launch Nokia Podcasting Application
     *
     */
    TInt CheckPodcastAppL();

    /**
     * Helper method that checks to see if the specified episode is
     * currently playing.
     *
     * @param aItem id of item to check if currently playing
     * @return ETrue if item currently playing else EFalse
     */
    TBool IsCurrentlyPlayingL(const TMPXItemId& aItem);

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
    
    /**
     * Stores the current list box item index.
     */
    void StoreListboxItemIndexL();

private:    // Data

    CMPXPodcastCollectionViewContainer* iContainer;  // own
    MMPXCollectionUtility*      iCollectionUtility;  // not own
    MMPXPlaybackUtility*        iPlaybackUtility;
    MMPXViewUtility*            iViewUtility;
    CMPXCommonUiHelper*         iCommonUiHelper;
    MMPXCollectionUiHelper*     iCollectionUiHelper;
    CMPXMedia*                  iMediaToSet; // own

    CSendUi*                    iSendUi;
    HBufC*                      iTitle;
    HBufC*                      iNumEpisode;
    HBufC*                      iDuration;
    HBufC*                      iOriginalTitle;
    HBufC*                      iOriginalDuration;
    CArrayFix<TInt>*            iBottomIndex;

    TBool                       iBackOneLevel;
    TBool                       iPossibleJump;
    TBool                       iChangeRTForAllProfiles;
    TBool                       iIsWaitNoteCanceled;
    TBool                       iIsDeleting;
    TBool                       iHandlingKeyEvent;
    TBool                       iExitOptionHidden;
    TBool                       iIgnoreNextFocusChangedMessage;

    // HandleOpen was called and listbox was updated, only EFalse
    // when view is activated
    TBool                       iHandleOpenProcessed;

    TInt                        iCurrentMediaLOp;   // current mediaL operation
    TInt                        iCurrentFindAllLOp; // current FindAllL operation
    TInt                        iLastDepth;
    TInt                        iLastSelectedIndex;
    TInt                        iCurrNotYetPlayedListIndex;
    TInt                        iResourceOffset;        // must be freed
#ifdef __ENABLE_MSK
    TInt                        iCurrentMskId;
    TBool                       iShowContextMenu;
#endif // __ENABLE_MSK

    CAknNavigationDecorator*        iNaviDecorator;
    CAknNaviLabel*                  iNaviLabel;
    CAknNavigationControlContainer* iNaviPane;

    CEikButtonGroupContainer*       iCba; // owned

    RArray<TUid> iPlayersList;
    CAiwServiceHandler* iServiceHandler;
    TInt iErrorAttachCopyMenu;
    TMPXPlaybackPlayerType iCurrentPlayerType;
    TInt iCurrentlyAttachedCopyToMenuPane;
    HBufC* iSubPlayerName;
    TBool iUpnpFrameworkSupport;

    TMPXItemId iSelectedItem;
    // Progress note for delete
    CAknProgressDialog*	iProgressDialog;
    CEikProgressInfo*	iProgressInfo;
#ifdef UPNP_INCLUDED
    CUpnpCopyCommand* iUpnpCopyCommand; 
#endif
    };

#endif  // C_CMPXPODCASTCOLLECTIONVIEWIMP_H

// End of File
