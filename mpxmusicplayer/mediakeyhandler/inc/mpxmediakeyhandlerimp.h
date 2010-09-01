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
* Description:  Implementation of media key handler
*
*/



#ifndef C_CMPXMEDIAKEYHANDLERIMP_H
#define C_CMPXMEDIAKEYHANDLERIMP_H


// INCLUDES
#include <remconcoreapitargetobserver.h>
#include <coecntrl.h>
#include <mpxplaybackobserver.h>
#include <mpxplaybackframeworkdefs.h>


// FORWARD DECLARATIONS
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;
class CRepository;
class CMPXRemConKeyResponse;
class MMPXPlaybackUtility;
class CAknVolumePopup;
class CMPXNotifierDialog;
class MMPXMediaKeyHandlerObserver;
class MMPXMetaDataHandler;
#ifdef UPNP_INCLUDED
class CUpnpCopyCommand;
#endif
// CLASS DECLARATION

/**
 *  Media hard key handler implementation class
 *
 *  @lib mpxmediakeyhandler.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXMediaKeyHandlerImp ): public CBase,
                                             public MMPXMediaKeyHandler,
                                             public MRemConCoreApiTargetObserver,
                                             public MMPXPlaybackObserver,
                                             public MCoeControlObserver,
                                             public MMPXPlaybackCallback
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.1
     * @param aFlags Flags for creating popups.
     * @param aObserver Observer for media key handling.
     * @return Pointer to newly created object.
     */
    static MMPXMediaKeyHandler* NewL(
        TMPXMediaKeyPopupFlags aFlags,
        MMPXMediaKeyHandlerObserver* aObserver );

    /**
     * Destructor.
     */
    virtual ~CMPXMediaKeyHandlerImp();

private:

    /**
     * C++ default constructor.
     *
     * @param aFlags Flags for creating popups.
     * @param aObserver Custom command observer for media key handling.
     */
    CMPXMediaKeyHandlerImp( MMPXMediaKeyHandlerObserver* aObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( TMPXMediaKeyPopupFlags aFlags );

    /**
     * Filter and send command to playback utility.
     *
     * @param aCommandId command Id to be sent.
     */
    void FilterAndSendCommand( TMPXPlaybackCommand aCommandId );

    /**
     * Help filter and send command to playback utility.
     *
     * @param aCommandId command Id to be sent.
     */
    void DoFilterAndSendCommandL( TMPXPlaybackCommand aCommandId );

    /**
     *  Handle playback message
     *
     *  @param aMessage playback message
     */
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );

    /**
     *  Handle playback property
     *
     *  @param aProperty the property
     *  @param aValue the value of the property
     *  @param aError error code
     */
    void DoHandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );

    /**
     *  Handle media properties.
     *
     *  @param aMedia media properties
     *  @param aError error code
     */
    void DoHandleMediaL( const CMPXMedia& aMedia, TInt aError );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     */
    void DoHandleStateChangedL( TMPXPlaybackState aState );

    /**
     * Callback for timer
     *
     * @param aPtr Pointer pass to this callback function.
     * @return Zero if callback function doesn't need to be called again.
     *         Otherwise, non-zero.
     */
    static TInt TimerCallback( TAny* aPtr );

    /**
     * Handle repeat event
     */
    void HandleRepeatEvent();

    /**
     * Retrieves the current player name and type
     */
    void GetSubPlayerInfoL(
        TDes& aSubPlayerName,
        TMPXPlaybackPlayerType& aCurrentPlayerType );

    /**
     * Sets the volume popup title to current remote player
     */
    void SetVolumePopupTitleL();

    /**
     * Checks if UPnP is set up and available
     */
    TBool IsUpnpVisibleL();

    /**
     * Updates the volume and the volume command
     */
	void UpdateVolume();

// from base class MMPXMediaKeyHandler

    /**
     * From MMPXMediaKeyHandler
     * Show playback popup
     *
     * @param aMode Playback popup behaviour mode
     */
    void ShowPlaybackPopupL( TMPXPlaybackPopupModes aMode );

    /**
     * From MMPXMediaKeyHandler
     * Show Volume popup
     */
    void ShowVolumePopupL();

    /**
     * From MMPXMediaKeyHandler
     * Dismiss notifier popup.
     *
     * @param aFlags Popup flag to determine which popup(s) should
     *               be dismissed.
     */
    void DismissNotifier( TMPXMediaKeyPopupFlags aFlags );

    /**
     * Enable or disable Media Key behavior
     * @since 3.1
     * @param aEnable, ETrue, media keys are sent to observers
     *                 EFalse, media keys are ignored
     */
    void SetEnableMediaKeys( TBool aEnable );
    
    /**
     * Gets called when orientation change begins
     */    
    void NotifyOrientationChangeBegin();
    
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
    void HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );

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
     *
     * @param aMedia media
     * @param aError error code
     */
    void HandleMediaL( const CMPXMedia& aMedia, TInt aError );

