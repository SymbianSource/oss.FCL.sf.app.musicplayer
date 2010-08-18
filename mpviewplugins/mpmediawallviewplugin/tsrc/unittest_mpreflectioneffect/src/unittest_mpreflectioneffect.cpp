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
* Description: Unit test for MpReflectionEffect
*
*/

#include <hbinstance.h>
#include "unittest_mpreflectioneffect.h"

/*!
 * Nothing valuable to test for following functions
 * boundingRectFor() 
 * draw()
 * 
 */

// Do this so we can access all member variables.
#define private public
#include "mpreflectioneffect.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpReflectionEffect tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpreflectioneffect.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpReflectionEffect::TestMpReflectionEffect()
    : mTest(0)
{
}

TestMpReflectionEffect::~TestMpReflectionEffect()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpReflectionEffect::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpReflectionEffect::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpReflectionEffect::init()
{
    mTest = new MpReflectionEffect( new QObject() ); 
}

/*!
 Called after every testfunction.
 */
void TestMpReflectionEffect::cleanup()
{
    delete mTest;
    mTest = 0;
}


// End of file
