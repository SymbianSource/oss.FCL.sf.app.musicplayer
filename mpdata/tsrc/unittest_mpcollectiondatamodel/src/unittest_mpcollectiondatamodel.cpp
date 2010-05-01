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
#include <QTranslator>
#include <QLocale>

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbicon.h>
#include <hbnamespace.h>

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
      mStubData(0),
      mMpTranslator(0)
{
}

TestMpCollectionDataModel::~TestMpCollectionDataModel()
{
    delete mTest;
    delete mHelper;
    delete mStubData;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionDataModel::initTestCase()
{
    QString lang = QLocale::system().name();
    QString path = QString("z:/resource/qt/translations/");
    bool translatorLoaded = false;

    mMpTranslator = new QTranslator(this);
    translatorLoaded = mMpTranslator->load(path + "musicplayer_" + lang);
    if ( translatorLoaded ) {
        qApp->installTranslator(mMpTranslator);
    }

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
 Tests refreshModel() request.
 */
void TestMpCollectionDataModel::testRefreshModel()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextAlbums;
    mTest->refreshModel();
    // Verify that:
    // - It sets correct row count
    QCOMPARE(mTest->mRowCount, 100);
    QCOMPARE(mTest->mAlbumArtManager->mCacheAlbumArtCount, 1);
    // Stub QAbstractListModel to verify call to reset()
    mTest->mCollectionData->mCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtistAlbums;
    mTest->refreshModel();
    // Verify that:
    // - It sets correct row count
    QCOMPARE(mTest->mRowCount, 100);
    QCOMPARE(mTest->mAlbumArtManager->mCacheAlbumArtCount, 2);
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
    QCOMPARE(mTest->mRowCount, 0);
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
    
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
    
    
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
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    QCOMPARE(dataList.at(1), hbTrId("txt_mus_other_unknown3"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
    
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
}

/*!
 Tests data() request for Artists context.
 */
void TestMpCollectionDataModel::testDataArtists()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtists;

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
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
}

/*!
 Tests data() request for Artists context with no data available.
 */
void TestMpCollectionDataModel::testDataArtistsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtists;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
}

/*!
 Tests data() request for Artist Albums context.
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
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), QString("Title"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.userType(), QMetaType::type("QIcon"));
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
}

/*!
 Tests data() request for Artist Albums context with no data available.
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
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.userType(), QMetaType::type("QIcon"));
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
}

/*!
 Tests data() request for Artist Songs context.
 */
void TestMpCollectionDataModel::testDataArtistSongs()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtistSongs;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), QString("Title"));
    QCOMPARE(dataList.at(1), QString("Album"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
}

/*!
 Tests data() request for Artists context with no data available.
 */
void TestMpCollectionDataModel::testDataArtistSongsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextArtistSongs;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
}


/*!
 Tests data() request for Albums context.
 */
void TestMpCollectionDataModel::testDataAlbums()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextAlbums;

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
    QCOMPARE(data.userType(), QMetaType::type("QIcon"));
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
}

/*!
 Tests data() request for Albums context with no data available.
 */
void TestMpCollectionDataModel::testDataAlbumsNoData()
{
    mTest->mCollectionData->mCount = 100;
    mTest->mRowCount = 100;
    mTest->mCollectionData->mContext = ECollectionContextAlbums;
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 2);
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    QCOMPARE(dataList.at(1), hbTrId("txt_mus_other_unknown3"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.userType(), QMetaType::type("QIcon"));
        
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
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
    
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
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
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));

    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
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
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), QString("Title"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
    
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
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
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
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
    
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString("Title"));
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
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
    QCOMPARE(dataList.at(1), hbTrId("txt_mus_other_unknown3"));

    // Qt::DecorationRole
    data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isNull(), true);
    
    // Hb::IndexFeedbackRole
    data = mTest->data(modelIndex, Hb::IndexFeedbackRole);
    QCOMPARE(data.toString(), QString(""));
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
 Tests supportedDropActions()
 */
void TestMpCollectionDataModel::testSupportedDropActions()
{
 QCOMPARE(mTest->supportedDropActions(), Qt::MoveAction);
}

/*!
 Tests removeRows()
 */
