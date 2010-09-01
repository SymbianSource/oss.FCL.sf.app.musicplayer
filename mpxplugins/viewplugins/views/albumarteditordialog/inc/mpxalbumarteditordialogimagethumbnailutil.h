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
* Description:  Music Player album art editor dialog declaration
*
*/


#ifndef MPXALBUMARTEDITORDIALOGIMAGETHUMBNAILUTIL_H
#define MPXALBUMARTEDITORDIALOGIMAGETHUMBNAILUTIL_H

//  INCLUDES
#include <badesca.h>
#include <MMGFetchVerifier.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Thumbnail command event handling class.
 */
class CMPXAlbumArtEditorDialogImageThumbnailUtil :  public CBase,
                                                    private MMGFetchVerifier
    {
public:  // Constructors and destructor

    /**
     * Creates a new instance of this class.
     * @param aEngine           Phonebook contact engine.
     * @param aThumbnailPopup   the UI control that will display the 
     *                          thumbnail.
     * @return a new instance of this class.
     */
    static CMPXAlbumArtEditorDialogImageThumbnailUtil* NewL();
    
    /**
     * Destructor.
     */
    ~CMPXAlbumArtEditorDialogImageThumbnailUtil();

public: // New functions

    /**
     * Fetch a thumbnail from the photo album, attach it to a contact and
     * display the thumbnail.
     *
     * @param aContactItem contact to attach the thumbnail to.
     * @return result of MGFetch::RunL
     */
    TBool FetchThumbnailL( CDesCArray& aFilesArray );
    
private:  // Implementation
          
    /**
    * Default constructor.
    */
    CMPXAlbumArtEditorDialogImageThumbnailUtil();

    /**
    * Second-phase constructor.
    */
    void ConstructL();        

private: // from MMGFetchVerifier

    /**
    * Verify image selection.
    * @param aSelectedFiles is the pathname of the selected file.
    */
    TBool VerifySelectionL( const MDesCArray* aSelectedFiles );

private:    // Data
    };

#endif // MPXALBUMARTEDITORDIALOGIMAGETHUMBNAILUTIL_H
            
// End of File
