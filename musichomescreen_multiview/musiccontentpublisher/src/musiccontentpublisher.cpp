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

// These extend macros in <mpxlog.h>

#ifdef _DEBUG
#define MPX_DEBUG2_8(cstr, p) RDebug::Printf(cstr, p)
#else
#define MPX_DEBUG2_8(cstr, p)
#endif

_LIT( KPubData,        "publisher" );

_LIT8( KMyActive, "active" );
_LIT8( KMyDeActive, "deactive");
_LIT8( KMySuspend, "suspend");
_LIT8( KMyResume, "resume");
_LIT8( KMyPluginshutdown, "pluginshutdown");
_LIT8( KMySystemshutdown, "systemshutdown");
_LIT8( KMyActionMap, "action_map" );
_LIT8( KMyItem, "item" );
_LIT8( KMyAdd, "Add" );
_LIT8( KMyMusic, "music");
_LIT8( KMyItemId, "item_id" );
_LIT( KMyActionName, "data" );

_LIT( KEmpty, "" );

_LIT( KMWPublisher, "MWPublisher");
_LIT( KactionMessageToMusicPlayer, "MessageToMusicPlayer" );
_LIT( KGoToAlbumView, "GoToAlbumView" );
_LIT8( KMessage, "message" );

//for application launcher AHPlugin
const TInt KMmUid3AsTInt( 0x101f4cd2 );
const TInt KMSGUidAsTInt( 0x10003A39 );
const TInt KDelayTime( 6000000 ); // CPeriodic timer
_LIT8( KAdditionalData, "additional_data" );
_LIT( KLaunchApp, "launch_application" );
_LIT( KMessageWithTail, "message_with_tail" );
_LIT8( KMessageForMMOpenMusicSuiteWithHide, "mm://root/musicsuite?exit=hide");

_LIT( KResourceFile, "z:musichomescreen.rsc");

// ======== MEMBER FUNCTIONS ========

