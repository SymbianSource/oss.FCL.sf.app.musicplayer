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
* Description: Unit test for mpnowplayingbackend
*
*/

#include <QSignalSpy>
#include <QMetaType>

#include "unittest_mpnowplayingbackend.h"
#include "mpcommondefs.h"
#include "stub/inc/mpxplaybackutility.h"

// Do this so we can access all member variables.
#define private public
#include "mpnowplayingbackend.h"
#include "mpnowplayingbackend_p.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{

    TestMpNowPlayingBackEnd tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpnowplayingbackend.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpNowPlayingBackEnd::TestMpNowPlayingBackEnd()
    : mTest(0)
{
}

TestMpNowPlayingBackEnd::~TestMpNowPlayingBackEnd()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpNowPlayingBackEnd::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpNowPlayingBackEnd::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpNowPlayingBackEnd::init()
{
    mTest = new MpNowPlayingBackEnd(MpCommon::KMusicPlayerUid);
    mTestPrivate = mTest->d_ptr;

    MMPXSource::setMediaLeave(false);
    MMPXPlaybackUtility::setUtilityLeave(false);
    MMPXPlaybackUtility::setAddObserverLeave(false);
    MMPXPlaybackUtility::setRemoveObserverLeave(false);
    MMPXPlaybackUtility::setCommandLeave(false);
    MMPXPlaybackUtility::setStateLeave(false);
    MMPXPlaybackUtility::setSource(true);
}

/*!
 Called after every testfunction.
 */
void TestMpNowPlayingBackEnd::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor. Good case.
 */
void TestMpNowPlayingBackEnd::testConstructor()
{
    QVERIFY(mTestPrivate->mPlaybackUtility != 0);
    QCOMPARE(mTestPrivate->mPreviousState, NotPlaying);
}

/*!
 Tests constructor.
 - MMPXPlaybackUtility::UtilityL leaves.
 */
void TestMpNowPlayingBackEnd::testInitUtilCreateFail()
{
    // AK - There is no way for the client to tell if constructor fails.
    // This should be split into constructor and initialize.
    cleanup();
    MMPXPlaybackUtility::setUtilityLeave(true);
    init();
    QVERIFY(mTestPrivate->mPlaybackUtility == 0);
}

/*!
 Tests constructor.
 - MMPXPlaybackUtility::AddObserverL leaves.
 */
void TestMpNowPlayingBackEnd::testInitAddObserverFail()
{
    cleanup();
    MMPXPlaybackUtility::resetCloseCounter();
    MMPXPlaybackUtility::setAddObserverLeave(true);
    init();
    QVERIFY(mTestPrivate->mPlaybackUtility == 0);
    QCOMPARE(MMPXPlaybackUtility::getCloseCounter(), 1);
}

/*!
 Tests destructor. Good case.
 */
void TestMpNowPlayingBackEnd::testDestructor()
{
    MMPXPlaybackUtility::resetCloseCounter();
    MMPXPlaybackUtility::resetRemoveObserverCounter();

    QVERIFY(mTestPrivate->mPlaybackUtility->iObserver != 0);

    cleanup();
    QCOMPARE(MMPXPlaybackUtility::getRemoveObserverCounter(), 1);
    QCOMPARE(MMPXPlaybackUtility::getCloseCounter(), 1);
}

/*!
 Tests destructor.
 - MMPXPlaybackUtility::RemoveObserverL leaves.
 */
void TestMpNowPlayingBackEnd::testDestructorRemoveObserverFail()
{
    MMPXPlaybackUtility::resetCloseCounter();
    MMPXPlaybackUtility::resetRemoveObserverCounter();
    MMPXPlaybackUtility::setRemoveObserverLeave(true);

    QVERIFY(mTestPrivate->mPlaybackUtility->iObserver != 0);

    cleanup();
    QCOMPARE(MMPXPlaybackUtility::getRemoveObserverCounter(), 0);
    QCOMPARE(MMPXPlaybackUtility::getCloseCounter(), 1);
}

