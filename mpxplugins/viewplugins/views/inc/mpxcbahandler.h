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
* Description:  MPX CBA handler
*
*/



#ifndef MMPXCBAHANDLER_H
#define MMPXCBAHANDLER_H


// INCLUDES

#include <e32std.h>

// FORWARD DECLARATIONS

class MMpxCbaHandler
    {
public:

    virtual void ChangeCbaVisibility( TBool aVisible ) = 0;

    };


#endif  // MMPXCBAHANDLER_H

// End of File
