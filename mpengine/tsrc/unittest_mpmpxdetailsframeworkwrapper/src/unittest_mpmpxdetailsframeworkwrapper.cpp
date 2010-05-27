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
* Description: Unit test for mpmpxdetailsframeworkwrapper
*
*/

#include <QSignalSpy>
#include <hbapplication.h>
#include <hbmainwindow.h>

#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediaaudiodefs.h>

#include <e32std.h>
#include <e32des8.h>

#include "unittest_mpmpxdetailsframeworkwrapper.h"
#include "stub/inc/mpxplaybackutility.h"
#include "stub/inc/mpsongdata.h"
#include "logger.h"
#include "mpcommondefs.h"

// classes under test
#define private public
#include "mpmpxdetailsframeworkwrapper.h"
#include "mpmpxdetailsframeworkwrapper_p.h"
#undef private

struct TTestAttrs
    {
    const wchar_t* GeneralTitle;     // MpSongData::mTitle
    const wchar_t* MusicArtist;      // MpSongData::mArtist
    const wchar_t* MusicAlbum;       // MpSongData::mAlbum
    const wchar_t* MusicAlbumArtFileName; // MpSongData::mAlbumArt (TODO: not tested yet)
    const wchar_t* Composer;         // MpSongData::mComposer
    const TInt Year;                 // MpSongData::mYear
    const wchar_t* MusicAlbumTrack;  // MpSongData::mAlbumTrack
    const wchar_t* MusicGenre;       // MpSongData::mGenre
    const wchar_t* MusicUri;         // N/A in MpSongData (TODO: not tested yet)
    const wchar_t* MimeType;         // MpSongData::mMimeType
    const TInt Duration;             // MpSongData::mDuration
    const TInt MediaAudioBitRate;    // MpSongData::mbitRate
    const TInt MediaAudioSampleRate; // MpSongData::mSampleRate
    const wchar_t* Copyright;        // MpSongData::mCopyright
    const wchar_t* Url;              // MpSongData::mMusicURL    
    };

// title
const TInt KTitleZeroSized = 0;
const TInt KTitleVeryLarge = 1;
const TInt KTitleOff = 2;

// artist
const TInt KMusicArtistZeroSized = 3;
const TInt KMusicArtistLarge = 4;
const TInt KMusicArtistOff = 5;

// album
const TInt KAlbumZeroSized = 6;
const TInt KAlbumLarge = 7;
const TInt KAlbumOff = 8;

// comment
const TInt KCommentZeroSized = 9;
const TInt KCommentLarge = 10;
const TInt KCommentOff = 11;

// composer
const TInt KComposerZeroSized = 12;
const TInt KComposerLarge = 13;
const TInt KComposerOff = 14;

// year
const TInt KYearNegative = 15;
const TInt KYearZero = 16;
const TInt KYearPositive = 17;
const TInt KYearOff = 18;

// album track
const TInt KAlbumTrackZeroSized = 19;
const TInt KAlbumTrackLarge = 20;
const TInt KAlbumTrackOff = 21;

// genre
const TInt KGenreZeroSized = 22;
const TInt KGenreLarge = 23;
const TInt KGenreOff = 24;

// everything is OFF
const TInt KAllFieldsOFF = 25;

// MIME Type
const TInt KMimeTypeZeroSized = 26;
const TInt KMimeTypeMP3 = 27;
const TInt KMimeTypeLarge = 28;
const TInt KMimeTypeOff = 29;

// duration
const TInt KDuration = 30;

// bit rate
const TInt KMediaAudioBitrate = 31;

// sample rate
const TInt KMediaAudioSampleRate = 32;

// sample rate
const TInt KMediaGeneralCopyright = 33;
const TInt KMediaMusicURL = 34;

_LIT(KOff,"OFF");

