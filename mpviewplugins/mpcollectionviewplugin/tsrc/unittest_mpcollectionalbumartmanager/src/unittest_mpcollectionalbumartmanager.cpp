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
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbicon.h>

#include "unittest_mpcollectionalbumartmanager.h"
#include "stub/inc/thumbnailmanager_qt.h"

// Do this so we can access all member variables.
#define private public
#include "mpcollectionalbumartmanager.h"
#undef private

// Max request defined in mpcollectionalbumartmanager.cpp
const int KMaxThumbnailReq = 5;

const char* ALBUM_URI_1 = "album1.mp3";
const char* ALBUM_URI_2 = "album2.mp3";
const char* ALBUM_URI_3 = "album3.mp3";

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpCollectionAlbumArtManager tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpcollectionalbumartmanager.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpCollectionAlbumArtManager::TestMpCollectionAlbumArtManager()
    : mTest(0)
{
}

TestMpCollectionAlbumArtManager::~TestMpCollectionAlbumArtManager()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionAlbumArtManager::initTestCase()
{
    // Create an icon (any icon will do) to help with testing.
    mIcon = new HbIcon( QString(":/icons/default_album.png"));
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionAlbumArtManager::cleanupTestCase()
{
    delete mIcon;
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionAlbumArtManager::init()
{
    mTest = new MpCollectionAlbumArtManager();
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionAlbumArtManager::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests correct cleanup of member variables.
 */
void TestMpCollectionAlbumArtManager::testMemberCleanup()
{
    cleanup();
    ThumbnailManager::resetInitCounter();
    init();
    cleanup();
    QCOMPARE(ThumbnailManager::getInitCounter(), 0);
}

/*!
 Tests albumArt() request when album art is not in local cache.
 Single request.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtNoCache()
{
    HbIcon icon = mTest->albumArt(ALBUM_URI_1, 1);
    // Verify that:
    // - It didn't return an icon
    // - A request has been made to thumbnail manager
    QVERIFY(icon.isNull() == true);
    QCOMPARE(mTest->mRequestCount, 1);
    QCOMPARE(mTest->mTnmReqMap.count(), 1);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 1);
}

/*!
 Tests albumArt() request when art is not in local cache.
 Request more than max to test request queue.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtNoCacheQueue()
{
    int max = KMaxThumbnailReq + 1;
    for ( int i = 0; i < max; i++) {
        HbIcon icon = mTest->albumArt(ALBUM_URI_1, i);
        QVERIFY(icon.isNull() == true);
    }
    // Verify that:
    // - Max number of requests were sent to thumbnail manager
    // - A request has been queued
    QCOMPARE(mTest->mRequestCount, KMaxThumbnailReq);
    QCOMPARE(mTest->mTnmReqMap.count(), KMaxThumbnailReq);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, KMaxThumbnailReq);
    QCOMPARE(mTest->mRequestQueue.count(), 1);
}

/*!
 Tests albumArt() request when art is in local cache.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtCache()
{
    mTest->mImageCache.insert(ALBUM_URI_1, *mIcon);

    HbIcon icon = mTest->albumArt(ALBUM_URI_1, 1);
    // Verify that:
    // - A valid icon has been returned
    // - No request has been sent to thumbnail manager
    QVERIFY(icon.isNull() == false);
    QVERIFY(icon == *mIcon);
    QCOMPARE(mTest->mRequestCount, 0);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 0);
}

/*!
 Tests albumArt() request when thumbnail request fails.
 */
void TestMpCollectionAlbumArtManager::testAlbumArtFail()
{
    mTest->mThumbnailManager->mGetThumbFails = true;
    HbIcon icon = mTest->albumArt(ALBUM_URI_1, 1);
    // Verify that:
    // - It didn't return an icon
    // - There is no request pending from thumbnail manager
    QVERIFY(icon.isNull() == true);
    QCOMPARE(mTest->mRequestCount, 0);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
}

/*!
 Tests cacheAlbumArt() request with empty list.
 */
void TestMpCollectionAlbumArtManager::testCacheAlbumArtEmpty()
{
    bool cachingStarted = mTest->cacheAlbumArt(QStringList());
    QCOMPARE(cachingStarted, false);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 0);
}

/*!
 Tests cacheAlbumArt() request with 3 URIs.
 */
void TestMpCollectionAlbumArtManager::testCacheAlbumArt()
{
    QStringList list;
    list << ALBUM_URI_1 << ALBUM_URI_2 << ALBUM_URI_3;
    bool cachingStarted = mTest->cacheAlbumArt(list);
    QCOMPARE(cachingStarted, true);
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mTnmReqMap.count(), 3);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 3);
}

/*!
 Tests cacheAlbumArt() request with one item already in local cache.
 */
