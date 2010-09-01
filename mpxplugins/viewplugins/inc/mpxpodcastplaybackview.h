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
* Description:  MPX podcast playback view implementation
*
*/



#ifndef C_CMPXPODCASTPLAYBACKVIEW_H
#define C_CMPXPODCASTPLAYBACKVIEW_H


// INCLUDES
#include "mpxcommonplaybackviewimp.h"

// CLASS DECLARATION

/**
 *  MPX podcast playback view.
 *
 *  @lib mpxpodcastplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPodcastPlaybackView ) : public CMPXCommonPlaybackViewImp
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXPodcastPlaybackView* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXPodcastPlaybackView* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXPodcastPlaybackView();
    };

#endif  // C_CMPXPODCASTPLAYBACKVIEW_H

// End of File
