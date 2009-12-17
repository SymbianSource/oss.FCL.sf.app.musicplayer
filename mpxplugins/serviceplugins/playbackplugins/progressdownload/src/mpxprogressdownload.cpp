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


// INCLUDE FILES
#include <f32plugin.h>
#include <AudioPreference.h>
#include <badesca.h>
#include <apgcli.h>
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

#include "mpxprogressdownload.h"


// CONSTANTS
const TUid KProgressDownloadUid={0x101FFC09};
const TInt KProgressPlayEndThreshold=8000000; // 8 seconds, in milliseconds

_LIT(KMMFMetaEntryAuthor, "author"); // For WMA progressive download

// ============================ LOCAL FUNCTIONS ==============================
LOCAL_C TInt Balance(TInt aMMFBalance)
    {
    return (aMMFBalance-KMMFBalanceCenter) *
           (EPbBalanceMaxRight-EPbBalanceMaxLeft) /
           (KMMFBalanceMaxRight-KMMFBalanceMaxLeft);
    }

LOCAL_C TInt MMFBalance(TInt aBalance)
    {
    return KMMFBalanceCenter+(KMMFBalanceMaxRight-KMMFBalanceMaxLeft)/
           (EPbBalanceMaxRight-EPbBalanceMaxLeft)*aBalance;
    }


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXProgressDownload* CMPXProgressDownload::NewL(TAny* /*aInitParams*/)
    {
    CMPXProgressDownload* p=new(ELeave)CMPXProgressDownload();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::ConstructL()
    {
    iVolumeWatcher = CMPXCenRepWatcher::NewL(KCRUidMPXSettings,
                                             KMPXPlaybackVolume,
                                             this);
    iMuteWatcher = CMPXCenRepWatcher::NewL(KCRUidMPXSettings,
                                           KMPXPlaybackMute,
                                           this);
    iFeatureFlag = EPbFeatureBalance | EPbFeatureVolumeRamp;
    iDrmMediaUtility = CMPXDrmMediaUtility::NewL();
    }

// ----------------------------------------------------------------------------
// C++ constructor
// ----------------------------------------------------------------------------
//
CMPXProgressDownload::CMPXProgressDownload()
    {}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXProgressDownload::~CMPXProgressDownload()
    {
    delete iMuteWatcher;
    delete iVolumeWatcher;
    if ( iDrmMediaUtility )
        {
        if ( EPbDlStateDownloadCompleted == iDownloadState )
            {
            TRAP_IGNORE( ConsumeRightsL( ContentAccess::EStop ) );
            }
        delete iDrmMediaUtility;
        }
    if (iPdPlayer)
        {
        iPdPlayer->Close();
        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPActive,EFalse,KErrNone);
        delete iPdPlayer;
        }
    delete iPdUtil;
    delete iPdPath;
    }

// ----------------------------------------------------------------------------
// Initializes a song for playback
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::InitialiseL(const TDesC& aSong)
    {
    MPX_DEBUG2("CMPXProgressDownload::InitialiseL(%S) entering", &aSong );

    // Re-initialize ProgressiveDownloadUtility to make sure multiple sequential
    // downloads work
    delete iPdPlayer;
    iPdPlayer = NULL;
    delete iPdUtil;
    iPdUtil = NULL;
    iPdUtil = CProgressiveDownloadUtility::NewL();

    iPdPlayer = iPdUtil->OpenL( aSong, *this );
    delete iPdPath;
    iPdPath = NULL;
    iPdPath = aSong.AllocL();
    iDownloadState = EPbDlStateBuffering;
    iState = EStateInitialising;
    iConsumeStarted = EFalse;

    MPX_DEBUG1("CMPXProgressDownload::InitialiseL() exiting");
    }

