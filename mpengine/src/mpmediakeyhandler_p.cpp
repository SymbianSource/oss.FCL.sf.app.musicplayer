/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Music Player media key handler - private implementation.
*
*/

#include "mpmediakeyhandler_p.h"
#include "mpmediakeyhandler.h"
#include "mpmediakeyremconresponse.h"
#include "mpcommondefs.h"

//symbian header files
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

//mpx header files
#include <mpxplaybackutility.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxlog.h>

const TInt KFirstTimerExpiryInterval( 1 ); // Expire immediately
const TInt KTimerExpiryInterval( 1000000/6 );

// ---------------------------------------------------------------------------
// MpMediaKeyHandlerPrivate()
// ---------------------------------------------------------------------------
//
MpMediaKeyHandlerPrivate::MpMediaKeyHandlerPrivate( MpMediaKeyHandler *wrapper )
    : q_ptr( wrapper ),
      iInterfaceSelector(NULL),
      iResponseHandler(NULL),
      iTimer(NULL),
      iPlaybackUtility(NULL),
      iEnabled(ETrue)
{
}

// ---------------------------------------------------------------------------
// ~MpMediaKeyHandlerPrivate()
// ---------------------------------------------------------------------------
//
MpMediaKeyHandlerPrivate::~MpMediaKeyHandlerPrivate()
{
    if ( iPlaybackUtility ) {
        //TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
    }

    delete iResponseHandler;
    delete iInterfaceSelector;

    if ( iTimer ) {
        iTimer->Cancel();
        delete iTimer;
    }

}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::init()
{
    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        emit q_ptr->errorOccured(err);
    }

}

// ---------------------------------------------------------------------------
// setEnabled
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::setEnabled( bool enable )
{
    iEnabled = enable;
}

// ---------------------------------------------------------------------------
// From MRemConCoreApiTargetObserver
// A command has been received.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::MrccatoCommand(
    TRemConCoreApiOperationId aOperationId,
    TRemConCoreApiButtonAction aButtonAct )
{
    MPX_DEBUG3( "MpMediaKeyHandlerPrivate::MrccatoCommand(aOperationId=%d, aButtonAct=%d)",
        aOperationId, aButtonAct );

    switch ( aOperationId ) {
    case ERemConCoreApiPausePlayFunction:
        if ( aButtonAct == ERemConCoreApiButtonClick ) {
            SendCommand( EPbCmdPlayPause );
        }
        break;
    case ERemConCoreApiPlay:
        if ( aButtonAct == ERemConCoreApiButtonClick
             || aButtonAct == ERemConCoreApiButtonPress ) {
            SendCommand( EPbCmdPlay );
        }
        break;
    case ERemConCoreApiStop:
        if ( aButtonAct == ERemConCoreApiButtonClick
             || aButtonAct == ERemConCoreApiButtonPress ) {
            SendCommand( EPbCmdStop );
        }
        break;
    case ERemConCoreApiPause:
        if ( aButtonAct == ERemConCoreApiButtonClick
             || aButtonAct == ERemConCoreApiButtonPress ) {
            SendCommand( EPbCmdPause );
        }
        break;
    case ERemConCoreApiRewind:
        switch ( aButtonAct ) {
        case ERemConCoreApiButtonPress:
            SendCommand( EPbCmdStartSeekBackward );
            break;
        case ERemConCoreApiButtonRelease:
            SendCommand( EPbCmdStopSeeking );
            break;
        default:
            break;
        }
        break;
    case ERemConCoreApiFastForward:
        switch ( aButtonAct ) {
        case ERemConCoreApiButtonPress:
            SendCommand( EPbCmdStartSeekForward );
            break;
        case ERemConCoreApiButtonRelease:
            SendCommand( EPbCmdStopSeeking );
            break;
        default:
            break;
        }
        break;
    case ERemConCoreApiBackward:
        if ( aButtonAct == ERemConCoreApiButtonClick ) {
            SendCommand( EPbCmdPrevious );
        }
        break;
    case ERemConCoreApiForward:
        if ( aButtonAct == ERemConCoreApiButtonClick ) {
            SendCommand( EPbCmdNext );
        }
        break;
    case ERemConCoreApiVolumeUp:
        iTimer->Cancel();
        iIncreaseVol = ETrue;
        switch ( aButtonAct ) {
        case ERemConCoreApiButtonPress:
            //Start Timer
            iTimer->Start(
                KFirstTimerExpiryInterval,
                KTimerExpiryInterval,
                TCallBack( TimerCallback, this ) );
            break;
        case ERemConCoreApiButtonClick:
            SendCommand( EPbCmdIncreaseVolume );
            break;
        case ERemConCoreApiButtonRelease:
        default:
            break;
        }
        break;
    case ERemConCoreApiVolumeDown:
        iTimer->Cancel();
        iIncreaseVol = EFalse;
        switch ( aButtonAct ) {
        case ERemConCoreApiButtonPress:
            //Start Timer
            iTimer->Start(
                KFirstTimerExpiryInterval,
                KTimerExpiryInterval,
                TCallBack( TimerCallback, this ) );
            break;
        case ERemConCoreApiButtonClick:
            SendCommand( EPbCmdDecreaseVolume );
            break;
        case ERemConCoreApiButtonRelease:
        default:
            break;
        }
        break;
    default:
        break;
    }
    iResponseHandler->CompleteAnyKey( aOperationId );

}

