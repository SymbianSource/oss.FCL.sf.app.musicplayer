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
* Description:  An interface providing skin and layout data for common playback view.
*
*/


#ifndef MMPXCOMMONPLAYBACKVIEWLAYOUT_H
#define MMPXCOMMONPLAYBACKVIEWLAYOUT_H

//  INCLUDES
#include <e32base.h>
#include <AknUtils.h>
#include "mpxcommonplaybackviewdefs.h"

// CLASS DECLARATION

/**
*  An interface providing skin and layout data for playback view.
*
*  @lib mpxplaybackview.dll
*  @since S60 v3.0
*/
NONSHARABLE_CLASS( MMPXPlaybackViewLayout )
    {
public:

    /**
     * Get layout data for a button.
     *
     * @since 3.0
     * @param aParentRect Parent control's rect
     * @param aButton Enumeration value identifying the button
     * @return Layout data
     */
    virtual TRect ButtonLayout(
        const TRect& aParentRect )=0;

    /**
     * Get layout data for an graphical indicator.
     *
     * @since 3.0
     * @param aParentRect Parent control's rect
     * @param  aIndicator Enumeration value identifying the indicator
     * @return Layout data
     */
    virtual TRect IndicatorLayout(
        const TRect& aParentRect,
        TMPXPbvIndicator aIndicator )=0;

    /**
     * Set layout data for a text label.
     *
     * @since 3.0
     * @param aLabel      Label to set the layout to
     * @param aParentRect Parent rectangle for layout
     * @param aIndicator  Enumeration value identifying the indicator,
     * @param aOffsetx    Offset in x direction to apply to label
     * @param aOffsety    Offset in y direction to apply to label
     */
    virtual void LayoutLabel(
        CEikLabel* aLabel, 
        const TRect& aParentRect, 
        TMPXPbvTextIndicator aText,
        TInt aOffsetx=0,
        TInt aOffsety=0 )=0;

    /**
     * Get the bitmap and mask for a soft button in a given state.
     *
     * @since 3.0
     * @param  aButton      Enumeration value identifying the button.
     * @param  aButtonState State of the button
     * @param  aBitmap      Reference to a CFbsBitmap pointer to receive
     *                      the requested bitmap pointer. Ownership is
     *                      transferred to the caller.
     * @param  aMask        Reference to a CFbsBitmap pointer to receive
     *                      the mask of the requested bitmap. Ownership is
     *                      transferred to the caller.
     */
    virtual void GetButtonBitmapsL(
        TMPXPbvButton aButton,
        TInt aButtonState,
        CFbsBitmap*& aBitmap,
        CFbsBitmap*& aMask )=0;

    /**
     * Get the bitmap and mask for a given indicator.
     *
     * @since 3.0
     * @param  aIndicator   Enumeration value identifying the indicator
     * @return CGulIcon     Indicator graphic and mask. Ownership 
     *                      transferred to caller.
     */
    virtual CGulIcon* GetIndicatorIconMaskL(
        TMPXPbvIndicator aIndicator )=0;
        
    virtual TRect ButtonLayout(const TRect& aParentRect,
                                  TInt aButton)=0;

    };

#endif  // C_CMPXCOMMONPLAYBACKVIEWLAYOUT_H

// End of file