// ----------------------------------------------------------------------------
// Initializes a song for playback
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::InitialiseL(RFile& aSong)
    {
    MPX_DEBUG1("CMPXProgressDownload::InitialiseL(RFile) entering");

    // Re-initialize ProgressiveDownloadUtility to make sure multiple sequential
    // downloads work
    delete iPdPlayer;
    iPdPlayer = NULL;
    delete iPdUtil;
    iPdUtil = NULL;
    iPdUtil = CProgressiveDownloadUtility::NewL();

    iPdPlayer = iPdUtil->OpenL( aSong, *this );
    delete iPdPath;
    iPdPath = NULL;
    TFileName filename;
    aSong.FullName(filename);
    iPdPath = filename.AllocL();
    iDownloadState = EPbDlStateBuffering;
    iState = EStateInitialising;
    iConsumeStarted = EFalse;

    MPX_DEBUG1("CMPXProgressDownload::InitialiseL() exiting");
    }

// ----------------------------------------------------------------------------
// Executes a command on the selected song
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::CommandL(TMPXPlaybackCommand aCmd, TInt /*aData*/)
    {
    MPX_DEBUG2("CMPXProgressDownload::CommandL(%d) entering", aCmd);

    if (iPdPlayer)
        {
        switch(aCmd)
            {
            case EPbCmdPlay:
                {
                if ( EPbDlStateDownloadCompleted == iDownloadState )
                    {
                    if (iConsumeStarted)
                        {
                        ConsumeRightsL( ContentAccess::EContinue );
                        }
                    else
                        {
                        MPX_TRAPD( AEErr,  ConsumeRightsL( ContentAccess::EPlay ) );
                        if (AEErr == KErrDiskFull)
                        	{
                			iDrmMediaUtility->Close();
                			iPdPlayer->Close();
                			iState = EStateNotInitialised;
                			iDownloadState = EPbDlStateNotDownloading;
                			iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed,
                                         0, KErrDiskFull);
				            break;
                        	}
                        iConsumeStarted = ETrue;
                        }
                    }
                iPdPlayer->Play();
                iState = EStateInitialised;
                iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPlaying,
                                         0, KErrNone);
                break;
                }
            case EPbCmdPause:
                if ( EPbDlStateDownloadCompleted == iDownloadState && iConsumeStarted )
                    {
                    ConsumeRightsL( ContentAccess::EPause );
                    }
                iPdPlayer->Pause();
                iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPaused,
                                         0, KErrNone);
                break;
            case EPbCmdStop:
                if ( EPbDlStateDownloadCompleted == iDownloadState && iConsumeStarted )
                    {
                    ConsumeRightsL( ContentAccess::EStop );
                    iConsumeStarted = EFalse;
                    }
                iDrmMediaUtility->Close();
                iPdPlayer->Stop();
                iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPStopped,
                                         0, KErrNone);
                break;
            case EPbCmdClose:
                if ( EPbDlStateDownloadCompleted == iDownloadState && iConsumeStarted )
                    {
                    ConsumeRightsL( ContentAccess::EStop );
                    iConsumeStarted = EFalse;
                    }
                iDrmMediaUtility->Close();
                iPdPlayer->Close();
                iState = EStateNotInitialised;
                iDownloadState = EPbDlStateNotDownloading;
                iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed,
                                         0, KErrNone);
                break;
            }
        }

    MPX_DEBUG1("CMPXProgressDownload::CommandL() exiting");
    }

