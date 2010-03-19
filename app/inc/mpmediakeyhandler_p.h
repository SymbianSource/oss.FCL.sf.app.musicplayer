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

#ifndef MPMEDIAKEYHANDLER_P_H
#define MPMEDIAKEYHANDLER_P_H


#include <remconcoreapitargetobserver.h>
#include <mpxplaybackobserver.h>
#include <mpxplaybackframeworkdefs.h>

class CRemConInterfaceSelector;
class MMPXPlaybackUtility;
class MpMediaKeyHandler;
class MpMediaKeyRemConResponse;

class MpMediaKeyHandlerPrivate : public MRemConCoreApiTargetObserver,
                                 public MMPXPlaybackCallback
{
public:

    MpMediaKeyHandlerPrivate( MpMediaKeyHandler *wrapper );
    virtual ~MpMediaKeyHandlerPrivate();

    void init();

// From MRemConCoreApiTargetObserver

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

private:

    void DoInitL();
    void SendCommand( TMPXPlaybackCommand aCommandId );
    void DoSendCommandL( TMPXPlaybackCommand aCommandId );

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

private:

    MpMediaKeyHandler *q_ptr;

    CRemConInterfaceSelector    *iInterfaceSelector;   // owned
    MpMediaKeyRemConResponse    *iResponseHandler;     // owned
    CPeriodic                   *iTimer;               // owned

    MMPXPlaybackUtility         *iPlaybackUtility;

    TBool                       iIncreaseVol;

};

#endif // MPMEDIAKEYHANDLER_P_H
