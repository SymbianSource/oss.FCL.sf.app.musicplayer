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
* Description: Unit test for mpequalizerwidget
*
*/

#include <QMetaType>
#include <QtGui>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbradiobuttonlist.h>

#include "stub/inc/mpequalizerwrapper.h"
#include "unittest_mpequalizerwidget.h"

// Do this so we can access all member variables.
#define private public
#include "mpequalizerwidget.h"
#undef private

//This so we can test private functions
#include "../../src/mpequalizerwidget.cpp"



/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpEqualizerWidget tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpequalizerwidget.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpEqualizerWidget::TestMpEqualizerWidget()
    : mTest(0)
{
}

TestMpEqualizerWidget::~TestMpEqualizerWidget()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpEqualizerWidget::initTestCase()
{
   
}   

/*!
 Called after the last testfunction was executed.
 */
void TestMpEqualizerWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpEqualizerWidget::init()
{
    mTest = new MpEqualizerWidget();
}

/*!
 Called after every testfunction.
 */
void TestMpEqualizerWidget::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
 */
void TestMpEqualizerWidget::testInitialize()
{
    QVERIFY(mTest->mEqualizerWrapper != 0 );    
}

/*!
 Tests testPrepare.
 */
void TestMpEqualizerWidget::testPrepareDialog()
{
    //With Equalizer not ready
    mTest->prepareDialog();
    QVERIFY(mTest->mOriginalPreset == -1 );    
    QVERIFY(mTest->timeout() == HbDialog::NoTimeout);
    QVERIFY(mTest->dismissPolicy() == HbDialog::NoDismiss);
    QVERIFY(mTest->frameType() == HbPopup::Strong);
    QVERIFY(mTest->headingWidget() != 0);
    QVERIFY(mTest->actions().count() != 0);
    QVERIFY(mTest->contentWidget() == 0);
    
    
    //With Equalizer ready
    cleanup();
    init();
    mTest->equalizerReady();
    mTest->prepareDialog();
    QVERIFY(mTest->mPresetsList != 0);
    QCOMPARE(qobject_cast<HbRadioButtonList*>(mTest->contentWidget()), mTest->mPresetsList);
    QVERIFY(mTest->mPresetsList->selected() == 0);
}

/*!
 Tests equalizerReady.
 */
void TestMpEqualizerWidget::testEqualizerReady()
{
    mTest->equalizerReady();
    
    QVERIFY(mTest->mEqualizerReady);
}

/*!
 Tests presetSelected.
 */
void TestMpEqualizerWidget::testPresetSelected()
{
    //Off preset must call DisableEqualizer
    mTest->presetSelected(0);
    QVERIFY(mTest->mEqualizerWrapper->mDisableEquqlizerCount == 1);

    //Any other preset must be applied
    mTest->presetSelected(3);
    QVERIFY(mTest->mEqualizerWrapper->mApplyPresetCount == 1);
}

/*!
 Tests cancelSelected.
 */
void TestMpEqualizerWidget::testCancelSelected()
{
    mTest->equalizerReady();
    mTest->prepareDialog();
    
    //Cancel, when Current preset selected != Original Preset
    
    //When Original Preset == -1 "Off"
    mTest->mEqualizerWrapper->mActivePreset = 4;
    mTest->mOriginalPreset = -1;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mEqualizerWrapper->mDisableEquqlizerCount == 1);
    QCOMPARE(mTest->mEqualizerWrapper->mActivePreset, mTest->mOriginalPreset);
    
    //When Original Preset != -1 "Any other different than Off"
    mTest->mEqualizerWrapper->mActivePreset = 4;
    mTest->mOriginalPreset = 2;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mEqualizerWrapper->mApplyPresetCount == 1);
    QCOMPARE(mTest->mEqualizerWrapper->mActivePreset, mTest->mOriginalPreset);
        
    //Cancel, when Current preset selected == Original Preset
    mTest->mEqualizerWrapper->mActivePreset = 3;
    mTest->mOriginalPreset = 3;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mEqualizerWrapper->mApplyPresetCount == 1);
    QVERIFY(mTest->mEqualizerWrapper->mDisableEquqlizerCount == 1);
    QCOMPARE(mTest->mEqualizerWrapper->mActivePreset, mTest->mOriginalPreset);
}

/*!
 Tests okSelected.
 */
void TestMpEqualizerWidget::testOkSelected()
{
    mTest->equalizerReady();
    
    mTest->mEqualizerWrapper->mActivePreset = 2;
    mTest->mOriginalPreset = -1;
    mTest->okSelected(true);
    QCOMPARE(mTest->mEqualizerWrapper->mActivePreset, mTest->mOriginalPreset);   
}

// End of file
