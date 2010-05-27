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

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsongdata.txt";

        return QTest::qExec(&tv, 3, pass);
    }    
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
 Test correct cleanup of member variables.
 */
void TestMpSongData::testMemberCleanup()
{
    cleanup();
    ThumbnailManager::resetInitCounter();
    init();
    cleanup();
    QCOMPARE( ThumbnailManager::getInitCounter(), 0 );
}


/*!
 Test title() 
 */
void TestMpSongData::testTitle()
{
    mTest->mTitle = QString();
    QVERIFY( mTest->title().isNull() == true );    
    
    QString title( "title" );
    mTest->mTitle = title;
    QVERIFY( mTest->title().isNull() == false );
    QCOMPARE( mTest->title(), title );
}

/*!
 Test album() 
 */
void TestMpSongData::testAlbum()
{
    mTest->mAlbum = QString();
    QVERIFY( mTest->album().isNull() == true );
    
    QString album( "album" );
    mTest->mAlbum = album;
    QVERIFY( mTest->album().isNull() == false );
    QCOMPARE( mTest->album(), album );
}

/*!
 Test artist() 
 */
void TestMpSongData::testArtist()
{
    mTest->mArtist = QString();
    QVERIFY( mTest->artist().isNull() == true );
    
    QString artist( "artist");
    mTest->mArtist = artist;
    QVERIFY( mTest->artist().isNull() == false );
    QCOMPARE( mTest->artist(), artist );
}

/*!
 Test comment() 
 */
void TestMpSongData::testComment()
{
    mTest->mComment = QString();
    QVERIFY( mTest->comment().isNull() == true );
    
    QString comment( "This is a nice song" );
    mTest->mComment = comment;
    QVERIFY( mTest->comment().isNull() == false );
    QCOMPARE( mTest->comment(), comment );
}

/*!
 Test albumArt() 
 */
void TestMpSongData::testAlbumArt()
{
    HbIcon dummyAlbumArt;
    mTest->albumArt( dummyAlbumArt );
    QVERIFY( dummyAlbumArt.isNull() == true );
    
    HbIcon dummyAlbumArtTwo;
    mTest->mAlbumArt = new HbIcon( ":/playbackviewicons/someAlbumArt.png" );
    mTest->albumArt( dummyAlbumArtTwo );
    QVERIFY( dummyAlbumArtTwo.isNull() == false );
}

/*!
 Test year() 
 */
void TestMpSongData::testYear()
{
    mTest->mYear = QString();
    QVERIFY( mTest->year().isNull() == true );
    
    QString year("2000");
    mTest->mYear = year;
    QVERIFY( mTest->year().isNull() == false );
    QCOMPARE( mTest->year(), year );
}

/*!
 Test genre() 
 */
void TestMpSongData::testGenre()
{
    mTest->mGenre = QString();    
    QVERIFY( mTest->genre().isNull() == true );

    QString genre( "pop" );    
    mTest->mGenre = genre;    
    QVERIFY( mTest->genre().isNull() == false );
    QCOMPARE( mTest->genre(), genre );
}

/*!
 Test composer() 
 */
void TestMpSongData::testComposer()
{
    mTest->mComposer = QString();
    QVERIFY( mTest->composer().isNull() == true );
    
    QString composer( "composer ");
    mTest->mComposer = composer;
    QVERIFY( mTest->composer().isNull() == false );
    QCOMPARE( mTest->composer(), composer );
}


/*!
 Test albumTrack() 
 */
void TestMpSongData::testAlbumtrack()
{
    mTest->mAlbumTrack = QString();
    QVERIFY( mTest->albumTrack().isNull() == true );
    
    QString albumTrack("2");
    mTest->mAlbumTrack = albumTrack;
    QVERIFY( mTest->albumTrack().isNull() == false );   
    QCOMPARE( mTest->albumTrack(), albumTrack );
}

/*!
 Test link() 
 */