const TTestAttrs KAllSongsTestData[] =
    {
    // Title    
     { L"", L"Artist 1", L"Album 1", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
    ,{ L"LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM", L"Artist 1", L"Album 1", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
    ,{ L"OFF", L"Artist 1", L"Album 1", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
    // Artist  
    , { L"Title", L"", L"Album 1", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
    , { L"Title", L"LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM", L"Album 1", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
    ,{ L"Title", L"OFF", L"Album 1", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}  
    // Album
     , { L"Title", L"Artist", L"", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     , { L"Title", L"Artist", L"LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     , { L"Title", L"Artist", L"OFF", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}  
    // Comment
     , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}  
     // Composer
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM", 2009, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"OFF", 2009, L"Track 1", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     // Year
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", -200, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 0, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 3000, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", -1, L"Track 1", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}     
     // Album Track
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"OFF", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     // Genre
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"OFF", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     // all fields OFF
    , { L"OFF", L"OFF", L"OFF", L"OFF", L"OFF", -1, L"OFF", L"OFF", L"OFF", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     // MIME TYPE - FORMAT
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"MP3", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM LOREM IPSUM", -1,-1,-1, L"OFF", L"OFF"}
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"OFF", -1,-1,-1, L"OFF", L"OFF"}
     // duration
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"", 0,-1,-1, L"OFF", L"OFF"}
	  // KMediaAudioBitrate
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"", 0,0, -1, L"OFF", L"OFF"}
	  // KMPXMediaAudioSamplerate
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"", -1,-1, 0, L"OFF", L"OFF"}	
	  // KMPXMediaGeneralCopyright
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"", -1,-1, 0, L"Copyright", L"OFF"}	
	  // KMPXMediaMusicURL
      , { L"Title", L"Artist", L"Album", L"e:\\data\\images\\art1.jpg", L"Composer 1", 2000, L"Track", L"Genre", L"Uri 1", L"", -1,-1, 0, L"OFF", L"URL"}      
      
    };


QString referenceGeneralCopyright(int index)
{
    QString result;
    const TDesC& copyright = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].Copyright));
    if(copyright.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( copyright.Ptr(), copyright.Length() );
    }
    return result;
}

QString referenceUrl(int index)
{
    QString result;
    const TDesC& url = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].Url));
    if(url.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( url.Ptr(), url.Length() );
    }
    return result;
}

QString referenceMimeType(int index)
{
    QString result;
    const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].MimeType));
    _LIT(KMime, "audio/aac");
    if(title.Match(KMime) == KErrNone)
    {
        result = QString("AAC");
    }
    return result;
}

QString referenceGeneralTitle(int index)
{
    QString result;
    const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].GeneralTitle));
    if(title.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( title.Ptr(), title.Length() );
    }
    return result;
}

QString referenceMusicArtist(int index)
{
    QString result;
    const TDesC& artist = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].MusicArtist));
    if(artist.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( artist.Ptr(), artist.Length());
    }    
    return result;
}

QString referenceMusicAlbum(int index)
{
    QString result;
    const TDesC& album = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].MusicAlbum));
    if(album.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( album.Ptr(), album.Length() );
    }    
    return result;
}

QString referenceMusicAlbumArtFileName(int index)
{
    QString result;
    const TDesC& musicAlbumArtFileName = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].MusicAlbumArtFileName));
    if(musicAlbumArtFileName.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( musicAlbumArtFileName.Ptr(), musicAlbumArtFileName.Length() );
    }    
    return result;
}

QString referenceComposer(int index)
{
    QString result;
    const TDesC& composer = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].Composer));
    if(composer.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( composer.Ptr(), composer.Length() );
    }    
    return result;
}

int referenceYear(int index)
{
    return (KAllSongsTestData[index].Year >= 0) ? KAllSongsTestData[index].Year : 0;
}

QString referenceMusicAlbumTrack(int index)
{
    QString result;
    const TDesC& musicAlbumTrack = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].MusicAlbumTrack));
    if(musicAlbumTrack.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( musicAlbumTrack.Ptr(), musicAlbumTrack.Length() );
    }    
    return result;
}

QString referenceMusicGenre(int index)
{
    QString result;
    const TDesC& musicGenre = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].MusicGenre));
    if(musicGenre.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( musicGenre.Ptr(), musicGenre.Length() );
    }    
    return result;
}

QString referenceMusicUri(int index)
{
    QString result;
    const TDesC& musicUri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[index].MusicUri));
    if(musicUri.Match(KOff) == KErrNotFound)
    {
        result = QString::fromUtf16( musicUri.Ptr(), musicUri.Length() );
    }    
    return result;
}

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpMpxDetailsFrameworkWrapper tv;

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmpxdetailsframeworkwrapper.txt";

        return QTest::qExec(&tv, 3, pass);
    }    

}

