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
* Description: Unit test for mpcollectiondatamodel
*
*/

#include <QAbstractItemModel>
#include <QSignalSpy>
#include <QMetaType>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbicon.h>

#include "unittest_mpcollectiondatamodel.h"
#include "unittest_helper.h"
#include "stub/inc/mpcollectionalbumartmanager.h"
#include "stub/inc/mpmpxcollectiondata.h"

// Do this so we can access all member variables.
#define private public
#include "mpcollectiondatamodel.h"
#undef private

// Init cache size defined in mpcollectiondatamodel.cpp
const int KInitCacheSize = 11;

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpCollectionDataModel tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpcollectiondatamodel.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpCollectionDataModel::TestMpCollectionDataModel()
    : mTest(0),
      mHelper(0),
      mStubData(0)
{
}

TestMpCollectionDataModel::~TestMpCollectionDataModel()
{
    delete mTest;
    delete mHelper;
    delete mStubData;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionDataModel::initTestCase()
{
    mStubData = new MpMpxCollectionData();
    mHelper = new TestHelper();
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionDataModel::cleanupTestCase()
{
    delete mStubData;
    mStubData = 0;
    delete mHelper;
    mHelper = 0;
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionDataModel::init()
{
    mTest = new MpCollectionDataModel(mStubData);
    MpMpxCollectionData::resetItemDataCounter();
    mTest->mCollectionData->mItemDataReturn = true;
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionDataModel::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests correct cleanup of member variables.
 */
void TestMpCollectionDataModel::testMemberCleanup()
{
    cleanup();
    MpCollectionAlbumArtManager::resetInitCounter();
    int count = MpMpxCollectionData::getInitCounter();
    init();
    cleanup();
    QCOMPARE(MpCollectionAlbumArtManager::getInitCounter(), 0);
    QCOMPARE(MpMpxCollectionData::getInitCounter(), count);
}

/*!
 Tests refreshModel() request with empty data model.
 */
void TestMpCollectionDataModel::testRefreshModelZeroCount()
{
    mTest->mCollectionData->mCount = 0;
    mTest->refreshModel();
    // Verify that:
    // - It sets correct row count
    // - It doesn't attempt to cache initial set of album art
    QCOMPARE(mTest->mRowCount, 0);
    QCOMPARE(MpMpxCollectionData::getItemDataCounter(), 0);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mAlbumArtManager->mCacheAlbumArtCount, 0);
    // AK - Stub QAbstractListModel to verify call to reset()
}

/*!
 Tests refreshModel() request with large data size.
 */
void TestMpCollectionDataModel::testRefreshModelLargeData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->refreshModel();
    // Verify that:
    // - It sets correct row count
    // - It caches the initial cache size
    QCOMPARE(mTest->mRowCount, 100);
    QCOMPARE(MpMpxCollectionData::getItemDataCounter(), KInitCacheSize);
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mAlbumArtManager->mCacheAlbumArtCount, 1);

    // Verify that:
    // - rowCount returns 0 when caching is in progress
    QCOMPARE(mTest->rowCount(QModelIndex()), 0);
}

/*!
 Tests refreshModel() request with small data size.
 */
void TestMpCollectionDataModel::testRefreshModelSmallData()
{
    mTest->mCollectionData->mCount = 3;
    mTest->mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->refreshModel();
    // Verify that:
    // - It sets correct row count
    // - It caches all data
    QCOMPARE(mTest->mRowCount, 3);
    QCOMPARE(MpMpxCollectionData::getItemDataCounter(), 3);
    QCOMPARE(mTest->mCachingInProgress, true);
    QCOMPARE(mTest->mAlbumArtManager->mCacheAlbumArtCount, 1);
}

/*!
 Tests refreshModel() request with data that has no album art.
 */
void TestMpCollectionDataModel::testRefreshModelNoAlbumArtUri()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mCollectionData->mItemDataReturn = false;
    mTest->mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->refreshModel();
    // Verify that:
    // - It sets correct row count
    // - Caching is not in progress
    QCOMPARE(mTest->mRowCount, 100);
    QCOMPARE(MpMpxCollectionData::getItemDataCounter(), KInitCacheSize);
    QCOMPARE(mTest->mCachingInProgress, false);
    QCOMPARE(mTest->mAlbumArtManager->mCacheAlbumArtCount, 1);

    // Verify that:
    // - rowCount returns actual data size when caching is not in progress
    QCOMPARE(mTest->rowCount(QModelIndex()), 100);
}