// ----------------------------------------------------------------------------
// Executes a command
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::CommandL( CMPXCommand& aCmd )
    {
    MPX_FUNC("CMPXProgressDownload::CommandL(CMPXCommand)");

    ASSERT( aCmd.IsSupported( KMPXCommandGeneralId ));
    TInt id( aCmd.ValueTObjectL<TInt>( KMPXCommandGeneralId ));
    if ( KMPXCommandIdPlaybackPD == id )
        {
        ASSERT(aCmd.IsSupported(KMPXCommandPlaybackGeneralType));
        TMPXPlaybackPdCommand cmd(
            static_cast<TMPXPlaybackPdCommand>(
                aCmd.ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType)));
        switch ( cmd )
            {
            case ( EPbCmdStartPd ):
                {
                ASSERT( aCmd.IsSupported( KMPXCommandPlaybackPDTransactionID ));
                iTransactionId = aCmd.ValueTObjectL<TUint>( KMPXCommandPlaybackPDTransactionID );
                break;
                }
            case ( EPbCmdFinishPd ):
                {
                iObs->HandlePluginEvent(
                        MMPXPlaybackPluginObserver::EPPlayComplete,
                        0,
                        KErrNone);
                break;
                }
            case ( EPbCmdHandlePdEvent ):
                {
                ASSERT( aCmd.IsSupported( KMPXCommandPlaybackPDTransactionID ) &&
                        aCmd.IsSupported( KMPXCommandPlaybackPDEvent ) &&
                        aCmd.IsSupported( KMPXCommandPlaybackGeneralData ));
                TUint transId( aCmd.ValueTObjectL<TUint>( KMPXCommandPlaybackPDTransactionID ));
                TBrCtlDownloadEvent event(
                    aCmd.ValueTObjectL<TBrCtlDownloadEvent>( KMPXCommandPlaybackPDEvent ));
                TUint val( aCmd.ValueTObjectL<TUint>( KMPXCommandPlaybackGeneralData ));
                HandleDownloadEventL( transId, event, val );
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
                iObs->HandlePluginEvent(
                        MMPXPlaybackPluginObserver::EPDownloadCmdResumeDownload,
                        iTransactionId,
                        KErrNone );
                break;
                }
            case ( EPbCmdCancelPd ):
                {
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
void CMPXProgressDownload::SetL(TMPXPlaybackProperty aProperty, TInt aValue)
    {
    MPX_DEBUG3("CMPXProgressDownload::SetL(%d, %d) entering", aProperty, aValue);

    TBool isSupported=ETrue;
    switch(aProperty)
        {
        case EPbPropertyVolume:
            {
            SetVolume( aValue );
            break;
            }
        case EPbPropertyVolumeRamp:
            iPdPlayer->SetVolumeRamp(TTimeIntervalMicroSeconds(TInt64(aValue)));
            break;
        case EPbPropertyMute:
            SetMute( aValue );
            break;
        case EPbPropertyBalance:
            iPdPlayer->SetBalance(MMFBalance(aValue));
            break;
        case EPbPropertyPosition:
            {
            TInt64 pos(aValue);
            pos *= KPbMilliMultiplier;
            iPdPlayer->SetPosition(pos);
            }
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

    MPX_DEBUG1("CMPXProgressDownload::SetL() exiting");
    }

// ----------------------------------------------------------------------------
// Gets a property of the plugin (async)
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::PropertyL(TMPXPlaybackProperty aProperty) const
    {
    MPX_DEBUG2("CMPXProgressDownload::ValueL(%d) entering", aProperty);

    TBool isSupported=ETrue;
    TInt value=KErrNotFound;
    TInt err(KErrNone);
    switch(aProperty)
        {
        case EPbPropertyVolume:
            {
            value = iVolumeWatcher->CurrentValueL();
            break;
            }
        case EPbPropertyMaxVolume:
            value=iPdPlayer->MaxVolume();
            break;
        case EPbPropertyMute:
            value = iMuteWatcher->CurrentValueL();
            break;
        case EPbPropertyBalance:
            err = iPdPlayer->GetBalance(value);
            value=Balance(value);
            break;
        case EPbPropertyDuration:
            {
            TTimeIntervalMicroSeconds duration = iPdPlayer->Duration();
            value = duration.Int64() / KPbMilliMultiplier;
            }
            break;
        case EPbPropertyPosition:
            {
            TTimeIntervalMicroSeconds pos;
            iPdPlayer->GetPosition(pos);
            value = pos.Int64() / KPbMilliMultiplier;
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

    MPX_DEBUG1("CMPXProgressDownload::ValueL() exiting");
    }

// ----------------------------------------------------------------------------
// Gets a list of sub players, UPnP only
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::SubPlayerNamesL()
    {
    iObs->HandleSubPlayerNames(KProgressDownloadUid, NULL, ETrue, KErrNone);
    }

// ----------------------------------------------------------------------------
// Select a sub player
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::SelectSubPlayerL(TInt /*aIndex*/)
    {
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
// Returns current sub player name
// ----------------------------------------------------------------------------
//
const TDesC& CMPXProgressDownload::SubPlayerName()
    {
    return KNullDesC; //No subplayer name for local playback
    }

// ----------------------------------------------------------------------------
// Current sub player index
// ----------------------------------------------------------------------------
//
TInt CMPXProgressDownload::SubPlayerIndex() const
   {
   return KErrNotFound;
   }

// ----------------------------------------------------------------------------
// Gets media properties
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::MediaL(const TArray<TMPXAttribute>& aAttrs)
    {
    MPX_DEBUG1("CMPXProgressDownload::MediaL() entering");

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

    // Get number of metadata
    TInt metaCount = 0;

    // Get metadata from MMF
    TInt error = iPdPlayer->GetNumberOfMetaDataEntries(metaCount);
    MPX_DEBUG3("CMPXProgressDownload::MediaL(): metaCount = %d, error = %d", metaCount, error);
    CMMFMetaDataEntry* metaData = NULL;

    if (!error)
        {
        if ( metaCount > 0 )
            {
            for (TInt i = 0; i < metaCount; ++i)
                {
                metaData = iPdPlayer->GetMetaDataEntryL(i);
                CleanupStack::PushL(metaData);

                if (metaData->Name().CompareF(KMMFMetaEntrySongTitle()) == 0 &&
                    attrG & EMPXMediaGeneralTitle)
                    { // TODO to check request
                    if ( metaData->Value().Length() > 0 )
                        {
                        media->SetTextValueL(
                            TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                            metaData->Value());
                        }
                    else if ( iPdPath )
                        {
                        TParsePtrC ptr( *iPdPath );
                        media->SetTextValueL(
                            TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                            ptr.Name() );
                        }
                    else
                        {
                        media->SetTextValueL(
                            TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                            KNullDesC );
                        }
                    }
                else if( ( metaData->Name().CompareF( KMMFMetaEntryArtist() ) == 0 ||
                        metaData->Name().CompareF( KMMFMetaEntryAuthor() ) == 0 ) &&
                    attrM & EMPXMediaMusicArtist)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicArtist),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryAlbum()) == 0 &&
                    attrM & EMPXMediaMusicAlbum)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbum),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryYear()) == 0 &&
                    attrM & EMPXMediaMusicYear)
                    {
                    TInt year;
                    TLex lex( metaData->Value() );
                    lex.Val( year );

                    TDateTime dt;
                    dt.SetYear( year );
                    TTime time( dt );

                    media->SetTObjectValueL(
                            TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicYear),
                            time.Int64());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryComment()) == 0 &&
                    attrG & EMPXMediaGeneralComment)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralComment),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryComposer()) == 0 &&
                    attrM & EMPXMediaMusicComposer)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicComposer),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryAlbumTrack()) == 0 &&
                    attrM & EMPXMediaMusicAlbumTrack)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbumTrack),
                        metaData->Value());
                    }
                else if (metaData->Name().CompareF(KMMFMetaEntryGenre()) == 0 &&
                    attrM & EMPXMediaMusicGenre)
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicGenre),
                        metaData->Value());
                    }
                //else if (metaData->Name().CompareF(KMMFMetaEntryWOAF()) == 0 &&
                //    attrM & EMPXMediaMusicGenre)
                //    {
                    //media->SetTextValueL(
                    //    TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicGenre),
                    //    metaData->Value());
                    //}
                else if (metaData->Name().CompareF(KMMFMetaEntryAPIC()) == 0 &&
                    attrM & EMPXMediaMusicAlbumArtFileName)
                    {
                    // TODO check collection if user defined album art available
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbumArtFileName),
                        *iPdPath);
                    }
                CleanupStack::PopAndDestroy(metaData);
                metaData = NULL;
                }
            }
        else // metaCount == 0
            {
            // no metadata available, use file path for a couple of the attributes
            if ( attrG & EMPXMediaGeneralTitle )
                { // TODO to check request
                MPX_DEBUG2( "CMPXProgressDownload::MediaL() iPdPath = 0x%x", iPdPath );
                if ( iPdPath )
                    {
                    TParsePtrC ptr( *iPdPath );
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                        ptr.Name() );
                    }
                else
                    {
                    media->SetTextValueL(
                        TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                        KNullDesC );
                    }
                }
            if ( ( attrM & EMPXMediaMusicAlbumArtFileName ) && iPdPath )
                {
                // TODO check collection if user defined album art available
                media->SetTextValueL(
                    TMPXAttribute(KMPXMediaIdMusic,EMPXMediaMusicAlbumArtFileName),
                    *iPdPath);
                }
            }
        }
    else // if error, then use the file path to display (for the Active Idle case)
        {
        if ( attrG & EMPXMediaGeneralTitle )
            {
            MPX_DEBUG2( "**** CMPXProgressDownload::MediaL() iPdPath = 0x%x", iPdPath );
            if ( iPdPath )
                {
                TParsePtrC ptr( *iPdPath );
                media->SetTextValueL(
                    TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                    ptr.Name() );
                }
            else
                {
                media->SetTextValueL(
                    TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralTitle),
                    KNullDesC );
                }
            }
        }


   if (attrG & EMPXMediaGeneralUri)
        {
        media->SetTextValueL(
            TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralUri),
            *iPdPath);
        }

    if (attrG & EMPXMediaGeneralDuration)
        {
        TTimeIntervalMicroSeconds duration = iPdPlayer->Duration();
        media->SetTObjectValueL<TInt>(
               TMPXAttribute(KMPXMediaIdGeneral, EMPXMediaGeneralDuration),
               duration.Int64() / KPbMilliMultiplier);
        }

    // Set bitrate
    TPckgBuf<TMMFAudioConfig> data;

    const TMMFMessageDestinationPckg
        destinationPckg(KUidInterfaceMMFAudioController);

    if (attrA & EMPXMediaAudioBitrate)
        {
        error = iPdPlayer->CustomCommandSync(destinationPckg,
            EMMFAudioControllerGetSourceBitRate, KNullDesC8, KNullDesC8, data);
        if (!error)
            {
            media->SetTObjectValueL<TInt>(
                   TMPXAttribute(KMPXMediaIdAudio, EMPXMediaAudioBitrate),
                   data().iSampleRate);
            }
        }

    if (attrA & EMPXMediaAudioSamplerate)
        {

        // Set sampling rate
        error = iPdPlayer->CustomCommandSync(destinationPckg,
            EMMFAudioControllerGetSourceSampleRate, KNullDesC8, KNullDesC8, data);
        if (!error)
            {
            media->SetTObjectValueL<TInt>(
                   KMPXMediaAudioSamplerate,
                   data().iSampleRate);
            }
        }

   if (attrG & EMPXMediaGeneralSize)
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
        RApaLsSession aps;
        error  = aps.Connect(); // always fail in console test
        if (KErrNone == error)
            {
            CleanupClosePushL(aps);
            TDataType dataType;
            TUid ignore;
            if(aps.AppForDocument(iPdPath->Des(),ignore,dataType)==KErrNone)
                {
                media->SetTextValueL(
                    TMPXAttribute(KMPXMediaIdGeneral,EMPXMediaGeneralMimeType),
                    dataType.Des());
                }
            CleanupStack::PopAndDestroy(&aps);
            } // Notes, STIF console test always fail
        }

    // Set DRM info
    // Only try to get DRM info if the song has completed downloading
    MPX_DEBUG2("CMPXProgressDownload::MediaL(): iDownloadState = %d", iDownloadState);
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
                                drmMedia->ValueTObjectL<TInt>( mpxAtt ));
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
                                drmMedia->ValueTObjectL<TBool>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmStartTime:
                        case EMPXMediaDrmEndTime:
                        case EMPXMediaDrmIntervalStartTime:
                        case EMPXMediaDrmAccumulatedTime:
                            {
                            TInt64 val(
                                drmMedia->ValueTObjectL<TInt64>( mpxAtt ));
                            media->SetTObjectValueL( mpxAtt, val );
                            break;
                            }
                        case EMPXMediaDrmInterval:
                            {
                            TTimeIntervalSeconds val(
                                drmMedia->ValueTObjectL<TTimeIntervalSeconds>(mpxAtt));
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

    MPX_DEBUG1("CMPXProgressDownload::MediaL() exiting");
    }

