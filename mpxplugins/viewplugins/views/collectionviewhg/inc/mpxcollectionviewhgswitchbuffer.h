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
* Description: Defines a container for buffer switch handling.
*
*/

#ifndef MPXCOLLECTIONVIEWHGSWITCHBUFFER_H_
#define MPXCOLLECTIONVIEWHGSWITCHBUFFER_H_

#include <e32base.h>

class CHgScroller;
class CHgVgMediaWall;
class CGulIcon;

NONSHARABLE_CLASS(CMPXCollectionViewHgSwitchBuffer) : public CBase
    {
public:

    static CMPXCollectionViewHgSwitchBuffer* CreateBufferLC( CHgScroller& aScroller );

    static CMPXCollectionViewHgSwitchBuffer* CreateBufferLC( CHgVgMediaWall& aMediaWall );

    virtual ~CMPXCollectionViewHgSwitchBuffer();

public:
    void FillFromBufferL( CHgVgMediaWall& aMediaWall );
    void FillFromBufferL( CHgScroller& aScroller, TSize aSize );
    void SetIndexOffset( TInt aOffset );

protected:
    CMPXCollectionViewHgSwitchBuffer();

protected:
    void AddToBufferL( CGulIcon& aIcon, TInt aIndex );

private:
    NONSHARABLE_CLASS( CBufferItem ) : public CBase
        {
    public:
        CBufferItem(CGulIcon* aIcon, TInt aIndex)
        : iIcon( aIcon ), iIndex( aIndex )
            {

            }

        virtual ~CBufferItem()
            {
            delete iIcon;
            iIcon = NULL;
            }

        /**
         *
         */
        CGulIcon* GetIcon()
            {
            CGulIcon* icon = iIcon;
            iIcon = NULL;
            return icon;
            }

        /**
         *
         */
        TInt Index()
            {
            return iIndex;
            }

        private:
        CGulIcon* iIcon;
        TInt iIndex;
        };

    RPointerArray<CBufferItem> iBuffer;
    TInt iOffset;
    };


#endif /* MPXCOLLECTIONVIEWHGSWITCHBUFFER_H_ */
