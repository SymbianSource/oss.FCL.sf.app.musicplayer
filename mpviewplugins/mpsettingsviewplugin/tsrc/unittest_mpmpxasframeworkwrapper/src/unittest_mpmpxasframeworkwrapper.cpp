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
* Description: Unit test for MpMpxAsFrameworkWrapper
*
*/

#include <QSignalSpy>
#include <qnamespace.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>

#include "unittest_mpmpxasframeworkwrapper.h"
#include "stub/inc/mpxaudioeffectproperties.h"
#include "stub/inc/mpxplaybackutility.h"


// Do this so we can access all member variables.
#define private public
#include "mpmpxasframeworkwrapper.h"
#include "mpmpxasframeworkwrapper_p.h"
#undef private

//This so we can test private functions
#include "mpmpxasframeworkwrapper_p.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestMpMpxAsFrameworkWrapper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpmpxasframeworkwrapper.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpMpxAsFrameworkWrapper::TestMpMpxAsFrameworkWrapper()
    : mTest(0)
{
}

TestMpMpxAsFrameworkWrapper::~TestMpMpxAsFrameworkWrapper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxAsFrameworkWrapper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxAsFrameworkWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxAsFrameworkWrapper::init()
{
    mTest = new MpMpxAsFrameworkWrapper();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMpxAsFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 test Constructor
 */
void TestMpMpxAsFrameworkWrapper::testConstructor()
{
    QVERIFY(mTestPrivate->iPlaybackUtility == 0);
    QVERIFY(mTestPrivate->iAudioEffectProperties != 0);
}

/*!
 test balance
 */
void TestMpMpxAsFrameworkWrapper::testBalance()
{
    for(TInt i = -10; i < 10; i++){
        mTest->persistBalance(i);
        QCOMPARE(mTestPrivate->iAudioEffectProperties->iBalance, i);
        QCOMPARE(mTestPrivate->iAudioEffectProperties->iSaveToFile, TBool(ETrue));
        mTestPrivate->iAudioEffectProperties->iSaveToFile = EFalse;
        QCOMPARE(mTest->balance(), i);
        QCOMPARE(mTestPrivate->iAudioEffectProperties->iLoadFromFile, TBool(ETrue));
        mTestPrivate->iAudioEffectProperties->iLoadFromFile = EFalse;
    }
    mTestPrivate->iAudioEffectProperties->iLeave = ETrue;
    QCOMPARE(mTest->balance(),0);
}


/*!
 test setBalance
 */
void TestMpMpxAsFrameworkWrapper::testSetBalance()
{
    for(TInt i = -10; i < 10; i++){
        mTest->setBalance(i);
        QVERIFY(mTestPrivate->iPlaybackUtility != 0);
        QCOMPARE(mTestPrivate->iPlaybackUtility->iProperty, EPbPropertyBalance);
        QCOMPARE(mTestPrivate->iPlaybackUtility->iBalance, i);        
    }   
}

/*!
 test persistBalance
 */
void TestMpMpxAsFrameworkWrapper::testPersistBalance()
{
    for(TInt i = -10; i < 10; i++){
        mTest->persistBalance(i);
        QCOMPARE(mTestPrivate->iAudioEffectProperties->iBalance, i);
        QCOMPARE(mTestPrivate->iAudioEffectProperties->iSaveToFile, TBool(ETrue));
        mTestPrivate->iAudioEffectProperties->iSaveToFile = EFalse;
    }   
}
    
    
// end of file
