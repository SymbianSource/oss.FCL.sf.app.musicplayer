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
* Description:  MPX embedded playback view's container implementation.
*
*/



#ifndef C_CMPXEMBEDDEDPLAYBACKVIEWCONTAINER_H
#define C_CMPXEMBEDDEDPLAYBACKVIEWCONTAINER_H


// INCLUDES
#include "mpxcommonplaybackviewcontainer.h"


// FORWARD DECLARATIONS
class MMPXLayoutSwitchObserver;
class MMPXPlaybackViewLayout;
class CMPXCommonUiHelper;

// CLASS DECLARATION

/**
 *  Container class for audio view.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXEmbeddedPlaybackViewContainer ) : public CMPXCommonPlaybackViewContainer
    {
public:

    /**
     * C++ default constructor.
     *
     * @param aLayoutObserver    Observer to send layout switch events to.
     * @param aIsEmbedded        ETrue if Music Player is in embedded mode.
     * @param aLayout            Playback view layout
     * @param aCommonUiHelper    Instance of common ui helper
     */
    CMPXEmbeddedPlaybackViewContainer(
        MEikCommandObserver* aCommandObserver,
        MMPXLayoutSwitchObserver* aLayoutObserver,
        MMPXPlaybackViewLayout* aLayout,
        CMPXCommonUiHelper* aCommonUiHelper );

    /**
     * Symbian 2nd phase constructor.
     * 
     * @param aRect Frame rectangle for container.
     */
    void ConstructL( const TRect& aRect );

    /**
     * Destructor.
     */
    ~CMPXEmbeddedPlaybackViewContainer();


    /**
     * Set layout for all elements and set sizes for icons.
     *
     * @since S60 3.0
     */
    void UpdateLayout();

// from base class CMPXCommonPlaybackViewContainer
    
    /**
     * Sets main pane mode.
     *
     * @since S60 3.1
     * @param aMode Playback mode.
     */
    void SetMode( TMPXPbvPlaybackMode aMode );
    
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
    
private:

    CMPXCommonUiHelper* iCommonUiHelper; // not owned
    };

#endif  // C_CMPXEMBEDDEDPLAYBACKVIEWCONTAINER_H

// End of File
