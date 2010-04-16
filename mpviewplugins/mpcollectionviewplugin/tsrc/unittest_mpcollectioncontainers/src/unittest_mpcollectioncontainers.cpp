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
* Description: Unit test for mpcollectioncontainers
*
*/

#include <qnamespace.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>
#include <hbListView.h>
#include <hbListViewItem.h>
#include "mpcollectiondocumentloader.h"
#include "stub/inc/mpcollectionview.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include "stub/inc/mpcollectiondatamodel.h"

#include "unittest_mpcollectioncontainers.h"


// Do this so we can access all member variables.
#define private public
#define protected public
#include "mpcollectioncontainerfactory.h"
#include "mpcollectioncontainer.h"
#include "mpcollectionlistcontainer.h"
#include "mpcollectioncontainerallsongs.h"
#include "mpcollectioncontaineralbums.h"
#include "mpcollectioncontainerplaylists.h"
#include "mpcollectioncontainergenres.h"
#undef private
#undef protected
/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestMpCollectionContainers tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpcollectioncontainers.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpCollectionContainers::TestMpCollectionContainers()
    : mTest(0),
      mCollectionDataModel(0),
      mCollectionData(0)
{
}

TestMpCollectionContainers::~TestMpCollectionContainers()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionContainers::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionContainers::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionContainers::init()
{
    mView = new MpCollectionView();
    mView->initializeView();
    mCollectionData = new MpMpxCollectionData();
    mCollectionDataModel = new MpCollectionDataModel(mCollectionData);
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionContainers::cleanup()
{
    delete mCollectionDataModel;
    delete mCollectionData;
    delete mView;
}

/*!
 Test constructors
 */
void TestMpCollectionContainers::testConstructors()
{
    mTest = new MpCollectionContainerAllSongs(mView->mDocumentLoader);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mNoMusic == 0);
    QCOMPARE(mTest->mCollectionContext,ECollectionContextUnknown);

    delete mTest;
    mTest = 0;
    
    mTest = new MpCollectionContainerAlbums(mView->mDocumentLoader);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mNoMusic == 0);
    QCOMPARE(mTest->mCollectionContext,ECollectionContextUnknown);
    
    delete mTest;
    mTest = 0;
    
    mTest = new MpCollectionContainerPlaylists(mView->mDocumentLoader);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mNoMusic == 0);
    QCOMPARE(mTest->mCollectionContext,ECollectionContextUnknown);
    
    delete mTest;
    mTest = 0;
    
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mNoMusic == 0);
    QCOMPARE(mTest->mCollectionContext,ECollectionContextUnknown);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAllSongs);
   
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mNoMusic == 0);
    QCOMPARE(mTest->mCollectionContext,ECollectionContextUnknown);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAlbums);
      
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mNoMusic == 0);
    QCOMPARE(mTest->mCollectionContext,ECollectionContextUnknown);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextPlaylists);        
}

/*!
 Test setupContainer via setDataModel 
 */
