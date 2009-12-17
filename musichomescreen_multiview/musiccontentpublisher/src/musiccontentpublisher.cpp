/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages MCP plugins, and content publishing.
*
*/

#include <mpxlog.h>                     // MPX_DEBUG
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>

#include <mcpplugin.h>
#include <mcppluginuids.hrh>
#include <fbs.h>
#include <coemain.h>
#include <AknsUtils.h>
#include <gdi.h>
#include <musichomescreen.rsg>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <s32mem.h>
#include <e32hashtab.h>

#include "cpglobals.h" //This will be moved to domain API (HSFW)
#include "musiccontentpublisher.h"
#include "pluginmanager.h"
#include "mcpharvesterpublisherobserver.h"

_LIT( KPubData,        "publisher" );


_LIT8( KMyActive, "active" );
_LIT8( KMyDeActive, "deactive");
_LIT8( KMySuspend, "suspend");
_LIT8( KMyResume, "resume");
_LIT8( KMyActionMap, "action_map" );
_LIT8( KMyItem, "item" );
_LIT8( KMyAdd, "Add" );
_LIT8( KMyItemId, "item_id" );
_LIT( KMyActionName, "data" );

_LIT( KEmpty, "" );
_LIT( KLoc, "LOC:");
_LIT( KWildCard, "*");

_LIT( KNowPlaying, "LOC:NOW PLAYING" );
_LIT( KLastPlayed, "LOC:LAST PLAYED" );
_LIT( KMask, "_mask");
_LIT( KMWPublisher, "MWPublisher");
_LIT( KactionMessageToMusicPlayer, "MessageToMusicPlayer" );
_LIT( KGoToAlbumView, "GoToAlbumView" );
_LIT8( KMessage, "message" );

//for application launcher AHPlugin
const TInt KMmUid3AsTInt( 0x101f4cd2 );
const TInt KMSGUidAsTInt( 0x10003A39 );
_LIT8( KAdditionalData, "additional_data" );
_LIT( KLaunchApp, "launch_application" );
_LIT( KMessageWithTail, "message_with_tail" );
_LIT8( KMessageForMMOpenMusicSuiteWithHide, "mm://root/musicsuite?exit=hide");

