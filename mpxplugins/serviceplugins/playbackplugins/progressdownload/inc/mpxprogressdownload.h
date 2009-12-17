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
* Description:  This class manages progress download
*
*/



#ifndef CMPXPROGRESSDOWNLOAD_H
#define CMPXPROGRESSDOWNLOAD_H

// INCLUDES
#include <ProgressiveDownloadUtility.h>
#include <MAudioPdPlayUtility.h>
#include <mpxplaybackplugin.h>
#include <mpxcenrepobserver.h>
#include <mpxplaybackcommanddefs.h>

// FORWARD DECLARATIONS
class CMPXDrmMediaUtility;
class CMPXCenRepWatcher;

// CONSTANTS

// CLASS DECLARATION

/**
*  CMPXProgressDownload class
*
*  Plug-in basically handles download via S60's progress download utility.
*  @lib LocalAudioPlayback.lib
*/
NONSHARABLE_CLASS(CMPXProgressDownload) : public CMPXPlaybackPlugin,
                                          public MAudioPdPlayUtilityCallback,
                                          public MMPXCenRepObserver
    {
    private:
        enum TMPXLocalAudioPlaybackState
            {
            EStateNotInitialised,
            EStateInitialising,
            EStateInitialised
            };
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aInitParams, initialization parameter
        * @return a pointer to the created instance
        */
        static CMPXProgressDownload* NewL(TAny* aInitParams);

        /**
        * Destructor
        * Destroy the object and release all memory objects
        */
        ~CMPXProgressDownload();

    private: // Functions from base classes
        /**
        *  from CMPXPlaybackPlugin
        */

        /**
        * Initializes a song for playback
        * @param aSong the song path
        */
        void InitialiseL(const TDesC& aSong);

        /**
        * Initializes a song for playback
        * @param aFile file handle of a song
        */
        void InitialiseL(RFile& aFile);

        /**
        * Executes a command on the selected song
        * @param aCmd a command
        * @param aData data (optional)
        */
        void CommandL(TMPXPlaybackCommand aCmd, TInt aData=0);

        /**
        * Executes a command
        * @param aCmd Command
        */
        void CommandL(CMPXCommand& aCmd);

        /**
        * Sets a property of the plugin
        * @param aProperty a property
        * @param aValue the value of the setting
        */
        void SetL(TMPXPlaybackProperty aProperty,TInt aValue);

        /**
        * Gets a property of the plugin (async)
        * @param aProperty a property
        */
        void PropertyL(TMPXPlaybackProperty aProperty) const;

        /**
        * Gets a list of sub players
        * @return a list of names of sub players
        */
        void SubPlayerNamesL();

        /**
        * Select a sub player
        * @param aIndex index to the sub player
        */
    	void SelectSubPlayerL(TInt aIndex);

        /**
        * Returns current sub player name
        * @return friendly name of the current the sub player
        */
        const TDesC& SubPlayerName();

        /**
        * Current sub player index
        * @return index to the sub player
        */
        TInt SubPlayerIndex() const;

        /**
        * Media properties of the current file (async)
        * @param aAttrs attributes requested
        */
        void MediaL(const TArray<TMPXAttribute>& aAttrs);

        /**
        * Cancel async request
        */
        void CancelRequest();

        /**
        *  From MMdaAudioPlayerCallback which is the base class of
        *  MAudioPdPlayUtilityCallback
        */
        void MapcInitComplete(TInt aError,
                              const TTimeIntervalMicroSeconds& aDuration);

        /**
        *  From MMdaAudioPlayerCallback which is the base class of
        *  MAudioPdPlayUtilityCallback
        */
        void MapcPlayComplete(TInt aError);

        /**
        * MProgressiveDownloadUtilityCallback which is the base class of
        * MAudioPdPlayUtilityCallback
        */
        void Paused();

        /**
        * MProgressiveDownloadUtilityCallback which is the base class of
        * MAudioPdPlayUtilityCallback
        */
        void Playing();

        /**
        * MAudioPdPlayUtilityCallback
        */
        void HandleDownloadEventL(TUint aTransactionID,
                                  TBrCtlDownloadEvent aEvent,
                                  TUint aValue);

        /**
        * From MMPXCenRepObserver
        * Handle a change in a setting value.
        *
        * @param aRepositoryUid Central repository UID containing the setting
        * @param aSettingId Id of the setting
        * @since 3.0
        */
        void HandleSettingChange( const TUid& aRepositoryUid,
                                  TUint32 aSettingId);

    private:
       /**
        * C++ default constructor
        */
        CMPXProgressDownload();

        /**
        * the second phase constructor ConstructL to safely construct things
        * that can leave
        */
        void ConstructL();

        /**
        * Sets the volume level in audio controller
        * @param aVolume Volume level, ranges from 0 - KPbPlaybackVolumeLevelMax
        */
        void SetVolume( TInt aVolume );

        /**
        * Sets the volume level in audio controller to mute.
        * @param aMute ETrue to indicated mute
        */
        void SetMute( TBool aMute );

        /**
        *  Consumes the rights for the current media according
        *  to the specified consume type
        *
        *  @param aType Type of consumption to execute
        */
        void ConsumeRightsL(ContentAccess::TIntent aIntent);

    private: // Data
        CMPXCenRepWatcher* iVolumeWatcher;
        CMPXCenRepWatcher* iMuteWatcher;
        CProgressiveDownloadUtility* iPdUtil; // owned
        MAudioPdPlayUtility* iPdPlayer; // owned
        HBufC* iPdPath;
        TUint iTransactionId;
        TInt iVolume;
        TInt iFeatureFlag;
        CMPXDrmMediaUtility* iDrmMediaUtility;
        TMPXPlaybackPdDownloadState iDownloadState;
        TInt iDownloadBytes;
        TInt iDownloadSize;
        TMPXLocalAudioPlaybackState iState;
        TBool iConsumeStarted;
    };

#endif      // CMPXPROGRESSDOWNLOAD_H


