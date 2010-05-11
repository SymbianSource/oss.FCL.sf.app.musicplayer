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
* Description:  This class manages progress download using Enhanced Media
                 Client API
*
*/


// INCLUDE FILES
#include <f32plugin.h>
#include <AudioPreference.h>
#include <badesca.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/common/mmfmeta.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmedia.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxcmn.h>
#include <mpxdrmmediautility.h>
#include <mpxprivatecrkeys.h>
#include <mpxcenrepwatcher.h>
#include <mpxlog.h>
#include <pathinfo.h>

#include <mmf/common/mmfdrmcustomcommands.h>
#include <MetaDataUtility.h>
#include <MetaDataFieldContainer.h>

#include <MMControlFactory.h>
using namespace multimedia;


#include "mpxprogressdownloadsb.h"


// CONSTANTS
const TUid KProgressDownloadUid={0x10207BCD};

// ============================ LOCAL FUNCTIONS ==============================


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXProgressDownloadSB* CMPXProgressDownloadSB::NewL(TAny* /*aInitParams*/)
    {
    CMPXProgressDownloadSB* p=new(ELeave)CMPXProgressDownloadSB();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::ConstructL()
    {
    iVolumeWatcher = CMPXCenRepWatcher::NewL(KCRUidMPXSettings,
                                             KMPXPlaybackVolume,
                                             this);
    iMuteWatcher = CMPXCenRepWatcher::NewL(KCRUidMPXSettings,
                                           KMPXPlaybackMute,
                                           this);
    iFeatureFlag = EPbFeatureBalance | EPbFeatureVolumeRamp;
    iDrmMediaUtility = CMPXDrmMediaUtility::NewL();

    //Create Factory
    TInt status = CMultimediaFactory::CreateFactory( iFactory );
    User::LeaveIfError(status);

    User::LeaveIfError(iFs.Connect());

    }

// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
//
CMPXProgressDownloadSB::CMPXProgressDownloadSB()
    : iStreamBuffering(EFalse),
      iFirstBuffer(EFalse),
      iFileSaved(EFalse),
      iDownloadBytes(0),
      iDownloadSize(0),
      iFileHandle(NULL),
      iPdPath(NULL),
      iPlaying(EFalse),
      iOngoingCmdCancelDownload(EFalse),
      iErrorOfStreamClosedEvent(KErrNone)
    {}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXProgressDownloadSB::~CMPXProgressDownloadSB()
    {
    delete iMuteWatcher;
    delete iVolumeWatcher;
    if ( iDrmMediaUtility )
        {
        if ( EPbDlStateDownloadCompleted == iDownloadState )
            {
            ConsumeRights( ContentAccess::EStop );
            }
        iDrmMediaUtility->Close();
        delete iDrmMediaUtility;
        iDrmMediaUtility = NULL;
        }

    ResetEnhancedMediaClient();
    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPActive, EFalse, KErrNone);

    delete iFactory;
    iFactory = NULL;
    delete iPdPath;
    delete iDrmCustomCommand;
    iDrmCustomCommand = NULL;

    iFs.Close();
    }


// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::ResetEnhancedMediaClient()
    {

    if (iMStreamControl)
        {
        iMStreamControl->Close();
        }

    if (iMAudioProgDLSource)
        {
        iMAudioProgDLSource->Close();
        }

    if(iFactory)
        {
        if(iMStreamControl)
            {
            iFactory->DeleteStreamControl(iMStreamControl);
            iMStreamControl = NULL;
            }

        if(iMAudioProgDLSource)
           {
           MSourceControl* objPtr = iMAudioProgDLSource;
           iFactory->DeleteSourceControl(objPtr);
           iMAudioProgDLSource = NULL;
           }


        if(iMAudioSink)
            {
            MSinkControl* objPtr = iMAudioSink;
            iFactory->DeleteSinkControl(objPtr);
            iMAudioSink = NULL;
            }

        if(iMVolumeControl)
            {
            MEffectControl* effectObj = iMVolumeControl;
            iFactory->DeleteEffectControl(effectObj);
            iMVolumeControl = NULL;
            }
        }


    }

// ----------------------------------------------------------------------------
// Initializes a song for playback
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::InitialiseL(const TDesC& aSong)
    {
    MPX_DEBUG2("CMPXProgressDownloadSB::InitialiseL(%S) entering", &aSong );

    // Re-initializes EMC to make sure multiple sequential
    // downloads work
    ResetEnhancedMediaClient();

    //Create Stream Source
    TInt err = iFactory->CreateStreamControl( KStreamControl, iMStreamControl );
    User::LeaveIfError(err);
    iMStreamControl->AddObserver( *this );

    //Create PD Source
    if ( iMStreamControl == NULL )
        {
        User::Leave(KErrNotReady);
        }

    if ( iMAudioProgDLSource != NULL )
        {
        User::Leave(KErrAlreadyExists);
        }

    MSourceControl* tempCtrl(NULL);
    err = iFactory->CreateSourceControl( KProgDLSourceControl, tempCtrl );
    User::LeaveIfError(err);

    iMAudioProgDLSource = static_cast<MProgDLSource*>(tempCtrl);

    //Open PDL Source
    err = iMAudioProgDLSource->Open(aSong, iTransactionId);
    User::LeaveIfError(err);

    err = iMAudioProgDLSource->AddObserver( *this );
     User::LeaveIfError(err);

    err = iMStreamControl->AddSource( *iMAudioProgDLSource );
    User::LeaveIfError(err);


    //Create Sink
    MSinkControl* tempSinkCtrl(NULL);
    err = iFactory->CreateSinkControl( KMMFAudioOutputSinkControl, tempSinkCtrl );
    User::LeaveIfError(err);
    iMAudioSink = tempSinkCtrl;
    iMStreamControl->AddSink( *iMAudioSink );

    //Create Volume Control
    MEffectControl* effectCtrl(NULL);
    err = iFactory->CreateEffectControl( KVolumeEffectControl, effectCtrl );
    User::LeaveIfError(err);

    iMVolumeControl = static_cast<MVolumeControl*>(effectCtrl);
    err = iMStreamControl->AddEffect( *iMVolumeControl );

    //Open Stream
    iMStreamControl->SetPriority( KAudioPriorityRealOnePlayer, KAudioPrefRealOneLocalPlayback );
    err =  iMStreamControl->Open();
    User::LeaveIfError(err);
    iStreamBuffering = ETrue;

    delete iPdPath;
    iPdPath = NULL;
    iPdPath = aSong.AllocL();
    iDownloadState = EPbDlStateBuffering;
    iState = EStateInitialising;
    iConsumeStarted = EFalse;
    iFileSaved = EFalse;
    MPX_DEBUG1("CMPXProgressDownloadSB::InitialiseL() exiting");
    }

