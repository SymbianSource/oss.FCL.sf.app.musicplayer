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

#ifndef TESTMPCOLLECTIONTBONELISTDATAMODEL_H
#define TESTMPCOLLECTIONTBONELISTDATAMODEL_H

#include <QtTest/QtTest>

class QTranslator;
class MpCollectionTBoneListDataModel;
class MpMpxCollectionData;
class TestHelper;

class TestMpCollectionTBoneListDataModel : public QObject
{
    Q_OBJECT

public:

    TestMpCollectionTBoneListDataModel();
    virtual ~TestMpCollectionTBoneListDataModel();

signals:

    void updateAlbumArt( int index );

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testMemberCleanup();
    void testRefreshModel();
    void testData();
    void testDataNoData();
    void testDataAnyOtherRole();

private:

    MpCollectionTBoneListDataModel  *mTest;
    TestHelper                      *mHelper;
    MpMpxCollectionData             *mStubData;
    QTranslator                     *mMpTranslator; // Own

};

#endif  // TESTMPCOLLECTIONTBONELISTDATAMODEL_H




