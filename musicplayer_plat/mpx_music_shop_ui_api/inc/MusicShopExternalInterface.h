/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains external interface UID/DNL
*
*/


#ifndef MUSICSHOP_EXTERNAL_INTERFACE_H
#define MUSICSHOP_EXTERNAL_INTERFACE_H


#include <vwsdef.h>
#include <HttpDownloadMgrCommon.h> // KMaxUrlLength

const TUid KUidMusicShop = { 0x101F8839 }; // UID of the application
const TUid KUidMusBmViewId = {0x101F883A};
const TUid KUidMusBrViewId = {0x101F883B};

const TUid KUidMusDnlBmLinkMsg = {0x00000001};
const TUid KUidMusDnlBrUrlLinkMsg = {0x00000002};
const TUid KUidMusDnlBmViewBackToPageMsg = {0x00000003};


class TMushopBrViewDnl
    {
    public: // member variable
        TBuf<KMaxUrlLength> iUrl;
        TUint iDefaultAccessPoint;
        TInt  iRskId;
    };

typedef TPckgBuf<TMushopBrViewDnl> TMushopBrViewDnlBuf;


#endif // MUSICSHOP_EXTERNAL_INTERFACE_H
// End of File

