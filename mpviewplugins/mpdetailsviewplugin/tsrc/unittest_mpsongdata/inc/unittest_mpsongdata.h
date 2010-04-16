/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Unit test for mpsongdata
*
*/

#ifndef UNITTEST_MPSONGDATA_H_
#define UNITTEST_MPSONGDATA_H_

#include <QtTest/QtTest>

class MpSongData;

class TestMpSongData : public QObject
{
    Q_OBJECT

public:

    TestMpSongData();
    virtual ~TestMpSongData();

signals:

    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:
    void testMemberCleanup();
    void testAlbumArtIsNull();
    void testAlbumArtNotNull();
    void testTitleIsNull();
    void testAlbumIsNull();
    void testArtistIsNull();
    void testCommentIsNull();
    void testComposerIsNull();
    void testGenreIsNull();
    void testAlbumtrackIsNull();
    void testLinkIsNull();
    void testYearIsNull();
    void testSetTitleTrue();
    void testSetTitleFalse();
    void testsetAlbumTrue();
    void testsetAlbumFalse();
    void testsetArtistTrue();
    void testsetArtistFalse();
    void testsetCommentTrue();
    void testsetCommentFalse();
    void testsetComposerTrue();
    void testsetComposerFalse();
    void testsetGenreTrue();
    void testsetGenreFalse();
    void testsetYearTrue();
    void testsetYearFalse();
    void testsetAlbumTrackTrue();
    void testsetAlbumTrackFalse();
    void testsetAlbumArtUri();
    void testThumbnailReady();
    void testCommitPlaybackInfo();
    void testCommitSongDetailInfo();
private:
    MpSongData *mTest;
    
};


#endif /* UNITTEST_MPSONGDATA_H_ */
