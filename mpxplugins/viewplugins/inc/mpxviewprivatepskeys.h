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
* Description:  MPX View PS Keys
*
*/



#ifndef MMPXVIEWPRIVATEPSKEYS_H
#define MMPXVIEWPRIVATEPSKEYS_H

const TUid KMPXViewPSUid = {0x10207C5C};    // MPXView PS Category UID
const TInt KMPXUSBUnblockingPSStatus = 1;   // MPXView PS Key ID
 
enum TMPXUSBAndNotMTPPSStatus
    {
    EMPXUSBUnblockingPSStatusUninitialized = 0, 
    EMPXUSBUnblockingPSStatusActive
    };

#endif  // MMPXVIEWPRIVATEPSKEYS_H

// End of File
