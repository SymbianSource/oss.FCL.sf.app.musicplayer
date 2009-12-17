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
* Description:  MPX embeddedplayback view implementation
*
*/



#ifndef C_CMPXEMBEDDEDPLAYBACKVIEWIMP_H
#define C_CMPXEMBEDDEDPLAYBACKVIEWIMP_H


// INCLUDES
#include <AknProgressDialog.h>  // MProgressDialogCallback
#include <mpxcollectionhelper.h>
#include <mpxcollectionuihelper.h>
#include "mpxembeddedplaybackview.h"
#include <akntoolbarobserver.h>

// FORWARD DECLARATIONS
class CMPXEmbeddedPlaybackViewContainer;
class CMPXEmbeddedPlaybackViewLayout;
class CAknToolbar;

// CLASS DECLARATION

/**
 *  MPX playback view.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXEmbeddedPlaybackViewImp ) : public CMPXEmbeddedPlaybackView
                                           ,public MAknToolbarObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXEmbeddedPlaybackViewImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXEmbeddedPlaybackViewImp* NewLC();

    /**
     * Destructor.
     */
    ~CMPXEmbeddedPlaybackViewImp();

private:
    enum TMPXNpvSaveMode
    {
    EMPXNpvUsePreviousMode = 0,
    EMPXNpvOpenAfterSave,
    EMXPNpvExitAppAfterSave,
    EMPXNpvExitHostAppAfterSave
    };    

private:

    /**
     * C++ default constructor.
     */
    CMPXEmbeddedPlaybackViewImp();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
// from base class CMPXCommonPlaybackViewImp

    /**
     * Updates the title pane
     */
    void UpdateTitlePaneL();

    /**
     * Updates track info field.
     *
     * @param aMedia Media's properties. If NULL, default info will
     *                    be shown.
     */
    void UpdateTrackInfoL( const CMPXMedia* aMedia );
    
    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    virtual void HandleErrorL( TInt aError );
    
    /**
     *  Handle playback property
     *
     *  @param aProperty the property
     *  @param aValue the value of the property
     *  @param aError error code
     */
    virtual void DoHandlePropertyL(
        TMPXPlaybackProperty aProperty,
        TInt aValue, 
        TInt aError ); 

// from base class MMPXCHelperEmbeddedOpenObserver

    /**
    * From MMPXCHelperEmbeddedOpenObserver
    * Handles errors from opening in embedded mode
    * @param aErr, error code
    */
    void HandleEmbeddedOpenL( TInt aErr, TMPXGeneralCategory aCategory );
    
// from base class MEikMenuObserver

    /**
     * From MEikMenuObserver
     * Dynamically initialises a menu pane.
     *
     * @param aResourceId The ID of the menu pane.
     * @param aMenuPane The menu pane itself.
     */
    void DynInitMenuPaneL(
        TInt aResourceId, 
        CEikMenuPane* aMenuPane );


// from base class CAknView

    /**
     * From CAknView
     * Returns views id.
     *
     * @since 3.0
     * @return Views Uid
     */
    TUid Id() const;

    /**
     * From CAknView
     * Command handling function.
     *
     * @since 3.0
     * @param aCommand Command which is handled
     */
    void HandleCommandL( TInt aCommand );

    /**
     * From CAknView
     * Handles a view activation.
     *
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
     */
    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );


private:    // new methods

    /**
     *  Handle media properties.
     *  
     *  @param aMedia media properties
     *  @param aError error code    
     */
    void DoHandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     * Handle saving a file
     *
     * @param aMode The action to perform after save is completed
     */
    void HandleSaveL( TMPXNpvSaveMode aMode );
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );

    /**
     * From MAknToolbarObserver
     * Processes user commands.
     *
     * @since 5.0
     * @param aCommand ID of the command to respond to.
     */
    void OfferToolbarEventL( TInt aCommandId );
      
    /**
     * From MAknToolbarObserver
     * Used to change toolbar settings before it is made visible
     *
     * @since 5.0
     * @param aCommand ID of the command to respond to.
     */
    void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolBar );

private:    // data
    TInt iEmbeddedResourceOffset;
    CMPXEmbeddedPlaybackViewLayout* iPlaybackViewLayout;    // owned
    TBool iSaveInProgress;
    TBool iPrepareToSave;
    TMPXNpvSaveMode iSaveMode;
    CAknToolbar* iToolbar; //owned
    };

#endif  // C_CMPXPLAYBACKVIEWIMP_H

// End of File