_LIT( KResourceFile, "z:musichomescreen.rsc");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMusicContentPublisher::CMusicContentPublisher( MLiwInterface* aCPSInterface )
    {
    iCPSInterface = aCPSInterface;
    iActivePlugin=NULL;
    iIsPublisherActive = EFalse;

    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::ConstructL()
    {
    MPX_DEBUG1("CMusicContentPublisher::ConstructL <---");
    //Load the destination translation table.
    TInt dstl (sizeof(KMCPDestinationInfo)/sizeof(TMCPDestinationItem));
    for (TInt i = 0; i < dstl; i++)
        {
        iDestinationMap.InsertL(KMCPDestinationInfo[i].id, 
                KMCPDestinationInfo[i]);
        }
    
    dstl = (sizeof(KMCPImageDestinationInfo)/sizeof(
            TMCPImageDestinationInfoItem));
    for (TInt i = 0; i < dstl; i++)
        {
        iImageDestinationSizeMap.Insert(KMCPImageDestinationInfo[i].id,
                TSize(KMCPImageDestinationInfo[i].sizex,
                        KMCPImageDestinationInfo[i].sizey));
        }
    
    MPX_DEBUG1("CMusicContentPublisher::ConstructL loading resources");
    //Load Loc strings
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);  
    TFileName fileName;
    TParse* parseObj = new(ELeave) TParse();
    TInt errInt = parseObj->Set( KResourceFile(),&KDC_APP_RESOURCE_DIR,NULL );
    if(KErrNone != errInt)
      {
      delete parseObj;
      User::Leave(errInt);
      }
    fileName = parseObj->FullName();
    delete parseObj;
    BaflUtils::NearestLanguageFile(fs,fileName);
    if(!BaflUtils::FileExists(fs,fileName))
        {
        User::Leave(KErrNotFound);
        }
    RResourceFile resourceFile;
    resourceFile.OpenL(fs,fileName);
    CleanupClosePushL(resourceFile);
    resourceFile.ConfirmSignatureL();
    GetLocalizedStringL(resourceFile, iLastPlayedBuffer, R_MUSICHOMESCREEN_LAST_PLAYED);
    GetLocalizedStringL(resourceFile, iNowPlayingBuffer, R_MUSICHOMESCREEN_NOW_PLAYING);
    GetLocalizedStringL(resourceFile, iGoToMusicBuffer, R_MUSICHOMESCREEN_GO_TO_MUSIC);
    CleanupStack::PopAndDestroy(&resourceFile);
    CleanupStack::PopAndDestroy(&fs);
    MPX_DEBUG1("CMusicContentPublisher::ConstructL resources loaded");
    
    // connect to the skin server, to receive skin changed event.
    iAknsSrvSession.Connect(this);
    
    // enable skin.
    AknsUtils::InitSkinSupportL();

    MPX_DEBUG1("CMusicContentPublisher::ConstructL subscribing to observer");
 
    TUint id = RegisterPublisherL( 
            KMWPublisher,
            KAll, 
            KAll );
        
    if( id != 0 )
        {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();

    filter->InsertL( KPublisherId, TLiwVariant( KMWPublisher ) );
    filter->InsertL( KContentId, TLiwVariant( KAll) );
    filter->InsertL( KContentType, TLiwVariant( KAll ) );

    iHPObserver = CMCPHarvesterPublisherObserver::NewL(this);
    iHPObserver->RegisterL(filter);
    CleanupStack::PopAndDestroy(filter);
        }
    //Reset the music menu info
    InstallEmptyActionL(EMusicMenuMusicInfoTrigger);
    PublishTextL( NULL, EMusicMenuMusicInfoLine1, iLastPlayedBuffer->Des() );
    PublishTextL( NULL, EMusicMenuMusicInfoLine2, KEmpty );
    PublishImageL( NULL, EMusicMenuMusicInfoImage1, KEmpty );
    
    MPX_DEBUG1("CMusicContentPublisher::ConstructL --->");
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMusicContentPublisher* CMusicContentPublisher::NewL(  
        MLiwInterface* aCPSInterface )
    {
    CMusicContentPublisher* self = new ( ELeave ) CMusicContentPublisher( 
            aCPSInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMusicContentPublisher::~CMusicContentPublisher()
    {
    
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher <---");
    if(iHPObserver)
        {
        MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher deleting observer");
        delete iHPObserver; 
        }
    delete iNowPlayingBuffer;
    delete iLastPlayedBuffer;
    delete iGoToMusicBuffer;
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher closing destination maps");
    iImageDestinationSizeMap.Close();
    iDestinationMap.Close();
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher resetting bitmap cache");
    ResetBitmapCache();
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher closing skin server session");
    iAknsSrvSession.Close();
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher deleting plugin manager");
    delete iPluginManager;
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher resetting publishing buffers");
    ResetPublishingBuffers();
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher --->");
    if ( iInstanceId )
    	{
    	delete iInstanceId;
    	}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::InstallGoToMusicL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMusicContentPublisher::InstallGoToMusicL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();
    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10282E5F ) ) );
    mapData->InsertL( KType, TLiwVariant( KLaunchApp ) );
    mapData->InsertL( KLaunchMethod, TLiwVariant( KMessageWithTail ) );
    mapData->InsertL(KApplicationUid,
            TLiwVariant(TInt32(KMmUid3AsTInt) ) );
        mapData->InsertL( KAdditionalData, 
            TLiwVariant( KMessageForMMOpenMusicSuiteWithHide ) );
    mapData->InsertL(KMessageUid,
            TLiwVariant(TInt32(KMSGUidAsTInt) ) );
    
    mapTrigger->InsertL( KData, TLiwVariant( mapData ) );
    
    PublishActionL( NULL, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMusicContentPublisher::InstallGoToMusicL --->");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::InstallGoToAlbumL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMusicContentPublisher::InstallGoToAlbumL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();
    
    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10207C16 ) ) );
       mapData->InsertL( KType, TLiwVariant( KactionMessageToMusicPlayer ) );
       mapData->InsertL( KMessage, TLiwVariant( KGoToAlbumView ) );
    mapTrigger->InsertL( KData, TLiwVariant( mapData ) );
    
    PublishActionL( NULL, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMusicContentPublisher::InstallGoToAlbumL --->");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::InstallEmptyActionL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMusicContentPublisher::InstallEmptyActionL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    PublishActionL( NULL, aDestination, mapTrigger );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMusicContentPublisher::InstallEmptyActionL --->");
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::MapEnumToDestinationInfoL(TInt aEnum,
        TPtrC& aType, TPtrC8& aDataKey, TPtrC& aContent)
    {
    MPX_DEBUG1("CMusicContentPublisher::MapEnumToDestinationInfoL <---");
    TMCPDestinationItem* tmpdstitemp;
    tmpdstitemp = iDestinationMap.Find(aEnum);
    
    if (tmpdstitemp)
        {
        TPtrC type ( reinterpret_cast<const TUint16*>(
                tmpdstitemp->type) );
        TPtrC8 dataKey ( reinterpret_cast<const TUint8*>(
                tmpdstitemp->dataKey ) );
        TPtrC content ( reinterpret_cast<const TUint16*>(
                    tmpdstitemp->content) );

        aType.Set(type);
        aDataKey.Set(dataKey);

        if ( !content.Compare( KWildCard ) )
            {
            aContent.Set( iInstanceId->Des() );
            }
        else
            {
            aContent.Set(content);
            }
        }
    else
        {
        //API user provided an invalid destination or the destination is not
        //defined properly.
        __ASSERT_DEBUG(EFalse,User::Invariant());
        }
    MPX_DEBUG1("CMusicContentPublisher::MapEnumToDestinationInfoL --->");
    }

