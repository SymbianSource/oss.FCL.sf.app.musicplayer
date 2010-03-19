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
* Description: Unit test for mpcollectionalbumartmanager
*
*/

#ifndef TESTMPCOLLECTIONALBUMARTMANAGER_H
#define TESTMPCOLLECTIONALBUMARTMANAGER_H

#include <QtTest/QtTest>

class MpCollectionAlbumArtManager;
class HbIcon;

class TestMpCollectionAlbumArtManager : public QObject
{
    Q_OBJECT

public:

    TestMpCollectionAlbumArtManager();
    virtual ~TestMpCollectionAlbumArtManager();

signals:

    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testMemberCleanup();
    void testAlbumArtNoCache();
    void testAlbumArtNoCacheQueue();
    void testAlbumArtCache();
    void testAlbumArtFail();
    void testCacheAlbumArtEmpty();
    void testCacheAlbumArt();
    void testCacheAlbumArtCached();
    void testCacheAlbumArtAllCached();
    void testCacheAlbumArtFail();
    void testCancel();
    void testThumbnailReadyCache();
    void testThumbnailReadyCacheError();
    void testThumbnailReadyAlbumArt();
    void testThumbnailReadyAlbumArtError();
    void testThumbnailReadyAlbumArtQueue();
    void testThumbnailReadyAlbumArtQueueError();

private:

    MpCollectionAlbumArtManager *mTest;
    HbIcon                      *mIcon;

};

#endif  // TESTMPCOLLECTIONALBUMARTMANAGER_H




