/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CmpxMusicStoreTest test class for STIF Test Framework TestScripter.
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "mpxmusicstoretest.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CmpxMusicStoreTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CmpxMusicStoreTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CmpxMusicStoreTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CmpxMusicStoreTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CmpxMusicStoreTest::ExampleL ),
        ENTRY( "CMPXFindInMShopCreate", CmpxMusicStoreTest::CMPXFindInMShopCreate ),
        ENTRY( "CMPXFindInMShopDestructor", CmpxMusicStoreTest::CMPXFindInMShopDestructor ),
       

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CmpxMusicStoreTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CmpxMusicStoreTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KmpxMusicStoreTest, "mpxMusicStoreTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KmpxMusicStoreTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KmpxMusicStoreTest, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CmpxMusicStoreTest::CMPXFindInMShopCreate
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CmpxMusicStoreTest::CMPXFindInMShopCreate( CStifItemParser& /*aItem*/ )
	{
	 TInt err=KErrNone;
     iLog->Log(_L("CmpxMusicStoreTest::CMPXFindInMShopCreate"));
    
    // iMPXFindInMShop=CMPXFindInMShop::NewL();
    
	 iLog->Log(_L("CmpxMusicStoreTest::NewL returned: %d"), err);
     return err;
	}

// -----------------------------------------------------------------------------
// CmpxMusicStoreTest::CMPXFindInMShopDestructor
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CmpxMusicStoreTest::CMPXFindInMShopDestructor( CStifItemParser& /*aItem*/ )
	{
	 TInt err=KErrNone;
     iLog->Log(_L("CmpxMusicStoreTest::CMPXFindInMShopDestructor"));
    
    /* if(iMPXFindInMShop!=NULL)
       {
	    delete iMPXFindInMShop;
	    iMPXFindInMShop=NULL;
       }*/
	 iLog->Log(_L("CmpxMusicStoreTest::NewL returned: %d"), err);
	// REComSession::FinalClose();
     return err;
	}



// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