// ----------------------------------------------------------------------------
// Cancel request
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::CancelRequest()
    {
    }

// ----------------------------------------------------------------------------
// File open complete event
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::MapcInitComplete(TInt aError,
                                const TTimeIntervalMicroSeconds& aDuration)
    {
    MPX_DEBUG2("CMPXProgressDownload::MapcInitComplete(%d) entering", aError);

    iState = EStateInitialised;

    // Restore volume level
    if ( KErrNone == aError )
        {
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
        }

    if ( iPdPlayer )
        {
        MMMFDRMCustomCommand* drmCustom = iPdPlayer->GetDRMCustomCommand();
        if ( drmCustom )
            {
            drmCustom->DisableAutomaticIntent( ETrue );
            }
        }
    if ( EPbDlStateDownloadCompleted == iDownloadState )
        {
        TRAP_IGNORE( iDrmMediaUtility->InitL( *iPdPath ));
        }

    if ( EPbDlStateBuffering == iDownloadState )
        {
        iDownloadState = EPbDlStateDownloading;
        TRAP_IGNORE(
            iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                                     iDownloadState, aError));
        }

    TRAP_IGNORE(
        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPActive,
                                 ETrue,aError);
        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPInitialised,
                                aDuration.Int64()/KPbMilliMultiplier,aError);
        iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPaused,
                                 0,aError));

    MPX_DEBUG1("CMPXProgressDownload::MapcInitComplete() exiting");
    }

