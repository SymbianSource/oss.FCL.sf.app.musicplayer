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
* Description:  Text label for Common Playback view.
*
*/



#ifndef CMPXPLAYBACKVIEWINFOLABEL_H
#define CMPXPLAYBACKVIEWINFOLABEL_H


//  INCLUDES
#include <eiklabel.h>


// FORWARD DECLARATIONS
class MAknsControlContext;
class CAknMarqueeControl;


// CLASS DECLARATION

/**
 *  Text label for Playback View. Otherwise similar to CEikLabel, but also 
 *  handles background bitmap drawing and marquee scrolling.
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPlaybackViewInfoLabel ) : public CEikLabel
    {
public:

    /**
     * C++ constructor.
     */
    CMPXPlaybackViewInfoLabel();
    
    /**
     * Symbian 2nd phase constructor
     *
     * @param aEnableMarqueeSupport If true, CAknMarqueeControl is also
     *                              created to support marquee.
     */
    void ConstructL( TBool aEnableMarqueeSupport );
    
    /**
     * Destructor
     */
    virtual ~CMPXPlaybackViewInfoLabel();
    
    /**
     * Set background bitmap
     *
     * @since S60 3.0
     * @param aBackground       Pointer to the skin background control
     *                          context. Ownership not transferred.
     */
    void SetBackground( MAknsControlContext* aBackground );

    /**
     * Redraw rectangle
     *
     * @since S60 3.0
     * @param aRect Dirty rectangle. Drawing can be clipped to this
     *              rectangle
     */
    void RedrawRect( const TRect& aRect ) const;

    /**
     * Start marquee scrolling if supported and needed.
     *
     * @since S60 3.0
     */
    void StartMarquee();

    /**
     * Stop marquee scrolling.
     *
     * @since S60 3.0
     */
    void StopMarquee();

    /**
     * Get the full unclipped text for the label
     *
     * @since S60 3.0
     * @return Reference to label text. Must not be stored. 
     */
    const TDesC& FullText() const;

    /**
     * Set label text and clip if necessary.
     *
     * @since S60 3.0
     * @param aText Text for the label
     */
    void SetLabelTextL( const TDesC& aText );

private:

    /**
     * Check if scrolling is needed.
     *
     * @return ETrue if marquee is necessary (ie. text length is larger
     *               that what can be visible at one time.
     */
    TBool NeedToScroll() const;

    /**
     *  Redraw callback for marquee control
     */
    static TInt RedrawCallback( TAny* aPtr );

// from base class CCoeControl

    /**
     * From CCoeControl.
     * Draws the control.
     *
     * @param aRect The rectangular region of the control to be drawn.
     */
    void Draw( const TRect& aRect ) const;

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

private:    // Data

    MAknsControlContext* iBackground; // not owned
    CAknMarqueeControl* iMarquee; // owned
    HBufC* iFullText; // owned
    };

#endif  // CMPXPLAYBACKVIEWINFOLABEL_H

// End of File
