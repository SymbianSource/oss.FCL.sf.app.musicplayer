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
* Description:  MPX podcast playback view's container implementation.
*
*/



#ifndef C_CMPXPODCASTPLAYBACKVIEWCONTAINER_H
#define C_CMPXPODCASTPLAYBACKVIEWCONTAINER_H


// INCLUDES
#include "mpxcommonplaybackviewcontainer.h"


// FORWARD DECLARATIONS
class MMPXLayoutSwitchObserver;
class MMPXPlaybackViewLayout;

// CLASS DECLARATION

/**
 *  Container class for podcast playback view.
 *
 *  @lib mpxpodcastplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPodcastPlaybackViewContainer ) : public CMPXCommonPlaybackViewContainer
    {
public:

    /**
     * C++ default constructor.
     *
     * @param aCommandObserver  Observer for commands.
     * @param aLayoutObserver   Observer to send layout switch events to.
     * @param aLayout           Pointer to playback view layout.
     */
    CMPXPodcastPlaybackViewContainer(
        MEikCommandObserver* aCommandObserver,
        MMPXLayoutSwitchObserver* aLayoutObserver,
        MMPXPlaybackViewLayout* aLayout );

    /**
     * Symbian 2nd phase constructor.
     * 
     * @param aRect Frame rectangle for container.
     */
    void ConstructL( const TRect& aRect );

    /**
     * Destructor.
     */
    ~CMPXPodcastPlaybackViewContainer();

    /**
     * Set layout for all elements and set sizes for icons.
     *
     * @since S60 3.0
     */
    void UpdateLayout();
    
// from base class CoeControl

    /**
     * From CoeControl.
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );
    /**
     * From CoeControl.
     * Gets the control's help context.
     *
     * @param aContext The control's help context.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    };

#endif  // C_CMPXPODCASTPLAYBACKVIEWCONTAINER_H

// End of File