// ----------------------------------------------------------------------------
// File play complete event
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::MapcPlayComplete(TInt aError)
    {
    MPX_DEBUG2("CMPXProgressDownload::MapcPlayComplete(%d) entering", aError);

    iState = EStateNotInitialised;

    // START WORKAROUND
    // Check if error is KErrDied and played to the end, then just
    // treat that as a normal play complete and overwrite the error
    // handling here
    // NOTE: This is only a temporary workaround for S60 3.1.
    // S60 3.2 will have a proper fix in the PD Utility
    if ( KErrDied == aError )
        {
        MPX_DEBUG2("MPXProgressDownload::MapcPlayComplete(): iDownloadState = %d", iDownloadState);
        if ( EPbDlStateDownloadCompleted == iDownloadState )
            {
            TTimeIntervalMicroSeconds dur( iPdPlayer->Duration() );
            TTimeIntervalMicroSeconds pos;
            iPdPlayer->GetPosition(pos);
            if ( Abs( dur.Int64() - pos.Int64() ) < KProgressPlayEndThreshold &&
                 dur.Int64() > KProgressPlayEndThreshold )
                {
                MPX_DEBUG1("CMPXProgressDownload::MapcPlayComplete(): Resetting error to KErrNone");
                aError = KErrNone;
                }
            }
        }
    // END WORKAROUND

    // KErrEof is sometimes returned when play finishes at the end
    if ( KErrEof == aError )
        {
        aError = KErrNone;
        }

    if ( iConsumeStarted )
        {
        if ( KErrNone != aError )
            {
            TRAP_IGNORE( ConsumeRightsL( ContentAccess::EPause ) );
            }
        else
            {
            TRAP_IGNORE( ConsumeRightsL( ContentAccess::EStop ) );
            }
        iConsumeStarted = EFalse;
        }

    // If killed by audio policy, mimic a paused state
    if ( KErrDied == aError )
        {
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                 0,
                                 aError );
        }
    else
        {
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPlayComplete,
                                 0,
                                 aError);
        }

    MPX_DEBUG1("CMPXProgressDownload::MapcPlayComplete() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXProgressDownload::Paused
// -----------------------------------------------------------------------------
//
void CMPXProgressDownload::Paused()
    {
    MPX_DEBUG1("CMPXProgressDownload::Paused() entering");

    // This callback will only be called when the PD Utility runs out
    // of data (i.e. all downloaded content has been played).
    iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                             0,
                             KErrUnderflow);
    MPX_DEBUG1("CMPXProgressDownload::Paused() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXProgressDownload::Playing
// -----------------------------------------------------------------------------
//
void CMPXProgressDownload::Playing()
    {
    MPX_DEBUG1("CMPXProgressDownload::Playing() entering");
    iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPPlaying,
                             0,
                             KErrNone);
    MPX_DEBUG1("CMPXProgressDownload::Playing() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXProgressDownload::HandleDownloadEventL
// -----------------------------------------------------------------------------
//
void CMPXProgressDownload::HandleDownloadEventL(TUint aTransactionID,
                                                TBrCtlDownloadEvent aEvent,
                                                TUint aValue)
    {
    MPX_DEBUG4("CMPXProgressDownload::HandleDownloadEventL(%d, %d, %d) entering",
               aTransactionID, aEvent, aValue);

    // Notes. It is API in current music player which is called by MusicShop
    //
    if (iPdPlayer)
        {
        iPdPlayer->HandleDownloadEventL( aTransactionID, aEvent, aValue );

        __ASSERT_DEBUG(
            aTransactionID == iTransactionId,
            User::Panic(_L("CMPlayerAudioUIController::HandleProgressiveDownloadEventL"),
                        KErrArgument));

        switch (aEvent)
            {
            case EDownloadEventStarted:
                {
                iDownloadState = EPbDlStateBuffering;
                iDownloadSize = static_cast<TInt>( aValue );

                // send new state
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                            iDownloadState,
                            KErrNone);
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPDownloadStarted,
                            iDownloadSize,
                            KErrNone );
                break;
                }
            case EDownloadEventCompleted:
                {
                iDownloadState = EPbDlStateDownloadCompleted;
                iDownloadSize = static_cast<TInt>( aValue );
                iDownloadBytes = iDownloadSize;
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                            iDownloadState,
                            KErrNone);
                break;
                }
            case EDownloadEventCanceled:
                {
                iDownloadState = EPbDlStateNotDownloading;
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                            iDownloadState,
                            KErrNone);
                break;
                }
            case EDownloadEventError:
                {
                iDownloadState = EPbDlStateDownloadError;
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                            iDownloadState,
                            KErrNone);
                break;
                }
            case EDownloadEventProgress:
                {
                if ( EPbDlStateDownloadPaused == iDownloadState )
                    {
                    iDownloadState = EPbDlStateDownloading;
                    iObs->HandlePluginEvent(
                                MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                                iDownloadState,
                                KErrNone);
                    }
                iDownloadBytes = static_cast<TInt>( aValue );
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPDownloadPositionChanged,
                            iDownloadBytes,
                            KErrNone);
                break;
                }
            case EDownloadEventPaused:
                {
                iDownloadState = EPbDlStateDownloadPaused;
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPDownloadStateChanged,
                            iDownloadState,
                            KErrNone);
                break;
                }
            case EDownloadEventPausable:
                {
                iFeatureFlag |= EPbFeaturePdPausable;
                iObs->HandlePluginEvent(
                            MMPXPlaybackPluginObserver::EPSupportedFeaturesChanged,
                            iFeatureFlag,
                            KErrNone);
                break;
                }
            default:
                {
                // pass
                break;
                }
            }
        }
    MPX_DEBUG1("CMPXProgressDownload::HandleDownloadEventL() exiting");
    }

