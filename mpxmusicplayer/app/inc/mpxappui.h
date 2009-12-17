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
* Description:  Application UI class required by AVKON application architecture.
*
*/



#ifndef CMPXAPPUI_H
#define CMPXAPPUI_H


// INCLUDES
#include <e32std.h>
#include <f32file.h>
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
#include <AknServerApp.h>
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER
#include <aknViewAppUi.h>
#include <apparc.h>
#include <AknProgressDialog.h>  // MProgressDialogCallback
#ifdef FF_OOM_MONITOR2_COMPONENT
#include <oommonitorsession.h>
#endif
#include <iaupdateobserver.h>  //IAD
#include <mpxplaybackobserver.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelper.h>
#include <mpxmediakeyhandlerobserver.h>
#include <mpxviewactivationobserver.h>
#include <mpxcollectionhelper.h>
#include <mpxcollectionuihelperobserver.h>
#include "mpxsavehelper.h"
#include "mpxcontroller.h"

// FORWARD DECLARATIONS
class MMPXPlaybackUtility;
class MMPXCollectionUtility;
class MMPXViewUtility;
class CMPXCommonUiHelper;
class MMPXMediaKeyHandler;
class CRepository;
class CMPXRestorePath;
class MMPXHarvesterUtility;
class MMPXCollectionHelper;
class CMPXCommandHandler;
class CIdle;
class CIAUpdate; //IAD
class CIAUpdateParameters; //IAD
//class CMPXController;

// Cover UI start
//#ifdef __COVER_DISPLAY
class CMediatorCommandInitiator;
//#endif // __COVER_DISPLAY
// Cover UI end
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
class CMPXPdCommandHandler;
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT
class CCameseDrmHandler;
#endif
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
class CDocumentHandler;
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER

// CLASS DECLARATION

/**
 * Application UI class.
 * Provides support for the following features:
 * - EIKON control architecture
 * - view architecture
 * - status pane
 *
 *  @lib mpx.exe
 *  @since 3.1
 */
