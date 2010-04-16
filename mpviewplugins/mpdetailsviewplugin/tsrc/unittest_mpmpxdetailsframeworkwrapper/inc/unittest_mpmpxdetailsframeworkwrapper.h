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
* Description: Unit test for mpmpxdetailsframeworkwrapper
*
*/

#ifndef TESTMPMPXDETAILSFRAMEWORKWRAPPER_H
#define TESTMPMPXDETAILSFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpMpxDetailsFrameworkWrapper;
class MpMpxDetailsFrameworkWrapperPrivate;
class CMPXMedia;
class MpSongData;

class TestMpMpxDetailsFrameworkWrapper : public QObject
{
    Q_OBJECT

public:

    TestMpMpxDetailsFrameworkWrapper();
    virtual ~TestMpMpxDetailsFrameworkWrapper();
    
private:     //test utility functions
    void loadTestData(TInt aPos);
    void verifyEmptyTestData();
    void verify(int index);
    void handleSongDetailInfoChanged();
    void doTestHandleMedia(int index, TInt aResult);
    
private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots: // test functions
    void testConstructor();
    void testSongData();
    void testHandlePlaybackMessage();
    void testHandleMedia();        
    void testRetrieveSong();
    void testHandleProperty();
    void testHandleSubPlayerNames();

private:

    MpMpxDetailsFrameworkWrapper           *mTest;
    MpMpxDetailsFrameworkWrapperPrivate    *mTestPrivate;
    CMPXMedia                              *iMediaTestData;
    bool                                   mSongDetailInfoChanged;
    TInt mAllocCells;

};

#endif  // TESTMPMPXDETAILSFRAMEWORKWRAPPER_H




