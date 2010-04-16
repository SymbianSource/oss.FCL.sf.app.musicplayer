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
* Description: Unit test for mpmpxasframeworkwrapper
*
*/

#ifndef TESTMPMPXASFRAMEWORKWRAPPER_H
#define TESTMPMPXASFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpMpxAsFrameworkWrapper;
class MpMpxAsFrameworkWrapperPrivate;

class TestMpMpxAsFrameworkWrapper : public QObject
{
    Q_OBJECT

public:

    TestMpMpxAsFrameworkWrapper();
    virtual ~TestMpMpxAsFrameworkWrapper();

signals:


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testBalance();
    void testSetBalance();
    void testPersistBalance();
    
private:

    MpMpxAsFrameworkWrapper          *mTest;
    MpMpxAsFrameworkWrapperPrivate   *mTestPrivate;

};

#endif  // TESTMPMPXASFRAMEWORKWRAPPER_H
