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
* Description:  MPX common container HG Texture manager.
*
*/



#ifndef CMPXCOMMONCONTAINERTEXTUREMANAGER2_H
#define CMPXCOMMONCONTAINERTEXTUREMANAGER2_H


// INCLUDES
#include <e32base.h>
#include <mpxitemid.h>
#include <gulicon.h>
#include <e32hashtab.h>

#include "mpxcommoncontainer.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
 *  Texture manager for common container.
 *
 *  @lib mpxcommoncontainer.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CMPXCommonContainerTextureManager2 )
    : public CBase
    {
public:
    
    /**
     * default C++ constructor.
     */
    CMPXCommonContainerTextureManager2();
    
    /**
     * Destructor.
     */
    virtual ~CMPXCommonContainerTextureManager2();

    /**
     * Creates texture from the specified bitmap.
     *
     * @param aIndex container item index
     * @param aItemId container item id
     * @param aBitmap a bitmap
     */
	void CreateTextureL( TInt aIndex, TMPXItemId aItemId, CFbsBitmap* bitmap );
	    
	    
    /**
     * Returns texture for the specified item
     *
     * @param aItemId container item id
     * @return ETrue for the item, EFalse if non-existent
     */
	TBool TextureExist( TMPXItemId aItemId ) const;

    /**
     * Called with array index when a bitmap is needed
     *
     * @param aId container item id
     */
    CFbsBitmap* ProvideBitmapL( TInt aIndex );

    /**
     * Removes the cached texture information
     *
     */
    void ClearCache();
    
    
private: // data
    
    typedef RHashMap<TInt, CFbsBitmap*> RTextureMap;
    RTextureMap  iTextureMap;
    typedef RHashMap<TInt32, TInt> RTextureIndex;
    RTextureIndex iTextureIndexMap;
    };

#endif  // CMPXCOMMONCONTAINERTEXTUREMANAGER2_H

// End of File
