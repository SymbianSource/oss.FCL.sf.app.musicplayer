/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SDK, AI Player Plugin Content Model API
*
*/


#ifndef AIPLPLCONTENTMODEL_H
#define AIPLPLCONTENTMODEL_H

#include <aicontentmodel.h>
#include "aiplayerpluginuids.hrh"

// Plug-in Implementation UID.
const TInt KImplUidMusicPlayerPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PLAYERPLUGIN;
const TUid KUidMusicPlayerPlugin = { KImplUidMusicPlayerPlugin };


// ================================= CONTENT ===================================

/**
 * Content Ids
 */
enum TAiPlplContentIds
    {
     EAiPlplContentArtistCaption
    ,EAiPlplContentTitleCaption
    ,EAiPlplContentDurationCaption
    ,EAiPlplContentVolume
    ,EAiPlplContentStatus
    ,EAiPlplContentElapsedTime
    };

/**
 * Content that the plugin will publish
 */
const TAiContentItem KAiPlplContent[] =
    {
     { EAiPlplContentArtistCaption,   L"Artist",    KAiContentTypeText }
    ,{ EAiPlplContentTitleCaption,    L"Title",     KAiContentTypeText }
    ,{ EAiPlplContentDurationCaption, L"Duration",  KAiContentTypeText }
     //Current volume level. Content needs to be re-published every time when volume is changed
     //Always published as resource see KAiPlplResources \ VolumeLevel
    ,{ EAiPlplContentVolume,   		  L"Volume",    KAiContentTypeText }
     //Current player status. Content needs to be re-published every time when player status is changed.
     //When player is stoped, the plug-in MUST publish empty item
     //Always published as resource see KAiPlplResources \ Status
    ,{ EAiPlplContentStatus,          L"Status",    KAiContentTypeBitmap }
    ,{ EAiPlplContentElapsedTime,     L"ElapsedTime",KAiContentTypeText }
    };


// ================================ RESOURCES ==================================


/**
 * Resource Ids
 */
enum TAiPlplPluginResourceIds
    {
     EAiPlplResourceVol0
    ,EAiPlplResourceVol1
    ,EAiPlplResourceVol2
    ,EAiPlplResourceVol3
    ,EAiPlplResourceVol4
    ,EAiPlplResourceVol5
    ,EAiPlplResourceVol6
    ,EAiPlplResourceVol7
    ,EAiPlplResourceVol8
    ,EAiPlplResourceVol9
    ,EAiPlplResourceVol10
    ,EAiPlplResourcePlayIcon
    ,EAiPlplResourcePauseIcon
    };

/**
 * Resources that the plugin will publish
 */
const TAiContentItem KAiPlplResources[] =
    {
    //UI resources that represents volume levels
    { EAiPlplResourceVol0,     L"Vol0",       KAiContentTypeText }
   ,{ EAiPlplResourceVol1,     L"Vol1",       KAiContentTypeText }
   ,{ EAiPlplResourceVol2,     L"Vol2",       KAiContentTypeText }
   ,{ EAiPlplResourceVol3,     L"Vol3",       KAiContentTypeText }
   ,{ EAiPlplResourceVol4,     L"Vol4",       KAiContentTypeText }
   ,{ EAiPlplResourceVol5,     L"Vol5",       KAiContentTypeText }
   ,{ EAiPlplResourceVol6,     L"Vol6",       KAiContentTypeText }
   ,{ EAiPlplResourceVol7,     L"Vol7",       KAiContentTypeText }
   ,{ EAiPlplResourceVol8,     L"Vol8",       KAiContentTypeText }
   ,{ EAiPlplResourceVol9,     L"Vol9",       KAiContentTypeText }
   ,{ EAiPlplResourceVol10,   L"Vol10",       KAiContentTypeText }
    
    //UI resources that represents status
   ,{ EAiPlplResourcePlayIcon,  L"PlayIcon",  KAiContentTypeBitmap }
   ,{ EAiPlplResourcePauseIcon, L"PauseIcon", KAiContentTypeBitmap }
    };

// ============================ SERVICES (Events) ==============================

/**
 * Event Ids.
 */
enum TAiPlplEventIds
    {
     EAiPlplEventVolInc
    ,EAiPlplEventVolDec
    ,EAiPlplEventLaunchLibrary
    ,EAiPlplEventVolSet
    ,EAiPlplEventVolMute
    ,EAiPlplEventVolRestore
    };

/**
 * Services that the plugin can perform
 */
const TAiContentItem KAiPlplEvents[] =
    {
     { EAiPlplEventVolInc,         L"VolInc", "" }
    ,{ EAiPlplEventVolDec,         L"VolDec", "" }
    ,{ EAiPlplEventLaunchLibrary,  L"Library", "" }
    ,{ EAiPlplEventVolSet,         L"VolSet", "" }
    ,{ EAiPlplEventVolMute,        L"Mute", "" }
    ,{ EAiPlplEventVolRestore,     L"Restore", "" }
    };

#endif // AIPLPLCONTENTMODEL_H
