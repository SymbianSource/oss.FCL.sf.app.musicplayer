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



#ifndef MPXMUSICPLAYERMEDIAKEYHANDERTEST_H
#define MPXMUSICPLAYERMEDIAKEYHANDERTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <mpxmediakeyhandlerobserver.h>
#include <mpxmediakeyhandler.h>
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0
#ifdef __WINSCW__
// Logging path
_LIT( KmpxmusicplayermediakeyhandertestLogPath, "c:\\logs\\testframework\\mpxmusicplayermediakeyhandertest\\" ); 
// Log file
_LIT( KmpxmusicplayermediakeyhandertestLogFile, "mpxmusicplayermediakeyhandertest.txt" ); 
_LIT( KmpxmusicplayermediakeyhandertestLogFileWithTitle, "mpxmusicplayermediakeyhandertest_[%S].txt" );
#else
_LIT( KmpxmusicplayermediakeyhandertestLogPath, "e:\\logs\\testframework\\mpxmusicplayermediakeyhandertest\\" ); 
// Log file
_LIT( KmpxmusicplayermediakeyhandertestLogFile, "mpxmusicplayermediakeyhandertest.txt" ); 
_LIT( KmpxmusicplayermediakeyhandertestLogFileWithTitle, "mpxmusicplayermediakeyhandertest_[%S].txt" );
#endif
// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMPXMusicplayermediakeyhandertest;
class CCoeControl;
class MMPXMediaKeyHandler;
// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMPXMusicplayermediakeyhandertest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMPXMusicplayermediakeyhandertest) : public CScriptBase,
													   public MMPXMediaKeyHandlerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXMusicplayermediakeyhandertest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMPXMusicplayermediakeyhandertest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        virtual void HandleMediaKeyCommand(const TMPXPlaybackCommand /*aCommand*/,const TInt /*aData*/ ) {};
    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CMPXMusicplayermediakeyhandertest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

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
        TInt CancelMoveMR( CStifItemParser& aItem );
        virtual TInt EDWT_DeleteL( CStifItemParser& aItem );
        virtual TInt MPXmusicplayermediahandlerInstance( CStifItemParser& /*aItem*/ );
        TInt MPXmusicplayermediahandlerDestructor( CStifItemParser& );
        TInt MPXmusicplayermediahandlerShowPlaybackPopupL( CStifItemParser& /* aItem*/ );
        TInt MPXmusicplayermediahandlerShowVolumePopupL( CStifItemParser& /* aItem*/ );
        TInt MPXmusicplayermediahandlerDismissNotifier( CStifItemParser& /* aItem*/ );
        TInt MPXmusicplayermediahandlerSetEnableMediaKeys( CStifItemParser& /* aItem*/ );
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
    	CCoeControl* iEdwinTestControl;
    	MMPXMediaKeyHandler* iMPXMediaKeyHandler;
    };

#endif      // MPXMUSICPLAYERMEDIAKEYHANDERTEST_H

// End of File