// ----------------------------------------------------------------------------
// Get a heap descriptor from the resource file
// ----------------------------------------------------------------------------
//
void CMusicContentPublisher::GetLocalizedStringL(RResourceFile& aResourceFile,
        HBufC*& aRetBuf, TInt aResourceId )
   {
   MPX_DEBUG1("CMusicContentPublisher::GetLocalizedStringL <---");
   HBufC8* dataBuffer = aResourceFile.AllocReadLC(aResourceId);
   TResourceReader theReader;
   theReader.SetBuffer(dataBuffer);
   aRetBuf = theReader.ReadHBufCL();
   CleanupStack::PopAndDestroy(dataBuffer);
   MPX_DEBUG1("CMusicContentPublisher::GetLocalizedStringL --->");
   }

// ----------------------------------------------------------------------------
// Publishes buffered data and actions.
// ----------------------------------------------------------------------------
//
void CMusicContentPublisher::DoPublishL()
    {
    MPX_DEBUG1("CMusicContentPublisher::DoPublishL <---");
    THashMapIter<TInt, TMyBufferItem> dataIter( iPublishingDataBuffers );
    MPX_DEBUG1("CMusicContentPublisher::DoPublishL publishing data");
    
    TMyBufferItem const* itemptr;
    itemptr = dataIter.NextValue();
    while (itemptr)
        {
        CLiwGenericParamList* inParam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outParam = CLiwGenericParamList::NewLC();
        
        TPtrC8 dataKey;
        TPtrC type;
        TPtrC content;
        MapEnumToDestinationInfoL(*dataIter.CurrentKey(), type, dataKey, content);
        
        TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
        inParam->AppendL( cptype );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC( );
        
        
        RMemReadStream rs( itemptr->buf, itemptr->size );
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC( rs );
         
        cpdatamap->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdatamap->InsertL( KContentType , TLiwVariant( type ) );
        cpdatamap->InsertL( KContentId  , TLiwVariant( content ) );
        cpdatamap->InsertL( KDataMap  , TLiwVariant( map ) );
    
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ) ); 
        inParam->AppendL( item );
 
        iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
        CleanupStack::PopAndDestroy( map );
        CleanupStack::PopAndDestroy( cpdatamap );
        CleanupStack::PopAndDestroy( outParam );
        CleanupStack::PopAndDestroy( inParam );
        
        itemptr = dataIter.NextValue();
        }
        
    //The order in wich we publish is important, actions should be published after the data contents.
    THashMapIter<TInt, TMyBufferItem> actionIter( iPublishingActionBuffers );
    MPX_DEBUG1("CMusicContentPublisher::DoPublishL publishing actions");
    
    itemptr = NULL;
    itemptr = actionIter.NextValue();
    while (itemptr)
        {
        CLiwGenericParamList* inParam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outParam = CLiwGenericParamList::NewLC();
        
        TPtrC8 dataKey;
        TPtrC type;
        TPtrC content;
        MapEnumToDestinationInfoL( *actionIter.CurrentKey(), type, dataKey, content );
        
        TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
        inParam->AppendL( cptype );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC( );
        
        
        RMemReadStream rs( itemptr->buf, itemptr->size );
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC( rs );
         
        cpdatamap->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdatamap->InsertL( KContentType , TLiwVariant( type ) );
        cpdatamap->InsertL( KContentId  , TLiwVariant( content ) );
        cpdatamap->InsertL( KMyActionMap  , TLiwVariant( map ) );
    
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ) ); 
        inParam->AppendL( item );
 
        iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
        CleanupStack::PopAndDestroy( map );
        CleanupStack::PopAndDestroy( cpdatamap );
        CleanupStack::PopAndDestroy( outParam );
        CleanupStack::PopAndDestroy( inParam );
        
        itemptr = actionIter.NextValue();
        }
    MPX_DEBUG1("CMusicContentPublisher::CMusicContentPublisher::DoPublishL --->");
    }

