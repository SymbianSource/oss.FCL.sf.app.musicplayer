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
* Description:  MPX Progressive Download playback view's container implementation.
*
*/



#ifndef C_CMPXPDSBPLAYBACKVIEWCONTAINER_H
#define C_CMPXPDSBPLAYBACKVIEWCONTAINER_H


// INCLUDES
#include <mpxplaybackcommanddefs.h>
#include "mpxcommonplaybackviewcontainer.h"


// FORWARD DECLARATIONS
class MMPXLayoutSwitchObserver;
class MMPXPlaybackViewLayout;

// CLASS DECLARATION

/**
 *  Container class for progressive download from browser playback view.
 *
 *  @lib mpxpdsbplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPdSbPlaybackViewContainer ) : public CMPXCommonPlaybackViewContainer
    {
public:

    /**
     * C++ default constructor.
     *
     * @param aLayoutObserver    Observer to send layout switch events to.
     * @param aLayout            Layout
     */
    CMPXPdSbPlaybackViewContainer(
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
    ~CMPXPdSbPlaybackViewContainer();


    /**
     * Handles when the download position has changed
     */
    void HandleDownloadPositionChanged(
        TInt aDownloadedBytes,
        TInt aTotalBytes );

// from MPXCommonPlaybackViewContainer
    /**
     * Redraw part of the screen to the bitmap context.
     *
     * @param aRect Area to be redrawn.
     * @param aGc Graphics context to draw to.
     */
    void RedrawRect(
        const TRect& aRect,
        CBitmapContext& aGc) const;

    /**
     * From MPXCommonPlaybackViewContainer and CCoeControl.
     * Handle events from the touch screen. Overridden from Commonplayback view.
     * PdSbPlaybackview does not utilise pointer events, since EMC doesn't support
     * seeking from progressbar during progressive download.
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);    
    
    /**
     * Sets main pane mode.
     *
     * @since S60 3.0
     * @param aMode Playback mode.
     */
    void SetMode( TMPXPbvPlaybackMode aMode );

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

#endif  // C_CMPXPDSBPLAYBACKVIEWCONTAINER_H

// End of File
