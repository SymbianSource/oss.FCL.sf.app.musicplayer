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
* Description: Unit test for MpDetailsShareDialog
*
*/
#include <QSignalSpy>
#ifdef Q_OS_SYMBIAN
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbicon.h>
#endif
#include"unittest_mpdetailssharedialog.h"

// Do this so we can access all member variables.
#define private public
#include "mpdetailssharedialog.h"
#undef private
#include "mpsharedata.h"
#include "mpsongdata.h"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
#ifdef Q_OS_SYMBIAN
int main(int argc, char *argv[])
{

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpDetailsShareDialog tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_testmpdetailssharedialog.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}
#endif

#ifdef Q_OS_WIN32
QTEST_MAIN(TestMpDetailsShareDialog)
#endif
//Constructor
TestMpDetailsShareDialog::TestMpDetailsShareDialog()
    : mTest(0)
{

}

//Destructor
TestMpDetailsShareDialog::~TestMpDetailsShareDialog()
{
    if (mTest) delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpDetailsShareDialog::initTestCase()
{
    qDebug() << "unit test for mpdetailssharedialog begin.";
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpDetailsShareDialog::cleanupTestCase()
{
    qDebug() << "unit test for mpdetailssharedialog end.";
}

/*!
 Called before each testfunction is executed.
 */
void TestMpDetailsShareDialog::init()
{
    mSongData = new MpSongData();
    mTest = new MpDetailsShareDialog();
    mTest->initialize(mSongData);
}

/*!
 Called after every testfunction.
 */
void TestMpDetailsShareDialog::cleanup()
{
    delete mTest;
    delete mSongData;
    mTest = 0;
    mSongData = 0;
}

void TestMpDetailsShareDialog::testConstruction()
{
    mTest->updateSharedData();
    //nothing to verify
    QCOMPARE(true, true);
}

void TestMpDetailsShareDialog::testIndexLoadOk()
{
    mTest->onIndexLoad(true);
    mTest->onIndexLoad(false);
    //nothing to verify
    QCOMPARE(true, true);
}

