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
* Description:  An interface providing skin and layout data for upnp playback view.
*
*/


#ifndef C_CMPXUPNPPLAYBACKVIEWLAYOUT_H
#define C_CMPXUPNPPLAYBACKVIEWLAYOUT_H

//  INCLUDES
#include "mpxcommonplaybackviewlayout.h"

// CLASS DECLARATION

/**
*  An interface providing skin and layout data for playback view.
*
*  @lib mpxplaybackview.dll
*  @since S60 v3.0
*/
NONSHARABLE_CLASS( CMPXUPnPPlaybackViewLayout ) : public CMPXCommonPlaybackViewLayout
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXUPnPPlaybackViewLayout* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXUPnPPlaybackViewLayout* NewLC();

    /**
     * Destructor.
     */
    ~CMPXUPnPPlaybackViewLayout();

    /**
     * Get layout data for an graphical indicator.
     *
     * @since 3.0
     * @param aParentRect Parent control's rect
     * @param  aIndicator Enumeration value identifying the indicator
     * @return Layout data
     */
    TRect IndicatorLayout(
        const TRect& aParentRect,
        TMPXPbvIndicator aIndicator );

    /**
     * Get the bitmap and mask for a given indicator.
     *
     * @since 3.0
     * @param  aIndicator   Enumeration value identifying the indicator
     * @return CGulIcon     Indicator graphic and mask. Ownership 
     *                      transferred to caller.
     */
    CGulIcon* GetIndicatorIconMaskL(
        TMPXPbvIndicator aIndicator );

    /**
     * Sets whether we should use podcast or normal playback album art icon
     *
     * @since 3.1
     * @param  aPodcastAlbumArt true if we're playing a podcast
     * @return void
     */
        
    void SetAlbumArtLayout(TBool aPodcastAlbumArt);

private:

    /**
     * C++ default constructor.
     */
    CMPXUPnPPlaybackViewLayout();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
private:    // Data

    TBool iPodcastAlbumArt;
    };

#endif  // C_CMPXUPNPPLAYBACKVIEWLAYOUT_H

// End of file
