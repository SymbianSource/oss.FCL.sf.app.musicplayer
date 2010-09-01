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


#ifndef ui_mpxmusicplayercommonuitest_H
#define ui_mpxmusicplayercommonuitest_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <mpxalbumartutil.h>
#include <mpxmedia.h>
#include "mpxalbumartutilobserver.h"
#include "mpxmediageneraldefs.h"
#include "mpxmediamusicdefs.h"

#include "EdwinTestControl.h"
#include "ListBoxTestControl.h"
#include "mpxlbxextendedfeatures.h"




// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

#ifdef __WINSCW__
// Logging path
_LIT( Kui_mpxmusicplayercommonuitestLogPath, "c:\\logs\\testframework\\ui_mpxmusicplayercommonuitest\\" );
//Data path
_LIT( Kui_mpxmusicplayercommonuitestDataPath, "c:\\testing\\data\\" );
// Log file
_LIT( Kui_mpxmusicplayercommonuitestLogFile, "ui_mpxmusicplayercommonuitest.txt" ); 
_LIT( Kui_mpxmusicplayercommonuitestLogFileWithTitle, "ui_mpxmusicplayercommonuitest_[%S].txt" );
#else
// Logging path
_LIT( Kui_mpxmusicplayercommonuitestLogPath, "e:\\logs\\testframework\\ui_mpxmusicplayercommonuitest\\" );
//Data path
_LIT( Kui_mpxmusicplayercommonuitestDataPath, "e:\\testing\\data\\" );
// Log file
_LIT( Kui_mpxmusicplayercommonuitestLogFile, "ui_mpxmusicplayercommonuitest.txt" ); 
_LIT( Kui_mpxmusicplayercommonuitestLogFileWithTitle, "ui_mpxmusicplayercommonuitest_[%S].txt" );
#endif

//class ?FORWARD_CLASSNAME;
class CMPXMusicPlayerCommonuiStifTest ;

class CmpxMusicPlayerCommonuiTest;
class CMPXAlbumArtUtil;
class CMPXMedia;
class CMPXLbxExtendedFeatures;

class CMPXLbxExtendedFeatures;


// CLASS DECLARATION

/**
*  CMPXMusicPlayerCommonuiStifTest  test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ui_mpxmusicplayercommonuitest.lib
*  @since Series60_version
*/
NONSHARABLE_CLASS(CMPXMusicPlayerCommonuiStifTest ) : public CScriptBase, public MMPXAlbumArtUtilObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXMusicPlayerCommonuiStifTest * NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMPXMusicPlayerCommonuiStifTest ();

    public: 
       	    //function from mpxalbumartutilobserver.h
       	     void ExtractAlbumArtStarted();

       	  //function from mpxalbumartutilobserver.h
       	     void ExtractAlbumArtCompleted(CFbsBitmap* aBitmap, TInt aErr );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
  

    private:

        /**
        * C++ default constructor.
        */
        CMPXMusicPlayerCommonuiStifTest ( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        //test functions for mpxalbumartutil.h
        //to test IMPORT_C static CMPXAlbumArtUtil* NewL();
        TInt CMPXAlbumArtUtilCreate( CStifItemParser& aItem );//Pass Debug
        //to test IMPORT_C ~CMPXAlbumArtUtil();
        TInt CMPXAlbumArtUtilDestructor (CStifItemParser& aItem );//Pass Debug
        // to test IMPORT_C void ExtractAlbumArtL(const CMPXMedia& aProp,
        //											MMPXAlbumArtUtilObserver& aObs,
        //											const TSize& aSize,
        //											TDisplayMode aDisplayMode = EColor64K);
        TInt CMPXAlbumArtUtilExtractAlbumArtL (CStifItemParser& aItem );//Pass Debug
        //to test IMPORT_C void CancelRequest();
        TInt CMPXAlbumArtUtilCancelRequest (CStifItemParser& aItem );//Pass Debug
        
        
        //test functions for mpxlbxextendedfeatures.h
        //to test  IMPORT_C static CMPXLbxExtendedFeatures* NewL(
        //														CEikTextListBox* aLbx,
        //														TBool aEnableSetIndex = ETrue );
        TInt CMPXLbxExtendedFeaturesCreate( CStifItemParser& aItem );//Pass Debug
        //to test IMPORT_C virtual ~CMPXLbxExtendedFeatures();
        TInt CMPXLbxExtendedFeaturesDestructor( CStifItemParser& aItem );//Pass Debug
        //to test IMPORT_C TKeyResponse HandleLbxKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        TInt CMPXLbxExtendedFeaturesHandleLbxKeyEventL( CStifItemParser& aItem );//Pass Debug
        //to test   IMPORT_C void EnableSpeedScrollL( TBool isEnable );
        TInt CMPXLbxExtendedFeaturesEnableSpeedScrollL( CStifItemParser& aItem );//Pass Debug
        //to test IMPORT_C void SpeedNaviUpdating( TBool aNaviEnable );
        TInt CMPXLbxExtendedFeaturesSpeedNaviUpdating( CStifItemParser& aItem );//Pass Debug
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    private:    // Friend classes
    	CMPXAlbumArtUtil* iMPXAlbumArtUtil;
    	CMPXLbxExtendedFeatures* iMPXLbxExtendedFeatures;
    };

#endif      // ui_mpxmusicplayercommonuitest_H

// End of File
