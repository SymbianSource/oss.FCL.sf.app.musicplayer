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
* Description:  Document class required by AVKON application framework.
*
*/



#ifndef CMPXDOCUMENT_H
#define CMPXDOCUMENT_H


// INCLUDES
#include <GenericParamConsumer.h>

   
// FORWARD DECLARATIONS
class CEikAppUi;
class MMPXViewUtility;


// CLASS DECLARATION

/**
 *  CMPXDocument application class.
 *
 *  @lib mpx.exe
 *  @since S60 v3.1
 */
class CMPXDocument : public CAiwGenericParamConsumer
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aApp AppUi of musicplayer.
     * @return Pointer to newly created object.
     */
    static CMPXDocument* NewL( CEikApplication& aApp );

    /**
     * Destructor.
     */
    virtual ~CMPXDocument();

private:

    /**
     * C++ constructor.
     *
     * @param aApp AppUi of musicplayer.
     */
    CMPXDocument( CEikApplication& aApp );

    /**
     * By default Symbian 2nd phase constructor is private.
     */  
    void ConstructL();

    /**
     * Parse input paramaters from AIW interface.
     * @return ETrue if open a file by framework is required. Otherwise, EFalse.
     */  
    TBool ParseInputParameters();

// from base class CEikDocument

    /**
     * From CEikDocument.
     * Create CMPXAppUi "App UI" object.
     *
     * @return Pointer to newly created object.
     */
    CEikAppUi* CreateAppUiL();

    /**
     * From CEikDocument.
     * Called by the framework to open a file.
     *
     * @param aDoOpen ETrue to open an existing file, 
                      EFalse to create a new default file.
     * @param aFileName THe path and name of the file to open or create.
     * @param aFs FIle server session to use.
     * @return Pointer to newly created object.
     */
    CFileStore* OpenFileL( 
        TBool aDoOpen,
        const TDesC& aFilename,
        RFs& aFs );

    /**
     * From CEikDocument.
     * Called by the framework to open a file.
     *
     * @param aFileStore This is set by the function to the file 
                         store that stores the main document, 
                         if the file is a "native" Symbian OS file, 
                         otherwise it is set to NULL.
     * @param aFile The path and name of the file to read from
     */
    void OpenFileL(
        CFileStore*& aFileStore, 
        RFile& aFile );

private:

    MMPXViewUtility* iViewUtility;
    };

#endif  // CMPXDOCUMENT_H

// End of File
