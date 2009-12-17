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
* Description:  Container class.
*
*/



#ifndef MPXAUDIOEFFECTSVIEWCONTAINER_H
#define MPXAUDIOEFFECTSVIEWCONTAINER_H


// INCLUDES
#include <coecntrl.h>


// FORWARD DECLARATIONS
class CMPXAudioEffectsItems;
class CMPXAudioEffectsModel;
class CMPXAudioEffectsViewImp;


/**
*
* @class    CMPXAudioEffectsViewContainer 
* @brief    This is the container class for a list example based on the
* standard Symbian OS architecture.  
* 
*/
class CMPXAudioEffectsViewContainer : public CCoeControl
    {
public:

    /**
     * C++ default constructor.
     */
    CMPXAudioEffectsViewContainer();

    /**
    * Two-phased constructor
    */
    void ConstructL(
        const TRect& aRect,
        CMPXAudioEffectsModel* aModel );

    /**
    * Destructor
    */
    ~CMPXAudioEffectsViewContainer();

    /**
    * Change currently selected item
    */
    void ChangeSelectedItemL();

private: // from CoeControl
    
    /**
    * From CCoeControl
    * Called by the framework in compound controls
    * @return The number of controls in this CMPXAudioEffectContainer
    */
    TInt CountComponentControls() const;
    
    /**
    * From CCoeControl
    * Called by the framework in compound controls  
    * @param The index of the control to return
    * @return The control for aIndex
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
    /**
    * From CCoeControl
    * Called by the framework whenever a key event occurs.  
    * Passes the key event to the saved games list if it is not null, 
    * otherwise returns EKeyWasNotConsumed
    *
    * @param aKeyEvent the Key event which occured, e.g. select key pressed
    * @param aType the type of Key event which occurred, e.g. key up, key
    *              down
    * @return TKeyResponse EKeyWasNotConsumed if the key was not processed,
    *         EKeyWasConsumed if it was
    */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * From CCoeControl.
     * Responds to changes to the size and position of the contents of this 
     * control.
     */
    void SizeChanged();

    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );

    /**
    * From CCoeControl
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

private: //data

    CMPXAudioEffectsModel* iModel;  // Not own
    CMPXAudioEffectsItems* iSettingItems; // Own. The settings list
    };

#endif

// End of File