/*!
 Tests collectionData() request.
 */
void TestMpCollectionDataModel::testCollectionData()
{
    QVERIFY(mTest->collectionData() == mStubData);
}

/*!
 Tests data() request for AllSongs context.
 */
void TestMpCollectionDataModel::testDataAllSongs()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextAllSongs;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Title"));
    QCOMPARE(dataList.at(1), QString("Artist"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
}

/*!
 Tests data() request for AllSongs context with no data available.
 */
void TestMpCollectionDataModel::testDataAllSongsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextAllSongs;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Unknown"));
    QCOMPARE(dataList.at(1), QString("Unknown"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
}

/*!
 Tests data() request for ArtistAlbums context.
 */
void TestMpCollectionDataModel::testDataArtistAlbums()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtistAlbums;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Title"));
    // AK - Second data will eventually change to "Album"
    QCOMPARE(dataList.at(1), QString("Artist"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.userType(), QMetaType::type("HbIcon"));
    HbIcon icon = data.value<HbIcon>();
    QCOMPARE(icon.iconName(), QString(":/icons/artists"));
}

/*!
 Tests data() request for ArtistAlbums context with no data available.
 */
void TestMpCollectionDataModel::testDataArtistAlbumsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Unknown"));
    QCOMPARE(dataList.at(1), QString("Unknown"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.userType(), QMetaType::type("HbIcon"));
    HbIcon icon = data.value<HbIcon>();
    QCOMPARE(icon.iconName(), QString(":/icons/default_album.png"));
}

/*!
 Tests data() request for AlbumSongs context.
 */
void TestMpCollectionDataModel::testDataAlbumSongs()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextAlbumSongs;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), QString("Title"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
}

/*!
 Tests data() request for AlbumSongs context with no data available.
 */
void TestMpCollectionDataModel::testDataAlbumSongsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextAlbumSongs;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), QString("Unknown"));
}

/*!
 Tests data() request for Playlists context.
 */
void TestMpCollectionDataModel::testDataPlaylists()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextPlaylists;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Title"));
    QCOMPARE(dataList.at(1), QString("Count"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
}

/*!
 Tests data() request for Playlists context with no data available.
 */
void TestMpCollectionDataModel::testDataPlaylistsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextPlaylists;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Unknown"));
    QCOMPARE(dataList.at(1), QString("Count"));
}

/*!
 Tests data() request for PlaylistSongs context.
 */
void TestMpCollectionDataModel::testDataPlaylistSongs()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextPlaylistSongs;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Title"));
    QCOMPARE(dataList.at(1), QString("Artist"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
}

/*!
 Tests data() request for PlaylistSongs context with no data available.
 */
void TestMpCollectionDataModel::testDataPlaylistSongsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextPlaylistSongs;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Unknown"));
    QCOMPARE(dataList.at(1), QString("Unknown"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
}

/*!
 Tests data() request for a role it doesn't support.
 Make sure it doesn't crash.
 */
void TestMpCollectionDataModel::testDataAnyOtherRole()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::EditRole);
    QCOMPARE(data.isValid(), false);
}

/*!
 Tests updateAlbumArt() slot.
 */
void TestMpCollectionDataModel::testUpdateAlbumArt()
{
    connect( this, SIGNAL(updateAlbumArt(int)),
        mTest->mAlbumArtManager, SIGNAL(albumArtReady(int)) );
    qRegisterMetaType<QModelIndex>("QModelIndex");
    QSignalSpy spy(mTest, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;

    emit updateAlbumArt(15);
    QCOMPARE(spy.count(), 1);

    disconnect( this, SIGNAL(updateAlbumArt(int)),
        mTest->mAlbumArtManager, SIGNAL(albumArtReady(int)) );
}

/*!
 Tests albumCacheReady() slot.
 */
void TestMpCollectionDataModel::testAlbumCacheReady()
{
    connect( this, SIGNAL(albumCacheReady()),
        mTest->mAlbumArtManager, SIGNAL(albumCacheReady()) );
    mTest->mCachingInProgress = true;

    emit albumCacheReady();
    QCOMPARE(mTest->mCachingInProgress, false);

    disconnect( this, SIGNAL(albumCacheReady()),
        mTest->mAlbumArtManager, SIGNAL(albumCacheReady()) );
}

// End of file
