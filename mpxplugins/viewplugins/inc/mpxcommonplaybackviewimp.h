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
* Description:  MPX common playback view implementation
*  Version     : %version: da1mmcf#41.1.6.1.1.1.9 % << Don't touch! Updated by Synergy at check-out.
*
*/



#ifndef CMPXCOMMONPLAYBACKVIEWIMP_H
#define CMPXCOMMONPLAYBACKVIEWIMP_H


// INCLUDES
#include <aknview.h>
#include <AknServerApp.h>
#include <AknProgressDialog.h>  // MProgressDialogCallback
#include <mpxplaybackobserver.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionuihelperobserver.h>
#include <mpxpskeyobserver.h>   // MMPXPSKeyObserver
#include <mpxcenrepobserver.h>  // MMPXCenRepObserver
#include <mpxviewactivationobserver.h>  // MMPXViewActivationObserver

#include "mpxlayoutswitchobserver.h"
#include "mpxplaybackframeworkdefs.h" // TMPXPlaybackPlayerType

#include "../../../inc/musicplayerbldvariant.hrh"

// FORWARD DECLARATIONS
class CMPXCommonPlaybackViewContainer;
class MMPXPlaybackUtility;
class CMPXAlbumArtUtil;
class CMPXCommonUiHelper;
class MMPXViewUtility;
class CMPXCollectionPlaylist;
class CMPXCommonPlaybackViewLayout;
class MMPXPlayerManager;
class MMPXCollectionUtility;
class CMPXPSKeyWatcher;
class CMPXCenRepWatcher;
#ifdef IAD_INCLUDE_BACKSTEPPING
class MMPXBackSteppingUtility;
#endif // IAD_INCLUDE_BACKSTEPPING
class CAiwServiceHandler;
class CMediaRecognizer;
class CAiwGenericParamList;
#ifdef IAD_INCLUDE_UPNP
class CUpnpCopyCommand;
#endif
// CLASS DECLARATION

/**
 *  MPX playback view.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
class CMPXCommonPlaybackViewImp : public CAknView,
                                  public MMPXPlaybackObserver,
                                  public MMPXPlaybackCallback,
                                  public MAknServerAppExitObserver,
                                  public MMPXLayoutSwitchObserver,
                                  public MCoeViewActivationObserver,
                                  public MMPXCHelperObserver,
                                  public MMPXPSKeyObserver,
                                  public MMPXCenRepObserver,
                                  public MProgressDialogCallback,
                                  public MMPXViewActivationObserver,
                                  public MMPXCollectionObserver
    {
public:
    enum TCommandSender
        {
        ECsUnknown,
        ECsMediaKey,
        ECsRenderer
        };
public:
    /**
     * Destructor.
     */
    IMPORT_C virtual ~CMPXCommonPlaybackViewImp();

