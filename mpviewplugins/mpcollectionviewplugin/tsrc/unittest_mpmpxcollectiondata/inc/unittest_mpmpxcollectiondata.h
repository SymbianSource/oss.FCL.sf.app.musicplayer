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
* Description: Unit test for MpMpxCollectionData
*
*/

#ifndef TESTMPMPXCOLLECTIONDATA_H
#define TESTMPMPXCOLLECTIONDATA_H

#include <QtTest/QtTest>

class MpMpxCollectionData;
class MpMpxCollectionDataPrivate;

class TestMpMpxCollectionData : public QObject
{
    Q_OBJECT

public:

    TestMpMpxCollectionData();
    virtual ~TestMpMpxCollectionData();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testContextRetrieval();
    void testCount();
    void testCollectionTitle();
    void testItemData();
    void testIsAutoPlaylist();
    void testIsAutoPlaylistWithIndex();
    void TestItemCountWithIndex();
    void testSetCollectionContextL();
    void testSetMpxMediaAllSongs();
    
    /*
    TODO: test all the permutations for context on SetCollectionContextL
    */
private:



    MpMpxCollectionData         *mTest;
    MpMpxCollectionDataPrivate  *mTestPrivate;

};

#endif  // TESTMPMPXCOLLECTIONDATA_H