class CMPXAppUi : public CAknViewAppUi,
                  public MMPXPlaybackObserver,
                  public MMPXCollectionObserver,
                  public MMPXCHelperEmbeddedOpenObserver,
                  public MMPXMediaKeyHandlerObserver,
                  public MMPXViewActivationObserver,
                  public MMPXSaveHelperObserver,
                  public MMPXCollectionHelperObserver,
                  public MProgressDialogCallback,
                  public MFileManObserver,
                  public MMPXPlaybackCallback,
                  public MIAUpdateObserver
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
                  ,public MAknServerAppExitObserver
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    {
public:

    /**
     * C++ default constructor.
     */
    CMPXAppUi();

    /**
     * Destructor.
     */
    virtual ~CMPXAppUi();

    /**
     * This function is used for querying whether the application
     * is launched in embedded mode or not.
     * @since 3.1
     * @return ETrue:  The application is launched in embedded mode.
     *         EFalse: The application is launched in standalone mode.
     */
    TBool IsEmbedded() const;

    /**
     * Opens temporary playlist or album from buffer. Buffer contents are
     * handled as album if the album name is provided.
     * @since 3.1
     * @param aBuffer Descriptor containing playlist contents (each track's
     *                path is separated with a line feed "\n").
     * @param aName Album name.
     */
    void OpenPlaylistFromBufferL(
        const TDesC& aBuffer,
        TDesC& aName );

    /**
     * Opens the specified file in response.
     * @since 3.1
     * @param aFileStore Not used
     * @param aFile      File handle to open
     */
    void OpenFileL(
        CFileStore*& aFileStore,
        RFile& aFile,
        const CAiwGenericParamList* aParams );

    /**
     * Make Phone/Idle/ActiveIdle come to foreground.
     */
    void ActivatePhoneAppL();

    /**
     * Function for deferred exit call back
     *
     * @param aPtr Pointer to self
     * @return System wide error code
     */
    static TInt DeferredExitCallback( TAny* aPtr );

private:

    /**
     * Start a waitnote dialog
     */
    void StartWaitNoteL( TWaitNoteType aNoteType );

    /**
     * Close the waitnote dialog
     */
    void CloseWaitNoteL( );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Handle media key commands.
     *
     * @param aCommand Media key commands. see mpxappui.hrh
     */
    void DoHandleMediaKeyCommandL( TMPXPlaybackCommand aCommand );

    /**
     * Control popup behaviour.
     *
     * @param aCommand Media key commands. see mpxappui.hrh
     * @param aValue Parameter for aCommand
     */
    void HandlePopupL( TMPXPlaybackCommand aCommand );

    /**
     * Check if Active Idle app is on foreground.
     *
     * @return ETrue if idle app is in the foreground, EFalse otherwise.
     */
    TBool IsIdleAppForeground();

    /**
     * Check if Active Idle is enabled.
     *
     * @return ETrue if active idle is in the enabled, EFalse otherwise.
     */
    TBool IsActiveIdleEnabled();

    /**
     * Creates and initializes all member variables
     */
    void CreateMemberVariablesL();

    /**
     * Saves a file
     */
    void DoSaveL();

    /**
     * Creates a new save path filename
     *
     * @param aNewPath Contains new save path on return
     */
    void CreateAutomaticSavePathL( TFileName& aNewPath );

    /**
     * Handling to be done after save/add file is complete
     */
    void HandlePostSaveOperationL();
    
     /**
     * Handles WMA DRM Protected Media
     *
     * @return ETrue if it's ok to open the document, EFalse otherwise.
     */
    TBool HandleWMAProtectionL();

    /**
     * Handles both OMA and WM DRM Protected Media
     *
     * @param aMedia Media object containing the rights information.
     * @return ETrue if it's ok to open the document, EFalse otherwise.
     */
    TBool HandleDRMProtectionL(const CMPXMedia& aMedia);

    /**
     * Handles broadcast messages
     */
    void HandleBroadcastMessageL( const CMPXMessage& aMessage );

    /**
     * Handles commands passed in by other applications
     */
    void HandleCommandParametersL( const TDesC8& aCommand );

    /**
     * Handle playback message
     *
     * @param aMessage playback message
     */
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );
    
    #ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT
    /**
     * Handle Camese Playback PreProcessing
     *
     * @param aMessage playback message
     * @param aSkipEvent whether App Ui should skip this event
     */
    void HandleCamesePlaybackPreProcessingL(const CMPXMessage& aMessage, TBool& aSkipEvent);
   #endif


    /**
     * Handle Player Changed Playback Message
     *
     */
    void HandlePlaybackPlayerChangedL();

    /**
     * Handle custom commands.
     *
     * @param aUid Command category Uid. Different command set will have
     *             different Uid to avoid clash.
     * @param aCommand Command to be handled.
     */
    void DoHandleCustomCommandL(
        const TUid& aUid,
        const TInt aCommand );

    /**
     * Callback function to signal that saving a file is complete
     *
     * @param aErr Error of the operation.
     */
    void DoHandleSaveCompleteL( TInt aErr );

    /**
     * Handle collection message
     *
     * @param aMessage collection message
     */
    void DoHandleCollectionMessageL( const CMPXMessage& aMessage );
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
    /**
     * Check if the Browser is closed.
     *
     * @param aParameter AIW parameter passed in by other applications
     */
    TBool IsBrowserClosed(const TDesC8& aParameter);

    /**
     * Brings the browser to the foreground.
     *
     * @return ETrue if browser was found, EFalse otherwise.
     */

    TBool BringUpBrowserL();
#endif

    /*
    * Returns whether playback is remote i.e using UPNP.
    * @return ETrue if playback is remote, EFalse otherwise.
    */
    TBool IsPlaybackRemote();

    /*
    * Sends a command to collection to set media
    */
    void SetMediaL( CMPXMedia& aMedia );
// from base class MMPXPlaybackObserver

    /**
     * From MMPXPlaybackObserver
     * Handle playback message
     *
     * @since 3.1
     * @param aMessage playback message
     * @param aErr system error code.
     */
    void HandlePlaybackMessage(
        CMPXMessage* aMessage, TInt aError );

// from base class MMPXCollectionObserver

    /**
     * From MMPXCollectionObserver
     * Handle media properties.
     *
     * @since 3.1
     * @param aMedia  media properties.
     * @param aError Error code.
     */
    void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     * From MMPXCollectionObserver
     * Handle collection message.
     *
     * @since 3.1
     * @param aMessage Collection message
     * @param aErr system error code.
     */
    void HandleCollectionMessage(
        CMPXMessage* aMessage, TInt aError );

    /**
     * From MMPXCollectionObserver
     * Handles the collection entries being opened.
     *
     * @since 3.1
     * @param aEntries Collection entries opened.
     * @param aIndex Focused entry.
     * @param aComplete ETrue no more entries. EFalse more entries
     *                  expected.
     * @param aError Error code.
     */
    void HandleOpenL(
        const CMPXMedia& aEntries,
        TInt aIndex,
        TBool aComplete,
        TInt aError );

    /**
     * From MMPXCollectionObserver
     * Handles the item being opened.
     *
     * @since 3.1
     * @param aPlaylist Collection playlist, owner ship is transfered.
     * @param aError Error code.
     */
    void HandleOpenL(
        const CMPXCollectionPlaylist& aPlaylist,
        TInt aError );

