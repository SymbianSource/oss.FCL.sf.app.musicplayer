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
* Description:  CMPXMusicPlayerCommonuiStifTest test class for STIF Test Framework TestScripter.
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ui_mpxmusicplayercommonuitest.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMPXMusicPlayerCommonuiStifTest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CMPXMusicPlayerCommonuiStifTest ::ExampleL ),
        //test functions for mpxalbumartutil.h
        ENTRY( "CMPXAlbumArtUtilCreate", CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilCreate ),
        ENTRY( "CMPXAlbumArtUtilDestructor", CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilDestructor ),
        ENTRY( "CMPXAlbumArtUtilExtractAlbumArtL", CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilExtractAlbumArtL ),
        ENTRY( "CMPXAlbumArtUtilCancelRequest", CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilCancelRequest ),
        //test functions for mpxlbxextendedfeatures.h
        ENTRY( "CMPXLbxExtendedFeaturesCreate", CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesCreate ),
        ENTRY( "CMPXLbxExtendedFeaturesDestructor", CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesDestructor ),
        ENTRY( "CMPXLbxExtendedFeaturesHandleLbxKeyEventL", CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesHandleLbxKeyEventL ),
        ENTRY( "CMPXLbxExtendedFeaturesEnableSpeedScrollL", CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesEnableSpeedScrollL ),
        ENTRY( "CMPXLbxExtendedFeaturesSpeedNaviUpdating", CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesSpeedNaviUpdating ),

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::ExampleL( CStifItemParser& aItem )
    {

	    // Print to UI
	    _LIT( Kui_mpxmusicplayercommonuitest, "ui_mpxmusicplayercommonuitest" );
	    _LIT( KExample, "In Example" );
	    TestModuleIf().Printf( 0, Kui_mpxmusicplayercommonuitest, KExample );
	    // Print to log file
	    iLog->Log( KExample );
	
	    TInt i = 0;
	    TPtrC string;
	    _LIT( KParam, "Param[%i]: %S" );
	    while ( aItem.GetNextString ( string ) == KErrNone )
	        {
	          TestModuleIf().Printf( i, Kui_mpxmusicplayercommonuitest, 
	                                KParam, i, &string );
	          i++;
	        }
	
	    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilCreate
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilCreate( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
   
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilCreate"));
    
    TRAP( err , iMPXAlbumArtUtil = CMPXAlbumArtUtil::NewL() );
    
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilCreate returned: %d"), err);
    return err;
    }


// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilDestructor
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilDestructor( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilDestructor begin:"));
    
    if( iMPXAlbumArtUtil != NULL )
    	{
		 delete iMPXAlbumArtUtil;
		 iMPXAlbumArtUtil = NULL;
    	}
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilDestructor end."));
    return err;
    }

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilExtractAlbumArtL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilExtractAlbumArtL( CStifItemParser& aItem )
    {
    TInt err=KErrNone;
    TPtrC string;
   // CFbsBitmap* bitmap(NULL);
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilExtractAlbumArtL"));
    
    while ( aItem.GetNextString( string ) == KErrNone )
           {
	    	TBuf<120> uri;
	    	uri.Append(Kui_mpxmusicplayercommonuitestDataPath);
	    	uri.Append(string);
	    	while ( aItem.GetNextString( string ) == KErrNone )
	    		 {
		    	  TBuf<120> KFileName;
		    	  KFileName.Append(Kui_mpxmusicplayercommonuitestDataPath);
		    	  KFileName.Append(string);
				  CMPXMedia* media = CMPXMedia::NewL();
				  CleanupStack::PushL(media);
				  media->SetTObjectValueL(KMPXMediaMusicAlbumArtFileName, KFileName);
				  media->SetTObjectValueL(KMPXMediaGeneralUri,uri);
				  TRAP(err,iMPXAlbumArtUtil->ExtractAlbumArtL(*media,*this,TSize(1,1)));
				  CleanupStack::PopAndDestroy(media);
			      iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilExtractAlbumArtL returned: %d"), err);
			    }
           }
    
    return err;
    }



// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::CMPXAlbumArtUtilCancelRequest
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilCancelRequest( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXAlbumArtUtilCancelRequest begin:"));
    
    iMPXAlbumArtUtil->CancelRequest();
    
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CancelRequest end."));
	
    return err;
    }

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesCreate
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesCreate( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesCreate"));
    
    CEikTextListBox* listBox = new (ELeave) CEikTextListBox;
    TRAP( err,iMPXLbxExtendedFeatures = CMPXLbxExtendedFeatures::NewL(listBox) );
    
    if ( listBox != NULL )
    	{
		 delete listBox;
		 listBox = NULL;
    	}
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::NewL returned: %d"), err);
    return err;
    }

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesDestructor
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesDestructor( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesDestructor begin:"));
    
    if( iMPXLbxExtendedFeatures != NULL )
    	{
		 delete iMPXLbxExtendedFeatures;
		 iMPXLbxExtendedFeatures = NULL;
    	}
    
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::Destructor end."));
    return err;
    }

// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesHandleLbxKeyEventL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesHandleLbxKeyEventL( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
    TKeyEvent keyEvent;
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesHandleLbxKeyEventL"));
    
    iMPXLbxExtendedFeatures->EnableSpeedScrollL(ETrue);
    TRAP( err , iMPXLbxExtendedFeatures->HandleLbxKeyEventL(keyEvent,EEventNull) );
    
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest ::HandleLbxKeyEventL returned: %d"), err);
    return err;
    }


// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesEnableSpeedScrollL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesEnableSpeedScrollL( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesEnableSpeedScrollL"));
    
    TRAP( err,iMPXLbxExtendedFeatures->EnableSpeedScrollL(ETrue) );
    
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::EnableSpeedScrollL returned: %d"), err);
    return err;
    }


// -----------------------------------------------------------------------------
// CMPXMusicPlayerCommonuiStifTest ::CMPXLbxExtendedFeaturesSpeedNaviUpdating
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesSpeedNaviUpdating( CStifItemParser& /*aItem*/ )
    {
    TInt err=KErrNone;
    iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::CMPXLbxExtendedFeaturesSpeedNaviUpdating begin:"));
    
    iMPXLbxExtendedFeatures->SpeedNaviUpdating(ETrue);
    
	iLog->Log(_L("CMPXMusicPlayerCommonuiStifTest::SpeedNaviUpdating end."));
    return err;
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
