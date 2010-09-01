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
* Description:  MPX Audio effects view implementation
*
*/



#ifndef C_CMPXAUDIOEFFECTSVIEW_H
#define C_CMPXAUDIOEFFECTSVIEW_H


// INCLUDES
#include <aknview.h>


// CLASS DECLARATION

/**
 *  MPX Audio effects view.
 *
 *  @lib mpxaudioeffectsview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXAudioEffectsView ) : public CAknView
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXAudioEffectsView* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXAudioEffectsView* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXAudioEffectsView();
    };

#endif  // C_CMPXAUDIOEFFECTSVIEW_H

// End of File
