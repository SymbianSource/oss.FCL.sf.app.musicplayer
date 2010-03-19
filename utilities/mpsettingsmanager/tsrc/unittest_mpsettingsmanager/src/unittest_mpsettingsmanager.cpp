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
* Description: Unit test for MpSettingsManager
*
*/

#include <QSignalSpy>
#include <qnamespace.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>
#include "mpsettingsmanagerdefs.h"


#include "unittest_mpsettingsmanager.h"
#include "stub/inc/xqsettingskey.h"
#include "stub/inc/xqsettingsmanager.h"


// Do this so we can access all member variables.
#define private public
#include "mpsettingsmanager.h"
#undef private

//This so we can test private functions
#include "../../src/mpsettingsmanager.cpp"


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestMpSettingsManager tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpsettingsmanager.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpSettingsManager::TestMpSettingsManager()
{
}

TestMpSettingsManager::~TestMpSettingsManager()
{

}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSettingsManager::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSettingsManager::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSettingsManager::init()
{

}

/*!
 Called after every testfunction.
 */
void TestMpSettingsManager::cleanup()
{

}

/*!
 TestConstructor.
 */
void TestMpSettingsManager::testConstructor()
{
    MpSettingsManager *test;
    test = new MpSettingsManager();
    QVERIFY(test);
    QVERIFY(test->mSettingsManager);
    
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingShuffleKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingShuffleKey), 1);
    QCOMPARE(test->mShuffle, true);
    
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mRepeat, true);
    
    delete test;
    test = 0;
}

/*!
 TestInstanceConstruction.
 */
void TestMpSettingsManager::testInstanceConstruction()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    QVERIFY(test);
    QVERIFY(test->mSettingsManager);
    
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingShuffleKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingShuffleKey), 1);
    QCOMPARE(test->mShuffle, true);
    
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingRepeatKey),1);
    QCOMPARE(test->mRepeat, true);
    
    QCOMPARE(test, MpSettingsManager::instance()); // test that instance returns the singleton
}

/*!
testShuffleGetAndSet
 */
void TestMpSettingsManager::testShuffleGetAndSet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mShuffle = true;
    MpSettingsManager::setShuffle(false);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingShuffleKey),QVariant(false));
    QCOMPARE(test->mShuffle, true); //should not chaqnge until sotred in cenrep

  
    test->mShuffle = false;
    test->setShuffle(true);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingShuffleKey),QVariant(true));
    QCOMPARE(test->mShuffle, false);//should not chaqnge until sotred in cenrep

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingShuffleKey),2);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingShuffleKey),0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingShuffleKey), 0);
}

/*!
testRepeatGetAndSet
 */
void TestMpSettingsManager::testRepeatGetAndSet()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
    test->mSettingsManager->clear();
    
    test->mRepeat = true;
    MpSettingsManager::setRepeat(false);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingRepeatKey),QVariant(false));
    QCOMPARE(test->mRepeat, true); //should not chaqnge until sotred in cenrep

  
    test->mRepeat = false;
    test->setRepeat(true);
    QCOMPARE(test->mSettingsManager->writeRequestValue(KMPCenRepSettingRepeatKey),QVariant(true));
    QCOMPARE(test->mRepeat, false);//should not chaqnge until sotred in cenrep

    //make sure there are not unecesary request to cenrep.
    QCOMPARE(test->mSettingsManager->writeItemValueRequestCount(KMPCenRepSettingRepeatKey),2);
    QCOMPARE(test->mSettingsManager->readItemValueRequestCount(KMPCenRepSettingRepeatKey),0);
    QCOMPARE(test->mSettingsManager->startMonitoringRequestCount(KMPCenRepSettingRepeatKey), 0);
}

/*!
 testValueChangedShuffleCase.
 */ 
void TestMpSettingsManager::testValueChangedShuffleCase()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();

    
    XQSettingsKey suffleProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingShuffleKey);
    test->mShuffle = true;
    test->valueChanged(suffleProfileKey, QVariant(0));
    QCOMPARE(test->mShuffle, false);

    test->mShuffle = false;
    QSignalSpy spy( test, SIGNAL(shuffleChanged(bool) ) );
    test->mSettingsManager->emitValueChanged(suffleProfileKey, QVariant(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(true) );
    QCOMPARE(test->mShuffle, true);
}

/*!
 testValueChangedRepeatCase.
 */ 
void TestMpSettingsManager::testValueChangedRepeatCase()
{
    MpSettingsManager *test;
    test = MpSettingsManager::instance();
      
    XQSettingsKey repeatProfileKey(XQSettingsKey::TargetCentralRepository, 
            KMPCenRepSettingsFeature, 
            KMPCenRepSettingRepeatKey);
    test->mRepeat = true;
    test->valueChanged(repeatProfileKey, QVariant(0));
    QCOMPARE(test->mRepeat, false);

    test->mRepeat = false;
    QSignalSpy spy( test, SIGNAL(repeatChanged(bool) ) );
    test->mSettingsManager->emitValueChanged(repeatProfileKey, QVariant(1));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(true) );
    QCOMPARE(test->mRepeat, true);

}

// End of file

