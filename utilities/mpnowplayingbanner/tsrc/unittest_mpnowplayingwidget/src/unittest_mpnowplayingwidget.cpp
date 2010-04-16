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
* Description: Unit test for mpnowplayingwidget
*
*/

#include <qnamespace.h>
#include <QSignalSpy>
#include <QMetaType>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>
#include <hblabel.h>
#include <hbevent.h>

#include "unittest_mpnowplayingwidget.h"
#include "stub/inc/mpnowplayingbackend.h"
#include "stub/inc/hbcolorscheme.h"
#include "mpcommondefs.h"

// Do this so we can access all member variables.
#define private public
#include "mpnowplayingwidget.h"
#include "mpnowplayingwidget_p.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpNowPlayingWidget tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpnowplayingwidget.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpNowPlayingWidget::TestMpNowPlayingWidget()
    : mTest(0)
{
}

TestMpNowPlayingWidget::~TestMpNowPlayingWidget()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpNowPlayingWidget::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpNowPlayingWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpNowPlayingWidget::init()
{
    mTest = new MpNowPlayingWidget(MpCommon::KMusicPlayerUid);
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpNowPlayingWidget::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor. Good case.
 */
void TestMpNowPlayingWidget::testConstructor()
{
    cleanup();
    MpNowPlayingBackEnd::resetUpdateCounter();
    init();
    QVERIFY(mTestPrivate->mPrimaryText != 0);
    QVERIFY(mTestPrivate->mSecondaryText != 0);
    QVERIFY(mTestPrivate->mBackEnd != 0);
    QVERIFY(mTestPrivate->mPlayIconNormal != 0);
    QVERIFY(mTestPrivate->mPauseIconNormal != 0);
    QVERIFY(mTestPrivate->mPlayIconPressed != 0);
    QVERIFY(mTestPrivate->mPauseIconPressed != 0);
    QVERIFY(mTestPrivate->mIcon != 0);
    QVERIFY(mTestPrivate->mDocumentLoader != 0);
    QCOMPARE(mTestPrivate->mState, NotPlaying);
    QCOMPARE(MpNowPlayingBackEnd::getUpdateCounter(), 1);
}

/*!
 Tests various state change combinations.
 */
void TestMpNowPlayingWidget::testSetState()
{
    QSignalSpy spy(mTest, SIGNAL(playbackAttachmentChanged(bool)));

    mTestPrivate->mState = NotPlaying;
    mTestPrivate->mBackEnd->triggerStateUpdate(NotPlaying);
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mTestPrivate->mState, NotPlaying);

    mTestPrivate->mState = Playing;
    mTestPrivate->mBackEnd->triggerStateUpdate(NotPlaying);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
    QCOMPARE(mTestPrivate->mState, NotPlaying);

    spy.clear();
    mTestPrivate->mState = Paused;
    mTestPrivate->mBackEnd->triggerStateUpdate(NotPlaying);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
    QCOMPARE(mTestPrivate->mState, NotPlaying);

    spy.clear();
    mTestPrivate->mState = NotPlaying;
    mTestPrivate->mBackEnd->triggerStateUpdate(Playing);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QCOMPARE(mTestPrivate->mState, Playing);

    spy.clear();
    mTestPrivate->mState = NotPlaying;
    mTestPrivate->mBackEnd->triggerStateUpdate(Paused);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QCOMPARE(mTestPrivate->mState, Paused);

    spy.clear();
    mTestPrivate->mState = Playing;
    mTestPrivate->mBackEnd->triggerStateUpdate(Paused);
    mTestPrivate->mState = Paused;
    mTestPrivate->mBackEnd->triggerStateUpdate(Playing);
    QCOMPARE(spy.count(), 0);
}

/*!
 Tests title change from the backend.
 */
void TestMpNowPlayingWidget::testTitleChange()
{
    mTestPrivate->mBackEnd->triggerTitleChanged(QString("Title"));
    QCOMPARE(mTestPrivate->mPrimaryText->plainText(), QString("Title"));
}

