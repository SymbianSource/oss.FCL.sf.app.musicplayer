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


#ifndef C_MUSICCONTENTPUBLISHER_H
#define C_MUSICCONTENTPUBLISHER_H 

#include <contentharvesterplugin.h>
#include <mcppluginobserver.h>
#include <libc/stddef.h>  // For wchar_t
#include <e32hashtab.h>
#include <AknsSrvClient.h>

class MLiwInterface;
class CLiwGenericParamList;
class CPluginManager;
class CFbsBitmap;
class CCoeEnv;
class TAknsItemID;
class CMCPHarvesterPublisherObserver;


struct TMCPDestinationItem
    {
    const TInt id;
    const wchar_t* type;
    const char* dataKey;
    const wchar_t* content;
    };

struct TMCPImageDestinationInfoItem
    {
    const TMCPImageDestination id;
    const TInt sizex;
    const TInt sizey;
    };


struct TBmpMsk
    {
    CFbsBitmap *bitmap;
    CFbsBitmap *mask;
    };

struct TMyBufferItem
    {
    TAny* buf;
    TInt size;
    };


const TMCPDestinationItem KMCPDestinationInfo[] =
    {
     { EMusicWidgetImage1,      L"area1_image1", "area1_image1_data",       L"*" }
    ,{ EMusicWidgetText1,       L"area2_text1",  "area2_text1_data",        L"*" }
    ,{ EMusicWidgetToolbarB1,   L"toolbar_b1", "toolbar_b1_data",         L"*" }
    ,{ EMusicWidgetToolbarB2,   L"toolbar_b2", "toolbar_b2_data",         L"*" }
    ,{ EMusicWidgetToolbarB3,   L"toolbar_b3", "toolbar_b3_data",         L"*" }
    ,{ EMusicWidgetDefaultText, L"default_text",  "default_text_data",       L"*" }
    
    ,{ EMusicWidgetTrigger1,    L"area1_image1", "MWTrigger1",  L"*" }
    ,{ EMusicWidgetTrigger2,    L"default_text",  "MWTrigger2", L"*" }
    ,{ EMusicWidgetTB1Trigger,  L"toolbar_b1", "TB1Trigger",    L"*" }
    ,{ EMusicWidgetTB2Trigger,  L"toolbar_b2", "TB2Trigger",    L"*" }
    ,{ EMusicWidgetTB3Trigger,  L"toolbar_b3", "TB3Trigger",    L"*" }
            
    ,{ EMusicMenuMusicInfoImage1,   L"image",  "musicmenu_musicinfo_imagedata", L"musicmenu_musicinfo_image" }
    ,{ EMusicMenuMusicInfoLine1,        L"text",   "musicmenu_musicinfo_text1data",  L"musicmenu_musicinfo_text1" }
    ,{ EMusicMenuMusicInfoLine2,        L"text",   "musicmenu_musicinfo_text2data",  L"musicmenu_musicinfo_text2" }
    ,{ EMusicMenuMusicInfoTrigger, L"text",   "MMInfoTrigger",  L"musicmenu_musicinfo_text1" }
    };


const TMCPImageDestinationInfoItem KMCPImageDestinationInfo[] =

    {
        {EMusicWidgetImage1, 70,70}
    ,
        {EMusicWidgetToolbarB1, 41,41}
    ,
        {EMusicWidgetToolbarB2, 41,41}
    ,
        {EMusicWidgetToolbarB3, 41,41}
    ,
        {EMusicMenuMusicInfoImage1, 70,70} 
    };

/**
 *  Music Content Publisher (a Content Harvester plugin)
 *
 *  @lib musiccontentpublisher.dll
 *  @since S60 S60 v5.0
 */
