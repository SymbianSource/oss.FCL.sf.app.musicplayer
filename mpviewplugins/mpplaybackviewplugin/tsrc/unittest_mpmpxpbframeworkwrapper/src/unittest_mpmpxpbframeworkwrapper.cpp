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
* Description: Unit test for mpmpxpbframeworkwrapper
*
*/

#include <QSignalSpy>
#include <hbapplication.h>
#include <hbmainwindow.h>

#include <mpxmedia.h>
#include <mpxcollectionplaylist.h>



#include "unittest_mpmpxpbframeworkwrapper.h"
#include "stub/inc/mpplaybackdata.h"
#include "stub/inc/mpxcollectionutility.h"
#include "stub/inc/mpxplaybackutility.h"


// Do this so we can access all member variables.
#define private public
#include "mpmpxpbframeworkwrapper.h"
#include "mpmpxpbframeworkwrapper_p.h"
#undef private

//This so we can test private functions
#include "../../src/mpmpxpbframeworkwrapper.cpp"
#include "../../src/mpmpxpbframeworkwrapper_p.cpp"
//Test data
struct TTestAttrs
    {
    const wchar_t* GeneralTitle;
    const wchar_t* MusicArtist;
    const TInt GeneralCount;
    const wchar_t* MusicAlbumArtFileName;
    const wchar_t* MusicAlbum;
    const wchar_t* MusicUri;
    };
    
const TTestAttrs KAllSongsTestData[] =
    {
     { L"Title 1", L"Artist 1", 1, L"e:\\data\\images\\art1.jpg", L"Album 1", L"Uri 1"}
    ,{ L"Title 2", L"Artist 2", 1, L"e:\\data\\images\\art2.jpg", L"Album 2", L"Uri 2"}  
    };

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpMpxPbFrameworkWrapper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpmpxpbframeworkwrapper.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpMpxPbFrameworkWrapper::TestMpMpxPbFrameworkWrapper()
    : mTest(0),
      iMediaTestData(0)
{
}

TestMpMpxPbFrameworkWrapper::~TestMpMpxPbFrameworkWrapper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxPbFrameworkWrapper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxPbFrameworkWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxPbFrameworkWrapper::init()
{
    
    mTest = new MpMpxPbFrameworkWrapper();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMpxPbFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
 */
void TestMpMpxPbFrameworkWrapper::testConstructor()
{
    QVERIFY(mTestPrivate->iCollectionUtility != 0);
    QVERIFY(mTestPrivate->iPlaybackUtility != 0);
    QVERIFY(mTestPrivate->iPlaybackData != 0);
}

/*!
 Tests playbackData
 */
void TestMpMpxPbFrameworkWrapper::testPlaybackData()
{
    MpPlaybackData *playbackdata;
    playbackdata = mTest->playbackData();
    QCOMPARE(mTestPrivate->iPlaybackData, playbackdata);
}

/*!
 Tests stop.
 */
void TestMpMpxPbFrameworkWrapper::testStop()
{
    mTest->stop();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdStop);
}

/*!
 Tests playPause.
 */
void TestMpMpxPbFrameworkWrapper::testPlayPause()
{
    mTest->playPause();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdPlayPause);
}

/*!
 Tests skipForward.
 */
void TestMpMpxPbFrameworkWrapper::testSkipForward()
{
    mTest->skipForward();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdNext);
}

/*!
 Tests skipBackward
 */
void TestMpMpxPbFrameworkWrapper::testSkipBackward()
{
    mTest->skipBackward();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCmd, EPbCmdPrevious);
}

/*!
 Tests setPosition
 */
void TestMpMpxPbFrameworkWrapper::testSetPosition()
{
    mTest->setPosition(100);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, 100);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyPosition);
    mTest->setPosition(50);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, 50);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyPosition);
    mTest->setPosition(700);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, 700);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyPosition);    
}

/*!
 Tests setShuffle
 */
void TestMpMpxPbFrameworkWrapper::testSetShuffle()
{
    mTest->setShuffle(true);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRandomMode);
    mTest->setShuffle(false);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iValue, TBool(EFalse));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRandomMode);
}

/*!
 Tests setRepeat
 */
void TestMpMpxPbFrameworkWrapper::testSetRepeat()
{
    mTest->setRepeat(true);
    QCOMPARE(TMPXPlaybackRepeatMode(mTestPrivate->iPlaybackUtility->iValue), EPbRepeatAll);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRepeatMode);
    mTest->setRepeat(false);
    QCOMPARE(TMPXPlaybackRepeatMode(mTestPrivate->iPlaybackUtility->iValue), EPbRepeatOff);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyRepeatMode);
}

/*!
 Tests handleProperty
 */
void TestMpMpxPbFrameworkWrapper::testHandlePlaybackMessage()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(testMessage);
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral);
    
    //Update State
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EStateChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,0);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,0);
    mTestPrivate->iPlaybackUtility->iState = EPbStatePlaying;
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Playing);
    
    //Initialize complete
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EInitializeComplete);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,0);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,1);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandGeneralId),KMPXCommandIdPlaybackGeneral);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType),TInt(EPbCmdPlay));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandGeneralDoSync),TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandPlaybackGeneralNoBuffer),TBool(ETrue));

    //Property change
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EPropertyChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,EPbPropertyPosition);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,50);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mPosition, 50);
    
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,EPbPropertyDuration);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,100);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mDuration, 100);
    
    //Media change
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EMediaChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,0);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,0);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iAttrs->Count(), 5);
}

