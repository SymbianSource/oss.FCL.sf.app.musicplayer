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



#ifndef C_CMPXPDSBPLAYBACKVIEW_H
#define C_CMPXPDSBPLAYBACKVIEW_H


// INCLUDES
#include "mpxcommonplaybackviewimp.h"

// CLASS DECLARATION

/**
 *  MPX Progressive Download playback view.
 *
 *  @lib mpxpdsbplaybackview.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMPXPdSbPlaybackView ) : public CMPXCommonPlaybackViewImp
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXPdSbPlaybackView* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXPdSbPlaybackView* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXPdSbPlaybackView();
    };

#endif  // C_CMPXPDSBPLAYBACKVIEW_H

// End of File
