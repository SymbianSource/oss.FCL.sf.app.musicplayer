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
* Description:  An interface providing skin and layout data for embedded playback view.
*
*/


#ifndef C_CMPXEMBEDDEDPLAYBACKVIEWLAYOUT_H
#define C_CMPXEMBEDDEDPLAYBACKVIEWLAYOUT_H

//  INCLUDES
#include "mpxcommonuihelper.h"
#include "mpxcommonplaybackviewlayout.h"

// CLASS DECLARATION

/**
*  An interface providing skin and layout data for playback view.
*
*  @lib mpxplaybackview.dll
*  @since S60 v3.0
*/
NONSHARABLE_CLASS( CMPXEmbeddedPlaybackViewLayout ) : public CMPXCommonPlaybackViewLayout
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXEmbeddedPlaybackViewLayout* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXEmbeddedPlaybackViewLayout* NewLC();

    /**
     * Destructor.
     */
    ~CMPXEmbeddedPlaybackViewLayout();

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
    CMPXEmbeddedPlaybackViewLayout();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
private: // data

    CMPXCommonUiHelper* iCommonUiHelper;    // own    
    };

#endif  // C_CMPXEMBEDDEDPLAYBACKVIEWLAYOUT_H

// End of file
