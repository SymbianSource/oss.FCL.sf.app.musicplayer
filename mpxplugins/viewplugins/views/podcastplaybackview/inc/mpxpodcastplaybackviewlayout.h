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
* Description:  An interface providing skin and layout data for podcast playback view.
*
*/


#ifndef C_CMPXPODCASTPLAYBACKVIEWLAYOUT_H
#define C_CMPXPODCASTPLAYBACKVIEWLAYOUT_H

//  INCLUDES
#include "mpxcommonplaybackviewlayout.h"

// CLASS DECLARATION

/**
*  An interface providing skin and layout data for podcast playback view.
*
*  @lib mpxpodcastplaybackview.dll
*  @since S60 v3.0
*/
NONSHARABLE_CLASS( CMPXPodcastPlaybackViewLayout ) : public CMPXCommonPlaybackViewLayout
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastPlaybackViewLayout* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPodcastPlaybackViewLayout* NewLC();

    /**
     * Destructor.
     */
    ~CMPXPodcastPlaybackViewLayout();

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

private:

    /**
     * C++ default constructor.
     */
    CMPXPodcastPlaybackViewLayout();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_CMPXPODCASTPLAYBACKVIEWLAYOUT_H

// End of file
