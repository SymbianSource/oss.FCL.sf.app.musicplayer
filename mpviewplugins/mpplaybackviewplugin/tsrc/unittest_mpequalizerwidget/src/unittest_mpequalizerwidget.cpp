/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hbradiobuttonlist.h>
#include <QTranslator>
#include <QLocale>

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
    QApplication app(argc, argv);
    TestMpEqualizerWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpequalizerwidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpEqualizerWidget::TestMpEqualizerWidget()
    : mTest(0),
      mMpTranslator(0)
{
}

TestMpEqualizerWidget::~TestMpEqualizerWidget()
{
    delete mTest;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpEqualizerWidget::initTestCase()
{
    QString lang = QLocale::system().name();
    QString path = QString("z:/resource/qt/translations/");
    bool translatorLoaded = false;

    mMpTranslator = new QTranslator(this);
    translatorLoaded = mMpTranslator->load(path + "musicplayer_" + lang);
    if ( translatorLoaded ) {
        qApp->installTranslator(mMpTranslator);
    }
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
    MpEngineFactory::createSharedEngine();
    mTest = new MpEqualizerWidget();
}

/*!
 Called after every testfunction.
 */
void TestMpEqualizerWidget::cleanup()
{
    delete mTest;
    mTest = 0;
    MpEngineFactory::close();
}

/*!
 Tests constructor.
 */
void TestMpEqualizerWidget::testInitialize()
{
    QVERIFY(mTest->mMpEngine != 0 );    
}

/*!
 Tests testPrepare.
 */
void TestMpEqualizerWidget::testPrepareDialog()
{
    mTest->prepareDialog();
    QVERIFY(mTest->mOriginalPreset == -1 );    
    QVERIFY(mTest->timeout() == HbDialog::NoTimeout);
    QVERIFY(mTest->dismissPolicy() == HbDialog::NoDismiss);
    QVERIFY(mTest->frameType() == HbPopup::Strong);
    QVERIFY(mTest->headingWidget() != 0);
    QVERIFY(mTest->actions().count() != 0);
    QVERIFY(mTest->mPresetsList != 0);
    QCOMPARE(qobject_cast<HbRadioButtonList*>(mTest->contentWidget()), mTest->mPresetsList);
    QVERIFY(mTest->mPresetsList->selected() == 0);
}

/*!
 Tests presetSelected.
 */
void TestMpEqualizerWidget::testPresetSelected()
{
    //Off preset must call DisableEqualizer
    mTest->presetSelected(0);
    QVERIFY(mTest->mMpEngine->mDisableEquqlizerCount == 1);

    //Any other preset must be applied
    mTest->presetSelected(3);
    QVERIFY(mTest->mMpEngine->mApplyPresetCount == 1);
}

/*!
 Tests cancelSelected.
 */
void TestMpEqualizerWidget::testCancelSelected()
{
    mTest->prepareDialog();
    
    //Cancel, when Current preset selected != Original Preset
    
    //When Original Preset == -1 "Off"
    mTest->mMpEngine->mActivePreset = 4;
    mTest->mOriginalPreset = -1;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mMpEngine->mDisableEquqlizerCount == 1);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPreset);
    
    //When Original Preset != -1 "Any other different than Off"
    mTest->mMpEngine->mActivePreset = 4;
    mTest->mOriginalPreset = 2;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mMpEngine->mApplyPresetCount == 1);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPreset);
        
    //Cancel, when Current preset selected == Original Preset
    mTest->mMpEngine->mActivePreset = 3;
    mTest->mOriginalPreset = 3;
    mTest->cancelSelected(true);
    QVERIFY(mTest->mMpEngine->mApplyPresetCount == 1);
    QVERIFY(mTest->mMpEngine->mDisableEquqlizerCount == 1);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPreset);
}

/*!
 Tests okSelected.
 */
void TestMpEqualizerWidget::testOkSelected()
{
    mTest->mMpEngine->mActivePreset = 2;
    mTest->mOriginalPreset = -1;
    mTest->okSelected(true);
    QCOMPARE(mTest->mMpEngine->mActivePreset, mTest->mOriginalPreset);   
}

/*!
 Tests getLogicalName.
 */
void TestMpEqualizerWidget::testGetLogicalName()
{
    QString logicalName;
    
    // test Bass Booster
    logicalName = mTest->getLogicalName( QString("Bass Booster") );
    QCOMPARE(logicalName, hbTrId("txt_mus_list_bass_booster"));

    // test Classic
    logicalName = mTest->getLogicalName( QString("Classic") );
    QCOMPARE(logicalName, hbTrId("txt_mus_list_classical"));

    // test Pop
    logicalName = mTest->getLogicalName( QString("Pop") );
    QCOMPARE(logicalName, hbTrId("txt_mus_list_pop"));

    // test Jazz
    logicalName = mTest->getLogicalName( QString("Jazz") );
    QCOMPARE(logicalName, hbTrId("txt_mus_list_jazz"));

    // test Rock
    logicalName = mTest->getLogicalName( QString("Rock") );
    QCOMPARE(logicalName, hbTrId("txt_mus_list_rock"));
}

// End of file
