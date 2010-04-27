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
* Description:  Music Player upnpplayback dialog declaration
*
*/



#ifndef MPXUPNPPLAYBACKDIALOG_H
#define MPXUPNPPLAYBACKDIALOG_H


//  INCLUDES
#include <akntoolbarobserver.h>
#include <AknDialog.h>
#include <mpxcommonframeworkdefs.h>
#include <mpxplaybackobserver.h>  //MMPXPlaybackObserver, MMPXPlaybackCallback
#include <mpxcollectionobserver.h>
#include "mpxlayoutswitchobserver.h"
#include "mpxplaybackframeworkdefs.h" // TMPXPlaybackPlayerType
#include <mpxmediakeyhandlerobserver.h>

// FORWARD DECLARATIONS
class CMPXUPnPPlaybackDialogCustomControl;
class MMPXPlaybackUtility;
class CMPXAlbumArtUtil;
class CMPXCommonUiHelper;
class MMPXViewUtility;
class CMPXCollectionPlaylist;
class CMPXUPnPPlaybackDialogLayout;
class MMPXPlayerManager;
class MMPXCollectionUtility;
class MMPXMediaKeyHandler;
class CAknTitlePane;
class CAknNavigationControlContainer;  // iNaviPane
class CAknNavigationDecorator;         // iOrigNaviPane
class CMPXMedia;
class CMPXMediaArray;
class CAknToolbar;
class CAknButton;

// CLASS DECLARATION

/**
 *  CMPXUpnpPlaybackEditorDialog dialog class for UPnP Remote ->Local
 *  or Remote -> Remote playback
 */
NONSHARABLE_CLASS( CMPXUPnPPlaybackDialog ) :  public CAknDialog,
                                               public MMPXPlaybackObserver,
                                               public MMPXPlaybackCallback,
                                               public MMPXLayoutSwitchObserver,
                                               public MMPXCollectionMediaObserver,
                                               public MMPXMediaKeyHandlerObserver,
                                               public MAknToolbarObserver
    {
public:  // Constructors and destructor

    /**
     * Symbian OS two-phased constructor
     * @return MP UpnpPlayback Dialog.
     */
    IMPORT_C static CMPXUPnPPlaybackDialog* NewL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CMPXUPnPPlaybackDialog();

    /**
     * Parameter passed in from viewframework
     *
     * @param aParam Parameter pass in from viewframework.
     */
    IMPORT_C void SetParamL( const TDesC* aParam );

protected:

    /**
     * Updates playback view.
     */
    void UpdateViewL();

    /**
     * Updates track info field.
     *
     * @param aMedia Media's properties. If NULL, default info will
     *                    be shown.
     */
    void UpdateTrackInfoL(
        const CMPXMedia* aMedia );

    /**
     * Update current playback state.
     *
     * @param aPlaybackState Current playback state.
     */
    void UpdatePlaybackStateL(
        TMPXPlaybackState aPlaybackState );

    /**
     * Updates track's playback position.
     *
     * @param  aPos, New playback postions in seconds
     * @param  aDuration, Track duration in seconds
     */
    void UpdateTrackPlaybackPositionL(
        TInt aPos,
        TInt aDuration );

    /**
     * Updates track's album art.
     *
     * @param aMedia Media's properties. If NULL, default album art
     *                    will be shown.
     */
    void UpdateAlbumArtL(
        const CMPXMedia* aMedia );

    /**
     * Updates track position in playlist field.
     */
    void UpdateTrackPosInPlaylistL();

    /**
     * Updates download state label.
     */
    void UpdateDownloadStateLabelL();

    /**
     * Updates the title pane
     */
    void UpdateTitlePaneL();

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
        TInt aError,
        TBool aInit = EFalse);

    /**
     *  Handle media properties.
     *
     *  @param aMedia media properties
     *  @param aError error code
     */
    void DoHandleMediaL( const CMPXMedia& aMedia,
                         TInt aError );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     *  @param aData Extra data parameter, used for any extra information
     *               along with the state change message
     */
    void DoHandleStateChangedL(
        TMPXPlaybackState aState,
        TInt aData );

    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    void HandleErrorL( TInt aError );

    /**
     * Displays error note for when all tracks are invalid,
     * and goes back to collection list view
     */
    void HandleAllTracksInvalidL();

    /**
     * Checks whether or not to skip to next track on an error
     *
     * @param aCurrentIndex Current index
     */
    void SkipOnErrorL( TInt aCurrentIndex, TInt aCount );


    /**
     * Request for the media object
     * aDrm Flag whether or not to request all DRM info as well
     */
    void RequestMediaL( TBool aDrm=EFalse );

    /**
    * Prepare status pane for full-screen playback view.
    */
    void PrepareStatusPaneForPlaybackViewL();

    /**
    * Restore status pane to it's normal state.
    */
    void PrepareStatusPaneForExitingViewL();

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

private:

    /**
     * C++ default constructor.
     */
    CMPXUPnPPlaybackDialog();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( TInt aResource );