void TestMpSongData::testLink()
{
    mTest->mLink = QString();
    QVERIFY( mTest->link().isNull() == true );
    
    QString link( "www.nokia.com" );
    mTest->mLink = link;
    QVERIFY( mTest->link().isNull() == false );
    QCOMPARE( mTest->link(), link );
}

/*!
 Test fileName()
 */
void TestMpSongData::testFileName()
{
    mTest->mFileName = QString();
    QVERIFY( mTest->fileName().isNull() == true );
    
    QString fileName( "test.mp3" );
    mTest->mFileName = fileName;
    QVERIFY( mTest->fileName().isNull() == false );
    QCOMPARE( mTest->fileName(), fileName );
}

/*!
 Test fileName()
 */
void TestMpSongData::testMimeType()
{
    mTest->mMimeType = QString();
    QVERIFY( mTest->mimeType().isNull() == true );
    
    QString mimeType( "mp3" );
    mTest->mMimeType = mimeType;
    QVERIFY( mTest->mimeType().isNull() == false );
    QCOMPARE( mTest->mimeType(), mimeType );
}

/*!
 Test fileName()
 */
void TestMpSongData::testDuration()
{
    mTest->mDuration = QString();
    QVERIFY( mTest->duration().isNull() == true );
    
    QString duration( "04:16" );
    mTest->mDuration = duration;
    QVERIFY( mTest->duration().isNull() == false );
    QCOMPARE( mTest->duration(), duration );
}

/*!
 Test bitRate()
 */
void TestMpSongData::testBitRate()
{
    mTest->mBitRate = QString();
    QVERIFY( mTest->bitRate().isNull() == true );
    
    QString bitRate( "320 Kbps" );
    mTest->mBitRate = bitRate;
    QVERIFY( mTest->bitRate().isNull() == false );
    QCOMPARE( mTest->bitRate(), bitRate );
}

/*!
 Test sampleRate()
 */
void TestMpSongData::testSampleRate()
{
    mTest->mSampleRate = QString();
    QVERIFY( mTest->sampleRate().isNull() == true );
    
    QString sampleRate( "44100 hz" );
    mTest->mSampleRate = sampleRate;
    QVERIFY( mTest->sampleRate().isNull() == false );
    QCOMPARE( mTest->sampleRate(), sampleRate );
}

/*!
 Test size()
 */
void TestMpSongData::testSize()
{
    mTest->mSize = QString();
    QVERIFY( mTest->size().isNull() == true );
    
    QString size( "4.3MB" );
    mTest->mSize = size;
    QVERIFY( mTest->size().isNull() == false );
    QCOMPARE( mTest->size(), size );
}

/*!
 Test modified()
 */
void TestMpSongData::testModified()
{
    mTest->mModified = QString();
    QVERIFY( mTest->modified().isNull() == true );
    
    QString modified( "5.7.2010 14:35:08" );
    mTest->mModified = modified;
    QVERIFY( mTest->modified().isNull() == false );
    QCOMPARE( mTest->modified(), modified );
}

/*!
 Test copyright()
 */
void TestMpSongData::testCopyright()
{
    mTest->mCopyright = QString();
    QVERIFY( mTest->copyright().isNull() == true );
    
    QString copyright( "Copyright holder" );
    mTest->mCopyright = copyright;
    QVERIFY( mTest->copyright().isNull() == false );
    QCOMPARE( mTest->copyright(), copyright );
}

/*!
 Test musicURL()
 */
void TestMpSongData::testMusicURL()
{
    mTest->mMusicURL = QString();
    QVERIFY( mTest->musicURL().isNull() == true );
    
    QString musicURL( "www.nokia.com" );
    mTest->mMusicURL = musicURL;
    QVERIFY( mTest->musicURL().isNull() == false );
    QCOMPARE( mTest->musicURL(), musicURL );
}

/*!
 Test isDrmProtected()
 */
void TestMpSongData::testIsDrmProtected()
{
    mTest->mDrmProtected = true;
    QVERIFY( mTest->isDrmProtected() == true );
    
    mTest->mDrmProtected = false;
    QVERIFY( mTest->isDrmProtected() == false );
}

/*!
 Test setTitle() 
 */
