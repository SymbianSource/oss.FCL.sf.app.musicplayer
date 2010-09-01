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
* Description:  MPX upnp playback view's container implementation.
*
*/



#ifndef C_CMPXUPNPPLAYBACKVIEWCONTAINER_H
#define C_CMPXUPNPPLAYBACKVIEWCONTAINER_H


// INCLUDES
#include "mpxcommonplaybackviewcontainer.h"


// FORWARD DECLARATIONS
class MMPXLayoutSwitchObserver;
class MMPXPlaybackViewLayout;

// CLASS DECLARATION

/**
 *  Container class for audio view.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXUPnPPlaybackViewContainer ) : public CMPXCommonPlaybackViewContainer
    {
public:

    /**
     * C++ default constructor.
     *
     * @param aLayoutObserver    Observer to send layout switch events to.
     * @param aIsEmbedded        ETrue if Music Player is in embedded mode.
     */
    CMPXUPnPPlaybackViewContainer(
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
    ~CMPXUPnPPlaybackViewContainer();

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
    
// from base class MMPXButtonCmdObserver
    
    /**
     * From MMPXButtonCmdObserver
     * Handle a button command.
     * @param aCommand Command id, as specified in the button constructor.
     */
    void HandleButtonCommandL( TInt aCommand );
    
private:    // new methods

    /**
     * Creates icons.
     */
    void CreateIconsL();

    /**
     * Redraw part of the screen to the bitmap context.
     *
     * @param aRect Area to be redrawn.
     *
     */
   void RedrawRect(
        const TRect& aRect) const;
   

    
private:    // data
        
    CGulIcon* iRemotePlayerIcon;    // owned
    TRect iRemotePlayerIconRect;
    TBool iRemotePlayerUsed;
    
    TKeyEvent iLastKeyEvent;
    TEventCode iLastKeyType;    
    };

#endif  // C_CMPXUPNPPLAYBACKVIEWCONTAINER_H

// End of File