class CMusicContentPublisher : public CContentHarvesterPlugin
                       ,public MMCPPluginObserver
                       ,public MAknsSkinChangeObserver
    {

public:

// from base class MMCPPluginObserver
    /**
     * from MMCPPluginObserver
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aImagePath Source path of the Image.
     */
    void PublishImageL(CMCPPlugin* aPlugin,
            TMCPImageDestination aDestination, const TDesC& aImagePath);

    /**
     * from MMCPPluginObserver
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aBitmapHandle Bitmap handle to the image.
     * @param aMaskBitmapHandle Mask Bitmap handle for the image.
     */
    void PublishImageL(CMCPPlugin* aPlugin, 
            TMCPImageDestination aDestination,
            TInt aBitmapHandle,
            TInt aMaskBitmapHandle = NULL);
    
    /**
     * from MMCPPluginObserver
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aID major and minor Skin ID info.
     * @param aFilename fallback mif/mbm file.
     * @param aFileBitmapId bitmap id within the fallback file.
     * @param aFileMaskId mask id within the fallback file.
     */
    void PublishImageL(CMCPPlugin* aPlugin, 
            TMCPImageDestination aDestination,
            const TAknsItemID& aID,
            const TDesC& aFilename,
            const TInt aFileBitmapId,
            const TInt aFileMaskId );

    /**
     * from MMCPPluginObserver
     * Called when a text needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the text is to be placed.
     * @param aText Descriptor containing the text.
     */
    void PublishTextL( CMCPPlugin* aPlugin, 
            TMCPTextDestination aDestination, const TDesC& aText );

    /**
     * from MMCPPluginObserver
     * Called when an action needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Destination trigger that should launch the action.
     * @param aTrigerMap LIW Map containint the action that can be interpreted
     * by the specified action handler plugin.
     */
    void PublishActionL( CMCPPlugin* aPlugin, 
            TMCPTriggerDestination aDestination, 
            CLiwDefaultMap* aTrigerMap );
    
    /**
     * from MMCPPluginObserver
     * Called when a plugin becomes active.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     */
    void BecameActiveL( CMCPPlugin* aPlugin );
    
    /**
     * from MMCPPluginObserver
     * Called when a plugin becomes active.
     *
     * @since S60 5.0
     * @param aDst The image destination.
     * @return The size of the image destination.
     */
    TSize GetImageDestinationSize(TMCPImageDestination aDst);
    
    /**
     * From MAknsSkinChangeObserver.
     * Called by the skin server when skin content is changed and the
     * connected client wants to be informed.
     *
     * @since 2.0
     */
    void SkinContentChanged();

    /**
     * From MAknsSkinChangeObserver.
     * Called by the skin server to indicate that the current skin
     * configuration has changed.
     *
     * @param aReason Reason code.
     *
     * @since 2.5
     */
    void SkinConfigurationChanged(
            const TAknsSkinStatusConfigurationChangeReason /*aReason*/){};

    /**
     * From MAknsSkinChangeObserver.
     * Called by the skin server to indicate that the available
     * skin packages have changed.
     *
     * @param aReason Reason code.
     *
     * @since 2.5
     */
    void SkinPackageChanged(const TAknsSkinStatusPackageChangeReason /*aReason*/){};

    void HandlePublisherNotificationL( const TDesC& aContentId, const TDesC8& aTrigger );
    /**
     * Two-phased constructor.
     */
    static CMusicContentPublisher* NewL( MLiwInterface* aCPSInterface );



    // from base class CContentHarvesterPlugin
    /**
     * From CContentHarvesterPlugin
     * This is called by the Content Harvester Server to signal an update. 
     */
    void UpdateL();
    
    /**
     * Destructor.
     */
    virtual ~CMusicContentPublisher();
    
    /**
     * Resets the destinations with default values and removes active plugin.
     */
    void ResetL();

private:

    /**
     * Default constructor.
     */
    CMusicContentPublisher( MLiwInterface* aCPSInterface );

    /**
     * Perform the second phase construction 
     */
    void ConstructL();
    
    /**
     * Installs an action, mainly for readability purposes.
     *
     * @since S60 5.0
     * @param aDestination Destination trigger that should launch the action.
     */
    void InstallGoToMusicL(TMCPTriggerDestination aDestination);
    
    /**
     * Installs an action, mainly for readability purposes.
     *
     * @since S60 5.0
     * @param aDestination Destination trigger that should launch the action.
     */
    void InstallGoToAlbumL(TMCPTriggerDestination aDestination);
    
    /**
     * Installs an rmpty action, mainly for readability purposes.
     *
     * @since S60 5.0
     * @param aDestination Destination trigger that should launch the action.
     */
    void InstallEmptyActionL(TMCPTriggerDestination aDestination);
    
    
    /**
     * Maps an enum from TMCPImageDestination, TMCPTextDestination and 
     * TMCPTriggerDestination to it's keys on CPS.
     * @param aEnum the enum/destination
     * @param aType the type key is loaded here
     * @param aDataKey the data key is loaded here
     * @param aContent the content key is loaded here
     *
     * @since S60 5.0
     */
    void MapEnumToDestinationInfoL(TInt aEnum,TPtrC& aType, TPtrC8& aDataKey, 
            TPtrC& aContent);
    
    /**
     * Resets the Bitmap cache.
     *
     * @since S60 5.0
     */
    void ResetBitmapCache();
    
    /**
     * Resets the publishing buffers.
     *
     * @since S60 5.0
     */
    void ResetPublishingBuffers();
    
    TUint RegisterPublisherL( 
        const TDesC& aPublisherId, 
        const TDesC& aContentId,
        const TDesC& aContentType );
    
    TUint CMusicContentPublisher::ExtractItemId( const CLiwGenericParamList& aInParamList );
    
    void RemoveL( TInt aDestination );
    
    /**
     * Get a heap descriptor from the resource file
     *
     * @since S60 5.0
     */
    void GetLocalizedStringL(RResourceFile& aResourceFile, HBufC*& aRetBuf,
            TInt aResourceId);
    
    void DoPublishL();
    

private: // data

    /**
     * Instance of CPS interface used for update with CPS.
     * Not own
     */
    MLiwInterface* iCPSInterface;

    /**
     * Creates the plugins and manages the plugins table.
     * Own.
     */
    CPluginManager* iPluginManager;
    
    /**
     * Pointer to the active plugin.
     * Not own.
     */    
    CMCPPlugin* iActivePlugin;
    
    /**
     * mapping for the destination translation.
     */    
    RHashMap<TInt, TMCPDestinationItem> iDestinationMap;
    
    /**
     * mapping for the destination  image info translation.
     */    
    RHashMap<TInt, TSize> iImageDestinationSizeMap;
    
    /**
     * bitmap cache map.
     */    
    RHashMap<TInt, TBmpMsk> iBitmapCache;
    
    /**
     * To keep the unpublished data.
     */
    RHashMap<TInt, TMyBufferItem> iPublishingDataBuffers;
    
    /**
     * To keep the unpublished actions.
     */
    RHashMap<TInt, TMyBufferItem> iPublishingActionBuffers;
    
    /**
    * Skin server session.
    */
    RAknsSrvSession iAknsSrvSession;
    
    /**
    * Buffers for the localized strings
    */
    HBufC* iNowPlayingBuffer;
    HBufC* iLastPlayedBuffer;
    HBufC* iGoToMusicBuffer;

    CMCPHarvesterPublisherObserver* iHPObserver;
    TBool iIsPublisherActive;

    HBufC* iInstanceId;
   
    };

#endif // C_MUSICCONTENTPUBLISHER_H

//  End of File
