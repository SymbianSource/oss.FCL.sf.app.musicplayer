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
* Description:  MPX UPnP browse dialog plugin definition.
*
*/



#ifndef C_CMPXUPNPBROWSEDIALOGPLUGIN_H
#define C_CMPXUPNPBROWSEDIALOGPLUGIN_H


// INCLUDES
#include <mpxakndialogplugin.h>

// FORWARD DECLARATIONS
class MMPXViewUtility;

// CLASS DECLARATION

/**
 *  MPX Visualization menu dialog plugin definition.
 *
 *  @lib mpxvisualizationmenudialogplugin.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXUPnPBrowseDialogPlugin ) : public CMPXAknDialogPlugin
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXUPnPBrowseDialogPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXUPnPBrowseDialogPlugin();
    
private:  // Functions from base class

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
    CMPXUPnPBrowseDialogPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
private: //data
     MMPXViewUtility* iViewUtility;
     TInt iPlayerUid;
    };

#endif  // C_CMPXUPNPBROWSEDIALOGPLUGIN_H

// End of File
