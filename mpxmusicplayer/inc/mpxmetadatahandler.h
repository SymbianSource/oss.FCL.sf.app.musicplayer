/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MPX metadata handler interface definition
*
*/



#ifndef C_MMPXMETADATAHANDLER_H
#define C_MMPXMETADATAHANDLER_H


// INCLUDES
#include <e32base.h>
#include <remconinterfaceselector.h>

// DATA TYPES

// CONSTANTS

// CLASS DECLARATION

/**
 *  MPX metadata handler interface.
 *
 *  @lib mpxmetadatahandler.lib
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MMPXMetaDataHandler )
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aFlags Flags for creating popups.
     * @param aObserver Media key command observer
     * @return Pointer to newly created object.
     */
    IMPORT_C static MMPXMetaDataHandler* NewL(CRemConInterfaceSelector &aInterfaceSelector);

    /**
     * Destructor.
     */
    virtual ~MMPXMetaDataHandler();
    };

#endif  // C_MMPXMETADATAHANDLER_H

// End of File
