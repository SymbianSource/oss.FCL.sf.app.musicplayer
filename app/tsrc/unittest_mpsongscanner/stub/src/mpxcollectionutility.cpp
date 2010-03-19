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
* Description: mpxcollectionutility stub for testing MpNowPlayingBackEnd
*
*/


#include <mpxmedia.h>
#include <mpxmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacollectiondetaildefs.h>

#include <e32debug.h>

#include "stub/inc/mpxcollectionutility.h"
#include "mpsongscanner_p.h"


int gMMPXCollectionCount = 0;
int gMMPXCollectionUtilityCount = 0;
bool gMMPXCollectionNewLLeave = false;
bool gMMPXCollectionOpenLLeave = false;
bool gMMPXCollectionMediaLLeave = false;
bool gMMPXCollectionUtilityNewLLeave = false;
bool gMMPXCollectionUtilityCollectionIDLLeave = false;


//static functions start

void MMPXCollection::setOpenLLeave()
{
    gMMPXCollectionOpenLLeave = true;
}

void MMPXCollection::OpenLLeaveIfDesiredL()
{
    if (gMMPXCollectionOpenLLeave)
    {
        RDebug::Print(_L("MMPXCollection::OpenLLeaveIfDesiredL Leave"));
        gMMPXCollectionOpenLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXCollection::setMediaLLeave()
{
    gMMPXCollectionMediaLLeave = true;
}

void MMPXCollection::MediaLLeaveIfDesiredL()
{
    if (gMMPXCollectionMediaLLeave)
    {
        RDebug::Print(_L("MMPXCollection::MediaLLeaveIfDesiredL Leave"));
        gMMPXCollectionMediaLLeave = false;
        User::Leave(KErrGeneral);
    }
}

int MMPXCollectionUtility::getCount()
{
    return gMMPXCollectionUtilityCount;
}

void MMPXCollectionUtility::setNewLLeave()
{
    gMMPXCollectionUtilityNewLLeave = true;
}

void MMPXCollectionUtility::NewLLeaveIfDesiredL()
{
    if (gMMPXCollectionUtilityNewLLeave)
    {
        RDebug::Print(_L("MMPXCollectionUtility::NewLLeaveIfDesiredL Leave"));
        gMMPXCollectionUtilityNewLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXCollectionUtility::setCollectionIDLLeave()
{
    gMMPXCollectionUtilityCollectionIDLLeave = true;
}

void MMPXCollectionUtility::CollectionIDLLeaveIfDesiredL()
{
    if (gMMPXCollectionUtilityCollectionIDLLeave)
    {
        RDebug::Print(_L("MMPXCollectionUtility::CollectionIDLLeaveIfDesiredL Leave"));
        gMMPXCollectionUtilityCollectionIDLLeave = false;
        User::Leave(KErrGeneral);
    }
}

//static functions end

MMPXCollection::MMPXCollection()
{
    RDebug::Print(_L("stub MMPXCollection::MMPXCollection"));
}

MMPXCollection::~MMPXCollection()
{
}
    
void MMPXCollection::OpenL(TMPXOpenMode /*aMode*/)
{
    MMPXCollection::OpenLLeaveIfDesiredL();
}

void MMPXCollection::MediaL(const CMPXCollectionPath& /*aPath*/, const TArray<TMPXAttribute>& /*aAttrs*/, CMPXAttributeSpecs* /*aSpecs*/, CMPXFilter* /*aFilter*/)
{
    MMPXCollection::MediaLLeaveIfDesiredL();
}
    
MMPXCollectionUtility::MMPXCollectionUtility()
{
}

MMPXCollectionUtility::~MMPXCollectionUtility()
{
}

MMPXCollectionUtility* MMPXCollectionUtility::NewL(MpSongScannerPrivate* aObs)
{
    RDebug::Print(_L("stub MMPXCollectionUtility::NewL"));
    MMPXCollectionUtility::NewLLeaveIfDesiredL();
    gMMPXCollectionUtilityCount++;
    static MMPXCollectionUtility collectionUtilility;
    collectionUtilility.iObserver = aObs;
    return &collectionUtilility;
}

void MMPXCollectionUtility::Close()
{
    iObserver = NULL;
    gMMPXCollectionUtilityCount--;
}

TUid MMPXCollectionUtility::CollectionIDL(const TArray<TUid>& /*aUids*/)
{
    RDebug::Print(_L("MMPXCollectionUtility::CancelScanL"));
    MMPXCollectionUtility::CollectionIDLLeaveIfDesiredL();
    return TUid::Uid(0);
}

MMPXCollection& MMPXCollectionUtility::Collection()
{
    RDebug::Print(_L("MMPXCollectionUtility::CancelScanL"));
    return iCollection;
}

TInt MMPXCollectionUtility::SendRefreshStartMessage()
{
    RDebug::Print(_L(">>MMPXCollectionUtility::SendRefreshStartMessage"));
    
    CMPXMessage* mpxMessage = NULL;
    TInt err = KErrNone;

    TRAP(err,
        {
            mpxMessage = CMPXMessage::NewL();
            CleanupStack::PushL(mpxMessage);
            TMPXMessageId id = static_cast<TMPXMessageId>(KMPXMessageGeneral);
            mpxMessage->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, id);
            mpxMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent);
            mpxMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType, EMcMsgRefreshStart);
            CleanupStack::Pop(mpxMessage);
        });
    
        
    if (err != KErrNone)
        {
        RDebug::Print(_L("err = %d"), err);
        CleanupStack::PopAndDestroy(mpxMessage);
        return err;
        }

    if (iObserver)
        iObserver->HandleCollectionMessage(mpxMessage, err);
    
    delete mpxMessage;
    
    RDebug::Print(_L("<<MMPXCollectionUtility::SendRefreshStartMessage"));
    return KErrNone;
}

TInt MMPXCollectionUtility::SendRefreshEndMessage()
{
    RDebug::Print(_L(">>MMPXCollectionUtility::SendRefreshEndMessage"));
    
    CMPXMessage* mpxMessage = NULL;
    TInt err = KErrNone;

    TRAP(err,
        {
            mpxMessage = CMPXMessage::NewL();
            CleanupStack::PushL(mpxMessage);
            TMPXMessageId id = static_cast<TMPXMessageId>(KMPXMessageGeneral);
            mpxMessage->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, id);
            mpxMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent);
            mpxMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType, EMcMsgRefreshEnd);
            CleanupStack::Pop(mpxMessage);
        });
        
    if (err != KErrNone)
        {
        RDebug::Print(_L("err = %d"), err);
        CleanupStack::PopAndDestroy(mpxMessage);
        return err;
        }

    if (iObserver)
        iObserver->HandleCollectionMessage(mpxMessage, err);
    
    delete mpxMessage;
    
    RDebug::Print(_L("<<MMPXCollectionUtility::SendRefreshEndMessage"));
    return KErrNone;
}

