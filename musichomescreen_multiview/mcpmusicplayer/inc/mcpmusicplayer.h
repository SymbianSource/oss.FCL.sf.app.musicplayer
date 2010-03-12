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
* Description:  Updates Music Content Plublisher
*
*/


#ifndef C_MCPMUSICPLAYER_H
#define C_MCPMUSICPLAYER_H 

#include <mpxplaybackcommanddefs.h> //for TMPXPlaybackCommand
#include <e32std.h> // for TExitType
#include <mcpplugin.h>
#include <mcppluginobserver.h>
#include <mpxcollectionobserver.h>
#include "aiplayerpluginengineobserver.h"
#include "filemonitorobserver.h"
#include "applicationmonitorobserver.h"
#include "pnsmonitorobserver.h"

class MLiwInterface;
class CLiwGenericParamList;
class CAiPlayerPluginEngine;
class CFileMonitor;
class CApplicationMonitor;
class CPNSMonitor;
class RResourceFile;
class MMPXCollectionUtility;

/**
 *  Music Player MCP plugin.
 *  CMCPMusicPlayer implements CMCPPlugin
 *
 *  @since S60 S60 v5.0
 */
class CMCPMusicPlayer : public CMCPPlugin,
                                public MAiPlayerPluginEngineObserver,
                                public MFileMonitorObserver,
                                public MApplicationMonitorObserver,
                                public MMPXCollectionObserver,
                                public MPNSMonitorObserver
    {
public:
    /**
     * Standard Symbian 2 phase constructor
     */
    static CMCPMusicPlayer* NewL(
            MMCPPluginObserver* aObserver);

    /**
     * Standard C++ destructor.
     */
    virtual ~CMCPMusicPlayer();

 // from base class CMCPPlugin

    /**
     * From CMCPPlugin
     * @since S60 5.0
     */
    void Deactivate();
    
    /**
     * From CMCPPlugin
     * @since S60 5.0
     */
    void SkinChanged();
        

// from base class MAiPlayerPluginEngineObserver
    
    /**
     * From MAiPlayerPluginEngineObserver
     * @since S60 5.0
     * See aiplayerpluginengineobserver.h for detailed description.
     */
    void PlayerStateChanged(TMPlayerState aState);
    
    /**
     * From MAiPlayerPluginEngineObserver
     * @since S60 5.0
     * See aiplayerpluginengineobserver.h for detailed description.
     */
    void TrackInfoChanged(const TDesC& aTitle, const TDesC& aArtist);
    
    /**
     * From MAiPlayerPluginEngineObserver
     * @since S60 5.0
     * See aiplayerpluginengineobserver.h for detailed description.
     */
    void PlaybackPositionChanged(TInt aPosition);
    
    /**
     * From MAiPlayerPluginEngineObserver
     * @since S60 5.0
     * See aiplayerpluginengineobserver.h for detailed description.
     */
    void AlbumArtChanged( CFbsBitmap* aBitmap );
    
    /**
     * From MAiPlayerPluginEngineObserver
     * @since S60 5.0
     * See aiplayerpluginengineobserver.h for detailed description.
     */
    void Opening();
    
    /**
     * From MAiPlayerPluginEngineObserver
     * @since S60 5.0
     * See aiplayerpluginengineobserver.h for detailed description.
     */
    void PlaylisIsEmpty();
    
// from base class MFileMonitorObserver
    /**
     * from MFileMonitorObserver
     * Handle a file system change notification
     *
     */
    void HandleFileRemovedL();
       
// from base class MApplicationMonitorObserver
    /**
     * from MApplicationMonitorObserver
     * Handle an application close event.
     */
    void HandleApplicationClosedL(TExitType aReason);
    
//from base class MMPXCollectionObserver  
    /** 
     *  Handle collection message
     * 
     *  @param aMsg collection message, ownership not transferred. 
     *         Please check aMsg is not NULL before using it. 
     *         If aErr is not KErrNone, plugin might still call back with more
     *         info in the aMsg.
     *  @param aErr system error code.
     */
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );

    /**
     *  Handles the collection entries being opened. Typically called
     *  when client has Open()'d a folder.
     *
     *  @since S60 3.2.3
     *  @param aEntries collection entries opened
     *  @param aIndex focused entry
     *  @param aComplete ETrue no more entries. EFalse more entries
     *                   expected
     *  @param aError error code   
     */
    void HandleOpenL( const CMPXMedia& aEntries, TInt aIndex, TBool aComplete,
            TInt aError );

    /**
     *  Handles the item being opened. Typically called
     *  when client has Open()'d an item. Client typically responds by
     *  'playing' the item via the playlist.
     *
     *  @since S60 3.2.3
     *  @param aPlaylist collection playlist
     *  @param aError error code
     */
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

    /**
     * From MMPXCollectionMediaObserver 
     * It handles MediaL event.
     *
     * @param aMedia object containing collection's informations
     * @param aError error code
     */
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

    /**
     * from MPNSMonitorObserver
     * Handle launching music player on the background.
     */
    void OpeningMusicPlayer();