// from base class MRemConCoreApiTargetObserver

    /**
     * From MRemConCoreApiTargetObserver
     * A 'play' command has been received.
     *
     * @param aSpeed The playback speed.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoPlay(
        TRemConCoreApiPlaybackSpeed aSpeed,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A command has been received.
     *
     * @param aOperationId The operation ID of the command.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoCommand(
        TRemConCoreApiOperationId aOperationId,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'tune function' command has been received.
     *
     * @param aTwoPart Determine which channel to be used.
     * @param aMajorChannel The major channel number.
     * @param aMinorChannel The minor channel number.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoTuneFunction(
        TBool aTwoPart,
        TUint aMajorChannel,
        TUint aMinorChannel,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'select disk function' has been received.
     *
     * @param aDisk The disk.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoSelectDiskFunction(
        TUint aDisk,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'select AV input function' has been received.
     *
     * @param aAvInputSignalNumber The AV input.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoSelectAvInputFunction(
        TUint8 aAvInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'select audio input function' has been received.
     *
     * @param aAudioInputSignalNumber The audio input.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoSelectAudioInputFunction(
        TUint8 aAudioInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct );

	/**
	* From MCoeControlObserver. To handle the volume events.
	*/
	void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );

    /**
     * Check the mpx app is foreground.
     * 
     * @return The check result. If mpx is foreground, return true.
     */
    TBool IsAppForeground();
    
    static TInt VolumePopupSynchTimerCallback( TAny* aPtr );
    void DoVolumePopupSynch();
    void StartVolumePopupSynchTimer();
    
    
    /**
     * Callback function of Filter event timer
     * 
     * @param aPtr, Pointer to an instance of CMPXMediaKeyHandlerImp
     * @return value is ignored and is irrelevant in that context
     */
    static  TInt FilterEventTimerCallback( TAny* aPtr );
    
    /**
     * Do with the rest event of VolPopup
     * 
     * VolPopup CallBack functon is frequently called, 
     * But Music Palyer deals with the event slowly.
     * Thus VolPopup will not display smoothly.
	 * So Music Player needs timer to filter some events.
     */
    void DoFilterEventTimer();

private:    // Data

    CRemConInterfaceSelector* iInterfaceSelector;   // owned
    CRemConCoreApiTarget* iCoreTarget;              // not own
    CPeriodic* iTimer;                              // owned
    CMPXRemConKeyResponse* iResponseHandler;        // owned
    CRepository* iSettingsRepository;               // owned
    CAknVolumePopup* iVolPopup;                     // owned
    CMPXNotifierDialog* iPlaybackPopup;             // owned

    MMPXPlaybackUtility* iPlaybackUtility;          // not own
    MMPXMediaKeyHandlerObserver* iObserver;         // not own

    // RemCon target for handling AVRCP 1.3 metadata commands
    MMPXMetaDataHandler* iMetaDataHandler;  				// own

    TInt iResourceOffset;                           // must be freed
    TInt iShowPopups;                               // Indicate which popup(s) should be shown
    TMPXPlaybackCommand iCommandId;                 // Command ID for repeat key events
    TBool iEnable;                                  // Media Key enabled / disabled
    TBool iUpnpFrameworkSupport;                    // UPnP framework support

    /**
     * Current track's info
     */
    HBufC* iTrackTitle;                             // owned
    TMPXPlaybackState iPlayerState;
    TInt iPlaybackPosition; // in seconds
    TBool iSkipping;
    TInt iCurrentVol;
    TBool iIncreaseVol;
    TBool iMuted;
    TInt iVolumeSteps;			// Number of volume steps in ui
    #ifdef UPNP_INCLUDED
    CUpnpCopyCommand* iUpnpCopyCommand;
    #endif
    TInt iUpnpVolume; // absolute upnp volume value
    TInt iPreviousVol;          //Last time volume
    CPeriodic* iVolumePopupSynchTimer;
    CPeriodic* iFilterEventTimer;         //owned, filter volumePopup event
    };

#endif  // C_CMPXMEDIAKEYHANDLERIMP_H

// End of File
