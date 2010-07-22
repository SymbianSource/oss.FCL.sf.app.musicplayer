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

#ifndef UNITTEST_MPSHAREDATA_H_
#define UNITTEST_MPSHAREDATA_H_

#include <QtTest/QtTest>

class MpShareData;

class TestMpShareData : public QObject
{
    Q_OBJECT

public:

    TestMpShareData();
    virtual ~TestMpShareData();

signals:

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testOwner();
    void testSongData();
    void testErrorMessage();
    void testUserName();
    void testPassword();
    void testObjectTypeWithLink();
    void testObjectTypeWithoutLink();
    void testObjectContentWithLink();
    void testObjectContentWithTitleAndArtist();
    void testObjectContentWithTitleOnly();
    void testObjectContentWithArtistOnly();
    void testObjectContentWithoutMetaInfo();
    void testSongTitle();
//    void testSongAlbum();
    void testSongArtist();
    void testAlbumArt();
    void testLanguage();
//    void testSongComment();
//    void testSongAlbumArt();
//    void testSongYear();
//    void testSongGenre();
//    void testSongComposer();
//    void testSongAlbumTrack();
//    void testSongLink();
    void testSongNoInfo();

private:
    MpShareData *mTest;

};


#endif /* UNITTEST_MPSHAREDATA_H_ */