// ----------------------------------------------------------------------------
// Initializes a song for playback
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::InitialiseL(RFile& /*aSong*/)
    {
    MPX_DEBUG1("CMPXProgressDownloadSB::InitialiseL(RFile) entering");
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected song
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::CommandL(
    TMPXPlaybackCommand aCmd,
    TInt /*aData*/)
    {
    MPX_DEBUG2("CMPXProgressDownloadSB::CommandL(%d) entering", aCmd);

    if ( iMStreamControl )
        {
        switch( aCmd )
            {
            case EPbCmdPlay:
                {
                if ( EPbDlStateDownloadCompleted == iDownloadState )
                    {
                    if ( iConsumeStarted )
                        {
                        ConsumeRights( ContentAccess::EContinue );
                        }
                    else
                        {
                        ConsumeRights( ContentAccess::EPlay );
                        iConsumeStarted = ETrue;
                        }
                    iMStreamControl->Start();
                    }
                if ( iMStreamControl->GetState() == MStreamControl::EXECUTING )
                    {
                    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPlaying,
                                             0, KErrNone);
                    }
                else
                    {
                    iMStreamControl->Start();
                    }

                break;
                }
            case EPbCmdPause:
            	{
                if ( EPbDlStateDownloadCompleted == iDownloadState && iConsumeStarted )
                    {
                    ConsumeRights( ContentAccess::EPause );
                    }
                TInt ret = iMStreamControl->Pause();
                iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPaused, 0, ret);
                break;
                }  
            case EPbCmdStop:
                iMStreamControl->Stop();
                iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPStopped,
                                         0, KErrNone);
                if ( EPbDlStateDownloadCompleted == iDownloadState  )
                    {
                    if ( iConsumeStarted )
                        {
                        ConsumeRights( ContentAccess::EStop );
                        iConsumeStarted = EFalse;
                        }
                    if ( !iFileSaved )
                        {
                        iDrmMediaUtility->Close(); // release file handle so we can move file.
                        MoveDownloadedFileToMusicFolder();
                        }
                    }
                iDrmMediaUtility->Close();
                break;
            case EPbCmdClose:
                if ( EPbDlStateDownloadCompleted == iDownloadState )
                    {
                    if ( iConsumeStarted )
                        {
                        ConsumeRights( ContentAccess::EStop );
                        iConsumeStarted = EFalse;
                        }
                    if ( !iFileSaved )
                        {
                        iDrmMediaUtility->Close(); // release file handle so we can move file.
                        MoveDownloadedFileToMusicFolder();
                        }
                    }
                iDrmMediaUtility->Close();
                iState = EStateNotInitialised;
                iDownloadState = EPbDlStateNotDownloading;
                iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed,
                                         0, KErrNone);
                break;
            }
        }

    MPX_DEBUG1("CMPXProgressDownloadSB::CommandL() exiting");
    }

