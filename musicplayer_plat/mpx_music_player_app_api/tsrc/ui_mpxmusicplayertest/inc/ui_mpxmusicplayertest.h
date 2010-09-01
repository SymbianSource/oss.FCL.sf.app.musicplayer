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


#ifndef UI_MPXMUSICPLAYERTEST_H
#define UI_MPXMUSICPLAYERTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <mpxtlshelper.h>
#include "mpxcommonuihelper.h"
#include <mpxcollectionutility.h>
#include "edwintestcontrol.h"
#include <mpxcollectionuihelperobserver.h>

#include <AknProgressDialog.h>
#include <EIKCMOBS.H>
// CONSTANTS
const TInt KErrBadTestParameter = -1000;         // Error on configuration file
const TUid KHostUid = { 0x101FB3E3 };
const TInt KMusicPlayerMSKControlID = 3;
// MACROS
// Logging path
#ifdef __WINSCW__
_LIT( KmpxmusicplayertestLogPath, "\\testing\\log\\" );
_LIT( KRingToneFileName, "c:\\testing\\data\\mpxtest1.mp3" );
_LIT( KRingToneFilePath, "c:\\testing\\data\\" );
#else
_LIT( KmpxmusicplayertestLogPath, "e:\\testing\\log\\" );
_LIT( KRingToneFileName, "e:\\testing\\data\\mpxtest1.mp3" );
_LIT( KRingToneFilePath, "e:\\testing\\data\\" );
#endif
// Log file
_LIT( KmpxmusicplayertestLogFile, "mpxmusicplayertest.txt" );

_LIT( KTagKErrNone, "KErrNone" );
_LIT( KTagKErrNotReady, "KErrNotReady" );
_LIT( KMPXIliegalCharacters, "IliegalCharacters" );
_LIT( KMPXFileNotFound, "FileNotFound" );
_LIT( KMPXSaveToCollectionNote, "Saved_to_collection_successfully!" );
_LIT( KMPXSaveNote, "Saved_successfully!" );

// FORWARD DECLARATIONS
class MPXTlsHelper;
class CMPXCommonUiHelper;
class MMPXCollectionUtility;
class CEdwinTestControl;

// CLASS DECLARATION