void TestMpCollectionDataModel::testRemoveRows()
{
    QSignalSpy spy(mTest, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    QSignalSpy spy2(mTest, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    MpMpxCollectionData::resetItemDataCounter();
    mTest->mCollectionData->mCount = 2;
    mTest->mRowCount = 2;
    
    QVERIFY(!mTest->removeRows(0,0,QModelIndex())); //no rows to remove.
    QVERIFY(!mTest->removeRows(0,2,QModelIndex())); //more than one row.
    QVERIFY(!mTest->removeRows(2,1,QModelIndex())); //row out of bounds.
    
    //a valid row to be removed.
    QVERIFY(mTest->removeRows(0,1,QModelIndex()));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(MpMpxCollectionData::getremoveItemCounter(), 1);
    QCOMPARE(mTest->mRowCount, 1);
    
}

/*!
 Tests mimeTypes()
 */
void TestMpCollectionDataModel::testMimeTypes()
{
 QCOMPARE(mTest->mimeTypes().count(), 1);
 QCOMPARE(mTest->mimeTypes().at(0), QLatin1String("application/x-mpcollectiondatamodelrowandids"));
}

/*!
 Tests mimeTypes()
 */
void TestMpCollectionDataModel::testMimeData()
{
    QModelIndexList indexList;
    indexList << mHelper->indexFor(2);
    indexList << mHelper->indexFor(5);
    QVERIFY(!mTest->mimeData(indexList)); //more than one item is not valid
    indexList.removeLast();
    QMimeData *data;
    data = mTest->mimeData(indexList);
    QVERIFY(data); //one item row 2
    
    QByteArray encoded = data->data(QLatin1String("application/x-mpcollectiondatamodelrowandids"));
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    int rowFrom;
    int mpxContainerId;
    int mpxItemId;
    stream >> rowFrom;
    stream >> mpxContainerId;
    stream >> mpxItemId;
    QCOMPARE(rowFrom,2);
    QCOMPARE(mpxContainerId,65535); //dummy value from the stub collection data stub
    QCOMPARE(mpxItemId,2000+2); //id from collection data stub is index with offset of 2000.
}

/*!
 Tests dropMimeData()
 */
void TestMpCollectionDataModel::testDropMimeData()
{
    int indexFrom = 5;
    int containerId = 123;
    int itemId = 20;
    
    QSignalSpy spy(mTest, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    QSignalSpy spy2(mTest, SIGNAL(rowsInserted(QModelIndex, int, int)));
    QSignalSpy spy3(mTest, SIGNAL(orderChanged(int, int, int, int)));
    MpMpxCollectionData::resetInsertCachedItemCounter();

    {//no data
    MpMpxCollectionData::setCachedItemId(itemId);
    mTest->mRowCount = 10;
    QVERIFY(!mTest->dropMimeData(0,Qt::MoveAction,0,0,QModelIndex()));
    }

    {//drag row out of bounds
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << 15;
    stream << containerId;
    stream << itemId;
    data->setData(QLatin1String("application/x-mpcollectiondatamodelrowandids"), encoded);
    MpMpxCollectionData::setCachedItemId(itemId);
    mTest->mRowCount = 10;

    QVERIFY(!mTest->dropMimeData(data,Qt::MoveAction,0,0,QModelIndex()));
    delete data;
    }    
    
    {//drop row out of bounds
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexFrom;
    stream << containerId;
    stream << itemId;
    data->setData(QLatin1String("application/x-mpcollectiondatamodelrowandids"), encoded);
    MpMpxCollectionData::setCachedItemId(itemId);
    mTest->mRowCount = 10;

    QVERIFY(!mTest->dropMimeData(data,Qt::MoveAction,15,0,QModelIndex()));
    delete data;
    }    
    
    {//incorrect action
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexFrom;
    stream << containerId;
    stream << itemId;
    data->setData(QLatin1String("application/x-mpcollectiondatamodelrowandids"), encoded);
    MpMpxCollectionData::setCachedItemId(itemId);
    mTest->mRowCount = 10;

    QVERIFY(!mTest->dropMimeData(data,Qt::IgnoreAction,0,0,QModelIndex()));
    delete data;
    }    
    
    {//bad item ids
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexFrom;
    data->setData(QLatin1String("application/x-mpcollectiondatamodelrowandids"), encoded);
    MpMpxCollectionData::setCachedItemId(itemId);
    mTest->mRowCount = 2;
    QVERIFY(!mTest->dropMimeData(data,Qt::MoveAction,0,0,QModelIndex()));
    delete data;
    }  
    
    {//bad item ids 2
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexFrom;
    stream << -1;
    stream << itemId;
    data->setData(QLatin1String("application/x-mpcollectiondatamodelrowandids"), encoded);
    MpMpxCollectionData::setCachedItemId(itemId);
    mTest->mRowCount = 2;
    QVERIFY(!mTest->dropMimeData(data,Qt::MoveAction,0,0,QModelIndex()));
    delete data;
    } 
    
    {//data with bad format
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexFrom;
    stream << containerId;
    stream << itemId;
    data->setData(QLatin1String("application/x-blabla"), encoded);
    MpMpxCollectionData::setCachedItemId(itemId);
    mTest->mRowCount = 10;        
    QVERIFY(!mTest->dropMimeData(data,Qt::MoveAction,0,0,QModelIndex()));
    delete data;
    }    
     
    QMimeData *data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexFrom;
    stream << containerId;
    stream << itemId;
    data->setData(QLatin1String("application/x-mpcollectiondatamodelrowandids"), encoded);
    MpMpxCollectionData::setCachedItemId(0);
    mTest->mRowCount = 10;
    
    // not matching item id
    QVERIFY(!mTest->dropMimeData(data,Qt::MoveAction,0,0,QModelIndex()));
   
    //matching item id
    MpMpxCollectionData::setCachedItemId(itemId);

    
    QVERIFY(mTest->dropMimeData(data,Qt::MoveAction,5,0,QModelIndex()));
    delete data;    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments = spy3.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), containerId);
    QCOMPARE(arguments.at(1).toInt(), itemId);
    QCOMPARE(arguments.at(2).toInt(), indexFrom);
    QCOMPARE(arguments.at(3).toInt(), 5);
    QCOMPARE(MpMpxCollectionData::getInsertCachedItemCounter(),1);
}


// End of file