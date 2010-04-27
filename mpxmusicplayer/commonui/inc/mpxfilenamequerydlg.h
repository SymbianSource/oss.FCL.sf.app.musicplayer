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
* Description:  Definition of file rename dialog for Common UI
*
*/


#ifndef C_CMPXFILENAMEQUERYDLG_H
#define C_CMPXFILENAMEQUERYDLG_H

//  INCLUDES
#include <AknQueryDialog.h>     // CAknTextQueryDialog
#include <mpxmediageneraldefs.h>

// CLASS DECLARATION
class CMPXCommonUiHelper;

/**
 *  @lib mpxcommonui.lib
 *  @since S60 v3.0
 *  Dialog for querying file names in File Manager
 */
NONSHARABLE_CLASS( CMPXFileNameQueryDlg ): public CAknTextQueryDialog
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aOldName Old name of the file, this will be the default name
         * @param aNewName User entered new name of the file.
         * @param aPath original path for the file, needed to check if the file
         *              already exist
         * @param aCategory specifies the category for the title
         * @return Newly created query dialog.
         */
        static CMPXFileNameQueryDlg* NewL( const TDesC& aOldName, 
                                           TDes& aNewName,
                                           const TDesC& aPath, 
                                           TMPXGeneralCategory aCategory,
                                           CMPXCommonUiHelper* aParent );

		/**
        * Destructor.
        */
        ~CMPXFileNameQueryDlg();

    public: // from CAknTextQueryDialog
        /**
         * @see CAknTextQueryDialog
         */
    	TBool OkToExitL( TInt aButtonId );
    	
    	/**
    	 * @see CAknTextQueryDialog
    	 */
    	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType ); 

	private:
        /**
        * C++ default constructor.
        */
        CMPXFileNameQueryDlg( 
            TDes& aNewName, 
            TMPXGeneralCategory aCategory,
            CMPXCommonUiHelper* aParent );

        /**
         * Symbian OS 2nd phase constructor.
         * @param aOldName Old name of the file, this will be the default name
         * @param aPath original path for the file
         */
		void ConstructL( const TDesC& aOldName, const TDesC& aPath );
		
		/**
        * Is given file name valid
        * @since 3.0
        * @param aDriveAndPath root path
        * @param aName name of the file
        * @param aIsFolder ETrue folder and EFalse file name
        * @return ETrue if file name is valid
        */
        TBool IsValidName(
            const TDesC& aDriveAndPath, const TDesC& aName, TBool aIsFolder ) const;

        /**
        * Check that if given file name contains illegal characters
        * @since 3.0
        * @param aName name of the file
        * @return ETrue if file name is valid
        */
		TBool IllegalChars( const TDesC& aName ) const;

    private:    // Data        
        HBufC*                      iOldName;  // own
        HBufC*                      iPath;     // own
        CMPXCommonUiHelper*         iParent; // not own
        MMPXCollectionUiHelper*     iCollectionUiHelper; //own
        TMPXGeneralCategory         iCategory;
    };

#endif      // C_CMPXFILENAMEQUERYDLG_H
            
// End of File
