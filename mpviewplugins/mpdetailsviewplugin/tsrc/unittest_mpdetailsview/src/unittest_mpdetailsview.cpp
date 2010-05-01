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
* Description: Unit Test for mpdetailsview.
*
*/
#include <QSignalSpy>
#include <qgraphicswebview>
#include <qwebsettings>
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>
#include <QUrl>
#include <QSslError>
#include <QDomElement>
#include <QWebPage>
#include <QWebFrame>
#include <QList>
#include <QFile>
#include <QTranslator>
#include <QLocale>

#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hbpopup.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <HbApplication>

#include "unittest_mpdetailsview.h"
#include "stub/inc/mpsongdata.h"
#include "stub/inc/thumbnailmanager_qt.h"
#include "stub/inc/mpmpxdetailsframeworkwrapper.h"
#include "mpcommondefs.h"

// Do this so we can access all member variables.
#define private public
#include "mpdetailsview.h"
#undef private



/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpDetailsView tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_testmpdetailsview.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

//Constructor
TestMpDetailsView::TestMpDetailsView()
    : mTest(0)
{

}

//Destructor
TestMpDetailsView::~TestMpDetailsView()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpDetailsView::initTestCase()
{
    
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpDetailsView::cleanupTestCase()
{

}

/*!
 Called before each testfunction is executed.
 */
void TestMpDetailsView::init()
{
    mTest = new MpDetailsView();
    mTest->initializeView();
}

/*!
 Called after every testfunction.
 */
void TestMpDetailsView::cleanup()
{
    delete mTest;
    mTest = 0;
}

void TestMpDetailsView::testactivateView()
    {
    mTest->activateView();
    QVERIFY(mTest->mActivated==true);
    }

void TestMpDetailsView::testdeactivateView()
    {
    /* TODO: Fix   
    mTest->deactivateView();
    QVERIFY(mTest->mActivated==false);
    */
     }

void TestMpDetailsView::testbackSlot() {
    connect(this, SIGNAL(back()), mTest->mNavigationBack, SIGNAL(triggered()));
    QSignalSpy spy(mTest, SIGNAL(command(int)));
    QVERIFY(spy.isValid());
    QCOMPARE(spy.count(),0);
    
    emit back();
    QCOMPARE(spy.count(),1);
    }


