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
* Description: Unit test for CMpMpxIsolatedCollectionHelper
*
*/

#include <hbapplication.h>
#include <hbmainwindow.h>

#include <mpxmedia.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>

#include "unittest_mpmpxisolatedcollectionhelper.h"


//This so we can test private functions
#include "../../src/mpmpxisolatedcollectionhelper.cpp"


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestCMpMpxIsolatedCollectionHelper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\unittest_mpmpxisolatedcollectionhelper.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestCMpMpxIsolatedCollectionHelper::TestCMpMpxIsolatedCollectionHelper()
    : mTest( 0 ),
      iEntriesFromoOpenCallback ( 0 ),
      iErrorFromoOpenCallback( KErrNone ),
      iOpenCallbackCounter( 0 )
{
}

TestCMpMpxIsolatedCollectionHelper::~TestCMpMpxIsolatedCollectionHelper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestCMpMpxIsolatedCollectionHelper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestCMpMpxIsolatedCollectionHelper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestCMpMpxIsolatedCollectionHelper::init()
{
    mTest = CMpMpxIsolatedCollectionHelper::NewL( this );
}

/*!
 Called after every testfunction.
 */
void TestCMpMpxIsolatedCollectionHelper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
*/
void TestCMpMpxIsolatedCollectionHelper::testConstructor()
{
    QVERIFY(mTest->iIncrementalOpenUtil != 0);
    QCOMPARE(mTest->iIncrementalOpenUtil->iMode , KMcModeIsolated);
    QCOMPARE(mTest->iObserver , dynamic_cast<MMpMpxIsolatedCollectionHelperObserver*>(this));
}

/*!
 Tests OpenCollection.
 */
void TestCMpMpxIsolatedCollectionHelper::testOpenCollection()
{
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    mTest->OpenCollectionL(*path);
    QCOMPARE(mTest->iIncrementalOpenUtil->iDelay, KIncrementalDelayHalfSecond);
    QCOMPARE(mTest->iIncrementalOpenUtil->iStop,TBool(ETrue));
    QCOMPARE(mTest->iIncrementalOpenUtil->iStart,TBool(ETrue));
    QCOMPARE(mTest->iFirstIncrementalOpen,TBool(ETrue));
    QCOMPARE(mTest->iIncrementalOpenUtil->iDirection,CMPXCollectionOpenUtility::EFetchNormal);
    QCOMPARE(mTest->iIncrementalOpenUtil->iChunkSize, KIncrementalFetchBlockSize);
    QCOMPARE((int)mTest->iIncrementalOpenUtil->iPath, (int)path);
    delete path;
}

/*!
 Tests HandleOpen.
 */
void TestCMpMpxIsolatedCollectionHelper::testHandleOpen()
{
     iEntriesFromoOpenCallback = 0;
     iErrorFromoOpenCallback = KErrArgument;
     iOpenCallbackCounter = 0;
     CMPXMedia *media = CMPXMedia::NewL();
     mTest->iFirstIncrementalOpen = ETrue;
     mTest->HandleOpenL(*media, 0, false, KErrNone);
     QCOMPARE((int)iEntriesFromoOpenCallback,(int)media);
     QCOMPARE(iErrorFromoOpenCallback,KErrNone);
     QCOMPARE(iOpenCallbackCounter,1);
}
 
/*!
 Used to keep track of the callback from teh isolated collection helper.
 */
void TestCMpMpxIsolatedCollectionHelper::HandleIsolatedOpenL( const CMPXMedia& aEntries, TInt aError )
{
     iEntriesFromoOpenCallback = &aEntries;
     iErrorFromoOpenCallback = aError;
     iOpenCallbackCounter++;
}

//end of file
