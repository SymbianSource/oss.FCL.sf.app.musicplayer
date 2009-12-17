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
* Description:  MPX Equalizer view implementation
*
*/



#ifndef C_CMPXEQUALIZERVIEW_H
#define C_CMPXEQUALIZERVIEW_H


//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 *  MPX Equalizer view.
 *
 *  @lib mpxequalizerview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXEqualizerView ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXEqualizerView* NewL();

    /**
     * Destructor.
     */
    virtual ~CMPXEqualizerView();

    /**
     * Launch equalizer dialog.
     */
    virtual void LaunchEqualizerL() = 0;
    };

#endif  // C_CMPXEQUALIZERVIEW_H

// End of File
