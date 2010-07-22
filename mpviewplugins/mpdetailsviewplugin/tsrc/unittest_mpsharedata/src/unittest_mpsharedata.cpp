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
* Description: Unit test for MpShareData
*
*/
#include <QSignalSpy>
#ifdef Q_OS_SYMBIAN
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbicon.h>
#endif
#include"unittest_mpsharedata.h"

// Do this so we can access all member variables.
#define private public
#include "mpsharedata.h"
#undef private
#include "mpsongdata.h"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
#ifdef Q_OS_SYMBIAN
int main(int argc, char *argv[])
{

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpShareData tv;

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsharedata.txt";

        return QTest::qExec(&tv, 3, pass);
    }    
}
#endif

#ifdef Q_OS_WIN32
QTEST_MAIN(TestMpShareData)
#endif
//Constructor
TestMpShareData::TestMpShareData()
    : mTest(0)
{

}

//Destructor
TestMpShareData::~TestMpShareData()
{
    if (mTest) delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpShareData::initTestCase()
{
    qDebug() << "unit test for mpsharedata begin.";
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpShareData::cleanupTestCase()
{
    qDebug() << "unit test for mpsharedata end.";
    QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpShareData::init()
{
    mTest = new MpShareData();
    mTest->setUnknownTr("Unknown");
}

/*!
 Called after every testfunction.
 */
void TestMpShareData::cleanup()
{
    delete mTest;
    mTest = 0;
}

void TestMpShareData::testOwner()
{
    mTest->setOwner(this);
    QCOMPARE(mTest->owner(), this);
}

void TestMpShareData::testSongData()
{
    MpSongData *song = new MpSongData();
    mTest->setSongData(song);
    QCOMPARE(mTest->songData(), song);
    delete song;
}

void TestMpShareData::testErrorMessage()
{
    QString msg("Test error message.");
    mTest->setErrorMessage(msg);
    QCOMPARE(mTest->errorMessage(), msg);
}

void TestMpShareData::testUserName()
{
    QString name("testname");
    mTest->setUsername(name);
    QCOMPARE(mTest->username(), name);
}

void TestMpShareData::testPassword()
{
    QString password("testpassword");
    mTest->setPassword(password);
    QCOMPARE(mTest->password(), password);
}

void TestMpShareData::testObjectTypeWithLink()
{
    MpSongData *song = new MpSongData();
    song->setLink("http://www.nokia.com");
    mTest->setSongData(song);
    QCOMPARE(mTest->objectType(), QString("URI"));
}

void TestMpShareData::testObjectTypeWithoutLink()
{
    MpSongData *song = new MpSongData();
    song->setLink("");
    mTest->setSongData(song);
    QCOMPARE(mTest->objectType(), QString("NOTE-APPEND"));
    delete song;
}

void TestMpShareData::testObjectContentWithLink()
{
    MpSongData *song = new MpSongData();
    song->setLink("http://www.nokia.com");
    song->setTitle("TestTitle");
    song->setArtist("TestArtist");
    song->setAlbum("TestAlbum");
    song->setAlbumArtUri("http://www.nokia.com/notexist.png");
    mTest->setSongData(song);
    QCOMPARE(mTest->objectContent(), QString("http://www.nokia.com"));
    QCOMPARE(mTest->objectReservedLength(), QString("http://www.nokia.com").length());
    delete song;
}

void TestMpShareData::testObjectContentWithTitleAndArtist()
{
    MpSongData *song = new MpSongData();
    song->setTitle("TestTitle");
    song->setArtist("TestArtist");
    song->setAlbum("TestAlbum");
    song->setAlbumArtUri("http://www.nokia.com/notexist.png");
    mTest->setSongData(song);
    QCOMPARE(mTest->objectContent(), QString("&#9835; TestArtist: TestTitle http://music.ovi.com"));
    QCOMPARE(mTest->objectReservedLength(), QString("&#9835; TestArtist: TestTitle http://music.ovi.com").length());
    delete song;
}


void TestMpShareData::testObjectContentWithTitleOnly()
{
    MpSongData *song = new MpSongData();
    song->setTitle("TestTitle");
    song->setAlbum("TestAlbum");
    song->setAlbumArtUri("http://www.nokia.com/notexist.png");
    mTest->setSongData(song);
    QCOMPARE(mTest->objectContent(), QString("&#9835; Unknown: TestTitle http://music.ovi.com"));
    QCOMPARE(mTest->objectReservedLength(), QString("&#9835; Unknown: TestTitle http://music.ovi.com").length());
    delete song;
}


void TestMpShareData::testObjectContentWithArtistOnly()
{
    MpSongData *song = new MpSongData();
    song->setArtist("TestArtist");
    song->setAlbum("TestAlbum");
    song->setAlbumArtUri("http://www.nokia.com/notexist.png");
    mTest->setSongData(song);
    QCOMPARE(mTest->objectContent(), QString("&#9835; TestArtist: Unknown http://music.ovi.com"));
    QCOMPARE(mTest->objectReservedLength(), QString("&#9835; TestArtist: Unknown http://music.ovi.com").length());
    delete song;
}


void TestMpShareData::testObjectContentWithoutMetaInfo()
{
    MpSongData *song = new MpSongData();
    mTest->setSongData(song);
    QCOMPARE(mTest->objectContent(), QString("&#9835; Unknown: Unknown http://music.ovi.com"));
    QCOMPARE(mTest->objectReservedLength(), QString("&#9835; Unknown: Unknown http://music.ovi.com").length());
    delete song;
}

void TestMpShareData::testSongTitle()
{
    MpSongData *song = new MpSongData();
    song->setTitle("TestTitle");
    mTest->setSongData(song);
    QCOMPARE(mTest->title(), QString("TestTitle"));
    delete song;
}

//void TestMpShareData::testSongAlbum()
//{
//    MpSongData *song = new MpSongData();
//    song->setAlbum("TestAlbum");
//    mTest->setSongData(song);
//    QCOMPARE(mTest->album(), QString("TestAlbum"));
//    delete song;
//}

void TestMpShareData::testSongArtist()
{
    MpSongData *song = new MpSongData();
    song->setArtist("TestArtist");
    mTest->setSongData(song);
    QCOMPARE(mTest->artist(), QString("TestArtist"));
    delete song;
}

void TestMpShareData::testAlbumArt()
{
    MpSongData *song = new MpSongData();
    mTest->setSongData(song);
    QCOMPARE(mTest->albumArtBase64(), QString("e:\\album_art.png"));
    delete song;
}

void TestMpShareData::testLanguage()
{
    MpSongData *song = new MpSongData();
    mTest->setSongData(song);
    mTest->setLanguage("fi");
    QCOMPARE(mTest->language(), QString("fi"));
    delete song;
}

/*
void TestMpShareData::testSongComment()
{
    MpSongData *song = new MpSongData();
    song->setComment("TestComment");
    mTest->setSongData(song);
    QCOMPARE(mTest->comment(), QString("TestComment"));
    delete song;
}

void TestMpShareData::testSongAlbumArt()
{
    MpSongData *song = new MpSongData();
    song->setAlbumArtUri("TestAlbum");
    mTest->setSongData(song);
    QPixmap pic;
    mTest->albumArt(pic);
    delete song;
}

void TestMpShareData::testSongYear()
{
    MpSongData *song = new MpSongData();
    song->setYear(1999);
    mTest->setSongData(song);
    QCOMPARE(mTest->year(), QString("1999"));
    delete song;
}

void TestMpShareData::testSongGenre()
{
    MpSongData *song = new MpSongData();
    song->setGenre("TestGenre");
    mTest->setSongData(song);
    QCOMPARE(mTest->genre(), QString("TestGenre"));
    delete song;
}

void TestMpShareData::testSongComposer()
{
    MpSongData *song = new MpSongData();
    song->setComposer("TestComposer");
    mTest->setSongData(song);
    QCOMPARE(mTest->composer(), QString("TestComposer"));
    delete song;
}

void TestMpShareData::testSongAlbumTrack()
{
    MpSongData *song = new MpSongData();
    song->setAlbumTrack("TestAlbumTrack");
    mTest->setSongData(song);
    QCOMPARE(mTest->albumTrack(), QString("TestAlbumTrack"));
    delete song;
}

void TestMpShareData::testSongLink()
{
    MpSongData *song = new MpSongData();
    song->setLink("TestLink");
    mTest->setSongData(song);
    QCOMPARE(mTest->link(), QString("TestLink"));
    delete song;
}
*/

void TestMpShareData::testSongNoInfo()
{
    QCOMPARE(mTest->title(), QString("Unknown"));
//    QCOMPARE(mTest->album(), QString(""));
    QCOMPARE(mTest->artist(), QString("Unknown"));
    QCOMPARE(mTest->albumArtBase64(), QString(""));
    QCOMPARE(mTest->objectContent(), QString("&#9835; Unknown: Unknown http://music.ovi.com"));
//    QCOMPARE(mTest->comment(), QString(""));
//    QCOMPARE(mTest->year(), QString(""));
//    QCOMPARE(mTest->genre(), QString(""));
//    QCOMPARE(mTest->composer(), QString(""));
//    QCOMPARE(mTest->albumTrack(), QString(""));
//    QCOMPARE(mTest->link(), QString(""));
}
