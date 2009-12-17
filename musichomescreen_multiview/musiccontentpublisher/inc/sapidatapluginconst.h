/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content model for data plug-in.
*
*/


#ifndef SAPIDATAPLUGINCONST_H
#define SAPIDATAPLUGINCONST_H

#include <aicontentmodel.h>
//#include "sapidatapluginuids.hrh"

// AI Data Plug-in ECOM implementation UID.
//const TInt KImplUidDataPlugin = SAPIDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN;
//const TUid KUidDataPlugin = { KImplUidDataPlugin };

// ================================= CONTENT ===================================

/**
 * Content Model type identifier
 */
_LIT(KText ,"text");
_LIT(KImage, "image");
_LIT(KPlugin, "plugin");


// CPS Constants
_LIT(KService,         "service");
_LIT(KInterface,       "interface");
_LIT(KCommand,         "command");
_LIT8( KType,          "type");
_LIT( KCpData,         "cp_data");
_LIT( KPubData,        "publisher" );
_LIT8( KFilter,        "filter" );
_LIT8( KDataMap,       "data_map");
_LIT8( KActionTrigger, "action_trigger" );
_LIT8( KGetList ,      "GetList" );
_LIT( KExecute,       "execute" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KChangeInfo, "change_info" );
_LIT8( KListMap, "list_map" );
_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId,   "content_id" );
_LIT8(KResults,      "results");
_LIT( KAll, "all");
_LIT8( KOperation, "operation" );

_LIT( KOperationAdd, "add" );
_LIT( KOperationUpdate, "update" );
_LIT( KOperationDelete, "delete" );
_LIT( KOperationExecute, "execute" );

_LIT( KDeActive, "deactive");
_LIT( KActive, "active");
_LIT( KSuspend , "suspend");
_LIT( KResume, "resume");

// reserved extension for retrieving mask handle 
_LIT8( KMask, "_mask");

// Sapi Data Plugin Constants
_LIT(KPluginNameSeprator, "/");

/**
 * Maximum length for the content name
 */
const TInt KMaxTypeLength = 8;

/**
 * Maximum length for the content name
 */
const TInt KSAPIContentNameMaxLength = 255;

/**
 * Maximum length for the content value
 */
const TInt KSAPIContentValueMaxLength = 255;

/**
 * Maximum length for the plugin name
 */
const TInt KAiPluginNameMaxLength = 255;


#endif /* SAPIDATAPLUGINCONST_H */
