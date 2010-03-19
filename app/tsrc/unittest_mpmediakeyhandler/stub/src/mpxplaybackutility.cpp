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
* Description: mpxplaybackutility stub for testing MpNowPlayingBackEnd
*
*/


#include <mpxplaybackcommanddefs.h>
#include <e32debug.h>

#include "stub/inc/mpxplaybackutility.h"
#include "mpmediakeyhandler_p.h"


int gMMPXPlaybackUtilityCount = 0;
bool gMMPXPlaybackUtilityNewLLeave = false;
bool gMMPXPlaybackUtilityAddObserverLLeave = false;
bool gMMPXPlaybackUtilityRemoveObserverLLeave = false;
bool gMMPXPlaybackUtilityCommandLLeave = false;
bool gMMPXPlaybackUtilityStateLLeave = false;
TMPXPlaybackState gMMPXPlaybackUtilityState = EPbStateNotInitialised;


//static functions start

int MMPXPlaybackUtility::getCount()
{
    return gMMPXPlaybackUtilityCount;
}

void MMPXPlaybackUtility::setNewLLeave()
{
    gMMPXPlaybackUtilityNewLLeave = true;
}

void MMPXPlaybackUtility::NewLLeaveIfDesiredL()
{
    if (gMMPXPlaybackUtilityNewLLeave)
    {
        RDebug::Print(_L("MMPXPlaybackUtility::NewLLeaveIfDesiredL Leave"));
        gMMPXPlaybackUtilityNewLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXPlaybackUtility::setAddObserverLLeave()
{
    gMMPXPlaybackUtilityAddObserverLLeave = true;
}

void MMPXPlaybackUtility::AddObserverLLeaveIfDesiredL()
{
    if (gMMPXPlaybackUtilityAddObserverLLeave)
    {
        RDebug::Print(_L("MMPXPlaybackUtility::AddObserverLLeaveIfDesiredL Leave"));
        gMMPXPlaybackUtilityAddObserverLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXPlaybackUtility::setRemoveObserverLLeave()
{
    gMMPXPlaybackUtilityRemoveObserverLLeave = true;
}

void MMPXPlaybackUtility::RemoveObserverLLeaveIfDesiredL()
{
    if (gMMPXPlaybackUtilityRemoveObserverLLeave)
    {
        RDebug::Print(_L("MMPXPlaybackUtility::RemoveObserverLLeaveIfDesiredL Leave"));
        gMMPXPlaybackUtilityRemoveObserverLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXPlaybackUtility::setCommandLLeave()
{
    gMMPXPlaybackUtilityCommandLLeave = true;
}

void MMPXPlaybackUtility::CommandLLeaveIfDesiredL()
{
    if (gMMPXPlaybackUtilityCommandLLeave)
    {
        RDebug::Print(_L("MMPXPlaybackUtility::CommandLLeaveIfDesiredL Leave"));
        gMMPXPlaybackUtilityCommandLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXPlaybackUtility::setStateLLeave()
{
    gMMPXPlaybackUtilityStateLLeave = true;
}

void MMPXPlaybackUtility::StateLLeaveIfDesiredL()
{
    if (gMMPXPlaybackUtilityStateLLeave)
    {
        RDebug::Print(_L("MMPXPlaybackUtility::StateLLeaveIfDesiredL Leave"));
        gMMPXPlaybackUtilityStateLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXPlaybackUtility::setPlaying()
{
    gMMPXPlaybackUtilityState = EPbStatePlaying;
}

//static functions end

MMPXPlaybackUtility::MMPXPlaybackUtility()
    : CActive(EPriorityStandard),
      iObserver(NULL),
      iVolume(5),
      iVolumeUp(ETrue)
{
    gMMPXPlaybackUtilityState = EPbStateNotInitialised;
}

MMPXPlaybackUtility::~MMPXPlaybackUtility()
{
}

MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL(const TUid& /*aModeId*/)
{
    RDebug::Print(_L("stub MMPXPlaybackUtility::UtilityL"));
    MMPXPlaybackUtility::NewLLeaveIfDesiredL();
    gMMPXPlaybackUtilityCount++;
    static MMPXPlaybackUtility playbackUtilility;
    return &playbackUtilility;
}

void MMPXPlaybackUtility::Close()
{
    iObserver = NULL;
    gMMPXPlaybackUtilityCount--;
}

void MMPXPlaybackUtility::AddObserverL(MpMediaKeyHandlerPrivate& aObs)
{
    MMPXPlaybackUtility::AddObserverLLeaveIfDesiredL();
    iObserver = &aObs;
}

void MMPXPlaybackUtility::RemoveObserverL(MpMediaKeyHandlerPrivate& /*aObs*/)
{
    MMPXPlaybackUtility::RemoveObserverLLeaveIfDesiredL();
    iObserver = NULL;
}

void MMPXPlaybackUtility::CommandL(TMPXPlaybackCommand aCmd, TInt /*aData*/)
{
    RDebug::Print(_L(">>MMPXPlaybackUtility::CommandL"));
    
    MMPXPlaybackUtility::CommandLLeaveIfDesiredL();
    
    if (IsActive())
        User::Leave(KErrNotReady);
    else
    {
        if ((aCmd == EPbCmdIncreaseVolume) || (aCmd == EPbCmdDecreaseVolume))
        {
            if (aCmd == EPbCmdIncreaseVolume)
                iVolumeUp = ETrue;
            else
                iVolumeUp = EFalse;
            
            // stub should not be async
            /*TRequestStatus* status = &iStatus;
            User::RequestComplete(status, KErrNone);
            SetActive();*/
            
            RunL();
        }
    }
    
    RDebug::Print(_L("<<MMPXPlaybackUtility::CommandL"));
}

TMPXPlaybackState MMPXPlaybackUtility::StateL() const
{
    MMPXPlaybackUtility::StateLLeaveIfDesiredL();
    return gMMPXPlaybackUtilityState;
}

void MMPXPlaybackUtility::DoCancel()
{
}

void MMPXPlaybackUtility::RunL()
{
    RDebug::Print(_L(">>MMPXPlaybackUtility::RunL"));
    
    TInt err = KErrNone;
    
    if (iVolumeUp)
    {
        if (iVolume < 10)
            iVolume++;
        else
            err = KErrArgument;
    }
    else
    {
        if (iVolume > 0)
            iVolume--;
        else
            err = KErrArgument;
    }
    
    if (iObserver)
        iObserver->HandlePropertyL(EPbPropertyVolume, iVolume, err);
    
    RDebug::Print(_L("<<MMPXPlaybackUtility::RunL"));
}

//end of file
