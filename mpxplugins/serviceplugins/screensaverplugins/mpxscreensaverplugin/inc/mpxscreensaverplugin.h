/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MPX Screen Saver Plugin implementation
*
*/


#ifndef C_CMPXSCREENSAVERPLUGIN_H
#define C_CMPXSCREENSAVERPLUGIN_H

// INCLUDES
#include <ScreensaverpluginIntDef.h>
#include <screensaverplugin.h>
#include <mpxplaybackobserver.h>

// CLASS DECLARATION
class CEikonEnv;
class MMPXPlaybackUtility;
class MAknsSkinInstance;
class TAknsItemID;
class CMPXScreenSaverPluginContainer;
class MProfileEngine;
class MProfile;
    
/**
*  Screen saver plugin for music player now playing view
*  @lib mpxscreensaverplugin.lib
*  @since 3.0
*/
 
class CMPXScreenSaverPlugin: public CScreensaverPluginInterfaceDefinition,
                             public MMPXPlaybackObserver,
                             public MMPXPlaybackCallback
{    
public: 

    /**
    * Two-phased constructor.
    * @since 3.1
    */
    static CMPXScreenSaverPlugin* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CMPXScreenSaverPlugin();     
    
public: // Functions From CScreensaverPluginInterfaceDefinition        
    
    /**
    * From MScreensaverPlugin, Method called by ScreenSaver Application
    * when an animation file is configured as a Screen Saver.
    * @param aHost  Screen saver plugin host.
    * @since 3.1
    */
    virtual TInt InitializeL( MScreensaverPluginHost *aHost );
        
    /**
    * From MScreensaverPlugin, Method called by ScreenSaver Application
    * when Screen Saver is activated.
    * @since 3.1
    */
    virtual TInt Draw( CWindowGc& aGc );
    
    /**
    * From MScreensaverPlugin, Method called by ScreenSaver Application
    * before the InitializeL.
    * @since 3.1
    */
    virtual const TDesC16& Name() const;
    
    /**
    * From MScreensaverPlugin, Method called by ScreenSaver Application
    * to get the capabilities of Screensaver plugin.
    * @since 3.1
    */
    virtual TInt Capabilities();
    
    /**
    * From MScreensaverPlugin, Method called by Psln Application
    * when the user selects Animated ScreenSaver 
    * @since 3.1
    */
    virtual TInt PluginFunction(
            TScPluginCaps /*aFunction*/,
            TAny* /*aParam*/ );

    /**
    * From MScreensaverPlugin, Method called by ScreenSaver Application
    * when there are Screen Saver events.
    * @since 3.1
    */
    virtual TInt HandleScreensaverEventL(
            TScreensaverEvent aEvent,
            TAny* aData );       
    
private:

    /**
    * Default Constructor
    * @since 3.1
    */  
    CMPXScreenSaverPlugin();
    
    /**
    * 2nd phase constructor
    * @since 3.1
    */
    void ConstructL();

// from base class MMPXPlaybackObserver

    /**
     * From MMPXPlaybackObserver
     * Handle playback message
     * 
     * @param aMessage playback message
     * @param aErr system error code.
     */
    void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

// from base class MMPXPlaybackCallback

    /**
     * From MMPXPlaybackCallback
     * Handle playback property
     *
     * @param aProperty the property
     * @param aValue the value of the property
     * @param aError error code
     */
    void HandlePropertyL(
        TMPXPlaybackProperty aProperty,
        TInt aValue, 
        TInt aError );
    
    /**
     * From MMPXPlaybackCallback
     * Method is called continously until aComplete=ETrue, signifying that 
     * it is done and there will be no more callbacks
     * Only new items are passed each time
     *
     * @param aPlayer UID of the subplayer
     * @param aSubPlayers a list of sub players
     * @param aComplete ETrue no more sub players. EFalse more subplayer
     *                  expected
     * @param aError error code
     */
    void HandleSubPlayerNamesL(
        TUid aPlayer, 
        const MDesCArray* aSubPlayers,
        TBool aComplete,
        TInt aError );
    
    /**
     * From MMPXPlaybackCallback
     * Handle extended media properties
     *
     * @param aMedia media 
     * @param aError error code    
     */
    void HandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );     
    
    /**
     *  Handle playback message
     *
     *  @param aMessage playback message
     */
    void DoHandlePlaybackMessageL( 
        const CMPXMessage& aMessage );
    
    /**
     *  Handle media properties.
     *  
     *  @param aProperties media properties
     *  @param aError error code    
     */
    void DoHandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     */
    void DoHandleStateChangedL(
        TMPXPlaybackState aState );       
        
    /**
     *  Creates the container
     */
    void CreateContainerL();    
           
private:
    // Pointer to the screen saver host
    MScreensaverPluginHost*     iScreenSaverHost;        

    CEikonEnv*                  iEikEnv;  
    MMPXPlaybackUtility*        iPlaybackUtility;  // not own
    CMPXScreenSaverPluginContainer*     iContainer;  
    MProfileEngine*             iProfileEngine; // not own
    
    TMPXPlaybackState           iPlaybackState;
    TInt                        iResourceOffset;          
    TBool                       iTimerChanged;           
    
    HBufC*                      iDisplayName;
};  

#endif // C_CMPXSCREENSAVERPLUGIN_H

