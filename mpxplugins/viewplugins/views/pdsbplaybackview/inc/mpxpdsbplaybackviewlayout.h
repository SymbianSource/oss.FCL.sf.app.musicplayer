/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An interface providing skin and layout data for
*                Progressive Download playback view.
*
*/


#ifndef C_CMPXPDSBPLAYBACKVIEWLAYOUT_H
#define C_CMPXPDSBPLAYBACKVIEWLAYOUT_H

//  INCLUDES
#include "mpxcommonplaybackviewlayout.h"

// CLASS DECLARATION

/**
*  An interface providing skin and layout data for playback view.
*
*  @lib mpxpdsbplaybackview.dll
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CMPXPdSbPlaybackViewLayout ) : public CMPXCommonPlaybackViewLayout
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdSbPlaybackViewLayout* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdSbPlaybackViewLayout* NewLC();

    /**
     * Destructor.
     */
    ~CMPXPdSbPlaybackViewLayout();

private:

    /**
     * C++ default constructor.
     */
    CMPXPdSbPlaybackViewLayout();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_CMPXPDSBPLAYBACKVIEWLAYOUT_H

// End of file