// ---------------------------------------------------------------------------
// Resets all the graphical elements.
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::ResetL()
    {
    MPX_DEBUG1("CMusicContentPublisher::ResetL <---");
    //Plugin deletion is handled by the pluginmanager.
    iActivePlugin = NULL;
    //Reset The Widget
    PublishImageL(NULL,EMusicWidgetImage1,KEmpty);
    InstallGoToAlbumL(EMusicWidgetTrigger1);
    PublishTextL( NULL,EMusicWidgetText1, KEmpty );
    PublishImageL(NULL,EMusicWidgetToolbarB1,KEmpty);
    PublishImageL(NULL,EMusicWidgetToolbarB2,KEmpty);
    PublishImageL(NULL,EMusicWidgetToolbarB3,KEmpty);
    InstallEmptyActionL(EMusicWidgetTB1Trigger);
    InstallEmptyActionL(EMusicWidgetTB2Trigger);
    InstallEmptyActionL(EMusicWidgetTB3Trigger);
    PublishTextL( NULL,EMusicWidgetDefaultText, iGoToMusicBuffer->Des() );  
    InstallGoToAlbumL(EMusicWidgetTrigger2);

    //Reset the music menu info
    InstallEmptyActionL(EMusicMenuMusicInfoTrigger);
    PublishTextL( NULL, EMusicMenuMusicInfoLine1, iLastPlayedBuffer->Des() );
    PublishTextL( NULL, EMusicMenuMusicInfoLine2, KEmpty );
    PublishImageL( NULL, EMusicMenuMusicInfoImage1, KEmpty );
    MPX_DEBUG1("CMusicContentPublisher::Reset --->");
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TSize CMusicContentPublisher::GetImageDestinationSize(
        TMCPImageDestination aDst)
    {
    MPX_DEBUG1("CMusicContentPublisher::GetImageDestinationSize <---");
    TSize * ret;
    ret = iImageDestinationSizeMap.Find(aDst);
    if (ret)
        {
        MPX_DEBUG1("CMusicContentPublisher::GetImageDestinationSize --->");
        return *ret;    
        }
    else
        {
        MPX_DEBUG1("CMusicContentPublisher::GetImageDestinationSize --->");
        return TSize(0,0);
        }
    }


// ---------------------------------------------------------------------------
// Destroys the bitmaps saved on the cache.
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::ResetBitmapCache()
    {
    MPX_DEBUG1("CMusicContentPublisher::ResetBitmapCache <---");
    THashMapIter<TInt, TBmpMsk> iter(iBitmapCache);
    TBmpMsk const* ptr = iter.NextValue();
    CFbsBitmap* bmp( NULL );
    CFbsBitmap* msk( NULL );
    while ( ptr )
        {
        bmp = ptr->bitmap;
        msk = ptr->mask;
        delete bmp;
        bmp = NULL;
        delete msk;
        msk = NULL;
        ptr = iter.NextValue();
        }
    iBitmapCache.Close();
    MPX_DEBUG1("CMusicContentPublisher::ResetBitmapCache --->");
    }
    
// ---------------------------------------------------------------------------
// Destroys the publishing buffers.
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::ResetPublishingBuffers()
    {
    THashMapIter<TInt, TMyBufferItem> dataIter( iPublishingDataBuffers );
    THashMapIter<TInt, TMyBufferItem> actionIter( iPublishingActionBuffers );
    
    TMyBufferItem const* itemptr;
    itemptr = dataIter.NextValue();
    while (itemptr)
        {
        User::Free(itemptr->buf);
        dataIter.RemoveCurrent();
        itemptr = dataIter.NextValue();
        }
    iPublishingDataBuffers.Close();
    itemptr = NULL;
    itemptr = actionIter.NextValue();
    while (itemptr)
        {
        User::Free(itemptr->buf);
        actionIter.RemoveCurrent();
        itemptr = actionIter.NextValue();
        }
    iPublishingActionBuffers.Close();
    }

TUint CMusicContentPublisher::RegisterPublisherL( 
    const TDesC& aPublisherId, 
    const TDesC& aContentId,
    const TDesC& aContentType )
    {
    MPX_DEBUG1("CMusicContentPublisher::RegisterPublisherL <---");
    TUint id( 0 );
    if( iCPSInterface )
        {   
        CLiwGenericParamList* inparam( CLiwGenericParamList::NewLC() );
        CLiwGenericParamList* outparam( CLiwGenericParamList::NewLC() );

        TLiwGenericParam type( KType, TLiwVariant( KPubData ));
        inparam->AppendL( type );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* actionmap( NULL );

        // Create the data map for publisher registry
        cpdatamap->InsertL( KContentType, TLiwVariant( aContentType ));
        cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
        cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));

        
        // Create the action map for publisher registry
        actionmap = CLiwDefaultMap::NewLC();
        actionmap->InsertL(KMyActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL(KMyDeActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL(KMySuspend, TLiwVariant( KMyActionName ));
        actionmap->InsertL(KMyResume, TLiwVariant( KMyActionName ));
 
        cpdatamap->InsertL( KMyActionMap, TLiwVariant(actionmap));
        CleanupStack::PopAndDestroy( actionmap );
        
        TLiwGenericParam item( KMyItem, TLiwVariant( cpdatamap ));        
        inparam->AppendL( item );
        
        iCPSInterface->ExecuteCmdL( KMyAdd , *inparam, *outparam);
        id = ExtractItemId(*outparam);
        
        CleanupStack::PopAndDestroy( datamap );
        CleanupStack::PopAndDestroy( cpdatamap );
        item.Reset();
        type.Reset();   
        CleanupStack::PopAndDestroy(outparam);
        CleanupStack::PopAndDestroy(inparam);
        }
    MPX_DEBUG1("CMusicContentPublisher::RegisterPublisherL --->");
    return id;
    }

TUint CMusicContentPublisher::ExtractItemId( const CLiwGenericParamList& aInParamList )
    {
    MPX_DEBUG1("CMusicContentPublisher::ExtractItemId <---");
    TUint result ( 0 );
    TInt pos( 0 );
    aInParamList.FindFirst( pos, KMyItemId );
    if( pos != KErrNotFound )
        {
        // item id present - extract and return
        aInParamList[pos].Value().Get( result );
        }
    MPX_DEBUG1("CMusicContentPublisher::ExtractItemId --->");
    return result;
    }
 
 // ---------------------------------------------------------------------------
// removes CPS entry for the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::RemoveL( TInt aDestination )
    {
    MPX_DEBUG1("CMusicContentPublisher::RemoveL <---");
    if( iCPSInterface )
        {
        CLiwGenericParamList * inParam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList * outParam = CLiwGenericParamList::NewLC();
        TPtrC8 dataKey;
        TPtrC type;
        TPtrC content;
        MapEnumToDestinationInfoL(aDestination, type, dataKey, content);
        TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
        inParam->AppendL( cptype );
        CLiwDefaultMap * cpdatamap = CLiwDefaultMap::NewLC( );
        cpdatamap->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdatamap->InsertL( KContentType , TLiwVariant( type ) );
        cpdatamap->InsertL( KContentId  , TLiwVariant( content ) );
        TLiwGenericParam item( KFilter, TLiwVariant( cpdatamap ) ); 
        inParam->AppendL( item );
        iCPSInterface->ExecuteCmdL( KDelete , *inParam, *outParam);
        CleanupStack::PopAndDestroy( cpdatamap );
        CleanupStack::PopAndDestroy( outParam );
        CleanupStack::PopAndDestroy( inParam );
        }  
    MPX_DEBUG1("CMusicContentPublisher::RemoveL --->");
    }

// ---------------------------------------------------------------------------
// Publishes an image from path to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishImageL( CMCPPlugin* aPlugin,  
        TMCPImageDestination aDestination, 
        const TDesC& aImagePath )
    {
    MPX_DEBUG1("CMusicContentPublisher::PublishImageL <---");
    if( iCPSInterface && iActivePlugin == aPlugin )
        {
        CLiwGenericParamList * inParam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList * outParam = CLiwGenericParamList::NewLC();
        
        TPtrC8 dataKey;
        TPtrC type;
        TPtrC content;
        MapEnumToDestinationInfoL(aDestination, type, dataKey, content);
        
        TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
        inParam->AppendL( cptype );
        CLiwDefaultMap * cpdatamap = CLiwDefaultMap::NewLC( );
        CLiwDefaultMap * map = CLiwDefaultMap::NewLC( );
        
         
        map->InsertL(dataKey, TLiwVariant( aImagePath ) );
        
        cpdatamap->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdatamap->InsertL( KContentType , TLiwVariant( type ) );
        cpdatamap->InsertL( KContentId  , TLiwVariant( content ) );
        cpdatamap->InsertL( KDataMap  , TLiwVariant( map ) );
    
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ) ); 
        inParam->AppendL( item );
        
        if ( aDestination >= EMusicMenuMusicInfoImage1 )
            {
            iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
            }
        else
            {
            if ( iIsPublisherActive )
                {
                iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
                }
            TMyBufferItem* ptr;
            ptr = iPublishingDataBuffers.Find(aDestination);
            if (ptr) //remove the old one
                {
                User::Free(ptr->buf);
                iPublishingDataBuffers.Remove(aDestination);
                }
            TInt sz = map->Size();
            TMyBufferItem bufferitem;
            bufferitem.size = map->Size();
            bufferitem.buf = User::AllocL(map->Size());
            RMemWriteStream ws(bufferitem.buf, bufferitem.size);
            map->ExternalizeL(ws);
            iPublishingDataBuffers.Insert(aDestination, bufferitem);
            }
        CleanupStack::PopAndDestroy( map );
        CleanupStack::PopAndDestroy( cpdatamap );
        CleanupStack::PopAndDestroy( outParam );
        CleanupStack::PopAndDestroy( inParam );
        }   
    MPX_DEBUG1("CMusicContentPublisher::PublishImageL --->");
    }

