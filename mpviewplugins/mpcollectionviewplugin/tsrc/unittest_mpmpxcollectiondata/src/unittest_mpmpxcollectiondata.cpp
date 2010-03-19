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
* Description: Unit test for MpMpxCollectionData
*
*/

#include <QSignalSpy>
#include <QMetaType>
#include <hbapplication.h>
#include <hbmainwindow.h>

#include <apacmdln.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxcollectionpath.h>

#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>


#include "unittest_mpmpxcollectiondata.h"
#include "mpcommondefs.h"


// Do this so we can access all member variables.
#define private public
#include "mpmpxcollectiondata.h"
#include "mpmpxcollectiondata_p.h"
#undef private

//This so we can test private functions
#include "mpmpxcollectiondata_p.cpp"

struct TTestAttrs
    {
    const wchar_t* GeneralTitle;
    const wchar_t* MusicArtist;
    const TInt GeneralCount;
    const wchar_t* MusicAlbumArtFileName;
    };
    
const TTestAttrs KAllSongsTestData[] =
    {
     { L"Title 1", L"Artist 1", 1, L"e:\\data\\images\\art1.jpg" }
    ,{ L"Title 2", L"Artist 2", 1, L"e:\\data\\images\\art2.jpg" }
    ,{ L"Title 3", L"Artist 3", 1, L"e:\\data\\images\\art3.jpg" }
    ,{ L"Title 4", L"Artist 4", 1, L"e:\\data\\images\\art4.jpg" }
    ,{ L"Title 5", L"Artist 5", 1, L"e:\\data\\images\\art5.jpg" }
    ,{ L"Title 6", L"Artist 6", 1, L"e:\\data\\images\\art6.jpg" }
    ,{ L"Title 7", L"Artist 7", 1, L"e:\\data\\images\\art7.jpg" }
    ,{ L"Title 8", L"Artist 8", 1, L"e:\\data\\images\\art8.jpg" }
    ,{ L"Title 9", L"Artist 9", 1, L"e:\\data\\images\\art9.jpg" }
    ,{ L"Title 10", L"Artist 10", 1, L"e:\\data\\images\\art10.jpg" }
    };
    
    
/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpMpxCollectionData tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpmpxcollectiondata.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpMpxCollectionData::TestMpMpxCollectionData()
    : mTest(0)
{
}

TestMpMpxCollectionData::~TestMpMpxCollectionData()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMpxCollectionData::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMpxCollectionData::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMpxCollectionData::init()
{
    mTest = new MpMpxCollectionData();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMpxCollectionData::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor. Good case.
 */
void TestMpMpxCollectionData::testConstructor()
{
    QCOMPARE(mTestPrivate->iContext, ECollectionContextUnknown);
}

/*!
 Tests getter for context.
 */
void TestMpMpxCollectionData::testContextRetrieval()
{
    QCOMPARE(mTest->context(), ECollectionContextUnknown);
    mTestPrivate->iContext = ECollectionContextPlaylistSongs;
    QCOMPARE(mTest->context(), ECollectionContextPlaylistSongs);
}



void TestMpMpxCollectionData::testCount()
{
    mTestPrivate->iMediaArray = NULL;
    QCOMPARE(mTest->count(), 0);
    
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);   
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        mTestPrivate->iMediaArray = array;
        QCOMPARE(mTest->count(), i);
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    mTestPrivate->iMediaArray = array;
    QCOMPARE(mTest->count(), count);
    CleanupStack::PopAndDestroy(array);
}

void TestMpMpxCollectionData::testCollectionTitle()
{
    mTestPrivate->iContainerMedia = NULL;
    QCOMPARE(mTest->collectionTitle(), QString());   
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
        
    mTestPrivate->iContainerMedia = entries;
    QCOMPARE(mTest->collectionTitle(), QString());
    
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("All songs"));

    mTestPrivate->iContainerMedia = entries;
    QCOMPARE(mTest->collectionTitle(), QString("All songs"));
    mTestPrivate->iContainerMedia = NULL;
    CleanupStack::PopAndDestroy(entries);
}