void TestMpCollectionContainers::testSetupContainers()
{
    mCollectionData->mCount = 1;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList != 0);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mInfoBar != 0);
    QCOMPARE(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList->itemRecycling(), true);
    QCOMPARE(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList->scrollingStyle(), HbListView::PanOrFlick);
    QCOMPARE(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList->clampingStyle(), HbListView::BounceBackClamping);
    QCOMPARE(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList->longPressEnabled(), true);
    QCOMPARE(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAllSongs);
    
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    mCollectionData->mContext = ECollectionContextAlbums;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mList != 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mInfoBar == 0);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->itemRecycling(), true);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->scrollingStyle(), HbListView::PanOrFlick);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->clampingStyle(), HbListView::BounceBackClamping);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->longPressEnabled(), true);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAlbums);
    
    mCollectionData->mContext = ECollectionContextAlbumSongs;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mList != 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mInfoBar != 0);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->itemRecycling(), true);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->scrollingStyle(), HbListView::PanOrFlick);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->clampingStyle(), HbListView::BounceBackClamping);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->longPressEnabled(), true);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(static_cast<MpCollectionContainerAlbums*>(mTest)->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAlbums);
    
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    mCollectionData->mContext = ECollectionContextPlaylists;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList != 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mInfoBar == 0);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->itemRecycling(), true);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->scrollingStyle(), HbListView::PanOrFlick);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->clampingStyle(), HbListView::BounceBackClamping);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->longPressEnabled(), true);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextPlaylists);
    
    mCollectionData->mContext = ECollectionContextPlaylistSongs;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList != 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mInfoBar != 0);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->itemRecycling(), true);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->scrollingStyle(), HbListView::PanOrFlick);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->clampingStyle(), HbListView::BounceBackClamping);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->longPressEnabled(), true);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->verticalScrollBarPolicy(), HbScrollArea::ScrollBarAsNeeded);
    QCOMPARE(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList->listItemPrototype()->graphicsSize(), HbListViewItem::Thumbnail);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextPlaylists);
    
    //No data
    mTest = 0;
    mCollectionData->mCount = 0;
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerAllSongs*>(mTest)->mNoMusic != 0);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAllSongs);
    
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    mCollectionData->mContext = ECollectionContextAlbums;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mNoMusic != 0);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAlbums);
    
    mCollectionData->mContext = ECollectionContextAlbumSongs;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerAlbums*>(mTest)->mNoMusic != 0);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextAlbums);
    
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    mCollectionData->mContext = ECollectionContextPlaylists;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mNoMusic != 0);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextPlaylists);
    
    mCollectionData->mContext = ECollectionContextPlaylistSongs;
    mTest->setDataModel(mCollectionDataModel);
    
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mList == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mInfoBar == 0);
    QVERIFY(static_cast<MpCollectionContainerPlaylists*>(mTest)->mNoMusic != 0);
    QCOMPARE(mView->mContainerFactory->mCurrentContext, ECollectionContextPlaylists);   
}

/*!
 Test itemActivated
 */
void TestMpCollectionContainers::testItemActivated()
{ 
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    
    QSignalSpy spy(mTest, SIGNAL(itemActivated( int )));
    static_cast<MpCollectionContainerAllSongs*>(mTest)->itemActivated(QModelIndex());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);
    
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    QSignalSpy spy2(mTest, SIGNAL(itemActivated( int )));
    static_cast<MpCollectionContainerAlbums*>(mTest)->itemActivated(QModelIndex());
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy2.at(0).at(0)), -1);
   
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    QSignalSpy spy3(mTest, SIGNAL(itemActivated( int )));
    static_cast<MpCollectionContainerPlaylists*>(mTest)->itemActivated(QModelIndex());
    QCOMPARE(spy3.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy3.at(0).at(0)), -1);
}

/*!
 Test onLongPressed
 */
void TestMpCollectionContainers::testOnLongPressed()
{
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAllSongs);
    QSignalSpy spy(mTest, SIGNAL(itemLongPressed( int, QPointF )));
    
    HbListView *viewItem = new HbListView();
    static_cast<MpCollectionContainerAllSongs*>(mTest)->onLongPressed(viewItem->listItemPrototype(), QPointF());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy.at(0).at(0)), -1);
        
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextAlbums);
    QSignalSpy spy2(mTest, SIGNAL(itemLongPressed( int, QPointF )));
    static_cast<MpCollectionContainerAlbums*>(mTest)->onLongPressed(viewItem->listItemPrototype(), QPointF());
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy2.at(0).at(0)), -1);
    
    mTest = 0;   
    mTest = mView->mContainerFactory->createContainer(ECollectionContextPlaylists);
    QSignalSpy spy3(mTest, SIGNAL(itemLongPressed( int, QPointF )));
    static_cast<MpCollectionContainerPlaylists*>(mTest)->onLongPressed(viewItem->listItemPrototype(), QPointF());
    QCOMPARE(spy3.count(), 1);
    QCOMPARE(qvariant_cast<int>(spy3.at(0).at(0)), -1);
}

// End of file
