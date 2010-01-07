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
* Description:  Music Player upnp playback dialog declaration
*
*/


#ifndef C_CMPXUPNPPLAYBACKDIALOGCUSTOMCONTROL_H
#define C_CMPXUPNPPLAYBACKDIALOGCUSTOMCONTROL_H


// INCLUDES
#include <coecntrl.h>
#include "mpxcommonplaybackviewcontainer.h"

// FORWARD DECLARATIONS
class CMPXUPnPPlaybackDialog;
class CAknsBasicBackgroundControlContext;
class MMPXLayoutSwitchObserver;
class MMPXPlaybackViewLayout;

// CLASS DECLARATION

/**
 * The custom control for UPnP playback Dialog.
 */
NONSHARABLE_CLASS( CMPXUPnPPlaybackDialogCustomControl ) : public CMPXCommonPlaybackViewContainer
    {
public:           

public:  // Constructors and destructor

    /**
     * Two-phased constructor. Leaves on failure. Places the instance on the
     * cleanup stack.
     * @param aParent Parent control.
     * @param aRect The control's extent.
     * @param aCommandObserver  Observer for commands.
     * @param aLayoutObserver   Observer to send layout switch events to.
     * @param aLayout           Pointer to playback view layout.
     * @return The constructed item.
     */
    static CMPXUPnPPlaybackDialogCustomControl* NewLC( const CCoeControl& aParent, 
                                                       const TRect& aRect,
                                                       MEikCommandObserver* aCommandObserver,
                                                       MMPXLayoutSwitchObserver* aLayoutObserver,
                                                       MMPXPlaybackViewLayout* aLayout,
                                                       TMPXPlaybackPlayerType aCurrentPlayerType );

    /**
     * Two-phased constructor. Leaves on failure.
     * @param aParent Parent control.
     * @param aRect The control's extent.
     * @param aCommandObserver  Observer for commands.
     * @param aLayoutObserver   Observer to send layout switch events to.
     * @param aLayout           Pointer to playback view layout.
     * @return The constructed item.
     */
    static CMPXUPnPPlaybackDialogCustomControl* NewL( const CCoeControl& aParent, 
                                                      const TRect& aRect,
                                                      MEikCommandObserver* aCommandObserver,
                                                      MMPXLayoutSwitchObserver* aLayoutObserver,
                                                      MMPXPlaybackViewLayout* aLayout,
                                                      TMPXPlaybackPlayerType aCurrentPlayerType );
    
    /**
     * Destructor.
     */
    virtual ~CMPXUPnPPlaybackDialogCustomControl();       
    

public: // New functions


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
   


private:

    /**
     * C++ default constructor.
     */
    CMPXUPnPPlaybackDialogCustomControl(
                    MEikCommandObserver* aCommandObserver,
                    MMPXLayoutSwitchObserver* aLayoutObserver,
                    MMPXPlaybackViewLayout* aLayout );

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aParent Parent control.
     * @param aRect The control's extent.
     */
    void ConstructL( const CCoeControl& aParent, 
                     const TRect &aRect,
                     TMPXPlaybackPlayerType aCurrentPlayerType );
    
private:    // data
   
    CAknsBasicBackgroundControlContext* iBackground;
        
    CGulIcon* iRemotePlayerIcon;    // owned
    TRect iRemotePlayerIconRect;
    TBool iRemotePlayerUsed;
    };

#endif      // C_CMPXUPNPPLAYBACKDIALOGCUSTOMCONTROL_H 

// End of File