// ---------------------------------------------------------------------------
// Publishes an image from bitmap handle to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishImageL( CMCPPlugin* aPlugin, 
        TMCPImageDestination aDestination,
        TInt aBitmapHandle,
        TInt aMaskBitmapHandle)
    {
    MPX_DEBUG1("CMusicContentPublisher::PublishImageL <---");
    if( iCPSInterface && iActivePlugin == aPlugin )
        {
        CLiwGenericParamList * inParam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList * outParam = CLiwGenericParamList::NewLC();
        
        TPtrC8 dataKey;
        TPtrC type;
        TPtrC content;
        MapEnumToDestinationInfoL(aDestination, type, dataKey, content);
        
        TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
        inParam->AppendL( cptype );
        CLiwDefaultMap * cpdatamap = CLiwDefaultMap::NewLC( );
        CLiwDefaultMap * map = CLiwDefaultMap::NewLC( );
        
         
        if (aDestination == EMusicMenuMusicInfoImage1)
            {
            //Matrix Menu expects a Tint32 (TVariantTypeId::EVariantTypeTInt32)   
            map->InsertL(dataKey, TLiwVariant( TInt32( aBitmapHandle ) ) );
            }
        else 
            {
            //Homescreen expects a TBool/TInt 
            //(TVariantTypeId::EVariantTypeTBool)
            map->InsertL(dataKey, TLiwVariant( aBitmapHandle ) );
            }
        
        if (aMaskBitmapHandle)
            {
            HBufC8* maskResult = HBufC8::NewLC( 
                    dataKey.Length() + KMask().Length() );
            TPtr8 maskResultPtr = maskResult->Des();
            maskResultPtr.Append( dataKey );
            maskResultPtr.Append( KMask );
            if (aDestination == EMusicMenuMusicInfoImage1)
                {
                //Matrix Menu expects a Tint32 (TVariantTypeId::EVariantTypeTInt32)   
                map->InsertL(maskResultPtr, TLiwVariant( (TInt32)aMaskBitmapHandle ) );
                }
            else 
                {
                //Homescreen expects a TBool/TInt 
                //(TVariantTypeId::EVariantTypeTBool)
                map->InsertL(maskResultPtr, TLiwVariant( aMaskBitmapHandle ) );
                }
            CleanupStack::PopAndDestroy( maskResult );
            }
        
        cpdatamap->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdatamap->InsertL( KContentType , TLiwVariant( type ) );
        cpdatamap->InsertL( KContentId  , TLiwVariant( content ) );
        cpdatamap->InsertL( KDataMap  , TLiwVariant( map ) );
    
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ) ); 
        inParam->AppendL( item );
        
        if ( aDestination >= EMusicMenuMusicInfoImage1 )
            {
            iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
            }
        else
            {
            if ( iIsPublisherActive )
                {
                iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );  	
                }
            TMyBufferItem* ptr;
            ptr = iPublishingDataBuffers.Find(aDestination);
            if (ptr) //remove the old one
                {
                User::Free(ptr->buf);
                iPublishingDataBuffers.Remove(aDestination);
                }
            TInt sz = map->Size();
            TMyBufferItem bufferitem;
            bufferitem.size = map->Size();
            bufferitem.buf = User::AllocL(map->Size());
            RMemWriteStream ws(bufferitem.buf, bufferitem.size);
            map->ExternalizeL(ws);
            iPublishingDataBuffers.Insert(aDestination, bufferitem);
            }
        CleanupStack::PopAndDestroy( map );
        CleanupStack::PopAndDestroy( cpdatamap );
        CleanupStack::PopAndDestroy( outParam );
        CleanupStack::PopAndDestroy( inParam );
        }
    MPX_DEBUG1("CMusicContentPublisher::PublishImageL --->");
    }

