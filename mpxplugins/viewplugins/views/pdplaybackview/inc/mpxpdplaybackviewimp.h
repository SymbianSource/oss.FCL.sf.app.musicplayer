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
* Description:  MPX Progressive Download playback view implementation
*
*/



#ifndef C_CMPXPDPLAYBACKVIEWIMP_H
#define C_CMPXPDPLAYBACKVIEWIMP_H


// INCLUDES
#include <mpxplaybackcommanddefs.h>
#include "mpxpdplaybackview.h"

#include <akntoolbarobserver.h>
// FORWARD DECLARATIONS
class CMPXPdPlaybackViewContainer;
class CMPXPdPlaybackViewLayout;

class CAknToolbar;

// CLASS DECLARATION

/**
 *  MPX Progressive Download playback view.
 *
 *  @lib mpxpdplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPdPlaybackViewImp ) : public CMPXPdPlaybackView
                                           ,public MAknToolbarObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdPlaybackViewImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdPlaybackViewImp* NewLC();

    /**
     * Destructor.
     */
    ~CMPXPdPlaybackViewImp();

private:

    /**
     * C++ default constructor.
     */
    CMPXPdPlaybackViewImp();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Gets download information
     */
    void GetDownloadInformationL(
        TUint& aTransactionId,
        TMPXPlaybackPdDownloadState& aState,
        TInt& aDownloadedBytes,
        TInt& aTotalBytes );

    /**
    * This checks only two things:  Network available, Network Strength>none.
    * @param: aCheck holds the UID of the failed check.
    * @param: aState holds the state of the failed check.
    * @return ETrue if network available and network strength
    * is Low or Medium or High, otherwise return EFalse.
    */
    TBool CheckSignalAvailableL( TUid& aCheck, TInt& aState );


	  /**
     *  Handle playback property
     *
     *  @param aProperty the property
     *  @param aValue the value of the property
     *  @param aError error code
     */
    void DoHandlePropertyL(
        TMPXPlaybackProperty aProperty,
        TInt aValue,
        TInt aError );

// from base class CMPXCommonPlaybackViewImp
    /**
     *  Handle playback message
     *
     *  @param aMessage playback message
     */
    void DoHandlePlaybackMessageL(
        const CMPXMessage& aMessage );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     *  @param aData Extra data parameter, used for any extra information
     *               along with the state change message
     */
    void DoHandleStateChangedL(
        TMPXPlaybackState aState,
        TInt aData );

    /**
     * Updates download state label.
     */
    void UpdateDownloadStateLabelL();

    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    void HandleErrorL( TInt aError );

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
     * Processes user commands.
     *
     * @since 3.0
     * @param aCommand ID of the command to respond to.
     */
    void ProcessCommandL( TInt aCommand );

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

    /**
    * Update the toolbar state
    *
    * @since 5.0
    *
    */
	void UpdateToolbar();

private:    // data
    CMPXPdPlaybackViewLayout* iPlaybackViewLayout;    // owned
    TInt iPdResourceOffset;       // must be freed
    TMPXPlaybackPdDownloadState iDownloadState;
    TInt iTotalBytes;
    CAknToolbar* iToolbar; //owned
    };

#endif  // C_CMPXPLAYBACKVIEWIMP_H

// End of File