/*!
 Tests update() slot. Good case.
 */
void TestMpNowPlayingBackEnd::testUpdateSuccess()
{
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    connect(this, SIGNAL(update()), mTest, SLOT(update()));
    MMPXSource::resetMediaCounter();


    MMPXPlaybackUtility::setState(EPbStatePlaying);
    // This should trigger stateUpdate(Playing) signal
    emit update();
    // Same state; This shouldn't trigger stateUpdate() signal
    emit update();

    MMPXPlaybackUtility::setState(EPbStatePaused);
    // This should trigger stateUpdate(Paused) signal
    emit update();
    // Same state; This shouldn't trigger stateUpdate() signal
    emit update();
    
    MMPXPlaybackUtility::setState(EPbStateStopped);
    //Stop is reinterpreted as Pause.
    // Same state; This shouldn't trigger stateUpdate() signal
    emit update();


    QCOMPARE(spy.count(), 2);
    SimplifiedPlayerState state = qvariant_cast<SimplifiedPlayerState>(spy.at(0).at(0));
    QCOMPARE(state, Playing);
    state = qvariant_cast<SimplifiedPlayerState>(spy.at(1).at(0));
    QCOMPARE(state, Paused);


    QCOMPARE(MMPXSource::getMediaCounter(), 5);

    disconnect(this, SIGNAL(update()), mTest, SLOT(update()));
}

/*!
 Tests update() slot.
 - MMPXPlaybackUtility::Source returns NULL.
 */
void TestMpNowPlayingBackEnd::testUpdateNoSource()
{
    MMPXPlaybackUtility::setSource(false);
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    connect(this, SIGNAL(update()), mTest, SLOT(update()));
    MMPXSource::resetMediaCounter();

    MMPXPlaybackUtility::setState(EPbStateStopped);
    emit update();

    QCOMPARE(spy.count(), 1);
    SimplifiedPlayerState state = qvariant_cast<SimplifiedPlayerState>(spy.at(0).at(0));
    QCOMPARE(state, NotPlaying);

    QCOMPARE(MMPXSource::getMediaCounter(), 0);
    disconnect(this, SIGNAL(update()), mTest, SLOT(update()));
}

/*!
 Tests update() slot.
 - MMPXPlaybackUtility::StateL leaves.
 */
void TestMpNowPlayingBackEnd::testUpdateStateFail()
{
    MMPXPlaybackUtility::setStateLeave(true);
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    connect(this, SIGNAL(update()), mTest, SLOT(update()));
    MMPXSource::resetMediaCounter();

    MMPXPlaybackUtility::setState(EPbStatePaused);
    emit update();

    QCOMPARE(spy.count(), 0);
    QCOMPARE(MMPXSource::getMediaCounter(), 0);
    disconnect(this, SIGNAL(update()), mTest, SLOT(update()));
}

/*!
 Tests update() slot.
 - MMPXSource::MediaL leaves.
 */
void TestMpNowPlayingBackEnd::testUpdateMediaFail()
{
    MMPXSource::setMediaLeave(true);
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    connect(this, SIGNAL(update()), mTest, SLOT(update()));
    MMPXSource::resetMediaCounter();

    MMPXPlaybackUtility::setState(EPbStatePaused);
    emit update();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(MMPXSource::getMediaCounter(), 0);
    disconnect(this, SIGNAL(update()), mTest, SLOT(update()));
}

/*!
 Tests playPause() slot. Good case.
 */
void TestMpNowPlayingBackEnd::testPlayPause()
{
    MMPXPlaybackUtility::setState(EPbStatePlaying);
    connect(this, SIGNAL(playPause()), mTest, SLOT(playPause()));

    emit playPause();
    QCOMPARE(MMPXPlaybackUtility::getState(), EPbStatePaused);

    emit playPause();
    QCOMPARE(MMPXPlaybackUtility::getState(), EPbStatePlaying);

    disconnect(this, SIGNAL(playPause()), mTest, SLOT(playPause()));
}