void TestMpCollectionAlbumArtManager::testCacheAlbumArtCached()
{
    mTest->mImageCache.insert(ALBUM_URI_1, *mIcon);
    QStringList list;
    list << ALBUM_URI_1 << ALBUM_URI_2 << ALBUM_URI_3;
    bool cachingStarted = mTest->cacheAlbumArt(list);
    QCOMPARE(cachingStarted, true);
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mTnmReqMap.count(), 2);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 2);
}

/*!
 Tests cacheAlbumArt() request with all items already in local cache.
 */
void TestMpCollectionAlbumArtManager::testCacheAlbumArtAllCached()
{
    mTest->mImageCache.insert(ALBUM_URI_1, *mIcon);
    mTest->mImageCache.insert(ALBUM_URI_2, *mIcon);
    mTest->mImageCache.insert(ALBUM_URI_3, *mIcon);
    QStringList list;
    list << ALBUM_URI_1 << ALBUM_URI_2 << ALBUM_URI_3;
    bool cachingStarted = mTest->cacheAlbumArt(list);
    QCOMPARE(cachingStarted, false);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 0);
}

/*!
 Tests cacheAlbumArt() request when thumbnail request fails.
 */
void TestMpCollectionAlbumArtManager::testCacheAlbumArtFail()
{
    mTest->mThumbnailManager->mGetThumbFails = true;
    QStringList list;
    list << ALBUM_URI_1 << ALBUM_URI_2 << ALBUM_URI_3;
    bool cachingStarted = mTest->cacheAlbumArt(list);
    QCOMPARE(cachingStarted, false);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
}

/*!
 Tests cancel() request.
 */