// from base class MMPXCHelperEmbeddedOpenObserver

    /**
    * From MMPXCHelperEmbeddedOpenObserver
    * Handles errors from opening in embedded mode
    *
    * @since 3.1
    * @param aErr Error code
    * @param aCategory Type of item to be opened.
    */
    void HandleEmbeddedOpenL( TInt aErr, TMPXGeneralCategory aCategory  );

// from base class MMPXMediaKeyHandlerObserver

    /**
     * Handle media key commands.
     *
     * @since 5.0
     * @param aCommand Command to be handled.
     * @param aData data for the specific command
     */
    void HandleMediaKeyCommand(
        const TMPXPlaybackCommand aCommand,
        const TInt aData = KErrNotFound );

// from base class MMPXViewActivationObserver

    /**
     * From MMPXViewActivationObserver
     * Handle view activation.
     *
     * @since 3.1
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

// from base class CEikAppUi

    /**
     * From CEikAppUi
     * opens the specified file in response to a corresponding message.
     *
     * @param aFileName File to be opened.
     */
    void OpenFileL( const TDesC& aFileName );

    /**
     * From CCoeAppUi
     * called when event occurs of type EEventFocusLost or EEventFocusGained.
     *
     * @param aForeground ETrue if the application is in the foreground,
     *                    otherwise EFalse.
     */
    void HandleForegroundEventL( TBool aForeground );

    /**
     * From CEikAppUi
     * processes shell commands.
     *
     * @param aCommand The shell command sent to the application.
     * @param aDocument Name The name of the document as
                        specified on the command line
     * @param aTail Tail end information in the command
     * @return ETrue if the file exist, EFalse otherwise
     */
    TBool ProcessCommandParametersL(
        TApaCommand aCommand,
        TFileName& aDocumentName,
        const TDesC8& aTail );

    /**
     * From CEikAppUi
     * takes care of command handling.
     *
     * @param aCommand The ID of the command.
     */
    void HandleCommandL( TInt aCommand );

    /**
     * From CEikAppUi
     * handle window server events.
     *
     * @param aEvent The window server event to be handled.
     * @param aDestination The control associated with the event.
     */
    void HandleWsEventL(
        const TWsEvent& aEvent,
        CCoeControl* aDestination );

    /**
     * From CEikAppUi
     * Handles window server messages.
     *
     * @param aClientHandleOfTargetWindowGroup The window group that the message
     *                                         was sent to.
     * @param aMessageUid The message UID.
     * @param aMessageParameters The Message parameters.
     * @return Indicates whether the message was handled or not.
     */
    MCoeMessageObserver::TMessageResponse HandleMessageL(
        TUint32 aClientHandleOfTargetWindowGroup,
        TUid aMessageUid,
        const TDesC8& aMessageParameters );

    /**
     * From CEikAppUi
     * Process message sent by another application
     *
     * @param aUid uid identifying the message.
     * @param aParams The message to be processed.
     */
    void ProcessMessageL( TUid aUid, const TDesC8& aParams );

    /**
     * From CEikAppUi
     * process command line parameter
     *
     * @param aCommandLine The message to be processed.
     */
    TBool ProcessCommandParametersL( CApaCommandLine& aCommandLine );


// from base class MMPXSaveHelperObserver

    /**
     * From MMPXSaveHelperObserver
     * Callback function to signal that saving a file is complete
     *
     * @since 3.1
     * @param aErr Error of the operation.
     */
    void HandleSaveComplete( TInt aErr );

// from base class MMPXCollectionHelperObserver

    /**
     * From MMPXCollectionHelperObserver
     * Callback function to signal that adding a file is complete
     *
     * @since 3.1
     * @param aErr Error of the operation.
     */
    void HandleAddFileCompleteL( TInt aErr );

// from base class MProgressDialogCallback

    /**
     * From MProgressDialogCallback
     * Callback method
     * Gets called when a dialog is dismissed.
     *
     * @param aButtonId Button pressed for the dialog to be dismissed.
     */
    void DialogDismissedL( TInt aButtonId );