void TestMpMpxCollectionData::testItemData()
{
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);   
    mTestPrivate->iMediaArray = array;
        
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {

        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
        mTestPrivate->iMediaArray = array;

        const TDesC& title = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Title), QString::fromUtf16( title.Ptr(), title.Length()));

        const TDesC& artist = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::Artist), QString::fromUtf16( artist.Ptr(), artist.Length()));

        //verification depends on language, just exercising the item.
        QVERIFY(!mTest->itemData( i, MpMpxCollectionData::Count).isNull());

        const TDesC& albumArtUri = TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName));
        QCOMPARE(mTest->itemData( i, MpMpxCollectionData::AlbumArtUri), QString::fromUtf16( albumArtUri.Ptr(), albumArtUri.Length()));

    }
    mTestPrivate->iMediaArray = array;

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    array->AppendL(*media);
    mTestPrivate->iMediaArray = array;
    media->SetTextValueL(KMPXMediaGeneralTitle,KNullDesC );
    media->SetTextValueL(KMPXMediaMusicArtist, KNullDesC);
    media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 0);
    media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, KNullDesC);
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);
    mTestPrivate->iMediaArray = array;
    
    //Media without attributes.
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::Title).isNull());
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::Artist).isNull());
    //verification depends on language, just exercising the item.
    QVERIFY(!mTest->itemData( count, MpMpxCollectionData::Count).isNull());
    QVERIFY(mTest->itemData( count, MpMpxCollectionData::AlbumArtUri).isNull());
    
    //Media with empty attributes.
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::Title).isNull());
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::Artist).isNull());
    //verification depends on language, just exercising the item.
    QVERIFY(!mTest->itemData( count + 1, MpMpxCollectionData::Count).isNull());
    QVERIFY(mTest->itemData( count + 1, MpMpxCollectionData::AlbumArtUri).isNull());

    /*Test Attributes that are not implemented yet.
        Uri,
        Duration,
        Album,
        Genre,
        Rating,
    */
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Uri).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Duration).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Album).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Genre).isNull());
    QVERIFY(mTest->itemData( 0, MpMpxCollectionData::Rating).isNull());

    CleanupStack::PopAndDestroy(array);
    mTestPrivate->iMediaArray = NULL;
}

void TestMpMpxCollectionData::testIsAutoPlaylist()
{
    //if cotext is different than playlistsongs value returned should be false
    QVERIFY(!mTest->isAutoPlaylist());
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("My Test Playlist Songs"));
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    CleanupStack::PopAndDestroy(array);
    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextPlaylistSongs);
    QVERIFY(!mTest->isAutoPlaylist());  
    
    entries->SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                    KMPXMediaGeneralNonPermissibleActions, EMPXWrite);
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 2);
    context = qvariant_cast<TCollectionContext>(spy.at(1).at(0));
    QCOMPARE(context, ECollectionContextPlaylistSongs);
    QVERIFY(mTest->isAutoPlaylist());  
    CleanupStack::PopAndDestroy(entries);

}

void TestMpMpxCollectionData::testIsAutoPlaylistWithIndex()
{
    //if cotext is different than playlists value returned should be false
    QVERIFY(!mTest->isAutoPlaylist(0));
    
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        if (i%2) {
            media->SetTObjectValueL<TMPXGeneralNonPermissibleActions>(
                    KMPXMediaGeneralNonPermissibleActions, EMPXWrite);
        }
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);

    }
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("My Test Playlist container"));
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    CleanupStack::PopAndDestroy(array);
        
    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextPlaylists);
    QVERIFY(!mTest->isAutoPlaylist( 0 ));
    QVERIFY(mTest->isAutoPlaylist( 1 ));
}


void TestMpMpxCollectionData::TestItemCountWithIndex()
{


    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    //no attributes
    array->AppendL(*media);
    CleanupStack::PopAndDestroy(media);

    CMPXMedia* media2 = CMPXMedia::NewL();
    CleanupStack::PushL(media2);
    media2->SetTObjectValueL<TInt>(KMPXMediaGeneralCount,0);
    array->AppendL(*media2);
    CleanupStack::PopAndDestroy(media2);

    CMPXMedia* media3 = CMPXMedia::NewL();
    CleanupStack::PushL(media3);
    media3->SetTObjectValueL<TInt>(KMPXMediaGeneralCount,20);
    array->AppendL(*media3);
    CleanupStack::PopAndDestroy(media3);
    
    
    mTestPrivate->iMediaArray=array;
    QCOMPARE(mTest->itemCount(0),0);
    QCOMPARE(mTest->itemCount(1),0);
    QCOMPARE(mTest->itemCount(2),20);

    mTestPrivate->iMediaArray=NULL;
    CleanupStack::PopAndDestroy(array);
}