void TestMpCollectionAlbumArtManager::testCancel()
{
    // First send enough requests to trigger max number of request to
    // thumbnail manager and also queue the extra request.
    int max = KMaxThumbnailReq + 1;
    for ( int i = 0; i < max; i++) {
        HbIcon icon = mTest->albumArt(ALBUM_URI_1, i);
        QVERIFY(icon.isNull() == true);
    }
    QCOMPARE(mTest->mRequestCount, KMaxThumbnailReq);
    QCOMPARE(mTest->mTnmReqMap.count(), KMaxThumbnailReq);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, KMaxThumbnailReq);
    QCOMPARE(mTest->mRequestQueue.count(), 1);

    // Verify that:
    // - All requests to thumbnail manager are cancelled
    // - Queue is emptied
    mTest->cancel();
    QCOMPARE(mTest->mRequestCount, 0);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mRequestQueue.count(), 0);
    QCOMPARE(mTest->mThumbnailManager->mCancelCounter, KMaxThumbnailReq);
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of cacheAlbumArt() request.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyCache()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumCacheReady()));

    // First send cacheAlbumArt() request with 3 items.
    QStringList list;
    list << ALBUM_URI_1 << ALBUM_URI_2 << ALBUM_URI_3;
    bool cachingStarted = mTest->cacheAlbumArt(list);
    QCOMPARE(cachingStarted, true);
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mTnmReqMap.count(), 3);

    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 3);

    // Emit thumbnailReady() signal for each request. Then verify that:
    // - Test object emitted signal albumCacheReady()
    // - 3 items are present in local cache
    QMapIterator<int, QString> iter(mTest->mTnmReqMap);
    while ( iter.hasNext() ) {
        iter.next();
        int reqId = iter.key();
        emit thumbnailReady(mIcon->pixmap(), 0, reqId, 0);
    }
    QCOMPARE(spy.count(), 1);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 3);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of cacheAlbumArt() request with error code returned.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyCacheError()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumCacheReady()));

    // First send cacheAlbumArt() request with 3 items.
    QStringList list;
    list << ALBUM_URI_1 << ALBUM_URI_2 << ALBUM_URI_3;
    bool cachingStarted = mTest->cacheAlbumArt(list);
    QCOMPARE(cachingStarted, true);
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mTnmReqMap.count(), 3);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, 3);

    // Emit thumbnailReady() signal for each request with error. Then verify that:
    // - Test object emitted signal albumCacheReady()
    // - 0 item is present in local cache
    QMapIterator<int, QString> iter(mTest->mTnmReqMap);
    while ( iter.hasNext() ) {
        iter.next();
        int reqId = iter.key();
        emit thumbnailReady(mIcon->pixmap(), 0, reqId, -1);
    }
    QCOMPARE(spy.count(), 1);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mRequestCount, 0);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 0);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of albumArt() request with empty request queue.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyAlbumArt()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send 3 albumArt() requests.
    HbIcon icon = mTest->albumArt(ALBUM_URI_1, 1);
    QVERIFY(icon.isNull() == true);
    icon = mTest->albumArt(ALBUM_URI_2, 2);
    QVERIFY(icon.isNull() == true);
    icon = mTest->albumArt(ALBUM_URI_3, 3);
    QVERIFY(icon.isNull() == true);
    QCOMPARE(mTest->mRequestCount, 3);
    QCOMPARE(mTest->mTnmReqMap.count(), 3);

    // Emit thumbnailReady() signal for each request. Then verify that:
    // - Test object emitted signal albumArtReady() 3 times
    // - 3 items are present in local cache
    QMapIterator<int, QString> iter(mTest->mTnmReqMap);
    while ( iter.hasNext() ) {
        iter.next();
        int reqId = iter.key();
        int *clientData = new int(reqId);
        emit thumbnailReady(mIcon->pixmap(), clientData, reqId, 0);
    }
    QCOMPARE(spy.count(), 3);
    QCOMPARE(mTest->mRequestCount, 0);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 3);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of albumArt() request with error code returned.
 This covers a case with no pending items in the queue.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyAlbumArtError()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send 3 albumArt() requests.
    HbIcon icon = mTest->albumArt(ALBUM_URI_1, 1);
    QVERIFY(icon.isNull() == true);
    icon = mTest->albumArt(ALBUM_URI_2, 2);
    QVERIFY(icon.isNull() == true);
    icon = mTest->albumArt(ALBUM_URI_3, 3);
    QVERIFY(icon.isNull() == true);
    QCOMPARE(mTest->mRequestCount, 3);
    QCOMPARE(mTest->mTnmReqMap.count(), 3);

    // Emit thumbnailReady() signal for each request with error. Then verify that:
    // - Test object doesn't emitted signal albumArtReady()
    // - 0 items are present in local cache
    QMapIterator<int, QString> iter(mTest->mTnmReqMap);
    while ( iter.hasNext() ) {
        iter.next();
        int reqId = iter.key();
        int *clientData = new int(reqId);
        emit thumbnailReady(mIcon->pixmap(), clientData, reqId, -1);
    }
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mTest->mRequestCount, 0);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mImageCache.count(), 0);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of albumArt() request with pending request in queue.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyAlbumArtQueue()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send enough requests to trigger max number of request to
    // thumbnail manager and also queue the extra request.
    int max = KMaxThumbnailReq + 1;
    for ( int i = 0; i < max; i++) {
        HbIcon icon = mTest->albumArt(ALBUM_URI_1, i);
        QVERIFY(icon.isNull() == true);
    }
    QCOMPARE(mTest->mRequestCount, KMaxThumbnailReq);

    // Emit thumbnailReady() signal for each request. Then verify that:
    // - Test object emitted signal albumArtReady() 5 times
    // - The extra request in queue is sent to thumbnail manager
    QMapIterator<int, QString> iter(mTest->mTnmReqMap);
    while ( iter.hasNext() ) {
        iter.next();
        int reqId = iter.key();
        int *clientData = new int(reqId);
        emit thumbnailReady(mIcon->pixmap(), clientData, reqId, 0);
    }
    QCOMPARE(spy.count(), KMaxThumbnailReq);
    QCOMPARE(mTest->mRequestCount, 1);
    QCOMPARE(mTest->mTnmReqMap.count(), 1);
    QCOMPARE(mTest->mThumbnailManager->mThumbnailReqCounter, max);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Tests thumbnailReady() slot.
 Tests completion of albumArt() request with pending request in queue.
 Queue item's request for thumbnail fails.
 */
void TestMpCollectionAlbumArtManager::testThumbnailReadyAlbumArtQueueError()
{
    connect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
    QSignalSpy spy(mTest, SIGNAL(albumArtReady(int)));

    // First send enough requests to trigger max number of request to
    // thumbnail manager and also queue the extra request.
    int max = KMaxThumbnailReq + 1;
    for ( int i = 0; i < max; i++) {
        HbIcon icon = mTest->albumArt(ALBUM_URI_1, i);
        QVERIFY(icon.isNull() == true);
    }
    QCOMPARE(mTest->mRequestCount, KMaxThumbnailReq);

    mTest->mThumbnailManager->mGetThumbFails = true;
    // Emit thumbnailReady() signal for each request. Then verify that:
    // - The extra request in queue is removed
    // - There is no request pending from thumbnail manager
    QMapIterator<int, QString> iter(mTest->mTnmReqMap);
    while ( iter.hasNext() ) {
        iter.next();
        int reqId = iter.key();
        int *clientData = new int(reqId);
        emit thumbnailReady(mIcon->pixmap(), clientData, reqId, 0);
    }
    QCOMPARE(spy.count(), KMaxThumbnailReq);
    QCOMPARE(mTest->mRequestCount, 0);
    QCOMPARE(mTest->mTnmReqMap.count(), 0);
    QCOMPARE(mTest->mRequestQueue.isEmpty(), true);
    disconnect( this, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
        mTest->mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)) );
}

// End of file