// ---------------------------------------------------------------------------
// From MRemConCoreApiTargetObserver
// A 'play' command has been received.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::MrccatoPlay(
    TRemConCoreApiPlaybackSpeed aSpeed,
    TRemConCoreApiButtonAction aButtonAct )
{
    MPX_DEBUG2( "MpMediaKeyHandlerPrivate::MrccatoPlay(aButtonAct=%d)", aButtonAct );
    Q_UNUSED(aSpeed);

    if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
        ( aButtonAct == ERemConCoreApiButtonPress ) )
        {
        SendCommand( EPbCmdPlay );
        }
    iResponseHandler->CompleteAnyKey( ERemConCoreApiPlay );

}

// ---------------------------------------------------------------------------
// From MRemConCoreApiTargetObserver
// A 'tune function' command has been received.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::MrccatoTuneFunction(
    TBool aTwoPart,
    TUint aMajorChannel,
    TUint aMinorChannel,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aTwoPart);
    Q_UNUSED(aMajorChannel);
    Q_UNUSED(aMinorChannel);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiTuneFunction );

}

// ---------------------------------------------------------------------------
// From MRemConCoreApiTargetObserver
// A 'select disk function' has been received.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::MrccatoSelectDiskFunction(
    TUint aDisk,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aDisk);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiSelectDiskFunction );

}

// ---------------------------------------------------------------------------
// From MRemConCoreApiTargetObserver
// A 'select AV input function' has been received.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::MrccatoSelectAvInputFunction(
    TUint8 aAvInputSignalNumber,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aAvInputSignalNumber);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiSelectAvInputFunction );

}

// ---------------------------------------------------------------------------
// From MRemConCoreApiTargetObserver
// A 'select audio input function' has been received.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::MrccatoSelectAudioInputFunction(
    TUint8 aAudioInputSignalNumber,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aAudioInputSignalNumber);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiSelectAudioInputFunction );

}

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
{
    MPX_FUNC( "MpMediaKeyHandlerPrivate::DoHandlePropertyL" );
    MPX_DEBUG4( "MpMediaKeyHandlerPrivate::HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    if ( KErrNone == aError ) {
        switch ( aProperty  ) {
        case EPbPropertyVolume:
            q_ptr->emit volumeChanged(aValue);
            break;
        case EPbPropertyPosition:
        case EPbPropertyMaxVolume:
        case EPbPropertyMute:
        default:
            break;
        }
    }

}

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// HandleSubPlayerNamesL
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::HandleSubPlayerNamesL(
    TUid aPlayer,
    const MDesCArray* aSubPlayers,
    TBool aComplete,
    TInt aError )
{
    MPX_FUNC( "MpMediaKeyHandlerPrivate::HandleSubPlayerNamesL" );
    Q_UNUSED(aPlayer);
    Q_UNUSED(aSubPlayers);
    Q_UNUSED(aComplete);
    Q_UNUSED(aError);

}

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media properties.
// Notes: The client is responsible for delete the object of aMedia.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
{
    MPX_FUNC( "MpMediaKeyHandlerPrivate::DoHandleMediaL" );
    Q_UNUSED(aMedia);
    Q_UNUSED(aError);

}

