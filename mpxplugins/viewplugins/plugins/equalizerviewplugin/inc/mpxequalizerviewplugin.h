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
* Description:  MPX Equalizer view plugin definition.
*
*/



#ifndef C_CMPXEQUALIZERVIEWPLUGIN_H
#define C_CMPXEQUALIZERVIEWPLUGIN_H


// INCLUDES
#include <mpxviewplugin.h>


// FORWARD DECLARATIONS
class CMPXEqualizerView;


// CLASS DECLARATION

/**
 *  MPX Equalizer view plugin definition.
 *
 *  @lib mpxequalizerviewplugin.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXEqualizerViewPlugin ) : public CMPXViewPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXEqualizerViewPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXEqualizerViewPlugin();

private:

    /**
     * C++ default constructor.
     */
    CMPXEqualizerViewPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

// from base class CMPXViewPlugin

    /**
     * From CMPXViewPlugin
     * Interface method to create view.
     *
     * @since S60 v3.0
     */
    void CreateViewL();

    /**
     * From CMPXViewPlugin
     * Interface method to destroy view.
     *
     * @since S60 v3.0
     */
    void DestroyView();

    /**
     * From CMPXViewPlugin
     * Interface method to activate view.
     *
     * @since S60 v3.0
     * @param aParam Parameter required to activate the view.
     */
    void ActivateViewL( const TDesC* aParam = NULL );
    
    /**
     * From CMPXViewPlugin
     * Interface method to activate as default view, will not
     * bring to foreground.
     *
     * @since S60 v3.0
     */
    void SetAsDefaultViewL();

    /**
     * From CMPXViewPlugin
     * Interface method to deactivate view.
     *
     * @since S60 v3.0
     */
    void DeactivateView();

private:    // data

    CMPXEqualizerView* iView;
    };

#endif  // C_CMPXEQUALIZERVIEWPLUGIN_H

// End of File