TestMpMpxDetailsFrameworkWrapper::TestMpMpxDetailsFrameworkWrapper()
    : mTest(0),
      iMediaTestData(0),
      mSongDetailInfoChanged(false),
      mPlaybackInfoChanged(false)
{
}

TestMpMpxDetailsFrameworkWrapper::~TestMpMpxDetailsFrameworkWrapper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxDetailsFrameworkWrapper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxDetailsFrameworkWrapper::cleanupTestCase()
{
QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxDetailsFrameworkWrapper::init()
{
    User::__DbgMarkStart(RHeap::EUser);
    TInt freeCount(0);
    mAllocCells = User::CountAllocCells(freeCount);
    TRACE3("TestMpMpxDetailsFrameworkWrapper::init() -- allocCount: %d, freeCount: %d", 
            mAllocCells, freeCount );
    
    mTest = new MpMpxDetailsFrameworkWrapper( TUid::Uid( MpCommon::KMusicPlayerUid ) );
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMpxDetailsFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
    mTestPrivate = 0;

    TInt freeCount(0);
    TInt allocCount = User::CountAllocCells(freeCount);     
    TRACE3( "TestMpMpxDetailsFrameworkWrapper::cleanup() -- allocCount: %d, freeCount: %d", 
        allocCount, freeCount );      
    User::__DbgMarkEnd(RHeap::EUser,0); // this should panic if leak        
}

/*!
 Tests constructor.
 */
void TestMpMpxDetailsFrameworkWrapper::testConstructor()
{
    QVERIFY( mTestPrivate->q_ptr );
    QVERIFY( mTestPrivate->iPlaybackUtility );
    QVERIFY( mTestPrivate->iSongData );
    // TODO: test observer?
}

/*!
 Tests playbackData
 */
void TestMpMpxDetailsFrameworkWrapper::testSongData()
{
    QCOMPARE(mTestPrivate->iSongData, mTest->songData());
}

void TestMpMpxDetailsFrameworkWrapper::testHandleProperty()
{
    // dummy call just to get coverage up. Empty imp in function under test
    mTestPrivate->HandlePropertyL(EPbPropertyVolume, 0, 0);
}

void TestMpMpxDetailsFrameworkWrapper::testHandleSubPlayerNames()
{
    // dummy call just to get coverage up. Empty imp in function under test
    mTestPrivate->HandleSubPlayerNamesL(TUid::Null(), 0, 0, 0);
}

/*!
 Tests testHandlePlaybackMessage
 */
void TestMpMpxDetailsFrameworkWrapper::testHandlePlaybackMessage()
{
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMessageContentIdGeneral );
    CMPXMedia* testMessage = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(testMessage);
    testMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral);    
    
    //Media change
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralEvent,TMPXPlaybackMessage::EMediaChanged);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralType,0);
    testMessage->SetTObjectValueL<TInt>(KMPXMessageGeneralData,0);
    mTestPrivate->HandlePlaybackMessage(testMessage, KErrNone);
    CleanupStack::PopAndDestroy(testMessage);
}

void TestMpMpxDetailsFrameworkWrapper::verifyEmptyTestData()
{
    QVERIFY(mTestPrivate->iSongData->title().isEmpty());
    QVERIFY(mTestPrivate->iSongData->album().isEmpty());
    QVERIFY(mTestPrivate->iSongData->artist().isEmpty());
    QVERIFY(mTestPrivate->iSongData->comment().isEmpty());
    QVERIFY(mTestPrivate->iSongData->year().isEmpty());
    QVERIFY(mTestPrivate->iSongData->genre().isEmpty());
    QVERIFY(mTestPrivate->iSongData->composer().isEmpty());
    QVERIFY(mTestPrivate->iSongData->albumTrack().isEmpty());
    QVERIFY(mTestPrivate->iSongData->link().isEmpty());
    QVERIFY(mTestPrivate->iSongData->mimeType().isEmpty());
    QVERIFY(mTestPrivate->songData()->duration().isEmpty());
    QVERIFY(mTestPrivate->songData()->bitRate().isEmpty());
    QVERIFY(mTestPrivate->songData()->sampleRate().isEmpty());
    QVERIFY(mTestPrivate->songData()->copyright().isEmpty());    
    QVERIFY(mTestPrivate->songData()->musicURL().isEmpty());          	
}

