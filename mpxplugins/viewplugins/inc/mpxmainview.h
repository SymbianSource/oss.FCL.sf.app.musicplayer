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
* Description:  MPX main view implementation
*
*/



#ifndef C_CMPXMAINVIEW_H
#define C_CMPXMAINVIEW_H


// INCLUDES
#include <aknview.h>


// CLASS DECLARATION

/**
 *  MPX main view.
 *
 *  @lib mpxmainview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CMPXMainView ) : public CAknView
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXMainView* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.0
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXMainView* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXMainView();
    };

#endif  // C_CMPXMAINVIEW_H

// End of File