// ----------------------------------------------------------------------------
// Handle a change in a setting value.
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::HandleSettingChange(
    const TUid& aRepositoryUid,
    TUint32 aSettingId )
    {
    MPX_DEBUG1("CMPXProgressDownload::HandleSettingChange() entering");

    if ( KCRUidMPXSettings == aRepositoryUid &&
         KMPXPlaybackVolume == aSettingId )
        {
        MPX_DEBUG1("CMPXProgressDownload::HandleSettingChange() Volume setting changed");
        TInt currentVol( 0 );
        MPX_TRAPD( volError, currentVol = iVolumeWatcher->CurrentValueL() );
        if ( KErrNone == volError )
            {
            SetVolume( currentVol );
            }
        }
    else if ( KCRUidMPXSettings == aRepositoryUid &&
         KMPXPlaybackMute == aSettingId )
        {
        MPX_DEBUG1("CMPXProgressDownload::HandleSettingChange() Mute setting changed");
        TBool mute( EFalse );
        MPX_TRAPD( error, mute = static_cast<TBool>(iMuteWatcher->CurrentValueL()) );
        if ( error == KErrNone )
            {
            TInt oldVolume( 0 );
            iPdPlayer->GetVolume( oldVolume );
            if ( (mute && oldVolume != 0) || (!mute && oldVolume == 0) )
                {
                SetMute( mute );
                }
            }
        }
    MPX_DEBUG1("CMPXProgressDownload::HandleSettingChange() exiting");
    }

