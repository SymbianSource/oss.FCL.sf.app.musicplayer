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
* Description: Implements a container for buffer switch handling.
*
*/


#include "mpxcollectionviewhgswitchbuffer.h"

#include <gulicon.h>

#include <ganes/HgScroller.h>
#include <ganes/HgVgMediaWall.h>
#include <ganes/HgItem.h>
#include <ganes/HgVgItem.h>

// ----------------------------------------------------------------------------
// Create a copy of the icons used from input
// ----------------------------------------------------------------------------
CMPXCollectionViewHgSwitchBuffer* CMPXCollectionViewHgSwitchBuffer::CreateBufferLC( CHgScroller& aScroller )
    {
    CMPXCollectionViewHgSwitchBuffer* buffer = new (ELeave) CMPXCollectionViewHgSwitchBuffer();
    CleanupStack::PushL(buffer);

    for(TInt i = 0; i < aScroller.ItemCount(); ++i)
        {
        CGulIcon* icon = aScroller.ItemL(i).Icon();
        if(icon)
            {
            buffer->AddToBufferL(*icon, i);
            }
        }

    return buffer;
    }

// ----------------------------------------------------------------------------
// Create a copy of the icons used from input
// ----------------------------------------------------------------------------
CMPXCollectionViewHgSwitchBuffer* CMPXCollectionViewHgSwitchBuffer::CreateBufferLC( CHgVgMediaWall& aMediaWall )
    {
    CMPXCollectionViewHgSwitchBuffer* buffer = new (ELeave) CMPXCollectionViewHgSwitchBuffer();
    CleanupStack::PushL(buffer);

    for(TInt i = 0; i < aMediaWall.ItemCount(); ++i)
        {
        CGulIcon* icon = aMediaWall.ItemL(i).Icon();
        if(icon)
            {
            buffer->AddToBufferL(*icon, i);
            }
        }

    return buffer;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
CMPXCollectionViewHgSwitchBuffer::~CMPXCollectionViewHgSwitchBuffer ()
    {
    iBuffer.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// Provide mediawall with the data from buffer.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgSwitchBuffer::FillFromBufferL( CHgVgMediaWall& aMediaWall )
    {
    for(TInt i = 0; i < iBuffer.Count(); ++i)
        {
        CGulIcon* icon = iBuffer[i]->GetIcon();
        CleanupStack::PushL(icon);
        if(icon)
            aMediaWall.ItemL(iBuffer[i]->Index() + iOffset).SetIcon(icon);

        CleanupStack::Pop(icon);
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgSwitchBuffer::FillFromBufferL( CHgScroller& aScroller, TSize aSize )
    {
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Offset to start filling.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgSwitchBuffer::SetIndexOffset( TInt aOffset )
    {
    iOffset = aOffset;
    }

// ----------------------------------------------------------------------------
// Add icon to the buffer.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgSwitchBuffer::AddToBufferL(CGulIcon& aIcon, TInt aIndex)
    {
    CGulIcon* newIcon = CGulIcon::NewL(aIcon.Bitmap(), aIcon.Mask());
    TBool isShared = aIcon.BitmapsOwnedExternally();
    aIcon.SetBitmapsOwnedExternally(ETrue);
    newIcon->SetBitmapsOwnedExternally(isShared);

    CleanupStack::PushL(newIcon);
    CBufferItem* item = new (ELeave) CBufferItem( newIcon, aIndex );
    iBuffer.AppendL(item);
    CleanupStack::Pop(newIcon);
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
CMPXCollectionViewHgSwitchBuffer::CMPXCollectionViewHgSwitchBuffer ()
    {

    }
