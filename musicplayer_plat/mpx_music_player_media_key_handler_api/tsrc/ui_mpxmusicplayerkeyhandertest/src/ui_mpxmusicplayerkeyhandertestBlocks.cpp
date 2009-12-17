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
* Description:  CMPXMusicplayermediakeyhandertest test class for STIF Test Framework TestScripter.
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ui_mpxmusicplayerkeyhandertest.h"

#include <coeaui.h>
#include "EdwinTestControl.h"
#include <EIKENV.H>
// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMPXMusicplayermediakeyhandertest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CMPXMusicplayermediakeyhandertest::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        ENTRY( "MPXmusicplayermediahandlerInstance", CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance ),
        ENTRY( "EDWT_Delete", CMPXMusicplayermediakeyhandertest::EDWT_DeleteL ),
        ENTRY( "MPXmusicplayermediahandlerDestructor", CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerDestructor ),
        ENTRY( "MPXmusicplayermediahandlerShowPlaybackPopupL", CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerShowPlaybackPopupL ),
        ENTRY( "MPXmusicplayermediahandlerShowVolumePopupL", CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerShowVolumePopupL ),
        ENTRY( "MPXmusicplayermediahandlerDismissNotifier", CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerDismissNotifier ),
        ENTRY( "MPXmusicplayermediahandlerSetEnableMediaKeys", CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerSetEnableMediaKeys ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Kmpxmusicplayermediakeyhandertest, "mpxmusicplayermediakeyhandertest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Kmpxmusicplayermediakeyhandertest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kmpxmusicplayermediakeyhandertest, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance
// Create Musicplayermediahandler Instance Move operation function
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance( CStifItemParser& /* aItem*/ )
	{
    iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"));
    
    TInt err = KErrNone;
    
    iEdwinTestControl = CEdwinTestControl::NewL();
    CleanupStack::PushL( iEdwinTestControl );    	    
    CCoeEnv::Static()->AppUi()->AddToStackL( iEdwinTestControl );
    iEdwinTestControl->MakeVisible(ETrue);
    CleanupStack::Pop( iEdwinTestControl );
    
    if ( !iMPXMediaKeyHandler )
        {
        // Handle media key in appui
        iMPXMediaKeyHandler = MMPXMediaKeyHandler::NewL(
            EDisplayVolumePopup | EDisplayMediaPopup,
            this );
        }
	if(err != KErrNone)
	  {
	  iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"),err);
	  }
       return err;
	}
  
// -----------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::EDWT_DeleteL
// EDWT DeleteL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::EDWT_DeleteL( CStifItemParser& )
	{
    CCoeEnv::Static()->AppUi()->RemoveFromStack( iEdwinTestControl );
    
    delete iEdwinTestControl;
    iEdwinTestControl = NULL;

    return KErrNone;		
	}

// -----------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::MMPXMediaKeyHandlerDestructor
// MMPXMediaKeyHandler Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerDestructor( CStifItemParser& )
	{
	
	iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MMPXMediaKeyHandlerDestructor"));
	    
	TInt err = KErrNone;
	
    delete iMPXMediaKeyHandler;
    iMPXMediaKeyHandler = NULL;
    
    return err;		
	}

// ---------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerShowPlaybackPopupL
// MPXmusicplayermediahandler Show Playback PopupL function
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerShowPlaybackPopupL( CStifItemParser& /* aItem*/ )
	{
    iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"));
    
    TInt err = KErrNone;
    
   TRAP(err,iMPXMediaKeyHandler->ShowPlaybackPopupL(EMPXPopupTimeout));
    
	if(err != KErrNone)
	  {
	  iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"),err);
	  }
       return err;
	}

// ---------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerShowVolumePopupL
// MPXmusic playermediahandler Show Volume PopupL function
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerShowVolumePopupL( CStifItemParser& /* aItem*/ )
	{
    iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"));
    
    TInt err = KErrNone;
    
   TRAP(err,iMPXMediaKeyHandler->ShowVolumePopupL());
    
	if(err != KErrNone)
	  {
	  iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"),err);
	  }
       return err;
	}
// ---------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerSetEnableMediaKeys
// MPXmusic player media handler Set Enable MediaKeys function
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerSetEnableMediaKeys( CStifItemParser& /* aItem*/ )
	{
    iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"));
    
    TInt err = KErrNone;
    
    TRAP(err,iMPXMediaKeyHandler->SetEnableMediaKeys(EFalse));
    
	if(err != KErrNone)
	  {
	  iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"),err);
	  }
       return err;
	}
// ---------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerDismissNotifier
// MPXmusic player media handler Dismiss Notifier function
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerDismissNotifier( CStifItemParser& /* aItem*/ )
	{
    iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"));
    
    TInt err = KErrNone;
    
    TRAP(err,iMPXMediaKeyHandler->DismissNotifier(EDisplayMediaPopup | EDisplayVolumePopup));
    
	if(err != KErrNone)
	  {
	  iLog->Log(_L("CMPXMusicplayermediakeyhandertest::MPXmusicplayermediahandlerInstance"),err);
	  }
       return err;
	}

// -----------------------------------------------------------------------------
// CMPXMusicplayermediakeyhandertest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CMPXMusicplayermediakeyhandertest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
