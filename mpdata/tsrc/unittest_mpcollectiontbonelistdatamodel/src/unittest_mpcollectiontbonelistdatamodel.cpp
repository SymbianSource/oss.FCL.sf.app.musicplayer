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
* Description: Unit test for mpcollectiontbonelistdatamodel
*
*/

#include <QAbstractItemModel>
#include <QSignalSpy>
#include <QMetaType>
#include <QTranslator>
#include <QLocale>

#include <hbicon.h>
#include <hbnamespace.h>

#include "unittest_mpcollectiontbonelistdatamodel.h"
#include "unittest_helper.h"
#include "stub/inc/mpmpxcollectiondata.h"

// Do this so we can access all member variables.
#define private public
#include "mpcollectiontbonelistdatamodel.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpCollectionTBoneListDataModel tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpcollectiontbonelistdatamodel.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpCollectionTBoneListDataModel::TestMpCollectionTBoneListDataModel()
    : mTest(0),
      mHelper(0),
      mStubData(0),
      mMpTranslator(0)
{
}

TestMpCollectionTBoneListDataModel::~TestMpCollectionTBoneListDataModel()
{
    delete mTest;
    delete mHelper;
    delete mStubData;
    delete mMpTranslator;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionTBoneListDataModel::initTestCase()
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
void TestMpCollectionTBoneListDataModel::cleanupTestCase()
{
    delete mStubData;
    mStubData = 0;
    delete mHelper;
    mHelper = 0;
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionTBoneListDataModel::init()
{
    mTest = new MpCollectionTBoneListDataModel(mStubData);
    mTest->mCollectionData->mItemDataReturn = true;
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionTBoneListDataModel::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests correct cleanup of member variables.
 */
void TestMpCollectionTBoneListDataModel::testMemberCleanup()
{
    cleanup();
    int count = MpMpxCollectionData::getInitCounter();
    init();
    cleanup();
    QCOMPARE(MpMpxCollectionData::getInitCounter(), count);
}

/*!
 Tests refreshModel() request.
 */
void TestMpCollectionTBoneListDataModel::testRefreshModel()
{
    mTest->mCollectionData->mAlbumSongCount = 10;
    mTest->refreshModel();
    QModelIndex modelIndex = mHelper->indexFor(1);
    // Verify that:
    // - It sets correct row count
    QCOMPARE(mTest->rowCount(modelIndex), 10);
}

/*!
 Tests data() request.
 */
void TestMpCollectionTBoneListDataModel::testData()
{
    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), QString("Title1"));
}

/*!
 Tests data() request with no data available.
 */
void TestMpCollectionTBoneListDataModel::testDataNoData()
{
    mTest->mCollectionData->mItemDataReturn = false;

    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DisplayRole
    QVariant data = mTest->data(modelIndex, Qt::DisplayRole);
    QCOMPARE(data.canConvert(QVariant::StringList), true);
    QStringList dataList = data.toStringList();
    QCOMPARE(dataList.count(), 1);
    QCOMPARE(dataList.at(0), hbTrId("txt_mus_other_unknown4"));
}

/*!
 Tests data() request for a role it doesn't support.
 Make sure it doesn't crash.
 */
void TestMpCollectionTBoneListDataModel::testDataAnyOtherRole()
{
    QModelIndex modelIndex = mHelper->indexFor(1);

    // Qt::DecorationRole
    QVariant data = mTest->data(modelIndex, Qt::DecorationRole);
    QCOMPARE(data.isValid(), false);
}