protected:
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    IMPORT_C virtual void ConstructL();

    /**
     * Updates playback view.
     */
    IMPORT_C virtual void UpdateViewL();

    /**
     * Updates track info field.
     *
     * @param aMedia Media's properties. If NULL, default info will
     *                    be shown.
     */
    IMPORT_C virtual void UpdateTrackInfoL(
        const CMPXMedia* aMedia );

    /**
     * Update current playback state.
     *
     * @param aPlaybackState Current playback state.
     */
    IMPORT_C virtual void UpdatePlaybackState(
        TMPXPlaybackState aPlaybackState );

    /**
     * Updates track's playback position.
     *
     * @param  aPos, New playback postions in seconds
     * @param  aDuration, Track duration in seconds
     */
    IMPORT_C virtual void UpdateTrackPlaybackPositionL(
        TInt aPos,
        TInt aDuration );

    /**
     * Updates track's album art.
     *
     * @param aMedia Media's properties. If NULL, default album art
     *                    will be shown.
     */
    IMPORT_C virtual void UpdateAlbumArtL(
        const CMPXMedia* aMedia );

    /**
     * Updates track position in playlist field.
     */
    IMPORT_C virtual void UpdateTrackPosInPlaylistL();

    /**
     * Updates download state label.
     */
    IMPORT_C virtual void UpdateDownloadStateLabelL();

    /**
     * Updates the title pane
     */
    IMPORT_C virtual void UpdateTitlePaneL();

    /**
     * Updates FM Transmitter Info
     */
    IMPORT_C virtual void UpdateFMTransmitterInfoL(
        TBool aForceUpdate = EFalse );

    /**
     *  Handle playback message
     *
     *  @param aMsg playback message
     */
    IMPORT_C virtual void DoHandlePlaybackMessageL(
        const CMPXMessage& aMessage );

    /**
     *  Handle playback property
     *
     *  @param aProperty the property
     *  @param aValue the value of the property
     *  @param aError error code
     */
    IMPORT_C virtual void DoHandlePropertyL(
        TMPXPlaybackProperty aProperty,
        TInt aValue,
        TInt aError );

    /**
     *  Handle media properties.
     *
     *  @param aMedia media properties
     *  @param aError error code
     */
    IMPORT_C virtual void DoHandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     *  @param aData Extra data parameter, used for any extra information
     *               along with the state change message
     */
    IMPORT_C virtual void DoHandleStateChangedL(
        TMPXPlaybackState aState,
        TInt aData );

    /**
     * Get simple embedded mode.
     *
     * @return ETrue if the player is in the embedded mode (i.e. launched
     *         from Browser or Message Centre).
     */
    IMPORT_C virtual TBool SimpleEmbeddedMode() const;

    /**
     * Get embedded status.
     *
     * @return ETrue if in embedded mode. Otherwise, EFalse.
     */
    IMPORT_C virtual TBool IsEmbedded() const;

    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    IMPORT_C virtual void HandleErrorL( TInt aError );

    /**
     * Displays error note for when all tracks are invalid,
     * and goes back to collection list view
     */
    IMPORT_C virtual void HandleAllTracksInvalidL();

    /**
     * Checks whether or not to skip to next track on an error
     *
     * @param aCurrentIndex Current index
     */
    IMPORT_C virtual void SkipOnErrorL( TInt aCurrentIndex, TInt aCount );

    /**
     * Function to display information notes
     *
     * @param aResourceId Resource ID of the text string.
     */
    IMPORT_C virtual void DisplayInfoNoteL( TInt aResourceId );

    /**
     * Function to display confirmation notes
     *
     * @param aResourceId Resource ID of the text string.
     */
    IMPORT_C virtual void DisplayConfirmionNoteL( TInt aResourceId );

    /**
     * Set process priority.
     */
    IMPORT_C virtual void SetProcessPriority();

    /**
     * Launch equalizer dialog.
     */
    IMPORT_C virtual void LaunchEqualizerL();

    /**
     * Launch Music Settings view.
     */
    IMPORT_C virtual void LaunchMusicSettingsL();

    /**
     * Launch FM Transmitter
     */
    IMPORT_C virtual void LaunchFMTransmitterL();

    /**
     * Request for the media object
     * aDrm Flag whether or not to request all DRM info as well
     */
    IMPORT_C virtual void RequestMediaL( TBool aDrm=EFalse );

    /**
    * Prepare status pane for full-screen playback view.
    */
    void PrepareStatusPaneForPlaybackViewL();

    /**
    * Restore status pane to it's normal state.
    */
    void PrepareStatusPaneForExitingViewL();

    /**
     * Checks if UPnP AP is defined
     */
    IMPORT_C virtual TBool IsUpnpVisible();

    /**
     * Displays the available players in the Play via submenu
     */
    IMPORT_C virtual void AddPlayersNamesToMenuL( CEikMenuPane& aMenuPane );

    /**
     * Adds one player type to the Play via submenu
     * @param aMenuPane Menu pane
     * @param aCommandId Command id
     * @param aPlayerManager Playback Utility manager
     * @param aPlayerType Current player type (Local, Remote)
     * @param aMenuText Text to be added to the menu
     */
    IMPORT_C virtual void AddPlayerNameToMenuL( CEikMenuPane& aMenuPane,
                                TInt aCommandId,
                                MMPXPlayerManager& aPlayerManager,
                                TMPXPlaybackPlayerType& aPlayerType,
                                const TDesC& aMenuText = KNullDesC );

    /**
     * Retrieves the current player name and type
     */
    IMPORT_C virtual void GetCurrentPlayerDetails();

    /**
     * Selects a new player for audio playback
     */
    IMPORT_C virtual void SelectNewPlayerL( TInt aCommand );

    /**
     * Adds current item to a saved playlist
     */
    IMPORT_C virtual void AddToSavedPlaylistL();

    /**
     * Adds current item to a new playlist
     */
    IMPORT_C virtual void AddToNewPlaylistL();

    /**
     * Prepares media for adding to a playlist
     * @return Media object for adding to playlist.  Caller is
     * responsible for deleting and popping off cleanupstack
     */
    IMPORT_C virtual CMPXMedia* PrepareMediaForPlaylistLC(
        const CMPXMedia& aMedia );

    /**
     * Check if the command is supported by the remote player
     * @return TBool whether the command is supported or not
     */
    IMPORT_C virtual TBool IsCommandSupportedL();

