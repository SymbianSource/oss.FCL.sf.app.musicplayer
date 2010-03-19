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
* Description: Unit test for MpCollectionInfoBar
*
*/

#include <qnamespace.h>
#include <QSignalSpy>
#include <QMetaType>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>
#include <hblabel.h>

#include "unittest_mpcollectioninfobar.h"
#include "mpcommondefs.h"

// Do this so we can access all member variables.
#define private public
#include "mpcollectioninfobar.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpCollectionInfoBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpcollectioninfobar.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpCollectionInfoBar::TestMpCollectionInfoBar()
    : mTest(0)
{
}

TestMpCollectionInfoBar::~TestMpCollectionInfoBar()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionInfoBar::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionInfoBar::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionInfoBar::init()
{
    mTest = new MpCollectionInfoBar();
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionInfoBar::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
 */
void TestMpCollectionInfoBar::testConstructor()
{
    QVERIFY(mTest->mContainer != 0);
    QVERIFY(mTest->mDocumentLoader != 0);
    QVERIFY(mTest->mTextLeft != 0);
    QVERIFY(mTest->mTextRight != 0);
    
    cleanup();
    QGraphicsRectItem rectItem;
    mTest = new MpCollectionInfoBar(&rectItem);
    QVERIFY(mTest->parentItem()  == &rectItem);
    mTest->setParentItem(0);
}

///*!
// Tests changing the text with one or two labels
// */
void TestMpCollectionInfoBar::testSetText()
{
    mTest->setText(QString("Left text"));
    QCOMPARE(mTest->mSingleText, true);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text"));
    mTest->setText(QString("Left text 2"),QString("Right text 2"));
    QCOMPARE(mTest->mSingleText, false);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text 2"));
    QCOMPARE(mTest->mTextRight->plainText(),QString("Right text 2"));
    mTest->setText(QString("Left text"));
    QCOMPARE(mTest->mSingleText, true);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text"));   
}

///*!
// Tests orientation changes.
// */
void TestMpCollectionInfoBar::testOrientationChange()
{
    connect(this, SIGNAL( orientationChange(Qt::Orientation) ),
                 mTest, SLOT( orientationChange(Qt::Orientation) ) );
    mTest->setText(QString("Left text"));
    QCOMPARE(mTest->mSingleText, true);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text"));
    emit orientationChange(Qt::Horizontal);
    QCOMPARE(mTest->mSingleText, true);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text"));
    QCOMPARE(mTest->mOrientation,Qt::Horizontal);
    
    emit orientationChange(Qt::Vertical);
    QCOMPARE(mTest->mSingleText, true);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text"));
    QCOMPARE(mTest->mOrientation,Qt::Vertical);
    
    mTest->setText(QString("Left text 2"),QString("Right text 2"));
    emit orientationChange(Qt::Horizontal);
    QCOMPARE(mTest->mSingleText, false);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text 2"));
    QCOMPARE(mTest->mTextRight->plainText(),QString("Right text 2"));
    QCOMPARE(mTest->mOrientation,Qt::Horizontal);
    
    emit orientationChange(Qt::Vertical);
    QCOMPARE(mTest->mSingleText, false);
    QCOMPARE(mTest->mTextLeft->plainText(),QString("Left text 2"));
    QCOMPARE(mTest->mTextRight->plainText(),QString("Right text 2"));
    QCOMPARE(mTest->mOrientation,Qt::Vertical);
}

// End of file