private:

    /**
     * Standard C++ constructor.
     */
    CMCPMusicPlayer( MMCPPluginObserver* aObserver );

    /**
     * Standard Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * returns ETrue if data can be propagated to the Music Content Publisher.
     */
    TBool IsOKToPublishData();
     
    /**
     * Installs Playback command action, the puppose of this is to improve
     * readability.
     */
    void InstallPlaybackCommandActionL( TMPXPlaybackCommand aCmd,
            TMCPTriggerDestination aDestination );
    /**
     * Installs Launch Music Player action, the puppose of this is to improve
     * readability.
     */
    void InstallLaunchMusicPlayerL( TMCPTriggerDestination aDestination );
    
    /**
     * Installs Go To Now playing action, the puppose of this is to improve
     * readability.
     */
    void InstallGoToNowPlayingL( TMCPTriggerDestination aDestination );
        
    /**
     * Installs Go To Last Played action, the puppose of this is to improve
     * readability.
     */
    void InstallGoToLastPlayedL( TMCPTriggerDestination aDestination, 
             TBool aMinimized );
    
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
     * Installs an empty action, mainly for readability purposes.
     *
     * @since S60 5.0
     * @param aDestination Destination trigger that should launch the action.
     */
    void InstallEmptyActionL(TMCPTriggerDestination aDestination);
    
    /**
     * Installs actions and icons in the toolbar.
     *
     * @since S60 5.0
     */
    void UpdateToolBarL(TUint aToolBarState);
    
    /**
     * Resets the widget an Music Suite.
     *
     * @since S60 5.0
     */
    void ResetL();

    /**
     * Get a heap descriptor from the resource file
     *
     * @since S60 5.0
     */
    void GetLocalizedStringL(RResourceFile& aResourceFile, HBufC*& aRetBuf,
            TInt aResourceId);
    
    
    /**
     * @since S60 5.0
     */
    void ActivateL();
    void DoUpdatePlayerStateL(TMPlayerState aState);
    void DoUpdateTrackInfoL(const TDesC& aTitle, const TDesC& aArtist);
    void DoUpdatePlaybackPositionL(TInt aPosition);
    void DoUpdateAlbumArtL( CFbsBitmap* aBitmap );
    void DoHandleOpeningL();
    void DoHandlePlaylisIsEmptyL();
    void DoHandleSkinChangedL();
    
    /**
     * Logic of HandleCollectionMessageL, to avoid the function leaves
     * 
     * @param aMsg collection message
     */
    void HandleCollectionMessageL( CMPXMessage* aMsg );

    /**
     * Logic of HandleCollectionMessage, to avoid the function leaves
     * 
     * @param aMsg collection message
     */
    void DoHandleGeneralMessageL( const CMPXMessage& aMsg );
       
    /**
    *  Music Player opening timer callback
    *
    *  @param aPtr pointer the this
    */
    static TInt MusicPlayerOpeningTimerCallback( TAny* aPtr );

private: // data

    /**
     * Instance of MCP plugin observer
     * Not owned.
     */
    MMCPPluginObserver* iMHObserver;
    
    /**
     * Instance of CAiPlayerPluginEngine used to observe Music player via 
     * MAiPlayerPluginEngineObserver.
     * Owned.
     */
    CAiPlayerPluginEngine* iEngine;
    
    /**
     * State indicator, the plugin can be active or inactive. 
     */
    TBool iActive;
    
    /**
     * art cached indicator. 
     */
    TBool iArtCached;
    
    /**
     * Used to monitor last played song. 
     */
    CFileMonitor* iFileMonitor;

    /**
     * Used to monitor if music player closes. 
     */
    CApplicationMonitor* iAppMonitor;
    
    /**
     * Used to monitor publish and subscribe property. 
     */
    CPNSMonitor* iPNSMonitor;

    /**
     * Current Album Art. 
     */
    CFbsBitmap* iArtBitmap;
    
    /**
     * The art that was extracted but not used due to the current stateof
     * the plugin (acctive, not active) 
     */
    CFbsBitmap* iCachedArtBitmap;
    
    /**
     * Album Art mask. 
     */
    CFbsBitmap* iArtBitmapMask;
    
    /**
     * Current Toolbar State.
     */  
    TUint iToolBarState;
    
    /**
    * Buffers for the localized strings
    */
    HBufC* iGoToMusicBuffer;
    HBufC* iOpeningBuffer;
    
    /**
     * Flag of Mass Storage mode or MTP mode
     */
    TBool iBlockingOperationOngoing;
   
    /**
     * collection utility
     */
    MMPXCollectionUtility* iCollectionUtility;

    CPeriodic* iMusicPlayerOpeningTimer;
    };

#endif // C_MCPMUSICPLAYER_H

//  End of File
