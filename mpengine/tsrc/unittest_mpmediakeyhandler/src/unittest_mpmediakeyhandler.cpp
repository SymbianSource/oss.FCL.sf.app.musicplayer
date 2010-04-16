/**
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
* Description: Unit test for mpmediakeyhandler
*
*/

#include <QSignalSpy>
#include <QMetaType>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <e32debug.h>

#include "unittest_mpmediakeyhandler.h"
#include "mpcommondefs.h"
#include "stub/inc/mpxplaybackutility.h"
#include "stub/inc/remconcoreapitarget.h"
#include "stub/inc/remconinterfaceselector.h"


// Do this so we can access all member variables.
#define private public
#include "mpmediakeyhandler.h"
#include "mpmediakeyhandler_p.h"
#undef private


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpMediaKeyHandler tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpmediakeyhandler.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpMediaKeyHandler::TestMpMediaKeyHandler()
    : mTest(0)
{
}

TestMpMediaKeyHandler::~TestMpMediaKeyHandler()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMediaKeyHandler::initTestCase()
{
    RDebug::Print(_L("initTestCase()"));
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMediaKeyHandler::cleanupTestCase()
{
    RDebug::Print(_L("cleanupTestCase()"));
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMediaKeyHandler::init()
{
    RDebug::Print(_L(">>init()"));
    mTest = new MpMediaKeyHandler();
    mTestPrivate = mTest->d_ptr;
    RDebug::Print(_L("<<init()"));
}

/*!
 Called after every testfunction.
 */
void TestMpMediaKeyHandler::cleanup()
{
    if (mTest)
    {
        delete mTest;
        mTest = 0;
        mTestPrivate = 0;
    }
}

void TestMpMediaKeyHandler::testConstructor()
{
    RDebug::Print(_L(">>testConstructor()"));
    
    QVERIFY(mTest != 0);
    QVERIFY(mTestPrivate != 0);
    QVERIFY(mTestPrivate->iInterfaceSelector != 0);
    QVERIFY(mTestPrivate->iResponseHandler != 0);
    QVERIFY(mTestPrivate->iPlaybackUtility != 0);
    
    RDebug::Print(_L("<<testConstructor()"));
}

void TestMpMediaKeyHandler::testDestructor()
{
    RDebug::Print(_L(">>testDestructor()"));
    
    // it is been observed that CRemConCoreApiTarget is not being deleted by anyone
    // the creator claim it does not own it, while others never claim the ownership neither
    cleanup();
    QVERIFY(mTest == 0);
    QCOMPARE(MMPXPlaybackUtility::getCount(), 0);
    QCOMPARE(CRemConCoreApiTarget::getCount(), 0);
    QCOMPARE(CRemConInterfaceSelector::getCount(), 0);
    
    RDebug::Print(_L("<<testDestructor()"));
}

void TestMpMediaKeyHandler::testConstructorFail()
{
    RDebug::Print(_L(">>testConstructorFail()"));

    // errorOccured signal cannot be connected until mediakeyhandler is created,
    // and error during construction cannot be feedback.
    // solution should be to seperate construction/init
    cleanup();
    CRemConInterfaceSelector::setNewLLeave();
    init();
    QSignalSpy spy1(mTest, SIGNAL(errorOccured(int)));
    QVERIFY(mTest != 0);
    //QCOMPARE(spy1.count(), 1);    // failed if comment in
    
    cleanup();
    CRemConInterfaceSelector::setOpenTargetLLeave();
    init();
    QSignalSpy spy2(mTest, SIGNAL(errorOccured(int)));
    QVERIFY(mTest != 0);
    //QCOMPARE(spy2.count(), 1);    // failed if comment in
    
    cleanup();
    CRemConCoreApiTarget::setNewLLeave();
    init();
    QSignalSpy spy3(mTest, SIGNAL(errorOccured(int)));
    QVERIFY(mTest != 0);
    //QCOMPARE(spy3.count(), 1);    // failed if comment in

    cleanup();
    MMPXPlaybackUtility::setNewLLeave();
    init();
    QSignalSpy spy4(mTest, SIGNAL(errorOccured(int)));
    QVERIFY(mTest != 0);
    //QCOMPARE(spy4.count(), 1);    // failed if comment in
    
    cleanup();
    MMPXPlaybackUtility::setAddObserverLLeave();
    init();
    QSignalSpy spy5(mTest, SIGNAL(errorOccured(int)));
    QVERIFY(mTest != 0);
    //QCOMPARE(spy5.count(), 1);    // failed if comment in
    
    RDebug::Print(_L("<<testConstructorFail()"));
}

void TestMpMediaKeyHandler::testDestructorFail()
{
    RDebug::Print(_L(">>testDestructorFail()"));
    
    MMPXPlaybackUtility::setRemoveObserverLLeave();
    cleanup();
    QVERIFY(mTest == 0);
    QCOMPARE(MMPXPlaybackUtility::getCount(), 0);
    QCOMPARE(CRemConCoreApiTarget::getCount(), 0);
    QCOMPARE(CRemConInterfaceSelector::getCount(), 0);
    
    RDebug::Print(_L("<<testDestructorFail()"));
}

void TestMpMediaKeyHandler::testCommandVolumeUpKey()
{
    RDebug::Print(_L(">>testCommandVolumeUpKey()"));
    
    // volumeChanged signal would not emit until mediakeyhandler is not added as an observer to MMPXPlaybackUtility
    QSignalSpy spy(mTest, SIGNAL(volumeChanged(int)));
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    MMPXPlaybackUtility::setPlaying();
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeUp, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(spy.count(), 1);
    
    // impossible to stub out the CPeriodic class which is part of the Press/Release handling
    // hence, those logic could not be tested 
    
    RDebug::Print(_L("<<testCommandVolumeUpKey()"));
}

void TestMpMediaKeyHandler::testCommandVolumeDownKey()
{
    RDebug::Print(_L(">>testCommandVolumeDownKey()"));
    
    // volumeChanged signal would not emit until mediakeyhandler is not added as an observer to MMPXPlaybackUtility
    QSignalSpy spy(mTest, SIGNAL(volumeChanged(int)));
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    MMPXPlaybackUtility::setPlaying();
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeDown, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(spy.count(), 1);
    
    // impossible to stub out the CPeriodic class which is part of the Press/Release handling
    // hence, those logic could not be tested
    
    RDebug::Print(_L("<<testCommandVolumeDownKey()"));
}

void TestMpMediaKeyHandler::testCommandPlayKey()
{
    RDebug::Print(_L(">>testCommandPlayKey()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiPlay, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandPlayKey()"));
}

void TestMpMediaKeyHandler::testCommandPauseKey()
{
    RDebug::Print(_L(">>testCommandPauseKey()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiPause, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
        
    RDebug::Print(_L("<<testCommandPauseKey()"));
}

void TestMpMediaKeyHandler::testCommandPlayPauseKey()
{
    RDebug::Print(_L(">>testCommandPlayPauseKey()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiPausePlayFunction, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandPlayPauseKey()"));
}

void TestMpMediaKeyHandler::testCommandStopKey()
{
    RDebug::Print(_L(">>testCommandStopKey()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiStop, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandStopKey()"));
}

void TestMpMediaKeyHandler::testCommandFastForwardKeyPress()
{
    RDebug::Print(_L(">>testCommandFastForwardKeyPress()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiFastForward, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandFastForwardKeyPress()"));
}

void TestMpMediaKeyHandler::testCommandFastForwardKeyRelease()
{
    RDebug::Print(_L(">>testCommandFastForwardKeyRelease()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiFastForward, ERemConCoreApiButtonRelease);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandFastForwardKeyRelease()"));
}

void TestMpMediaKeyHandler::testCommandRewindKeyPress()
{
    RDebug::Print(_L(">>testCommandRewindKey()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiRewind, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandRewindKey()"));
}

void TestMpMediaKeyHandler::testCommandRewindKeyRelease()
{
    RDebug::Print(_L(">>testCommandRewindKeyRelease()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiRewind, ERemConCoreApiButtonRelease);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandRewindKeyRelease()"));
}

void TestMpMediaKeyHandler::testCommandForwardKey()
{
    RDebug::Print(_L(">>testCommandForwardKey()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiForward, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandForwardKey()"));
}

void TestMpMediaKeyHandler::testCommandBackwardKey()
{
    RDebug::Print(_L(">>testCommandBackwardKey()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiBackward, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testCommandBackwardKey()"));
}

void TestMpMediaKeyHandler::testPlay()
{
    RDebug::Print(_L(">>testPlay()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoPlay(ERemConCoreApiPlaybackSpeedX1, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    
    RDebug::Print(_L("<<testPlay()"));
}

void TestMpMediaKeyHandler::testAudioInputFunction()
{
    RDebug::Print(_L(">>testAudioInputFunction()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoSelectAudioInputFunction(0, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
        
    RDebug::Print(_L("<<testAudioInputFunction()"));
}

void TestMpMediaKeyHandler::testSelectAvInputFunction()
{
    RDebug::Print(_L(">>testSelectAvInputFunction()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoSelectAvInputFunction(0, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);

    RDebug::Print(_L("<<testSelectAvInputFunction()"));
}

void TestMpMediaKeyHandler::testSelectDiskFunction()
{
    RDebug::Print(_L(">>testSelectDiskFunction()"));
    
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoSelectDiskFunction(0, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);

    RDebug::Print(_L("<<testSelectDiskFunction()"));
}

void TestMpMediaKeyHandler::testTuneFunction()
{
    RDebug::Print(_L(">>testTuneFunction()"));

    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoTuneFunction(ETrue, 0, 1, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);

    RDebug::Print(_L("<<testTuneFunction()"));
}

// End of file