CMusicContentPublisher::TDestinationItem::TDestinationItem(const TMCPDestinationItem& aItem):
    iType( reinterpret_cast<const TUint16*>(aItem.type) ),
    iDataKey ( reinterpret_cast<const TUint8*>(aItem.dataKey ) ),
    iMaskKey ( reinterpret_cast<const TUint8*>(aItem.maskKey ) )
    {}


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMusicContentPublisher::CMusicContentPublisher( MLiwInterface* aCPSInterface ):
        iCPSInterface (aCPSInterface)
    {
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
                                TDestinationItem(KMCPDestinationInfo[i]));
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
    GetLocalizedStringL(resourceFile, iGoToMusicBuffer, R_MUSICHOMESCREEN_GO_TO_MUSIC);
    CleanupStack::PopAndDestroy(&resourceFile);
    CleanupStack::PopAndDestroy(&fs);
    MPX_DEBUG1("CMusicContentPublisher::ConstructL resources loaded");
    
    // connect to the skin server, to receive skin changed event.
    iAknsSrvSession.Connect(this);
    
    // enable skin.
    AknsUtils::InitSkinSupportL();
    	
    iDeferredPublish = new (ELeave) CAsyncCallBack(TCallBack(DoPublish, this), 
                                                   CActive::EPriorityLow);

    MPX_DEBUG1("CMusicContentPublisher::ConstructL subscribing to observer");
 
    // Observer must be registered before the publisher
    // When publisher is registered, HS gets the notification and starts sending the events immediately.
    // Late registration of observer causes missing events.

    // 1. Register the Observer
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant( KMWPublisher ) );
    filter->InsertL( KContentId, TLiwVariant( KAll) );
    filter->InsertL( KContentType, TLiwVariant( KAll ) );
    iHPObserver = CMCPHarvesterPublisherObserver::NewL(this);
    iHPObserver->RegisterL(filter);
    CleanupStack::PopAndDestroy(filter);

    // 2. Register the Publisher 
    TUint id = RegisterPublisherL( 
            KMWPublisher,
            KAll, 
            KAll );
    
    MPX_DEBUG1("CMusicContentPublisher::ConstructL initializing content");
    PublishDefaultL();
    iUpdateIdle = CPeriodic::NewL(CActive::EPriorityStandard);
    
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
    delete iDeferredPublish;
    if(iHPObserver)
        {
        MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher deleting observer");
        delete iHPObserver; 
        }
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
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher resetting publishing maps");
    iPublishingDataMap.Close();
    iPublishingActionMap.Close();
    MPX_DEBUG1("CMusicContentPublisher::~CMusicContentPublisher --->");
    if ( iInstanceId )
    	{
    	delete iInstanceId;
    	}
    
    if ( iUpdateIdle )
        {
        iUpdateIdle->Cancel();
        delete iUpdateIdle;
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishDefaultL()
    {
    MPX_FUNC("CMusicContentPublisher::PublishDefaultL");
    PublishImageL(NULL,EMusicWidgetImage1,KEmpty);
    InstallGoToAlbumL( EMusicWidgetTrigger1 );
    PublishTextL( NULL,EMusicWidgetDefaultText, *iGoToMusicBuffer );
    PublishTextL( NULL,EMusicWidgetText1, KEmpty );
    PublishImageL(NULL,EMusicWidgetToolbarB1,KEmpty);
    PublishImageL(NULL,EMusicWidgetToolbarB2,KEmpty);
    PublishImageL(NULL,EMusicWidgetToolbarB3,KEmpty);
    InstallEmptyActionL(EMusicWidgetTB1Trigger);
    InstallEmptyActionL(EMusicWidgetTB2Trigger);
    InstallEmptyActionL(EMusicWidgetTB3Trigger);
    InstallGoToAlbumL( EMusicWidgetTrigger2 );
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
// Trigger deferred publishing through an active object
// ----------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishDeferred()
    {
    MPX_FUNC("CMusicContentPublisher::PublishDeferred");
    if ( iWidgetForeground )
        {
        iDeferredPublish->CallBack();
        }
    }

// ----------------------------------------------------------------------------
// Run deferred publishing
// ----------------------------------------------------------------------------
//
 TInt CMusicContentPublisher::DoPublish(TAny * aMusicContentPublisher)
    {
    MPX_FUNC("CMusicContentPublisher::DoPublish");
    CMusicContentPublisher * self = static_cast<CMusicContentPublisher*>(aMusicContentPublisher);
    if ( self->iWidgetForeground )
        {
        TRAPD(err, self->DoPublishModifiedL());
        if (err != KErrNone)
            {
            MPX_DEBUG2("CMusicContentPublisher::DoPublish failed, err=%d", err);
            }
        }    
    return KErrNone;
    }
     
// ----------------------------------------------------------------------------
// Publish all content
// ----------------------------------------------------------------------------
//
void CMusicContentPublisher::DoPublishAllL()
    {
    MPX_FUNC("CMusicContentPublisher::DoPublishAllL");
    if (iCPSInterface && iInstanceId)
        {
        CLiwMap * datamap = iPublishingDataMap.GetAllLC();
        CLiwMap * actionmap = iPublishingActionMap.GetAllLC();

        CLiwDefaultMap * cpdata = CLiwDefaultMap::NewLC( );
        cpdata->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdata->InsertL( KContentType , TLiwVariant( KMyMusic ) );
        cpdata->InsertL( KContentId  , TLiwVariant( iInstanceId ) );
        cpdata->InsertL( KDataMap  , TLiwVariant( datamap ) );
        cpdata->InsertL( KActionMap  , TLiwVariant( actionmap ) );

        MPX_DEBUG2("CMusicContentPublisher::DoPublishAllL Add in %S", iInstanceId);
        DoPublishCmdL( KAdd, KItem, cpdata );
        
        CleanupStack::PopAndDestroy( cpdata );
        CleanupStack::PopAndDestroy( actionmap );
        CleanupStack::PopAndDestroy( datamap );
        }
    }

// ----------------------------------------------------------------------------
// Publish modified content
// ----------------------------------------------------------------------------
//
void CMusicContentPublisher::DoPublishModifiedL()
    {
    MPX_FUNC("CMusicContentPublisher::DoPublishModifiedL");
    if (iCPSInterface && iInstanceId )
        {
        // Must re-publish all data and actions even if only one of them has changed,
        // otherwise non-modified data might go missing        
        CLiwMap * datamap = NULL;
        if ( iPublishingDataMap.IsModified() )
            {
            datamap = iPublishingDataMap.GetAllLC();
            }

        CLiwMap * actionmap = NULL;
        if ( iPublishingActionMap.IsModified() )
            {
            actionmap = iPublishingActionMap.GetAllLC();
            }
        if ( datamap || actionmap )
            {
            CLiwMap * cpdata = CLiwDefaultMap::NewLC( );
            cpdata->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
            cpdata->InsertL( KContentType , TLiwVariant( KMyMusic ) );
            cpdata->InsertL( KContentId  , TLiwVariant( iInstanceId ) );
            if ( datamap && datamap->Count() )
                {
                cpdata->InsertL( KDataMap  , TLiwVariant( datamap ) );
                }
            if ( actionmap && actionmap->Count() )
                {
                cpdata->InsertL( KActionMap  , TLiwVariant( actionmap ) );
                }

            MPX_DEBUG2("CMusicContentPublisher::DoPublishModifiedL Add in %S", iInstanceId);
            DoPublishCmdL( KAdd, KItem, cpdata );
        
            CleanupStack::PopAndDestroy( cpdata );
            }
        if (actionmap)
            {
            CleanupStack::PopAndDestroy( actionmap );
            }
        if (datamap)
            {
            CleanupStack::PopAndDestroy( datamap );
            }
        }
    }

// ---------------------------------------------------------------------------
// Remove all content
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::DoPublishDeleteAllL()
    {
    MPX_FUNC("CMusicContentPublisher::DoPublishDeleteAllL");
    if( iCPSInterface && iInstanceId)
        {
        CLiwDefaultMap * cpdata = CLiwDefaultMap::NewLC( );
        cpdata->InsertL( KPublisherId , TLiwVariant( KMWPublisher ) );
        cpdata->InsertL( KContentType , TLiwVariant( KAll ) );
        cpdata->InsertL( KContentId  , TLiwVariant( iInstanceId ) );

        MPX_DEBUG2("CMusicContentPublisher::DoPublishDeleteAllL Delete All in %S", iInstanceId);
        DoPublishCmdL( KDelete, KFilter, cpdata );

        CleanupStack::PopAndDestroy( cpdata );
        }  
    }
    
// ---------------------------------------------------------------------------
// Execute a CMD to iCPSInterface
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::DoPublishCmdL(const TDesC8& aCmd, 
	                                         const TDesC8& aKey, 
	                                         const CLiwMap * aValue)
    {
    MPX_FUNC("CMusicContentPublisher::DoPublishCmdL");
    CLiwGenericParamList * inParam = CLiwGenericParamList::NewLC();
    TLiwGenericParam cptype( KType , TLiwVariant( KCpData ) );
    inParam->AppendL( cptype );
    TLiwGenericParam item( aKey, TLiwVariant( aValue ) ); 
    inParam->AppendL( item );

    CLiwGenericParamList * outParam = CLiwGenericParamList::NewLC();

    iCPSInterface->ExecuteCmdL( aCmd , *inParam, *outParam);

    CleanupStack::PopAndDestroy( outParam );
    CleanupStack::PopAndDestroy( inParam );
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
    PublishDefaultL();
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
// 
// ---------------------------------------------------------------------------
//
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
// Publishes an image from path to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishImageL( CMCPPlugin* aPlugin,  
        TMCPImageDestination aDestination, 
        const TDesC& aImagePath )
    {
    MPX_FUNC("CMusicContentPublisher::PublishImageL");
    if ( aDestination >= EMusicMenuMusicInfoImage1 )
        {
        MPX_DEBUG2("CMusicContentPublisher::PublishImageL deprecated aDestination=%d", aDestination);
        return;
        }

    if( iCPSInterface && iActivePlugin == aPlugin )
        {
        TDestinationItem & destination (iDestinationMap.FindL( aDestination ));
        iPublishingDataMap.SetL( destination.iDataKey, aImagePath );
        iPublishingDataMap.Reset( destination.iMaskKey );
        PublishDeferred();
        }
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
    MPX_FUNC("CMusicContentPublisher::PublishImageL");
    if ( aDestination >= EMusicMenuMusicInfoImage1 )
        {
        MPX_DEBUG2("CMusicContentPublisher::PublishImageL deprecated aDestination=%d", aDestination);
        return;
        }
    if( iCPSInterface && iActivePlugin == aPlugin )
        {
        TDestinationItem & destination (iDestinationMap.FindL( aDestination ));
        
        iPublishingDataMap.SetL( destination.iDataKey, aBitmapHandle  );
        
        if (aMaskBitmapHandle)
            {
            iPublishingDataMap.SetL( destination.iMaskKey, aMaskBitmapHandle );
            }
        else
            {
            iPublishingDataMap.Reset( destination.iMaskKey );
            }
        PublishDeferred();
        }
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
    MPX_FUNC("CMusicContentPublisher::PublishImageL");
    if ( aDestination >= EMusicMenuMusicInfoImage1 )
        {
        MPX_DEBUG2("CMusicContentPublisher::PublishImageL deprecated aDestination=%d", aDestination);
        return;
        }
        
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
    }

// ---------------------------------------------------------------------------
// Publishes a text from path to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishTextL( CMCPPlugin* aPlugin, 
        TMCPTextDestination aDestination, 
        const TDesC& aText )
    {
    MPX_FUNC("CMusicContentPublisher::PublishTextL");
    if ( aDestination >= EMusicMenuMusicInfoLine1 )
        {
        MPX_DEBUG2("CMusicContentPublisher::PublishTextL deprecated aDestination=%d", aDestination);
        return;
        }

    if( iCPSInterface && iActivePlugin == aPlugin)
        {
        TPtrC8 dataKey (iDestinationMap.FindL(aDestination).iDataKey);
        TPtrC  text;
        text.Set( aText );
        iPublishingDataMap.SetL ( dataKey, text );
        PublishDeferred();
        }  
    }

// ---------------------------------------------------------------------------
// Publishes an action from a trigger map to the required destination
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::PublishActionL( CMCPPlugin* aPlugin, 
        TMCPTriggerDestination aDestination,
        CLiwDefaultMap* aTriggerMap )
    {
    MPX_FUNC("CMusicContentPublisher::PublishActionL");
    if ( aDestination >= EMusicMenuMusicInfoTrigger )
        {
        MPX_DEBUG2("CMusicContentPublisher::PublishActionL deprecated aDestination=%d", aDestination);
        return;
        }

    if( iCPSInterface && iActivePlugin == aPlugin)
        {
        TDestinationItem & destination ( iDestinationMap.FindL( aDestination ) );
        iPublishingActionMap.SetL( destination.iDataKey , aTriggerMap );
        PublishDeferred();
        }
    }

// ---------------------------------------------------------------------------
// Called when a plugin becomes active, it deactivates the previous plugin.
// ---------------------------------------------------------------------------
//
void CMusicContentPublisher::BecameActiveL( CMCPPlugin* aPlugin )
    {
    MPX_FUNC("CMusicContentPublisher::BecameActiveL");
    if ( iActivePlugin != aPlugin )
        {
        if ( iActivePlugin )
            {
            MPX_DEBUG1("CMusicContentPublisher::BecameActiveL deactivating old plugin");
            DoPublishDeleteAllL();
            iActivePlugin->Deactivate();
            iPublishingDataMap.Close();
            iPublishingActionMap.Close();
            }
        iActivePlugin = aPlugin;
        }
    }

// ---------------------------------------------------------------------------
// Callback function for CPeriodic.
// ---------------------------------------------------------------------------
//   
TInt CMusicContentPublisher::UpdateCallback(TAny* aContentPublisher)
    {
    MPX_FUNC("CMusicContentPublisher::UpdateCallback");
    
    CMusicContentPublisher* self = 
            reinterpret_cast<CMusicContentPublisher*> (aContentPublisher);
    self->DoUpdate();
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Create plugin manager.
// ---------------------------------------------------------------------------
//   
void CMusicContentPublisher::DoUpdate()
    {
    MPX_FUNC("CMusicContentPublisher::DoUpdate")
    
    TRAP_IGNORE( iPluginManager = CPluginManager::NewL(
            TUid::Uid( KMCPPluginUid ), 
            static_cast<MMCPPluginObserver*>( this ),
            this ));    
    iUpdateIdle->Cancel();
    }

// ---------------------------------------------------------------------------
// From CContentHarvesterPlugin
// This function updates information in CPS storage
// ---------------------------------------------------------------------------
//    
void CMusicContentPublisher::UpdateL() 
    {
    MPX_FUNC("CMusicContentPublisher::UpdateL");
        
    // Construct plugin manager asynchronously for not to block the caller
    // thread
    if ( !iPluginManager && !iUpdateIdle->IsActive())
        {
        MPX_DEBUG1("CMusicContentPublisher::UpdateL creating the plugin manager");
        
        iUpdateIdle->Start(KDelayTime, KDelayTime, TCallBack(UpdateCallback,this));
        }
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

void CMusicContentPublisher::HandlePublisherNotificationL( const TDesC& aContentId, 
	                                                         const TDesC8& aTrigger )
    {
    MPX_DEBUG2_8("-->CMusicContentPublisher::HandlePublisherNotificationL %S", &aTrigger);
    if ( aTrigger == KMyActive )
        {
        delete iInstanceId;
        iInstanceId = NULL;
        iInstanceId = aContentId.AllocL();
        DoPublishAllL();
        }
    else if ( aTrigger ==  KMyDeActive )
        {
        delete iInstanceId;  
        iInstanceId = NULL;
        }
    else if ( aTrigger ==  KMySuspend && iWidgetForeground)
        {
        iWidgetForeground = EFalse;
        }
    else if ( aTrigger ==  KMyResume && !iWidgetForeground)
        {
        iWidgetForeground = ETrue;
        DoPublishModifiedL();
        }
    else if ( aTrigger == KMyPluginshutdown || aTrigger == KMySystemshutdown )
        {
        DoPublishDeleteAllL();   // Delete the data only if widget is removed or Phone is shutting down.
		}

    MPX_DEBUG1("<--CMusicContentPublisher::HandlePublisherNotificationL");
    }


//  End of File

