/**
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
* Description: Unit test for mpmediakeyhandler
*
*/

#ifndef TESTMPSONGSCANNER_H
#define TESTMPSONGSCANNER_H

#include <QtTest/QtTest>

class MpSongScanner;
class MpSongScannerPrivate;

class TestMpSongScanner : public QObject
{
    Q_OBJECT

public:
    
    TestMpSongScanner();
    virtual ~TestMpSongScanner();

// from QtTest
public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

// internal
private slots:

    void testConstructor();
    void testDestructor();
    void testConstructorFail();
    void testIsScanning();
    void testScan();
    void testCancelScan();
    void testScanFail();
    void testCancelScanFail();
    void testHandleOpenLMedia();
    void testHandleOpenLPlaylist();
    void testHandleCollectionMessageRefresh();
    void testHandleCollectionMessageDiskInserted();
    void testHandleCollectionMessageItemChanged();
    void testHandleCollectionMediaL();

private:
    
    MpSongScanner           *mTest;
    MpSongScannerPrivate    *mTestPrivate;

};

#endif  // TESTMPSONGSCANNER_H




