/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active Idle player plug-in
*
*/


#include <ecom/implementationproxy.h>
#include <aiutility.h>

#include <AknUtils.h>
#include <StringLoader.h>
#include <bautils.h>
#include <avkon.rsg>

#include <data_caging_path_literals.hrh> 	// KDC_APP_BITMAP_DIR
#include <apgcli.h>           				// RApaLsSession
#include <apacmdln.h>         				// CApaCommandLine
#include <apgtask.h>

#include <mpxlog.h>
#include <mpxconstants.h> 					// KAppUidMusicPlayer
#include <mpxparameter.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <aiplayerpluginresource.rsg>

#include "aiplayerplugin.h"
#include "aiplayerpluginuids.hrh"
#include "aiplayerplugincontentmodel.h"

_LIT(KMPXZeroDurationMark, "--");
const TInt KMPXMinSecSeparatorIndex = 2;
const TInt KMPXOneSecInMicroSecs = 1000000;
const TInt KMPXOneHourInSeconds = 60*60;
const TInt KMPXTimeIndicatorLength = 16;
const TInt KPlayerMusicPlayerParameterGranularity = 50;
_LIT( KMPXAiPlayerRscPath, "z:aiplayerpluginresource.rsc" );

const TInt KMPlayerResumeWaitTime = 1000000; // 1.0s

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::ConstructL
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::ConstructL()
    {
    MPX_DEBUG1("CAiPlayerPlugin::ConstructL() - begin");

    iInfo.iUid = KUidMusicPlayerPlugin;
    iInfo.iName.Copy(_L8("PlayerPlugin"));

    iContent   = AiUtility::CreateContentItemArrayIteratorL(KAiPlplContent);
    iResources = AiUtility::CreateContentItemArrayIteratorL(KAiPlplResources);
    iEvents    = AiUtility::CreateContentItemArrayIteratorL(KAiPlplEvents);

    // Read time format strings from AVKON resource
    iLongFormatString = StringLoader::LoadL(R_QTN_TIME_DURAT_LONG_WITH_ZERO);
    iShortFormatString = StringLoader::LoadL(R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO);

    iCoeEnv = CCoeEnv::Static();

    TParse parse;
    parse.Set(KMPXAiPlayerRscPath, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    BaflUtils::NearestLanguageFile(iCoeEnv->FsSession(), resourceFile);
    iResourceOffset = iCoeEnv->AddResourceFileL(resourceFile);
    iUnknownArtistText = StringLoader::LoadL(R_MPX_QTN_AIPP_UNKNOWN_ARTIST);

    iPlayStarted = EFalse;
    iCleanTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    MPX_DEBUG1("CAiPlayerPlugin::ConstructL() - end");
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::NewL
// ----------------------------------------------------------------------------
//
CAiPlayerPlugin* CAiPlayerPlugin::NewL()
    {
    CAiPlayerPlugin* self = new (ELeave) CAiPlayerPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::CAiPlayerPlugin
// ----------------------------------------------------------------------------
//
CAiPlayerPlugin::CAiPlayerPlugin()
    : iEngine(NULL)
    {
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::~CAiPlayerPlugin()
// ----------------------------------------------------------------------------
//
CAiPlayerPlugin::~CAiPlayerPlugin()
    {
    iObservers.Close();

    Release( iContent );
    Release( iResources );
    Release( iEvents );

    if ( iResourceOffset )
        {
        iCoeEnv->DeleteResourceFile(iResourceOffset);
        }

    delete iEngine;

    delete iLongFormatString;
    delete iShortFormatString;
    delete iUnknownArtistText;

    iCoeEnv = NULL;
    iCleanTimer->Cancel();
    delete iCleanTimer;
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::Resume
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::Resume( TAiTransitionReason aReason )
    {
    MPX_DEBUG1("CAiPlayerPlugin::ResumeL");
    if ( !iEngine )
        {
        TRAPD(err, iEngine = CAiPlayerPluginEngine::NewL(*this));
        if ( err == KErrNone )
        	{
			iState = iEngine->PlayerState();
			PlayerStateChanged( iState );
		    if (iState == EMPlayerStatePlaying || iState == EMPlayerStatePaused )
				{
				TrackInfoChanged( iEngine->Title(), iEngine->Artist() );
				VolumeChanged( iEngine->Volume() );
				PlaybackPositionChanged( iEngine->Position() );
				}
			}
        }
     else if ( aReason == EAiGeneralThemeChanged )
        {
        PlayerStateChanged( iEngine->PlayerState() );
        }

    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::Suspend
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::Suspend( TAiTransitionReason /*aReason*/ )
    {
    MPX_DEBUG1("CAiPlayerPlugin::Suspend");
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::Stop
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::Stop( TAiTransitionReason /*aReason*/ )
    {
    MPX_DEBUG1("CAiPlayerPlugin::Stop");
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::SubscribeL
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::SubscribeL( MAiContentObserver& aObserver )
    {
    MPX_DEBUG1("CAiPlayerPlugin::SubscribeL");
    return iObservers.AppendL(&aObserver);
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::ConfigureL
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::ConfigureL( RAiSettingsItemArray& /*aSettings*/ )
    {
    MPX_DEBUG1("CAiPlayerPlugin::ConfigureL");
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::Extension
// ----------------------------------------------------------------------------
//
TAny* CAiPlayerPlugin::Extension( TUid aUid )
    {
    if (aUid == KExtensionUidProperty)
        {
        return static_cast<MAiPropertyExtension*>(this);
        }
    else if (aUid == KExtensionUidEventHandler)
        {
        return static_cast<MAiEventHandlerExtension*>(this);
        }
    return NULL; // Requested extension not supported
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::GetPropertyL
// ----------------------------------------------------------------------------
//
TAny* CAiPlayerPlugin::GetPropertyL( TInt aProperty )
    {
    switch (aProperty)
        {
        case EAiPublisherInfo:
        return &iInfo;

        case EAiPublisherContent:
        return static_cast<MAiContentItemIterator*>(iContent);

        case EAiPublisherResources:
        return static_cast<MAiContentItemIterator*>(iResources);

        case EAiPublisherEvents:
        return static_cast<MAiContentItemIterator*>(iEvents);
        }

    User::Leave(KErrNotSupported);
    return NULL;
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::SetPropertyL
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::SetPropertyL( TInt aProperty, TAny* aValue )
    {
    if (aValue)
        {
        switch (aProperty)
            {
            case EAiPublisherInfo:
                {
                const TAiPublisherInfo* info =
                    static_cast<const TAiPublisherInfo*>(aValue);

                iInfo.iUid.iUid = info->iUid.iUid;
                iInfo.iName.Copy( info->iName );
                }
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::HandleEvent
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::HandleEvent( TInt aEvent, const TDesC& aParam )
    {
    MPX_DEBUG1("CAiPlayerPlugin::HandleEvent");
    TRAPD(err, DoHandleEventL( aEvent, aParam ) );
    if ( err != KErrNone )
    	{
		MPX_DEBUG2("CAiPlayerPlugin::HandleEvent err[%d]", err);
		}
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::DoHandleEventL
// ----------------------------------------------------------------------------
//
void CAiPlayerPlugin::DoHandleEventL( TInt aEvent, const TDesC& aParam )
    {
    MPX_DEBUG2("CAiPlayerPlugin::DoHandleEventL %d", aEvent);

	TInt volume = iEngine->Volume();
    switch (aEvent)
        {
        case EAiPlplEventVolInc:
            {
            iEngine->SetVolumeL(++volume);
            iLastSetVolume = volume;
            }
            break;
        case EAiPlplEventVolDec:
            {
            iEngine->SetVolumeL(--volume);
            iLastSetVolume = volume;
            }
            break;
    	case EAiPlplEventVolSet:
    		{
    		TLex lex;
    		lex.Assign(aParam);
    		if( lex.Val(volume) == KErrNone )
    		    {
        		iEngine->SetVolumeL(volume);
    		    }
            iLastSetVolume = volume;
    		}
        	break;
    	case EAiPlplEventVolMute:
    		{
    		iLastSetVolume = volume;
       		iEngine->SetVolumeL(0);
    		}
	        break;
    	case EAiPlplEventVolRestore:
    		{
    		if( iLastSetVolume == 0 )
    		    {
    		    // So that "muting muted" don't function funky way.
    		    iLastSetVolume = 1;
    		    }
       		iEngine->SetVolumeL(iLastSetVolume);
    		}
	        break;
        case EAiPlplEventLaunchLibrary:
            {
            MPX_DEBUG1("CAiPlayerPlugin::DoHandleEventL() EAiPlplEventLaunchLibrary");
            //Launch player
            TApaTaskList tasList(iEikonEnv->WsSession());
            TApaTask task = tasList.FindApp(KAppUidMusicPlayerX);
            if (task.Exists())
                {
                MPX_DEBUG1("CAiPlayerPlugin::DoHandleEventL() Music app is already launched. Go to Now Playing view.");
                RWsSession& wsSession( iCoeEnv->WsSession() );
                CMPXParameter* param = new ( ELeave ) CMPXParameter();
                CleanupStack::PushL( param );
                param->iType.iUid = KMPXPluginTypePlaybackUid;
                param->iCmdForward = EMPXCmdFwdNowPlaying;

                MPX_DEBUG1( "CAiPlayerPlugin::DoHandleEventL start Externalize" );
                CBufBase* buffer = CBufFlat::NewL( KPlayerMusicPlayerParameterGranularity );
                CleanupStack::PushL( buffer );
                RBufWriteStream writeStream( *buffer );
                CleanupClosePushL( writeStream );
                param->ExternalizeL( writeStream );
                writeStream.CommitL();
                buffer->Compress();
                CleanupStack::PopAndDestroy( &writeStream );

                MPX_DEBUG2( "CAiPlayerPlugin::DoHandleEventL start Send message, message size = %d", buffer->Size() );
                wsSession.SendMessageToWindowGroup( task.WgId(), KAppUidMusicPlayerX, buffer->Ptr( 0 ));
                MPX_DEBUG1( "CAiPlayerPlugin::DoHandleEventL Send message complete" );
                CleanupStack::PopAndDestroy( buffer );
                CleanupStack::PopAndDestroy( param );
                }
            else
                {
				// Launch Music Player Application
                MPX_DEBUG1("CAiPlayerPlugin::DoHandleEventL() Launch Music app for the first time");
				RProcess process;
				TApaAppInfo appInfo;
				RApaLsSession session;
				TInt res = session.Connect(); // Ignore error
				CleanupClosePushL( session );
				TInt err = session.GetAppInfo( appInfo, KAppUidMusicPlayerX );
				if ( !err )
						{
						process.Create( appInfo.iFullName, KNullDesC );// Ignore error
						TRAP( err,
							{
							CApaCommandLine* commandLine = CApaCommandLine::NewLC();
							commandLine->SetDocumentNameL( KNullDesC );
							commandLine->SetExecutableNameL( appInfo.iFullName );
							commandLine->SetProcessEnvironmentL( process );
							session.StartApp( *commandLine ); // Ignore error
							CleanupStack::PopAndDestroy(); // commandLine
							});
						process.Resume();
						process.Close();
						}
				CleanupStack::PopAndDestroy(); // Close RApaLsSession session
                }
            }
            break;
        default:
        	break;
        }
    }

// ----------------------------------------------------------------------------
// CAiPlayerPlugin::ClearL
// ----------------------------------------------------------------------------
//
TInt CAiPlayerPlugin::ClearL(TAny* aPtr)
	{
    MPX_DEBUG1("CAiPlayerPlugin::ClearL");
  	CAiPlayerPlugin* plugin = reinterpret_cast<CAiPlayerPlugin*> (aPtr);
    for (TInt i = 0; i < plugin->iObservers.Count(); i++)
    	{
        MAiContentObserver* observer = plugin->iObservers[i];
        observer->Clean((MAiPropertyExtension&) *plugin, EAiPlplContentArtistCaption, 1);
        observer->Clean((MAiPropertyExtension&) *plugin, EAiPlplContentTitleCaption, 1);
        observer->Clean((MAiPropertyExtension&) *plugin, EAiPlplContentDurationCaption, 1);
        observer->Clean((MAiPropertyExtension&) *plugin, EAiPlplContentElapsedTime,1);
        observer->Clean((MAiPropertyExtension&) *plugin, EAiPlplContentStatus, 1);
        observer->Clean((MAiPropertyExtension&) *plugin, EAiPlplContentVolume,1);
        observer->Commit(0);
    	}
    plugin->iCleanTimer->Cancel();
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CAiPlayerPlugin::PlayerStateChanged
// -----------------------------------------------------------------------------
//
void CAiPlayerPlugin::PlayerStateChanged( TMPlayerState aState )
    {
    MPX_DEBUG2("CAiPlayerPlugin::PlayerStateChanged [%d]", aState);
    iState = aState;
    for (TInt i = 0; i < iObservers.Count(); i++)
        {
        MAiContentObserver* observer = iObservers[i];
        observer->StartTransaction(reinterpret_cast<TInt32>(this));
        switch(iState)
            {
            case EMPlayerStatePlaying:
                {
       			iPlayStarted = ETrue;
       			iCleanTimer->Cancel();
                observer->Publish(*this,
                                  EAiPlplContentStatus,
                                  EAiPlplResourcePlayIcon,
                                  1);
                }
                break;
            case EMPlayerStatePaused:
                {
       			iPlayStarted = ETrue;
                iCleanTimer->Cancel();
                observer->Publish(*this,
                                  EAiPlplContentStatus,
                                  EAiPlplResourcePauseIcon,
                                  1);
                PlaybackPositionChanged(iEngine->Position());
                }
                break;
            case EMPlayerStateSeeking:
                PlaybackPositionChanged(iEngine->Position());
                break;
            default:    // EMPlayerStateOther
       			if ( iPlayStarted )
       				{
       				iCleanTimer->Start(KMPlayerResumeWaitTime,
       			                        KMPlayerResumeWaitTime,
       			                        TCallBack(ClearL,this));
       				}
       			iPlayStarted = EFalse;
                break;
            }
        observer->Commit(reinterpret_cast<TInt32>(this));
        }
    }

// -----------------------------------------------------------------------------
// CAiPlayerPlugin::TrackInfoChanged
// -----------------------------------------------------------------------------
//
void CAiPlayerPlugin::TrackInfoChanged( const TDesC& aTitle, const TDesC& aArtist )
    {
    MPX_DEBUG1("CAiPlayerPlugin::TrackInfoChanged");
    for (TInt i = 0; i < iObservers.Count(); i++)
        {
        MAiContentObserver* observer = iObservers[i];
        observer->StartTransaction(reinterpret_cast<TInt32>(this));

        if ( &aTitle && aTitle.Length() ) //Check if hte reference exists add if is not empty
            {
            observer->Publish(*this, EAiPlplContentTitleCaption, aTitle, 1);
            }
        if ( &aArtist && aArtist.Length() ) //Check if hte reference exists add if is not empty
            {
            observer->Publish(*this, EAiPlplContentArtistCaption, aArtist, 1);
            }
        else
            {
            observer->Publish(*this, EAiPlplContentArtistCaption, *iUnknownArtistText, 1);
            }
        observer->Commit(reinterpret_cast<TInt32>(this));
        }
    }

// -----------------------------------------------------------------------------
// CAiPlayerPlugin::PlaybackPositionChanged
// -----------------------------------------------------------------------------
//
void CAiPlayerPlugin::PlaybackPositionChanged( TInt aPosition )
    {
    MPX_DEBUG1("CAiPlayerPlugin::PlaybackPositionChanged");

    TBuf<KMPXTimeIndicatorLength> elapsed;
    TBuf<KMPXTimeIndicatorLength> total;

    TInt64 playbackPosInSeconds;
    TInt64 totalLengthInSeconds;

    playbackPosInSeconds = aPosition;
    totalLengthInSeconds = iEngine->Duration();

    TPtrC format = *iShortFormatString;

    if (totalLengthInSeconds >= KMPXOneHourInSeconds)
        {
        // For tracks longer than an hour we use different time format and a
        // slightly different layout which has more space for the time labels.
        format.Set(*iLongFormatString);
        }

    TTime elapsedTime(playbackPosInSeconds * KMPXOneSecInMicroSecs);
    TTime totalTime(totalLengthInSeconds * KMPXOneSecInMicroSecs);

    if (aPosition == 0)
    	{
    	totalLengthInSeconds = 0;
    	}

    // Convert total playing time to texts.
    elapsedTime.FormatL(elapsed, format);

    if (totalLengthInSeconds)
        {
        // Time remaining
        totalTime.FormatL(total, format);
        }
    else
        {
        // Time remaining: --:--
        TLocale locale;
        TBuf<KMPXTimeIndicatorLength> pos;
        TChar separator = locale.TimeSeparator(KMPXMinSecSeparatorIndex);
        total = KMPXZeroDurationMark;
        total.Append(separator);
        total += KMPXZeroDurationMark;
        }

    AknTextUtils::LanguageSpecificNumberConversion(elapsed);
    AknTextUtils::LanguageSpecificNumberConversion(total);

    iElapsedTime.Copy(elapsed);

    iDuration.Copy(elapsed);
    iDuration.Append(_L("/"));
    iDuration.Append(total);

    if ( iState == EMPlayerStatePlaying || iState == EMPlayerStatePaused || iState == EMPlayerStateSeeking )
        {
        for (TInt i = 0; i < iObservers.Count(); i++)
            {
            MAiContentObserver* observer = iObservers[i];
            observer->StartTransaction(reinterpret_cast<TInt32>(this));

            //for (Classic view)
            observer->Publish(*this,
                           EAiPlplContentElapsedTime,
                           iElapsedTime,
                           1);

            //for (Navibar view)
            observer->Publish(*this,
                           EAiPlplContentDurationCaption,
                           iDuration,
                           1);

            observer->Commit(reinterpret_cast<TInt32>(this));
            }
        }
    }

// -----------------------------------------------------------------------------
// CAiPlayerPlugin::VolumeChanged
// -----------------------------------------------------------------------------
//
void CAiPlayerPlugin::VolumeChanged( TInt aVolume )
    {
    MPX_DEBUG1("CAiPlayerPlugin::VolumeChanged");

    for (TInt i = 0; i < iObservers.Count(); i++)
        {
        MAiContentObserver* observer = iObservers[i];
        observer->StartTransaction(reinterpret_cast<TInt32>(this));

        // Order of enum TAiPlplPluginResourceIds is important
        // must stay EAiPlplResourceVol0= 0 ... EAiPlplResourceVol10 = 10
        // for this to work
        if ( aVolume>=0 && aVolume<=10 )
            {
            observer->Publish(*this,
                              EAiPlplContentVolume,
                              aVolume,
                              1);
            }
        observer->Commit(reinterpret_cast<TInt32>(this));
        }
    }

// ======== GLOBAL FUNCTIONS ========

// Provide a key pair value table for ECOM.
// Used to identify the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY( AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PLAYERPLUGIN,
                                CAiPlayerPlugin::NewL )
};


// Return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
}

