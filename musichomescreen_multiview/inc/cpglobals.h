/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Global definitions
 *
*/


#ifndef CPCLIENTGLOBALS_H
#define CPCLIENTGLOBALS_H

/** Data types for CP content */
enum TDataTypes
    {
    ECPHardcodedText = 1, // 0001
    ECPResourceText = 2, // 0010
    ECPFilepathImage = 4, // 0100 
    ECPResourceIcon = 8 // 1000
    };

enum TCPGetListOrder
    {
    ECPRecent = 1, // 0001
    ECPPublisher = 2, // 0010
    ECPContent = 4, // 0100 
    ECPContentId = 8 // 1000 
    };

enum TFlag
    {
    EActivate = 1, // 0001
    };
    
const TInt KNameArgumentsLimit = 5;

_LIT( KService, "CP Service" );

_LIT8( KCPInterface, "IDataSource" );
_LIT8( KCPContentInterface, "IContentPublishing" );
_LIT8( KCPService, "Service.ContentPublishing" );
_LIT8( KCommand, "cmd" );

//Supported Commands
_LIT8( KGetList, "GetList" );
_LIT8( KAdd, "Add" );
_LIT8( KDelete, "Delete" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KExecuteAction, "ExecuteAction" );
_LIT8( KCmdCancel, "Cancel" );
_LIT8( KActivate, "Activate" );
_LIT8( KActivateTrigger, "activate" );
_LIT8( KDeactivateTrigger, "deactivate" );

// Input arguments
_LIT8( KType, "type" );
_LIT8( KAction, "action" );
_LIT8( KItem, "item" );
_LIT8( KFilter, "filter" );
_LIT8( KData, "data" );
_LIT8( KSortOrder, "sort_order" );
_LIT8( KItemId, "item_id" );
_LIT8( KByPublisher, "by_publisher" );
_LIT8( KByEntryIds, "by_entryids" );
_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId, "content_id" );
_LIT8( KExpirationDate, "expiration_date" );
_LIT8( KActivatePublisher, "activate" );
_LIT8( KIds, "ids" );
_LIT8( KId, "id" );
_LIT8( KFlag, "flag");
_LIT8( KAccessList, "ACL" );
_LIT8( KACLRead, "read");
_LIT8( KACLWrite, "write");
_LIT8( KACLDelete, "delete");
_LIT8( KUIDs, "UID" );
_LIT8( KCapability, "Capability" );
_LIT( KAll, "all");
_LIT( KSortPublisher, "publisher");
_LIT( KSortContentType, "content_type");
_LIT( KSortContentId, "content_id");
_LIT( KSortRecent, "recent");
_LIT( KOperationAdd, "add" );
_LIT( KOperationUpdate, "update" );
_LIT( KOperationDelete, "delete" );
_LIT( KOperationExecute, "execute" );
_LIT8( KDataType, "data_type" );
_LIT8( KOperation, "operation" );
_LIT8( KActionTrigger, "action_trigger" );
_LIT8( KHardcodedText, "harcoded_text" );
_LIT8( KResourceText, "resource_text" );
_LIT8( KFilepathImage, "filepath_image" );
_LIT8( KIdMask, "id_mask" );
_LIT8( KResourceIcon, "resource_icon" );
_LIT8( KIdIcon, "id_icon" );
_LIT8( KIdText, "id_text" );
_LIT( KCpData, "cp_data" );
_LIT( KPublisher, "publisher" );
_LIT8( KChangeInfo, "change_info" );

_LIT8( KPublisher_Uid, "publisher_Uid" );

// Argument in Action
_LIT8( KActionHandlerInterface, "IDataAction" );
_LIT8( KActionHandlerService, "Service.ActionHandler" );
_LIT8( KCmdExecute, "Execute" );
_LIT8( KPluginUid, "plugin_id" );
_LIT8( KDataForActionHandler, "data" );

_LIT8( KLaunchMethod, "launch_method" );
_LIT8( KApplicationUid, "application_uid" );
_LIT8( KDocumentName, "document_name" );
_LIT8( KViewId, "view_id" );
_LIT8( KMessageUid, "message_uid" );

// Output
_LIT8( KResults, "results" );
_LIT8( KListMap, "list_map" );
_LIT8( KStatus, "status" );
_LIT8( KDataMap, "data_map" );
_LIT8( KActionMap, "action_map" );
_LIT8( KKey, "key" );

_LIT8( KTransactionID, "TransactionID" );

#endif /*CPCLIENTGLOBALS_H*/

//  End of File