// ---------------------------------------------------------------------------
// DoInitL()
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::DoInitL()
{
    // Register to remote control framework
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    CRemConCoreApiTarget *coreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    CleanupStack::PushL( coreTarget );
    iInterfaceSelector->OpenTargetL();
    iResponseHandler = MpMediaKeyRemConResponse::NewL( *coreTarget );
    CleanupStack::Pop(coreTarget);

    // Timer for implementing repeat
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid(MpCommon::KMusicPlayerUid) );
    //iPlaybackUtility->AddObserverL( *this );

    iEnabled = ETrue;
}

// ---------------------------------------------------------------------------
// Send command to playback utility.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::SendCommand( TMPXPlaybackCommand aCommandId )
{
    MPX_FUNC( "MpMediaKeyHandlerPrivate::SendCommand" );
    TRAP_IGNORE( DoSendCommandL( aCommandId ) );

}

// ---------------------------------------------------------------------------
// Send command to playback utility.
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::DoSendCommandL( TMPXPlaybackCommand aCommandId )
{
    MPX_FUNC( "MpMediaKeyHandlerPrivate::DoFilterAndSendCommandL" );

    if ( iEnabled )
    {
        TMPXPlaybackState playerState( iPlaybackUtility->StateL() );

        switch ( aCommandId ) {
        case EPbCmdPlayPause:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused 
                 || playerState == EPbStateStopped ) {
                iPlaybackUtility->CommandL( EPbCmdPlayPause );
            }
            break;
        case EPbCmdPlay:
            if ( playerState == EPbStateStopped || playerState == EPbStatePaused ) {
                iPlaybackUtility->CommandL( EPbCmdPlay );
            }
            break;
        case EPbCmdStop:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused ) {
                iPlaybackUtility->CommandL( EPbCmdStop );
            }
            break;
        case EPbCmdPause:
            if ( playerState == EPbStatePlaying ) {
                iPlaybackUtility->CommandL( EPbCmdPause );
            }
            break;
        case EPbCmdStartSeekBackward:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused ) {
                iPlaybackUtility->CommandL( EPbCmdStartSeekBackward );
            }
            break;
        case EPbCmdStartSeekForward:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused ) {
                iPlaybackUtility->CommandL( EPbCmdStartSeekForward );
            }
            break;
        case EPbCmdStopSeeking:
            if ( playerState == EPbStateSeekingBackward || playerState == EPbStateSeekingForward ) {
                iPlaybackUtility->CommandL( EPbCmdStopSeeking );
            }
            break;
        case EPbCmdPrevious:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused 
                 || playerState == EPbStateStopped ) {
                iPlaybackUtility->CommandL( EPbCmdPrevious );
            }
            break;
        case EPbCmdNext:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused 
                 || playerState == EPbStateStopped ) {
                iPlaybackUtility->CommandL( EPbCmdNext );
            }
            break;
        case EPbCmdIncreaseVolume:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused 
                 || playerState == EPbStateStopped ) {
                iPlaybackUtility->CommandL( EPbCmdIncreaseVolume );
            }
            break;
        case EPbCmdDecreaseVolume:
            if ( playerState == EPbStatePlaying || playerState == EPbStatePaused 
                 || playerState == EPbStateStopped ) {
                iPlaybackUtility->CommandL( EPbCmdDecreaseVolume );
            }
            break;
        default:
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Callback for timer
// ---------------------------------------------------------------------------
//
TInt MpMediaKeyHandlerPrivate::TimerCallback( TAny* aPtr )
{

    static_cast<MpMediaKeyHandlerPrivate*>( aPtr )->HandleRepeatEvent();

    return KErrNone;
}

// ---------------------------------------------------------------------------
// Handle repeat event
// ---------------------------------------------------------------------------
//
void MpMediaKeyHandlerPrivate::HandleRepeatEvent()
{

    if ( iIncreaseVol ) {
        SendCommand( EPbCmdIncreaseVolume );
    }
    else {
        SendCommand( EPbCmdDecreaseVolume );
    }

}