TInt MMPXCollectionUtility::SendDiskInsertedMessage()
{
    RDebug::Print(_L(">>MMPXCollectionUtility::SendDiskInsertedMessage"));
    
    CMPXMessage* mpxMessage = NULL;
    TInt err = KErrNone;

    TRAP(err,
        {
            mpxMessage = CMPXMessage::NewL();
            CleanupStack::PushL(mpxMessage);
            TMPXMessageId id = static_cast<TMPXMessageId>(KMPXMessageGeneral);
            mpxMessage->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, id);
            mpxMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent, TMPXCollectionMessage::EBroadcastEvent);
            mpxMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType, EMcMsgDiskInserted);
            CleanupStack::Pop(mpxMessage);
        });
        
    if (err != KErrNone)
        {
        RDebug::Print(_L("err = %d"), err);
        CleanupStack::PopAndDestroy(mpxMessage);
        return err;
        }

    if (iObserver)
        iObserver->HandleCollectionMessage(mpxMessage, err);
    
    delete mpxMessage;    
    
    RDebug::Print(_L("<<MMPXCollectionUtility::SendDiskInsertedMessage"));
    return KErrNone;
}

TInt MMPXCollectionUtility::SendItemChangedMessage()
{
    RDebug::Print(_L(">>MMPXCollectionUtility::SendItemChangedMessage"));
    
    CMPXMessage* mpxMessage = NULL;
    TInt err = KErrNone;

    TRAP(err,
        {
            mpxMessage = CMPXMessage::NewL();
            CleanupStack::PushL(mpxMessage);
            TMPXMessageId id = static_cast<TMPXMessageId>(KMPXMessageIdItemChanged);
            mpxMessage->SetTObjectValueL<TMPXMessageId>(KMPXMessageGeneralId, id);
            mpxMessage->SetTObjectValueL<TMPXChangeEventType>(KMPXMessageChangeEventType, EMPXItemInserted);
            mpxMessage->SetTObjectValueL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory, EMPXSong);
            CleanupStack::Pop(mpxMessage);
        });
        
    if (err != KErrNone)
        {
        RDebug::Print(_L("err = %d"), err);
        CleanupStack::PopAndDestroy(mpxMessage);
        return err;
        }

    if (iObserver)
        iObserver->HandleCollectionMessage(mpxMessage, err);
    
    delete mpxMessage;    
    
    RDebug::Print(_L("<<MMPXCollectionUtility::SendItemChangedMessage"));
    return KErrNone;
}

TInt MMPXCollectionUtility::SendDbCreatedMedia()
{
    RDebug::Print(_L(">>MMPXCollectionUtility::SendDbCreatedMedia"));
    
    CMPXMedia *mpxMedia = NULL;
    TInt err = KErrNone;
    
    TRAP(err,
        {
            mpxMedia = CMPXMedia::NewL();
            CleanupStack::PushL(mpxMedia);
            mpxMedia->SetTObjectValueL<TBool>(KMPXMediaColDetailDBCreated, ETrue);
            CleanupStack::Pop(mpxMedia);
        });
    
    if (err != KErrNone)
        {
        RDebug::Print(_L("err = %d"), err);
        CleanupStack::PopAndDestroy(mpxMedia);
        return err;
        }
    
    if (iObserver)
        TRAP(err, iObserver->HandleCollectionMediaL(*mpxMedia, err));
    
    delete mpxMedia;
    RDebug::Print(_L("<<MMPXCollectionUtility::SendDbCreatedMedia"));
    return err;
}

TInt MMPXCollectionUtility::SendNotDbCreatedMedia()
{
    RDebug::Print(_L(">>MMPXCollectionUtility::SendNotDbCreatedMedia"));
    
    CMPXMedia *mpxMedia = NULL;
    TInt err = KErrNone;
    
    TRAP(err,
        {
            mpxMedia = CMPXMedia::NewL();
            CleanupStack::PushL(mpxMedia);
            mpxMedia->SetTObjectValueL<TBool>(KMPXMediaColDetailDBCreated, EFalse);
            CleanupStack::Pop(mpxMedia);
        });
    
    if (err != KErrNone)
        {
        RDebug::Print(_L("err = %d"), err);
        CleanupStack::PopAndDestroy(mpxMedia);
        return err;
        }
    
    if (iObserver)
        TRAP(err, iObserver->HandleCollectionMediaL(*mpxMedia, err));
    
    delete mpxMedia;
    RDebug::Print(_L("<<MMPXCollectionUtility::SendNotDbCreatedMedia"));
    return err;
}

//end of file