/*!
 Tests handleProperty
 */
void TestMpMpxPbFrameworkWrapper::testHandleProperty()
{
    for(TInt i = 0; i < 10; i++){
        mTestPrivate->HandlePropertyL(EPbPropertyPosition, i, KErrNone);
        mTestPrivate->HandlePropertyL(EPbPropertyDuration, i+1, KErrNone);
        QCOMPARE(mTestPrivate->iPlaybackData->mPosition, i);
        QCOMPARE(mTestPrivate->iPlaybackData->mDuration, i+1);       
    }
    mTestPrivate->iPlaybackData->mPosition = 100;
    mTestPrivate->iPlaybackData->mDuration = 100;
    mTestPrivate->HandlePropertyL(EPbPropertyPosition, 10, KErrNotFound);
    mTestPrivate->HandlePropertyL(EPbPropertyDuration, 10, KErrNotFound);
    
    QCOMPARE(mTestPrivate->iPlaybackData->mPosition, 100);
    QCOMPARE(mTestPrivate->iPlaybackData->mDuration, 100);
}

/*!
 Tests handleMedia
 */
void TestMpMpxPbFrameworkWrapper::testHandleMedia()
{
    loadTestData(0);
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mMediaReady, TBool(ETrue));
    mTestPrivate->iPlaybackData->mMediaReady = EFalse;
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mMediaReady, TBool(EFalse));
    
    loadTestData(1);
    mTestPrivate->iPlaybackData->mMediaReady = EFalse;
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE(mTestPrivate->iPlaybackData->mMediaReady, TBool(ETrue));
    const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].GeneralTitle));
    QCOMPARE(mTestPrivate->iPlaybackData->title(), QString::fromUtf16( title.Ptr(), title.Length() ));
    const TDesC& artist = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicArtist));
    QCOMPARE(mTestPrivate->iPlaybackData->artist(), QString::fromUtf16( artist.Ptr(), artist.Length() ));
    const TDesC& album = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicAlbum));
    QCOMPARE(mTestPrivate->iPlaybackData->album(), QString::fromUtf16( album.Ptr(), album.Length() ));
    const TDesC& albumArt = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicAlbumArtFileName));
    QCOMPARE(mTestPrivate->iPlaybackData->currentAAUri, QString::fromUtf16( albumArt.Ptr(), albumArt.Length() ));
    const TDesC& albumUri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[1].MusicUri));
    QCOMPARE(mTestPrivate->iPlaybackData->currentUri, QString::fromUtf16( albumUri.Ptr(), albumUri.Length() ));
}

/*!
 Tests retrieveSong
 */
void TestMpMpxPbFrameworkWrapper::testRetrieveSongDetails()
{
    mTestPrivate->RetrieveSongDetailsL();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iAttrs->Count(), 5);
}

/*!
 Tests updateState
 */
void TestMpMpxPbFrameworkWrapper::testUpdateState()
{
    mTestPrivate->iPlaybackUtility->iState = EPbStatePlaying;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Playing);
    mTestPrivate->iPlaybackUtility->iState = EPbStatePaused;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Paused);
    mTestPrivate->iPlaybackUtility->iState = EPbStatePlaying;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Playing);
    mTestPrivate->iPlaybackUtility->iState = EPbStateStopped;
    mTestPrivate->UpdateStateL();
    QCOMPARE(mTestPrivate->iPlaybackData->mPlaybackState, MpPlaybackData::Stopped);   
}

/*!
 Tests forceStop
 */
void TestMpMpxPbFrameworkWrapper::testForceStop()
{
    mTestPrivate->ForceStopL();
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandGeneralId),KMPXCommandIdPlaybackGeneral);
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType),TInt(EPbCmdStop));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandGeneralDoSync),TBool(ETrue));
    QCOMPARE(mTestPrivate->iPlaybackUtility->iCMPXCmd->ValueTObjectL<TBool>(KMPXCommandPlaybackGeneralNoBuffer),TBool(ETrue));
    QVERIFY(mTestPrivate->iPlaybackUtility->iObs == 0);
}

/*!
 Used to load test data
 */
void TestMpMpxPbFrameworkWrapper::loadTestData(TInt aPos)
{
    if(iMediaTestData){
        delete iMediaTestData;
        iMediaTestData=0;
    }
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);

    entries->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].GeneralTitle)));
    entries->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicArtist)));
    entries->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[aPos].GeneralCount);
    entries->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicAlbumArtFileName)));
    entries->SetTextValueL(KMPXMediaMusicAlbum, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicAlbum)));
    entries->SetTextValueL(KMPXMediaGeneralUri, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[aPos].MusicUri)));
 
    iMediaTestData = CMPXMedia::NewL(*entries);
    CleanupStack::PopAndDestroy(entries);
}

//end of file
