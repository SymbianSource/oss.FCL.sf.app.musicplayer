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
* Description:  MPX Album art editor dialog plugin definition.
*
*/



#ifndef C_CMPXALBUMARTEDITORDIALOGPLUGIN_H
#define C_CMPXALBUMARTEDITORDIALOGPLUGIN_H


// INCLUDES
#include <mpxakndialogplugin.h>


// CLASS DECLARATION

/**
 *  MPX album art editor dialog plugin definition.
 *
 *  @lib mpxalbumarteditordialogplugin.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXAlbumArtEditorDialogPlugin ) : public CMPXAknDialogPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXAlbumArtEditorDialogPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXAlbumArtEditorDialogPlugin();

private:

    /**
     * From CMPXAknDialogPlugin
     * Construct Avkon dialog.
     *
     * @since S60 v3.0
     * @return Pointer to a newly created Avkon dialog.
     */
    CAknDialog* ConstructDialogL();
    
    /**
     * From CMPXAknDialogPlugin
     * Resource Id of the dialog
     *
     * @since S60 v3.0
     * @return Dialog resource ID
     */
    TInt ResourceId();
    
    /**
     * From CMPXViewPlugin
     * Interface method to activate view.
     *
     * @since S60 v3.0
     * @param aParam Parameter required to activate the view.
     */
    void ActivateViewL( const TDesC* aParam = NULL );

private:

    /**
     * C++ default constructor.
     */
    CMPXAlbumArtEditorDialogPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_CMPXALBUMARTEDITORDIALOGPLUGIN_H

// End of File