// from base class MFileManObserver

    /**
     * From MFileManObserver
     * Callback method
     * Gets called during a file operation.
     *
     * @return Control status
     */
    MFileManObserver::TControl NotifyFileManOperation();

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER

// from base class MAknServerAppExitObserver

    /**
     * From MAknServerAppExitObserver.
     */
    void HandleServerAppExit(TInt aReason);
    /**
     * Create a Forwards parameters that were passed by browser to media player
     * If the new parameters is created successful, it will be leave on the CleanupStack
     * @param aParamList the original parameter
     * @return Forward parameters that is created.
     */
    CAiwGenericParamList* CreateForwardAiwParametersLC(const CAiwGenericParamList* aParamList) const;

#endif // RD_RA_SUPPORT_FOR_MUSIC_PLAYER

    /**
    * Update the refreshed time for music and podcast plugins
    */
    void DoUpdatePluginRefreshedTimeL();

// from base class MMPXPlaybackCallback
    /**
    *  Handle playback property
    *
    *  @param aProperty the property
    *  @param aValue the value of the property
    *  @param aError error code
    */
    void HandlePropertyL(TMPXPlaybackProperty aProperty,
                                 TInt aValue,
                                 TInt aError);

    /**
    *  Method is called continously until aComplete=ETrue, signifying that
    *  it is done and there will be no more callbacks
    *  Only new items are passed each time
    *
    *  @param aPlayer UID of the subplayer
    *  @param aSubPlayers a list of sub players
    *  @param aComplete ETrue no more sub players. EFalse more subplayer
    *                   expected
    *  @param aError error code
    */
    void HandleSubPlayerNamesL(TUid aPlayer,
                                       const MDesCArray* aSubPlayers,
                                       TBool aComplete,
                                       TInt aError);

    /**
    *  Call back of media request
    *
    *  @param aMedia media
    *  @param aError error code
    */
    void HandleMediaL(const CMPXMedia& aProperties,
                              TInt aError);

    /**
     *  Checks if the specified view is the active view
     *
     *  @param aViewType view identifier
     *  @return ETRUE if the view is active, EFALSE otherwise
     */
    TBool IsActiveView(TInt aViewType );

    /**
     * Reopens the path in order to get the latest data.
     *
     */
    void ReOpenPathL();

    /**
     * Suppress the collection view from coming up.
     *
     */
    TBool SuppressCollectionView();

    /**
     *  Sets the playback volume.
     *
     *  @param aVolume volume value.
     */
    void SetVolume( const TInt aVolume );

    /**
     *  Mutes the playback volume.
     */
    void MuteVolume();

    /**
     *  Unmutes the playback volume.
     */
    void UnMuteVolume();

    /**
     *  Translates Playback commands to MPX commands
     */
    TInt MPXCmdForPbCmd( TMPXPlaybackCommand aCommand );

    /**
    *  if current heap memory is less than the lowest need of music player
    *  music player well exit.
    *
    *  @param music player need memory
    */
    void CheckAvailableMemory( TInt aNeedMemory );

#ifdef FF_OOM_MONITOR2_COMPONENT
    /**
     * Dynamically set music player's busy or mornal priority in OOM
     * according to MPX playing state.
     *
     * @param aPriority     Correct priority to set.
     */
    void SetMpxOomPriorityL( ROomMonitorSession::TOomPriority aPriority );

    /**
    *  Check heap memory through OOM moudule.
    *  If less than the lowest need of music player
    *  music player will exit.
    *
    *  @param aNeedMemory   Memory needed by Music Player
    */
    void CheckAvailableMemoryByOomL( TInt aNeedMemory );
#endif //    FF_OOM_MONITOR2_COMPONENT

	/**
	* Launch matrix menu Music suite with given message
	*/
	void LaunchMmViewL( const TDesC8& aMessage );

    /**
    * Check updates
    */
    void CheckUpdatesL(); //IAD

    /**
    * Clean IAD update parameters
    */
    void CleanUpdateParams(); //IAD


    // from base class MIAUpdateObserver

    /**
    * This callback function is called when the update checking operation has completed.
    *
    * @param aErrorCode The error code of the observed update operation.
    *                   KErrNone for successful completion,
    *                   otherwise a system wide error code.
    * @param aAvailableUpdates Number of the updates that were found available.
    */
    void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates );

    /**
    * This callback function is called when an update operation has completed.
    *
    * @param aErrorCode The error code of the completed update operation.
    *                   KErrNone for successful completion,
    *                   otherwise a system wide error code.
    * @param aResult Details about the completed update operation.
    *                Ownership is transferred.
    */
    void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResultDetails );

    /**
    * This callback function is called when an update query operation has completed.
    *
    * @param aErrorCode The error code of the observed query operation.
    *                   KErrNone for successful completion,
    *                   otherwise a system wide error code.
    * @param aUpdateNow ETrue informs that an update operation should be started.
    *                   EFalse informs that there is no need to start an update
    *                   operation.
    */
    void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow );

