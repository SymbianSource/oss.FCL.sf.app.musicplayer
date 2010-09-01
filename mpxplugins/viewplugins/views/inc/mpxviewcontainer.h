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
* Description:  MPX View container definition.
*
*/



#ifndef MMPXVIEWCONTAINER_H
#define MMPXVIEWCONTAINER_H


// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CEikMenuPane;

// CLASS DECLARATION

/**
 *  Abstract class for view container.
 *  The view container class owns a common container to access List box
 *  commands, receive CoeControl events and GetHelpContext(). The view 
 *  container class should implement the following virtual functions:
 *
 *  virtual void HandleKeyEvent();
 *  virtual void HandleHelpContent();
 *
 *  common container uses this to pass CoeControl events back to the view
 *  container class. 
 *
 *  The view container class should implement the following virtual function
 *  from MEikCommandObserver 
 *
 *  virtual void ProcessCommandL(TInt aCommandId);
 *
 *  common container uses this to pass commands back to the view class
 *  the commands supported are defined in mpxcommoncontainer.hrh
 *
 *  @since S60 v3.2
 */
class MMPXViewContainer
    {
public:

    /**
     * Destructor.
     */
    virtual ~MMPXViewContainer() {};

    /**
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    virtual TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType ) = 0;
    
    /**
     * Handles the retrieval of the view container's help context.
     *
     * @param aContext The control's help context.
     */
    virtual void HandleHelpContext( TCoeHelpContext& aContext ) const = 0;

    /**
     * Returns the indicators for the specified item within the view container
     *
     * @param aIndex specified array index
     * @return Indicator icon indices
     */
    virtual RArray<TInt> IndicatorsL( TInt aIndex ) = 0;
    };

#endif  // MMPXVIEWCONTAINER_H

// End of File