void TestMpSongData::testSetTitle()
{
    bool result;    
    QString title( "title" );
    mTest->mTitle = QString();
    result = mTest->setTitle( title );
    QCOMPARE( result, true );
    QCOMPARE( mTest->title(), title ); 
    
    result = false;
    title = QString( "title" );
    mTest->mTitle = QString( "titleTwo" );
    result = mTest->setTitle( title );
    QCOMPARE( result, true );
    QCOMPARE( mTest->title(), title ); 
    
    result = false;
    title = QString();
    mTest->mTitle = QString();
    result = mTest->setTitle( title );
    QCOMPARE( result, false );
    QCOMPARE( mTest->title().isNull(), true ); 
    
    result = false;
    title = QString();
    mTest->mTitle = QString( "titleTwo" );
    result = mTest->setTitle( title );
    QCOMPARE( result, true );
    QCOMPARE( mTest->title().isNull(), true ); 
}

/*!
 Test SetAlbum() 
 */
void TestMpSongData::TestMpSongData::testSetAlbum()
{
    bool result;    
    QString album( "album" );
    mTest->mAlbum = QString();
    result = mTest->setAlbum( album );
    QCOMPARE( result, true );
    QCOMPARE( mTest->album(), album ); 
    
    result = false;
    album = QString( "album" );
    mTest->mAlbum = QString( "albumTwo" );
    result = mTest->setAlbum( album );
    QCOMPARE( result, true );
    QCOMPARE( mTest->album(), album ); 
    
    result = false;
    album = QString();
    mTest->mAlbum = QString();
    result = mTest->setAlbum( album );
    QCOMPARE( result, false );
    QCOMPARE( mTest->album().isNull(), true ); 
    
    result = false;
    album = QString();
    mTest->mAlbum = QString( "albumTwo" );
    result = mTest->setAlbum( album );
    QCOMPARE( result, true );
    QCOMPARE( mTest->album().isNull(), true ); 
}

/*!
 Test SetArtist() 
 */
void TestMpSongData::testSetArtist()
{
    bool result;    
    QString artist( "artist" );
    mTest->mArtist = QString();
    result = mTest->setArtist( artist );
    QCOMPARE( result, true );
    QCOMPARE( mTest->artist(), artist ); 
    
    result = false;
    artist = QString( "artist" );
    mTest->mArtist = QString( "artistTwo" );
    result = mTest->setArtist( artist );
    QCOMPARE( result, true );
    QCOMPARE( mTest->artist(), artist ); 
    
    result = false;
    artist = QString();
    mTest->mArtist = QString();
    result = mTest->setArtist( artist );
    QCOMPARE( result, false );
    QCOMPARE( mTest->artist().isNull(), true ); 
    
    result = false;
    artist = QString();
    mTest->mArtist = QString( "artistTwo" );
    result = mTest->setArtist( artist );
    QCOMPARE( result, true );
    QCOMPARE( mTest->artist().isNull(), true ); 
}

/*!
 Test SetComment() 
 */
void TestMpSongData::testSetComment()
{
    bool result;    
    QString comment( "comment" );
    mTest->mComment = QString();
    result = mTest->setComment( comment );
    QCOMPARE( result, true );
    QCOMPARE( mTest->comment(), comment ); 
    
    result = false;
    comment = QString( "comment" );
    mTest->mComment = QString( "commentTwo" );
    result = mTest->setComment( comment );
    QCOMPARE( result, true );
    QCOMPARE( mTest->comment(), comment ); 
    
    result = false;
    comment = QString();
    mTest->mComment = QString();
    result = mTest->setComment( comment );
    QCOMPARE( result, false );
    QCOMPARE( mTest->comment().isNull(), true ); 
    
    result = false;
    comment = QString();
    mTest->mComment = QString( "commentTwo" );
    result = mTest->setComment( comment );
    QCOMPARE( result, true );
    QCOMPARE( mTest->comment().isNull(), true ); 
}

/*!
 Test SetYear() 
 */