// ----------------------------------------------------------------------------
// Executes a command
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::CommandL( CMPXCommand& aCmd )
    {
    MPX_FUNC("CMPXProgressDownloadSB::CommandL(CMPXCommand)");

    ASSERT( aCmd.IsSupported( KMPXCommandGeneralId ));
    TInt id( *aCmd.Value<TInt>( KMPXCommandGeneralId ));
    if ( KMPXCommandIdPlaybackPD == id )
        {
        ASSERT(aCmd.IsSupported(KMPXCommandPlaybackGeneralType));
        TMPXPlaybackPdCommand cmd(
            static_cast<TMPXPlaybackPdCommand>(
                *aCmd.Value<TInt>(KMPXCommandPlaybackGeneralType)));
        switch ( cmd )
            {
            case ( EPbCmdStartPd ):
                {
                ASSERT( aCmd.IsSupported( KMPXCommandPlaybackPDTransactionID ));
                iTransactionId = *aCmd.Value<TUint>( KMPXCommandPlaybackPDTransactionID );
                break;
                }
            case ( EPbCmdFinishPd ):
                {
                iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPlayComplete,
                                         0,KErrNone);
                break;
                }
            case ( EPbCmdGetPdStatus ):
                {
                aCmd.SetTObjectValueL<TUint>(
                        KMPXCommandPlaybackPDTransactionID,
                        iTransactionId );
                aCmd.SetTObjectValueL<TMPXPlaybackPdDownloadState>(
                        KMPXCommandPlaybackPDState,
                        static_cast<TMPXPlaybackPdDownloadState>(iDownloadState));
                aCmd.SetTObjectValueL<TInt>(
                        KMPXCommandPlaybackPDDownloadedBytes,
                        iDownloadBytes );
                aCmd.SetTObjectValueL<TInt>(
                        KMPXCommandPlaybackPDTotalBytes,
                        iDownloadSize );
                break;
                }
            case ( EPbCmdPausePd ):
                {
                iObs->HandlePluginEvent(
                        MMPXPlaybackPluginObserver::EPDownloadCmdPauseDownload,
                        iTransactionId,
                        KErrNone );
                break;
                }
            case ( EPbCmdResumePd ):
                {
                iMAudioProgDLSource->ResumeDownload();

                iObs->HandlePluginEvent(
                        MMPXPlaybackPluginObserver::EPDownloadCmdResumeDownload,
                        iTransactionId,
                        KErrNone );
                break;
                }
            case ( EPbCmdCancelPd ):
                {

                iMStreamControl->Stop();
                iMStreamControl->Close();
                iMAudioProgDLSource->CancelDownload();
                iOngoingCmdCancelDownload = ETrue;
                iObs->HandlePluginEvent(
                        MMPXPlaybackPluginObserver::EPDownloadCmdCancelDownload,
                        iTransactionId,
                        KErrNone);
                break;
                }
            default:
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// Sets a property of the plugin
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::SetL(
    TMPXPlaybackProperty aProperty,
    TInt aValue)
    {
    MPX_DEBUG3("CMPXProgressDownloadSB::SetL(%d, %d) entering", aProperty, aValue);

    TBool isSupported=ETrue;
    switch(aProperty)
        {
        case EPbPropertyVolume:
            {
            SetVolume(aValue);
            }
            break;
        case EPbPropertyVolumeRamp:
            {
            TInt curVol = 0;
            MVolumeControl::TVolumeRampMode aMode = MVolumeControl::EIMMEDIATE;
            TUint64 duration = aValue;
            TInt err = iMVolumeControl->GetVolume(curVol);
            if(iMVolumeControl)
                {
                iMVolumeControl->SetVolumeRamp(0, curVol, duration, aMode);
                iMVolumeControl->Apply();
                }
            }
            break;
        case EPbPropertyMute:
            SetMute( aValue );
            break;
        case EPbPropertyBalance:
            break;
        default:
            isSupported=EFalse;
        }

    if (!isSupported)
        {
        User::Leave(KErrNotSupported);
        }
    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPSetComplete,
                             aProperty, KErrNone);

    MPX_DEBUG1("CMPXProgressDownloadSB::SetL() exiting");
    }

// ----------------------------------------------------------------------------
// Gets a property of the plugin (async)
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::PropertyL(TMPXPlaybackProperty aProperty) const
    {
    MPX_DEBUG2("CMPXProgressDownloadSB::ValueL(%d) entering", aProperty);

    TBool isSupported=ETrue;
    TInt value=KErrNotFound;
    TInt err(KErrNone);
    switch(aProperty)
        {
        case EPbPropertyVolume:
            value = iVolumeWatcher->CurrentValueL();
            break;
        case EPbPropertyMaxVolume:
            iMVolumeControl->GetMaxVolume(value);
            break;
        case EPbPropertyMute:
            value = iMuteWatcher->CurrentValueL();
            break;
        case EPbPropertyBalance:
            //err = iPdPlayer->GetBalance(value);
            //value=Balance(value);
            break;
        case EPbPropertyDuration:
            {
            TInt64 duration;
            iMStreamControl->GetDuration(duration);
            value = duration / KPbMilliMultiplier;
            }
            break;
        case EPbPropertyPosition:
            {
            TInt64 pos;
            iMStreamControl->GetPosition(pos);
            value = pos / KPbMilliMultiplier;
            }
            break;
        case EPbPropertySupportedFeatures:
            value = iFeatureFlag;
            break;
        default:
            isSupported=EFalse;
        }
    if (!isSupported)
        {
        User::Leave(KErrNotSupported);
        }
    iObs->HandleProperty(aProperty,value,err);

    MPX_DEBUG1("CMPXProgressDownloadSB::ValueL() exiting");
    }

// ----------------------------------------------------------------------------
// Gets a list of sub players, UPnP only
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::SubPlayerNamesL()
    {
    iObs->HandleSubPlayerNames(KProgressDownloadUid, NULL, ETrue, KErrNone);
    }

// ----------------------------------------------------------------------------
// Select a sub player
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::SelectSubPlayerL(TInt /*aIndex*/)
    {
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Returns current sub player name
// ----------------------------------------------------------------------------
//
const TDesC& CMPXProgressDownloadSB::SubPlayerName()
    {
    return KNullDesC; //No subplayer name for local playback
    }

// ----------------------------------------------------------------------------
// Current sub player index
// ----------------------------------------------------------------------------
//
TInt CMPXProgressDownloadSB::SubPlayerIndex() const
   {
   return KErrNotFound;
   }

// ----------------------------------------------------------------------------
// Gets media properties
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::MediaL(const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_DEBUG1("CMPXProgressDownloadSB::MediaL() entering");

    RArray<TInt> suppIds;
    CleanupClosePushL(suppIds);
    suppIds.AppendL(KMPXMediaIdMusic);
    suppIds.AppendL(KMPXMediaIdGeneral);
    suppIds.AppendL(KMPXMediaIdAudio);
    CMPXMedia* media=CMPXMedia::NewL(suppIds.Array());
    CleanupStack::PopAndDestroy(&suppIds);
    CleanupStack::PushL(media);

    TUint attrG(0); // General attributes
    TUint attrA(0); // Audio attributes
    TUint attrM(0); // Music attributes
    TUint attrD(0); // DRM attributes

    TInt error(KErrNone);

    for (TInt i=aAttrs.Count(); --i>=0;)
        {
        TMPXAttribute attr(aAttrs[i]);
        if (attr.ContentId() == KMPXMediaIdGeneral)
            {
            attrG |= attr.AttributeId();
            }
        else if (attr.ContentId() == KMPXMediaIdMusic)
            {
            attrM |= attr.AttributeId();
            }
        else if (attr.ContentId() == KMPXMediaIdAudio)
            {
            attrA |= attr.AttributeId();
            }
        else if ( attr.ContentId() == KMPXMediaIdDrm )
            {
            attrD |= attr.AttributeId();
            }
        }


    // Get the mime type

    RApaLsSession aps;
    error  = aps.Connect(); // always fail in console test
    if ( iPdPath != NULL && KErrNone == error )
        {
        CleanupClosePushL(aps);
        TUid ignore;
        aps.AppForDocument( iPdPath->Des(), ignore, iMimeType );
        CleanupStack::PopAndDestroy(&aps);
        }

    //MPX_DEBUG2("CMPXProgressDownloadSB::MediaL() mime type = %S", iMimeType.Des8().Ptr() );

	if ( iPlaying )
	{

    CMetaDataUtility *metaDataUtility = CMetaDataUtility::NewL();
    CleanupStack::PushL( metaDataUtility );

    TInt err = KErrNone;
    if ( iMimeType.Des8().Length() )
        {
        if ( iFileHandle )
            {
            TRAP( err, metaDataUtility->OpenFileL( *iFileHandle, iMimeType.Des8() ) );
            }
        else if ( iPdPath != NULL )
            {
            TRAP( err, metaDataUtility->OpenFileL( *iPdPath, iMimeType.Des8() ) );
            }
        }
    else
        {
        if ( iFileHandle )
            {
            TRAP( err, metaDataUtility->OpenFileL( *iFileHandle ) );
            }
        else if( iPdPath != NULL )
            {
            TRAP( err, metaDataUtility->OpenFileL( *iPdPath ) );
            }
        }

    MPX_DEBUG2("CMPXProgressDownloadSB::MediaL() Metadata utility error = %d", err);


    if ( err == KErrNone )
        {
        // Get number of metadata
        TInt count = metaDataUtility->MetaDataCount();
        for( TInt i = 0; i < count; i++ )
            {
            TMetaDataFieldId fieldId;

            TPtrC field = metaDataUtility->MetaDataFieldsL().At( i, fieldId );
            if( field != KNullDesC )
                {
                switch( fieldId )
                    {
                    case EMetaDataSongTitle:
                        {
                        if ( attrG & EMPXMediaGeneralTitle )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                                field );
                            }
                        break;
                        }
                    case EMetaDataArtist:
                        {
                        if ( attrM & EMPXMediaMusicArtist )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicArtist),
                                field );
                            }
                        break;
                        }
                    case EMetaDataAlbum:
                        {
                        if ( attrM & EMPXMediaMusicAlbum )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbum),
                                field );
                            }
                        break;
                        }
                    case EMetaDataYear:
                        {
                        if ( attrM & EMPXMediaMusicYear )
                            {
                            TInt year;
                            TLex lex( field );
                            lex.Val( year );

                            TDateTime dt;
                            dt.SetYear( year );
                            TTime time( dt );

                            media->SetTObjectValueL(
                                    TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicYear),
                                    time.Int64());
                            }
                        break;
                        }
                    case EMetaDataAlbumTrack:
                        {
                        if ( attrM & EMPXMediaMusicAlbumTrack )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbumTrack),
                                field );
                            }
                        break;
                        }
                    case EMetaDataGenre:
                        {
                        if ( attrM & EMPXMediaMusicGenre )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicGenre),
                                field );
                            }
                        break;
                        }
                    case EMetaDataComposer:
                        {
                        if ( attrM & EMPXMediaMusicComposer )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicComposer),
                                field );
                            }
                        break;
                        }
                    case EMetaDataComment:
                        {
                        if ( attrG & EMPXMediaGeneralComment )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralComment),
                                field );
                            }
                        break;
                        }
                    case EMetaDataJpeg:
                        if ( ( iPdPath != NULL ) && attrM & EMPXMediaMusicAlbumArtFileName )
                            {
                            media->SetTextValueL(
                                TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbumArtFileName),
                                *iPdPath);
                            }
                         break;
                    default:
                        {
                        // nothing to do
                        break;
                        }
                    }
                }
            }
        }

    metaDataUtility->ResetL();
    CleanupStack::PopAndDestroy(metaDataUtility);

	}


   if ( ( iPdPath != NULL ) && attrG & EMPXMediaGeneralUri)
        {
        media->SetTextValueL(
            TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralUri),
            *iPdPath);
        }

   if (attrG & EMPXMediaGeneralDuration)
        {
        TInt64 duration;
        iMStreamControl->GetDuration(duration);
        media->SetTObjectValueL<TInt>(
               TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralDuration),
               duration/ KPbMilliMultiplier);
        }


    // Set bitrate
    if (attrA & EMPXMediaAudioBitrate)
        {
        TUint bitRate;
        error = iMAudioProgDLSource->GetBitRate(bitRate);

        if (!error)
            {
            media->SetTObjectValueL<TInt>(
                   TMPXAttribute(KMPXMediaIdAudio, EMPXMediaAudioBitrate),
                   bitRate);
            }
        }

    if (attrA & EMPXMediaAudioSamplerate)
        {
        // TODO: How to get sampling rate through ProgDLSource?

        }

   if ( ( iPdPath != NULL ) && attrG & EMPXMediaGeneralSize)
        {
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        TEntry entry;
        fs.Entry(iPdPath->Des(), entry);
        media->SetTObjectValueL<TInt>(
               TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralSize),
               entry.iSize);
        CleanupStack::PopAndDestroy(&fs);
        }

   if (attrG & EMPXMediaGeneralMimeType)
        {
        if ( iMimeType.Des().Length() )
            {
            media->SetTextValueL(
                TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralMimeType),
                iMimeType.Des());
            }

        }

    // Set DRM info
    // Only try to get DRM info if the song has completed downloading
    if ( EPbDlStateDownloadCompleted == iDownloadState )
        {
        const CMPXMedia* drmMedia( iDrmMediaUtility->GetMediaL( attrD ));
        if ( drmMedia )
            {
            TInt count( drmMedia->Count() );
            for ( TInt i = 0; i < count; i++ )
                {
                TUint attrId( drmMedia->Attribute(i).AttributeId() );
                if ( attrD & attrId )
                    {
                    TMPXAttribute mpxAtt( KMPXMediaIdDrm, attrId );
                    switch ( attrId )
                        {
                        case EMPXMediaDrmType:
                        case EMPXMediaDrmRightsStatus:
                        case EMPXMediaDrmRightsType:
                        case EMPXMediaDrmCount:
                            {
                            TInt val(
                                *drmMedia->Value<TInt>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmProtected:
                        case EMPXMediaDrmSendingAllowed:
                        case EMPXMediaDrmCanSetAutomated:
                        case EMPXMediaDrmHasInfoUrl:
                        case EMPXMediaDrmHasPreviewUrl:
                        case EMPXMediaDrmAboutToExpire:
                            {
                            TBool val(
                                *drmMedia->Value<TBool>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmStartTime:
                        case EMPXMediaDrmEndTime:
                        case EMPXMediaDrmIntervalStartTime:
                        case EMPXMediaDrmAccumulatedTime:
                            {
                            TInt64 val(
                                *drmMedia->Value<TInt64>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmInterval:
                            {
                            TTimeIntervalSeconds val(
                                *drmMedia->Value<TTimeIntervalSeconds>(mpxAtt));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        default:
                            {
                            break;
                            }
                        }   // end switch (attriId)
                    }   // end if ( attrD & attrId )
                }
            }
        }

    iObs->HandleMedia(*media, KErrNone);
    CleanupStack::PopAndDestroy(media);

    MPX_DEBUG1("CMPXProgressDownloadSB::MediaL() exiting");
    }

// ----------------------------------------------------------------------------
// Cancel request
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::CancelRequest()
    {
    }


// -----------------------------------------------------------------------------
// CMPXProgressDownloadSB::Event
// -----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::Event(
    MControl* aControl,
    TUint aEventType,
    TAny* aEventObject )
    {

    MPX_DEBUG1("CMPXProgressDownloadSB::Event() entering");

    if (( aControl == NULL) || (aEventObject == NULL ))
        {
        MPX_DEBUG3("CMPXProgressDownloadSB::Event() Error: aControl=%x aEventObject=%x", aControl, aEventObject);
        }


    switch ( aEventType )
        {
        case MStreamControlObserver::KStateChangedEvent:
            {
            MStreamControl* control1 = (MStreamControl*)(aControl);
            if(aEventObject && control1)
                {
                MPX_DEBUG2("CMPXProgressDownloadSB::Event:EStateChanged[%d]",control1->GetState());
                MStateChangedEvent* event = (MStateChangedEvent*)aEventObject;
                switch(event->GetState())
                    {
                    case MStreamControl::INITIALIZED:
                        MPX_DEBUG1("CMPXProgressDownloadSB::Event:EStateChanged[INITIALIZED]");
                        if( event->GetErrorCode() == KErrNone && iState == EStateInitialising )
                            {
                            delete iDrmCustomCommand;
                            iDrmCustomCommand = NULL;
                            iDrmCustomCommand = (RMMFDRMCustomCommands*)iMStreamControl->CustomInterface(KUidInterfaceMMFDRMControl);

                            if ( iDrmCustomCommand )
                                {
                                TInt drmCCErr = iDrmCustomCommand->DisableAutomaticIntent(ETrue);
                                // TODO:
                                // for wmdrm pdl, we need to let helix consume rights.
                                // by calling ExecuteIntent() when playback completes.
                                }
                            iState = EStateInitialised;
                            // Restore volume level
                            TInt currentVol( 0 );
                            MPX_TRAPD( volError, currentVol = iVolumeWatcher->CurrentValueL() );
                            if ( volError == KErrNone )
                                {
                                SetVolume( currentVol );
                                TBool mute( EFalse);
                                MPX_TRAPD( muteError, mute = iMuteWatcher->CurrentValueL() );
                                if ( muteError == KErrNone && mute )
                                    {
                                    SetMute(mute);
                                    }
                                }
                            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPActive, ETrue, event->GetErrorCode());
                            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPaused, 0, event->GetErrorCode());
                            }
                        else if ( event->GetErrorCode() == KErrEof ) // Playback Complete
                            {
                            MPX_DEBUG2("CMPXProgressDownloadSB::Event:EStateChanged[PlaybackComplete] errorcode=%d",event->GetErrorCode());
                            if ( iDownloadState == EPbDlStateDownloadCompleted && !iFileSaved )
                                {
                                if ( event->GetErrorCode() == KErrEof )
                                    {
                                    ConsumeRights( ContentAccess::EStop );
                                    }
                                else
                                    {
                                    ConsumeRights( ContentAccess::EPause );
                                    }
                                MoveDownloadedFileToMusicFolder();
                                }
                            }
                        else if ( event->GetErrorCode() == KErrDied || event->GetErrorCode() == KErrInUse ||
                                event->GetErrorCode() == KErrAccessDenied )
                            {
                            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                    0, event->GetErrorCode() );
                            }
                        else
                            {
                            //Todo: Error cases such as no rights to play.
                            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPlayComplete, 0, event->GetErrorCode());
                            }
                        break;
                    case MStreamControl::CLOSED:
                        MPX_DEBUG1("CMPXProgressDownloadSB::Event:EStateChanged[Closed]");
                        //         if ( iDownloadState == EPbDlStateDownloadCompleted )
                        //             {
                        //             MoveDownloadedFileToMusicFolderL();	//The file should be moved somewhere else.
                        //             }
                        iErrorOfStreamClosedEvent = event->GetErrorCode();
                        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed, 0, event->GetErrorCode() );
                        break;
                    case MStreamControl::PRIMED:
                        MPX_DEBUG1("CMPXProgressDownloadSB::Event:EStateChanged[Primed]");
                        TInt64 duration;
                        if (iMStreamControl->GetDuration(duration) != KErrUnknown )
                            {
                            MPX_DEBUG2("CMPXProgressDownloadSB::Event:KDurationChangedEvent Duration = %d", I64INT(duration));
                            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDurationChanged, duration / KPbMilliMultiplier, KErrNone);
                            }
                        break;
                    case MStreamControl::EXECUTING://Playing
                        {
                        MPX_DEBUG1("CMPXProgressDownloadSB::Event:EStateChanged[Playing]");
                        iStreamBuffering = EFalse;
                        iPlaying = ETrue;
                        // Send the Started-message here since View may not have been initialized earlier.
                        TUint expectedFileSize = 0;
                        iMAudioProgDLSource->GetExpectedFileSize(expectedFileSize);
                        iDownloadSize = expectedFileSize;
                        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadStarted,
                                iDownloadSize,
                                KErrNone );

                        if ( iDownloadState == EPbDlStateBuffering )
                            {
                            iDownloadState = EPbDlStateDownloading;
                            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPDownloadStateChanged, iDownloadState, KErrNone);
                            }
                        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPlaying, 0, event->GetErrorCode());
                        TInt64 duration;
                        if (iMStreamControl->GetDuration(duration) != KErrUnknown )
                            {
                            MPX_DEBUG2("CMPXProgressDownloadSB::Event:EStateChanged Duration = %d", I64INT(duration));
                            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDurationChanged, duration / KPbMilliMultiplier, KErrNone);
                            }
                        }
                        break;
                    case MStreamControl::BUFFERING:
                        MPX_DEBUG2("CMPXProgressDownloadSB::Event:EStateChanged[Buffering] errorcode= %d",event->GetErrorCode());
                        iStreamBuffering = ETrue;
                        if ( iDownloadState != EPbDlStateDownloadPaused ||
                                iDownloadState != EPbDlStateDownloadCanceled ||
                                iDownloadState != EPbDlStateDownloadError ||
                                iDownloadState != EPbDlStateNotDownloading
                        )
                            {
                            iDownloadState = EPbDlStateBuffering;
                            }
                        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPaused, 0, event->GetErrorCode());
                        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPDownloadStateChanged, iDownloadState, KErrNone);
                        break;
                    case MStreamControl::PAUSED:
                        MPX_DEBUG1("CMPXProgressDownloadSB::Event:EStateChanged[Paused]");
                        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPaused, 0, event->GetErrorCode());
                        break;
                    default:
                        break;
                    }
                }
            else
                {
                MPX_DEBUG1("CMPXProgressDownloadSB::Event:EStateChanged aEventObject is KNullDesC");
                MPX_DEBUG1(" Code should not reach here as aEventObject is NULL");
                }      
            }
            break;

        case MStreamControlObserver::KDurationChangedEvent:
            MPX_DEBUG1("CMPXProgressDownloadSB::Event:KDurationChangedEvent");
            TInt64 duration;
            if (iMStreamControl->GetDuration(duration) != KErrUnknown )
                {
                MPX_DEBUG2("CMPXProgressDownloadSB::Event:KDurationChangedEvent Duration = %d", I64INT(duration));
                iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDurationChanged, duration / KPbMilliMultiplier, KErrNone);
                }
            break;

        case MSourceControlObserver::KDownloadStatusChangedEvent:
            {
            MProgDLSource* control1 = (MProgDLSource*)(aControl);
            if( control1) 
             {
            MPX_DEBUG2("CMPXProgressDownloadSB::Event:DownloadStatus[%d]",control1->GetDownloadStatus());
            switch ( control1->GetDownloadStatus() )
                {

                case MProgDLSource::EConnecting:
                    MPX_DEBUG1("CMPXProgressDownloadSB::Event:DownloadStatus [connecting]");
                    break;

                case MProgDLSource::EStarted:
                    {
                    MPX_DEBUG1("CMPXProgressDownloadSB::Event:DownloadStatus [started]");
                    iDownloadState = EPbDlStateDownloading;
                    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                                             iDownloadState,
                                             KErrNone);

                    TUint expectedFileSize = 0;
                    iMAudioProgDLSource->GetExpectedFileSize(expectedFileSize);
                    iDownloadSize = expectedFileSize;
                    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadStarted,
                                             iDownloadSize,
                                             KErrNone );
                    }
                    break;

                case MProgDLSource::EPaused:
                    {
                    MPX_DEBUG1("CMPXProgressDownloadSB::Event:DownloadStatus [paused]");
                    iDownloadState = EPbDlStateDownloadPaused;
                    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                                             iDownloadState,
                                             KErrNone);

                    }
                    break;

                case MProgDLSource::EDeleted: 	//download has been canceled from Browser
                    {							//or Browser has Exited
                    MPX_DEBUG1("CMPXProgressDownloadSB::Event:DownloadStatus [deleted]");

                    iMStreamControl->Stop();
                    iMStreamControl->Close();
                    iDrmMediaUtility->Close();
                    //Stop the play
                    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPStopped,
                                             0, KErrNone);
                    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPlayComplete, 0, KErrNone);
                    if (!iOngoingCmdCancelDownload) // when Dl was canceled by browser
                        {
                        iObs->HandlePluginEvent(
                                MMPXPlaybackPluginObserver::EPDownloadCmdCancelDownload,
                                iTransactionId,
                                KErrNone);
                        }
                    break;                    
                    }
                case MProgDLSource::ECompleted:
                    {
                    MPX_DEBUG1("CMPXProgressDownloadSB::Event:DownloadStatus [completed]");
                    iDownloadState = EPbDlStateDownloadCompleted;

                    // and notify playback engine of the change.
                    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadPositionChanged,
                                             iDownloadSize,
                                             KErrNone);

                    if ((iState == EStateNotInitialised || 
                        (iState == EStateInitialising && iErrorOfStreamClosedEvent == KErrCANoRights)) 
                        && !iFileSaved )
                         {
                         MoveDownloadedFileToMusicFolder();
                         }
                     iErrorOfStreamClosedEvent = KErrNone ;


                    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                                             iDownloadState,
                                             KErrNone);
                    }
                    break;
                case MProgDLSource::EFailed:
                case MProgDLSource::EUnknown:
                    MPX_DEBUG1("CMPXProgressDownloadSB::Event:DownloadStatus [failed]");
                    iDownloadState = EPbDlStateDownloadError;
                    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                                             iDownloadState,
                                             KErrNone);
                    break;
                default:
                  break;
                }
             }
             else
                {
                MPX_DEBUG1("CMPXProgressDownloadSB::Event:DownloadStatus control1 is KNullDesC");
				MPX_DEBUG1("Code should not reach here, as control1 is NULL"); 
                }
            }
            break;

        case MSourceControlObserver::KFileMoveCompleteEvent:
            {
            MPX_DEBUG1("CMPXProgressDownloadSB::Event:KFileMoveCompleteEvent");
            if( aEventObject)
             {            
            MErrorCode* errorObj = (MErrorCode*)aEventObject;
            TInt fileMoveError = errorObj->GetErrorCode();

            if ( !fileMoveError )
                {
                delete iPdPath;
                iPdPath = NULL;
                MPX_TRAPD( err, iPdPath = iMovedFileName.AllocL() );
                if( err != KErrNone )
                	{
                    MPX_DEBUG2("iPdPath = iMovedFileName.AllocL() with leave err=%d", err ); 
                    break;
                	}
                iFileSaved = ETrue;
                }
            else
                {
                // TODO: If file is renamed, we need to get the new path.
                if( fileMoveError == KErrAlreadyExists )
                    {
                    TPtr ptr( NULL, 0 );
                    iMAudioProgDLSource->FileName( ptr );
                    delete iPdPath;
                    iPdPath = NULL;
                    MPX_TRAPD(err, iPdPath = ptr.AllocL() );
                    if( err != KErrNone )
                    	{
                        MPX_DEBUG2("iPdPath = ptr.AllocL() with leave err=%d", err ); 
                        break;
                    	}
                    iFileSaved = ETrue;
                    }
                }

            TRAP_IGNORE( iDrmMediaUtility->InitL( *iPdPath ));
            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPlayComplete, 0, KErrNone);

            if ( !fileMoveError || fileMoveError == KErrAlreadyExists )
                {
                iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadFileMoved, (TInt)iPdPath, KErrNone);
                }
              }
            else
             {
             	 MPX_DEBUG1("CMPXProgressDownloadSB::Event:KFileMoveCompleteEvent aEventObject is KNullDesC");
               MPX_DEBUG1(" Code should not reach here as aEventObject is NULL");
             }    
            break;
            }
        case MSourceControlObserver::KPercentageDownloadedChangedEvent:
            {
            MPX_DEBUG1("CMPXProgressDownloadSB::Event:KPercentageDownloadedChangedEvent");

            if (iDownloadState == EPbDlStateDownloadPaused)
                {
                iDownloadState = EPbDlStateBuffering;
                }

            // update current file size
            TUint currentFileSize = 0;
            TUint expectedFileSize = 0;

            iMAudioProgDLSource->GetCurrentFileSize(currentFileSize);
            iMAudioProgDLSource->GetExpectedFileSize(expectedFileSize);
            iDownloadBytes = currentFileSize;
            if (expectedFileSize != iDownloadSize)
            	{
                iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadingUpdated,
                                         expectedFileSize,
                                         KErrNone);            	
            	}
            iDownloadSize = expectedFileSize;

            // and notify playback engine of the change.
            iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPDownloadPositionChanged,
                                     iDownloadBytes,
                                     KErrNone);

            }
            break;

        default:
            break;
        }//end switch (aEventType)
    MPX_DEBUG1("CMPXProgressDownloadSB::Event() exiting");
    }