/*!
 Tests artist change from the backend.
 */
void TestMpNowPlayingWidget::testArtistChange()
{
    mTestPrivate->mBackEnd->triggerArtistChanged(QString("Artist"));
    QCOMPARE(mTestPrivate->mSecondaryText->plainText(), QString("Artist"));
}

/*!
 Tests disabling/enabling the widget.
 Widget should stop observing events when disabled.
 */
void TestMpNowPlayingWidget::testSetEnabled()
{
    QSignalSpy spy(mTest, SIGNAL(playbackAttachmentChanged(bool)));
    mTestPrivate->mPrimaryText->setPlainText("Primary");
    mTestPrivate->mSecondaryText->setPlainText("Secondary");
    mTestPrivate->mState = NotPlaying;

    // Disable the widget; Nothing should update.
    mTest->setEnabled(false);
    mTestPrivate->mBackEnd->triggerStateUpdate(Playing);
    QCOMPARE(mTestPrivate->mState, NotPlaying);
    QCOMPARE(spy.count(), 0);

    mTestPrivate->mBackEnd->triggerTitleChanged(QString("Title"));
    QCOMPARE(mTestPrivate->mPrimaryText->plainText(), QString("Primary"));

    mTestPrivate->mBackEnd->triggerArtistChanged(QString("Artist"));
    QCOMPARE(mTestPrivate->mSecondaryText->plainText(), QString("Secondary"));

    // Enable the widget. It should start updating again.
    mTest->setEnabled(true);
    mTestPrivate->mState = Playing;
    mTestPrivate->mBackEnd->triggerStateUpdate(NotPlaying);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), false);
    QCOMPARE(mTestPrivate->mState, NotPlaying);

    mTestPrivate->mBackEnd->triggerTitleChanged(QString("Title"));
    QCOMPARE(mTestPrivate->mPrimaryText->plainText(), QString("Title"));

    mTestPrivate->mBackEnd->triggerArtistChanged(QString("Artist"));
    QCOMPARE(mTestPrivate->mSecondaryText->plainText(), QString("Artist"));
}

/*!
 Tests ThemeChange.
 */
void TestMpNowPlayingWidget::testThemeChange()
{
    
    HbEvent event(HbEvent::ThemeChanged);
    HbColorTheme::global()->setCurrentTheme(1);
    //TODO final color resources should be qtc_multimedia_trans_normal when available
    QColor normalColor( HbColorScheme::color("foreground") );
    //TODO final color resources should be qtc_multimedia_trans_pressed when available
    QColor pressedColor( HbColorScheme::color("popupbackground") );
    
    mTest->changeEvent(&event);
    
    QCOMPARE(mTestPrivate->mPrimaryText->textColor(),normalColor);
    QCOMPARE(mTestPrivate->mSecondaryText->textColor(),normalColor);
    QCOMPARE(mTestPrivate->mPlayIconNormal->color(),normalColor);
    QCOMPARE(mTestPrivate->mPauseIconNormal->color(),normalColor);
    QCOMPARE(mTestPrivate->mPlayIconPressed->color(),pressedColor);
    QCOMPARE(mTestPrivate->mPauseIconPressed->color(),pressedColor);
    
    HbColorTheme::global()->setCurrentTheme(0);
    
    mTest->changeEvent(&event);
    
    normalColor = HbColorScheme::color("foreground");
    pressedColor = HbColorScheme::color("popupbackground");
    
    QCOMPARE(mTestPrivate->mPrimaryText->textColor(),normalColor);
    QCOMPARE(mTestPrivate->mSecondaryText->textColor(),normalColor);
    QCOMPARE(mTestPrivate->mPlayIconNormal->color(),normalColor);
    QCOMPARE(mTestPrivate->mPauseIconNormal->color(),normalColor);
    QCOMPARE(mTestPrivate->mPlayIconPressed->color(),pressedColor);
    QCOMPARE(mTestPrivate->mPauseIconPressed->color(),pressedColor);
}

// End of file
