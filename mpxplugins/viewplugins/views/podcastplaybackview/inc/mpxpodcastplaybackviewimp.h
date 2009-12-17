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
* Description:  MPX Podcast playback view implementation
*
*/



#ifndef C_CMPXPODCASTPLAYBACKVIEWIMP_H
#define C_CMPXPODCASTPLAYBACKVIEWIMP_H


// INCLUDES
#include "mpxpodcastplaybackview.h"
#include <akntoolbarobserver.h>

// FORWARD DECLARATIONS
class MMPXCollectionUtility;
class MMPXCollectionUiHelper;
class CMPXPodcastPlaybackViewContainer;
class CMPXPodcastPlaybackViewLayout;
class CAknToolbar;


// CLASS DECLARATION

/**
 *  MPX playback view.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPodcastPlaybackViewImp ) : public CMPXPodcastPlaybackView
                                           ,public MAknToolbarObserver
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastPlaybackViewImp* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastPlaybackViewImp* NewLC();

    /**
     * Destructor.
     */
    ~CMPXPodcastPlaybackViewImp();

private:

    /**
     * C++ default constructor.
     */
    CMPXPodcastPlaybackViewImp();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

// from base class CMPXCommonPlaybackViewImp

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

    /**
     * Updates track info field.
     *
     * @param aMedia Media's properties. If NULL, default info will
     *                    be shown.
     */
    void UpdateTrackInfoL( const CMPXMedia* aMedia );

    /**
    * Restore status pane to it's normal state.
    */
    void PrepareStatusPaneForExitingViewL();

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
     *  Handle playback message
     *
     *  @param aMsg playback message
     */
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );

    /**
     * Updates the middle (play/pause) toolbar control key
     * icon based on playback state
     */
    void UpdateToolbar();

private:    // data

    CMPXPodcastPlaybackViewLayout* iPlaybackViewLayout;    // owned

    TInt  iPodcastResourceOffset;    // must be freed
    TInt  iPosition;
    CAknToolbar* iToolbar; //owned
    };

#endif  // C_CMPXPODCASTPLAYBACKVIEWIMP_H

// End of File
