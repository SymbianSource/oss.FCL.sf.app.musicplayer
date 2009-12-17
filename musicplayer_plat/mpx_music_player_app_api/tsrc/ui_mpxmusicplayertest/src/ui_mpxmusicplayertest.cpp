/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Cmpxmusicplayertest test class for STIF Test Framework TestScripter.
*
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include <SettingServerClient.h>
#include "ui_mpxmusicplayertest.h"

#include <COEAUI.H>
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::Cmpxmusicplayertest
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
Cmpxmusicplayertest::Cmpxmusicplayertest(
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void Cmpxmusicplayertest::ConstructL()
    {
    iLog = CStifLogger::NewL( KmpxmusicplayertestLogPath,
                          KmpxmusicplayertestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

	// Print title of the test case
    TName title;
    TestModuleIf().GetTestCaseTitleL(title);
    iLog->Log(_L(" "));
    iLog->Log(_L("[Title] %S"), &title);
    
    //=================================added UI controler
    iEdwinTestControl = CEdwinTestControl::NewL();
//    CleanupStack::PushL( iEdwinTestControl );
    CCoeEnv::Static()->AppUi()->AddToStackL( iEdwinTestControl );
    iEdwinTestControl->MakeVisible(ETrue);
//    CleanupStack::Pop( iEdwinTestControl );
    //=================================added UI controler
    }

// ---------------------------------------------------------------------------
// Cmpxmusicplayertest::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
Cmpxmusicplayertest* Cmpxmusicplayertest::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    Cmpxmusicplayertest* self = new (ELeave) Cmpxmusicplayertest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
// function from MMPXCHelperObserver
void Cmpxmusicplayertest::DialogDismissedL( TInt aButtonId )
    {
    iLog->Log(_L("MProgressDialogCallback::DialogDismissedL is called,with a TInt aButtonId %d pressed."),aButtonId);
    switch(aButtonId)
    	{
    	case EAknSoftkeyOk:
    	case EAknSoftkeySelect:
    	case EAknSoftkeyYes:
    		if(iEdwinTestControl)
    			{
    			iEdwinTestControl->DrawDeferred();
    			}
    		break;
    	case EAknSoftkeyCancel:
    		//TInt err = UiHelperDismissWaitNoteL();
    		TInt err = KErrNone;  
		    TRAP(err,iUiHelper->DismissWaitNoteL());
    		iLog->Log(_L("EAknSoftkeyCancel is pressed. %d returned."),err);
    		break;		
    	default:
    		iLog->Log(_L("Nothing is done."));
    		break;	
    	}
    }

// function from MEikCommandObserver
void Cmpxmusicplayertest::ProcessCommandL(TInt aCommandId)
    {
    iLog->Log(_L("MEikCommandObserver::ProcessCommandL is called,with a TInt aCommandId %d"),aCommandId);
    }

// function from MMPXCHelperObserver
void Cmpxmusicplayertest::HandleOperationCompleteL( TCHelperOperation aOperation,
        											TInt aErr,
        											void* /*aArgument*/ )
    {
    switch( aOperation )
    	{
    	case EAddOp:
    		iLog->Log(_L("A playlist is created with an error %d returned."),		aErr);
    		break;
    	case EDeleteOp:	
    		iLog->Log(_L("A playlist is deleted with an error %d returned."),		aErr);
    		break;
    	case ESetOp:	
    	    iLog->Log(_L("A playlist is set with an error %d returned."),			aErr);
    	    break;
    	case EEmbeddedOpenOp:	
    	    iLog->Log(_L("A playlist is embeded open with an error %d returned."),	aErr);
    	    break;
    	case EMoveOp:	
    	    iLog->Log(_L("A playlist is moved with an error %d returned."),			aErr);
    	    break;   
    	case EExportPlaylistOp:	
    	    iLog->Log(_L("A playlist is exported with an error %d returned."),		aErr);
    	    break;  
    	case ERenameOp:	
    	    iLog->Log(_L("A playlist is renamed with an error %d returned."),		aErr);
    	    break;
    	case EReorderPlaylistOp:	
    	    iLog->Log(_L("A playlist is reordered with an error %d returned."),		aErr);
    	    break;
    	default:
    		iLog->Log(_L("Nothing is done."));
    		break;
    	}     
    }


// Destructor
Cmpxmusicplayertest::~Cmpxmusicplayertest()
    {
    // Delete resources allocated from test methods
    Delete();
    // Delete logger
    delete iLog;
    
    //added 28,10,2008
    if(iEdwinTestControl)
    	{
    	CCoeEnv::Static()->AppUi()->RemoveFromStack( iEdwinTestControl );
    	delete iEdwinTestControl;
    	}
   
    }

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// ---------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    return ( CScriptBase* ) Cmpxmusicplayertest::NewL( aTestModuleIf );
    }