void TestMpSongData::testSetYear()
{
    bool result;    
    int year = 2010;
    mTest->mYear = QString();
    result = mTest->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year(), QString::number( year ) ); 
    
    result = false;
    year = 2010;
    mTest->mYear = QString::number( 2011 );
    result = mTest->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year(), QString::number( year ) ); 
    
    result = false;
    year = -1;
    mTest->mYear = QString();
    result = mTest->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year().isNull(), true ); 
    
    result = false;
    year = -1;
    mTest->mYear = QString::number( 2011 );
    result = mTest->setYear( year );
    QCOMPARE( result, true );
    QCOMPARE( mTest->year().isNull(), true ); 
}

/*!
 Test setGenre() 
 */
void TestMpSongData::testSetGenre()
{
    bool result;    
    QString genre( "genre" );
    mTest->mGenre = QString();
    result = mTest->setGenre( genre );
    QCOMPARE( result, true );
    QCOMPARE( mTest->genre(), genre ); 
    
    result = false;
    genre = QString( "genre" );
    mTest->mGenre = QString( "genreTwo" );
    result = mTest->setGenre( genre );
    QCOMPARE( result, true );
    QCOMPARE( mTest->genre(), genre ); 
    
    result = false;
    genre = QString();
    mTest->mGenre = QString();
    result = mTest->setGenre( genre );
    QCOMPARE( result, false );
    QCOMPARE( mTest->genre().isNull(), true ); 
    
    result = false;
    genre = QString();
    mTest->mGenre = QString( "genreTwo" );
    result = mTest->setGenre( genre );
    QCOMPARE( result, true );
    QCOMPARE( mTest->genre().isNull(), true ); 
}

/*!
 Test setComposer() 
 */
void TestMpSongData::testSetComposer()
{
    bool result;    
    QString composer( "composer" );
    mTest->mComposer = QString();
    result = mTest->setComposer( composer );
    QCOMPARE( result, true );
    QCOMPARE( mTest->composer(), composer ); 
    
    result = false;
    composer = QString( "composer" );
    mTest->mComposer = QString( "composerTwo" );
    result = mTest->setComposer( composer );
    QCOMPARE( result, true );
    QCOMPARE( mTest->composer(), composer ); 
    
    result = false;
    composer = QString();
    mTest->mComposer = QString();
    result = mTest->setComposer( composer );
    QCOMPARE( result, false );
    QCOMPARE( mTest->composer().isNull(), true ); 
    
    result = false;
    composer = QString();
    mTest->mComposer = QString( "composerTwo" );
    result = mTest->setComposer( composer );
    QCOMPARE( result, true );
    QCOMPARE( mTest->composer().isNull(), true ); 
}

/*!
 Test setAlbumTrack() 
 */
void TestMpSongData::testSetAlbumTrack()
{
    bool result;    
    QString albumTrack( "2" );
    mTest->mAlbumTrack = QString();
    result = mTest->setAlbumTrack( albumTrack );
    QCOMPARE( result, true );
    QCOMPARE( mTest->albumTrack(), albumTrack ); 
    
    result = false;
    albumTrack = QString( "2" );
    mTest->mAlbumTrack = QString( "3" );
    result = mTest->setAlbumTrack( albumTrack );
    QCOMPARE( result, true );
    QCOMPARE( mTest->albumTrack(), albumTrack ); 
    
    result = false;
    albumTrack = QString();
    mTest->mAlbumTrack = QString();
    result = mTest->setAlbumTrack( albumTrack );
    QCOMPARE( result, false );
    QCOMPARE( mTest->albumTrack().isNull(), true ); 
    
    result = false;
    albumTrack = QString();
    mTest->mAlbumTrack = QString( "3" );
    result = mTest->setAlbumTrack( albumTrack );
    QCOMPARE( result, true );
    QCOMPARE( mTest->albumTrack().isNull(), true ); 
}

/*!
 Test setLink() 
 */
