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
const TUid KCRUidMusicShopSettings = {0x101FFB51};

const TUint32 KMusicShopToProxy            = 0x00000001;
const TUint32 KMusicShopBookmarkName       = 0x00000002;
const TUint32 KMusicShopURI                = 0x00000003;
const TUint32 KMusicShopAuthType           = 0x00000004;
const TUint32 KMusicShopAuthName           = 0x00000005;
const TUint32 KMusicShopAuthSecret         = 0x00000006;
const TUint32 KMusicShopSearchURI          = 0x00000007;
#if 0 // NO __ALR_MUSICSHOP_SUPPORT
const TUint32 KMusicShopIAP                = 0x00000008;
#endif // NO __ALR_MUSICSHOP_SUPPORT
const TUint32 KMusicShopFeatureFlagSupport = 0x00000009;
const TUint32 KMusicShopAppWrapperSupport  = 0x0000000A;
#if 0 // NO __ALR_MUSICSHOP_SUPPORT
const TUint32 KMusicShopAlwaysAsk          = 0x0000000B;
#endif // NO __ALR_MUSICSHOP_SUPPORT
const TUint32 KMusicShopHideSetting        = 0x0000000C;
const TUint32 KMusicShopVersion            = 0x0000000D;
#if 1 // __ALR_MUSICSHOP_SUPPORT
const TUint32 KMusicShopConnectionMode     = 0x0000000E;
const TUint32 KMusicShopConnectionId       = 0x0000000F;
const TUint32 KMusicShopConfirmCellUsage   = 0x00000010;
const TUint32 KMusicShopConfirmRoaming     = 0x00000011;
#endif // __ALR_MUSICSHOP_SUPPORT
// Feature flag for Nokia Music Services
const TUint32 KMusicServiceFeatureFlagSupport = 0x00000012;
// Feature flag for Nokia Music Services (3-item toolbar)
const TUint32 KMusicShopHomeLink		   = 0x000000013;
const TUint32 KMusicShopAccountLink		   = 0x000000014;
// Feature flag for OTA Streaming and download with WLAN AP only
const TUint32 KMusicShopOtaWlanOnly        = 0x000000015;
//
// Bit definitions for KMusicShopFeatureFlagSupport
//
const TUint32 KSupportRTSP                    = 0x00000001;
const TUint32 KSupportWLan                    = 0x00000002;
const TUint32 KSupportMusicShopWrapper        = 0x00000004;
const TUint32 KSupportMusicShopSettings       = 0x00000008;
const TUint32 KSupportMusicShopDownloadUi     = 0x00000010;

//
// Bit definitions for KMusicServiceFeatureFlagSupport
// This is the capability of Music Store client on device.
// Must agree with Music Servie on the scheme when adding new
// feature on device.
//
const TUint32 KSupportCwnIMEISending          = 0x00000001;
const TUint32 KSupportHTTPStreaming           = 0x00000002;
const TUint32 KSupportOmaDownloadDescriptor   = 0x00000004;
const TUint32 KSupportMusicShopRedirection    = 0x00000008;

#endif      // MUSICWAPADAPTERCRKEYS_H


// End of File