// from base class MProgressDialogCallback
    /**
     * From MProgressDialogCallback
     * Callback method
     * Get's called when a dialog is dismissed.
     */
    IMPORT_C virtual void DialogDismissedL( TInt aButtonId );

    /*
     * From MMPXCHelperObserver
     * Handles the completion of any collection helper event.
     *
     * @param aOperation, operation completed
     * @param aErr. the error code
     * @param aArgument Argument returned from the operation
     */
    IMPORT_C virtual void HandleOperationCompleteL( TCHelperOperation aOperation,
                                                    TInt aError,
                                                    void* aArgument );

// from base class MMPXPlaybackObserver

    /**
     * From MMPXPlaybackObserver
     * Handle playback message
     *
     * @since 3.1
     * @param aMessage Playback Message
     * @param aErr system error code.
     */
    IMPORT_C virtual void HandlePlaybackMessage(
        CMPXMessage* aMessage, TInt aError );

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
    IMPORT_C virtual void HandlePropertyL(
        TMPXPlaybackProperty aProperty,
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
    IMPORT_C virtual void HandleSubPlayerNamesL(
        TUid aPlayer,
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
    IMPORT_C virtual void HandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

// from base class MMPXCollectionMediaObserver
    /**
    *  Handle extended media properties
    *
    *  @param aMedia media
    *  @param aError error code
    */
    IMPORT_C virtual void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError);


// from base class CAknView

    /**
     * From CAknView
     * Command handling function.
     *
     * @since 3.0
     * @param aCommand Command which is handled
     */
    IMPORT_C virtual void HandleCommandL( TInt aCommand );

    /**
     * From CAknView
     * Processes user commands.
     *
     * @since 3.0
     * @param aCommand ID of the command to respond to.
     */
    IMPORT_C virtual void ProcessCommandL( TInt aCommand );

    /**
     * From CAknView
     * Event handler for status pane size changes.
     *
     * @since 3.0
     */
    virtual void HandleStatusPaneSizeChange();

    /**
     * From CAknView
     * Handles a view activation.
     *
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
     */
    IMPORT_C virtual void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );

    /**
     * From CAknView
     * View deactivation function.
     */
    IMPORT_C virtual void DoDeactivate();

    /**
     * From CAknView
     * Foreground event handling function.
     *
     * @param aForeground Indicates the required focus state of the control.
     */
    IMPORT_C virtual void HandleForegroundEventL( TBool aForeground );

// from base class MEikMenuObserver

    /**
     * From MEikMenuObserver
     * Dynamically initialises a menu pane.
     *
     * @param aResourceId The ID of the menu pane.
     * @param aMenuPane The menu pane itself.
     */
    IMPORT_C void DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane );

