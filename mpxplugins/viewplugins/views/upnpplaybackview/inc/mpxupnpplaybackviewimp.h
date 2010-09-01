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
* Description:  MPX upnp playback view implementation
*
*/



#ifndef C_CMPXUPNPPLAYBACKVIEWIMP_H
#define C_CMPXUPNPPLAYBACKVIEWIMP_H


// INCLUDES
#include <akntoolbarobserver.h> 
#include "mpxupnpplaybackview.h"

// FORWARD DECLARATIONS
class CMPXUPnPPlaybackViewContainer;
class CMPXUPnPPlaybackViewLayout;
class CAknToolbar;  

// CLASS DECLARATION

/**
 *  MPX playback view.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXUPnPPlaybackViewImp ) : public CMPXUPnPPlaybackView,
                                               public MAknToolbarObserver

    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXUPnPPlaybackViewImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXUPnPPlaybackViewImp* NewLC();

    /**
     * Destructor.
     */
    ~CMPXUPnPPlaybackViewImp();

private:

    /**
     * C++ default constructor.
     */
    CMPXUPnPPlaybackViewImp();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

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
     * Retrieves the current player name and updates the Label
     */
    void UpdateTitlePaneL();

    /**
     * Updates track info field.
     *
     * @param aMedia Media's properties. If NULL, default info will
     *                    be shown.
     */
    void UpdateTrackInfoL( const CMPXMedia* aMedia );


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
     * Displays a wait note when callback from CIdle
     */
    static TInt LoadingSong( TAny* aPtr );

    /**
     * Displays a wait note
     */
    void LoadingSongL();

    /**
     * Dismisses loading song wait note
     */
    void DismissWaitNoteL();

    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    void HandleErrorL( TInt aError );

// from base class MProgressDialogCallback
    /**
     * From MProgressDialogCallback
     * Callback method
     * Get's called when a dialog is dismissed.
     */
    void DialogDismissedL( TInt aButtonId );

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
     * @param aResourceId The ID of the menu pane.
	 * @param aToolBar The ToolBar itself.
     */
    void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolBar );

private:    // data
    CMPXUPnPPlaybackViewLayout* iPlaybackViewLayout;    // owned
    TInt iUpnpResourceOffset;       // must be freed

    CIdle* iLoadingWaitNote; // Owned
    TBool iUPnPLoadingWaitNoteDisplayed;
    TBool iPodcastPlayback;
    CAknToolbar* iToolbar; //owned
    };

#endif  // C_CMPXUPNPPLAYBACKVIEWIMP_H

// End of File