// ----------------------------------------------------------------------------
// Handle a change in a setting value.
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::HandleSettingChange(
    const TUid& aRepositoryUid,
    TUint32 aSettingId )
    {
    MPX_DEBUG1("CMPXProgressDownloadSB::HandleSettingChange() entering");

    if ( KCRUidMPXSettings == aRepositoryUid &&
         KMPXPlaybackVolume == aSettingId )
        {
        MPX_DEBUG1("CMPXProgressDownloadSB::HandleSettingChange() Volume setting changed");
        TInt vol( 0 );
        MPX_TRAPD( error, vol = iVolumeWatcher->CurrentValueL() );
        if ( error == KErrNone )
            {
            SetVolume( vol );
            }
        }
    else if ( KCRUidMPXSettings == aRepositoryUid &&
         KMPXPlaybackMute == aSettingId )
        {
        MPX_DEBUG1("CMPXProgressDownloadSB::HandleSettingChange() Mute setting changed");
        TBool mute( EFalse );
        MPX_TRAPD( error, mute = static_cast<TBool>(iMuteWatcher->CurrentValueL()) );
        if ( error == KErrNone )
            {
            TInt oldVolume( 0 );
            iMVolumeControl->GetVolume( oldVolume );
            if ( (mute && oldVolume != 0) || (!mute && oldVolume == 0) )
                {
                SetMute( mute );
                }
            }
        }
    MPX_DEBUG1("CMPXProgressDownloadSB::HandleSettingChange() exiting");
    }

