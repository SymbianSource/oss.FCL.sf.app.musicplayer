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

#ifndef TESTMPCOLLECTIONDATAMODEL_H
#define TESTMPCOLLECTIONDATAMODEL_H

#include <QtTest/QtTest>

class MpCollectionDataModel;
class MpMpxCollectionData;
class TestHelper;

class TestMpCollectionDataModel : public QObject
{
    Q_OBJECT

public:

    TestMpCollectionDataModel();
    virtual ~TestMpCollectionDataModel();

signals:

    void updateAlbumArt( int index );
    void albumCacheReady();

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testMemberCleanup();
    void testRefreshModelZeroCount();
    void testRefreshModelLargeData();
    void testRefreshModelSmallData();
    void testRefreshModelNoAlbumArtUri();
    void testCollectionData();
    void testDataAllSongs();
    void testDataAllSongsNoData();
    void testDataArtistAlbums();
    void testDataArtistAlbumsNoData();
    void testDataAlbumSongs();
    void testDataAlbumSongsNoData();
    void testDataPlaylists();
    void testDataPlaylistsNoData();
    void testDataPlaylistSongs();
    void testDataPlaylistSongsNoData();
    void testDataAnyOtherRole();
    void testUpdateAlbumArt();
    void testAlbumCacheReady();

private:

    MpCollectionDataModel   *mTest;
    TestHelper              *mHelper;
    MpMpxCollectionData     *mStubData;

};

#endif  // TESTMPCOLLECTIONDATAMODEL_H




