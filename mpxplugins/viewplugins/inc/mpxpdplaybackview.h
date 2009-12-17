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
* Description:  MPX Progressive Download playback view implementation
*
*/



#ifndef C_CMPXPDPLAYBACKVIEW_H
#define C_CMPXPDPLAYBACKVIEW_H


// INCLUDES
#include "mpxcommonplaybackviewimp.h"

// CLASS DECLARATION

/**
 *  MPX Progressive Download playback view.
 *
 *  @lib mpxpdplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXPdPlaybackView ) : public CMPXCommonPlaybackViewImp
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXPdPlaybackView* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXPdPlaybackView* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXPdPlaybackView();
    };

#endif  // C_CMPXPDPLAYBACKVIEW_H

// End of File