// ----------------------------------------------------------------------------
// Sets the volume level in audio controller
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::SetVolume( TInt aVolume )
    {
    MPX_DEBUG2("CMPXProgressDownloadSB::SetVolume(%d) entering", aVolume);

    // Ensure that level is within min and max values
    if ( aVolume > KPbPlaybackVolumeLevelMax )
        {
        aVolume = KPbPlaybackVolumeLevelMax;
        }
    if ( aVolume < KPbPlaybackVolumeLevelMin )
        {
        aVolume = KPbPlaybackVolumeLevelMin;
        }

    TBool changed( EFalse );
    // Change MMF Audio player's volume
    if ( EStateInitialised == iState )
        {
        TInt maxVolume(0);
        iMVolumeControl->GetMaxVolume(maxVolume);
        TInt newVolume( aVolume * maxVolume / 100 );
        MPX_DEBUG2("CMPXProgressDownloadSB::SetVolume(): Setting volume = %d", newVolume);

        // First check if MMF Audio player's volume is changed by new value
        TInt oldVolume( 0 );
        iMVolumeControl->GetVolume( oldVolume );
        if ( newVolume != oldVolume )
            {
            iMVolumeControl->SetVolume( newVolume );
            iMVolumeControl->Apply();
            changed = ETrue;
            }
        }
    
    // Change setting in cenrep
    TInt currentVol( 0 );
    MPX_TRAPD( volError, currentVol = iVolumeWatcher->CurrentValueL() );
    if ( volError == KErrNone && aVolume != currentVol )
        {
        MPX_TRAP( volError, iVolumeWatcher->SetValueL( aVolume ) );
        if( aVolume == 0 )
            {
            MPX_TRAP( volError, iMuteWatcher->SetValueL( ETrue ) );
            }
        else if( aVolume > 0 )
            {
            TBool currentMute( EFalse );
            
            MPX_TRAP( volError, currentMute = iMuteWatcher->CurrentValueL() );
            if( volError == KErrNone && currentMute )
                {
                MPX_TRAP( volError, iMuteWatcher->SetValueL( EFalse ) );
                }
            }
        }


    // Notify observer if value changed
    if ( changed )
        {
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPVolumeChanged,
                                 aVolume,
                                 KErrNone);
        }

    MPX_DEBUG1("CMPXProgressDownloadSB::SetVolume() exiting");
    }