// from base class MMPXLayoutSwitchObserver

    /**
     * From MMPXLayoutSwitchObserver
     * Callback function when layout is changed.
     *
     * @since S60 v3.0
     */
    IMPORT_C virtual void HandleLayoutChange();

// from base class MCoeViewDeactivationObserver

    /**
     * From MCoeViewDeactivationObserver
     * Handles the activation of the view aNewlyActivatedViewId before
     * the old view aViewIdToBeDeactivated is to be deactivated
     *
     * @param aNewlyActivatedViewId View ID that is newly activated.
     * @param aViewIdToBeDeactivated View ID to be deactivated.
     */
    IMPORT_C virtual void HandleViewActivation(
        const TVwsViewId& aNewlyActivatedViewId,
        const TVwsViewId& aViewIdToBeDeactivated );

// from base class MMPXViewActivationObserver

    /**
     * From MMPXViewActivationObserver
     * Handle view activation.
     *
     * @param aCurrentViewType Current view type Uid.
     * @param aPreviousViewType Previous view type Uid.
     */
    IMPORT_C virtual void HandleViewActivation(
        const TUid& aCurrentViewType,
        const TUid& aPreviousViewType );

    /**
     * @see MMPXViewActivationObserver
     */
    IMPORT_C void HandleViewUpdate(
        TUid aViewUid,
        MMPXViewActivationObserver::TViewUpdateEvent aUpdateEvent,
        TBool aLoaded,
        TInt aData);

// from base class MMPXPSKeyObserver

    /**
    * From MMPXPSKeyObserver
    * Handle PS event
    *
    * @param aUid The UID that identifies the property category
    * @param aKey The property sub-key
    */
    IMPORT_C virtual void HandlePSEvent(
        TUid aUid,
        TInt aKey );

// from base class MMPXCenRepObserver

    /**
    * From MMPXCenRepObserver
    * Handle a change in a setting value.
    *
    * @param aRepositoryUid Central repository UID containing the setting
    * @param aSettingId Id of the setting
    */
    IMPORT_C virtual void HandleSettingChange(
        const TUid& aRepositoryUid,
        TUint32 aSettingId );

// From base class MMPXCollectionObserver

    /**
    * From MMPXCollectionObserver
    */
    IMPORT_C virtual void HandleCollectionMessage(CMPXMessage* aMsg, TInt aErr);

    /**
    * From MMPXCollectionObserver
    */
    IMPORT_C virtual void HandleOpenL(const CMPXMedia& aEntries, TInt aIndex,
                                      TBool aComplete, TInt aError);

    /**
    * From MMPXCollectionObserver
    */
    IMPORT_C virtual void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist,TInt aError);

    /**
    * From MMPXCollectionObserver
    */
    IMPORT_C virtual void HandleCommandComplete(CMPXCommand* aCommandResult,
                                                TInt aError);

    void FillAiwParametersL( CAiwGenericParamList& aParamList, TBool aSaved = EFalse );
    /**
     * Updates the middle softkey display to a label or icon depending on the playback state
     * @param aMskId middle softkey command id
     */
    IMPORT_C void UpdateMiddleSoftKeyDisplayL();
    /**
     * Updates the middle (play/pause) toolbar control key icon based on playback state
     */
    IMPORT_C void UpdateToolbar();
protected:
    static TInt DeferredAlbumArtExtractCallback( TAny* aPtr );

    /**
    * Callback function for delayed view deactivation
    * @param aPtr pointer to self
    */
    static TInt DelayedExit( TAny* aPtr);

private:
    /**
     *  Handle playback error message
     *
     *  @param aErr system error code.
     */
    void DoHandleErrorPlaybackMessageL( TInt aError );

    /**
     *  Handle collection message
     *
     *  @param aMsg Message to handle.
     *  @param aErr system error code.
     */
    void DoHandleCollectionMessageL( CMPXMessage* aMsg, TInt aErr );
    
    /**
     * Callback function of timer to handle delayed error
     * @param aPtr pointer to self
     */
    static TInt HandleDelayedError( TAny* aPtr );

