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


#ifndef M_MCPPLUGINOBSERVER_H
#define M_MCPPLUGINOBSERVER_H

class CLiwDefaultMap;
class CMCPPlugin;
class TAknsItemID;


/**
 * Music Widget and Music Suite Image Destinations.
 * 
 * 9.1 Music Widget has two panels, The left panel contains an image:
 * TMCPImageDestination:EMusicWidgetImage1.
 * If we publish an empty image here the default icon will be displayed.
 * The right panel contains a toolbar with 3 buttons:
 * TMCPImageDestination:EMusicWidgetToolbarB1, 
 * TMCPImageDestination:EMusicWidgetToolbarB2,
 * TMCPImageDestination:EMusicWidgetToolbarB3.
 * 
 * 9.1 Music Suite has a top row that displays information about the current
 * playing and last played. This row contains an image:
 * TMCPImageDestination:EMusicMenuMusicInfoImage1
 */
enum TMCPImageDestination
    {
     EMusicWidgetImage1 = 1000
     ,EMusicWidgetToolbarB1
     ,EMusicWidgetToolbarB2
     ,EMusicWidgetToolbarB3
     
     ,EMusicMenuMusicInfoImage1 = 1100
    };

/**
 * Music Widget and Music Suite Text Destinations.
 * 
 * 9.1 Music Widget has two panels, The right panel contains two text items:
 * TMCPTextDestination:EMusicWidgetText1, 
 * TMCPTextDestination:EMusicWidgetDefaultText.
 * If we publish a EMusicWidgetDefaultText the toolbar will not be displayed.
 * To show the toolbar and the text information row : EMusicWidgetText1 we 
 * should publish empty text to the EMusicWidgetDefaultText
 * 
 * 9.1 Music Suite has a top row that displays information about the current
 * playing and last played. This row contains two lines:
 * TMCPTextDestination:EMusicMenuMusicInfoLine1
 * TMCPTextDestination:EMusicMenuMusicInfoLine2
 */
enum TMCPTextDestination
    {
     EMusicWidgetText1 = 2000
     ,EMusicWidgetDefaultText
     
     ,EMusicMenuMusicInfoLine1 = 2100
     ,EMusicMenuMusicInfoLine2
    };


/**
 * Music Widget and Music Suite Trigger Destinations.
 * 
 * 9.1 Music Widget has two panels, The left panel contains an image where we
 * can install an action: 
 * TMCPTriggerDestination:EMusicWidgetTrigger1
 * When the toolbar is hiden because there is a default text the panel
 * functions as a button where we can install:
 * TMCPTriggerDestination:EMusicWidgetTrigger2
 * When there is no default text on the right panel it contains a toolbar with
 * 3 buttons where we can install the following actions:
 * TMCPTriggerDestination:EMusicWidgetTB1Trigger, 
 * TMCPTriggerDestination:EMusicWidgetTB2Trigger,
 * TMCPTriggerDestination:EMusicWidgetTB3Trigger.
 * 
 * 9.1 Music Suite has a top row that displays information about the current
 * playing and last played, when this item is selected the following action
 * will be triggered:
 * TMCPTriggerDestination:EMusicMenuMusicInfoTrigger
 */
enum TMCPTriggerDestination
    {
     EMusicWidgetTrigger1 = 3000
     ,EMusicWidgetTrigger2
     ,EMusicWidgetTB1Trigger
     ,EMusicWidgetTB2Trigger
     ,EMusicWidgetTB3Trigger
     
     ,EMusicMenuMusicInfoTrigger =3100
    };

/**
 *  Observer interface, to be implemented by the Music Content Publisher.
 *  This interface is to be used to inform the music content publisher of
 *  changes on the plugin that need to be propagated to the Music Widget UI
 *  or the Music suite UI. 
 *  @since S60 5.0
 */
class MMCPPluginObserver
    {
public:
    
    /**
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aImagePath Source path of the Image.
     */
    virtual void PublishImageL(CMCPPlugin* aPlugin,
            TMCPImageDestination aDestination, 
            const TDesC& aImagePath) = 0;

    /**
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aBitmapHandle Bitmap handle to the image.
     *        the bitmaps should be preseverd by the plugins.
     * @param aMaskBitmapHandle Mask Bitmap handle for the image.
     */
    virtual void PublishImageL(CMCPPlugin* aPlugin, 
            TMCPImageDestination aDestination,
            TInt aBitmapHandle,
            TInt aMaskBitmapHandle = NULL) = 0;
    
    /**
     * Called when an image needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the image is to be placed.
     * @param aID major and minor Skin ID info.
     * @param aFilename fallback mif/mbm file.
     * @param aFileBitmapId bitmap id within the fallback file.
     * @param aFileMaskId mask id within the fallback file.
     * NOTE:This keeps a cache of the skined incons, sizes are 
     *      calculated on the observer based on the widget version.
     */
    virtual void PublishImageL(CMCPPlugin* aPlugin, 
            TMCPImageDestination aDestination,
            const TAknsItemID& aID,
            const TDesC& aFilename,
            const TInt aFileBitmapId,
            const TInt aFileMaskId ) = 0;

    /**
     * Called when a text needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Where the text is to be placed.
     * @param aText Descriptor containing the text.
     */
    virtual void PublishTextL( CMCPPlugin* aPlugin, 
            TMCPTextDestination aDestination, 
            const TDesC& aText ) = 0;

    /**
     * Called when an action needs to be updated.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     * @param aDestination Destination trigger that should launch the action.
     * @param aTrigerMap LIW Map containint the action that can be interpreted
     * by the specified action handler plugin.
     */
    virtual void PublishActionL( CMCPPlugin* aPlugin, 
            TMCPTriggerDestination aDestination, 
            CLiwDefaultMap* aTrigerMap ) = 0;
    
    /**
     * Called when a plugin becomes active.
     *
     * @since S60 5.0
     * @param aPlugin Where the mesage is coming from.
     */
    virtual void BecameActiveL( CMCPPlugin* aPlugin ) = 0;
    
    /**
     * Called when a plugin becomes active.
     *
     * @since S60 5.0
     * @param aDst The image destination.
     * @return The size of the image destination.
     * NOTE: this should be used to query the size of the image destination.
     *       Resizing is responsibility of the plugin in the case of 
     *       bitmap handle publishing.
     */
    virtual TSize GetImageDestinationSize(TMCPImageDestination aDst) = 0;

    };

#endif /*MCPPLUGINOBSERVER_H*/
