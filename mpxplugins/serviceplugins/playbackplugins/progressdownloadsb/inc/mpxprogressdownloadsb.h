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
* Description:  This class manages progress download via Enhanced Media Client
*
*/



#ifndef CMPXProgressDownloadSBSB_H
#define CMPXProgressDownloadSBSB_H

// INCLUDES
#include <caf/content.h>
#include <mpxplaybackplugin.h>
#include <mpxcenrepobserver.h>
#include <mpxplaybackcommanddefs.h>

#include <MMControlFactory.h>
#include <StreamControl.h>
#include <SinkControl.h>
#include <ControlObserver.h>
#include <Events.h>
#include <ProgDLSource.h>
#include <SourceControlObserver.h>
#include <StreamControlObserver.h>
#include <VolumeControl.h>
#include <apgcli.h>

using multimedia::MStreamControl;
using multimedia::MControlObserver;
using multimedia::MProgDLSource;
using multimedia::MControl;
using multimedia::MSinkControl;
using multimedia::CMultimediaFactory;
using multimedia::MVolumeControl;



// FORWARD DECLARATIONS
class CMPXCenRepWatcher;
class CMPXDrmMediaUtility;
class RMMFDRMCustomCommands;
// CONSTANTS

// CLASS DECLARATION

/**
*  CMPXProgressDownloadSB class
*
*  Plug-in basically handles download via S60's enhanced media client.
*  @lib mpxprogressdownloadsb.lib
*/
NONSHARABLE_CLASS(CMPXProgressDownloadSB) : public CMPXPlaybackPlugin,
                                            public MMPXCenRepObserver,
                                            public MControlObserver
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
        static CMPXProgressDownloadSB* NewL(TAny* aInitParams);

        /**
        * Destructor
        * Destroy the object and release all memory objects
        */
        ~CMPXProgressDownloadSB();

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
        *  From MControlObserver
        */
        void Event( MControl* aControl, TUint aEventType, TAny* aEventObject );


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
        CMPXProgressDownloadSB();

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

        void ResetEnhancedMediaClient();

        void MoveDownloadedFileToMusicFolderL();

        /**
        *  Consumes the rights for the current media according
        *  to the specified consume type
        *
        *  @param aType Type of consumption to execute
        */
        void ConsumeRights(ContentAccess::TIntent aIntent);
    private: // Data

        RFs iFs;

        CMPXCenRepWatcher* iVolumeWatcher;
        CMPXCenRepWatcher* iMuteWatcher;

        TInt32 iDownloadID;
        MStreamControl* iMStreamControl; // owned
        MSinkControl* iMAudioSink; // owned
        MProgDLSource* iMAudioProgDLSource; // owned
        MVolumeControl*  iMVolumeControl; // owned
        CMultimediaFactory* iFactory; // owned
        TBool iStreamBuffering;
        TBool iFirstBuffer;
        TBool iFileSaved;
        RMMFDRMCustomCommands* iDrmCustomCommand;

        RFile* iFileHandle; // not owned
        HBufC16* iPdPath;
        TUint iTransactionId;
        TInt iVolume;
        TInt iFeatureFlag;
        CMPXDrmMediaUtility* iDrmMediaUtility;
        TMPXPlaybackPdDownloadState iDownloadState;
        TInt iDownloadBytes;
        TInt iDownloadSize;
        TMPXLocalAudioPlaybackState iState;
        TFileName iMovedFileName;
        TDataType iMimeType;
        TBool iPlaying;
        TBool iConsumeStarted;
        TBool iOngoingCmdCancelDownload;
        TInt iErrorOfStreamClosedEvent;

    };

#endif      // CMPXProgressDownloadSBSB_H


