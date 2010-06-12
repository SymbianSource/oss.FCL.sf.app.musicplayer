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
* Description: Unit test for mpmpxplaybackframeworkwrapper
*
*/

#ifndef TESTMPMPXPLAYBACKFRAMEWORKWRAPPER_H
#define TESTMPMPXPLAYBACKFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpMpxPlaybackFrameworkWrapper;
class MpMpxPlaybackFrameworkWrapperPrivate;
class CMPXMedia;
class MpPlaybackData;

class TestMpMpxPlaybackFrameworkWrapper : public QObject
{
    Q_OBJECT

public:

    TestMpMpxPlaybackFrameworkWrapper();
    virtual ~TestMpMpxPlaybackFrameworkWrapper();
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
    void testSetBalance();
    void testApplyAudioEffects();
    void testApplyEqualizer();
    void testPlay();
    void testStartSeekForward();
    void testStopSeeking();
    void testStartSeekBackward();
    void testHandleSubPlayerNamesL();

private:

    MpMpxPlaybackFrameworkWrapper           *mTest;
    MpMpxPlaybackFrameworkWrapperPrivate    *mTestPrivate;
    CMPXMedia                               *iMediaTestData;

};

#endif  // TESTMPMPXPLAYBACKFRAMEWORKWRAPPER_H



