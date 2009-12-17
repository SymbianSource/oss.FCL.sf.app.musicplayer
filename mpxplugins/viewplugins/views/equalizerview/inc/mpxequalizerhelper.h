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
* Description:  Helper class for managing equalizer utility instances.
*
*/



#ifndef C_MPXEQUALIZERHELPER_H
#define C_MPXEQUALIZERHELPER_H


//  INCLUDES
#include <EqualizerPresetsDialog.h>
#include <mdaaudiosampleplayer.h>
#include <mpxplaybackobserver.h>

#include "mpxequalizerview.h"


// FORWARD DECLARATIONS
class MMPXPlaybackUtility;
class CAudioEqualizerUtility;
class CEqualizerPresetsDialog;


// CLASS DECLARATION

/**
 *  Helper class for managing equalizer utility instances.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXEqualizerHelper ) : public CMPXEqualizerView,
                                           public MMPXPlaybackObserver,
                                           public MEqualizerPresetActivationObserver,
                                           public MMdaAudioPlayerCallback
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXEqualizerHelper* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXEqualizerHelper();

private:

    /**
     * C++ default constructor.
     */
    CMPXEqualizerHelper();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Initialize audio equalizer utility for current active playing track.
     */
    void InitializeEqualizerUtilityL();

    /**
     * Release audio equalizer utility.
     */
    void ReleaseEqualizerUtility();

    /**
     * Get current preset ID.
     *
     * @param aPresetId On return, Current preset ID.
     * @return Index to the preset array if found.
     *         KErrNotFound if not found.
     */
    TInt GetPresetId( TInt& aPresetId );

    /**
     * Initialize secondary audio equalizer utility
     * for situations when the engine cannot provide one.
     */
    void InitializeBackupEqualizerUtilityL();

    /**
     * Create secondary audio equalizer utility
     * based on latest preset settings.
     */
    void CreateBackupEqualizerUtility();

    /**
     * Setup active equalizer utility for equalizer dialog.
     */
    void SetupActiveEqualizerUtility();

    /**
     * Launch equalizer dialog.
     */
    void DoLaunchEqualizerDialogL();

    /**
     *  Handle playback message
     *
     *  @param aMessage playback message
     */
    void DoHandlePlaybackMessageL( 
        const CMPXMessage& aMessage );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     */
    void DoHandleStateChangedL(
        TMPXPlaybackState aState );

// from base class CMPXEqualizerView

    /**
     * From CMPXEqualizerView
     * Launch equalizer dialog.
     *
     * @since 3.0
     */
    void LaunchEqualizerL();

// from base class MMPXPlaybackObserver

    /**
     * From MMPXPlaybackObserver
     * Handle playback message
     * 
     * @param aMessage playback message
     * @param aErr system error code.
     */
    void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );
    
// from base class MEqualizerPresetActivationObserver

    /**
     * From MEqualizerPresetActivationObserver
     * Handle preset id activation from Equalizer dialog.
     * 
     * @param aActivatedPreset Current activated preset id.
     */
    void HandlePresetActivation( TInt aActivatedPreset );

// from base class MMdaAudioPlayerCallback

    /**
     * From MMdaAudioPlayerCallback.
     * Callback when an attempt to open and initialise an audio sample 
     * has completed.
     *
     * @param aError The status of the audio sample after initialisation.
     * @param aDuration The duration of the audio sample.
     */
    void MapcInitComplete(
        TInt aError,
        const TTimeIntervalMicroSeconds& aDuration );

    /**
     * From MMdaAudioPlayerCallback.
     * Callback when an attempt to playback an audio sample has completed.
     *
     * @param aError The status of playback.
     */
    void MapcPlayComplete( TInt aError );

private:    // Data

    // Pointer to the equalizer utility we use. 
    // This is either the equalizer utility provided by Music
    // Player Engine or iBackupEqualizerUtility.
    CAudioEqualizerUtility* iEqualizerUtility;          // not owned

    // Play utility used to open the 'null sound' file and
    // create iBackupEqualizerUtility
    CMdaAudioPlayerUtility* iPlayUtility;               // owned

    // Equalizer utility for the active playback utility
    CAudioEqualizerUtility* iAudioEqualizerUtility;     // owned

    // Backup equalizer utility instance used when the equalizer custom
    // interface cannot be created by using the active play utility
    CAudioEqualizerUtility* iBackupEqualizerUtility;   // owned

    // Current playback utility
    MMPXPlaybackUtility* iPlaybackUtility;  // not own, but need to close

    CEqualizerPresetsDialog* iEqDialog;     // not own

    // Should be launch equalizer dialog on open complete callback
    TBool iLaunchEqualizerOnOpenComplete;

    // iPlayUtility->OpenL() has been called but not yet completed
    TBool iOpenInProgress;
    
    // Used to syncronize backup equalizer creation and dialog launching
    CActiveSchedulerWait* iWait;
    };

#endif  // C_MPXEQUALIZERHELPER_H

// End of File
