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
* Description: mpxplaybackutility stub for testing mpmpxframeworkwrapper
*
*/


#ifndef MMPXPLAYBACKUTILITY_H
#define MMPXPLAYBACKUTILITY_H

#include "stub/inc/mpxplayermanager.h"
#include "stub/inc/mpxsource.h"
#include <mpxplaybackframeworkdefs.h>
#include <mpxcommand.h>
#include <f32file.h>
#include <BADESCA.H>

//Forward declarations
class CMPXCollectionPlaylist;
class MMPXPlaybackObserver;  
class MMPXPlaybackCallback;
class CAsyncCallBack;
class CMPXSubscription;

class MMPXPlaybackUtility 
{
public: 
    ~MMPXPlaybackUtility();
    
protected:
    MMPXPlaybackUtility();
    void ConstructL();

public: // MMPXPlaybackUtility's overridden (stubbed by us) functions
    static MMPXPlaybackUtility* NewL(const TUid& aModeId = KPbModeDefault,
                                     MMPXPlaybackObserver* aObs=NULL);
    
    static MMPXPlaybackUtility* NewL(const TMPXCategory aCategory,
                                     const TUid& aModeId = KPbModeDefault,
                                     MMPXPlaybackObserver* aObs=NULL);
    static MMPXPlaybackUtility* UtilityL(const TUid& aModeId = KPbModeDefault );
    static MMPXPlaybackUtility* UtilityL(const TMPXCategory aCategory,
                                  const TUid& aModeId = KPbModeDefault );    
    
public: // MMPXPlaybackUtility stubbed pure virtuals
    void AddObserverL(MMPXPlaybackObserver& aPlaybackObserver);
    void RemoveObserverL(MMPXPlaybackObserver& aPlaybackObserver);
    void GetClientsL(RArray<TProcessId>& aClients);
    void InitL(const CMPXCollectionPlaylist& aPlaylist, TBool aPlay=ETrue);
    void InitL(const TDesC& aUri, const TDesC8* aType=NULL);
    void InitL(RFile& aShareableFile);
    void Close();
    void CancelRequest();
    void CommandL(TMPXPlaybackCommand aCmd, TInt aData=0);
    void CommandL(CMPXCommand& aCmd, MMPXPlaybackCallback* aCallback=NULL);
    TMPXPlaybackState StateL() const;
    MMPXSource* Source();
    MMPXPlayerManager& PlayerManager();
    void SetL(TMPXPlaybackProperty aProperty, TInt aValue);
    void ValueL(MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty);
    void PropertyL(MMPXPlaybackCallback& aCallback, TMPXPlaybackProperty aProperty);
    CDesCArray* SupportedMimeTypes();
    CDesCArray* SupportedExtensions();
    CDesCArray* SupportedSchemas();
    void SetPriority( TInt aPriority );
    void AddSubscriptionL(const CMPXSubscription& aSubscription);
    void RemoveSubscriptionL(const CMPXSubscription& aSubscription);
    void ClearSubscriptionsL();
    void InitStreamingL(const TDesC& aUri, const TDesC8* aType, const TInt aAccessPoint);
    void InitStreamingL(RFile& aShareableFile, const TInt aAccessPoint);

private:
    MMPXSource*             iMpxSource;
    MMPXPlayerManager*      iMpxPlayerManager;
    MMPXPlaybackObserver*   iPlaybackObserver;
    MMPXPlaybackCallback*   iPlaybackCallback;
    TMPXPlaybackState       iState;
    CAsyncCallBack*         iOneShot;

private:
    static MMPXPlaybackUtility* singleton;
};

#endif      // MMPXPLAYBACKUTILITY_H

