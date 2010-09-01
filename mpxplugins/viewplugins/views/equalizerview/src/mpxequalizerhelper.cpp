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



// INCLUDE FILES
#include <AudioEqualizerUtility.h>
#include <mdaaudiosampleplayer.h>
#include <centralrepository.h>
#include <textresolver.h>
#include <aknnotewrappers.h>
#include <akntoolbar.h>
#include <layoutmetadata.cdl.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxprivatecrkeys.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxuser.h>
#include "mpxequalizerhelper.h"
#include "mpxlog.h"
#include <aknclearer.h>


// CONSTANTS
// Null file path
_LIT( KMPXNullSoundPath, "\\system\\data\\nullsound.mp3" );
const TInt KEqualizerID = 0x101FFC76;
// Music setting
const TUid KCRUidMPXMPSettings = {0x101FFCDC};
const TUint32 KMPXMPEqPresetId = 0x00000001;

const TUid KCRUidOldMPSettings = {0x101F880C};
const TUint32 KOldMPEqPresetId = 0x00000009;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXEqualizerHelper::CMPXEqualizerHelper()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::ConstructL()
    {
    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );
    iWait = new( ELeave )CActiveSchedulerWait;
    InitializeBackupEqualizerUtilityL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEqualizerHelper* CMPXEqualizerHelper::NewL()
    {
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
    delete iAudioEqualizerUtility;
    delete iBackupEqualizerUtility;
    iEqualizerUtility = NULL;
    iEqDialog = NULL;

    if ( iPlayUtility )
        {
        if ( iPlaybackUtility )
            {
            TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
            }
        iPlayUtility->Close();
        }
    delete iPlayUtility;

    if ( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }
   	delete iWait;
    iWait = NULL;
    }

// ---------------------------------------------------------------------------
// Initialize audio equalizer utility for current active playing track.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::InitializeEqualizerUtilityL()
    {
    MPX_FUNC( "CMPXEqualizerHelper::InitializeEqualizerUtilityL" );

    ReleaseEqualizerUtility();

    //CCustomCommandUtility* customCmdUtility =
    //    CMPXPlaybackCCustomCommandUtility::NewCustomCommandUtilityL(
    //        *iPlaybackUtility );
    //CleanupStack::PushL( customCmdUtility );

    //iAudioEqualizerUtility =
    //    CAudioEqualizerUtility::NewL( customCmdUtility );
    //MPX_DEBUG2( "--> CAudioEqualizerUtility created ok (0x%08x)",
    //    iAudioEqualizerUtility );

    //CleanupStack::Pop( customCmdUtility );  // ownership is transferred

    SetupActiveEqualizerUtility();

    TInt ignore( KEqualizerPresetNone );
    TInt index = GetPresetId( ignore );
    if ( KErrNotFound != index )
        {
        iEqualizerUtility->ApplyPresetL( index );
        }
    }