void TestMpSongData::testSetLink()
{  
    QString link( "www.nokia.com" );
    mTest->mLink = QString();
    mTest->setLink( link );
    QCOMPARE( mTest->link(), link ); 
    
    link = QString( "www.nokia.com" );
    mTest->mLink = QString( "www.nokia.fi" );
    mTest->setLink( link );
    QCOMPARE( mTest->link(), link ); 
    
    link = QString();
    mTest->mLink = QString();
    mTest->setLink( link );
    QCOMPARE( mTest->link().isNull(), true ); 
    
    link = QString();
    mTest->mLink = QString( "www.nokia.fi" );
    mTest->setLink( link );
    QCOMPARE( mTest->link().isNull(), true ); 
}

/*!
 Test setFileName() 
 */
void TestMpSongData::testSetFileName()
{
    bool result;    
    QString fileName( "fileName" );
    mTest->mFileName = QString();
    result = mTest->setFileName( fileName );
    QCOMPARE( result, true );
    QCOMPARE( mTest->fileName(), fileName ); 
    
    result = false;
    fileName = QString( "fileName" );
    mTest->mFileName = QString( "fileNameTwo" );
    result = mTest->setFileName( fileName );
    QCOMPARE( result, true );
    QCOMPARE( mTest->fileName(), fileName ); 
    
    result = false;
    fileName = QString();
    mTest->mFileName = QString();
    result = mTest->setFileName( fileName );
    QCOMPARE( result, false );
    QCOMPARE( mTest->fileName().isNull(), true ); 
    
    result = false;
    fileName = QString();
    mTest->mFileName = QString( "fileNameTwo" );
    result = mTest->setFileName( fileName );
    QCOMPARE( result, true );
    QCOMPARE( mTest->fileName().isNull(), true ); 
}

/*!
 Test setMimeType() 
 */
void TestMpSongData::testSetMimeType()
{
    bool result;    
    QString mimeType( "mimeType" );
    mTest->mMimeType = QString();
    result = mTest->setMimeType( mimeType );
    QCOMPARE( result, true );
    QCOMPARE( mTest->mimeType(), mimeType ); 
    
    result = false;
    mimeType = QString( "mimeType" );
    mTest->mMimeType = QString( "mimeTypeTwo" );
    result = mTest->setMimeType( mimeType );
    QCOMPARE( result, true );
    QCOMPARE( mTest->mimeType(), mimeType ); 
    
    result = false;
    mimeType = QString();
    mTest->mMimeType = QString();
    result = mTest->setMimeType( mimeType );
    QCOMPARE( result, false );
    QCOMPARE( mTest->mimeType().isNull(), true ); 
    
    result = false;
    mimeType = QString();
    mTest->mMimeType = QString( "mimeTypeTwo" );
    result = mTest->setMimeType( mimeType );
    QCOMPARE( result, true );
    QCOMPARE( mTest->mimeType().isNull(), true ); 
}

/*!
 Test setDuration()  // TODO: more cases to add here for different time interval
 */
void TestMpSongData::testSetDuration()
{
    bool result;    
    int duration = 100;
    mTest->mDuration = QString();
    result = mTest->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration(), QString("01:40") ); 
    
    result = false;
    duration = 100;
    mTest->mDuration = QString( "02:00" );
    result = mTest->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration(), QString("01:40") ); 
    
    result = false;
    duration = -1;
    mTest->mDuration = QString();
    result = mTest->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration().isNull(), true ); 
    
    result = false;
    duration = -1;
    mTest->mDuration = QString( "02:00" );
    result = mTest->setDuration( duration );
    QCOMPARE( result, true );
    QCOMPARE( mTest->duration().isNull(), true ); 
}

/*!
 Test setBitRate() 
 */
void TestMpSongData::testSetBitRate()
{
    bool result;    
    int bitRate = 302000;
    mTest->mBitRate = QString();
    result = mTest->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate(), QString::number( bitRate / 1000 ) ); 
    
    result = false;
    bitRate = 302000;
    mTest->mBitRate = QString::number( 412 );
    result = mTest->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate(), QString::number( bitRate / 1000 ) ); 
    
    result = false;
    bitRate = -1;
    mTest->mBitRate = QString();
    result = mTest->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate().isNull(), true ); 
    
    result = false;
    bitRate = -1;
    mTest->mBitRate = QString::number( 412 );
    result = mTest->setBitRate( bitRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->bitRate().isNull(), true ); 
}

