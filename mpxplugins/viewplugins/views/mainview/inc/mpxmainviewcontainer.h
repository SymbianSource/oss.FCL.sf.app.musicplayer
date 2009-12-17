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
* Description:  MPX main view's container implementation.
*
*/



#ifndef C_CMPXMAINVIEWCONTAINER_H
#define C_CMPXMAINVIEWCONTAINER_H


// INCLUDES
#include <coecntrl.h>


// FORWARD DECLARATIONS
class CMPXMainViewImp;
class CEikTextListBox;
class CMPXLbxExtendedFeatures;
class CAknIconArray;

// CLASS DECLARATION

/**
 *  Container class for main view.
 *
 *  @lib mpxmainview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXMainViewContainer ) : public CCoeControl
    {
public:

    /**
     * C++ default constructor.
     *
     * @param aView              Parent view.
     */
    CMPXMainViewContainer(
            CMPXMainViewImp* aView );

    /**
     * Symbian 2nd phase constructor.
     * 
     * @param aRect Frame rectangle for container.
     */
    void ConstructL( const TRect& aRect );

    /**
     * Destructor.
     */
    virtual ~CMPXMainViewContainer();

    /**
     * Set layout for all elements and set sizes for icons.
     *
     * @since S60 3.0
     */
    void UpdateLayout();    

    /**    
     *  Returns a pointer to the listbox
     */
    CEikTextListBox* ListBox();    
        
private:
    /**
     * Create list box 
     *
     */
    void CreateListBoxL();
    
    /**
     * Load an icon and append it to an icon array.
     * @param aArray    pointer to the icon array
     * @param aID       skin id of the icon
     * @param aColorId  Item ID of the color table.
     * @param aColorIndex Index in the color table.
     * @param aMbmFile  path to the mbm/mif file containing the icon
     * @param aBitmapId mbm id of the bitmap
     * @param aMaskId   mbm id of the mask
     */
    void AppendIconToArrayL(
        CAknIconArray* aArray,
        const TAknsItemID& aID,
        const TAknsItemID& aColorId,
        TInt aColorIndex,
        const TDesC& aMbmFile,
        TInt aBitmapId,
        TInt aMaskId );

// from base class CoeControl

    /**
     * From CCoeControl.
     * Responds to changes to the size and position of the contents of this 
     * control.
     */
    void SizeChanged();

    /**
     * From CCoeControl.
     * Gets the number of controls contained in a compound control.
     *
     * @return The number of component controls contained by this control.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl.
     * Gets an indexed component of a compound control.
     *
     * @param aIndex The index of the control.
     * @return The component control with an index of aIndex.
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;    

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
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );

    /**
     * From CoeControl.
     * Gets the control's help context.
     *
     * @param aContext The control's help context.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

 private:   // data

    CMPXMainViewImp*            iView;          // not owned
    CEikTextListBox*            iListBox;       // owned
    };

#endif  // C_CMPXMAINVIEWCONTAINER_H

// End of File