// ---------------------------------------------------------------------------
// Release audio equalizer utility.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::ReleaseEqualizerUtility()
    {
    MPX_FUNC( "CMPXEqualizerHelper::ReleaseEqualizerUtility" );

    if ( iEqDialog )
        {
        // Recreate Backup Equalizer Utility to make sure it can get the
        // latest presets.
        if ( !iBackupEqualizerUtility )
            {
            CreateBackupEqualizerUtility();
            }

        MPX_DEBUG2( "--> iEqDialog->SetAudioEqualizerL(0x%08x)",
            iBackupEqualizerUtility );

        TRAP_IGNORE(
            iEqDialog->SetAudioEqualizerL( iBackupEqualizerUtility ) );
        }

    iEqualizerUtility = iBackupEqualizerUtility;

    if ( iAudioEqualizerUtility )
        {
        delete iAudioEqualizerUtility;
        iAudioEqualizerUtility = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Get current preset ID.
// ---------------------------------------------------------------------------
//
TInt CMPXEqualizerHelper::GetPresetId( TInt& aPresetId )
    {
    MPX_FUNC( "CMPXEqualizerHelper::GetPresetId" );

    // Get preset id from cenrep
    TInt presetId( KEqualizerPresetNone );
    TRAP_IGNORE(
        {
        CRepository* repository = CRepository::NewL( KCRUidMPXMPSettings );
        repository->Get( KMPXMPEqPresetId, presetId );
        delete repository;
        repository = NULL;
        } );

    TInt index( KErrNotFound );
    if ( iEqualizerUtility && presetId != KEqualizerPresetNone )
        {
        TArray<TEfAudioEqualizerUtilityPreset> presetArray =
            iEqualizerUtility->Presets();

        TBool found( EFalse );
        for ( TInt i = 0; i < presetArray.Count() && found == EFalse; i++ )
            {
            if ( presetArray[i].iPresetNameKey ==
                static_cast<TUint32>( presetId ) )
                {
                found = ETrue;
                index = i;
                aPresetId = presetId;
                }
            }
        }
    else
        {
        aPresetId = KEqualizerPresetNone;
        }

    MPX_DEBUG2( "--> Preset id get: %d", presetId );
    return index;
    }

// ---------------------------------------------------------------------------
// Initialize secondary audio equalizer utility for situations when the
// engine cannot provide one.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::InitializeBackupEqualizerUtilityL()
    {
    MPX_FUNC( "CMPXEqualizerHelper::InitializeBackupEqualizerUtilityL()" );

    if ( iOpenInProgress )
        {
        // OpenFileL already called
        return;
        }

    if ( !iPlayUtility )
        {
        iPlayUtility = CMdaAudioPlayerUtility::NewL(
            *this,
            EMdaPriorityMin,
            EMdaPriorityPreferenceNone );
        }

    TFileName nullSoundPath( KMPXNullSoundPath );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( nullSoundPath ) );
    iPlayUtility->OpenFileL( nullSoundPath );
    iOpenInProgress = ETrue;
    }

// ---------------------------------------------------------------------------
// Create secondary audio equalizer utility based on latest preset settings.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::CreateBackupEqualizerUtility()
    {
    if ( iPlayUtility )
        {
        delete iBackupEqualizerUtility;
        iBackupEqualizerUtility = NULL;
        TRAP_IGNORE( iBackupEqualizerUtility =
            CAudioEqualizerUtility::NewL( *iPlayUtility ) );
        }
    }

// ---------------------------------------------------------------------------
// Setup active equalizer utility for equalizer dialog.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::SetupActiveEqualizerUtility()
    {
    CAudioEqualizerUtility* oldUtility( iEqualizerUtility );

    // Try to use the real equalizer utility first
    if ( !iAudioEqualizerUtility && iBackupEqualizerUtility )
        {
        iEqualizerUtility = iBackupEqualizerUtility;
        }
    else
        {
        iEqualizerUtility = iAudioEqualizerUtility;
        }

    if ( iEqDialog && oldUtility != iEqualizerUtility )
        {
        MPX_DEBUG2( "--> iEqDialog->SetAudioEqualizerL(0x%08x)", iEqualizerUtility );
        TRAP_IGNORE( iEqDialog->SetAudioEqualizerL( iEqualizerUtility ) );
        }
    }

// ---------------------------------------------------------------------------
// Launch equalizer dialog.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::DoLaunchEqualizerDialogL()
    {
    MPX_FUNC( "CMPXEqualizerHelper::DoLaunchEqualizerDialogL()" );

    __ASSERT_DEBUG(
        iEqualizerUtility,
        User::Panic( _L( "CMPXEqualizerHelper" ), KErrArgument ) );

    TInt presetId( KEqualizerPresetNone );
    GetPresetId( presetId );

    CEqualizerPresetsDialog* dlg = CEqualizerPresetsDialog::NewL(
        iEqualizerUtility, presetId, *this );
    iEqDialog = dlg;
    
    CAknToolbar* toolbar =
        static_cast< CAknAppUi* >(CEikonEnv::Static()->AppUi())->CurrentFixedToolbar();
                     
    TBool wasToolbarVisible = toolbar->IsVisible();            

    if ( toolbar ) 
        {
        toolbar->HideItemsAndDrawOnlyBackground( ETrue );
        }       

    MPX_TRAPD( err, dlg->ExecuteLD() );
    iEqDialog = NULL;

    if ( toolbar && wasToolbarVisible ) 
        {
        toolbar->HideItemsAndDrawOnlyBackground( EFalse );
        }

    if ( err )
        {
        MPX_DEBUG2( "--> dlg->ExecuteLD() left with %d", err );
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXEqualizerHelper::DoHandlePlaybackMessageL" );
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
    TMPXPlaybackState aState )
    {
    switch ( aState )
        {
        case EPbStateNotInitialised:
        case EPbStateStopped:
        case EPbStatePaused:
        case EPbStateShuttingDown:
            {
            // Need to release Equalizer utility when audio is stopped.
            // It's because of the limitation in adaptation layer.
            ReleaseEqualizerUtility();
            break;
            }
        case EPbStatePlaying:
            {
            // Need to initialize Equalizer utility when audio is started
            // playing. It's because of the limitation in adaptation layer.
            InitializeEqualizerUtilityL();
            break;
            }
        default:
            {
            // pass
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CMPXEqualizerView
// Launch equalizer dialog.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::LaunchEqualizerL()
    {
    MPX_FUNC( "CMPXEqualizerHelper::LaunchEqualizerL" );

    if ( iLaunchEqualizerOnOpenComplete )
        {
        // Waiting for open complete callback, cannot do anything now
        return;
        }

    if ( !iPlayUtility || !iBackupEqualizerUtility )
        {
        // Backup equalizer does not exist, must initialize first
        iLaunchEqualizerOnOpenComplete = ETrue;
        InitializeBackupEqualizerUtilityL();
        if( iWait )
        	{
        	iWait->Start();
        	}
        }
    else
        {
        // Recreate Backup Equalizer Utility to make sure it can get the
        // latest presets.
        CreateBackupEqualizerUtility();

        // Setup active equalizer utility and launch the dialog
        SetupActiveEqualizerUtility();
        DoLaunchEqualizerDialogL();
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
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
    TInt aActivatedPreset )
    {
    MPX_DEBUG2("CMPXEqualizerHelper::HandlePresetActivation %i", aActivatedPreset);
    MPX_FUNC( "CMPXEqualizerHelper::HandlePresetActivation" );

    // Store in CenRep file
    TRAP_IGNORE(
        {
        CRepository* repository = CRepository::NewL( KCRUidMPXMPSettings );
        TInt err = repository->Set( KMPXMPEqPresetId, aActivatedPreset );
        delete repository;
        repository = NULL;

        repository = CRepository::NewL( KCRUidOldMPSettings );
        err = repository->Set( KOldMPEqPresetId, aActivatedPreset );
        delete repository;
        repository = NULL;

        } );

    // Notify playback framework of the change.
    //
    TRAP_IGNORE( iPlaybackUtility->CommandL( EPbApplyEffect, KEqualizerID ) );
    }

// ---------------------------------------------------------------------------
// From MMdaAudioPlayerCallback
// Callback when an attempt to open and initialise an audio sample
// has completed.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::MapcInitComplete(
    TInt aError,
    const TTimeIntervalMicroSeconds& /* aDuration */ )
    {
    MPX_DEBUG3( "CMPXEqualizerHelper::MapcInitComplete(%d) -- iLaunchEqualizerOnOpenComplete=%d", aError, iLaunchEqualizerOnOpenComplete );

    iOpenInProgress = EFalse;
    if ( !aError )
        {
        // Recreate Backup Equalizer Utility to make sure it can get the
        // latest presets.
        CreateBackupEqualizerUtility();

        if ( iBackupEqualizerUtility )
            {
            MPX_DEBUG2( "--> iBackupEqualizerUtility=0x%08x, NewL ok", iBackupEqualizerUtility );

            // Setup active equalizer utility
            SetupActiveEqualizerUtility();

            if ( iLaunchEqualizerOnOpenComplete )
                {
                TRAP_IGNORE( DoLaunchEqualizerDialogL() );
                }
            }
        }
    else
        {
        TRAP_IGNORE(
            {
            // TextResolver instance for error resolving.
            CTextResolver* textresolver = CTextResolver::NewLC();
            // Resolve the error text
            const TDesC& text =
                textresolver->ResolveErrorString( aError );

            CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
            dlg->ExecuteLD( text );
            CleanupStack::PopAndDestroy( textresolver );
            } );
        }
    iLaunchEqualizerOnOpenComplete = EFalse;
    if( iWait && iWait->IsStarted() )
		{
    	iWait->AsyncStop();
    	}
    }

// ---------------------------------------------------------------------------
// From MMdaAudioPlayerCallback
// Callback when an attempt to playback an audio sample has completed.
// ---------------------------------------------------------------------------
//
void CMPXEqualizerHelper::MapcPlayComplete(
    TInt /* aError */ )
    {
    // Do nothing
    }

//  End of File