private:    // New functions

    /**
     * Find the components custom control.
     */
    CMPXUPnPPlaybackDialogCustomControl *GetUPnPPlaybackDialogCustomControl() const;

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
     * Backs up Status Pane.
     */
    void BackupPreviousStatusPaneL();

    /**
     * Restores Previous Status Pane.
     */
    void RestorePreviousStatusPaneL();

    /**
     * Shows a Not supported error note
     */
    void CommandNotSupportedL();

    /**
     * Exits the dialog when callback from CIdle
     */
    static TInt DelayedExit( TAny* aPtr );


private: // Functions from base classes

    /**
     * Creates custom control for the dialog
     * @param aControlType control type
     */
    SEikControlInfo CreateCustomControlL( TInt aControlType );

    /**
     * Initialises the dialog's controls before the dialog is sized and layed out.
     */
    void PreLayoutDynInitL();

    /**
     * This function ALWAYS returns ETrue.
     * @param aButtonId button ID
     */
    TBool OkToExitL( TInt aButtonId );

    /**
     * Returns the number of component controls.
     */
    TInt CountComponentControls() const;

    /**
     * Returns the component control.
     * @param aIndex index of the component
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

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
     * @since 3.0
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
     * @since 3.0
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
     *
     * @since 3.0
     * @param aMedia media
     * @param aError error code
     */
    void HandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

// from base class MMPXCollectionMediaObserver
    /**
    *  Handle extended media properties
    *
    *  @param aMedia media
    *  @param aError error code
    */
    void HandleCollectionMediaL(
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
    void HandleCommandL( TInt aCommand );

    /**
     * From CAknView
     * Processes user commands.
     *
     * @since 3.0
     * @param aCommand ID of the command to respond to.
     */
    void ProcessCommandL( TInt aCommandId ) ;

    /**
     * From CAknView
     * Event handler for status pane size changes.
     *
     * @since 3.0
     */
    void HandleStatusPaneSizeChange();

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

// from base class CoeControl
    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );

// from base class MMPXLayoutSwitchObserver

    /**
     * From MMPXLayoutSwitchObserver
     * Callback function when layout is changed.
     *
     * @since S60 v3.0
     */
    void HandleLayoutChange();


    TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

private:

    /**
     *  Sets the playback volume.
     *
     *  @param aVolume volume value.
     */
    void SetVolumeL( const TInt aVolume );

    /**
     *  Mutes the playback volume.
     */
    void MuteVolumeL();

    /**
     *  Unmutes the playback volume.
     */
    void UnMuteVolumeL();
    /**
     *  Update toolbar.
     */
    void UpdateToolbarL();
    /**
     * From MAknToolbarObserver
     * Processes user commands.
     *
     * @since 5.0
     * @param aCommand ID of the command to respond to.
     */
    void OfferToolbarEventL( TInt aCommandId );
      
    /**
     * From MAknToolbarObserver
     * Used to change toolbar settings before it is made visible
     *
     * @since 5.0
     * @param aCommand ID of the command to respond to.
     */
    void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolBar );

private:    // Data

    MMPXPlaybackUtility* iPlaybackUtility;  // not own
    CMPXAlbumArtUtil* iMPXUtility;   // own
    CMPXMedia* iMedia;  // own

    CMPXCommonUiHelper* iCommonUiHelper;    // own

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
    // Feature flag for set as ringtone
    TBool iChangeRTForAllProfiles;

    // Error code to be handled at a later time
    TInt iDelayedError;

    CMPXUPnPPlaybackDialogLayout* iPlaybackDialogLayout;    // owned

    RArray<TUid> iPlayersList;
    TMPXPlaybackPlayerType iCurrentPlayerType;
    MMPXCollectionUtility* iCollectionUtility;

    TInt iStartPlaybackIndex;
    TInt iLastSkipDirection;
    TBool iSkipping;

    // RemCon target for receiving media key events
    MMPXMediaKeyHandler* iMediaKeyHandler;  // owned
    TInt iResourceOffset;       // must be freed
    TInt iCommonPlaybackResourceOffset; // must be freed
    TInt iUpnpResourceOffset; // must be freed
    TRect iModRect; // MainPane rectangle

    // Backup and restore previous view title and navi pane
    TInt iPreviousLayoutId;
    CAknTitlePane* iTitlePane;             // Not owned
    HBufC* iOrigTitle;             // Original title text, owned
    CAknNavigationControlContainer* iNaviPane;              // Not owned

    TKeyEvent iLastKeyEvent;
    TEventCode iLastKeyType;
    HBufC* iSubPlayerName;

    CIdle* iDelayedEventExit; // Owned

    TBool iKeySoundDisabled;

    TInt iCurrentVolume;

    TInt iFMTXResourceOffset;   // must be freed
    CAknToolbar* iToolbar; // not owned
    
    CAknButton* iBtnPreviousItem; // not owned
    CAknButton* iBtnNextItem; // not owned
    CAknButton* iBtnPlayPause; // not owned
    TBool iSeekForward;
    };

#endif      // MPXUPNPPLAYBACKDIALOG_H

// End of File