/**
*  Cmpxmusicplayertest test class for STIF Test Framework TestScripter.
*
*  @lib mpxmusicplayertest.lib
*  @since S60 v3.2.3
*/
NONSHARABLE_CLASS(Cmpxmusicplayertest) : public CScriptBase,
										 public MProgressDialogCallback,
										 public MEikCommandObserver,
										 public MMPXCHelperObserver
    {
	public:  // Constructors and destructor
	
		/**
		* Two-phased constructor.
		*/
		static Cmpxmusicplayertest* NewL( CTestModuleIf& aTestModuleIf );
	
		/**
		* Destructor.
		*/
		virtual ~Cmpxmusicplayertest();
	
	public: // Functions from base classes
	
		/**
		* From CScriptBase Runs a script line.
		* @since S60 v3.2.3
		* @param aItem Script line containing method name and parameters
		* @return Symbian OS error code
		*/
		virtual TInt RunMethodL( CStifItemParser& aItem );
		
		// function from MProgressDialogCallback
		virtual void DialogDismissedL( TInt aButtonId );
		//function from MEikCommandObserver
		virtual void ProcessCommandL(TInt aCommandId);
		//function from MMPXCHelperObserver
		virtual void HandleOperationCompleteL( TCHelperOperation aOperation,
		                                           TInt aErr,
		                                           void* aArgument );
	private:
	
		/**
		* C++ default constructor.
		*/
		Cmpxmusicplayertest( CTestModuleIf& aTestModuleIf );
	
		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL();
	
		/**
		* Frees all resources allocated from test methods.
		* @since S60 v3.2.3
		*/
		void Delete();
	
		/**
		* Test methods are listed below.
		* @since S60 v3.2.3
		* @param aItem Script line containing parameters.
		* @return Symbian OS error code.
		*/
		TInt Initialize( CStifItemParser& aItem );
		TInt VerifyHostUid( CStifItemParser& aItem );
		TInt VerifyNeedSave( CStifItemParser& aItem );
		TInt VerifyAllowMove( CStifItemParser& aItem );
		TInt VerifyLaunchMode( CStifItemParser& aItem );
		TInt VerifyFilePath( CStifItemParser& aItem );
		TInt Uninitialize( CStifItemParser& aItem );                      			  //debug pass  10,28
		
		//test functions for mpxcommonuihelper.h
		//to test IMPORT_C static CMPXCommonUiHelper* NewL(MMPXCollectionUtility* aCollectionUtility = NULL);
		TInt CMPXCommonUiHelperNewL( CStifItemParser& aItem );                        //debug pass  10,28
		//to test virtual ~CMPXCommonUiHelper();
		TInt CMPXCommonUiHelperDestruct( CStifItemParser& aItem );                    //debug pass  10,28
		//to test IMPORT_C TBool IsHostMessagingBrowserL();
		TInt UiHelperIsHostMessagingBrowserL( CStifItemParser& aItem );               //debug pass  10,28
		//to test IMPORT_C TBool IsHostPodcastingAppL();
		TInt UiHelperIsHostPodcastingAppL( CStifItemParser& aItem ); 				  //debug pass  10,28
		//to test IMPORT_C TBool IsHostMessagingL();
		TInt UiHelperIsHostMessagingL( CStifItemParser& aItem );    				  //debug pass  10,28
		//to test IMPORT_C TBool IsHostBrowserL();
		TInt UiHelperIsHostBrowserL( CStifItemParser& aItem ); 						  //debug pass  10,28
		//to test IMPORT_C HBufC* DisplayableDurationL(
	    //											TInt64 aDuration,
	    //											TMPXDuratDisplayMode aMode = EMPXDuratAuto );
		TInt UiHelperDisplayableDurationL( CStifItemParser& aItem );  				  //debug pass  10,29
		//to test IMPORT_C HBufC* DisplayableDurationInTextL(TInt64 aDuration );
		TInt UiHelperDisplayableDurationInTextL( CStifItemParser& aItem ); 			  //debug pass  10,29 
	    //to test IMPORT_C void DoSetAsRingingToneL(
	    //											const CMPXMedia& aMedia,
	    //											TBool aSetToAllProfiles,
	    //											TBool aShowQuery = EFalse );
		TInt UiHelperDoSetAsRingingToneL( CStifItemParser& aItem );  			 	  //debug pass  10,29	
		//to test IMPORT_C TBool IsProfileOfflineModeL();
		TInt UiHelperIsProfileOfflineModeL( CStifItemParser& aItem );			 	  //debug pass  10,29	
		//to test IMPORT_C HBufC* DefaultDriveLC();
		TInt UiHelperDefaultDriveLC( CStifItemParser& aItem );			 	 		  //debug 10,29 crash-->pass 10,31
		//to test IMPORT_C HBufC* AvailableDriveLC( TInt aSize );
		TInt UiHelperAvailableDriveLC( CStifItemParser& aItem );		 	 		  //debug 10,29	crash-->pass 10,31
		//to test IMPORT_C void DisplayInfoNoteL( TInt aResourceId );
		TInt UiHelperDisplayInfoNoteLRSCID( CStifItemParser& aItem );  			 	  //debug pass  10,29			
		//to test IMPORT_C void DisplayInfoNoteL( const TDesC& aText );
		TInt UiHelperDisplayInfoNoteLText( CStifItemParser& aItem ); 			 	  //debug pass  10,29	 		
		//to test IMPORT_C void DisplayConfirmNoteL( TInt aResourceId );
		TInt UiHelperDisplayConfirmNoteLRSCID( CStifItemParser& aItem );		 	  //debug pass  10,29	 	
		//to test IMPORT_C void DisplayConfirmNoteL( const TDesC& aText );
		TInt UiHelperDisplayConfirmNoteLText( CStifItemParser& aItem );			 	  //debug pass  10,29	
		//to test IMPORT_C HBufC* UnitConversionL( TInt64 aByte, TBool aFree = EFalse );
		TInt UiHelperUnitConversionL( CStifItemParser& aItem );				 		  //debug pass  10,29	
		//to test IMPORT_C CMPXMedia* FindPlaylistsL();
		TInt UiHelperFindPlaylistsL( CStifItemParser& aItem );				 		  //debug pass  10,29		
		//to test IMPORT_C TBool AddToSavedPlaylistL(
	    //						const CMPXMedia& aPlaylists, const CMPXMedia& aTracks,
	    //						MMPXCHelperObserver* aObserver,
	    //						MProgressDialogCallback* aCallback );
		TInt UiHelperAddToSavedPlaylistL( CStifItemParser& aItem );		 			  //debug pass  10,30	
		//to test IMPORT_C TBool CreatePlaylistL( const CMPXMedia& aTracks,
	    //										 MMPXCHelperObserver* aObserver,
	    //										 MProgressDialogCallback* aCallback );
		TInt UiHelperCreatePlaylistL( CStifItemParser& aItem );		 			  	  //debug pass  10,30	
		//to test IMPORT_C TInt LaunchRenameDialogL(
	    //											const TDesC& aOldName,
		//											TDes& aNewName,
		//											const TDesC& aPath );
		TInt UiHelperLaunchRenameDialogL( CStifItemParser& aItem );		 			  //debug pass  10,30	
		//to test IMPORT_C void ShowWaitNoteL( TDesC& aText, TInt aCBAId,
	    //									   TBool aVisibilityDelayOff,
		//									   MProgressDialogCallback* aCallback,
		//									   TInt aResId = R_MPX_GENERIC_WAIT_NOTE );
		TInt UiHelperShowWaitNoteL( CStifItemParser& aItem );						  //debug 10,30	crash-->pass 5,11,2008
		//to test IMPORT_C void DismissWaitNoteL();
		TInt UiHelperDismissWaitNoteL( CStifItemParser& aItem );				  //debug 10,30	crash-->pass 5,11,2008
		//to test IMPORT_C TInt HandleErrorL( TInt aError,
	    //									  const CMPXMedia* aMedia=NULL,
	    //									  RFile* aFile=NULL );
		TInt UiHelperHandleErrorL( CStifItemParser& aItem );		 			 	  //debug pass  10,30
		//to test IMPORT_C void SetMiddleSoftKeyLabelL(
	    //										CEikButtonGroupContainer& aCba,
	    //										TInt aResourceId,
	    //										TInt aCommandId );
		TInt UiHelperSetMiddleSoftKeyLabelL( CStifItemParser& aItem );			  	  //debug 10,30	crash-->pass 6,11,2008
		// to test IMPORT_C void RemoveMiddleSoftKeyLabel(CEikButtonGroupContainer& aCba );
		TInt UiHelperRemoveMiddleSoftKeyLabel( CStifItemParser& aItem );		  	  //debug 10,30	crash-->pass 6,11,2008
	    //to test IMPORT_C void SetMiddleSoftKeyIconL(
	    //											  CEikButtonGroupContainer& aCba,
	    //											  CFbsBitmap* aBitmap,
	    //											  CFbsBitmap* aMask );
	    TInt UiHelperSetMiddleSoftKeyIconL( CStifItemParser& aItem );                 // debug 10,30 crash-->pass 6,11,2008 
	    //to test IMPORT_C HBufC* GenerateTitleL(
	    //											TMPXGeneralCategory aCategory,
	    //											const TDesC& aBaseTitle );
		TInt UiHelperGenerateTitleL( CStifItemParser& aItem );  		 			  //debug pass  10,30  
		//to test IMPORT_C TBool ExitOptionHiddenL();
		TInt UiHelperExitOptionHiddenL( CStifItemParser& aItem );		 			  //debug pass  10,30   
		//to test IMPORT_C static TBool SetStandAloneModePId( TInt64 aPId );
		TInt UiHelperSetStandAloneModePId( CStifItemParser& aItem );		 		  //debug pass  10,30 	
		//to test IMPORT_C static TInt64 StandAloneModePId();
		TInt UiHelperStandAloneModePId( CStifItemParser& aItem );		 		  	  //debug pass  10,30 
		//to test IMPORT_C TBool IsForegroundApplication(TInt aWindowGroupId);
		TInt UiHelperIsForegroundApplication( CStifItemParser& aItem );	 		  	  //debug pass  10,30 
		//to test IMPORT_C static TInt MMCDriveNumber();
		TInt UiHelperMMCDriveNumber( CStifItemParser& aItem );	 		 		  	  //debug pass  10,30 
		//to test IMPORT_C void CancelCollectionOperation();
		TInt UiHelperCancelCollectionOperation( CStifItemParser& aItem );		  	  //debug       10,30 	through
		
		
		
	private:    // Data
		// TLS Helper only contains static functions; no instance is necessary.
		CMPXCommonUiHelper* iUiHelper;
		CEdwinTestControl* iEdwinTestControl ;
		MMPXCollectionUtility* iUtilityForUiHelper;
    };

#endif      // MPXMUSICPLAYERTEST_H