private:
    enum TMPXSaveMode
    {
    EMPXSaveModeIdle = 0,
    EMPXOpenAfterSave,
    EMPXExitAppAfterSave,
    EMPXExitHostAppAfterSave,
    EMPXAssignRingtone,
    EMPXAssignContact
    };

private: //Data

    MMPXPlaybackUtility* iPlaybackUtility;  // own
    MMPXCollectionUtility* iCollectionUtility;  // own
    MMPXViewUtility* iViewUtility;  // own
    MMPXHarvesterUtility* iHarvesterUtility;  // own
    MMPXCollectionUiHelper* iCollectionUiHelper;  // own
    MMPXCollectionHelper* iCollectionHelper;  // own

    // RemCon target for receiving media key events
    MMPXMediaKeyHandler* iMediaKeyHandler;  // own

    CMPXCommonUiHelper* iCommonUiHelper;    // own
    CMPXSaveHelper* iSaveHelper;    // own
    CRepository* iSettingsRepository;  // own, for active idle setting

    // Handle commands passed in from other applications
    CMPXCommandHandler*  iProcessParameter; // own
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
    CMPXPdCommandHandler*  iPdParameterHandler; // own
    TBool iAppStartedByBrowser;
    TBool iPdPbvStartedByBrowser;
    TBool icollectionBrowsed;
    TBool iPbvStartedByBrowser;
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
    // Path backup and restore when music player is exited
    CMPXRestorePath* iPathRestore; // own

    // Music Player controller for System Events and startup check
    CMPXController*  iController;

    // Array of ring tone mime types for saving files
    CDesCArrayFlat iRingToneMimeTypes; // own

    HBufC* iSavePath;  // own

    CIdle* iIdle; // own

    RFile iFile;

    TInt iOpenState;
    TBool iIsUsbOnGoing; // USB status
    TBool iFormattingOnGoing; // Formatting state
    TBool iRefreshOnGoing;

    // Flag whether or not playback was paused for save operation
    TBool iPausedForSave;

    TBool iInitedVolume;

    TUid iPreviousViewType;  // previous view type
    TUid iCurrentViewType;  // current view type

    // UIDs for the music and podcast collection databases
    TUid iMusicCollectionId;
    TUid iPodcastCollectionId;

    RArray<TUid> iDeferredViewActivationUid;

    TInt iCurrentIndex;

    TMPXSaveMode iSaveMode;

    TBool iCancelFileMan;

    TBool iStandAlone;

    TInt iCurrentVolume;
    TInt iMaxVolume;

    TBool iUPnPUnavailableEventFromCollection;

    TBool iDelayedParameterHandling;

    TBool iIgnoreExternalCommand;

    TBool iRefreshCollectionView;

// Cover UI start
//#ifdef __COVER_DISPLAY
    CMediatorCommandInitiator* iCommandInitiator;
    TBool iCoverDisplay;
//#endif
// Cover UI end

#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT
    // Indicates we are handling Protected WMA Media on Open stage
    TBool iHandlingProtectedWMAMedia;
    CCameseDrmHandler* iCameseDrmHandler;       // own
#endif

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    CDocumentHandler* iDocHandler;
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER

    TInt iCmdActivatedViewTypeId;

    TBool iDisablePodcasting;
    TBool iLaunchModePlaying;   // Launch mode specified in startup parameter
    CIAUpdate* iUpdate;  // IAD client object
    CIAUpdateParameters* iParameters; // Parameters for IAD update

#ifdef FF_OOM_MONITOR2_COMPONENT
    ROomMonitorSession::TOomPriority iOomPriority;
#endif    // FF_OOM_MONITOR2_COMPONENT

	TBool iStartInAlbumMenu;
    };

#endif  // CMPXAPPUI_H

// End of File