protected:    // Data

    MMPXPlaybackUtility* iPlaybackUtility;  // not own
    CMPXCommonPlaybackViewContainer* iContainer;  // own

    CMPXAlbumArtUtil* iMPXUtility;   // own
    CMPXMedia* iMedia;  // own

    CMPXCommonUiHelper* iCommonUiHelper;    // own

    CMPXPSKeyWatcher*   iPSKeyWatcher;      // own
    CMPXCenRepWatcher*  iCRWatcher;         // own
    TInt                iFMTxFreqKHz;
    TInt                iFmTxActivity;
	
    // View utility.
    MMPXViewUtility* iViewUtility;

    // Current playback states
    TInt iDuration; // in seconds
    TInt iPosition; // in seconds
    TMPXPlaybackState iPlaybackState;
    TInt iRandomMode;
    TInt iRepeatMode;

    TBool iEmbedded;
    TBool iSwitchingView;
    TBool iExitOptionHidden;

    // Feature flag for set as ringtone
    TBool iChangeRTForAllProfiles;

    // Error code to be handled at a later time
    TInt iDelayedError;

    CMPXCommonPlaybackViewLayout* iLayout;  // owned

//#ifdef __UPNP_FRAMEWORK_2_0_
    RArray<TUid> iPlayersList;
    TMPXPlaybackPlayerType iCurrentPlayerType;
    HBufC* iSubPlayerName;
//#endif //__UPNP_FRAMEWORK_2_0_

    CAknProgressDialog* iWaitNote;

    MMPXCollectionUtility* iCollectionUtility;
    MMPXCollectionUiHelper* iCollectionUiHelper;

    TInt iStartPlaybackIndex;
    TInt iLastSkipDirection;
    TBool iSkipping;
    TInt iPreviousStatusPaneLayout;

    // Flag whether or not this was the last playback view activated,
    // used for error handling
    TBool iLastPBViewActivated;

    // Flag used for disable error msg pop up when usb event happens
    TBool iIgnoredByUsbEvent;

    // Flag that next view to be activated is a playback view type
    TBool iPBViewToBeActivated;
    
    // Flag that indicates Camese SuperDistribution Support
    TBool iCameseSuperDistSupport;

    // Flag that indicates a new item has been opened from Collection View.
    // Used for Camese Superdistribution error handling.
    TBool iNewItemOpened;

    CIdle* iIdle;

    // Idle for delayed exit
    CIdle* iDelayedExit;

private:    // Data

    TInt iResourceOffset;       // must be freed

    TInt iFMTXResourceOffset;   // must be freed

#ifdef IAD_INCLUDE_BACKSTEPPING
    MMPXBackSteppingUtility* iBackSteppingUtility;
#endif // BACKSTEPPING_INCLUDED

    CMPXMedia* iUserPlaylists;  // owned
    TBool iKeySoundDisabled;
    TBool iDatabaseNotReady;
    CAiwServiceHandler* iServiceHandler;
    CMediaRecognizer* iMediaRecognizer;
    TInt iErrorAttachAssignMenu;
    TBool iSkipBtnPressed;

//#ifdef __UPNP_FRAMEWORK_2_0_
    TBool iUpnpFrameworkSupport;
//#endif //__UPNP_FRAMEWORK_2_0_

    TInt iLastDelayedErr;
    TInt iErrIndex;
    CPeriodic* iDelayedErrorTimer;
    // Indicate FF button pressed or not
    TBool iIsffButtonPressed;
    TBool iBacking;
#ifdef IAD_INCLUDE_UPNP    
	CUpnpCopyCommand* iUpnpCopyCommand;
#endif
	TBool iUnsupportedNoteDisabled;
    HBufC* iOldUri;
	TInt iOldPosition; // in seconds
	TBool iIsTapped;
    TCommandSender iCommandSender;
    };

#endif  // CMPXCOMMONPLAYBACKVIEWIMP_H

// End of File