// ----------------------------------------------------------------------------
// Sets the volume level in audio controller
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::SetVolume( TInt aVolume )
    {
    MPX_DEBUG2("CMPXProgressDownload::SetVolume(%d) entering", aVolume);

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
        TInt newVolume( aVolume * iPdPlayer->MaxVolume() / 100 );
        MPX_DEBUG2("CMPXProgressDownload::SetVolume(): Setting volume = %d", newVolume);

        // First check if MMF Audio player's volume is changed by new value
        TInt oldVolume( 0 );
        iPdPlayer->GetVolume( oldVolume );
        if ( newVolume != oldVolume )
            {
            iPdPlayer->SetVolume( newVolume );
            changed = ETrue;
            }
        }

    // Change setting in cenrep
    TInt currentVol( 0 );
    MPX_TRAPD( volError, currentVol = iVolumeWatcher->CurrentValueL() );
    if ( volError == KErrNone && aVolume != currentVol )
        {
        MPX_TRAP( volError, iVolumeWatcher->SetValueL( aVolume ) );
        }

    // Notify observer if value changed
    if ( changed )
        {
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPVolumeChanged,
                                 aVolume,
                                 KErrNone);
        }

    MPX_DEBUG1("CMPXProgressDownload::SetVolume() exiting");
    }

