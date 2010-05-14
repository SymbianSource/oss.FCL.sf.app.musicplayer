/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Unit test for mpdetailssharedialog
*
*/

#ifndef UNITTEST_MPDETAILSSHAREDIALOG_H_
#define UNITTEST_MPDETAILSSHAREDIALOG_H_

#include <QtTest/QtTest>

class MpDetailsShareDialog;
class MpSongData;

class TestMpDetailsShareDialog : public QObject
{
    Q_OBJECT

public:

    TestMpDetailsShareDialog();
    virtual ~TestMpDetailsShareDialog();

signals:

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testIndexLoadOk();
    void testConstruction();

private:
    MpDetailsShareDialog *mTest;
    MpSongData* mSongData;

};


#endif /* UNITTEST_MPDETAILSSHAREDIALOG_H_ */
