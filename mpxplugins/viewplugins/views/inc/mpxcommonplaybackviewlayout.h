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


#ifndef CMPXCOMMONPLAYBACKVIEWLAYOUT_H
#define CMPXCOMMONPLAYBACKVIEWLAYOUT_H

//  INCLUDES
#include <e32base.h>
#include <AknUtils.h>

#include "mpxplaybackviewlayoutinterface.h"
#include "mpxcommonplaybackviewlayout.h"
#include "mpxcommonplaybackviewdefs.h"

// CLASS DECLARATION
class MAknsSkinInstance;
class TAknsItemID;

/**
*  An interface providing skin and layout data for playback view.
*
*  @lib mpxplaybackview.dll
*  @since S60 v3.0
*/
class CMPXCommonPlaybackViewLayout : public CBase,
                                     public MMPXPlaybackViewLayout
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
    IMPORT_C virtual TRect ButtonLayout(
        const TRect& aParentRect );

    /**
     * Get layout data for an graphical indicator.
     *
     * @since 3.0
     * @param aParentRect Parent control's rect
     * @param  aIndicator Enumeration value identifying the indicator
     * @return Layout data
     */
    IMPORT_C virtual TRect IndicatorLayout(
        const TRect& aParentRect,
        TMPXPbvIndicator aIndicator );

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
    IMPORT_C virtual void LayoutLabel(
        CEikLabel* aLabel,
        const TRect& aParentRect,
        TMPXPbvTextIndicator aText,
        TInt aOffsetx=0,
        TInt aOffsety=0 );

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
    IMPORT_C virtual void GetButtonBitmapsL(
        TMPXPbvButton aButton,
        TInt aButtonState,
        CFbsBitmap*& aBitmap,
        CFbsBitmap*& aMask );

    /**
     * Get the bitmap and mask for a given indicator.
     *
     * @since 3.0
     * @param  aIndicator   Enumeration value identifying the indicator
     * @return CGulIcon     Indicator graphic and mask. Ownership
     *                      transferred to caller.
     */
    IMPORT_C virtual CGulIcon* GetIndicatorIconMaskL(
        TMPXPbvIndicator aIndicator );

protected:

    /**
     * Create a color-skinned icon.
     *
     * @param aSkin Pointer to the current skin instance.
     * @param aIconSkinId Item ID of the masked bitmap to be created.
     * @param aColorId Item ID of the color table.
     * @param aColorIndex Index in the color table.
     * @param aBitmapFilename Filename to be used to construct the item.
     * @param aFileBitmapId ID of the bitmap in the file.
     * @param aFileMaskId ID of the mask in the file.
     */
    IMPORT_C CGulIcon* CreateColorIconL(
        MAknsSkinInstance* aSkin,
        const TAknsItemID& aIconSkinId,
        const TAknsItemID& aColorId,
        TInt aColorIndex,
        const TDesC& aBitmapFilename,
        TInt aFileBitmapId,
        TInt aFileMaskId );

    /**
     * Layouts the label by reading in the resource values
     *
     * @param aLabel      Label to set the layout to
     * @param aParentRect Parent rectangle for layout
     * @param aResId      Resource ID to read from
     * @param aOffsetx    Offset in x direction to apply to label
     * @param aOffsety    Offset in y direction to apply to label
     */
    IMPORT_C void DoLayoutLabelL(
        CEikLabel* aLabel,
        const TRect& aParentRect,
        TInt aResId,
        TInt aOffsetx,
        TInt aOffsety );

public:
	/**
     * Get layout data for a button.
     * @since 5.0
     * @param  aButton Enumeration value identifying the button,
     * @return Layout data
     */
    IMPORT_C virtual TRect ButtonLayout(const TRect& aParentRect,
                                  TInt aButton);

protected:
	/**
     * Determine avkon layout variant
     * @since 5.0
     * @return layout variant type
     */
    IMPORT_C TMPXPbvLayoutVariants DetermineLayoutVariant();

	/**
     * Get layout data for a text item
     * @since 5.0
     * @param  aText Enumeration value identifying the text
     * @return layout variant id
     */
    IMPORT_C TMPXPbvLayoutVariants GetTextVariant(TMPXPbvTextIndicator aText);

	/**
     * Get layout data for an indicator item
     * @since 5.0
     * @param  aIndicator Enumeration value identifying the indicator
     * @return Layout variant id
     */
    IMPORT_C TMPXPbvLayoutVariants GetIndicatorVariant(TMPXPbvIndicator aIndicator);

    };

#endif  // CMPXCOMMONPLAYBACKVIEWLAYOUT_H

// End of file
