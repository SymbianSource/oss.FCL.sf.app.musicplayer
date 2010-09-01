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


#ifndef MPXALBUMARTEDITORDIALOGCUSTOMCONTROL_H
#define MPXALBUMARTEDITORDIALOGCUSTOMCONTROL_H

//  INCLUDES
#include <coecntrl.h>
#include "mpxalbumartutilobserver.h"

// FORWARD DECLARATIONS
class CEikImage;
class CEikLabel;
class CMPXAlbumArtEditorDialog;
class MMPXAlbumArtEventObserver;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
 * The custom control for Album Art Dialog.
 */
NONSHARABLE_CLASS( CMPXAlbumArtEditorDialogCustomControl ) : public CCoeControl,
                                                             public MMPXAlbumArtUtilObserver
    {
public:           
    // States for category rename operations
    enum TMPXAlbumArtEditorDialogMode
        {
        EMPXAlbumArtEditorDialogBlank = 0,
        EMPXAlbumArtEditorDialogAlbumArt,
        EMPXAlbumArtEditorDialogNoAlbumArt
        };

public:  // Constructors and destructor

    /**
     * Two-phased constructor. Leaves on failure. Places the instance on the
     * cleanup stack.
     * @param aParent Parent control.
     * @param aRect The control's extent.
     * @param aObserver observer for album art events
     * @return The constructed item.
     */
    static CMPXAlbumArtEditorDialogCustomControl* NewLC( const CCoeControl& aParent, 
                                                       const TRect& aRect,
                                                       MMPXAlbumArtEventObserver* aObserver );

    /**
     * Two-phased constructor. Leaves on failure.
     * @param aParent Parent control.
     * @param aRect The control's extent.
     * @param aObserver observer for album art events
     * @return The constructed item.
     */
    static CMPXAlbumArtEditorDialogCustomControl* NewL( const CCoeControl& aParent, 
                                                      const TRect& aRect,
                                                      MMPXAlbumArtEventObserver* aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CMPXAlbumArtEditorDialogCustomControl();       
    
private:

    /**
     * C++ default constructor.
     */
    CMPXAlbumArtEditorDialogCustomControl();

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aParent Parent control.
     * @param aRect The control's extent.
     * @param aObserver observer for album art events
     */
    void ConstructL( const CCoeControl& aParent, 
                     const TRect &aRect,
                     MMPXAlbumArtEventObserver* aObserver );

public: // New functions
    
    /**
     * Updates and displays the new album art.
     * @param aMode album art mode.
     * @param aNewAlbumArt The new bitmap album art.
     */
    void UpdateAlbumArt( TMPXAlbumArtEditorDialogMode aMode, 
                         CFbsBitmap* aNewAlbumArt = NULL );

    /**
     * Displays image in a specific mode, 
     * either Album Art/No Album Art or Various
     * @param aMode album art mode.
     */      
    void SetModeL( TMPXAlbumArtEditorDialogMode aMode );
    
    /**
     * updates skin color
     */
    void HandleSkinChangedL();

private: // from base class MMPXAlbumArtUtilObserver

    /**
     * From MMPXAlbumArtUtilObserver.
     * Notify that extraction of album art started.
     */
    void ExtractAlbumArtStarted();

    /**
     * From MMPXAlbumArtUtilObserver.
     * Album art is extracted from file and ready to use.
     *
     * @param aBitmap a converted image. Ownership is transferred.
     * @param aErr error code
     */
    void ExtractAlbumArtCompleted(
        CFbsBitmap* aBitmap, 
        TInt aErr );    
    
private: // Functions from base classes
    	   
    /**
     * Draws to screen.
     * @param aRect The control's extent
     */
    void Draw( const TRect& aRect ) const;

    /**
     * Determines the number of components.
     */
    TInt CountComponentControls() const;

    /**
     * Passes control of indexed component.
     * @param aIndex The desired control index.
     * @return The components control.
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
     * Resizes controls.
     */
    void SizeChanged();
    
 private:    // Data
   
    CEikImage*                   iImage; // image to display Album Art
    CEikLabel*                   iLabel; // Labe to display either "(various)" or "(no album art)"       
    TMPXAlbumArtEditorDialogMode iMode;  // mode of either Album Art, Various or No Album Art
    MMPXAlbumArtEventObserver*   iObserver;
    CAknsBasicBackgroundControlContext* iBackground;
    TRect                        iBorder;
    };

#endif      // MPXALBUMARTEDITORDIALOGCUSTOMCONTROL_H 
            
// End of File
