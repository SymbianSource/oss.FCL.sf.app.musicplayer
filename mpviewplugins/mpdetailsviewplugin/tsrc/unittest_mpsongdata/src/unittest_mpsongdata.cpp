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
* Description: Unit test for mpsongdata
*
*/
#include <QSignalSpy>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbicon.h>

#include"unittest_mpsongdata.h"
#include "stub/inc/thumbnailmanager_qt.h"

// Do this so we can access all member variables.
#define private public
#include "mpsongdata.h"
#undef private


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpSongData tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_testmpsongdata.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

//Constructor
TestMpSongData::TestMpSongData()
    : mTest(0)
{

}

//Destructor
TestMpSongData::~TestMpSongData()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSongData::initTestCase()
{
    
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSongData::cleanupTestCase()
{

}

/*!
 Called before each testfunction is executed.
 */
void TestMpSongData::init()
{
    mTest = new MpSongData();
}

/*!
 Called after every testfunction.
 */
void TestMpSongData::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests correct cleanup of member variables.
 */
void TestMpSongData::testMemberCleanup()
{
    cleanup();
    ThumbnailManager::resetInitCounter();
    init();
    cleanup();
    QCOMPARE(ThumbnailManager::getInitCounter(), 0);
}

/*!
 Tests that albumArt() returns NULL
 */
void TestMpSongData::testAlbumArtIsNull()
    {
    QPixmap dummyAlbumArt;
    mTest->albumArt(dummyAlbumArt);
    QVERIFY(dummyAlbumArt.isNull() == true); // TODO: Hkn: What's the point of this ?
    }

/*!
 Tests that albumArt() returns correct QPixmap when not NULL.
 */

void TestMpSongData::testAlbumArtNotNull()
    {
    QPixmap dummyAlbumArt;
    mTest->mAlbumArt = QPixmap( ":/playbackviewicons/someAlbumArt.png" );
    mTest->albumArt(dummyAlbumArt);
    }

/*!
 Tests that title() returns NULL
 */
void TestMpSongData::testTitleIsNull()
    {
     QVERIFY(mTest->mTitle.isNull() == true);
    }

/*!
 Tests that album() returns NULL
 */
void TestMpSongData::testAlbumIsNull()
    {
     QVERIFY(mTest->mAlbum.isNull() == true);
    }

/*!
 Tests that artist() returns NULL
 */
void TestMpSongData::testArtistIsNull()
    {
     QVERIFY(mTest->mArtist.isNull() == true);
    }

/*!
 Tests that comment() returns NULL
 */
void TestMpSongData::testCommentIsNull()
    {
     QVERIFY(mTest->mComment.isNull() == true);
    }

/*!
 Tests that composer() returns NULL
 */
void TestMpSongData::testComposerIsNull()
    {
     QVERIFY(mTest->mComposer.isNull() == true);
    }

/*!
 Tests that genre() returns NULL
 */
void TestMpSongData::testGenreIsNull()
    {
     QVERIFY(mTest->mGenre.isNull() == true);
    }


/*!
 Tests that albumTrack() returns NULL
 */
void TestMpSongData::testAlbumtrackIsNull()
    {
     QVERIFY(mTest->mAlbumTrack.isNull() == true);
    }

/*!
 Tests that link() returns NULL
 */
void TestMpSongData::testLinkIsNull()
    {
     QVERIFY(mTest->mLink.isNull() == true);
    }

/*!
 Tests that year() returns NULL
 */
void TestMpSongData::testYearIsNull()
    {
     QVERIFY(mTest->mYear.isNull() == true);
    }

/*!
 Tests that setTitle() returns TRUE
 */
void TestMpSongData::testSetTitleTrue()
    {
    bool result;
    QString title("SomeTitle");
    result=mTest->setTitle( title );
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setTitle() returns FALSE
 */
void TestMpSongData::testSetTitleFalse()
    {
    bool result;
    QString title(NULL);
    result=mTest->setTitle( title );
    QCOMPARE(result, false);
    }

/*!
 Tests that setAlbum() returns TRUE
 */
void TestMpSongData::testsetAlbumTrue()
    {
    bool result;
    QString album("Some");
    result=mTest->setAlbum(album);
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setAlbum() returns FALSE
 */
void TestMpSongData::testsetAlbumFalse()
    {
    bool result;
    QString album(NULL);
    result=mTest->setAlbum( album );
    QCOMPARE(result, false);
    }


/*!
 Tests that setArtist() returns TRUE
 */
void TestMpSongData::testsetArtistTrue()
    {
    bool result;
    QString artist("SomeArtist");
    result=mTest->setArtist( artist );
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setArtist() returns FALSE
 */
void TestMpSongData::testsetArtistFalse()
    {
    bool result;
    QString artist(NULL);
    result=mTest->setArtist( artist );
    QCOMPARE(result, false);
    }

/*!
 Tests that setComment() returns TRUE
 */
void TestMpSongData::testsetCommentTrue()
    {
    bool result;
    QString comment("SomeComment");
    result=mTest->setComment(comment);
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setComment() returns FALSE
 */
void TestMpSongData::testsetCommentFalse()
    {
    bool result;
    QString comment(NULL);
    result=mTest->setComment(comment);
    QCOMPARE(result, false);
    }

/*!
 Tests that setComposer() returns TRUE
 */
void TestMpSongData::testsetComposerTrue()
    {
    bool result;
    QString composer("SomeComposer");
    result=mTest->setComposer(composer);
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setComposer() returns FALSE
 */
void TestMpSongData::testsetComposerFalse()
    {
    bool result;
    QString composer(NULL);
    result=mTest->setComposer( composer );
    QCOMPARE(result, false);
    }

/*!
 Tests that setGenre() returns TRUE
 */
void TestMpSongData::testsetGenreTrue()
    {
    bool result;
    QString genre("SomeGenre");
    result=mTest->setGenre(genre);
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setGenre() returns FALSE
 */
void TestMpSongData::testsetGenreFalse()
    {
    bool result;
    QString genre(NULL);
    result=mTest->setGenre( genre );
    QCOMPARE(result, false);
    }

/*!
 Tests that setYear() returns TRUE
 */
void TestMpSongData::testsetYearTrue()
    {
    bool result;
    int year = 2010;
    result=mTest->setYear(year);
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setYear() returns FALSE
 */
void TestMpSongData::testsetYearFalse()
    {
    /*    
    TODO: Fix this
    bool result;
    int year(0);
    result=mTest->setYear( year );
    QCOMPARE(result, false);
    */
    }

/*!
 Tests that setAlbumTrack() returns TRUE
 */
void TestMpSongData::testsetAlbumTrackTrue()
    {
    bool result;
    QString albumTrack("SomeAlbumTrack");
    result=mTest->setAlbumTrack(albumTrack);
    QCOMPARE(result, true);
     
    }

/*!
 Tests that setAlbumTrack() returns FALSE
 */
void TestMpSongData::testsetAlbumTrackFalse()
    {
    bool result;
    QString albumTrack(NULL);
    result=mTest->setAlbumTrack( albumTrack );
    QCOMPARE(result, false);
    }

/*!
 Tests setAlbumArtUri() 
 */
void TestMpSongData::testsetAlbumArtUri() 
    {
    const QString albumArtUri("AlbumArt");
    const QString albumArtUriEmpty("");
    
    QSignalSpy spy( mTest, SIGNAL( albumArtReady() ) );
    
    QVERIFY( spy.isValid() );
    QCOMPARE( spy.count(), 0 );
    
    mTest->setAlbumArtUri(albumArtUri);
    QCOMPARE( spy.count(), 0 );
    
    mTest->mThumbnailManager->mGetThumbFails=true;
    mTest->setAlbumArtUri(albumArtUri);
    QCOMPARE( spy.count(), 1 );
    QCOMPARE(mTest->mAlbumArt, mTest->mDefaultAlbumArt);
    
    mTest->setAlbumArtUri(albumArtUriEmpty);
    QCOMPARE( spy.count(), 2 );
    QCOMPARE(mTest->mAlbumArt, mTest->mDefaultAlbumArt);
    
    }

/*!
 Tests thumbnailReady()() 
 */
void TestMpSongData::testThumbnailReady()
{
   connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
           mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );    

   QSignalSpy spy(mTest, SIGNAL(albumArtReady()));
   QVERIFY( spy.isValid() );
   QCOMPARE( spy.count(), 0 );
   
   QPixmap dummyAlbumArt(":/playbackviewicons/someAlbumArt.png" );
   
   emit thumbnailReady(dummyAlbumArt, 0, -1, 0);
   QCOMPARE( spy.count(), 1 );
   QCOMPARE( mTest->mAlbumArt, dummyAlbumArt );
   
   emit thumbnailReady(dummyAlbumArt, 0, -1 , 1);
   QCOMPARE( spy.count(), 2 );
   QCOMPARE( mTest->mAlbumArt, mTest->mDefaultAlbumArt );
   
}


/*!
 Tests commitPlaybackInfo() 
 */
void TestMpSongData::testCommitPlaybackInfo() 
    {
    QSignalSpy spy( mTest, SIGNAL( playbackInfoChanged() ) );
    
    QVERIFY( spy.isValid() );
    QCOMPARE( spy.count(), 0 );
    
    mTest->commitPlaybackInfo();
    QCOMPARE( spy.count(), 1 );
    
    }

/*!
 Tests commitSongDetailInfo() 
 */
void TestMpSongData::testCommitSongDetailInfo() 
    {
    QSignalSpy spy( mTest, SIGNAL( songDetailInfoChanged() ) );
    
    QVERIFY( spy.isValid() );
    QCOMPARE( spy.count(), 0 );
    
    mTest->commitSongDetailInfo();
    QCOMPARE( spy.count(), 1 );
    
    }
