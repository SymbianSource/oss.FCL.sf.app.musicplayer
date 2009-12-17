/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Music Wap Adapter cenrep keys
*
*/



#ifndef MUSICWAPADAPTERCRKEYS_H
#define MUSICWAPADAPTERCRKEYS_H

// CenRep UID for Music Store
const TUid KCRUidMusicShopSettings = {0x10207BF5};

const TUint32 KMusicShopToProxy            = 0x00000001;
const TUint32 KMusicShopBookmarkName       = 0x00000002;
const TUint32 KMusicShopURI                = 0x00000003;
const TUint32 KMusicShopAuthType           = 0x00000004;
const TUint32 KMusicShopAuthName           = 0x00000005;
const TUint32 KMusicShopAuthSecret         = 0x00000006;
const TUint32 KMusicShopSearchURI          = 0x00000007;
const TUint32 KMusicShopIAP                = 0x00000008;
const TUint32 KMusicShopFeatureFlagSupport = 0x00000009;
const TUint32 KMusicShopAppWrapperSupport  = 0x0000000A;
const TUint32 KMusicShopAlwaysAsk          = 0x0000000B;
const TUint32 KMusicShopHideSetting        = 0x0000000C;
const TUint32 KMusicShopVersion            = 0x0000000D;

#define KSupportRTSP 0x00000001;
#define KSupportWLan 0x00000002;
#define KSupportMusicShopWrapper 0x00000004;
#define KSupportMusicShopSettings 0x00000008;

#endif      // MUSICWAPADAPTERCRKEYS_H


// End of File