/*!
 Test setSampleRate
 */
void TestMpSongData::testSetSampleRate()
{
    bool result;    
    int sampleRate = 44100;
    mTest->mSampleRate = QString();
    result = mTest->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate(), QString::number( sampleRate ) ); 
    
    result = false;
    sampleRate = 44100;
    mTest->mSampleRate = QString::number( 55000 );
    result = mTest->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate(), QString::number( sampleRate ) ); 
    
    result = false;
    sampleRate = -1;
    mTest->mSampleRate = QString();
    result = mTest->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate().isNull(), true ); 
    
    result = false;
    sampleRate = -1;
    mTest->mSampleRate = QString::number( 55000 );
    result = mTest->setSampleRate( sampleRate );
    QCOMPARE( result, true );
    QCOMPARE( mTest->sampleRate().isNull(), true ); 
}

/*!
 Test setSize
 */
void TestMpSongData::testSetSize()
{
    bool result;    
    int size = 4300;
    mTest->mSize = QString();
    result = mTest->setSize( size );
    QCOMPARE( result, true );
    QCOMPARE( mTest->size(), QString::number( size / 1000 ) ); 
    
    result = false;
    size = 4300;
    mTest->mSize = QString( "5" );
    result = mTest->setSize( size );
    QCOMPARE( result, true );
    QCOMPARE( mTest->size(), QString::number( size / 1000 ) );     
}

/*!
 Test setModified
 */
void TestMpSongData::testSetModified()
{
    bool result;    
    QString modified( "5.7.2010 14:35:08" );
    mTest->mModified = QString();
    result = mTest->setModified( modified );
    QCOMPARE( result, true );
    QCOMPARE( mTest->modified(), modified ); 
    
    result = false;
    modified = QString( "5.7.2010 14:35:08" );
    mTest->mModified = QString( "9.7.2010 16:35:08" );
    result = mTest->setModified( modified );
    QCOMPARE( result, true );
    QCOMPARE( mTest->modified(), modified ); 
    
    result = false;
    modified = QString();
    mTest->mModified = QString();
    result = mTest->setModified( modified );
    QCOMPARE( result, false );
    QCOMPARE( mTest->modified().isNull(), true ); 
    
    result = false;
    modified = QString();
    mTest->mModified = QString( "9.7.2010 16:35:08" );
    result = mTest->setModified( modified );
    QCOMPARE( result, true );
    QCOMPARE( mTest->modified().isNull(), true ); 
}

/*!
 Test setCopyright
 */
void TestMpSongData::testSetCopyright()
{
    bool result;    
    QString copyright( "copyright" );
    mTest->mCopyright = QString();
    result = mTest->setCopyright( copyright );
    QCOMPARE( result, true );
    QCOMPARE( mTest->copyright(), copyright ); 
    
    result = false;
    copyright = QString( "copyright" );
    mTest->mCopyright = QString( "copyrightTwo" );
    result = mTest->setCopyright( copyright );
    QCOMPARE( result, true );
    QCOMPARE( mTest->copyright(), copyright ); 
    
    result = false;
    copyright = QString();
    mTest->mCopyright = QString();
    result = mTest->setCopyright( copyright );
    QCOMPARE( result, false );
    QCOMPARE( mTest->copyright().isNull(), true ); 
    
    result = false;
    copyright = QString();
    mTest->mCopyright = QString( "copyrightTwo" );
    result = mTest->setCopyright( copyright );
    QCOMPARE( result, true );
    QCOMPARE( mTest->copyright().isNull(), true ); 
}

/*!
 Test setMusicURL
 */