// ---------------------------------------------------------------------------
// Publishes an image from skin id and mif to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishImageL(CMCPPlugin* aPlugin, 
        TMCPImageDestination aDestination,
        const TAknsItemID& aID,
        const TDesC& aFilename,
        const TInt aFileBitmapId,
        const TInt aFileMaskId )
    {
    MPX_DEBUG1("CMusicContentPublisher::PublishImageL <---");
    TBmpMsk* bitmapandmask;
    bitmapandmask = iBitmapCache.Find(aID.iMajor+aID.iMinor);
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    if (!bitmapandmask)
        {
        // Load from skin
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsUtils::CreateIconL(
                    skin,
                    aID,
                    bitmap,
                    mask,
                    aFilename,
                    aFileBitmapId,
                    aFileMaskId);
        AknIconUtils::SetSize(
                bitmap,
                GetImageDestinationSize(aDestination),
                EAspectRatioPreserved );
        AknIconUtils::SetSize(mask,
                GetImageDestinationSize(aDestination),
                EAspectRatioPreserved );
        TBmpMsk bmpAndMsk;
        bmpAndMsk.bitmap=bitmap;
        bmpAndMsk.mask=mask;
        iBitmapCache.Insert(aID.iMajor+aID.iMinor,bmpAndMsk);
        bitmapandmask = iBitmapCache.Find(aID.iMajor+aID.iMinor);
        __ASSERT_DEBUG(bitmapandmask,User::Invariant());
        }
    bitmap = bitmapandmask->bitmap;
    mask = bitmapandmask->mask;
    
    PublishImageL( aPlugin, aDestination, bitmap->Handle() , mask->Handle()); 
    MPX_DEBUG1("CMusicContentPublisher::PublishImageL --->");
    }