void TestMpMpxCollectionData::testSetCollectionContextL()
{
    
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);


    
    /* FROM MPXDBPLUGIN.CPP
    // All Songs
    SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXSong ...
    ...
    // All artists  
    SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXArtist ...
    ...            
    // All albums of an artist
    SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXArtist ...
    ...
    // all songs for an artist
    SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXSong ...
    ...        
    // all songs of an album for an artist
    SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXAlbum ...
    ...        
    // All Albums
    SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXAlbum ...
    ...
    // All songs in one or multiple albums
    SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXAlbum ...
    ...       
    // All playlists
    SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXPlaylist ...
    ...
    // All songs in a playlist
    SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXPlaylist ...
    ...     
    // All genres
    SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXGenre ...
    ...            
    // All songs of a genre
    SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXGenre ...
    ...                    
    // All composers
    SetMediaGeneralAttributesL(aEntries, EMPXGroup, EMPXComposer ...
    ...
    // All songs of a composer
    SetMediaGeneralAttributesL(aEntries, EMPXItem, EMPXComposer ...
    */
    
    // All Songs
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextAllSongs);
    
    // All artists  
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXArtist);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextUnknown);//not implemented because is not in use.
    
    // All albums of an artist
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXArtist);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextUnknown);//not implemented because is not in use.
    
    // all songs for an artist
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextUnknown);//not implemented because is not in use.
    
    // all songs of an album for an artist
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextAlbumSongs);
    
    // All Albums
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextArtistAlbums);
    
    // All songs in one or multiple albums
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextAlbumSongs);
    
    // All playlists
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextPlaylists);
    
    // All songs in a playlist
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXPlaylist);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextPlaylistSongs);
    
    // All genres
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXGenre);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextGenres);//not in use and implementation could go away.
    
    // All songs of a genre
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXGenre);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextGenreSongs);///not in use and implementation could go away.
    
    // All composers
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXComposer);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextUnknown);//not implemented because is not in use.
    
    // All songs of a composer
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXItem);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXComposer);
    mTestPrivate->iContainerMedia = entries;
    mTestPrivate->SetCollectionContextL();
    QCOMPARE(mTestPrivate->iContext,ECollectionContextUnknown);//not implemented because is not in use.
    
    
    mTestPrivate->iContainerMedia = NULL;
    CleanupStack::PopAndDestroy(entries);
}


void TestMpMpxCollectionData::testSetMpxMediaAllSongs()
{


    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds );
    supportedIds.AppendL( KMPXMediaIdMusic );
    supportedIds.AppendL( KMPXMediaIdGeneral );
    CMPXMedia* entries = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PopAndDestroy(&supportedIds);
    CleanupStack::PushL(entries);
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL(array);
    TInt count = sizeof(KAllSongsTestData)/sizeof(TTestAttrs);
    for (TInt i =0; i < count; i++) {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTextValueL(KMPXMediaGeneralTitle, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].GeneralTitle)));
        media->SetTextValueL(KMPXMediaMusicArtist, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicArtist)));
        media->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, KAllSongsTestData[i].GeneralCount);
        media->SetTextValueL(KMPXMediaMusicAlbumArtFileName, TPtrC(reinterpret_cast<const TUint16*>(KAllSongsTestData[i].MusicAlbumArtFileName)));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    
    entries->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
    entries->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXSong);
    entries->SetTextValueL(KMPXMediaGeneralTitle, _L("All songs"));
   
    entries->SetCObjectValueL(KMPXMediaArrayContents, array);
    entries->SetTObjectValueL<TInt>(KMPXMediaArrayCount, array->Count());
    CleanupStack::PopAndDestroy(array);
 
    qRegisterMetaType<TCollectionContext>();
    QSignalSpy spy(mTest, SIGNAL(contextChanged(TCollectionContext)));
    mTest->setMpxMedia(*entries);
    QCOMPARE(spy.count(), 1);
    TCollectionContext context = qvariant_cast<TCollectionContext>(spy.at(0).at(0));
    QCOMPARE(context, ECollectionContextAllSongs);
    CleanupStack::PopAndDestroy(entries);
}