/*!
 Tests playPause() slot.
 - MMPXPlaybackUtility::CommandL leaves.
 */
void TestMpNowPlayingBackEnd::testPlayPauseFail()
{
    MMPXPlaybackUtility::setCommandLeave(true);
    MMPXPlaybackUtility::setState(EPbStatePlaying);
    connect(this, SIGNAL(playPause()), mTest, SLOT(playPause()));
    emit playPause();
    // Just need to make sure nothing crashes.
    QCOMPARE(MMPXPlaybackUtility::getState(), EPbStatePlaying);
    disconnect(this, SIGNAL(playPause()), mTest, SLOT(playPause()));
}

/*!
 Tests HandlePlaybackMessage callback with StateChanged event. Good case.
 */
void TestMpNowPlayingBackEnd::testHandlePlaybackMessageStateChanged()
{
    MMPXPlaybackUtility::setState(EPbStatePlaying);
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    mTestPrivate->mPlaybackUtility->sendHandlePlaybackMessage(TMPXPlaybackMessage::EStateChanged);

    QCOMPARE(spy.count(), 1);
    SimplifiedPlayerState state = qvariant_cast<SimplifiedPlayerState>(spy.at(0).at(0));
    QCOMPARE(state, Playing);
}

/*!
 Tests HandlePlaybackMessage callback with StateChanged event.
 - MMPXPlaybackUtility::StateL leaves.
 */
void TestMpNowPlayingBackEnd::testHandlePlaybackMessageStateChangedFail()
{
    MMPXPlaybackUtility::setStateLeave(true);
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    mTestPrivate->mPlaybackUtility->sendHandlePlaybackMessage(TMPXPlaybackMessage::EStateChanged);

    QCOMPARE(spy.count(), 0);
}

/*!
 Tests HandlePlaybackMessage callback with PlaylistUpdated event. Good case.
 */
void TestMpNowPlayingBackEnd::testHandlePlaybackMessagePlaylistUpdated()
{
    MMPXSource::resetMediaCounter();
    mTestPrivate->mPlaybackUtility->sendHandlePlaybackMessage(TMPXPlaybackMessage::EPlaylistUpdated);
    QCOMPARE(MMPXSource::getMediaCounter(), 1);
}

/*!
 Tests HandlePlaybackMessage callback with MediaChanged event. Good case.
 */
void TestMpNowPlayingBackEnd::testHandlePlaybackMessageMediaChanged()
{
    MMPXSource::resetMediaCounter();
    mTestPrivate->mPlaybackUtility->sendHandlePlaybackMessage(TMPXPlaybackMessage::EMediaChanged);
    QCOMPARE(MMPXSource::getMediaCounter(), 1);
}

/*!
 Tests HandlePlaybackMessage callback with PlayerChanged event. Good case.
 */
void TestMpNowPlayingBackEnd::testHandlePlaybackMessagePlayerChanged()
{
    MMPXPlaybackUtility::setState(EPbStatePlaying);
    MMPXSource::resetMediaCounter();
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    mTestPrivate->mPlaybackUtility->sendHandlePlaybackMessage(TMPXPlaybackMessage::EActivePlayerChanged);

    QCOMPARE(spy.count(), 1);
    SimplifiedPlayerState state = qvariant_cast<SimplifiedPlayerState>(spy.at(0).at(0));
    QCOMPARE(state, Playing);
    QCOMPARE(MMPXSource::getMediaCounter(), 1);
}

/*!
 Tests HandlePlaybackMessage callback with PropertyChanged event. Good case.
 Tests receiving events that it doesn't care about.
 */
