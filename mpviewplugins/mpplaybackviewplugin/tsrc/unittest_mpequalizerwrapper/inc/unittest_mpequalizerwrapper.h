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
* Description: Unit test for mpequalizerwrapper
*
*/

#ifndef TESTMPEQUALIZERWRAPPER_H
#define TESTMPEQUALIZERWRAPPER_H

#include <QtTest/QtTest>

class MpEqualizerWrapper;
class MpEqualizerWrapperPrivate;

class TestMpEqualizerWrapper: public QObject
{
    Q_OBJECT

public:
    TestMpEqualizerWrapper();
    virtual ~TestMpEqualizerWrapper();

signals:

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testMapcInitComplete();
    void testGetPresetId();
    void testGetEngineIndexL();
    void testApplyPreset();
    void testActivePreset();
    void testDisableEqualizer();
    void testPresetNames();
    
private:

    MpEqualizerWrapper        *mTest;
    MpEqualizerWrapperPrivate *mTestPrivate;
    
    QObject           *qObject;

};

#endif  // TESTMPEQUALIZERWRAPPER_H




