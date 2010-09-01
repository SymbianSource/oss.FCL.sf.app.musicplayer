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
* Description:  MPX embedded playback view implementation
*
*/



#ifndef C_CMPXEMBEDDEDPLAYBACKVIEW_H
#define C_CMPXEMBEDDEDPLAYBACKVIEW_H


// INCLUDES
#include "mpxcommonplaybackviewimp.h"

// CLASS DECLARATION

/**
 *  MPX embedded playback view.
 *
 *  @lib mpxembeddedplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXEmbeddedPlaybackView ) : public CMPXCommonPlaybackViewImp
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXEmbeddedPlaybackView* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXEmbeddedPlaybackView* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXEmbeddedPlaybackView();
    };

#endif  // C_CMPXEMBEDDEDPLAYBACKVIEW_H

// End of File