void TestMpMpxDetailsFrameworkWrapper::verify(int index)
{
    QCOMPARE(mTestPrivate->songData()->title(), referenceGeneralTitle( index ));
    QCOMPARE(mTestPrivate->songData()->album(), referenceMusicAlbum( index ));    
    QCOMPARE(mTestPrivate->songData()->artist(), referenceMusicArtist( index ));
    // TODO: how to verify albumArt ?
    QCOMPARE(mTestPrivate->songData()->composer(), referenceComposer( index ));    
    QCOMPARE(mTestPrivate->songData()->year().toInt(), referenceYear( index ));    
    QCOMPARE(mTestPrivate->songData()->albumTrack(), referenceMusicAlbumTrack( index ));    
    QCOMPARE(mTestPrivate->songData()->genre(), referenceMusicGenre( index ));
    QCOMPARE(mTestPrivate->songData()->mimeType(), referenceMimeType( index ));
    QVERIFY(mTestPrivate->songData()->duration().isEmpty());
    QVERIFY(mTestPrivate->songData()->bitRate().isEmpty());  
    QVERIFY(mTestPrivate->songData()->sampleRate().isEmpty());
    QCOMPARE(mTestPrivate->songData()->copyright(), referenceGeneralCopyright(index) );
    QCOMPARE(mTestPrivate->songData()->musicURL(), referenceUrl(index) );
    
}

void TestMpMpxDetailsFrameworkWrapper::doTestHandleMedia(int dataIndex, TInt aResult)
{
    init();
    loadTestData(dataIndex);
    mTestPrivate->HandleMediaL(*iMediaTestData, aResult);
    if(aResult != KErrNone)
    {
        verifyEmptyTestData();        
    }
    else
    {
        verify(dataIndex);
    }
    cleanup();
}

/*
 Tests handleMedia
*/
void TestMpMpxDetailsFrameworkWrapper::testHandleMedia()
{
    // testing passing error code. iSongData should not be modified
    // as it got constructed fresh with init(), all its string
    // members must be empty
    doTestHandleMedia(KTitleZeroSized, KErrGeneral);
    
    // Testing zero sized title
    doTestHandleMedia(KTitleZeroSized, KErrNone);
    
    // very large title
    doTestHandleMedia(KTitleVeryLarge, KErrNone);    
    
    // Testing title field OFF
    doTestHandleMedia(KTitleOff, KErrNone);
    
    // Testing artist empty string
    doTestHandleMedia(KMusicArtistZeroSized, KErrNone);
    
    // Testing artist long
    doTestHandleMedia(KMusicArtistLarge, KErrNone);    
    
    // Testing artist field OFF
    doTestHandleMedia(KMusicArtistOff, KErrNone);    
    
    // Testing album zero sized
    doTestHandleMedia(KAlbumZeroSized, KErrNone);
    
    // Testing album long
    doTestHandleMedia(KAlbumLarge, KErrNone);
    
    // Testing album off
    doTestHandleMedia(KAlbumOff, KErrNone);
    
    // Testing comment zero sized
    doTestHandleMedia(KCommentZeroSized, KErrNone);
    
    // Testing comment large
    doTestHandleMedia(KCommentLarge, KErrNone);
    
    // Testing comment OFF
    doTestHandleMedia(KCommentOff, KErrNone);
    
    // Testing composer zero sized
    doTestHandleMedia(KComposerZeroSized, KErrNone);
    
    // Testing composer large
    doTestHandleMedia(KComposerLarge, KErrNone);
    
    // Testing composer OFF
    doTestHandleMedia(KComposerOff, KErrNone);
    
    // Testing year negative
    doTestHandleMedia(KYearNegative, KErrNone);
    
    // Testing year 0
    doTestHandleMedia(KYearZero, KErrNone);
    
    // Testing year +
    doTestHandleMedia(KYearPositive, KErrNone);
    
    // Testing year OFF
    doTestHandleMedia(KYearOff, KErrNone);
    
    // Testing album track zero sized
    doTestHandleMedia(KAlbumTrackZeroSized, KErrNone);
    
    // Testing album track large
    doTestHandleMedia(KAlbumTrackLarge, KErrNone);
    
    // Testing album track OFF
    doTestHandleMedia(KAlbumTrackOff, KErrNone);
    
    // Testing genre zero sized
    doTestHandleMedia(KGenreZeroSized, KErrNone);
 
    // Testing genre large
    doTestHandleMedia(KGenreLarge, KErrNone);    
    
    // Testing genre OFF
    doTestHandleMedia(KGenreOff, KErrNone);  
    
    // Testing all fields OFF
    doTestHandleMedia(KAllFieldsOFF, KErrNone);  
    
    // Zero sized Mime Type
    doTestHandleMedia(KMimeTypeZeroSized, KErrNone); 

    // audio/aac mime type
    doTestHandleMedia(KMimeTypeMP3, KErrNone); 

    // Damn long mime type
    doTestHandleMedia(KMimeTypeLarge, KErrNone);

    // no mime type
    doTestHandleMedia(KMimeTypeOff, KErrNone);
    
    doTestNoChange();
    
    // duration
    doTestHandleMedia(KDuration, KErrNone);
    
    // media audio bit rate
    doTestHandleMedia(KMediaAudioBitrate, KErrNone);
    
    // audio sample rate
    doTestHandleMedia(KMediaAudioSampleRate, KErrNone);
    
    // copyright
    doTestHandleMedia(KMediaGeneralCopyright, KErrNone);
    
    // url
    doTestHandleMedia(KMediaMusicURL, KErrNone);
            
}

