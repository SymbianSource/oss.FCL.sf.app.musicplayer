/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MPX asynchronous save helper utility
*
*/



#ifndef CMPXSAVEHELPER_H
#define CMPXSAVEHELPER_H

//  INCLUDES
#include <f32file.h>
#include <e32base.h>

// INTERFACE DEFINITION
/**
*  Observer class for Async operation
*/
NONSHARABLE_CLASS(MMPXSaveHelperObserver)
    {
public:
    /**
    * 
    *  @param aBitmap a converted image.
    *  @param aErr error code
    */
    virtual void HandleSaveComplete( TInt aErr ) = 0;
    };



// CLASS DECLARATION

/**
*  CMPXSaveHelper
*/
NONSHARABLE_CLASS(CMPXSaveHelper) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMPXSaveHelper* NewL( MMPXSaveHelperObserver* aObserver, MFileManObserver* aFMObserver );
                 
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPXSaveHelper();

    public: // New functions
        
        /**
        * Starts async copy operation
        * @param anOld source
        * @param aNew target
        * @param aMove flag whether to move or just copy
        * @return void
        */
        IMPORT_C void StartCopyOperationL( const TDesC& anOld,
                                           const TDesC& aNew,
                                           TBool aMove=EFalse );

        /**
        * Starts async copy operation
        * @param anOldFile source
        * @param aNew target
        * @param aMove flag whether to move or just copy
        * @return void
        */
        IMPORT_C void StartCopyOperationL( RFile& anOldFile, 
                                           const TDesC& aNew,
                                           TBool aMove=EFalse,
                                           TBool aSync=EFalse );

   private: // Constructors

        /**
        * C++ default constructor.
        */
        CMPXSaveHelper( MMPXSaveHelperObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MFileManObserver* aFMObserver );

    private: 
        
        // From CActive
        
        /** @see CActive::RunL() */
        void RunL();
        
        /** @see CActive::DoCancel() */
        void DoCancel();

    private:    // Data
        CFileMan* iFileMan;
        MMPXSaveHelperObserver* iObserver; // not owned
    };

#endif      // CMPXSAVEHELPER_H 
            
// End of File
