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
* Description: Unit test for mpmpxpbframeworkwrapper
*
*/

#ifndef TESTMPMPXPBFRAMEWORKWRAPPER_H
#define TESTMPMPXPBFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpMpxPbFrameworkWrapper;
class MpMpxPbFrameworkWrapperPrivate;
class CMPXMedia;
class MpPlaybackData;

class TestMpMpxPbFrameworkWrapper : public QObject
{
    Q_OBJECT

public:

    TestMpMpxPbFrameworkWrapper();
    virtual ~TestMpMpxPbFrameworkWrapper();
    //test utility functions
    void loadTestData(TInt aPos);

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void testConstructor();
    void testPlaybackData();
    void testStop();
    void testPlayPause();    
    void testSkipForward();  
    void testSkipBackward();  
    void testSetPosition();
    void testSetShuffle();
    void testSetRepeat();
    void testHandlePlaybackMessage();
    void testHandleProperty();
    void testHandleMedia();        
    void testRetrieveSongDetails();
    void testUpdateState();
    void testForceStop();

private:

    MpMpxPbFrameworkWrapper           *mTest;
    MpMpxPbFrameworkWrapperPrivate    *mTestPrivate;
    CMPXMedia                         *iMediaTestData;

};

#endif  // TESTMPMPXPBFRAMEWORKWRAPPER_H




