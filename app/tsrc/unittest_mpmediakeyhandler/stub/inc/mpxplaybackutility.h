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
* Description: mpxplaybackutility stub for testing mpmediakeyhandler
*
*/


#ifndef MMPXPLAYBACKUTILITY_H
#define MMPXPLAYBACKUTILITY_H


#include <mpxplaybackframeworkdefs.h>   // KPbModeDefault, TMPXPlaybackCommand
#include <e32base.h>                    // CBase


class MpMediaKeyHandlerPrivate;

class MMPXPlaybackUtility : public CActive
    {
    
public:
    
    static int getCount();
    static void setNewLLeave();
    static void NewLLeaveIfDesiredL();
    static void setAddObserverLLeave();
    static void AddObserverLLeaveIfDesiredL();
    static void setRemoveObserverLLeave();
    static void RemoveObserverLLeaveIfDesiredL();
    static void setCommandLLeave();
    static void CommandLLeaveIfDesiredL();
    static void setStateLLeave();
    static void StateLLeaveIfDesiredL();
    static void setPlaying();
    
public:

    static MMPXPlaybackUtility* UtilityL(const TUid& aModeId = KPbModeDefault);
    virtual ~MMPXPlaybackUtility();

    void Close();
    void AddObserverL(MpMediaKeyHandlerPrivate& aObs);
    void RemoveObserverL(MpMediaKeyHandlerPrivate& aObs);
    void CommandL(TMPXPlaybackCommand aCmd, TInt aData = 0);
    
    TMPXPlaybackState StateL() const;    

protected:

    // from CActive
    virtual void DoCancel();
    virtual void RunL();
    
private:
    
    MMPXPlaybackUtility();
    
private:

    MpMediaKeyHandlerPrivate*   iObserver;
    TInt                        iVolume;
    TBool                       iVolumeUp;
    
    };

#endif      // MMPXPLAYBACKUTILITY_H

// End of File
