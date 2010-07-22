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

#include "stub/inc/mpxplaybackutility.h"
#include "stub/inc/mpxplayermanager.h"
#include "stub/inc/mpxsource.h"
#include "mptrace.h"

MMPXPlaybackUtility* MMPXPlaybackUtility::singleton = NULL;

MMPXPlaybackUtility* MMPXPlaybackUtility::NewL(const TUid& /*aModeId*/,
                                            MMPXPlaybackObserver* /*aObs*/)
{
    MMPXPlaybackUtility* obj = new(ELeave) MMPXPlaybackUtility();
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
}

void MMPXPlaybackUtility::ConstructL()
{
    TX_ENTRY_ARGS("STUB")
    iMpxPlayerManager = new(ELeave) MMPXPlayerManager(); 
    iMpxSource = new(ELeave) MMPXSource();
    TX_EXIT_ARGS("STUB")
}

MMPXPlaybackUtility::~MMPXPlaybackUtility()
{
    TX_ENTRY_ARGS("STUB")    
}

MMPXPlaybackUtility::MMPXPlaybackUtility()
    : iMpxSource(0),
      iMpxPlayerManager(0),
      iPlaybackObserver(0),
      iPlaybackCallback(0),
      iState(EPbStateNotInitialised),
      iOneShot(0)
{
    TX_ENTRY_ARGS("STUB")
}

  
void MMPXPlaybackUtility::AddObserverL(MMPXPlaybackObserver& aPlaybackObserver)
{
    TX_ENTRY_ARGS("STUB")
    iPlaybackObserver = &aPlaybackObserver;
}

void MMPXPlaybackUtility::RemoveObserverL(MMPXPlaybackObserver& aPlaybackObserver)
{
    TX_ENTRY_ARGS("STUB")
    if(iPlaybackObserver == &aPlaybackObserver)
    {
        iPlaybackObserver = NULL;
    }
}

void MMPXPlaybackUtility::GetClientsL(RArray<TProcessId>& /*aClients*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlaybackUtility::InitL(const CMPXCollectionPlaylist& /*aPlaylist*/, TBool /*aPlay*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}
    
void MMPXPlaybackUtility::InitL(const TDesC& /*aUri*/, const TDesC8* /*aType*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}
    
void MMPXPlaybackUtility::InitL(RFile& /*aShareableFile*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlaybackUtility::Close()
{
    TX_ENTRY_ARGS("STUB")
    delete singleton;
    singleton = NULL;
    TX_EXIT_ARGS("STUB")
}

void MMPXPlaybackUtility::CancelRequest()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}

void MMPXPlaybackUtility::CommandL(TMPXPlaybackCommand /*aCmd*/, TInt /*aData*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlaybackUtility::CommandL(CMPXCommand& /*aCmd*/, MMPXPlaybackCallback* /*aCallback*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

TMPXPlaybackState MMPXPlaybackUtility::StateL() const
{
    TX_ENTRY_ARGS("STUB")
    return iState;
}
    
MMPXSource* MMPXPlaybackUtility::Source()
{
    TX_ENTRY_ARGS("STUB")
    return iMpxSource;
}

MMPXPlayerManager& MMPXPlaybackUtility::PlayerManager()
{
    TX_ENTRY_ARGS("STUB")
    return *iMpxPlayerManager;
}

void MMPXPlaybackUtility::SetL(TMPXPlaybackProperty /*aProperty*/, TInt /*aValue*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}

void MMPXPlaybackUtility::ValueL(MMPXPlaybackCallback& /*aCallback*/, TMPXPlaybackProperty /*aProperty*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlaybackUtility::PropertyL(MMPXPlaybackCallback& /*aCallback*/, TMPXPlaybackProperty /*aProperty*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

CDesCArray* MMPXPlaybackUtility::SupportedMimeTypes()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
    return 0;    
}

CDesCArray* MMPXPlaybackUtility::SupportedExtensions()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
    return 0;
}

CDesCArray* MMPXPlaybackUtility::SupportedSchemas()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
    return 0;
}

void MMPXPlaybackUtility::SetPriority( TInt /*aPriority*/ )
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}

void MMPXPlaybackUtility::AddSubscriptionL(const CMPXSubscription& /*aSubscription*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}

void MMPXPlaybackUtility::RemoveSubscriptionL(const CMPXSubscription& /*aSubscription*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}

void MMPXPlaybackUtility::ClearSubscriptionsL()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}
    
void MMPXPlaybackUtility::InitStreamingL(const TDesC& /*aUri*/, const TDesC8* /*aType*/, const TInt /*aAccessPoint*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}

void MMPXPlaybackUtility::InitStreamingL(RFile& /*aShareableFile*/, const TInt /*aAccessPoint*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
}

MMPXPlaybackUtility* MMPXPlaybackUtility::NewL(const TMPXCategory /*aCategory*/,
                                               const TUid& /*aModeId*/,
                                               MMPXPlaybackObserver* /*aObs*/)
{
    return 0;    
}

MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL(const TMPXCategory /*aCategory*/,
                                                   const TUid& /*aModeId*/ )
{
    return 0;    
}

MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL(const TUid& /*aModeId*/ )
{
    if(!singleton) {
        singleton = MMPXPlaybackUtility::NewL();
    }
    return singleton;
}

//end of file
