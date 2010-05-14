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
* Description: Unit test for mpnowplayingwidget
*
*/

#ifndef TESTNOWPLAYINGBACKEND_H
#define TESTNOWPLAYINGBACKEND_H

#include <QtTest/QtTest>

#include "stub/inc/mpnowplayingbackend.h"

class MpNowPlayingWidget;
class MpNowPlayingWidgetPrivate;

class TestMpNowPlayingWidget : public QObject
{
    Q_OBJECT

public:

    TestMpNowPlayingWidget();
    virtual ~TestMpNowPlayingWidget();

signals:
    void setState(SimplifiedPlayerState state);

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testSetState();
    void testTitleChange();
    void testArtistChange();
    void testSetEnabled();

private:

    MpNowPlayingWidget          *mTest;
    MpNowPlayingWidgetPrivate   *mTestPrivate;

};

#endif  // TESTNOWPLAYINGBACKEND_H




