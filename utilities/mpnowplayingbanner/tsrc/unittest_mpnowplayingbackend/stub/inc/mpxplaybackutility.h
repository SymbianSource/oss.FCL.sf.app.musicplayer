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
* Description: mpxplaybackutility stub for testing MpNowPlayingBackEnd
*
*/


#ifndef MMPXPLAYBACKUTILITY_H
#define MMPXPLAYBACKUTILITY_H

#include <mpxplaybackframeworkdefs.h>
#include <mpxcommonframeworkdefs.h>
#include <mpxplaybackmessage.h>
#include <badesca.h>
#include <mpxattribute.h>
#include <mpxcommand.h>
#include <mpxattributespecs.h>

class MMPXPlaybackObserver;
class MMPXPlaybackCallback;

class MMPXSource
    {
public:

    // Test utility functions
    MMPXSource();
    virtual ~MMPXSource();
    static void setMediaLeave(bool leave);
    static int getMediaCounter();
    static void resetMediaCounter();
    void sendHandleMediaL(bool title, bool uri, bool artist);
    void setObserver(MMPXPlaybackCallback* obs);

    // Stub functions
    void MediaL(const TArray<TMPXAttribute>& aAttrs, MMPXPlaybackCallback& aCallback);

public:

    MMPXPlaybackCallback*   iObserver;

    };

class MMPXPlaybackUtility
    {
public:

    // Test utility functions
    MMPXPlaybackUtility();
    virtual ~MMPXPlaybackUtility();
    static void setUtilityLeave(bool leave);
    static void setAddObserverLeave(bool leave);
    static void setRemoveObserverLeave(bool leave);
    static void setCommandLeave(bool leave);
    static void setStateLeave(bool leave);
    static int getCloseCounter();
    static void resetCloseCounter();
    static int getRemoveObserverCounter();
    static void resetRemoveObserverCounter();
    static void setSource(bool exist);
    static void setState(TMPXPlaybackState state);
    static TMPXPlaybackState getState();
    void sendHandlePlaybackMessage(TMPXPlaybackMessage::TEvent event);

    // Stub functions
    static MMPXPlaybackUtility* UtilityL(const TUid& aModeId = KPbModeDefault);
    void AddObserverL(MMPXPlaybackObserver& aObs);
    void RemoveObserverL(MMPXPlaybackObserver& aObs);
    void Close();
    void CommandL(TMPXPlaybackCommand aCmd, TInt aData=0);
    TMPXPlaybackState StateL() const;
    MMPXSource* Source();

public:

    MMPXPlaybackObserver*   iObserver;
    MMPXSource              iSource;    // concrete source object

    };

#endif      // MMPXPLAYBACKUTILITY_H

// End of File