// ----------------------------------------------------------------------------
// Sets the volume level in audio controller
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::SetMute( TBool aMute )
    {
    MPX_DEBUG3("-->CMPXProgressDownloadSB::SetMute 0x%08x vol (%d)", this, aMute);

    TBool changed( EFalse );
    // Change MMF Audio player's volume
    TInt currentVolume(0);
    iMVolumeControl->GetVolume(currentVolume);
    if ( aMute && currentVolume != 0 )
        {
        iVolume = currentVolume;
        TInt vol = 0;
        iMVolumeControl->SetVolume(vol);
        iMVolumeControl->Apply();
        changed = ETrue;
        }
    else if ( !aMute && currentVolume == 0 )      // UnMute
        {
        iMVolumeControl->SetVolume(iVolume);
        iMVolumeControl->Apply();
        changed = ETrue;
        }

    // Change setting in cenrep
    TBool currentMute( EFalse );
    MPX_TRAPD( muteError, currentMute = iMuteWatcher->CurrentValueL() );
    if ( muteError == KErrNone )
        {
        if ( aMute && !currentMute )
            {
            MPX_TRAP( muteError, iMuteWatcher->SetValueL( aMute ) );
            }
        else if ( !aMute && currentMute )
            {
            MPX_TRAP( muteError, iMuteWatcher->SetValueL( aMute ) );
            }
        }

    // Notify observer if value changed
    if ( changed )
        {
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPMuteChanged,
                                 aMute,
                                 KErrNone);
        }

    MPX_DEBUG3("<--CMPXProgressDownloadSB::SetMute 0x%08x vol (%d)", this, aMute);
    }

