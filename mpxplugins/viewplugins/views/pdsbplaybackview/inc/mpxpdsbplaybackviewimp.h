/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MPX Progressive Download Init from Stand-alone Browser playback
*                view implementation
*
*/



#ifndef C_CMPXPDSBPLAYBACKVIEWIMP_H
#define C_CMPXPDSBPLAYBACKVIEWIMP_H


// INCLUDES
#include <mpxplaybackcommanddefs.h>
#include "mpxpdsbplaybackview.h"
#include <akntoolbarobserver.h>

// FORWARD DECLARATIONS
class CMPXPdSbPlaybackViewContainer;
class CMPXPdSbPlaybackViewLayout;

class CAknToolbar;

// CLASS DECLARATION

/**
 *  MPX Progressive Download playback view.
 *
 *  @lib mpxpdsbplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPdSbPlaybackViewImp ) : public CMPXPdSbPlaybackView,
                                               public MAknToolbarObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdSbPlaybackViewImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdSbPlaybackViewImp* NewLC();

    /**
     * Destructor.
     */
    ~CMPXPdSbPlaybackViewImp();

private:

    /**
     * C++ default constructor.
     */
    CMPXPdSbPlaybackViewImp();

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

    void HandleForegroundEventL( TBool aForeground );

    /**
    * Find Brower by unique id and bring it to foreground
    *
    * @return ETrue if browser if found. Otherwise, EFalse.
    * @since 5.0
    *
    */
    TBool BringUpBrowserL();

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
    CMPXPdSbPlaybackViewLayout* iPlaybackViewLayout;    // owned
    TInt iPdResourceOffset;       // must be freed
    TMPXPlaybackPdDownloadState iDownloadState;
    TInt iTotalBytes;
    TBool iForeground;
    TBool iPlaybackCompleted;
    TBool iExiting;
    CAknToolbar* iToolbar; //owned
    };

#endif  // C_CMPXPDSBPLAYBACKVIEWIMP_H

// End of File
