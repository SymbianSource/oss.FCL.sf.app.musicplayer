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
* Description:  MPX common container Alf Texture manager.
*
*/



#ifndef CMPXCOMMONCONTAINERTEXTUREMANAGER_H
#define CMPXCOMMONCONTAINERTEXTUREMANAGER_H


// INCLUDES
#include <e32base.h>
#include <alf/alfbitmapprovider.h>
#include <mpxitemid.h>
#include "mpxcommoncontainer.h"

// CONSTANTS
const TInt KMPXFirstAlfThumbnailTextureId = 0x20000001;
const TInt KMPXLastAlfThumbnailTextureId = 0x30000000;

// FORWARD DECLARATIONS
class CFbsBitmap;
class CAlfTexture;
class CAlfTextureManager;

// CLASS DECLARATION

/**
 *  Texture manager for common container.
 *
 *  @lib mpxcommoncontainer.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CMPXCommonContainerTextureManager )
    : public CBase
    , public MAlfBitmapProvider
    {
public:
    
    enum TMPXTextureType
        {
        EMPXTextureNone,
        EMPXTextureImage,
        EMPXTextureIcon
        };
    
    /**
     * default C++ constructor.
     */
    CMPXCommonContainerTextureManager(
        CAlfTextureManager& aTextureManager );
    
    /**
     * Destructor.
     */
    virtual ~CMPXCommonContainerTextureManager();

    /**
     * Creates texture from the specified bitmap and mask
     *
     * @param aIndex container item index
     * @param aItemId container item id
     * @param aBitmap a bitmap
     * @param aMask a bitmap mask
     */
	CAlfTexture& CreateTextureL( TInt aIndex, TMPXItemId aItemId,
	    CFbsBitmap* aBitmap, CFbsBitmap* aMask );
	    
    /**
     * Creates icon texture from the specified bitmap and mask
     * (not cached)
     *
     * @param aBitmap a bitmap
     * @param aMask a bitmap mask
     */
	CAlfTexture& CreateIconTextureL( TInt aIconIndex, CFbsBitmap* aBitmap, CFbsBitmap* aMask );
	    
    /**
     * Returns texture for the specified item
     *
     * @param aItemId container item id
     * @return Texture Id for the item, KErrNotFound if non-existent
     */
	TInt TextureId( TMPXItemId aItemId ) const;

    /**
     * Returns texture for the specified icon
     *
     * @param aIconIndex container item id
     * @return Texture Id for the item, KErrNotFound if non-existent
     */
	TInt IconTextureId( TInt aIconIndex ) const;

    /**
     * Removes the cached texture information
     *
     * @param aId container item id
     */
    void ClearCache( TMPXTextureType aType=EMPXTextureNone );
    
private:

// from base class MAlfBitmapProvider

    /**
     * Callback with texture id when a texture is created
     *
     * @param aId container item id
     * @param aBitmap a bitmap
     * @param aMask a bitmap mask
     */
    void ProvideBitmapL( TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aBitmapMask);
    
private: // data
    
    struct TMPXTextureItem
        {
        TMPXTextureType iType;
        TInt            iItemIndex;
        TMPXItemId      iItemId;
        TInt            iTextureId;
        CFbsBitmap*     iBitmap;
        CFbsBitmap*     iMask;
        };

    CAlfTextureManager&         iTextureManager;    // not owned
    MMPXCommonContainer*        iCommonContainer;   // not owned
    TMPXTextureItem             iTextureItem;
    RArray<TMPXTextureItem>     iTextureArray;      // owned
    RArray<TMPXTextureItem>     iIconTextureArray;  // owned
    };

#endif  // CMPXCOMMONCONTAINERTEXTUREMANAGER_H

// End of File