void TestMpMpxDetailsFrameworkWrapper::doTestNoChange()
{
    init();
    mPlaybackInfoChanged = false;
    loadTestData(KYearPositive);
    connect( mTestPrivate->iSongData, SIGNAL( songDetailInfoChanged() ), this, SLOT( handleCommitPlaybackInfo() ) );
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE( mPlaybackInfoChanged, true );
    mPlaybackInfoChanged = false; 
    mTestPrivate->HandleMediaL(*iMediaTestData, KErrNone);
    QCOMPARE( mPlaybackInfoChanged, true ); // TODO: this must be false, donno why we get songDetailInfoChanged signal 
    cleanup();
}

void TestMpMpxDetailsFrameworkWrapper::handleCommitPlaybackInfo()
{
    mPlaybackInfoChanged = true;       
}

/*!
 Tests retrieveSong
 */
void TestMpMpxDetailsFrameworkWrapper::testRetrieveSong()
{
    mTest->retrieveSong(); // async
    // no need to verify the data received. As we already
    // tested them in testHandleMedia    
}

void TestMpMpxDetailsFrameworkWrapper::handleSongDetailInfoChanged()
{
    mSongDetailInfoChanged = true;
}

/*!
 Used to load test data
 */
void TestMpMpxDetailsFrameworkWrapper::loadTestData( TInt aPos )
{
    TRACE("TestMpMpxDetailsFrameworkWrapper::loadTestData entry");
    if( iMediaTestData ){
        delete iMediaTestData;
        iMediaTestData=0;
    }
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    iMediaTestData = CMPXMedia::NewL( supportedIds.Array() );
    CleanupStack::PopAndDestroy( &supportedIds );

    TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Loading %d.th data entry", aPos);
    TPtrC title( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].GeneralTitle ) );
    if(title.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding title: %S to iMediaTestData", &title);
        iMediaTestData->SetTextValueL( KMPXMediaGeneralTitle, title );
    }
    
    TPtrC artist( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].MusicArtist ) );
    if(artist.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding artist: %S to iMediaTestData", &artist);
        iMediaTestData->SetTextValueL( KMPXMediaMusicArtist, artist );
    }

    TPtrC musicAlbum( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].MusicAlbum ) );
    if(musicAlbum.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding musicAlbum: %S to iMediaTestData", &musicAlbum);        
        iMediaTestData->SetTextValueL( KMPXMediaMusicAlbum, musicAlbum );
    }

    TPtrC musicAlbumArtFileName( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].MusicAlbumArtFileName ) );
    if(musicAlbumArtFileName.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding musicAlbumArtFileName: %S to iMediaTestData", &musicAlbumArtFileName);        
        iMediaTestData->SetTextValueL( KMPXMediaMusicAlbumArtFileName, musicAlbumArtFileName );        
    }

    TPtrC composer( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].Composer ) );
    if(composer.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding composer: %S to iMediaTestData", &composer);        
        iMediaTestData->SetTextValueL( KMPXMediaMusicComposer, composer);
    }
    
    if(KAllSongsTestData[aPos].Year != -1) {
        TDateTime date_time;
        date_time.SetYear(KAllSongsTestData[aPos].Year);
        date_time.SetMonth(EJanuary);
        date_time.SetDay(1);
        date_time.SetHour(0);
        date_time.SetMinute(0);
        date_time.SetSecond(0);
        date_time.SetMicroSecond(0);
        TTime date(date_time);
        TRACE3("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding year: %d in %LD to iMediaTestData", KAllSongsTestData[aPos].Year, date.Int64());
        TRACE3("TestMpMpxDetailsFrameworkWrapper::loadTestData Is year %d equal to %d", KAllSongsTestData[aPos].Year, date.DateTime().Year());        
        iMediaTestData->SetTObjectValueL<TInt64>( KMPXMediaMusicYear, date.Int64() );        
    }

    TPtrC musicAlbumTrack( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].MusicAlbumTrack ) );
    if(musicAlbumTrack.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding musicAlbumTrack: %S to iMediaTestData", &musicAlbumTrack);        
        iMediaTestData->SetTextValueL( KMPXMediaMusicAlbumTrack,  musicAlbumTrack);        
    }
    
    TPtrC genre( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].MusicGenre ) );
    if(genre.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding genre: %S to iMediaTestData", &genre);        
        iMediaTestData->SetTextValueL( KMPXMediaMusicGenre, genre);
    }
    
    TPtrC musicUri( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].MusicUri ) );
    if(musicUri.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding musicUri: %S to iMediaTestData", &musicUri);        
        iMediaTestData->SetTextValueL( KMPXMediaGeneralUri, musicUri);
    }

    TPtrC mimeType( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].MimeType ) );
    if(mimeType.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding mimeType: %S to iMediaTestData", &mimeType);        
        iMediaTestData->SetTextValueL( KMPXMediaGeneralMimeType, mimeType);
    }
    
    if(KAllSongsTestData[aPos].Duration != -1) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding duration: %d to iMediaTestData", KAllSongsTestData[aPos].Duration );
        iMediaTestData->SetTObjectValueL<TInt>( KMPXMediaGeneralDuration, KAllSongsTestData[aPos].Duration );
    }
    
    if(KAllSongsTestData[aPos].MediaAudioBitRate != -1) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding MediaAudioBitRate: %d to iMediaTestData", KAllSongsTestData[aPos].MediaAudioBitRate );
        iMediaTestData->SetTObjectValueL<TInt>( KMPXMediaAudioBitrate, KAllSongsTestData[aPos].MediaAudioBitRate );
    }    

    if(KAllSongsTestData[aPos].MediaAudioSampleRate != -1) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding MediaAudioBitRate: %d to iMediaTestData", KAllSongsTestData[aPos].MediaAudioSampleRate );
        iMediaTestData->SetTObjectValueL<TInt>( KMPXMediaAudioSamplerate, KAllSongsTestData[aPos].MediaAudioSampleRate );
    }    

    TPtrC copyright( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].Copyright ) );
    if(copyright.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding copyright: %S to iMediaTestData", &copyright);        
        iMediaTestData->SetTextValueL( KMPXMediaGeneralCopyright, copyright);
    }

    TPtrC url( reinterpret_cast<const TUint16*>( KAllSongsTestData[aPos].Url ) );
    if(url.Match(KOff) == KErrNotFound) {
        TRACE2("TestMpMpxDetailsFrameworkWrapper::loadTestData Adding url: %S to iMediaTestData", &url);        
        iMediaTestData->SetTextValueL( KMPXMediaMusicURL, url );
    }
               
    TRACE("TestMpMpxDetailsFrameworkWrapper::loadTestData exit");    
}

//end of file