void TestMpSongData::testSetMusicURL()
{
    bool result;    
    QString musicURL( "musicURL" );
    mTest->mMusicURL = QString();
    result = mTest->setMusicURL( musicURL );
    QCOMPARE( result, true );
    QCOMPARE( mTest->musicURL(), musicURL ); 
    
    result = false;
    musicURL = QString( "musicURL" );
    mTest->mMusicURL = QString( "musicURLTwo" );
    result = mTest->setMusicURL( musicURL );
    QCOMPARE( result, true );
    QCOMPARE( mTest->musicURL(), musicURL ); 
    
    result = false;
    musicURL = QString();
    mTest->mMusicURL = QString();
    result = mTest->setMusicURL( musicURL );
    QCOMPARE( result, false );
    QCOMPARE( mTest->musicURL().isNull(), true ); 
    
    result = false;
    musicURL = QString();
    mTest->mMusicURL = QString( "musicURLTwo" );
    result = mTest->setMusicURL( musicURL );
    QCOMPARE( result, true );
    QCOMPARE( mTest->musicURL().isNull(), true ); 
}

/*!
 Test setDrmProtected
 */
void TestMpSongData::testSetDrmProtected()
{
    bool result;    
    bool drmProtected = false;
    mTest->mDrmProtected = true;
    result = mTest->setDrmProtected( drmProtected );
    QCOMPARE( result, true );
    QCOMPARE( mTest->isDrmProtected(), false ); 
    
    result = false;
    drmProtected = false;
    mTest->mDrmProtected = false;
    result = mTest->setDrmProtected( drmProtected );
    QCOMPARE( result, false );
    QCOMPARE( mTest->isDrmProtected(), false ); 
    
    result = false;
    drmProtected = true;
    mTest->mDrmProtected = true;
    result = mTest->setDrmProtected( drmProtected);
    QCOMPARE( result, false );
    QCOMPARE( mTest->isDrmProtected(), true ); 
    
    result = false;
    drmProtected = true;
    mTest->mDrmProtected = false;
    result = mTest->setDrmProtected( drmProtected );
    QCOMPARE( result, true );
    QCOMPARE( mTest->isDrmProtected(), true ); 
}


/*!
 Test setAlbumArtUri() 
 */
void TestMpSongData::testSetAlbumArtUri() 
{
    const QString albumArtUri( "AlbumArt" );
    const QString albumArtUriEmpty( "" );
    
    QSignalSpy spy( mTest, SIGNAL( albumArtReady() ) );
    
    QVERIFY( spy.isValid() );
    QCOMPARE( spy.count(), 0 );
    
    mTest->setAlbumArtUri( albumArtUri );
    QCOMPARE( spy.count(), 0 );
    
    mTest->mThumbnailManager->mGetThumbFails = true;
    mTest->setAlbumArtUri( albumArtUri );
    QCOMPARE( spy.count(), 1 );
    QCOMPARE( mTest->mAlbumArt, mTest->mDefaultAlbumArt );
    
    mTest->setAlbumArtUri( albumArtUriEmpty );
    QCOMPARE( spy.count(), 2 );
    QCOMPARE( mTest->mAlbumArt, mTest->mDefaultAlbumArt );  
}

/*!
 Test thumbnailReady()() 
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
   HbIcon dummyAlbumArtCompare;
   mTest->albumArt(dummyAlbumArtCompare);
   QCOMPARE( dummyAlbumArtCompare.isNull(), false );
   
   emit thumbnailReady(dummyAlbumArt, 0, -1 , 1);
   QCOMPARE( spy.count(), 2 );
   mTest->albumArt(dummyAlbumArtCompare);
   QCOMPARE( dummyAlbumArtCompare.isNull(), false );
   
}


/*!
 Test commitPlaybackInfo() 
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
 Test commitSongDetailInfo() 
 */
void TestMpSongData::testCommitSongDetailInfo() 
    {
    QSignalSpy spy( mTest, SIGNAL( songDetailInfoChanged() ) );
    
    QVERIFY( spy.isValid() );
    QCOMPARE( spy.count(), 0 );
    
    mTest->commitSongDetailInfo();
    QCOMPARE( spy.count(), 1 );
    
    }
