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
* Description:  An interface providing skin and layout data for 
*                Progressive Download playback view.
*
*/


#ifndef C_CMPXPDPLAYBACKVIEWLAYOUT_H
#define C_CMPXPDPLAYBACKVIEWLAYOUT_H

//  INCLUDES
#include "mpxcommonplaybackviewlayout.h"

// CLASS DECLARATION

/**
*  An interface providing skin and layout data for playback view.
*
*  @lib mpxpdplaybackview.dll
*  @since S60 v3.0
*/
NONSHARABLE_CLASS( CMPXPdPlaybackViewLayout ) : public CMPXCommonPlaybackViewLayout
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdPlaybackViewLayout* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    static CMPXPdPlaybackViewLayout* NewLC();

    /**
     * Destructor.
     */
    ~CMPXPdPlaybackViewLayout();
       
private:

    /**
     * C++ default constructor.
     */
    CMPXPdPlaybackViewLayout();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif  // C_CMPXPDPLAYBACKVIEWLAYOUT_H

// End of file
