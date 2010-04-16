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
* Description: Unit test for mpcollectiondocumentloader
*
*/

#include <qnamespace.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>

#include "unittest_mpcollectiondocumentloader.h"
#include "mpnowplayingwidget.h"

// Do this so we can access all member variables.
#define private public
#include "mpcollectiondocumentloader.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestMpCollectionDocumentLoader tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpcollectiondocumentloader.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestMpCollectionDocumentLoader::TestMpCollectionDocumentLoader()
    : mTest(0)
{
}

TestMpCollectionDocumentLoader::~TestMpCollectionDocumentLoader()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpCollectionDocumentLoader::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpCollectionDocumentLoader::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpCollectionDocumentLoader::init()
{
    mTest = new MpCollectionDocumentLoader();
}

/*!
 Called after every testfunction.
 */
void TestMpCollectionDocumentLoader::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests the correct creation of MpNowPlayingWidget.
 */
void TestMpCollectionDocumentLoader::testCreateMpNowPlayingWidget()
{
    cleanup();
    init();
    QObject *theObject;
    theObject = mTest->createObject(QString("MpNowPlayingWidget"),QString("myMpNowPlayingWidget"));
    QCOMPARE(theObject->metaObject()->className(),"MpNowPlayingWidget");
    QCOMPARE(theObject->objectName(),QString("myMpNowPlayingWidget"));
    QVERIFY(qobject_cast<MpNowPlayingWidget*>(theObject));
    delete theObject;

}

/*!
 Tests the correct creation of QObject, this should be pased to the
 base clas and base clas should return a named object.
 */
void TestMpCollectionDocumentLoader::testCreateQObject()
{
    cleanup();
    init();
    QObject *theObject;
    theObject = mTest->createObject(QString("QObject"),QString("myQObject"));
    QCOMPARE(theObject->metaObject()->className(),"QObject");
    QCOMPARE(theObject->objectName(),QString("myQObject"));
    delete theObject;
}

// End of file