void TestMpNowPlayingBackEnd::testHandlePlaybackMessagePropertyChanged()
{
    MMPXPlaybackUtility::setState(EPbStatePlaying);
    MMPXSource::resetMediaCounter();
    qRegisterMetaType<SimplifiedPlayerState>();
    QSignalSpy spy(mTest, SIGNAL(stateUpdate(SimplifiedPlayerState)));
    mTestPrivate->mPlaybackUtility->sendHandlePlaybackMessage(TMPXPlaybackMessage::EPropertyChanged);

    QCOMPARE(spy.count(), 0);
    QCOMPARE(MMPXSource::getMediaCounter(), 0);
}

/*!
 Tests HandleMediaL callback with new title. Good case.
 */
void TestMpNowPlayingBackEnd::testHandleMediaLTitle()
{
    QSignalSpy spyTitle(mTest, SIGNAL(titleChanged(QString)));
    QSignalSpy spyArtist(mTest, SIGNAL(artistChanged(QString)));
    mTestPrivate->mPreviousState = Playing;
    mTestPrivate->mPlaybackUtility->Source()->setObserver(mTestPrivate);
    mTestPrivate->mPlaybackUtility->Source()->sendHandleMediaL(true, false, false);

    QCOMPARE(spyTitle.count(), 1);
    QCOMPARE(spyTitle.at(0).at(0).toString(), QString("Title"));
    QCOMPARE(spyArtist.count(), 1);
    QCOMPARE(spyArtist.at(0).at(0).toString(), QString());
}

/*!
 Tests HandleMediaL callback with new title and uri. Good case.
 When both title and uri exist, only title is used.
 */
void TestMpNowPlayingBackEnd::testHandleMediaLTitleUri()
{
    QSignalSpy spyTitle(mTest, SIGNAL(titleChanged(QString)));
    QSignalSpy spyArtist(mTest, SIGNAL(artistChanged(QString)));
    mTestPrivate->mPreviousState = Playing;
    mTestPrivate->mPlaybackUtility->Source()->setObserver(mTestPrivate);
    mTestPrivate->mPlaybackUtility->Source()->sendHandleMediaL(true, true, false);

    QCOMPARE(spyTitle.count(), 1);
    QCOMPARE(spyTitle.at(0).at(0).toString(), QString("Title"));
    QCOMPARE(spyArtist.count(), 1);
    QCOMPARE(spyArtist.at(0).at(0).toString(), QString());
}

/*!
 Tests HandleMediaL callback with new uri. Good case.
 */
void TestMpNowPlayingBackEnd::testHandleMediaLUri()
{
    QSignalSpy spyTitle(mTest, SIGNAL(titleChanged(QString)));
    QSignalSpy spyArtist(mTest, SIGNAL(artistChanged(QString)));
    mTestPrivate->mPreviousState = Playing;
    mTestPrivate->mPlaybackUtility->Source()->setObserver(mTestPrivate);
    mTestPrivate->mPlaybackUtility->Source()->sendHandleMediaL(false, true, false);

    QCOMPARE(spyTitle.count(), 1);
    QCOMPARE(spyTitle.at(0).at(0).toString(), QString("Uri"));
    QCOMPARE(spyArtist.count(), 1);
    QCOMPARE(spyArtist.at(0).at(0).toString(), QString());
}

/*!
 Tests HandleMediaL callback with new artist. Good case.
 */
void TestMpNowPlayingBackEnd::testHandleMediaLArtist()
{
    QSignalSpy spyTitle(mTest, SIGNAL(titleChanged(QString)));
    QSignalSpy spyArtist(mTest, SIGNAL(artistChanged(QString)));
    mTestPrivate->mPreviousState = Playing;
    mTestPrivate->mPlaybackUtility->Source()->setObserver(mTestPrivate);
    mTestPrivate->mPlaybackUtility->Source()->sendHandleMediaL(false, false, true);

    QCOMPARE(spyTitle.count(), 1);
    QCOMPARE(spyTitle.at(0).at(0).toString(), QString());
    QCOMPARE(spyArtist.count(), 1);
    QCOMPARE(spyArtist.at(0).at(0).toString(), QString("Artist"));
}

// End of file
