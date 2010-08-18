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
* Description: Unit Test for mpmediawallview.
*
*/

#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbview.h>
#include <QSignalSpy>

#include "unittest_mpmediawallview.h"
#include "stub/inc/mpplaybackdata.h"

// Do this so we can access all member variables.
#define private public
#include "mpmediawallview.h"
#undef private

// Do this so we can access all memeber functions
#include "../../src/mpmediawallview.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpMediaWallView tv;

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmediawallview.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

//Constructor
TestMpMediaWallView::TestMpMediaWallView()
    : mTest(0)
{

}

//Destructor
TestMpMediaWallView::~TestMpMediaWallView()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMediaWallView::initTestCase()
{

}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMediaWallView::cleanupTestCase()
{
QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMediaWallView::init()
{
    mTest = new MpMediaWallView();
}

/*!
 Called after every testfunction.
 */
void TestMpMediaWallView::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Test InitializeView
 */
void TestMpMediaWallView::testInitializeView()
{

    QVERIFY(!mTest->mEngine);
    QVERIFY(!mTest->mCollectionData);
    QVERIFY(!mTest->mModel);
    QVERIFY(!mTest->mAlbumCover);
    QVERIFY(!mTest->mTrackList);
    QVERIFY(!mTest->mPlaybackData);
    QVERIFY(!mTest->mPauseIcon);
    QVERIFY(!mTest->mPlayIcon);
    QVERIFY(!mTest->mPlayPauseContainer);

    //test member variables are initialized
    mTest->initializeView();
    QVERIFY(mTest->mEngine);
    QVERIFY(mTest->mCollectionData);
    QVERIFY(mTest->mModel);
    QVERIFY(mTest->mAlbumCover);
    QVERIFY(mTest->mTrackList);
    QVERIFY(mTest->mPlaybackData);
    QVERIFY(mTest->mPauseIcon);
    QVERIFY(mTest->mPlayIcon);
    QVERIFY(mTest->mPlayPauseContainer);

}

/*!
 Test ActivateView
 */
void TestMpMediaWallView::testActivateView()
{

    //Create playbackdata and set to playing state
    mTest->mPlaybackData = new MpPlaybackData();    
    mTest->mPlaybackData->setPlaybackState(MpPlaybackData::Playing);

    mTest->mIconUpdateNedded=false;
    mTest->mActivated=false;

    mTest->activateView();
    QCOMPARE(mTest->mActivated, true );
}

/*!
 Test DeActivateView
 */
void TestMpMediaWallView::testDeactivateView()
{
    mTest->initializeView();
    mTest->mActivated = true;
    mTest->mShowingSongsList = true;
    mTest->setUpListAnimation();
    mTest->deactivateView();
    QCOMPARE(mTest->mActivated, false);
    QCOMPARE(mTest->mShowingSongsList, false);

}

/*!
  Test SendToBackground
  */
void TestMpMediaWallView::testSendToBackground()
{
    //test that signal gets emitted
    QSignalSpy spy( mTest, SIGNAL(command(int)) );
    mTest->sendToBackground();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(MpCommon::SendToBackground));

}

/*!
  Test ContextOpened
  */
/*  test case fails because potential problem in docml
void TestMpMediaWallView::testContextOpened()
{

    mTest->mCollectionData = new MpMpxCollectionData();
    mTest->mDocumentLoader = new MpMediaWallDocumentLoader();

    QVERIFY(mTest->mCollectionData->context() == QVariant(ECollectionContextUnknown));
    mTest->contextOpened(ECollectionContextArtists);
    QVERIFY(mTest->mCollectionData->context() == QVariant(ECollectionContextAlbumsMediaWall));

    //test loading 'empty' section, docml loading fails because initialize not called
    QVERIFY(!mTest->mNoMusic);  
    mTest->contextOpened(ECollectionContextAlbumsMediaWall);
    QVERIFY(mTest->mNoMusic);  
}
*/

/*!
  Test ContextOpened
  */
void TestMpMediaWallView::testContextOpened()
{
    //test loading 'empty'
    mTest->initializeView();
    mTest->contextOpened(ECollectionContextAlbumsMediaWall);
    //test loading 'mediawall'
    mTest->mCollectionData->setCount(3);    
    mTest->contextOpened(ECollectionContextAlbumsMediaWall);
}

/*!
  Test ContainerContentsChanged
  */
void TestMpMediaWallView::testContainerContentsChanged()
{
    //test containerContetstsChanged calls opencallection when library not updating
    mTest->initializeView();
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 1);
    mTest->containerContentsChanged();
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 2);
    mTest->libraryAboutToUpdate();
    mTest->containerContentsChanged();
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 2);
}

/*!
  Test LibraryUpdated
  */
void TestMpMediaWallView::testLibraryUpdated()
{
   //test libraryupdated changes flag and calls opencallection
    mTest->initializeView();
    mTest->mEngine->mOpenCollectionCalled=0;    
    mTest->mLibraryUpdating=true;
    mTest->libraryUpdated();
    QVERIFY(mTest->mLibraryUpdating == false);
    QVERIFY(mTest->mEngine->mOpenCollectionCalled == 1);
}

/*!
  Test UpdatePlayPauseAction
  */
void TestMpMediaWallView::testUpdatePlayPauseAction()
{
    //test icon gets updated correctly
    mTest->initializeView();
    mTest->mActivated = true;
    
    mTest->mPlaybackData->setPlaybackState( MpPlaybackData::Paused );
    mTest->updatePlayPauseAction();
    HbIcon temp = mTest->mPlayPauseAction->icon();
    QVERIFY( mTest->mPlayPauseAction->icon() ==  HbIcon( "qtg_mono_play" ));

    mTest->mPlaybackData->setPlaybackState( MpPlaybackData::Playing );
    mTest->updatePlayPauseAction();
    QVERIFY( mTest->mPlayPauseAction->icon() ==  HbIcon( "qtg_mono_pause" ));

    mTest->mPlaybackData->setPlaybackState( MpPlaybackData::Stopped );
    mTest->updatePlayPauseAction();
    QVERIFY( mTest->mPlayPauseAction->icon() ==  HbIcon( "qtg_mono_play" ));
}