// ---------------------------------------------------------------------------
// Publishes a text from path to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishTextL( CMCPPlugin* aPlugin, 
        TMCPTextDestination aDestination, 
        const TDesC& aText )
    {
    MPX_DEBUG1("CMusicContentPublisher::PublishTextL <---");
    if( iCPSInterface && iActivePlugin == aPlugin)
        {
        CLiwGenericParamList * inParam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList * outParam = CLiwGenericParamList::NewLC();
        
        TPtrC8 dataKey;
        TPtrC type;
        TPtrC content;
        MapEnumToDestinationInfoL(aDestination, type, dataKey, content);
        
        TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
        inParam->AppendL( cptype );
        CLiwDefaultMap * cpdatamap = CLiwDefaultMap::NewLC( );
        CLiwDefaultMap * map = CLiwDefaultMap::NewLC( );
        
        if (KErrNotFound == aText.Find(KLoc))
            {
            map->InsertL( dataKey , TLiwVariant( aText ) );    
            }
        else
            {
            if ( ! aText.Compare( KNowPlaying ) )
                {
                map->InsertL( dataKey , TLiwVariant( iNowPlayingBuffer ) );
                }
            else if ( ! aText.Compare( KLastPlayed ) )
                {
                map->InsertL( dataKey , TLiwVariant( iLastPlayedBuffer ) );
                }
            else
                {
                map->InsertL( dataKey , TLiwVariant( aText ) );
                }
            }
        
        cpdatamap->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdatamap->InsertL( KContentType , TLiwVariant( type ) );
        cpdatamap->InsertL( KContentId  , TLiwVariant( content ) );
        cpdatamap->InsertL( KDataMap  , TLiwVariant( map ) );
        
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ) ); 
        inParam->AppendL( item );
        
        if ( aDestination >= EMusicMenuMusicInfoLine1 )
            {
            if (aText  != KEmpty)
                {
                iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
                }
            else
                {
                iCPSInterface->ExecuteCmdL( KDelete , *inParam, *outParam);
                }
            }
        else
            {
            if ( iIsPublisherActive )
                {
                iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );        	
                }
            TMyBufferItem* ptr;
            ptr = iPublishingDataBuffers.Find(aDestination);
            if (ptr) //remove the old one
                {
                User::Free(ptr->buf);
                iPublishingDataBuffers.Remove(aDestination);
                }
            TInt sz = map->Size();
            TMyBufferItem bufferitem;
            bufferitem.size = map->Size();
            bufferitem.buf = User::AllocL(map->Size());
            RMemWriteStream ws(bufferitem.buf, bufferitem.size);
            map->ExternalizeL(ws);
            iPublishingDataBuffers.Insert(aDestination, bufferitem);
            }
        CleanupStack::PopAndDestroy( map );
        CleanupStack::PopAndDestroy( cpdatamap );
        CleanupStack::PopAndDestroy( outParam );
        CleanupStack::PopAndDestroy( inParam );
        }  
    MPX_DEBUG1("CMusicContentPublisher::PublishTextL --->");
    }

// ---------------------------------------------------------------------------
// Publishes an action from a trigger map to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishActionL( CMCPPlugin* aPlugin, 
        TMCPTriggerDestination aDestination,
        CLiwDefaultMap* aTriggerMap )
    {
    MPX_DEBUG1("CMusicContentPublisher::PublishActionL <---");
     if( iCPSInterface && iActivePlugin == aPlugin)
        {
        CLiwGenericParamList * inParam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList * outParam = CLiwGenericParamList::NewLC();
        
        TPtrC8 triggerKey;
        TPtrC hostType;
        TPtrC hostContent;
        
        MapEnumToDestinationInfoL(aDestination, hostType, triggerKey, 
                hostContent);
        
        TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
        inParam->AppendL( cptype );
        
        CLiwDefaultMap * cpdatamap = CLiwDefaultMap::NewLC( );
        cpdatamap->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdatamap->InsertL( KContentType , TLiwVariant( hostType ) );
        cpdatamap->InsertL( KContentId  , TLiwVariant( hostContent ) );
        
        CLiwDefaultMap * actionmap = CLiwDefaultMap::NewLC( );
        actionmap->InsertL( triggerKey , TLiwVariant( aTriggerMap ) );
        cpdatamap->InsertL( KActionMap  , TLiwVariant( actionmap ) );
        
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ) ); 
        inParam->AppendL( item );
        
        if ( aDestination >= EMusicMenuMusicInfoTrigger )
            {
            iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
            }
        else
            {
            if ( iIsPublisherActive )
                {
            	  iCPSInterface->ExecuteCmdL( KAdd , *inParam, *outParam );
                }
            TMyBufferItem* ptr;
            ptr = iPublishingActionBuffers.Find(aDestination);
            if (ptr) //remove the old one
                {
                User::Free(ptr->buf);
                iPublishingActionBuffers.Remove(aDestination);
                }
            TInt sz = actionmap->Size();
            TMyBufferItem bufferitem;
            bufferitem.size = actionmap->Size();
            bufferitem.buf = User::AllocL(actionmap->Size());
            RMemWriteStream ws(bufferitem.buf, bufferitem.size);
            actionmap->ExternalizeL(ws);
            iPublishingActionBuffers.Insert(aDestination, bufferitem);
            }
        CleanupStack::PopAndDestroy( actionmap );
        CleanupStack::PopAndDestroy( cpdatamap );
        CleanupStack::PopAndDestroy( outParam );
        CleanupStack::PopAndDestroy( inParam );
        }
    MPX_DEBUG1("CMusicContentPublisher::PublishActionL --->");
    }

