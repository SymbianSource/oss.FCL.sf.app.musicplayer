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

#include "unittest_mpsongscanner.h"
#include "mpcommondefs.h"
#include "stub/inc/mpxharvesterutility.h"
#include "stub/inc/mpxcollectionutility.h"


// Do this so we can access all member variables.
#define private public
#include "mpsongscanner.h"
#include "mpsongscanner_p.h"
#undef private


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpSongScanner tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpsongscanner.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpSongScanner::TestMpSongScanner()
    : mTest(0)
{
}

TestMpSongScanner::~TestMpSongScanner()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSongScanner::initTestCase()
{
    RDebug::Print(_L("initTestCase()"));
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSongScanner::cleanupTestCase()
{
    RDebug::Print(_L("cleanupTestCase()"));
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSongScanner::init()
{
    RDebug::Print(_L(">>init()"));
    mTest = new MpSongScanner();
    mTestPrivate = mTest->d_ptr;
    RDebug::Print(_L("<<init()"));
}

/*!
 Called after every testfunction.
 */
void TestMpSongScanner::cleanup()
{
    if (mTest)
    {
        delete mTest;
        mTest = 0;
        mTestPrivate = 0;
    }
}

void TestMpSongScanner::testConstructor()
{
    RDebug::Print(_L(">>testConstructor()"));
    
    QVERIFY(mTest != 0);
    QVERIFY(mTestPrivate != 0);
    QVERIFY(mTestPrivate->iCollectionUtility != 0);
    QVERIFY(mTestPrivate->iHarvesterUtility != 0);
    
    RDebug::Print(_L("<<testConstructor()"));
}

void TestMpSongScanner::testDestructor()
{
    RDebug::Print(_L(">>testDestructor()"));
    
    cleanup();
    QVERIFY(mTest == 0);
    QCOMPARE(MMPXCollectionUtility::getCount(), 0);
    QCOMPARE(MMPXHarvesterUtility::getCount(), 0);
   
    RDebug::Print(_L("<<testDestructor()"));
}

void TestMpSongScanner::testConstructorFail()
{
    RDebug::Print(_L(">>testConstructorFail()"));

    cleanup();
    MMPXCollectionUtility::setNewLLeave();
    init();
    QVERIFY(mTest != 0);
    
    cleanup();
    CMPXHarvesterFactory::setNewLLeave();
    init();
    QVERIFY(mTest != 0);
    
    cleanup();
    MMPXHarvesterUtility::setCheckForSystemEventsLLeave();
    init();
    QVERIFY(mTest != 0);
    
    cleanup();
    MMPXCollectionUtility::setCollectionIDLLeave();
    init();
    QVERIFY(mTest != 0);
    
    cleanup();
    MMPXCollection::setOpenLLeave();
    init();
    QVERIFY(mTest != 0);
    
    cleanup();
    MMPXCollection::setMediaLLeave();
    init();
    QVERIFY(mTest != 0);
   
    RDebug::Print(_L("<<testConstructorFail()"));
}

void TestMpSongScanner::testIsScanning()
{
    RDebug::Print(_L(">>testIsScanning()"));
    
    QVERIFY(mTest->isScanning() == false);
   
    RDebug::Print(_L("<<testIsScanning()"));
}

void TestMpSongScanner::testScan()
{
    RDebug::Print(_L(">>testScan()"));

    mTest->scan();
    QCOMPARE(mTest->isScanning(), true);
    
    RDebug::Print(_L("<<testScan()"));
}

void TestMpSongScanner::testCancelScan()
{
    RDebug::Print(_L(">>testCancelScan()"));

    mTest->scan();
    mTest->cancelScan();
    QCOMPARE(mTest->isScanning(), false);
    
    RDebug::Print(_L("<<testCancelScan()"));
}

void TestMpSongScanner::testScanFail()
{
    RDebug::Print(_L(">>testScanFail()"));

    MMPXHarvesterUtility::setScanLLeave();
    mTest->scan();
    QVERIFY(mTest->isScanning() == true);
    
    RDebug::Print(_L("<<testScanFail()"));
}

void TestMpSongScanner::testCancelScanFail()
{
    RDebug::Print(_L(">>testCancelScanFail()"));

    MMPXHarvesterUtility::setCancelScanLLeave();
    mTest->scan();
    mTest->cancelScan();
    QCOMPARE(mTest->isScanning(), false);
    
    cleanup();
    init();
    mTest->scan();
    MMPXHarvesterUtility::setCancelScanLLeave();
    CMPXHarvesterFactory::setNewLLeave();
    mTest->cancelScan();
    QCOMPARE(mTest->isScanning(), false);
    QVERIFY(mTestPrivate->iHarvesterUtility == 0);
    
    cleanup();
    init();
    mTest->scan();
    MMPXHarvesterUtility::setCancelScanLLeave();
    MMPXHarvesterUtility::setCheckForSystemEventsLLeave();
    mTest->cancelScan();
    QCOMPARE(mTest->isScanning(), false);
    QVERIFY(mTestPrivate->iHarvesterUtility != 0);
    
    RDebug::Print(_L("<<testCancelScanFail()"));
}

void TestMpSongScanner::testHandleOpenLMedia()
{
    RDebug::Print(_L(">>testHandleOpenLMedia()"));
    
    CMPXMedia* mpxMedia = NULL;
    mTestPrivate->HandleOpenL(*mpxMedia, 0, ETrue, KErrNone);
    
    // no implementation on HandleOpenL now, check something that should be obvious (isScanning)
    QCOMPARE(mTest->isScanning(), false);
    
    RDebug::Print(_L("<<testHandleOpenLMedia()"));
}

void TestMpSongScanner::testHandleOpenLPlaylist()
{
    RDebug::Print(_L(">>testHandleOpenLPlaylist()"));
    
    CMPXCollectionPlaylist* mpxPlaylist = NULL;
    mTestPrivate->HandleOpenL(*mpxPlaylist, KErrNone);
    
    // no implementation on HandleOpenL now, check something that should be obvious (isScanning)
    QCOMPARE(mTest->isScanning(), false);
        
    RDebug::Print(_L("<<testHandleOpenLPlaylist()"));
}

void TestMpSongScanner::testHandleCollectionMessageRefresh()
{
    RDebug::Print(_L(">>testHandleCollectionMessageRefresh()"));
    
    QSignalSpy spy1(mTest, SIGNAL(scanStarted()));
    QSignalSpy spy2(mTest, SIGNAL(scanEnded()));
    TInt err = KErrNone;
    
    mTest->scan();
    err = mTestPrivate->iCollectionUtility->SendRefreshStartMessage();
    
    QCOMPARE(err, KErrNone);
    QCOMPARE(spy1.count(), 1);
    
    err = mTestPrivate->iCollectionUtility->SendRefreshEndMessage();
    
    QCOMPARE(err, KErrNone);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(mTest->isScanning(), false);
    
    RDebug::Print(_L("<<testHandleCollectionMessageRefresh()"));
}

void TestMpSongScanner::testHandleCollectionMessageDiskInserted()
{
    RDebug::Print(_L(">>testHandleCollectionMessageDiskInserted()"));

    QSignalSpy spy(mTest, SIGNAL(scanEnded()));
    TInt err = KErrNone;
    
    mTest->scan();
    err = mTestPrivate->iCollectionUtility->SendDiskInsertedMessage();
    
    QCOMPARE(err, KErrNone);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(mTest->isScanning(), false);
    
    RDebug::Print(_L("<<testHandleCollectionMessageDiskInserted()"));
}

void TestMpSongScanner::testHandleCollectionMessageItemChanged()
{
    RDebug::Print(_L(">>testHandleCollectionMessageItemChanged()"));

    TInt err = KErrNone;

    // must send a Refresh start first or itemadded check would not be valid
    mTest->scan();
    err = mTestPrivate->iCollectionUtility->SendRefreshStartMessage();
        
    QCOMPARE(err, KErrNone);
    
    err = mTestPrivate->iCollectionUtility->SendItemChangedMessage();
    
    QCOMPARE(err, KErrNone);
    QCOMPARE(mTestPrivate->iNumItemsAdded, 1);

    RDebug::Print(_L("<<testHandleCollectionMessageItemChanged()"));
}

void TestMpSongScanner::testHandleCollectionMediaL()
{
    RDebug::Print(_L(">>testHandleCollectionMediaL()"));
    
    QSignalSpy spy(mTest, SIGNAL(scanEnded()));
    TInt err = KErrNone;
    
    err = mTestPrivate->iCollectionUtility->SendDbCreatedMedia();
    
    QCOMPARE(err, KErrNone);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(mTest->isScanning(), false);
    
    err = mTestPrivate->iCollectionUtility->SendNotDbCreatedMedia();
        
    QCOMPARE(err, KErrNone);
    QCOMPARE(mTest->isScanning(), true);        
    
    RDebug::Print(_L("<<testHandleCollectionMediaL()"));
}

// End of file
