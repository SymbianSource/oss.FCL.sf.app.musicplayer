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
* Description: Unit test for mpequalizerwrapper
*
*/

#include <QSignalSpy>
#include <QStringList>
#include <QMetaType>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <centralrepository.h>

#include "unittest_mpequalizerwrapper.h"
#include "stub/inc/mpxuser.h"
#include "stub/inc/mpxplaybackutility.h"
#include "stub/inc/audioequalizerutility.h"
#include "mpsettingsmanager.h"

// Do this so we can access all member variables.
#define private public
#include "mpequalizerwrapper.h"
#include "mpequalizerwrapper_p.h"
#undef private

//This so we can test private functions
#include "mpequalizerwrapper_p.cpp"

//    Predifined values
//sa stub/inc/audioequalizerutility.cpp for Predifined Values
//PresetName    PresetEngineId  PresetId (Ui)
//  Off              -1             -1
//  Bass             0x0A           1
//  Classical        0x0B           2
//  Pop              0x0C           3
//  Jazz             0x0D           4
//  Rock             0x0E           5

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpEqualizerWrapper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpequalizerwrapper.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpEqualizerWrapper::TestMpEqualizerWrapper()
{
}

TestMpEqualizerWrapper::~TestMpEqualizerWrapper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpEqualizerWrapper::initTestCase()
{
   
}   

/*!
 Called after the last testfunction was executed.
 */
void TestMpEqualizerWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpEqualizerWrapper::init()
{
    mTest = new MpEqualizerWrapper();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpEqualizerWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor and DoInit().
 */
void TestMpEqualizerWrapper::testConstructor()
{    
    QVERIFY(mTestPrivate->iMpxPlaybackUtility != 0);
    QVERIFY(mTestPrivate->iMdaPlayUtility != 0);
}

/*!
 Tests MapcInitComplete.
 */
void TestMpEqualizerWrapper::testMapcInitComplete()
{   
    QSignalSpy spyEqualizerReady(mTest, SIGNAL(equalizerReady()));
    
    mTestPrivate->MapcInitComplete(KErrNone, 0);

    QVERIFY(mTestPrivate->iEqualizerUtility != 0);
    QCOMPARE(spyEqualizerReady.count(), 1);
}

/*!
 Tests getPresetId
 */
void TestMpEqualizerWrapper::testGetPresetId()
{       
    TInt presetId(0);
    mTestPrivate->MapcInitComplete(KErrNone, 0);
    
    //When a valid preset is currently set
    MpSettingsManager::setPreset(0x0A);
    QTest::qWait(250);
    
    mTestPrivate->GetPresetId(presetId);    
    QCOMPARE(mTestPrivate->iCurrentPresetIndex, presetId);                   
    QVERIFY(presetId == 1);
    
    //When there is no any preset set
    MpSettingsManager::setPreset(KEqualizerPresetNone);
    QTest::qWait(250);
    
    mTestPrivate->GetPresetId(presetId);    
    QCOMPARE(mTestPrivate->iCurrentPresetIndex, presetId);                   
    QVERIFY(presetId == KEqualizerPresetNone);
}

/*!
 Tests GetEngineIndexL.
 */
void TestMpEqualizerWrapper::testGetEngineIndexL()
{   
    mTestPrivate->MapcInitComplete(KErrNone, 0);    
    
    QVERIFY(mTestPrivate->GetEngineIndexL(2) == 0x0C);    
}

/*!
 Tests activePreset.
 */
void TestMpEqualizerWrapper::testActivePreset()
{
    MpSettingsManager::setPreset(0x0A);
    mTestPrivate->MapcInitComplete(KErrNone, 0);
    
    QCOMPARE(mTest->activePreset(), mTestPrivate->iCurrentPresetIndex );
}

/*!
 Tests applyPreset and HandlePresetActivation.
 */
void TestMpEqualizerWrapper::testApplyPreset()
{   
    int presetId(2);    
    
    mTestPrivate->MapcInitComplete(KErrNone, 0);    
    mTest->applyPreset(presetId);
    QTest::qWait(250);
    
    QVERIFY(mTestPrivate->iMpxPlaybackUtility->commandSent);        
    QVERIFY(MpSettingsManager::preset() == 0x0B);
    
    QCOMPARE(mTestPrivate->iCurrentPresetIndex, presetId);
}

/*!
 Tests disableEqualizer and HandlePresetActivation.
 */
void TestMpEqualizerWrapper::testDisableEqualizer()
{   
    MpSettingsManager::setPreset(0x0A); //Set any preset different than KEqualizerPresetNone
    
    mTestPrivate->MapcInitComplete(KErrNone, 0);    
    mTest->disableEqualizer();
    QTest::qWait(250);
    
    QVERIFY(mTestPrivate->iMpxPlaybackUtility->commandSent);        
    QVERIFY(MpSettingsManager::preset() == KEqualizerPresetNone);
    
    QVERIFY(mTestPrivate->iCurrentPresetIndex == KEqualizerPresetNone);
}

/*!
 Tests presetNames
 */
void TestMpEqualizerWrapper::testPresetNames()
{   
    
    mTestPrivate->MapcInitComplete(KErrNone, 0);
    
    QStringList presetNames = mTest->presetNames();
    
    QVERIFY(presetNames.at(0) == "Bass");
    QVERIFY(presetNames.at(1) == "Classical");
    QVERIFY(presetNames.at(2) == "Pop");
    QVERIFY(presetNames.at(3) == "Jazz");
    QVERIFY(presetNames.at(4) == "Rock");    
}

// End of file
