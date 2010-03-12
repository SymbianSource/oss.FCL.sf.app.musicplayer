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
* Description:  Implementation of Collection view container
*
*/


// INCLUDE FILES

#include "mpxpopuplist.h"

CMpxPopupList::CMpxPopupList(): CAknPopupList()
    {
    }

CMpxPopupList::~CMpxPopupList()
    {
    }

CMpxPopupList* CMpxPopupList::NewL( 
    CEikListBox* aListBox, 
    TInt aCbaResource,
    AknPopupLayouts::TAknPopupLayouts aType )
    {
    CMpxPopupList* self = new (ELeave) CMpxPopupList();
    CleanupStack::PushL( self );
    self->ConstructL( aListBox, aCbaResource, aType );
    CleanupStack::Pop( self );
    return self;
    }

TRect CMpxPopupList::LayoutRect()
    {
    SetupWindowLayout( iWindowType );
    TRect rect = Rect();
    rect.Move( PositionRelativeToScreen() );
    return rect;
    }

//  End of File