// -----------------------------------------------------------------------------
// CMPXProgressDownloadSB::MoveDownloadedFileToMusicFolderL
// -----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::MoveDownloadedFileToMusicFolder()
    {
    if ( iFileSaved || iPdPath == NULL ||
         ( (*iPdPath).Length() == 0 ) ||
         iMAudioProgDLSource->GetDownloadStatus() == MProgDLSource::EDeleted )
        {
        return;
        }

    MPX_DEBUG1("CMPXProgressDownloadSB::MoveDownloadedFileToMusicFolder() entering");
    TParse parse;
    parse.Set(*iPdPath,NULL,NULL);
    TPtrC drive = parse.Drive();

    iMovedFileName.Copy(drive);
    iMovedFileName.Append(_L("\\Data\\") );
    iMovedFileName.Append(_L("Download\\") );
    iFs.MkDirAll(iMovedFileName);
    iMovedFileName.Append(parse.NameAndExt() );
    TInt error = iMAudioProgDLSource->MoveFile(iMovedFileName);

    if ( !error )
        {
        iFileSaved = ETrue;
        }

    MPX_DEBUG1("CMPXProgressDownloadSB::MoveDownloadedFileToMusicFolder() exiting");
    }

// ----------------------------------------------------------------------------
// Consumes the rights for the current media
// ----------------------------------------------------------------------------
//
void CMPXProgressDownloadSB::ConsumeRights(
    ContentAccess::TIntent aIntent )
    {
    MPX_DEBUG2("-->CMPXProgressDownloadSB::ConsumeRights(%d)", aIntent);
    if ( iDrmCustomCommand )
        {
        switch ( aIntent )
            {
            case ContentAccess::EPlay:
            case ContentAccess::EStop:
            case ContentAccess::EPause:
            case ContentAccess::EContinue:
                {
                break;
                }
            default:
                {
                aIntent = ContentAccess::EUnknown;
                iConsumeStarted = EFalse;
                break;
                }
            }
        MPX_DEBUG2("-->CMPXProgressDownloadSB::ConsumeRights(): Executing intent %d", aIntent);
        iDrmCustomCommand->ExecuteIntent(aIntent);
        }
    MPX_DEBUG2("<--CMPXProgressDownloadSB::ConsumeRights(%d)", aIntent);
    }

// End of file