// ---------------------------------------------------------------------------
// Called when a plugin becomes active, it deactivates the previous plugin.
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::BecameActiveL( CMCPPlugin* aPlugin )
    {
    MPX_DEBUG1("CMusicContentPublisher::BecameActiveL <---");
    if ( iActivePlugin != aPlugin )
        {
        if ( iActivePlugin )
            {
            MPX_DEBUG1("CMusicContentPublisher::BecameActiveL deactivating old plugin");
            iActivePlugin->Deactivate();
            MPX_DEBUG1("CMusicContentPublisher::BecameActiveL plugin deactivated");
            PublishImageL( iActivePlugin, EMusicMenuMusicInfoImage1, KEmpty );
            }
        iActivePlugin = aPlugin;
        }
    MPX_DEBUG1("CMusicContentPublisher::BecameActiveL --->");
    }
// ---------------------------------------------------------------------------
// From CContentHarvesterPlugin
// This function updates information in CPS storage
// ---------------------------------------------------------------------------
//    
void CMusicContentPublisher::UpdateL() 
    {
    MPX_DEBUG1("CMusicContentPublisher::UpdateL <---");
    if ( !iPluginManager )
        {
        MPX_DEBUG1("CMusicContentPublisher::UpdateL creating the plugin manager");
        iPluginManager = CPluginManager::NewL(
                TUid::Uid( KMCPPluginUid ), 
                static_cast<MMCPPluginObserver*>( this ),
                this );
        }
    MPX_DEBUG1("CMusicContentPublisher::UpdateL --->");
    }

// ---------------------------------------------------------------------------
// From MAknsSkinChangeObserver.
// Called by the skin server when skin content is changed and the
// connected client wants to be informed.
// ---------------------------------------------------------------------------
// 
void CMusicContentPublisher::SkinContentChanged()
    {
    MPX_DEBUG1("CMusicContentPublisher::SkinContentChanged <---");
    ResetBitmapCache();
    if(iActivePlugin)
        {
        iActivePlugin->SkinChanged();
        }
    MPX_DEBUG1("CMusicContentPublisher::SkinContentChanged --->");
    }

void CMusicContentPublisher::HandlePublisherNotificationL( const TDesC& aContentId, const TDesC8& aTrigger )
    {
    MPX_DEBUG1("CMusicContentPublisher::HandlePublisherNotificationL <---");
    if ( aTrigger == KMyActive )
        {
        MPX_DEBUG1("CMusicContentPublisher::HandlePublisherNotificationL activate");
        if ( !iInstanceId )
            {
            iInstanceId = aContentId.AllocL();
        	  //Reset The Widget
        	  MPX_DEBUG1("CMusicContentPublisher::HandlePublisherNotificationL activate --> Reset Widget");
        	  RDebug::Print(aContentId);
            PublishImageL(NULL,EMusicWidgetImage1,KEmpty);
        	InstallGoToAlbumL( EMusicWidgetTrigger1 );
            PublishTextL( NULL,EMusicWidgetText1, KEmpty );
            PublishImageL(NULL,EMusicWidgetToolbarB1,KEmpty);
            PublishImageL(NULL,EMusicWidgetToolbarB2,KEmpty);
            PublishImageL(NULL,EMusicWidgetToolbarB3,KEmpty);
            InstallEmptyActionL(EMusicWidgetTB1Trigger);
            InstallEmptyActionL(EMusicWidgetTB2Trigger);
            InstallEmptyActionL(EMusicWidgetTB3Trigger);
            PublishTextL( NULL,EMusicWidgetDefaultText, iGoToMusicBuffer->Des() );
			InstallGoToAlbumL( EMusicWidgetTrigger2 );
            }
        else
            {
            delete iInstanceId;
            iInstanceId = NULL;
            iInstanceId = aContentId.AllocL();
            }
        }
    else if ( aTrigger ==  KMyDeActive)
        {
        MPX_DEBUG1("CMusicContentPublisher::HandlePublisherNotificationL deactivate");
        //Removing al the CPS entrys to prevent flicker of old text and unwanted images (old/expired handles).
        RemoveL( EMusicWidgetImage1 );
        RemoveL( EMusicWidgetText1 );
        RemoveL( EMusicWidgetToolbarB1 );
        RemoveL( EMusicWidgetToolbarB2 );
        RemoveL( EMusicWidgetToolbarB3 );
        RemoveL( EMusicWidgetDefaultText );
        }
    else if ( aTrigger ==  KMySuspend)
        {
        MPX_DEBUG1("CMusicContentPublisher::HandlePublisherNotificationL suspend");
        iIsPublisherActive = EFalse;
        }
    else if ( aTrigger ==  KMyResume)
        {
        MPX_DEBUG1("CMusicContentPublisher::HandlePublisherNotificationL resume");
        iIsPublisherActive = ETrue;
        DoPublishL();
        }
    MPX_DEBUG1("CMusicContentPublisher::HandlePublisherNotificationL --->");
    }


//  End of File

