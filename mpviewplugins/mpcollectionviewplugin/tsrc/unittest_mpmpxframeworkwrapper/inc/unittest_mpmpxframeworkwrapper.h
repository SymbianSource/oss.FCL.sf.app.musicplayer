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
* Description: Unit test for mpmpxframeworkwrapper
*
*/

#ifndef TESTMPMPXFRAMEWORKWRAPPER_H
#define TESTMPMPXFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpMpxFrameworkWrapper;
class MpMpxFrameworkWrapperPrivate;
class CMPXMedia;

class TestMpMpxFrameworkWrapper : public QObject
{
    Q_OBJECT

public:

    TestMpMpxFrameworkWrapper();
    virtual ~TestMpMpxFrameworkWrapper();
    //test utility functions
    void loadTestData();
    void loadPlaylists();
    bool waitForSignal(QSignalSpy* spy, int timeoutms);  

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testConstructor();
    void testCollectionData();
    void testHandleOpenEntries();
    void testHandleOpenPlaylist();    
    void testHandleOperationComplete();  
    void testOpenCollection();  
    void testDoIncrementalOpen();
    void testReopenCollection();
    void testOpenCollectionItem();  
    void testGoBack();
    void testFindPlaylists();
    void testCreatePlaylist();        
    void testSaveToPlaylist();
    void testRenamePlaylist();
    void testDeleteSongs();
    void testSetShuffle();
    void testScan();
    void testCancelScan();
    void testPreviewItem();
    void testHandleCollectionMessage();
    void testPreparePlaylistMediaSongsContext();
    void testPreparePlaylistMediaArtistAlbumsContext();

private:

    MpMpxFrameworkWrapper           *mTest;
    MpMpxFrameworkWrapperPrivate    *mTestPrivate;
    CMPXMedia                       *iMediaTestData;
    CMPXMedia                       *iPlaylistsTestData;

};

#endif  // TESTMPMPXFRAMEWORKWRAPPER_H




