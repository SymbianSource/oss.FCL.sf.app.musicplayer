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
* Description:  MPX Collection view implementation
*
*/



#ifndef CMPXCOLLECTIONVIEW_H
#define CMPXCOLLECTIONVIEW_H


// INCLUDES
#include <aknview.h>


// CLASS DECLARATION

/**
 *  MPX Collection view.
 *
 *  @lib mpxcollectionview.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXCollectionView ) : public CAknView
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since 3.1
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXCollectionView* NewL();

    /**
     * Two-phased constructor.
     *
     * @since 3.1
     * @return Pointer to newly created object.
     */
    IMPORT_C static CMPXCollectionView* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMPXCollectionView();
    };

#endif  // CMPXCOLLECTIONVIEW_H

// End of File
