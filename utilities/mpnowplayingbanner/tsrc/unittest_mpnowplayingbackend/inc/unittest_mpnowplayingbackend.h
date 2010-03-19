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
* Description: Unit test for mpnowplayingbackend
*
*/

#ifndef TESTNOWPLAYINGBACKEND_H
#define TESTNOWPLAYINGBACKEND_H

#include <QtTest/QtTest>

class MpNowPlayingBackEnd;
class MpNowPlayingBackEndPrivate;

class TestMpNowPlayingBackEnd : public QObject
{
    Q_OBJECT

public:

    TestMpNowPlayingBackEnd();
    virtual ~TestMpNowPlayingBackEnd();

signals:
    void update();
    void playPause();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testInitUtilCreateFail();
    void testInitAddObserverFail();
    void testDestructor();
    void testDestructorRemoveObserverFail();
    void testUpdateSuccess();
    void testUpdateNoSource();
    void testUpdateStateFail();
    void testUpdateMediaFail();
    void testPlayPause();
    void testPlayPauseFail();
    void testHandlePlaybackMessageStateChanged();
    void testHandlePlaybackMessageStateChangedFail();
    void testHandlePlaybackMessagePlaylistUpdated();
    void testHandlePlaybackMessageMediaChanged();
    void testHandlePlaybackMessagePlayerChanged();
    void testHandlePlaybackMessagePropertyChanged();
    void testHandleMediaLTitle();
    void testHandleMediaLTitleUri();
    void testHandleMediaLUri();
    void testHandleMediaLArtist();

private:

    MpNowPlayingBackEnd         *mTest;
    MpNowPlayingBackEndPrivate  *mTestPrivate;

};

#endif  // TESTNOWPLAYINGBACKEND_H




