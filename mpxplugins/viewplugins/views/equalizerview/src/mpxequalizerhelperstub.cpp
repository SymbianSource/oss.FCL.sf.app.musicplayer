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
* Description:  Helper class for managing equalizer utility instances
*                (stub version).
*
*/



// INCLUDE FILES
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include "mpxequalizerhelper.h"
#include "mpxlog.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXEqualizerHelper::CMPXEqualizerHelper()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::ConstructL()
    {
    MPX_FUNC("CMPXEqualizerHelper::ConstructL");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEqualizerHelper* CMPXEqualizerHelper::NewL()
    {
    MPX_FUNC("CMPXEqualizerHelper::NewL");
    CMPXEqualizerHelper* self =
        new ( ELeave ) CMPXEqualizerHelper();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXEqualizerHelper::~CMPXEqualizerHelper()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Initialize audio equalizer utility for current active playing track.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::InitializeEqualizerUtilityL()
    {
    MPX_FUNC("CMPXEqualizerHelper::InitializeEqualizerUtilityL");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Release audio equalizer utility.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::ReleaseEqualizerUtility()
    {
    MPX_FUNC("CMPXEqualizerHelper::ReleaseEqualizerUtility");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Get current preset ID.
// ---------------------------------------------------------------------------
//
TInt CMPXEqualizerHelper::GetPresetId( TInt& /* aPresetId */ )
    {
    MPX_FUNC("CMPXEqualizerHelper::GetPresetId");
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Initialize secondary audio equalizer utility for situations when the
// engine cannot provide one.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::InitializeBackupEqualizerUtilityL()
    {
    MPX_FUNC("CMPXEqualizerHelper::InitializeBackupEqualizerUtilityL");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Create secondary audio equalizer utility based on latest preset settings.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::CreateBackupEqualizerUtility()
    {
    MPX_FUNC("CMPXEqualizerHelper::CreateBackupEqualizerUtility");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Setup active equalizer utility for equalizer dialog.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::SetupActiveEqualizerUtility()
    {
    MPX_FUNC("CMPXEqualizerHelper::SetupActiveEqualizerUtility");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Launch equalizer dialog.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::DoLaunchEqualizerDialogL()
    {
    MPX_FUNC("CMPXEqualizerHelper::DoLaunchEqualizerDialogL");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC("CMPXEqualizerHelper::DoHandlePlaybackMessageL");
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EStateChanged:
                {
                TMPXPlaybackState state(
                    static_cast<TMPXPlaybackState>(
                        aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) ) );
                DoHandleStateChangedL( state );
                break;
                }
            default:
                {
                // pass
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::DoHandleStateChangedL(
    TMPXPlaybackState /* aState */ )
    {
    MPX_FUNC("CMPXEqualizerHelper::DoHandleStateChangedL");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From CMPXEqualizerView
// Launch equalizer dialog.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::LaunchEqualizerL()
    {
    MPX_FUNC("CMPXEqualizerHelper::LaunchEqualizerL");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    MPX_FUNC("CMPXEqualizerHelper::HandlePlaybackMessage");
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MEqualizerPresetActivationObserver
// Handle preset id activation from Equalizer dialog.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::HandlePresetActivation(
    TInt /* aActivatedPreset */ )
    {
    MPX_FUNC("CMPXEqualizerHelper::HandlePresetActivation");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From MMdaAudioPlayerCallback
// Callback when an attempt to open and initialise an audio sample
// has completed.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::MapcInitComplete(
    TInt /* aError */,
    const TTimeIntervalMicroSeconds& /* aDuration */ )
    {
    MPX_FUNC("CMPXEqualizerHelper::MapcInitComplete");
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From MMdaAudioPlayerCallback
// Callback when an attempt to playback an audio sample has completed.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::MapcPlayComplete(
    TInt /* aError */ )
    {
    MPX_FUNC("CMPXEqualizerHelper::MapcPlayComplete");
    // Do nothing
    }

//  End of File