// ----------------------------------------------------------------------------
// Sets the volume level in audio controller
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::SetMute( TBool aMute )
    {
    MPX_DEBUG3("-->CMPXProgressDownload::SetMute 0x%08x vol (%d)", this, aMute);

    TBool changed( EFalse );
    // Change MMF Audio player's volume
    TInt currentVolume(0);
    iPdPlayer->GetVolume(currentVolume);
    if ( aMute && currentVolume != 0 )
        {
        iVolume = currentVolume;
        iPdPlayer->SetVolume(0);
        changed = ETrue;
        }
    else if ( !aMute && currentVolume == 0 )      // UnMute
        {
        iPdPlayer->SetVolume(iVolume);
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

    MPX_DEBUG3("<--CMPXProgressDownload::SetMute 0x%08x vol (%d)", this, aMute);
    }

// ----------------------------------------------------------------------------
// Consumes the rights for the current media
// ----------------------------------------------------------------------------
//
void CMPXProgressDownload::ConsumeRightsL(ContentAccess::TIntent aIntent)
    {
    MPX_DEBUG2("-->CMPXProgressDownload::ConsumeRightsL(%d)", aIntent);
    if ( iPdPlayer )
        {
        MMMFDRMCustomCommand* drmCustom = iPdPlayer->GetDRMCustomCommand();
        if ( drmCustom )
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
            MPX_DEBUG2("-->CMPXProgressDownload::ConsumeRightsL(): Executing intent %d", aIntent);
            TInt returnCode( drmCustom->ExecuteIntent(aIntent) );
            MPX_DEBUG2("CMPXProgressDownload::ConsumeRightsL() ExecuteIntent return (%d)", returnCode);
            User::LeaveIfError(returnCode);
            }
        }
    MPX_DEBUG2("<--CMPXProgressDownload::ConsumeRightsL(%d)", aIntent);
    }

// End of file
