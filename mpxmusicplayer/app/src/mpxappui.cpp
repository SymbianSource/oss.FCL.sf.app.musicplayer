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
* Description:  Application UI class required by AVKON application architecture.
*
*/



// INCLUDE FILES
#include <AknGlobalNote.h>
#include <avkon.hrh>
#include <apgwgnam.h>
#include <hlplch.h>
#include <e32base.h>
#include <e32property.h>
#include <eikproc.h>
#include <apacln.h>
#include <coeutils.h>
#include <StringLoader.h>
#include <textresolver.h>
#include <aknnotewrappers.h>
#include <AknTaskList.h>
#include <aknview.h>
#include <featmgr.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>
#include <ctsydomainpskeys.h>
#include <caf/caferr.h>
#include <eikapp.h>
#include <pathinfo.h>
#include <sysutil.h>
#include <apgcli.h>
#include <AknDlgShut.h>
#include <aknconsts.h>
#include <activeidle2domainpskeys.h>
#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
#include <AiwGenericParam.h>
#endif

#include <mpxplaybackutility.h>
#include <mpxcollectionutility.h>
#include <mpxplaybackmessage.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpx.rsg>
#include <mpxcollectionmessage.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionplugin.hrh>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediakeyhandler.h>
#include <mpxharvesterutility.h>
#include <mpxdrmmediautility.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>

// Cover UI start
//#ifdef __COVER_DISPLAY
#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include <MediatorCommandInitiator.h>
#include <MediatorDomainUIDs.h>
#include "mplayersecondarydisplayapi.h"
#include <featmgr.h>
//#endif
// Cover UI end

#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT 
#include "camesedrmhandler.h"
#endif 

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
#include <DocumentHandler.h>
#include <apmstd.h>
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER

#include "mpxinternalcrkeys.h"
#include "mpxcommandhandler.h"
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
#include <AiwGenericParam.h>
#include "mpxPDcommandhandler.h"
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
#include "mpxcommonuihelper.h"
#include <mpxcommonui.rsg> //for R_MPX_FILE_NOT_FOUND_TEXT
#include "mpxappui.h"
#include "mpxcontroller.h"
#include "mpxrestorepath.h"
#include "mpxdocument.h"
#include "mpxtlshelper.h"
#include "mpxlog.h"
#include "mpxappui.hrh"
#include <mpxconstants.h>
#include <mpxviewpluginmanager.h>
#include <mpxviewplugin.h>

#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>

// CONSTANTS
_LIT( KMPXLineChange, "\n" );
_LIT(KMPXMusicplayerPrivateFolder, "\\private\\");
_LIT(KMPXMusicPlayerExec, "mpx.exe" );

const TInt KMPXMusicPlayerAlreadySaved( -5000 );

// Application Uid for Active Idle app
#ifdef __ACTIVE_IDLE
const TUid KMPXUidIdleApp = { 0x101FD64C };
#else
const TUid KMPXUidIdleApp = KPSUidActiveIdle2; // use this one instead because the "aisystemuids.hrh" is not export
#endif

// MusicPlayer SIS Package UID
#define KMPXMusicPlayerSISPkgUid  0x101FFC62 

_LIT8( KMmMessage, "mm://musicsuite?action=exit" );
const TInt KMmUid3( 0x101f4cd2 );

// Granularity for ring tone mime types array
const TInt KMPXRingToneMimeTypesGranularity( 10 );

const TInt KMPXMinVolume( 0 );
const TInt KMPXMaxVolume(100);

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
_LIT8(KDataTypeRAM,"audio/x-pn-realaudio-plugin");
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER

const TInt KMaxIntLen( 10 );
_LIT8( KMVPrefix, "MVviewID:" );
const TInt KMVPrefixLen( 9 );

const TUid  KLocalPlaybackUid = { 0x101FFC06 };
const TUid  KPhoneAppUid      = {0x100058B3};
const TInt  KWaitNoteImpUid   = 0x101FFC6C; // refresh wait note UID
const TInt KLowestMemoryNeeded = 5242880; //5 Mbytes

const TUid KCRUidMPXMPSettings = {0x101FFCDC};
const TUint32 KMPXMPPlaybackRandomMode = 0x0B;
const TUint32 KMPXMPPlaybackRepeatMode = 0x0C;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor.
// ---------------------------------------------------------------------------
//
CMPXAppUi::CMPXAppUi()
    : iRingToneMimeTypes( KMPXRingToneMimeTypesGranularity ),
      iCurrentIndex( KErrNotFound ), iSaveMode( EMPXSaveModeIdle ),
      iStandAlone(EFalse),
      iMaxVolume(KMPXMaxVolume)
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
      ,iAppStartedByBrowser(EFalse)
      ,iPdPbvStartedByBrowser(EFalse)
      ,icollectionBrowsed(EFalse)
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
#ifdef FF_OOM_MONITOR2_COMPONENT   //__ENABLE_MPX_BUSYSETTING_FOR_OOM2
      ,iOomPriority( ROomMonitorSession::EOomPriorityNormal )
#endif //FF_OOM_MONITOR2_COMPONENT
    {
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::ConstructL()
    {
    MPX_FUNC( "CMPXAppUi::ConstructL" );
#ifndef __ENABLE_MSK
    BaseConstructL( EAknEnableSkin 
#ifdef SINGLE_CLICK_INCLUDED
    | EAknSingleClickCompatible
#endif
	);
#else
    BaseConstructL( EAknEnableSkin | EAknEnableMSK 
#ifdef SINGLE_CLICK_INCLUDED
    | EAknSingleClickCompatible
#endif
	);
#endif // __ENABLE_MSK

    iCommonUiHelper = CMPXCommonUiHelper::NewL();

// Cover UI start
//#ifdef __COVER_DISPLAY
// initialize FeatureManager
    FeatureManager::InitializeLibL();

    // Check updates from IAD, continue UI launching even if something fails there
	// Uncomment when to add IAD update checking functionality to music player
    // TRAP_IGNORE( CheckUpdatesL() );

    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        iCoverDisplay = ETrue;
        iCommandInitiator = CMediatorCommandInitiator::NewL( NULL );
        }
    else
        {
        iCoverDisplay = EFalse;
        }
//#endif // __COVER_DISPLAY
// Cover UI end

    if ( MPXUser::IsCallOngoing( EPSCTsyCallTypeH324Multimedia ) )
        {
        HBufC* text =
            StringLoader::LoadLC( R_MPX_VIDEO_CALL_ONGOING );
        CAknGlobalNote* globalInfoNote = CAknGlobalNote::NewLC();

// Cover UI start
//#ifdef __COVER_DISPLAY
    if ( iCoverDisplay )
        {
        CAknSDData* sddata = CAknSDData::NewL(
            KMPlayerNoteCategory,
            EMPlayerNoteVideoCall,
            KNullDesC8);
        globalInfoNote->SetSecondaryDisplayData(sddata); // ownership transferred
        }
//#endif //__COVER_DISPLAY
// Cover UI end

        globalInfoNote->ShowNoteL(
            EAknGlobalInformationNote,
            text->Des() );
        CleanupStack::PopAndDestroy( globalInfoNote );
        CleanupStack::PopAndDestroy( text );
        Exit();
        return;
        }

    //check memory
#ifdef FF_OOM_MONITOR2_COMPONENT
    CheckAvailableMemoryByOomL( KLowestMemoryNeeded );
#else
    CheckAvailableMemoryL( KLowestMemoryNeeded );
#endif    //FF_OOM_MONITOR2_COMPONENT

    if ( !IsEmbedded() )
        {
        // Do we support podcasting in music player?
        // Event if repository get fails, it will default to EFalse
        //
        TInt val(0);
        CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
        repository->Get( KMPXMPLocalVariation, val );
        iDisablePodcasting = val&KMPXDisablePodcastingOption ? ETrue : EFalse;
        iStartInAlbumMenu = val&KMPXStartMusicPlayerinArtistsandAlbums ? ETrue : EFalse;
        delete repository;
        CreateMemberVariablesL();

        TUid defaultView( iViewUtility->DefaultViewUid() );

        // Pre-load views in standalone mode
        iViewUtility->PreLoadViewsL( TUid::Uid( KMusicPlayerAppUidConstant ) );
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
        iCollectionUtility->Collection().OpenL(
            TUid::Uid(EMPXCollectionPluginMusic));
#endif

		if ( iStartInAlbumMenu )
			{

			// We want to open the music player to the albums level for 9.2
			// Changes in other areas will also be needed.

			CMPXCollectionPath* path = iCollectionUiHelper->MusicMenuPathL();
			CleanupStack::PushL( path );
			path->AppendL(3); // Albums
			MPX_DEBUG_PATH(*path);
			iCollectionUtility->Collection().OpenL( *path );
			CleanupStack::PopAndDestroy( path );
			}
		else
			{
			CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
			CleanupStack::PushL( path );
			MPX_DEBUG1("CMPXAppUi::ConstructL() current collection path");
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        MPX_DEBUG1("CMPXAppUi::ConstructL() restoring stale path");
        CMPXCollectionPath* mainMusicMenu = CMPXCollectionPath::NewL();
        CleanupStack::PushL( mainMusicMenu );
        mainMusicMenu->AppendL(0x101FFC3A);
        iCollectionUtility->Collection().OpenL( *mainMusicMenu );
        CleanupStack::PopAndDestroy( mainMusicMenu );
        CleanupStack::PopAndDestroy( path );
#else
        MPX_DEBUG1( "CMPXAppUi::ConstructL wrong path" );
        if( path->Levels() > 1 )
            {
            // Correct the path by opening 0 levels
            MPX_DEBUG1("CMPXAppUi::ConstructL() restoring stale path");
            CMPXCollectionPath* root = CMPXCollectionPath::NewL();
            CleanupStack::PushL( root );
            iCollectionUtility->Collection().OpenL( *root );
            CleanupStack::PopAndDestroy( root );
            }
        CleanupStack::PopAndDestroy( path );
#endif
			}


        if ( defaultView != KNullUid )
            {
            iViewUtility->SetAsDefaultViewL( defaultView );
            }
        else
            {
            iViewUtility->SetAsDefaultViewL(
                TUid::Uid( KMPXPluginTypeMainUid ) );
            }

#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        // Startup database checker
        iController = CMPXController::NewL(iDisablePodcasting);
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU

        // Only in stand alone mode
        iHarvesterUtility = CMPXHarvesterFactory::NewL();
        iHarvesterUtility->CheckForSystemEventsL();

#ifdef _DEBUG
        TBool iExitOptionHidden = EFalse;
#else // _DEBUG
        TBool iExitOptionHidden = iCommonUiHelper->ExitOptionHiddenL();
#endif // _DEBUG
        if(iExitOptionHidden)
            {
            // Do not offer choice to exit application from task swapper.
            CEikonEnv::Static()->SetSystem( ETrue );
            }

        RProcess proc;
        CleanupClosePushL( proc );
        CMPXCommonUiHelper::SetStandAloneModePId( proc.Id().Id() );
        CleanupStack::PopAndDestroy( &proc );
        }

    iRingToneMimeTypes.AppendL(_L("application/vnd.nokia.ringing-tone"));
    iRingToneMimeTypes.AppendL(_L("audio/x-midi"));
    iRingToneMimeTypes.AppendL(_L("audio/midi"));
    iRingToneMimeTypes.AppendL(_L("audio/sp-midi"));
    iRingToneMimeTypes.AppendL(_L("audio/amr"));
    iRingToneMimeTypes.AppendL(_L("audio/amr-wb"));
    iRingToneMimeTypes.AppendL(_L("audio/awb"));
    iRingToneMimeTypes.AppendL(_L("audio/x-amr"));
    iRingToneMimeTypes.AppendL(_L("audio/audio/vnd.nokia.mobile-xmf"));
    iLaunchModePlaying = EFalse;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXAppUi::~CMPXAppUi()
    {
    MPX_DEBUG1( "-->CMPXAppUi::~CMPXAppUi()" );
#ifdef FF_OOM_MONITOR2_COMPONENT
    TRAP_IGNORE( SetMpxOomPriorityL( ROomMonitorSession::EOomPriorityNormal ) );
#endif //FF_OOM_MONITOR2_COMPONENT

    if ( !IsEmbedded() )
        {
        CMPXCommonUiHelper::SetStandAloneModePId( 0 );
        }

// Cover UI Start
// #ifdef __COVER_DISPLAY

    FeatureManager::UnInitializeLib();
    delete iCommandInitiator;

// #endif // __COVER_DISPLAY
// Cover UI End
    delete iPathRestore;
    delete iController;
    delete iProcessParameter;
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
    delete iPdParameterHandler;
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
    delete iIdle;

    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->CommandL( EPbCmdClose ) );
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    if ( iViewUtility )
        {
        iViewUtility->RemoveObserver( this );
        iViewUtility->Close();
        }
    delete iCommonUiHelper;

    if( iCollectionUiHelper )
        {
        iCollectionUiHelper->Close();
        }

    if ( iCollectionHelper )
        {
        iCollectionHelper->Close();
        }
    if( iHarvesterUtility )
        {
        iHarvesterUtility->Close();
        }

    delete iMediaKeyHandler;
    delete iSettingsRepository;
    iFile.Close();
    iDeferredViewActivationUid.Close();
    delete iSavePath;
    iRingToneMimeTypes.Reset();

    delete iSaveHelper;
#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT 
    delete iCameseDrmHandler;
#endif 

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    delete iDocHandler;
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER

    CleanUpdateParams();

    MPX_DEBUG1( "<--CMPXAppUi::~CMPXAppUi()" );
    }


// ---------------------------------------------------------------------------
// Querying whether the application is launched in embedded mode or not.
// ---------------------------------------------------------------------------
//
TBool CMPXAppUi::IsEmbedded() const
    {
    return iEikonEnv->StartedAsServerApp();
    }


// ---------------------------------------------------------------------------
// Opens temporary playlist or album from buffer.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::OpenPlaylistFromBufferL(
    const TDesC& aBuffer,
    TDesC& aName )
    {
    MPX_FUNC( "CMPXAppUi::OpenPlaylistFromBufferL" );
    TPtrC bufPtr( aBuffer );
    TInt offset = 0;
    CMPXMediaArray* tracksArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( tracksArray );

    while ( bufPtr.Length() > 0 )
        {
        offset = bufPtr.Find( KMPXLineChange );
        CMPXMedia* track = CMPXMedia::NewL();
        CleanupStack::PushL( track );
        track->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem);
        track->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
        if ( offset == KErrNotFound )
            {
            TParsePtrC fn( bufPtr );
            track->SetTextValueL( KMPXMediaGeneralTitle, fn.Name() );
            track->SetTextValueL( KMPXMediaGeneralUri, bufPtr );
            // Set bufPtr to the end of buffer
            bufPtr.Set( bufPtr.Right( 0 ) ); //magic
            }
        else
            {
            TParsePtrC fn( bufPtr.Left( offset ) );
            track->SetTextValueL( KMPXMediaGeneralTitle, fn.Name() );
            track->SetTextValueL( KMPXMediaGeneralUri, fn.FullName() );
            // Move past the line feed
            bufPtr.Set( bufPtr.Mid( ++offset ) );
            }
        tracksArray->AppendL( track );
        CleanupStack::Pop( track );
        }

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem);
    media->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXPlaylist );
    if ( aName.Length() > 0 )
        {
        media->SetTObjectValueL( KMPXMediaGeneralTitle, aName );
        }
    media->SetCObjectValueL( KMPXMediaArrayContents, tracksArray );
    media->SetTObjectValueL( KMPXMediaArrayCount, tracksArray->Count() );
    iCollectionUiHelper->OpenL( MPXTlsHelper::HostUid(), *media,
                                this, EMPXCollectionPluginMusic );
    CleanupStack::PopAndDestroy( media );
    CleanupStack::PopAndDestroy( tracksArray );
    }

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
// -----------------------------------------------------------------------------
// CMPXAppUi::ForwardAiwParametersToMediaPlayer
// -----------------------------------------------------------------------------
//
CAiwGenericParamList* CMPXAppUi::CreateForwardAiwParametersLC(
        const CAiwGenericParamList* aParamList) const
    {
    MPX_FUNC( "CMPXAppUi::ForwardAiwParametersToMediaPlayerL" );

    //Read parameters that were appended to the parameter list

    //Creates a new parameter list which will be forwarded to Media Player
    CAiwGenericParamList* newParamList = CAiwGenericParamList::NewLC();

    if (aParamList)
        {
        TInt index = 0;

        //param contains the specific parameter being looked for in the parameter list
        //if found, append it again so it can be passed to Media Player

        //Move parameter
        const TAiwGenericParam* param = aParamList->FindFirst(index,
                                            EGenericParamAllowMove);
        if (param)
            {
            TBool allowMove(ETrue);
            TAiwVariant allowMoveVariant(allowMove);
            TAiwGenericParam genericParamAllowMove(EGenericParamAllowMove,
                                                   allowMoveVariant);
            newParamList->AppendL(genericParamAllowMove);
            }

        index = 0;
        param = NULL;
        //Save parameter
        param = aParamList->FindFirst(index, EGenericParamAllowSave);
        if (param)
            {
            TBool allowSave(ETrue);
            TAiwVariant allowSaveVariant(allowSave);
            TAiwGenericParam genericParamAllowSave(EGenericParamAllowSave,
                                                   allowSaveVariant);
            newParamList->AppendL(genericParamAllowSave);
            }

        index = 0;
        param = NULL;
        //Resolution parameter
        param = aParamList->FindFirst(index, EGenericParamResolution);
        if (param)
            {
            //For future use
            }

        index = 0;
        param = NULL;
        //File parameter
        param = aParamList->FindFirst(index, EGenericParamFile);
        if (param)
            {
            TDataType dataType( _L8("audio/x-pn-realaudio") );
            TAiwVariant filename( _L8("c:\\system\\temp\\RtspTemp.ram") );
            TAiwGenericParam param( EGenericParamFile, filename );
            newParamList->AppendL( param );
            }

        index = 0;
        param = NULL;
        //Access Point parameter
        param = aParamList->FindFirst(index, EGenericParamAccessPoint);
        if (param)
            {
            TInt32 value = 0;
            TInt32 ret = param->Value().Get(value);
            if (ret)
                {
                TAiwVariant accessPointVariant(value);
                TAiwGenericParam genericParamAccessPoint(EGenericParamAccessPoint,
                                                         accessPointVariant);
                newParamList->AppendL(genericParamAccessPoint);
                }
            }
        }
    return newParamList;
    }
#endif

// ---------------------------------------------------------------------------
// Opens the specified file in response to a corresponding message.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::OpenFileL(
    CFileStore*& /* aFileStore */,
    RFile& aFile,
    const CAiwGenericParamList* aParams )
    {
    MPX_FUNC( "CMPXAppUi::OpenFileL" );

    // Duplicate the file handle passed in before
    // creating the member variables.
    TFileName filename;
    aFile.FullName( filename );
    iFile.Close();
    iFile.Duplicate( aFile );
    aFile.Close();

    if ( IsEmbedded() )
        {
        CreateMemberVariablesL();
        iViewUtility->PreLoadViewL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ));
        }

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    // Get MIME type
    RApaLsSession apa;
    User::LeaveIfError( apa.Connect() );
    CleanupClosePushL( apa );
    TDataRecognitionResult result;
    User::LeaveIfError( apa.RecognizeData( iFile, result ));
    CleanupStack::PopAndDestroy( &apa );

    TDataType dataType = TDataType( KDataTypeRAM() );

    if ( result.iDataType == dataType )
        {
        CAiwGenericParamList* paramList = CreateForwardAiwParametersLC(aParams);
        iDocHandler->OpenFileEmbeddedL(iFile, dataType, *paramList);
        CleanupStack::PopAndDestroy(paramList);
        iDocHandler->SetExitObserver(this);
        }
    else
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER
        {
        // Open File with Collection Helper
        iCollectionUiHelper->OpenL( MPXTlsHelper::HostUid(), filename,
                                    this, EMPXCollectionPluginMusic );
        }
    }


// ---------------------------------------------------------------------------
// CMPXAppUi::ActivatePhoneAppL
// ---------------------------------------------------------------------------
//
void CMPXAppUi::ActivatePhoneAppL()
    {
    MPX_FUNC( "CMPXAppUi::ActivatePhoneAppL" );
    TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
    TApaTask apaTask = apaTaskList.FindApp( KPhoneAppUid );
    if ( apaTask.Exists() )
        {
        // Bring phone to foreground
        apaTask.BringToForeground();
        }

    // Move Music Player to background
    // magic: -1 = force wg to background
    }

// ----------------------------------------------------------------------------
// void CMPXAppUi::DeferredExitCallback
// call back function for deferred exit.
// ----------------------------------------------------------------------------
//
TInt CMPXAppUi::DeferredExitCallback( TAny* aPtr )
    {
    MPX_FUNC( "CMPXAppUi::DeferredExitCallback" );
    CMPXAppUi* self = static_cast<CMPXAppUi*>( aPtr );
    TRAP_IGNORE( self->HandleCommandL( EEikCmdExit ) );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Start a waitnote dialog
// ---------------------------------------------------------------------------
//
void CMPXAppUi::StartWaitNoteL( TWaitNoteType aNoteType )
    {
    CloseWaitNoteL();
    TUid waitnoteId = TUid::Uid( KMPXPluginTypeWaitNoteDialogUid );
    TPckg<TWaitNoteType> note = aNoteType;
    HBufC* arg = MPXUser::AllocL( note );
    CleanupStack::PushL( arg );
    iViewUtility->ActivateViewL( waitnoteId, arg );
    CleanupStack::PopAndDestroy( arg );
    }

// ---------------------------------------------------------------------------
// Close the waitnote dialog
// ---------------------------------------------------------------------------
//
void CMPXAppUi::CloseWaitNoteL()
    {
    TUid waitnoteId = TUid::Uid( KMPXPluginTypeWaitNoteDialogUid );
    TUid activeView = iViewUtility->ActiveViewType();
    if( activeView == waitnoteId )
        {
        CMPXViewPlugin* pi =
            iViewUtility->ViewPluginManager().PluginL( TUid::Uid(KWaitNoteImpUid) );
        pi->DeactivateView();
        }
    }

// ---------------------------------------------------------------------------
// From MMPXMediaKeyHandlerObserver.
// Handle media key commands.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleMediaKeyCommand(
    const TMPXPlaybackCommand aCommand,
    const TInt aData )
    {
    MPX_FUNC( "CMPXAppUi::HandleMediaKeyCommand" );

    switch ( aCommand )
        {
        case EPbCmdSetVolume:
            {
            TRAP_IGNORE(
            TMPXPlaybackState playerState( EPbStateNotInitialised );
            playerState = iPlaybackUtility->StateL();
            if ( !MPXUser::IsCallOngoing( KMPXCallTypeGenericVoice ) ||
                ( playerState == EPbStatePlaying ) ||
                ( playerState == EPbStateSeekingForward ) ||
                ( playerState == EPbStateSeekingBackward ) ||
                iPdParameterHandler )
                {
                TMPXPlaybackState pdPlayerState( EPbStateNotInitialised );
                if ( iPdParameterHandler )
                    {
                    pdPlayerState = iPdParameterHandler->PdStateL();
                    }
                
                // only process command if there no call ongoing
                // or if we're actively playing during call
                if ( IsForeground() || EPbStatePlaying == playerState || EPbStatePlaying == pdPlayerState )
                    {
					if ( aData != iCurrentVolume )
                        {
                        SetVolumeL( aData );
                        }
                    // popup volume control even if volume didn't change
                    HandlePopupL( EPbCmdSetVolume );
                    }
                }
            );
            break;
            }
        case EPbCmdMuteVolume:
            {
            TRAP_IGNORE(
            TMPXPlaybackState playerState( EPbStateNotInitialised );
            playerState = iPlaybackUtility->StateL();
            if ( !MPXUser::IsCallOngoing( KMPXCallTypeGenericVoice ) ||
                ( playerState == EPbStatePlaying ) ||
                ( playerState == EPbStateSeekingForward ) ||
                ( playerState == EPbStateSeekingBackward ) ||
                iPdParameterHandler )
                {
                TMPXPlaybackState pdPlayerState( EPbStateNotInitialised );
                if ( iPdParameterHandler )
                    {
                    pdPlayerState = iPdParameterHandler->PdStateL();
                    }
                // only process command if there no call ongoing
                // or if we're actively playing during call
                if ( IsForeground() || EPbStatePlaying == playerState || EPbStatePlaying == pdPlayerState )
                    {
                    MuteVolumeL();
                    HandlePopupL( EPbCmdMuteVolume );
                    }
                }
            );
            break;
            }
        case EPbCmdUnMuteVolume:
            {
            TRAP_IGNORE(
            TMPXPlaybackState playerState( EPbStateNotInitialised );
            playerState = iPlaybackUtility->StateL();
            if ( !MPXUser::IsCallOngoing( KMPXCallTypeGenericVoice ) ||
                ( playerState == EPbStatePlaying ) ||
                ( playerState == EPbStateSeekingForward ) ||
                ( playerState == EPbStateSeekingBackward ) ||
                iPdParameterHandler )
                {
                // only process command if there no call ongoing
                // or if we're actively playing during call
                TMPXPlaybackState pdPlayerState( EPbStateNotInitialised );
                if ( iPdParameterHandler )
                    {
                    pdPlayerState = iPdParameterHandler->PdStateL();
                    }
                if ( IsForeground() || EPbStatePlaying == playerState || EPbStatePlaying == pdPlayerState )
                    {
                    UnMuteVolumeL();
                    HandlePopupL( EPbCmdUnMuteVolume );
                    }
                }
            );
            break;
            }
        default:
            {
            // Dispatches all other commands

            TBool foregroundApp( IsForeground() );
            if ( !IsDisplayingMenuOrDialog() && foregroundApp )
                {
                // Forward the command to view to handle when
                // music is foreground app and not displaying
                // dialogs or the options menu
                if ( iView )
                    {                    
                    MPX_TRAPD( err, iView->HandleCommandL( MPXCmdForPbCmd(aCommand) ) );
                    MPX_DEBUG3( "CMPXAppUi::HandleMediaKeyCommand sending command %d to view err%d", MPXCmdForPbCmd(aCommand), err );
                    }
                }
            else
                {
                TMPXPlaybackState playerState( EPbStateNotInitialised );
                MPX_TRAPD( err, playerState = iPlaybackUtility->StateL() );
                if ( foregroundApp && IsDisplayingDialog() )
                    {
                    // displaying dialog in the foreground
                    if ( EPbStatePlaying == playerState ||
                         EPbStatePaused == playerState ||
                         EPbStateSeekingForward == playerState ||
                         EPbStateSeekingBackward == playerState ||
                         aCommand == EPbCmdPlay ||
                         aCommand == EPbCmdPlayPause  )
                        {
                        // In playing/paused state, all media keys are active.
                        // Now playing view is not bring to foreground.
                        // Seeking forward/backwards is also "playing"                       
                        MPX_TRAP( err, DoHandleMediaKeyCommandL( aCommand ) );
                        MPX_DEBUG3( "CMPXAppUi::HandleMediaKeyCommand Dialog displaying, sending command %d to DoHandleMediaKeyCommandL err%d", aCommand, err );
                        }
                    else
                        {
                        MPX_DEBUG3( "CMPXAppUi::HandleMediaKeyCommand Dialog displaying, inactive state, command %d, err%d ignored", aCommand , err );
                        }
                    }
                else
                    {
                    // displaying menu/not foreground, pass to view to handle only play command
                    if ( EPbStatePlaying == playerState ||
                        EPbStatePaused == playerState ||
                        EPbStateSeekingForward == playerState ||
                        EPbStateSeekingBackward == playerState )
                        {
                        MPX_TRAP( err, DoHandleMediaKeyCommandL( aCommand ) );
                        MPX_DEBUG3( "CMPXAppUi::HandleMediaKeyCommand Menu displaying/not foreground, sending command %d to DoHandleMediaKeyCommandL err%d", aCommand, err );
                        }
                    else if ( ( aCommand == EPbCmdPlay || aCommand == EPbCmdPlayPause ) && iView )
                        {
                        if ( !foregroundApp )
                            {                
                            // not foreground, play without loading playback view
                            MPX_TRAP( err, DoHandleMediaKeyCommandL( aCommand ) );
                            MPX_DEBUG3( "CMPXAppUi::HandleMediaKeyCommand Menu displaying/not foreground, active idle foreground, sending command %d to DoHandleMediaKeyCommandL err%d", aCommand, err );
                            }
                        else
                            {                           
                            MPX_TRAP( err, iView->HandleCommandL( MPXCmdForPbCmd(aCommand) ) );
                            MPX_DEBUG3( "CMPXAppUi::HandleMediaKeyCommand Menu displaying/not foreground, sending command %d to view err%d", aCommand, err );
                            }
                        }
                    else if ( aCommand == EPbCmdPlay || aCommand == EPbCmdPlayPause )
                        {
                        // play command without a view, this case
                        // happens when music player is started in starter
                        // and has not been activated yet
                        // if music player gets this event, that means embedded mode
                        // is not active. safe to just use TApaTask to switch task
                        // bring music player to foreground in this case
                        TApaTaskList tasList( CEikonEnv::Static()->WsSession() );
                        TApaTask task =
                             tasList.FindApp( KAppUidMusicPlayerX );
                        if ( task.Exists() )
                            {
                            task.BringToForeground();
                            }
                        }
                    else
                        {
                        MPX_DEBUG2( "CMPXAppUi::HandleMediaKeyCommand Menu displaying/not foreground, inactive state, command %d ignored", aCommand );
                        }
                    }
                }
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Additional handling of media key commands.
// Determine if the command will be passed to the playback engine.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::DoHandleMediaKeyCommandL(
    TMPXPlaybackCommand aCommand )
    {
    MPX_FUNC( "CMPXAppUi::DoHandleMediaKeyCommandL" );

    TBool handleCommand(EFalse);
    TMPXPlaybackState playerState( iPlaybackUtility->StateL() );

    MMPXSource* source( iPlaybackUtility->Source() );
    switch ( aCommand )
        {
        case EPbCmdPlay:
        case EPbCmdPause:
        case EPbCmdPlayPause:
            {
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
            if ( source || iPdParameterHandler )
#else
            if ( source )
#endif
                {
                handleCommand = ETrue;
                }
            break;
            }
        case EPbCmdStop:
            {
            handleCommand = ETrue;
            break;
            }
        case EPbCmdNext:
            {
            // TODO: Invalid if in progressive download mode??

            // ignore next key in stopped mode, except when
            // the foreground is playbackview
            if (( EPbStateStopped != playerState ||
                  KMPXPluginTypePlaybackUid == iCurrentViewType.iUid ||
                  KMPXPluginTypeEmbeddedPlaybackUid == iCurrentViewType.iUid  ) &&
                source )
                {
                handleCommand = ETrue;
                }
            break;
            }
        case EPbCmdPrevious:
            {
            // TODO: Invalid if in progressive download mode??

            // ignore previous key in stopped mode, except when
            // the foreground is playbackview 
            if (( EPbStateStopped != playerState ||
                  KMPXPluginTypePlaybackUid == iCurrentViewType.iUid ||
                  KMPXPluginTypeEmbeddedPlaybackUid == iCurrentViewType.iUid) &&
                source )
                {
                handleCommand = ETrue;
                }
            break;
            }
        case EPbCmdStartSeekForward:
            {
            // TODO: Invalid if in progressive download mode??
            if (( EPbStatePlaying == playerState ||
                  EPbStatePaused == playerState ||
                  EPbStateSeekingBackward == playerState ) &&
                source )
                {
                handleCommand = ETrue;
                }
            break;
            }
        case EPbCmdStartSeekBackward:
            {
            // TODO: Invalid if in progressive download mode??
            if (( EPbStatePlaying == playerState ||
                  EPbStatePaused == playerState ||
                  EPbStateSeekingForward == playerState  ) &&
                source )
                {
                handleCommand = ETrue;
                }
            break;
            }
        case EPbCmdStopSeeking:
            {
            if (( EPbStateSeekingForward == playerState ||
                  EPbStateSeekingBackward == playerState ) &&
                source )
                {
                handleCommand = ETrue;
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if ( handleCommand )
        {
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD

        if (iPdParameterHandler)
            {
            iPdParameterHandler->HandleCommandL( aCommand );
            }
        else
            {
            iPlaybackUtility->CommandL( aCommand );
            }
#else
        iPlaybackUtility->CommandL( aCommand );
#endif
        // Control popup behaviour
        HandlePopupL( aCommand );
        }
    }

// -----------------------------------------------------------------------------
// Control popup behaviour.
// -----------------------------------------------------------------------------
//
void CMPXAppUi::HandlePopupL(
    TMPXPlaybackCommand aCommand )
    {
    MPX_FUNC( "CMPXAppUi::HandlePopupL" );
    TBool showPlaybackPopup( ETrue );
    TBool showVolumePopup( ETrue );

    if ( IsForeground() )
        {
        switch ( iCurrentViewType.iUid )
            {
            case KMPXPluginTypePlaybackUid:
            case KMPXPluginTypeEmbeddedPlaybackUid:
            case KMPXPluginTypeMainUid:
                {
                // do not show playback popup in these views
                MPX_DEBUG2( "CMPXAppUi::HandlePopupL active view type is 0x%x, not showing popup", iCurrentViewType.iUid );
                showPlaybackPopup = EFalse;
                break;
                }
            default:
                {
                MPX_DEBUG2( "CMPXAppUi::HandlePopupL active view type is 0x%x, showing popup", iCurrentViewType.iUid );
                break;
                }
            }
        }
    else if ( !IsForeground() && IsActiveIdleEnabled() && IsIdleAppForeground() )
        {
        showPlaybackPopup = EFalse;
        }

    if ( !IsForeground())
        {
        if ( IsActiveIdleEnabled() && IsIdleAppForeground() )
            {
            showVolumePopup = ETrue;
            }
        else
            {
            showVolumePopup = EFalse;
            }
        }

    switch ( aCommand )
        {
        case EPbCmdStop:
            {
            iMediaKeyHandler->DismissNotifier( EDisplayMediaPopup );
            break;
            }
        case EPbCmdPrevious:
        case EPbCmdNext:
            {
            if ( showPlaybackPopup )
                {
                // skip backwards should start timer right away
                // because it'll not receive events to trigger the timer
                // if skipping multiple songs, the timer will be reset
                // everytime it reaches a song

                // skip fowards should also start timer right away
                // becuase if it's the last song, there won't be any events
                // to trigger the timer
                iMediaKeyHandler->ShowPlaybackPopupL( EMPXPopupTimeout );
                }
            break;
            }
        case EPbCmdPlay:
        case EPbCmdPause:
        case EPbCmdPlayPause:
        case EPbCmdStartSeekForward:
        case EPbCmdStartSeekBackward:
            {
            if ( showPlaybackPopup )
                {
                iMediaKeyHandler->ShowPlaybackPopupL( EMPXPopupNoTimeout );
                }
            break;
            }
        case EPbCmdStopSeeking:
            {
            if ( showPlaybackPopup )
                {
                iMediaKeyHandler->ShowPlaybackPopupL( EMPXPopupTimeout );
                }
            break;
            }
        case EPbCmdSetVolume:
        case EPbCmdMuteVolume:
        case EPbCmdUnMuteVolume:
            {
            if ( showVolumePopup && iMediaKeyHandler)
                {
                // display pop up volume control
                iMediaKeyHandler->ShowVolumePopupL();
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// Check if Active Idle app is on foreground
// -----------------------------------------------------------------------------
//
TBool CMPXAppUi::IsIdleAppForeground()
    {
    MPX_FUNC( "CMPXAppUi::IsIdleAppForeground" );
    TBool isIdleActive( EFalse );
    // Gets the window group id of the app in foreground
    RWsSession& wsSession = CCoeEnv::Static()->WsSession();
    TInt windowGroupId = wsSession.GetFocusWindowGroup();
    if ( windowGroupId >= 0 )
        {
        TRAP_IGNORE(
            {
            CApaWindowGroupName* wgName = CApaWindowGroupName::NewL(
                wsSession, windowGroupId );

            isIdleActive = ( wgName->AppUid() == KMPXUidIdleApp );
            delete wgName;
            } );
        }

    return isIdleActive;
    }

// -----------------------------------------------------------------------------
// Check if Active Idle is enabled
// -----------------------------------------------------------------------------
//
TBool CMPXAppUi::IsActiveIdleEnabled()
    {
    MPX_FUNC( "CMPXAppUi::IsActiveIdleEnabled" );
    TBool res( EFalse );

#ifdef __ACTIVE_IDLE
    if (! FeatureManager::FeatureSupported( KFeatureIdActiveIdle ) )
        return EFalse;
#endif
    if ( !iSettingsRepository )
        {
        TRAP_IGNORE(
            {
            iSettingsRepository = CRepository::NewL(
                KCRUidPersonalizationSettings );
            } );
        }

    if ( iSettingsRepository )
        {
        TInt value( 0 );
        TInt err( iSettingsRepository->Get( KSettingsActiveIdleState, value ) );
        if ( !err && value )
            {
            res = ETrue;
            }
        }
    return res;
    }

// ---------------------------------------------------------------------------
// Creates and initializes all member variables
// ---------------------------------------------------------------------------
//
void CMPXAppUi::CreateMemberVariablesL()
    {
    MPX_FUNC("CMPXAppUi::CreateMemberVariablesL()");

    TUid playbackMode( KAppUidMusicPlayerX );
    TUid collectionMode( KAppUidMusicPlayerX );
    if ( IsEmbedded() )
        {
        playbackMode = KPbModeNewPlayer;
        collectionMode = MPXTlsHelper::HostUid(); // Collection context == host UID
        }

    if ( !iCollectionUtility )
        {
        iCollectionUtility = MMPXCollectionUtility::NewL( this, collectionMode );
        }

    // Get an instance of view utility
    if ( !iViewUtility )
        {
        iViewUtility = MMPXViewUtility::UtilityL();
        iViewUtility->AddObserverL( this );
        iViewUtility->ConstructDefaultViewHistoryL();
        }

    if ( !iCollectionUiHelper )
        {
        iCollectionUiHelper = CMPXCollectionHelperFactory:: NewCollectionUiHelperL();
        }

    // Create a new playback utility instance
    if ( !iPlaybackUtility )
        {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( playbackMode );
        iPlaybackUtility->AddObserverL( *this );

        // get the current Volume and Max Volume
        iPlaybackUtility->PropertyL(*this, EPbPropertyVolume);
        iPlaybackUtility->PropertyL(*this, EPbPropertyMaxVolume);
        if( !IsEmbedded() )
            {
            TInt randomMode = 0;
            TInt repeatMode = 0;
            
            CRepository* repository = CRepository::NewL( KCRUidMPXMPSettings );
            repository->Get( KMPXMPPlaybackRandomMode, randomMode );
            repository->Get( KMPXMPPlaybackRepeatMode, repeatMode );
            delete repository;
            repository = NULL;
            
            MPX_DEBUG3("CMPXAppUi::CreateMemberVariablesL() randomMode(%d), repeatMode(%d)", randomMode, repeatMode);
            
            iPlaybackUtility->SetL( EPbPropertyRandomMode, randomMode );
            iPlaybackUtility->SetL( EPbPropertyRepeatMode, repeatMode );
            }
        }

    if ( !iMediaKeyHandler )
        {
        // Handle media key in appui
        iMediaKeyHandler = MMPXMediaKeyHandler::NewL(
            EDisplayVolumePopup | EDisplayMediaPopup,
            this );
        }

    if( !IsEmbedded() )
        {
        iPathRestore = CMPXRestorePath::NewL( iPlaybackUtility,
                                              iCollectionUiHelper );

        // Fetch the music collection UID
        RArray<TUid> uid;
        CleanupClosePushL( uid );
        uid.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
        iMusicCollectionId = iCollectionUtility->CollectionIDL( uid.Array() );

        // Fetch the podcast collection UID
        if( !iDisablePodcasting )
            {
            uid.Reset();
            uid.AppendL( TUid::Uid(EMPXCollectionPluginPodCast) );
            iPodcastCollectionId = iCollectionUtility->CollectionIDL( uid.Array() );;
            }

        CleanupStack::PopAndDestroy( &uid );
        }
#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT 
    // In case Camese Superdistribution is enabled,
    // we just need to initialize the Camese Drm Handler
    // object. It will act as a collection utility
    // observer and will handle WMDRM protected content
    // in lieu of the AppUi.
    if ( !iCameseDrmHandler )
        {
        iCameseDrmHandler = CCameseDrmHandler::NewL(iFile);
        }
#endif

#ifdef RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    if( !iDocHandler )
        {
        iDocHandler = CDocumentHandler::NewL(iEikonEnv->Process());
        }
#endif //RD_RA_SUPPORT_FOR_MUSIC_PLAYER
    }

// ---------------------------------------------------------------------------
// Save file
// ---------------------------------------------------------------------------
//
void CMPXAppUi::DoSaveL()
    {
    MPX_DEBUG1("CMPXAppUi::DoSaveL(): entering");
    ASSERT( iFile.SubSessionHandle() );

    // Get target path
    TFileName newFile;
    CreateAutomaticSavePathL( newFile );
    delete iSavePath;
    iSavePath = NULL;
    iSavePath = newFile.AllocL();

    // display waiting note
    TParsePtrC fileParse( newFile );
    HBufC* string( StringLoader::LoadLC( R_MPX_SAVE_ONE_ITEM_WAIT_NOTE,
                                         fileParse.Name() ));
// Cover UI start
//#ifdef __COVER_DISPLAY
        if ( iCommandInitiator )
        {
        HBufC8* filename8 = HBufC8::NewLC( KMaxFileName );
        filename8->Des().Copy( fileParse.Name() );
        TPtrC name(KNullDesC);
        name.Set( fileParse.Name() );
        HBufC8* data = HBufC8::NewLC(KMaxFileName);

        TPtr8 dataPtr = data->Des();
        RDesWriteStream str(dataPtr);

        TPtrC8 b(KNullDesC8);
        b.Set(*filename8);

        str << b;
        str.CommitL();

        iCommandInitiator->IssueCommand( KMediatorSecondaryDisplayDomain,
                                        KMPlayerNoteCategory,
                                        EMPlayerNoteSaveOneItem,
                                        TVersion (0,0,0),
                                        *data );
        CleanupStack::PopAndDestroy(2);
        }
//#endif // __COVER_DISPLAY
// Cover UI end

    iCommonUiHelper->ShowWaitNoteL(
        *string,
        R_AVKON_SOFTKEYS_EMPTY,
        EFalse,
        this );
    CleanupStack::PopAndDestroy( string );

    TMPXLaunchMode mode( MPXTlsHelper::LaunchMode() );
    iPausedForSave = EFalse;
    if ( EMPXLaunchModeTrack == mode )
        {
        // Close the file in playback engine
        iPlaybackUtility->CommandL( EPbCmdPreservePosition );
        iPlaybackUtility->CommandL( EPbCmdPreserveState );
        iPlaybackUtility->CommandL( EPbCmdClose );
        }
    else
        {
        if ( EPbStatePlaying == iPlaybackUtility->StateL() )
            {
            iPlaybackUtility->CommandL( EPbCmdPause );
            iPausedForSave = ETrue;
            }
        }

    // Check disk space
    TBool allowMove( MPXTlsHelper::AllowMove() );
#ifdef RD_MULTIPLE_DRIVE
    // Only need to check if on phone memory. All other drives were
    // checked while creating the automatic save path
    if( fileParse.Drive().CompareF( PathInfo::PhoneMemoryRootPath().Left(2) ) == 0 )
#else
    // Do not need to check if on memory card as that was already done
    if( fileParse.Drive().CompareF( PathInfo::MemoryCardRootPath().Left(2) ) != 0 )
#endif // RD_MULTIPLE_DRIVE
        {
        // If a file is being moved then there is no need to check for
        // available space, as moving the file doesn't increase space consumption
        // Check is still needed if a file is being copied
        RFs& fs( iEikonEnv->FsSession() );
        TInt size( 0 );
        iFile.Size( size );
        if ( !allowMove )
            {
            if ( SysUtil::FFSSpaceBelowCriticalLevelL( &fs, size ) )
                {
                iCommonUiHelper->DismissWaitNoteL();
                User::Leave( KErrDiskFull ); // Phone memory full, Avkon shows note
                }
            }
        }

    // Create save helper if not created
    if ( !iSaveHelper )
        {
        iSaveHelper = CMPXSaveHelper::NewL( this, this );
        }

    // s60 change. playlist could be opened from messaging app.
    // in the case we don't want to copy in async mode
    if (( EMPXLaunchModeTrack != MPXTlsHelper::LaunchMode() )
        && (!iCommonUiHelper->IsHostMessagingL()))
        {
        iSaveHelper->StartCopyOperationL( iFile, newFile, allowMove );
        iFile.Close();
        }
    else
        {
        if ( iCommonUiHelper->IsHostMessagingL() )
            {
            // save ringtone. We need to do sync call to save file because
            // the asyc calls doesn't guarantee the file is copied completely.
            // This is deal with a situation when set ringtone from messaging app
            // that the ringtone files are kept in proviate directory.
            iSaveHelper->StartCopyOperationL( iFile, newFile, EFalse, ETrue );
            }
        else
            {
            iSaveHelper->StartCopyOperationL( iFile, newFile, allowMove );
            iFile.Close();
            }
        }

    MPX_DEBUG1("CMPXAppUi::DoSaveL(): exiting");
    }

// -----------------------------------------------------------------------------
// CreateAutomaticSavePathL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::CreateAutomaticSavePathL( TFileName& aNewPath )
    {
    MPX_DEBUG1("CMPXAppUi::CreateAutomaticSavePathL(): entering");

    RFs& fs( iEikonEnv->FsSession() );
    TFileName fileName;
    iFile.FullName( fileName );
    TParsePtrC fileParse( fileName );

    // Get the size and check if there's enough room to store
    TInt size( 0 );
    User::LeaveIfError( iFile.Size( size ));

#ifdef RD_MULTIPLE_DRIVE
    // Get the next available drive (internal mass store, MMC or phone memory)
    HBufC* defaultRootPath( iCommonUiHelper->AvailableDriveLC( size ) );
    aNewPath = *defaultRootPath;
    CleanupStack::PopAndDestroy( defaultRootPath );
#else
    HBufC* defaultRootPath( iCommonUiHelper->DefaultDriveLC() );
    aNewPath = *defaultRootPath;
    CleanupStack::PopAndDestroy( defaultRootPath );
    if ( aNewPath.CompareF( PathInfo::MemoryCardRootPath()) == 0 )
        {
        // only if memory card is supported, check if there is enough space
        // on the card.
        TBool isBelowCritical( EFalse );
        TRAPD( err, isBelowCritical =
                    SysUtil::MMCSpaceBelowCriticalLevelL( &fs, size ));
        if ( err || isBelowCritical )
            {
            aNewPath = PathInfo::PhoneMemoryRootPath();
            }
        }
#endif // RD_MULTIPLE_DRIVE

    TInt pathResId( KErrNotFound );

    // Get MIME type to see if it's a ringtone
    RApaLsSession apa;
    User::LeaveIfError( apa.Connect() );
    CleanupClosePushL( apa );
    TDataRecognitionResult result;
    User::LeaveIfError( apa.RecognizeData( iFile, result ));
    CleanupStack::PopAndDestroy( &apa );
    TPtrC resultPtr( result.iDataType.Des() );
    TInt val( 0 );
    if ( iRingToneMimeTypes.Find( resultPtr, val ) == 0 )
        {
        pathResId = R_MPX_TARGETFOLDER_RINGTONES;
        }
    else if ( iCommonUiHelper->IsHostMessagingL() )
        {
        pathResId = R_MPX_TARGETFOLDER_ATTACHMENT;
        }
    else
        {
        pathResId = R_MPX_TARGETFOLDER_DOWNLOADS;
        }

    HBufC* defaultPath( StringLoader::LoadLC( pathResId ));
    aNewPath += *defaultPath;
    aNewPath += fileParse.NameAndExt();
    CleanupStack::PopAndDestroy( defaultPath );

    if ( !ConeUtils::FileExists( aNewPath ) )
        {
        User::LeaveIfError(
            Application()->GenerateFileName( fs, aNewPath ));
        }
    else if ( iCommonUiHelper->IsHostMessagingL() )
        {
        MPXTlsHelper::SetNeedSave( EFalse );
        }

    MPX_DEBUG1("CMPXAppUi::CreateAutomaticSavePathL(): exiting");
    }

// -----------------------------------------------------------------------------
// HandlePostSaveOperationL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::HandlePostSaveOperationL()
    {
    MPX_DEBUG1("CMPXAppUi::HandlePostSaveOperationL(): entering");

    if ( iSaveMode == EMPXExitAppAfterSave )
        {
        MPX_DEBUG1( "CMPXAppUi::HandlePostSaveOperationL() exit after save" );
        iSaveMode = EMPXSaveModeIdle;
        iFile.Close();
        RunAppShutter();
        }
    else if ( iSaveMode == EMPXExitHostAppAfterSave )
        {
        MPX_DEBUG1( "CMPXAppUi::HandlePostSaveOperationL() exit host app after save" );
        iSaveMode = EMPXSaveModeIdle;
        iFile.Close();
        CAknEnv::RunAppShutter();
        }
    else
        {
        MPX_DEBUG1( "CMPXAppUi::HandlePostSaveOperationL() reopen" );
        if ( EMPXLaunchModeTrack == MPXTlsHelper::LaunchMode() )
            {
            if ( iSavePath )
                {
                iCollectionUiHelper->OpenL( MPXTlsHelper::HostUid(),
                                            *iSavePath,
                                            this,
                                            EMPXCollectionPluginMusic );
                delete iSavePath;
                iSavePath = NULL;
                // User may set ringtone or contact more than once so don't close it after save.
                // The iFile.Close will be called at destructor. It is safe because we are
                // in Embedded mode.
                if ( EMPXLaunchModeTrack != MPXTlsHelper::LaunchMode() )
                    {
                    iFile.Close();
                    }
                }
            else
                {
                if ( iFile.SubSessionHandle() )
                    {
                    TFileName filename;
                    iFile.FullName( filename );
                    iCollectionUiHelper->OpenL( MPXTlsHelper::HostUid(),
                                                filename,
                                                this,
                                                EMPXCollectionPluginMusic );
                    }
                }
            }
        else
            {
            if ( iPausedForSave )
                {
                iPlaybackUtility->CommandL( EPbCmdPlay );
                }
            }
        iSaveMode = EMPXSaveModeIdle;
        }

    MPX_DEBUG1("CMPXAppUi::HandlePostSaveOperationL(): exiting");
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    TBool skip = EFalse;

#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT       
    if (aMessage)
        {            
        TMPXMessageId* id( aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ) );
        if ( id )
            {
            if ( KMPXMessageGeneral == *id )
                {
                // Prefilter the playback event before passing in to 
                // AppUi handler.        
                MPX_TRAP( aError, HandleCamesePlaybackPreProcessingL(*aMessage, skip) );
                }
            }
        }
#endif  

    if ( !skip && aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle media properties.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError )
    {
    MPX_FUNC( "CMPXAppUi::HandleCollectionMediaL" );
    TBool okToOpen( ETrue );

    const CMPXMedia* media( &aMedia );

    // Check if rights OK
    if ( !aError )
        {
        // First make sure it's a DRM file and the file protection type
        if ( media->IsSupported( KMPXMediaDrmProtected ) &&
                media->IsSupported( KMPXMediaDrmType ) )
            {
            TBool prot(
                    media->ValueTObjectL<TBool>( KMPXMediaDrmProtected ) );
            if ( prot )
                {
#ifdef CAMESE_IN_DRM_UTILITY
                // OMA and WMDRM cases will be handled with same logic now
                okToOpen = HandleDRMProtectionL(*media);
#else
                TMPXMediaDrmType type( 
                        media->ValueTObjectL<TMPXMediaDrmType>( KMPXMediaDrmType ) );
                if ( EMPXDrmTypeWMA == type )
                    {
                    okToOpen = HandleWMAProtectionL();
                    }

                else
                    {
                    okToOpen = HandleDRMProtectionL(*media);
                    }
#endif                        
                }

            }
        }
    else
        {
        okToOpen = EFalse;
#ifndef __CAMESE_SUPERDISTRIBUTION_SUPPORT
        // Leave Error Handling for Camese Ui
        // if Camese support is enabled
        iCommonUiHelper->HandleErrorL( aError );
#endif
        }

    // If all checking passed then open
    if ( okToOpen )
        {
        iCollectionUtility->Collection().OpenL();
        }
    else if ( MPXTlsHelper::NeedSave() )
        {
        MPX_DEBUG1( "CMPXAppUi::HandleCollectionMediaL open failed, attempting to save" );
        HandleCommandL( EMPXCmdSaveAndExit );
        }
    }

    // ---------------------------------------------------------------------------
    // CMPXAppUi::HandleWMAProtectionL
    // ---------------------------------------------------------------------------
    //
TBool CMPXAppUi::HandleWMAProtectionL()
    {
#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT    

    if ( !IsPlaybackRemote() )
        {
        // Indicates we are currently handling WMA Protected Media
        iHandlingProtectedWMAMedia = ETrue;  
        }
#endif
    // Regardless of Camese support enabled or not,
    // we always proceed to starting playback of
    // protected WMA media.      
    return ETrue;       
    }

    // ---------------------------------------------------------------------------
    // CMPXAppUi::HandleDRMProtectionL
    // ---------------------------------------------------------------------------
    //
    TBool CMPXAppUi::HandleDRMProtectionL(const CMPXMedia& aMedia)
        {
        TBool okToOpen( ETrue );

    // DRM Protection
    // Now check the rights status and if it's about to expire.
    if ( aMedia.IsSupported( KMPXMediaDrmRightsStatus ) &&
         aMedia.IsSupported( KMPXMediaDrmAboutToExpire ) )
        {
        TMPXMediaDrmRightsStatus status(
            aMedia.ValueTObjectL<TMPXMediaDrmRightsStatus>(
                KMPXMediaDrmRightsStatus ) );
        TBool aboutToExpire(
            aMedia.ValueTObjectL<TBool>( KMPXMediaDrmAboutToExpire) );

        // Preview clips cannot be saved
        if ( EMPXDrmRightsPreview == status )
            {
            MPXTlsHelper::SetNeedSave( EFalse );
            }

        RFile* file( NULL );
        if ( iFile.SubSessionHandle() &&
             MPXTlsHelper::LaunchMode() != EMPXLaunchModePlaylist )
            {
            file = &iFile;
            }

        // Display error message if about to expire, or if
        // there are no rights
        if ( aboutToExpire )
            {
            TInt ret( iCommonUiHelper->HandleErrorL(
                        KMPXRightsAboutToExpire, &aMedia, file ));
            if ( KErrNone != ret )
                {
                okToOpen = EFalse;
                }
            }
        else if ( EMPXDrmRightsMissing == status ||
                EMPXDrmRightsExpired == status )
            {
            // DRMStream:
            // for silent cases the rights may have been recieved
            TInt ret( iCommonUiHelper->HandleErrorL( KErrCANoRights, &aMedia, file ));
            if ( KErrNone != ret )
                {
                okToOpen = EFalse;
                }
            }

        if ( !okToOpen )
            {
            // If embedded and no rights, then close
            if ( IsEmbedded() )
                {
                // Only close embedded mode if not a playlist
                if ( MPXTlsHelper::LaunchMode() != EMPXLaunchModePlaylist )
                    {
                    RunAppShutter();
                    }
                else
                    {
                    // Reset collection path and save index
                    MMPXCollection& col( iCollectionUtility->Collection() );
                    CMPXCollectionPath* cPath( col.PathL() );
                    CleanupStack::PushL( cPath );
                    iCurrentIndex = cPath->Index();
                    cPath->Back();
                    col.OpenL( *cPath );
                    CleanupStack::PopAndDestroy( cPath );
                    }
                }

            // If standalone and no rights, mark item as invalid
            // and reset collection path
            else
                {
                // Check if DRM Flag is already mark as invalid, only set if it's not.
                MMPXCollection& col( iCollectionUtility->Collection() );
                TBool setDrm( ETrue );
                TBool clearOtherFlags( EFalse );
                TUint flag( 0 );

                if ( aMedia.IsSupported( KMPXMediaGeneralFlags ))
                    {
                    flag = aMedia.ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
                    if ( flag & KMPXMediaGeneralFlagsIsDrmLicenceInvalid )
                        {
                        setDrm = EFalse;
                        }
                    if ( flag & KMPXMediaGeneralFlagsIsCorrupted ||
                         flag & KMPXMediaGeneralFlagsIsInvalid )
                        {
                        clearOtherFlags = ETrue;
                        }
                    }
                if ( setDrm )
                    {
                    CMPXMedia* entry( CMPXMedia::NewL( aMedia ));
                    CleanupStack::PushL( entry );

                    flag = KMPXMediaGeneralFlagsSetOrUnsetBit |
                           KMPXMediaGeneralFlagsIsDrmLicenceInvalid;
                    entry->SetTObjectValueL( KMPXMediaGeneralFlags, flag );
                    SetMediaL( *entry );
                    CleanupStack::PopAndDestroy( entry );
                    }

                // Reset corrupted and invalid flags if they are set
                if ( clearOtherFlags )
                    {
                    CMPXMedia* entry( CMPXMedia::NewL( aMedia ));
                    CleanupStack::PushL( entry );

                    flag = KMPXMediaGeneralFlagsIsCorrupted |
                           KMPXMediaGeneralFlagsIsInvalid;
                    entry->SetTObjectValueL( KMPXMediaGeneralFlags, flag );
                    SetMediaL( *entry );
                    CleanupStack::PopAndDestroy( entry );
                    }

                // Reset collection path and save index
                CMPXCollectionPath* cPath( col.PathL() );
                CleanupStack::PushL( cPath );
                iCurrentIndex = cPath->Index();
                cPath->Back();
                col.OpenL( *cPath );
                CleanupStack::PopAndDestroy( cPath );
                }
            }
        }
    else
        {
        // If rights status and about to expire attributes are
        // not set, retrieve them.
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        attrs.Append( TMPXAttribute( KMPXMediaIdGeneral,
                                     EMPXMediaGeneralBasic |
                                     EMPXMediaGeneralUri |
                                     EMPXMediaGeneralFlags |
                                     EMPXMediaGeneralCollectionId |
                                     EMPXMediaGeneralId ));
        attrs.Append( KMPXMediaDrmAll );
        CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( cPath );
        iCollectionUtility->Collection().MediaL(
            *cPath,
            attrs.Array() );
        CleanupStack::PopAndDestroy( cPath );
        CleanupStack::PopAndDestroy( &attrs );
        okToOpen = EFalse;
        }

    return okToOpen;
    }

// ---------------------------------------------------------------------------
// Handle Broadcast messages from the collection
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleBroadcastMessageL( const CMPXMessage& aMessage )
    {
    // Disable media keys during system operations.
    TInt op( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
    MPX_DEBUG2( "CMPXAppUi::HandleBroadcastMessageL EBroadcastEvent, op = %d", op );
    if( op == EMcMsgUSBMassStorageStart ||
        op == EMcMsgUSBMTPStart ||
        op == EMcMsgRefreshStart ||
        op == EMcMsgFormatStart )
        {
        MPX_DEBUG1( "CMPXAppUi::HandleBroadcastMessageL EBroadcastEvent disable media key" );
        if ( !IsEmbedded() || op != EMcMsgUSBMTPStart )
            {
            iMediaKeyHandler->SetEnableMediaKeys( EFalse );
            }
        }
    else if( op == EMcMsgUSBMassStorageEnd ||
             op == EMcMsgUSBMTPEnd ||
             op == EMcMsgRefreshEnd ||
             op == EMcMsgFormatEnd )
        {
        MPX_DEBUG1( "CMPXAppUi::HandleBroadcastMessageL EBroadcastEvent enable media key" );
        if ( !IsEmbedded() || op != EMcMsgUSBMTPEnd )
            {
            iMediaKeyHandler->SetEnableMediaKeys( ETrue );
            }
        }
    if (op == EMcMsgRefreshEnd)
        {
        TInt err = aMessage.ValueTObjectL<TInt>(KMPXMessageGeneralData);

        MPX_DEBUG2("EMcMsgRefreshEnd, err = %d", err);

        if (err < KErrNone)
            iCommonUiHelper->HandleErrorL(err);
        }

    // Re-open the path so that the view can be refreshed,
    // because there could be changes in the view contents
    //
    if ( op == EMcMsgUSBMassStorageStart ||
         op == EMcMsgUSBMTPStart )
        {
        iIsUsbOnGoing = ETrue;
        if ( iProcessParameter )
            {
            iProcessParameter->SetUsbInUse( ETrue );
            }
        }
    if ( op == EMcMsgUSBMassStorageEnd ||
        op == EMcMsgUSBMTPEnd )
        {
        iIsUsbOnGoing = EFalse;
        if ( iProcessParameter )
            {
            iProcessParameter->SetUsbInUse( EFalse );
            }
        if( op == EMcMsgUSBMTPEnd )
            {
            DoUpdatePluginRefreshedTimeL();
            }
        }
    if( op == EMcMsgFormatStart )
        {
        iFormattingOnGoing = ETrue;
        }
    else if( op == EMcMsgFormatEnd )
        {
        iFormattingOnGoing = EFalse;
        }
    else if ( op == EMcMsgRefreshStart )
        {
        iRefreshOnGoing = ETrue;
        if( iProcessParameter )
            {
            iProcessParameter->SetRefreshState( ETrue );
            }
        }
    else if ( op == EMcMsgRefreshEnd )
        {
        iRefreshOnGoing = EFalse;
        if( iProcessParameter )
            {
            iProcessParameter->SetRefreshState( EFalse );
            }
        }

    if ( op == EMcMsgUSBMTPEnd )
        {
            if ( !IsEmbedded() )
                {
                CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL ( );
                CleanupStack::PushL ( cPath );
                TBool usbDiskRemovalEvent(EFalse);
                if ( (op == EMcMsgDiskRemoved) && iIsUsbOnGoing )
                    {
                    usbDiskRemovalEvent = ETrue;
                    }

                if ( cPath->Levels ( )> 1 && !iFormattingOnGoing && !usbDiskRemovalEvent &&
                     iCurrentViewType.iUid != KMPXPluginTypePlaybackUid &&
                     iCurrentViewType.iUid != KMPXPluginTypeAudioEffectsUid )
                    {

                        cPath->Back ( );
                        iCollectionUtility->Collection().OpenL ( *cPath );
                        
                    }
                CleanupStack::PopAndDestroy ( cPath );
                }

        }
    else if ( op == EMcMsgDiskRemoved )
        {
        TBool usbDiskRemovalEvent( EFalse );
        if ( (op == EMcMsgDiskRemoved) && iIsUsbOnGoing )
            {
            usbDiskRemovalEvent = ETrue;
            }
        else
            {
            MPX_DEBUG1( "CMPXAppUi::HandleBroadcastMessageL. MMC ejected and the application needs to close." );
            RunAppShutter();
            return;
            }

            CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL ( );
            CleanupStack::PushL ( cPath );
            if ( cPath->Levels ( )> 1 && !iFormattingOnGoing && !usbDiskRemovalEvent &&
                 iCurrentViewType.iUid != KMPXPluginTypePlaybackUid )
                {
            cPath->Back();
            iCollectionUtility->Collection().OpenL( *cPath );

            }
        CleanupStack::PopAndDestroy( cPath );
        }
    // Re-open the collection view after a refresh
    // and after album art or metadata dialog or add songs editor dialog closes
    if (((op == EMcMsgRefreshEnd ) || (op == EMcMsgUSBMassStorageEnd))
            && (IsActiveView(KMPXPluginTypeAlbumArtEditorUid)
                    || IsActiveView(KMPXPluginTypeMetadataEditorUid)
                    || IsActiveView( KMPXPluginTypeAddSongsEditorUid ) ) )
        {
        if ( !IsEmbedded() )
            {
            iRefreshCollectionView = ETrue;
            }
        }
    // Re-open the collection view after a refresh
    // if we were currently in collection view.
    if( op == EMcMsgRefreshEnd && IsActiveView(KMPXPluginTypeCollectionUid))
        {
        if( !IsEmbedded() )
            {
            iRefreshCollectionView = EFalse;
            CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( path );
            if( path->Levels() > 1 &&
                iCurrentViewType.iUid != KMPXPluginTypePlaybackUid )
                {
                path->Back();
                iCollectionUtility->Collection().OpenL( *path );
                }
            CleanupStack::PopAndDestroy( path );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handles commands passed in by other applications
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleCommandParametersL( const TDesC8& aCommand )
    {
    MPX_FUNC( "CMPXAppUi::HandleCommandParametersL" );
    if ( !iIgnoreExternalCommand && aCommand.Size() > 0 )
        {
        delete iProcessParameter;
        iProcessParameter = NULL;
        TUid collectionMode( KAppUidMusicPlayerX );
        if ( IsEmbedded() )
            {
            collectionMode = MPXTlsHelper::HostUid(); // Collection context == host UID
            }
        iProcessParameter = CMPXCommandHandler::NewL(
            iPlaybackUtility, collectionMode, aCommand );

        TBool fwdStandAlone( iProcessParameter->ForwardCmdToStandAloneMode() );
        TBool fwdNowPlaying( iProcessParameter->ForwardCmdToNowPlaying() );

        MPX_DEBUG2( "CMPXAppUi::HandleCommandParametersL iIsUsbOnGoing = %d", iIsUsbOnGoing );
        // if ActiveIdle plugin sends a command i.e., EMPXCmdFwdNowPlaying, it doesn't make any dialogs dismiss.
        if ( !fwdNowPlaying && iCurrentViewType.iUid != KMPXPluginTypeWaitNoteDialogUid && !iIsUsbOnGoing )
            {
            MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL dismissing dialogs" );
            // dismiss all the dialogs except scaning wait note
            AknDialogShutter::ShutDialogsL( *CEikonEnv::Static() );
            }

        iCmdActivatedViewTypeId = iProcessParameter->ActivatedViewTypeId();
        MPX_DEBUG2( "CMPXAppUi::HandleCommandParametersL activated view type= =0x%x",
            iCmdActivatedViewTypeId );

        MPX_DEBUG3( "CMPXAppUi::HandleCommandParametersL should forward to standalone %d, now playing %d", fwdStandAlone, fwdNowPlaying );

        RArray<TProcessId> procArray;
        CleanupClosePushL( procArray );
        TProcessId npProcId( 0 );
        MMPXPlaybackUtility* activePbU = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer );
        CleanupClosePushL( *activePbU );
        activePbU->GetClientsL( procArray );
        CleanupStack::PopAndDestroy( activePbU );
        npProcId = procArray[ 0 ];
        CleanupStack::PopAndDestroy( &procArray );

        TInt64 standAlonePId( CMPXCommonUiHelper::StandAloneModePId() );

        RProcess currProc;
        TProcessId currPId( currProc.Id() );
        currProc.Close();

        MPX_DEBUG2( "CMPXAppUi::HandleCommandParametersL current pid 0x%lx", currPId.Id() );
        MPX_DEBUG2( "CMPXAppUi::HandleCommandParametersL nppid 0x%lx", npProcId.Id() );
        MPX_DEBUG2( "CMPXAppUi::HandleCommandParametersL standalone pid 0x%lx", standAlonePId );

        if ( ( !fwdStandAlone && !fwdNowPlaying ) || // no forward
             ( fwdStandAlone && ( currPId.Id() == standAlonePId ) ) || // we're standalone mode
             ( fwdNowPlaying && ( currPId == npProcId ) ) ) // we are now playing
            {
            if ( iPathRestore )
                {
                if ( !iPathRestore->Initialized() &&
                    iProcessParameter->IsInitPlaybackEngineNeeded() )
                    {
                    if ( iPathRestore->IsInitializing() )
                        {
                        MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL command handling delayed until path restore completes" );
                        // delay command processing until path restore is complete
                        iDelayedParameterHandling = ETrue;
                        }
                    else
                        {
                        MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL canceling path restore" );
                        // command will init playback engine, can cancel path restore's init
                        iPathRestore->CancelInit();
                        }
                    }
                }
            if ( !iDelayedParameterHandling )
                {
                // command should be handled locally
                iProcessParameter->ProcessMessageL( iIsUsbOnGoing, iRefreshOnGoing, iFormattingOnGoing );
                }
            }
        else
            {
            TInt64 procId( 0 );
            if ( iProcessParameter->ForwardCmdToStandAloneMode() )
                {
                MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL forwarding to standalone" );
                procId = standAlonePId;
                }
            else if ( iProcessParameter->ForwardCmdToNowPlaying() )
                {
                MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL forwarding to now playing" );
                procId = npProcId.Id();
                }
            delete iProcessParameter;
            iProcessParameter = NULL;

            if ( procId != 0 )
                {
                // standalone music player exist, forward command to it
                RWsSession& wsSession( iCoeEnv->WsSession() );
                TInt wgId( MPXUser::FindWindowGroupIdL( procId, wsSession ) );
                MPX_DEBUG2( "CMPXAppUi::HandleCommandParametersL() forwarding message to wgId 0x%x", wgId );
                wsSession.SendMessageToWindowGroup( wgId, KAppUidMusicPlayerX, aCommand );
                }
            else
                {
                MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL no music player instance, starting a new one" );
                // no music player instance, start a new one
                RApaLsSession ls;
                CleanupClosePushL( ls );
                User::LeaveIfError( ls.Connect() );
                TApaAppInfo appInfo;
                User::LeaveIfError( ls.GetAppInfo( appInfo, KAppUidMusicPlayerX ) );
                CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();
                apaCommandLine->SetExecutableNameL( appInfo.iFullName );
                apaCommandLine->SetTailEndL( aCommand );
                User::LeaveIfError( ls.StartApp( *apaCommandLine ) );
                CleanupStack::PopAndDestroy(); // apaCommandLine
                CleanupStack::PopAndDestroy(); // ls
                }
            }
        }
    else if ( iIgnoreExternalCommand )
        {
        MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL command to be ignored, requested by views" );
        // bring musicplayer to foreground
        // have to use window group to activate, in case some other window
        // has higher priority than us (e.g. playback popup and volume popup)
        RWindowGroup myWin = CEikonEnv::Static()->RootWin();
        myWin.SetOrdinalPosition( 0, ECoeWinPriorityNormal );
        }
    else
        {
        MPX_DEBUG1( "CMPXAppUi::HandleCommandParametersL invalid parameter length, command ignored" );
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXAppUi::DoHandlePlaybackMessageL" );

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EPlayerChanged:
                {
                MPX_DEBUG1( "CMPXAppUi::DoHandlePlaybackMessageL EPlayerChanged" );
                // get the current Volume and Max Volume
                iPlaybackUtility->PropertyL(*this, EPbPropertyVolume);
                iPlaybackUtility->PropertyL(*this, EPbPropertyMaxVolume);
                HandlePlaybackPlayerChangedL();
                break;
                }
            case TMPXPlaybackMessage::EInitializeComplete:
                {
                MPX_DEBUG1( "CMPXAppUi::DoHandlePlaybackMessageL init complete" );
                TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
                if ( ( IsEmbedded() || iPathRestore && iPathRestore->Initialized() ) && data )
                    {
                    TMPXLaunchMode mode = MPXTlsHelper::LaunchMode();
                    if ( EMPXLaunchModeStopped != mode &&
                       EMPXLaunchModeUnknown!= mode )
                        {
                        // Play the track
                        MPX_DEBUG1( "CMPXAppUi::DoHandlePlaybackMessageL send play command" );
                        CMPXCommand* cmd( CMPXCommand::NewL() );
                        CleanupStack::PushL( cmd );
                        cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId,
                                               KMPXCommandIdPlaybackGeneral );
                        cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType,
                                               EPbCmdPlay );
                        cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync,
                                                      ETrue );
                        cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer,
                                                ETrue );
                        iPlaybackUtility->CommandL( *cmd );
                        CleanupStack::PopAndDestroy( cmd );
                        }
                    }
                else // iPathRestore not initialized
                    {
                    if ( iPathRestore )
                        {
                        iPathRestore->SetInitialized( ETrue );
                        }

                    if ( iStandAlone )
                        {
                        iPlaybackUtility->CommandL( EPbCmdPlay );
                        }

                    if ( iDelayedParameterHandling )
                        {
                        MPX_DEBUG1( "CMPXAppUi::DoHandlePlaybackMessageL start command handling" );
                        iDelayedParameterHandling = EFalse;
                        iProcessParameter->ProcessMessageL( iIsUsbOnGoing, iRefreshOnGoing, iFormattingOnGoing );
                        }
                    }

                break;
                }
            case TMPXPlaybackMessage::EPlayerUnavailable:
                {
                MPX_DEBUG1( "CMPXAppUi::DoHandlePlaybackMessageL EPlayerUnavailable" );
                iUPnPUnavailableEventFromCollection = EFalse;

                MMPXSource* source = iPlaybackUtility->Source();
                if ( source )
                    {
                    CMPXCollectionPlaylist* playlist = source->PlaylistL();
                    if ( playlist )
                        {
                        CleanupStack::PushL( playlist );
                        iPlaybackUtility->PlayerManager().ClearSelectPlayersL();
                        MPX_DEBUG2( "CMPXAppUi::DoHandlePlaybackMessageL playlist index = %d", playlist->Index());
                        iPlaybackUtility->InitL( *playlist, ETrue );
                        CleanupStack::PopAndDestroy( playlist );
                        }
                    }

                if ( iCurrentViewType.iUid != KMPXPluginTypePlaybackUid )
                    {
                    iUPnPUnavailableEventFromCollection = ETrue;
                    }

                break;
                }
            case TMPXPlaybackMessage::EPropertyChanged:
                {
                TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
                TMPXPlaybackProperty property(
                    static_cast<TMPXPlaybackProperty>( type ) );
                if ( property == EPbPropertyVolume )
                    {
                    iCurrentVolume = aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData );
                    MPX_DEBUG2( "CMPXAppUi::DoHandlePlaybackMessageL iCurrentVolume = %d", iCurrentVolume );
                    }
                else if( property == EPbPropertyRandomMode 
                        || property == EPbPropertyRepeatMode )
                    {
                    TInt value = aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData );
                    TUint32 key = KMPXMPPlaybackRandomMode;
                    if( property == EPbPropertyRepeatMode )
                        {
                        key = KMPXMPPlaybackRepeatMode;
                        }
                    MPX_DEBUG3( "CMPXAppUi::DoHandlePlaybackMessageL EPropertyChanged key = %d, value = %d", key, value );
                    CRepository* repository = CRepository::NewL( KCRUidMPXMPSettings );
                    repository->Set( key, value );
                    delete repository;
                    repository = NULL;
                    }
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
                TMPXPlaybackState state(
                    static_cast<TMPXPlaybackState>( type ) );

#ifndef FF_OOM_MONITOR2_COMPONENT
                // Obtain wgname
                // Gets the window group id of the app in foreground
                RWsSession& wsSession( iCoeEnv->WsSession() );
                RWindowGroup myWin = iEikonEnv->RootWin();
                CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( wsSession, myWin.Identifier() );
#endif  //FF_OOM_MONITOR2_COMPONENT
                switch( state )
                    {
                    case EPbStateInitialising:
                    case EPbStatePlaying:
                    case EPbStatePaused:
                    case EPbStateSeekingForward:
                    case EPbStateSeekingBackward:
                    case EPbStateBuffering:
                    case EPbStateDownloading:
                        {
#ifdef FF_OOM_MONITOR2_COMPONENT
                        SetMpxOomPriorityL( ROomMonitorSession::EOomPriorityBusy );
#else
                        wgName->SetBusy(ETrue);
#endif //FF_OOM_MONITOR2_COMPONENT
                        break;
                        }
                    case EPbStateNotInitialised:
                    case EPbStateStopped:
                    case EPbStateShuttingDown:
                        {
#ifdef FF_OOM_MONITOR2_COMPONENT
                        SetMpxOomPriorityL( ROomMonitorSession::EOomPriorityNormal );
#else
                        wgName->SetBusy(EFalse);
#endif //FF_OOM_MONITOR2_COMPONENT
                        break;
                        }
                    default:
                        {
                        break;
                        }
                    }
#ifndef FF_OOM_MONITOR2_COMPONENT
                CleanupStack::PopAndDestroy();  //wgName
#endif //FF_OOM_MONITOR2_COMPONENT

                break;
                }
            case TMPXPlaybackMessage::EError:
                {
                MPX_DEBUG2( "CMPXAppUi::DoHandlePlaybackMessageL error = %d", aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

#ifdef __CAMESE_SUPERDISTRIBUTION_SUPPORT   
// ---------------------------------------------------------------------------
// Handle Camese Playback Message Preprocessing
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleCamesePlaybackPreProcessingL(const CMPXMessage& aMessage,                                                   
                                                   TBool& aSkipEvent)
    {
    aSkipEvent = EFalse;
    
    // Pre-filter events, avoiding handling if Camese 
    // is handling WMA songs in pre-init phase.     
    if (iHandlingProtectedWMAMedia)
        {
        TInt event = aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent );
        if (event == TMPXPlaybackMessage::EPlayerChanged)            
            { 
            MPX_DEBUG1("CMPXAppUi::HandleCamesePlaybackPreProcessingL EPlayerChanged");

            // Do not allow processing of the player changed stage
            // to avoid view switching.
            aSkipEvent = ETrue;     
            }
        else if (event == TMPXPlaybackMessage::EInitializeComplete)
            {
            MPX_DEBUG1("CMPXAppUi::HandleCamesePlaybackPreProcessingL EStateChanged EInitializeComplete");
            // Execute view switching now.
            HandlePlaybackPlayerChangedL();  
            // Finished Protected Media Handling stage.
            iHandlingProtectedWMAMedia = EFalse;                                                    
            }
        else if (event == TMPXPlaybackMessage::EError)
            {
            MPX_DEBUG2( "CMPXAppUi::HandleCamesePlaybackPreProcessingL error = %d", aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );

            // Finished Protected Media Handling stage.
            iHandlingProtectedWMAMedia = EFalse;
            }
        }   
    }
#endif

// ---------------------------------------------------------------------------
// Handle playback message for player changed.
// ---------------------------------------------------------------------------
//
 void CMPXAppUi::HandlePlaybackPlayerChangedL()
    {
    // Don't switch views if we are getting player changed
    // event from path restore or when we switch player because of a UPnP
    // Unavailable event in Collection view
    //
    if ( !iUPnPUnavailableEventFromCollection &&
         ( IsEmbedded() || iPathRestore ) )
        {
        if( iPathRestore && !iPathRestore->Initialized() )
            {
            iPathRestore->CancelInit();
            }
        MMPXPlayer* player =
            iPlaybackUtility->PlayerManager().CurrentPlayer();
        TUid pluginUid( KNullUid );
        RArray<TUid> array;
        CleanupClosePushL( array );

        if ( player )
            {
            pluginUid = player->UidL();
            MPX_DEBUG2( "CMPXAppUi::HandlePlaybackPlayerChangedL pluginUid uid = 0x%x", pluginUid.iUid );
            array.AppendL( pluginUid );
            }

        MMPXSource* source = iPlaybackUtility->Source();
        if ( source )
            {
            CMPXCollectionPlaylist* playlist = source->PlaylistL();
            if ( playlist )
                {
                CleanupStack::PushL( playlist );
                CMPXCollectionPath* pbPath =
                    CMPXCollectionPath::NewL( playlist->Path() );
                CleanupStack::PushL( pbPath );
                if ( pbPath->Levels() > 0 )
                    {
                    MPX_DEBUG2( "CMPXAppUi::HandlePlaybackPlayerChangedL collection uid = 0x%x", pbPath->Id( 0 ).iId1 );
                    array.AppendL( TUid::Uid( pbPath->Id( 0 ) ) );
                    }
                CleanupStack::PopAndDestroy( pbPath );
                CleanupStack::PopAndDestroy( playlist );
                }
            }

        if ( IsEmbedded() )
            {
            array.AppendL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ));
            }
        else
            {
            array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ) );
            }
        iViewUtility->ActivateViewL( array );
        CleanupStack::PopAndDestroy( &array );

        MPX_DEBUG2( "-->Plugin Uid:(%d)", pluginUid.iUid );
        }
    iUPnPUnavailableEventFromCollection = EFalse;
    }


// ---------------------------------------------------------------------------
// Callback function to signal that saving a file is complete
// ---------------------------------------------------------------------------
//
void CMPXAppUi::DoHandleSaveCompleteL( TInt aErr )
    {
    MPX_DEBUG2("CMPXAppUi::DoHandleSaveCompleteL(%d) entering", aErr);

    if( iIsUsbOnGoing && (aErr != KErrNone) )
        {
        // Kill mpx embedded mode so usb can start
        //
        MPX_DEBUG1("CMPXAppUi::HandleAddFileCompleteL usb ongoing");
        iSaveMode = EMPXExitAppAfterSave;
        HandlePostSaveOperationL();
        }
    else if ( !iCancelFileMan )
        {
        if ( !aErr )
            {
            if ( !iCollectionHelper )
                {
                iCollectionHelper = CMPXCollectionHelperFactory::NewCollectionHelperL();
                }
            iCollectionHelper->AddL( *iSavePath, this );
            if ( iView )
                {
                if ( iSaveMode == EMPXAssignRingtone )
                    {
                    TRAP_IGNORE(
                        MPXTlsHelper::SetFilePath( iSavePath->Des() );
                        iView->HandleCommandL( EMPXCmdAfterSaveAiwCmdAssign ));
                    MPXTlsHelper::SetNeedSave( EFalse );
                    }
                else if ( iSaveMode == EMPXAssignContact )
                    {
                    TRAP_IGNORE(
                        MPXTlsHelper::SetFilePath( iSavePath->Des() );
                        iView->HandleCommandL( EMPXCmdAfterSaveUseAsCascade ));
                    MPXTlsHelper::SetNeedSave( EFalse );
                    }
                }
            }
        else
            {
            iCommonUiHelper->DismissWaitNoteL();
            iCommonUiHelper->HandleErrorL( aErr );
            delete iSavePath;
            iSavePath = NULL;
            if ( iView )
                {
                iView->HandleCommandL( EMPXCmdSaveCancelled );
                }
            HandlePostSaveOperationL();
            }
        }
    else
        {
        MPX_DEBUG1("CMPXAppUi::DoHandleSaveCompleteL() during cancel, starting callback" );
        iIdle = CIdle::NewL( CActive::EPriorityIdle );
        iSaveMode = EMPXSaveModeIdle;
        iIdle->Start( TCallBack( CMPXAppUi::DeferredExitCallback, this ) );
        }

    MPX_DEBUG1("CMPXAppUi::DoHandleSaveCompleteL() exiting" );
    }

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMPXAppUi::DoHandleCollectionMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXAppUi::DoHandleCollectionMessageL" );

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXCustomMessageId == id )
        {
        // This is a custom collection message
        //
        TInt colId = aMessage.ValueTObjectL<TInt>( TMPXAttribute( KMPXCustomMessageId,
                                                      EMPXCustomMessageCollectionId ) );
        TInt op = aMessage.ValueTObjectL<TInt>( TMPXAttribute( KMPXCustomMessageId,
                                                   EMPXCustomMessageEventType ) );
        TInt err = aMessage.ValueTObjectL<TInt>( TMPXAttribute( KMPXCustomMessageId,
                                                    EMPXCustomMessageErrorCode ) );

        MPX_DEBUG4("CMPXAppUi::DoHandleCollectionMessageL -- KMPXCustomMessageId \
                    %i %i %i", colId, op, err);

        if( colId == iMusicCollectionId.iUid ||
            colId == iPodcastCollectionId.iUid )
            {
            if( op == EMcsOpen )
                {
                // @todo, need UI spec
                // popup special error note for out of disk
                iCommonUiHelper->HandleErrorL( err );
                }
            }
        }
    else if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );

        MPX_DEBUG3( "CMPXAppUi::DoHandleCollectionMessageL Event = %d, Type() = %d",
            event, type );
        if ( event == TMPXCollectionMessage::ECollectionChanged && !SuppressCollectionView() )
            {
            MPX_DEBUG1( "CMPXAppUi::DoHandleCollectionMessageL ECollectionChanged" );
            if ( EMPXLaunchModeTrack != MPXTlsHelper::LaunchMode() )
                {
                CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL( cPath );
                if ( cPath->Levels() != 1 )
                    {
                    RArray<TUid> uids;
                    CleanupClosePushL( uids );
                   if ( iLaunchModePlaying && EMPXLaunchModePlaylist == MPXTlsHelper::LaunchMode() )
                        {
                        iLaunchModePlaying = EFalse;
                        cPath->SelectL(TMPXItemId(0));
                        iCollectionUtility->Collection().OpenL( *cPath );
                        }
                    else
                        {
                        uids.AppendL( TUid::Uid( KMPXPluginTypeCollectionUid ) );
                        uids.AppendL( TUid::Uid( data ) );
                        }

                    // If transitioning from Now Playing to Collection view,
                    // We have to set the selection index
                    // Only applicable if path levels is > 2, ie not music / podcast menu
                    //
                    if( iCurrentViewType.iUid == KMPXPluginTypePlaybackUid )
                        {
                        MMPXSource* s = iPlaybackUtility->Source();
                        if( s && cPath->Levels() > 2 )
                            {
                            CMPXCollectionPlaylist* pl = s->PlaylistL();
                            TInt browsePathCount = cPath->Count();
                            													
                            if( pl )
                                {
                                CleanupStack::PushL( pl );
                                if( browsePathCount > 0 ) // don't set index since this playlist
                                                          // no longer contains any items, this can
                                                          // happen in the case of autoplaylists and cause
                                                          // a hang
                                    {
                                    TInt playlistIndex = pl->Path().Index();
                                    if(playlistIndex < browsePathCount)
                                        {
                                        iCollectionUtility->Collection().CommandL(EMcCmdSelect,
                                                                                  playlistIndex);
                                        }
                                    }                                
                                CleanupStack::PopAndDestroy( pl );
                                }
                            }
                        }

                    // Activate the collection view
                    MPX_DEBUG2( "CMPXAppUi::DoHandleCollectionMessageL cpath depth = %d", cPath->Levels() );
                    // only activate collection view when it's not at plugin selection
                    // screen, it is handled by main view
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
                    icollectionBrowsed = ETrue;
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
                    iViewUtility->ActivateViewL( uids );
                    CleanupStack::PopAndDestroy( &uids );
                    }
                CleanupStack::PopAndDestroy( cPath );
                }
            }
        else if ( event == TMPXCollectionMessage::EPathChanged &&
            !SuppressCollectionView() &&
            type == EMcPathChangedByOpen &&
            data == EMcItemOpened)
            { // Open new entries

            MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL EPathChanged EMcItemOpened" );

            // First get media to check for protected state
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append( TMPXAttribute( KMPXMediaIdGeneral,
                                         EMPXMediaGeneralBasic |
                                         EMPXMediaGeneralUri ));

            attrs.Append( TMPXAttribute( KMPXMediaIdDrm,
                                         EMPXMediaDrmType |
                                         EMPXMediaDrmProtected ));
            CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cPath );
            if( cPath->Count() > 0 )
                {
                iCollectionUtility->Collection().MediaL(
                    *cPath,
                    attrs.Array() );
                }
            else
                {
                // OpenL the blank playlist
                iCollectionUtility->Collection().OpenL();
                }
            // Default start up state for local playback is Playing
            MPXTlsHelper::SetLaunchModeL( EMPXLaunchModePlaying );
            CleanupStack::PopAndDestroy( cPath );
            CleanupStack::PopAndDestroy( &attrs );
            }
        else if( event == TMPXCollectionMessage::EPathChanged &&
            !SuppressCollectionView() &&
            type == EMcPathChangedByOpen &&
            data == EMcContainerOpened  &&
            ( iCurrentViewType.iUid == KMPXPluginTypePlaybackUid ||
              iCurrentViewType.iUid == KMPXPluginTypeEmbeddedPlaybackUid ||
              iCurrentViewType.iUid == KMPXPluginTypeAudioEffectsUid ||
              iCurrentViewType.iUid == KMPXPluginTypeEqualizerUid) )
            {
            MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL EPathChanged EMcContainerOpened" );
            CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cPath );

            TInt levels( cPath->Levels() );
            if ( levels != 1 )
                {
                iDeferredViewActivationUid.Reset();
                if ( iLaunchModePlaying && EMPXLaunchModePlaylist == MPXTlsHelper::LaunchMode() )
                    {
                    iLaunchModePlaying = EFalse;
                    cPath->SelectL(TMPXItemId(0));
                    iCollectionUtility->Collection().OpenL( *cPath );
                    }
                else
                    {
                    iDeferredViewActivationUid.AppendL( TUid::Uid( KMPXPluginTypeCollectionUid ) );
                    iDeferredViewActivationUid.AppendL( TUid::Uid( data ) );
                    }
                // add the collection uid to make sure the correct view is
                // activated
                if ( levels > 0 )
                    {
                    iDeferredViewActivationUid.AppendL( TUid::Uid( cPath->Id( 0 ) ) );
                    }

                // Set selection to playlist index if transitioning from
                // playback to collection view
                // Only applicable if path levels is > 2, ie not music / podcast menu
                //
                MMPXSource* s = iPlaybackUtility->Source();
                if( s && levels > 2 )
                    {
                    CMPXCollectionPlaylist* pl = s->PlaylistL();
                    TInt browsePathCount = cPath->Count();

                    // don't set index since this playlist
                    // no longer contains any items, this can
                    // happen in the case of autoplaylists and cause
                    // a hang
                    //
                    // Make sure we are the same level and the same container before making index call
                    //
                    if( pl )
                        {                    	
                        CleanupStack::PushL( pl );
                        if ( browsePathCount > 0 &&
                            levels == pl->Path().Levels() &&
                            cPath->Id(levels-2) == pl->Path().Id(levels-2) )
                            {
                            TInt playlistIndex = pl->Path().Index();

                            if(playlistIndex < browsePathCount)
                                {
                                iCollectionUtility->Collection().CommandL(EMcCmdSelect,
                                                                         playlistIndex);
                                }
                            }
                        CleanupStack::PopAndDestroy( pl );
                        }
                    }

                MPX_DEBUG2( "CMPXAppUi::HandleCollectionMessageL cpath depth = %d", cPath->Levels() );
                if ( IsForeground() )
                    {
                    MPX_DEBUG3( "CMPXAppUi::HandleCollectionMessageL Activating view now CurUid=0x%x, DefUid=0x%x", iCurrentViewType.iUid, iDeferredViewActivationUid[iDeferredViewActivationUid.Count()-1].iUid);
                    // only activate collection view when it's not at plugin selection
                    // screen, it is handled by main view
                    iViewUtility->ActivateViewL( iDeferredViewActivationUid );
                    iDeferredViewActivationUid.Reset();
                    }
                else
                    {
                    MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL musicplayer in background, deferred view activation" );
                    }
                }
            CleanupStack::PopAndDestroy( cPath );
            }

        else if( event == TMPXCollectionMessage::EPathChanged &&
            type == EMcPathChangedByOpen &&
            data == EMcContainerOpened )
            {
            MPXTlsHelper::SetLaunchModeL( EMPXLaunchModePlaying );
            CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cPath );

            if ( cPath->Levels() == 1 && !IsEmbedded() &&
                iCurrentViewType.iUid != KMPXPluginTypeMainUid &&
                iCurrentViewType.iUid != KMPXPluginTypeWaitNoteDialogUid &&
                !SuppressCollectionView() )
                {
                // activate main view if other clients navigated to first level
                if ( IsForeground() )
                    {
                    MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL Switching to main view" );
                    iViewUtility->ActivateViewL(
                        TUid::Uid( KMPXPluginTypeMainUid ) );
                    }
                else
                    {
                    MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL musicplayer in background, deferred view activation" );
                    iDeferredViewActivationUid.Reset();
                    iDeferredViewActivationUid.Append( TUid::Uid( KMPXPluginTypeMainUid ) );
                    }
                }

			TInt level(2);
			if ( iStartInAlbumMenu )
				{
				level = 3;
				}
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
            // Music Player opens directly at level 2 so we need to initialize
            // the path then
            if( cPath->Levels() == level && iPathRestore && !iPathRestore->Initialized() && !iStandAlone )
#else
            if( cPath->Levels() == 1 && iPathRestore && !iPathRestore->Initialized() )
#endif
                {
                if ( iProcessParameter )
                    {
                    if ( !iProcessParameter->IsPathInited() )
                        {
                        MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL load path" );
                        iPathRestore->LoadPathL();
                        }
                    else
                        {
                        MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL path inited by command handler, not restore path" );
                        }
                    }
                else
                    {
                    MPX_DEBUG1( "CMPXAppUi::HandleCollectionMessageL load path" );
                    iPathRestore->LoadPathL();
                    }
                }
            else if ( iCurrentIndex != KErrNotFound )
                {
                iCollectionUtility->Collection().CommandL( EMcCmdSelect, iCurrentIndex );
                iCurrentIndex = KErrNotFound;
                iCollectionUtility->Collection().OpenL();
                }
            CleanupStack::PopAndDestroy( cPath );
            }

        else if( event == TMPXCollectionMessage::EBroadcastEvent )
            {
            HandleBroadcastMessageL( aMessage );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::IsPlaybackRemote
// -----------------------------------------------------------------------------
//
TBool CMPXAppUi::IsPlaybackRemote()
    {
    TBool isRemote = EFalse;

    TMPXPlaybackPlayerType type( EPbLocal );
    TUid uid;
    TInt index;
    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
    HBufC* subPlayerName = NULL;
    TRAP_IGNORE( manager.GetSelectionL( type, uid, index, subPlayerName ) );
    delete subPlayerName;

//#ifdef __UPNP_FRAMEWORK_2_0_
    if ( FeatureManager::FeatureSupported( KFeatureIdUpnpFramework20 ) )
        {
        // Check for Remote, since in embedded mode
        // playback utility will report type as Unknown.
        if ( type == EPbRemote )
            {
            isRemote = ETrue;
            }
        }

//#endif //__UPNP_FRAMEWORK_2_0_

    return isRemote;
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::SetMediaL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::SetMediaL( CMPXMedia& aMedia )
    {
    // Set to collection
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KMPXCommandIdCollectionSet );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );
    TUid colId( aMedia.ValueTObjectL<TUid>(KMPXMediaGeneralCollectionId));
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, &aMedia );
    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }

// ---------------------------------------------------------------------------
// Handle Custom messages from the collection
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage ) )
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the collection entries being opened.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleOpenL(
    const CMPXMedia& /* aEntries */,
    TInt /* aIndex */,
    TBool /* aComplete */,
    TInt aError )
    {
    if ( aError != KErrNone )
        {
        // Clip the collection path to root level and reopen
        // This could happen after a USB refresh event where the
        // item no longer exists
        //
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( path );
        while( path->Levels() > 1)
            {
            path->Back();
            }
        iCollectionUtility->Collection().OpenL( *path );
        CleanupStack::PopAndDestroy( path );
        }
    }


// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the item being opened.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleOpenL(
    const CMPXCollectionPlaylist& aPlaylist,
    TInt aError )
    {
    MPX_FUNC("CMPXAppUi::HandleOpenL()");
    if ( KErrNone == aError )
        {
        if( !IsEmbedded() )
            {
            TInt count ( aPlaylist.Count() ) ;

            // temporarily disable repeat and shuffle if playlist
            // is a podcast playlist and the list is not empty.
            if(KMPXUidPodcastDBPlugin == TUid::Uid((TInt)aPlaylist.Path().Id(0)))
                {
                if(count > 0)
                    {
                    CMPXCollectionPlaylist* tmpPlaylist =
                                    CMPXCollectionPlaylist::NewL( aPlaylist );
                    CleanupStack::PushL( tmpPlaylist );
                    tmpPlaylist->SetRepeatEnabled( EFalse );
                    tmpPlaylist->SetShuffleEnabledL( EFalse );
                    iPlaybackUtility->InitL( *tmpPlaylist, ETrue );
                    CleanupStack::PopAndDestroy( tmpPlaylist );
                    }
                else
                    {
                    // Empty now playing
                    //
                    CMPXCollectionPlaylist* tmpPlaylist =
                                    CMPXCollectionPlaylist::NewL( aPlaylist );
                    CleanupStack::PushL( tmpPlaylist );
                    tmpPlaylist->SetRepeatEnabled( EFalse );
                    tmpPlaylist->SetShuffleEnabledL( EFalse );
                    iPlaybackUtility->InitL( *tmpPlaylist, ETrue );

                    RArray<TUid> array;
                    CleanupClosePushL( array );

                    CMPXCollectionPath* pbPath =
                            CMPXCollectionPath::NewL( tmpPlaylist->Path() );
                    CleanupStack::PushL( pbPath );
                    if ( pbPath->Levels() > 0 )
                        {
                        array.AppendL( TUid::Uid( pbPath->Id( 0 ) ) );
                        }
                    CleanupStack::PopAndDestroy( pbPath );

                    array.AppendL( KLocalPlaybackUid );

                    if ( IsEmbedded() )
                        {
                        array.AppendL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ));
                        }
                    else
                        {
                        array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ) );
                        }
                    iViewUtility->ActivateViewL( array );
                    CleanupStack::PopAndDestroy( &array );
                    CleanupStack::PopAndDestroy( tmpPlaylist );
                    }
                }
            else
                {
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
                if ( iPdParameterHandler && !iPdParameterHandler->IsPlaybackCompleted())
                    {
                        iPdParameterHandler->HandleCommandL(EPbCmdPause);
                    }
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
                if ( count > 0 )
                    {
                    iPlaybackUtility->InitL( aPlaylist, ETrue );
                    }
                else if( count == 0 &&
                         iPathRestore->Initialized() )
                    {
                    // Empty now playing
                    //
                    iPlaybackUtility->InitL( aPlaylist, EFalse );

                    RArray<TUid> array;
                    CleanupClosePushL( array );

                    CMPXCollectionPath* pbPath =
                            CMPXCollectionPath::NewL( aPlaylist.Path() );
                    CleanupStack::PushL( pbPath );
                    if ( pbPath->Levels() > 0 )
                        {
                        array.AppendL( TUid::Uid( pbPath->Id( 0 ) ) );
                        }
                    CleanupStack::PopAndDestroy( pbPath );

                    if ( IsEmbedded() )
                        {
                        array.AppendL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ));
                        }
                    else
                        {
                        array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ) );
                        }
                    iViewUtility->ActivateViewL( array );
                    CleanupStack::PopAndDestroy( &array );
                    }
                }
            // ETrue->initialize next song concurrently
            }
        else
            {
            // aPlaylist is const, must copy
            //
            CMPXCollectionPlaylist* tmp =
                                    CMPXCollectionPlaylist::NewL( aPlaylist );
            CleanupStack::PushL( tmp );
            tmp->SetEmbeddedPlaylist( ETrue );
            tmp->SetRepeatEnabled( EFalse );
            tmp->SetShuffleEnabledL( EFalse );
            iPlaybackUtility->InitL( *tmp, ETrue );
            CleanupStack::PopAndDestroy( tmp );
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCHelperEmbeddedOpenObserver
// Handles OpenL from that occured in embedded mode
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleEmbeddedOpenL( TInt aErr, TMPXGeneralCategory aCategory  )
    {
    MPX_FUNC( "CMPXAppUi::HandleEmbeddedOpenL" );
    if( aErr != KErrNone )
        {
        iCommonUiHelper->HandleErrorL( aErr );

        // Kill MPX
        RunAppShutter();
        }
    else // No error, set the launch mode
        {
        iPlaybackUtility->CommandL( EPbCmdDisableEffect );
        if( aCategory == EMPXPlaylist )
            {
            if ( MPXTlsHelper::LaunchMode() == EMPXLaunchModePlaying )
                {
                iLaunchModePlaying = ETrue;
                }
            MPXTlsHelper::SetLaunchModeL( EMPXLaunchModePlaylist );
            }
        else // ( aCategory == EMPXSong )
            {
            MPXTlsHelper::SetLaunchModeL( EMPXLaunchModeTrack );
            iViewUtility->ActivateViewL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ));
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXViewActivationObserver.
// Handle view activation.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleViewActivation(
    const TUid& aCurrentViewType,
    const TUid& aPreviousViewType )
    {
    MPX_FUNC( "CMPXAppUi::HandleViewActivation" );
    MPX_DEBUG3("CurrentViewType=0x%x, PreviousViewType=0x%x", aCurrentViewType.iUid, aPreviousViewType.iUid);
    iCurrentViewType = aCurrentViewType;
    iPreviousViewType = aPreviousViewType;
    iDeferredViewActivationUid.Reset();

    switch ( aCurrentViewType.iUid )
        {
        case KMPXPluginTypePlaybackUid:
            {
            if ( IsForeground() )
                {
                iMediaKeyHandler->DismissNotifier( EDisplayMediaPopup );
                }
            break;
            }
      
        case KMPXPluginTypeCollectionUid:
            {
            if (iRefreshCollectionView )
                {
                iRefreshCollectionView = EFalse;
                TRAP_IGNORE(ReOpenPathL());
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CEikAppUi.
// Opens the specified file
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleViewUpdate(
    TUid /* aViewUid */,
    MMPXViewActivationObserver::TViewUpdateEvent /* aUpdateEvent */,
    TBool /* aLoaded */,
    TInt /* aData */)
    {
    MPX_FUNC( "CMPXAppUi::HandleViewUpdate" );
    }

// ---------------------------------------------------------------------------
// From CEikAppUi.
// Opens the specified file
// ---------------------------------------------------------------------------
//
void CMPXAppUi::OpenFileL( const TDesC& aFileName )
    {
    MPX_FUNC( "CMPXAppUi::OpenFileL" );
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
   if ( iPdParameterHandler )
        {
        // no need to open file at this time.
        return;
        }
#endif //RD_BROWSER_PROGRESSIVE_DOWNLOAD
    if ( IsEmbedded() )
        {
        CreateMemberVariablesL();

        iViewUtility->PreLoadViewL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ));
    // Open file with collection helper
    iCollectionUiHelper->OpenL( MPXTlsHelper::HostUid(),
                                aFileName, this,
                                EMPXCollectionPluginMusic );
        }
    else
        {
       
        MPX_DEBUG1( "CMPXAppUi::OpenFileL Standalone" );
        TDataType datatype( TUid::Uid( KMusicPlayerAppUidConstant ) );
        iDocHandler->OpenTempFileL( aFileName, iFile );
        
        CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
        TMPXLaunchMode  mode( EMPXLaunchModeStopped );
        TAiwVariant value(  mode );
        TAiwGenericParam param( EGenericParamPlayerInitMode, value );
        paramList->AppendL( param ); // Takes ownership

        TRAPD( err, iDocHandler->OpenFileEmbeddedL( iFile, datatype, *paramList ));

        //
        //  ignore some document handler errors
        //
        if ( err == KErrServerTerminated )
            {
            //
            //  Document handler leaves with -15 if end key is pressed when
            //  embedded application is not fully started or embedded
            //  application panics in start up.  So we can only close the
            //  App in this point and hope that user is press end key
            //
            HandleCommandL( EEikCmdExit );
            }
        else if ( err != KErrNone )
            {
            //  Reset error since it was handled here and
            //  the program will exit
            //
            User::Leave( err );
            }

        CleanupStack::PopAndDestroy(paramList);
        MPX_DEBUG1( "CMPXAppUi::OpenFileL Register as callback for exit" );
        iDocHandler->SetExitObserver(this);
        
        iStandAlone = ETrue;
            
        }
    }

// ---------------------------------------------------------------------------
// From CCoeAppUi.
// Handle foreground event.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleForegroundEventL(
    TBool aForeground )
    {
    MPX_FUNC_EX( "CMPXAppUi::HandleForegroundEventL" );

    if ( !aForeground )
        {
        // music player is going to background
        // set key repeat rate to normal avkon rate
        CEikonEnv::Static()->WsSession().SetKeyboardRepeatRate(
            KAknKeyboardRepeatInitialDelay, KAknStandardKeyboardRepeatRate );
        CEikonEnv::Static()->WsSession().Flush();
        }

    // Dismiss playback popup if foreground app is active idle or playback
    // view is on foreground
    if ( ( !aForeground && IsActiveIdleEnabled() && IsIdleAppForeground() )
        || ( aForeground
            && ( KMPXPluginTypePlaybackUid == iCurrentViewType.iUid ) ) )
        {
        iMediaKeyHandler->DismissNotifier( EDisplayMediaPopup );
        }

    if ( aForeground && iDeferredViewActivationUid.Count() != 0 )
        {
        MPX_DEBUG1( "CMPXAppUi::HandleForegroundEventL deferred view activation, activating" );
        iViewUtility->ActivateViewL( iDeferredViewActivationUid );
        iDeferredViewActivationUid.Reset();
        }

    CAknViewAppUi::HandleForegroundEventL( aForeground );
    }

// ---------------------------------------------------------------------------
// From CEikAppUi.
// Owerwritten -> file opening
// (app startup via shell commands)
// ---------------------------------------------------------------------------
//
TBool CMPXAppUi::ProcessCommandParametersL(
    TApaCommand /*aCommand*/,
    TFileName& aDocumentName,
    const TDesC8& /*aTail*/ )
    {
    MPX_FUNC_EX( "CMPXAppUi::ProcessCommandParametersL TApaCommand" );
    return ConeUtils::FileExists( aDocumentName );
    }


// ---------------------------------------------------------------------------
// Launch matrix menu suite with specific message
// ---------------------------------------------------------------------------
//
void CMPXAppUi::LaunchMmViewL( const TDesC8& aMessage )
    {
    TApaTaskList taskList( iCoeEnv->WsSession() );
    TApaTask task = taskList.FindApp( TUid::Uid( KMmUid3 ) );

    if ( task.Exists() )
        {
        task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ), 
             aMessage );
        }
    }  

// ---------------------------------------------------------------------------
// From CEikAppUi.
// Handle commands.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleCommandL(
    TInt aCommand )
    {
    MPX_FUNC( "CMPXAppUi::HandleCommandL" );
    TMPXPlaybackCommand cmd = EPbCmdEnd;
    switch ( aCommand )
        {
        case EMPXCmdPlay:
        case EMPXCmdPause:
        case EMPXCmdPlayPause:
        case EMPXCmdStop:
        case EMPXCmdNext:
        case EMPXCmdPrevious:
        case EMPXCmdSeekForward:
        case EMPXCmdSeekBackward:
        case EMPXCmdStopSeeking:
            {
            switch ( aCommand )
                {
                case EMPXCmdPlay:
                    cmd = EPbCmdPlay;
                    break;
                case EMPXCmdPause:
                    cmd = EPbCmdPause;
                    break;
                case EMPXCmdPlayPause:
                    cmd = EPbCmdPlayPause;
                    break;
                case EMPXCmdStop:
                    cmd = EPbCmdStop;
                    break;
                case EMPXCmdNext:
                    cmd = EPbCmdNext;
                    break;
                case EMPXCmdPrevious:
                    cmd = EPbCmdPrevious;
                    break;
                case EMPXCmdSeekForward:
                    cmd = EPbCmdStartSeekForward;
                    break;
                case EMPXCmdSeekBackward:
                    cmd = EPbCmdStartSeekBackward;
                    break;
                case EMPXCmdStopSeeking:
                    cmd = EPbCmdStopSeeking;
                    break;
                default:
                    break;
                }
            DoHandleMediaKeyCommandL( cmd );
            break;
            }
        case EMPXCmdGoToNowPlaying:
            {
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD

            TBool pdPlaybackCompleted(EFalse);
            if ( iPdParameterHandler )
                {
                pdPlaybackCompleted = iPdParameterHandler->IsPlaybackCompleted();
                if ( pdPlaybackCompleted )
                    {
                    delete iPdParameterHandler;
                    iPdParameterHandler = NULL;
                    }
                }

            MMPXPlaybackUtility* activePlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer );

            MMPXPlayer* player =
                activePlaybackUtility->PlayerManager().CurrentPlayer();
            TUid pluginUid( KNullUid );
            RArray<TUid> array;
            CleanupClosePushL( array );


            MMPXSource* source = activePlaybackUtility->Source();
            CMPXCollectionPlaylist* pl = NULL;
            if( source )
                {
                pl = source->PlaylistL();
                }

            if ( pl || !pdPlaybackCompleted )
                {
                if (pl)
                  delete pl;
                if ( player )
                    {
                    array.AppendL( player->UidL() );
                    if ( IsEmbedded() )
                        {
                        array.AppendL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ) );
                        }
                    }
                else
                    {
                    if ( IsEmbedded() )
                        {
                        array.AppendL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ) );
                        }
                    else
                        {
                        array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ) );
                        }
                    }

                // add the collection plugin UID to ensure the correct
                // last playback view is activated
                //MMPXSource* source = activePlaybackUtility->Source();
                if( source )
                    {
                    CMPXCollectionPlaylist* pl = source->PlaylistL();
                    if (pl)
                        {
                        CleanupStack::PushL( pl );
                        const CMPXCollectionPath& path = pl->Path();
                        if(path.Levels() > 0)
                            {
                            array.AppendL(TUid::Uid(path.Id(0)));
                            }
                        CleanupStack::PopAndDestroy( pl );
                        }
                    }
                else if ( !iPathRestore->Initialized() )
                    {
                    if ( iPathRestore->RooPathId() != KNullUid )
                        {
                        array.AppendL( iPathRestore->RooPathId() );
                        }
                    }
                }
            else
                {
                CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL( cPath );

                array.AppendL( TUid::Uid( KMPXPluginTypeCollectionUid ) );
                if ( cPath->Levels() > 0 )
                    {
                    array.AppendL( TUid::Uid( cPath->Id( 0 ) ) );
                    }
                CleanupStack::PopAndDestroy( cPath );
                }

            if ( iAppStartedByBrowser && !icollectionBrowsed && pdPlaybackCompleted)
                {
                activePlaybackUtility->Close();
                CleanupStack::PopAndDestroy( &array );
                RunAppShutter();
                break;
                }
            else
                {
                activePlaybackUtility->Close();
                iViewUtility->ActivateViewL( array );
                CleanupStack::PopAndDestroy( &array );
                }

            break;
            }
#else // RD_BROWSER_PROGRESSIVE_DOWNLOAD not supported
            MMPXPlayer* player =
                iPlaybackUtility->PlayerManager().CurrentPlayer();
            TUid pluginUid( KNullUid );
            RArray<TUid> array;
            CleanupClosePushL( array );

            if ( player )
                {
                array.AppendL( player->UidL() );
                if ( IsEmbedded() )
                    {
                    array.AppendL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ) );
                    }
                }
            else
                {
                if ( IsEmbedded() )
                    {
                    array.AppendL( TUid::Uid( KMPXPluginTypeEmbeddedPlaybackUid ) );
                    }
                else
                    {
                    array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ) );
                    }
                }

            // add the collection plugin UID to ensure the correct
            // last playback view is activated
            MMPXSource* source = iPlaybackUtility->Source();
            if( source )
                {
                CMPXCollectionPlaylist* pl = source->PlaylistL();
                if (pl)
                    {
                    CleanupStack::PushL( pl );
                    const CMPXCollectionPath& path = pl->Path();
                    if(path.Levels() > 0)
                        {
                        array.AppendL(TUid::Uid(path.Id(0)));
                        }
                    CleanupStack::PopAndDestroy( pl );
                    }
                }

            iViewUtility->ActivateViewL( array );
            CleanupStack::PopAndDestroy( &array );
            break;
            }
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD

        case EMPXCmdVolumeChanged:
            cmd = EPbCmdSetVolume;
            HandlePopupL(cmd);
            break;
        case EMPXCmdIgnoreExternalCommand:
            {
            iIgnoreExternalCommand = ETrue;
            break;
            }
        case EMPXCmdHandleExternalCommand:
            {
            iIgnoreExternalCommand = EFalse;
            break;
            }
        case EMPXCmdHideApp:
            {
            // Move Music Player to background
            // magic: -1 = force wg to background
            iEikonEnv->RootWin().SetOrdinalPosition( -1 );
            break;
            }

        case EAknSoftkeyBack:
            {
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
            if ( iViewUtility->ViewHistoryDepth() == 1 || iAppStartedByBrowser )
                {
                HandleCommandL( EEikCmdExit );
                }

            else if ( iPbvStartedByBrowser && !iAppStartedByBrowser )
                {
                iPbvStartedByBrowser = EFalse;

                if( iPdParameterHandler )
                    {
                    if( !( iPdParameterHandler->IsPlaybackCompleted() ) )
                        {

                        iPdParameterHandler->HandleCommandL( EPbCmdStop );
                        }
                    iPdParameterHandler->HandleCommandL( EPbCmdClose );
                    delete iPdParameterHandler;
                    iPdParameterHandler = NULL;
                    }

                iRefreshCollectionView = ETrue;
                iViewUtility->ActivatePreviousViewL();
                BringUpBrowserL();
                }

            else if ( !iPdParameterHandler || !iAppStartedByBrowser )
                {
                iViewUtility->ActivatePreviousViewL();
                }

            else
                {
                HandleCommandL( EEikCmdExit );
                }
#else
            // if we're in top level of our local view stack,
            // back command exits the app.  otherwise activate
            // the previous view in our local view stack
            if ( iViewUtility->ViewHistoryDepth() == 1 )
                {
                HandleCommandL( EEikCmdExit );
                }
            else
                {
                iViewUtility->ActivatePreviousViewL();
                }
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
            break;
            }
        case EEikCmdExit: // fall through        
        case EAknCmdExit:
		case EAknSoftkeyExit:
			{
            if ( iView && KMPXPluginTypeCollectionUid == iCurrentViewType.iUid )
                {
                MPX_DEBUG2( "CMPXAppUi::HandleCommand() "
                            "sending EMPXCmdCancelReorder to view %d",
                            KMPXPluginTypeCollectionUid );
                iView->HandleCommandL( EMPXCmdCancelReorder );
                }
            CloseWaitNoteL();

			if( aCommand == EAknCmdExit || aCommand == EEikCmdExit ) 
				{
				TKeyEvent key;
				key.iCode = EKeyEscape;
				key.iModifiers = 0;

				// Under EQ it is possible to have multiple dialogs open
				TInt err( KErrNone );
				while( IsDisplayingDialog() && err == KErrNone )
					{
					TRAP( err, iCoeEnv->SimulateKeyEventL(key,EEventKey); )
					}
                }

            TBool needToExit( EFalse );
            if ( iSaveMode != EMPXSaveModeIdle )
                {
                iCancelFileMan = ETrue;
                }
            else
                {
                if( iPathRestore )
                    {
                    TRAP_IGNORE(iPathRestore->SavePathL());
                    }

                if ( iPlaybackUtility )
                    {
                    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
                    TRAP_IGNORE( manager.ClearSelectPlayersL() );
                    iPlaybackUtility->CommandL( EPbCmdClose );
                    }
                needToExit = ETrue;
                }


            if( needToExit )
                {
                Exit();
                }
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(
                iEikonEnv->WsSession(),
                AppHelpContextL() );
            break;
            }
        case EMPXCmdSaveAndReopen:
            {
            iCancelFileMan = EFalse;
            iSaveMode = EMPXOpenAfterSave;
            MPX_TRAPD( err, DoSaveL());
            if ( KErrNone != err )
                {
                HandleSaveComplete( err );
                }
            break;
            }
        case EMPXCmdIsFileSaved:
            {
            TFileName SavedAudioFile;
            CreateAutomaticSavePathL( SavedAudioFile );
            break;
            }
        case EMPXCmdSaveForUseAsContact:
        case EMPXCmdSaveForUseAsRingtone:
            {
            iCancelFileMan = EFalse;
            ASSERT( iFile.SubSessionHandle() );
            TBool abort = EFalse;
            TBool aiwExecuted = EFalse;
            TFileName fullName;
            iFile.FullName( fullName );
            TParsePtrC fileParse( fullName );
            TBool isPrivate = (fileParse.NamePresent() &&
                !KMPXMusicplayerPrivateFolder().CompareF(
                    fileParse.Path().Left(
                        KMPXMusicplayerPrivateFolder().Length())));
            TFileName savedFile;
            CreateAutomaticSavePathL( savedFile );


            if ( isPrivate && !ConeUtils::FileExists( savedFile ) )
                {
                HBufC* string( StringLoader::LoadLC( R_MPX_QUERY_CONF_SAVE_DOWNLOADED,
                                                     fileParse.Name() ));
                CAknQueryDialog* confirmationDlg( CAknQueryDialog::NewL(
                    CAknQueryDialog::EConfirmationTone ));
                if ( confirmationDlg->ExecuteLD( R_MPX_SAVE_SONG_CONFIRMATION_QUERY,
                                                 *string ))
                    {
                    // User wants to save the file and execute AIW function
                    if ( aCommand == EMPXCmdSaveForUseAsRingtone )
                        {
                        iSaveMode = EMPXAssignRingtone;
                        }
                    else if ( aCommand == EMPXCmdSaveForUseAsContact )
                        {
                        iSaveMode = EMPXAssignContact;
                        }
                    MPX_TRAPD( err, DoSaveL() );
                    if ( KErrNone != err )
                        {
                        HandleSaveComplete( err );
                        }
                    abort = EFalse;
                    aiwExecuted = ETrue;
                    if ( iView )
                        {
                        iView->HandleCommandL( EMPXCmdSaveComplete );
                        }
                    }
                else
                    {
                    if ( iView )
                        {
                        iView->HandleCommandL( EMPXCmdSaveNotProgress );
                        }
                    abort = ETrue;
                    }
                CleanupStack::PopAndDestroy( string );
                }
            else if ( ConeUtils::FileExists( savedFile ) )
                {
                if ( iView )
                    {
                    iView->HandleCommandL( EMPXCmdSaveNotProgress );
                    }
                MPXTlsHelper::SetFilePath( savedFile );
                }

            if ( !abort && !aiwExecuted && iView )
                {
                if ( aCommand == EMPXCmdSaveForUseAsRingtone )
                    {
                    TRAP_IGNORE(
                        iView->HandleCommandL( EMPXCmdAfterSaveAiwCmdAssign ));
                    }
                else if ( aCommand == EMPXCmdSaveForUseAsContact )
                    {
                    TRAP_IGNORE(
                        iView->HandleCommandL( EMPXCmdAfterSaveUseAsCascade ));
                    }
                }
            break;
            }
        case EMPXCmdSaveAndExit: // fall though
        case EMPXCmdSaveAndExitHostApp:
            {
            iCancelFileMan = EFalse;
            ASSERT( iFile.SubSessionHandle() );
            TFileName fullName;
            iFile.FullName( fullName );
            TParsePtrC fileParse( fullName );
            HBufC* string( StringLoader::LoadLC( R_MPX_QUERY_CONF_SAVE_DOWNLOADED,
                                                 fileParse.Name() ));
            CAknQueryDialog* confirmationDlg( CAknQueryDialog::NewL(
                CAknQueryDialog::EConfirmationTone ));
//#ifdef __COVER_DISPLAY
            confirmationDlg->PublishDialogL(
                    EMPlayerQueryNotSaved,
                    KMPlayerNoteCategory);

            CAknMediatorFacade* covercl(
                AknMediatorFacade( confirmationDlg ) );
            if ( covercl )
                {
                covercl->BufStream() << fileParse.Name();
                }
//#endif //__COVER_DISPLAY

            if ( confirmationDlg->ExecuteLD( R_MPX_SAVE_SONG_CONFIRMATION_QUERY,
                                             *string ))
                {
                iSaveMode = ( aCommand == EMPXCmdSaveAndExit ?
                    EMPXExitAppAfterSave : EMPXExitHostAppAfterSave );
                MPX_TRAPD( err, DoSaveL() );
                if ( KErrNone != err )
                    {
                    HandleSaveComplete( err );
                    }
                }
            else
                {
                if ( aCommand == EMPXCmdSaveAndExit )
                    {
                    RunAppShutter();
                    }
                else
                    {
                    // EMPXCmdSaveAndExitHostApp
                    CAknEnv::RunAppShutter();
                    }
                }
            CleanupStack::PopAndDestroy( string );
            break;
            }
        case EAknCmdTaskSwapper:
            {
            // Handle command when Music Player is selected in Multitasker
            // and Music Player is the foreground app.
            break;
            }

        case EMPXCmdGotoCollection:
            {
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
            iPdPbvStartedByBrowser = EFalse;
            icollectionBrowsed = ETrue;
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
            iStandAlone = EFalse;

            break;
            }

        default:
            {
            // do nothing if the command is not reconized
            MPX_DEBUG2( "CMPXAppUi::HandleCommandL unknown command 0x%x", aCommand );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// From CEikAppUi.
// Handle window server events.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleWsEventL(
    const TWsEvent& aEvent,
    CCoeControl* aDestination )
    {
    MPX_FUNC_EX( "CMPXAppUi::HandleWsEventL" );
    if ( aEvent.Type() == KAknUidValueEndKeyCloseEvent )
        {
		TMPXPlaybackState playerState( EPbStateNotInitialised );
		if( iPlaybackUtility )
			{
			TRAP_IGNORE( playerState = iPlaybackUtility->StateL() );
			}

        if ( ( iCommonUiHelper->ExitOptionHiddenL() && !IsEmbedded() ) ||
			EPbStatePlaying == playerState ||
			EPbStatePaused == playerState )
            {
            // Go back to Idle when End key is pressed in non-embedded mode or
            // player is in playing/paused mode
            if( IsForeground() )
                {
                ActivatePhoneAppL();
                }
            return;
            }
        }

    // Notify the playback popup that orientation is about to change
    if( iMediaKeyHandler && aEvent.Type() == EEventScreenDeviceChanged )
        {
        iMediaKeyHandler->NotifyOrientationChangeBegin();
        }

    CAknAppUi::HandleWsEventL( aEvent, aDestination );
    }

// ---------------------------------------------------------------------------
// From MCoeMessageObserver.
// Handle messages from windows server.
// ---------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CMPXAppUi::HandleMessageL(
    TUint32 aClientHandleOfTargetWindowGroup,
    TUid aMessageUid,
    const TDesC8& aMessageParameters )
    {
    MPX_FUNC_EX( "CMPXAppUi::HandleMessageL" );

    MCoeMessageObserver::TMessageResponse ret( EMessageNotHandled );

    if ( aMessageUid == KAppUidMusicPlayerX )
        {
        MPX_DEBUG2( "CMPXAppUi::HandleMessageL Message received for music player, size = %d", aMessageParameters.Size() );
        // message for music player
        HandleCommandParametersL( aMessageParameters );
        ret = EMessageHandled;
        }
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
    else if ( aMessageUid == KNullUid )
        {
        ASSERT(!IsEmbedded());////browser should not send messages to music player if it is not RootApp.
        if ( aMessageParameters.Length() )
            {
            if ( IsBrowserClosedL(aMessageParameters) )
                {
                if ( iAppStartedByBrowser )
                    {
                    RunAppShutter();
                    return EMessageHandled;
                    }
                }
            else
                {
                if (!iPdParameterHandler)
                    {
                    iPdParameterHandler = CMPXPdCommandHandler::NewL(iPlaybackUtility);
                    }
                if(KErrNotFound == iPdParameterHandler->HandleCommandParametersL( aMessageParameters )) // File or dl id do not exist.
                    {
                    iCommonUiHelper->DisplayInfoNoteL(R_MPX_FILE_NOT_FOUND_TEXT);
                    HandleCommandL(EMPXCmdGoToNowPlaying);
                    BringUpBrowserL();
                    }
                else
                    {
                    iPbvStartedByBrowser = ETrue;
                    }
                ret = EMessageHandled;
                }
            }
        }
#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD
    else
        {
        ret = CAknViewAppUi::HandleMessageL(
            aClientHandleOfTargetWindowGroup,
            aMessageUid,
            aMessageParameters );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// From CEikAppUi.
// process message sent by another application
// -----------------------------------------------------------------------------
//
void CMPXAppUi::ProcessMessageL(
        TUid aUid, const TDesC8& aParams )
    {
    MPX_FUNC_EX( "CMPXAppUi::ProcessMessageL" );
    if ( aUid == KAppUidMusicPlayerX )
        {
        // message for music player
        MPX_DEBUG2( "CMPXAppUi::ProcessMessageL Message received for music player, size = %d", aParams.Size() );
        HandleCommandParametersL( aParams );
        }
    }

// -----------------------------------------------------------------------------
// From CEikAppUi.
// process command line parameter
// -----------------------------------------------------------------------------
//
TBool CMPXAppUi::ProcessCommandParametersL( CApaCommandLine& aCommandLine )
    {
    MPX_FUNC_EX( "CMPXAppUi::ProcessCommandParametersL CApaCommandLine" );
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
    if (EApaCommandOpen == aCommandLine.Command())
        {
        TPtrC param = aCommandLine.DocumentName();
        if ( param.Length() )
            {
            if (!ConeUtils::FileExists(param))
                {
                iAppStartedByBrowser = ETrue;
                iPdPbvStartedByBrowser = ETrue;
                // Convert param to 8-bit.
                HBufC8* param8 = HBufC8::NewLC( param.Length() );
                param8->Des().Copy( param );
                if ( IsBrowserClosedL(*param8) )
                    {
                    Exit();
                    }
                else
                    {
                    if (!iPdParameterHandler)
                        {
                        iPdParameterHandler = CMPXPdCommandHandler::NewL(iPlaybackUtility);//this is where it is created the first time
                        }
                    if ( KErrNotFound == iPdParameterHandler->HandleCommandParametersL( *param8 ) )
                        {
                        delete iPdParameterHandler;
                        iPdParameterHandler = NULL;
                        iCommonUiHelper->DisplayInfoNoteL(R_MPX_FILE_NOT_FOUND_TEXT);
                        BringUpBrowserL();
                        Exit();
                        }
                    else
                        {
                        // Default start up state for Progressive Download playback is Playing
                        MPXTlsHelper::SetLaunchModeL( EMPXLaunchModePlaying );
                        }
                    }
                CleanupStack::PopAndDestroy( param8 );
                }
            }
        }
#endif  // RD_BROWSER_PROGRESSIVE_DOWNLOAD
    const TPtrC8 command = aCommandLine.TailEnd();
        if ( command.Size() > 0 )
        {
        HandleCommandParametersL( command );
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From MMPXSaveHelperObserver
// Callback function to signal that saving a file is complete
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleSaveComplete( TInt aErr )
    {
    MPX_DEBUG2("CMPXAppUi::HandleSaveComplete(%d) entering", aErr);
    TRAP_IGNORE( DoHandleSaveCompleteL( aErr ) );
    MPX_DEBUG1("CMPXAppUi::HandleSaveComplete() exiting" );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionHelperObserver
// Callback function to signal that adding a file is complete
// ---------------------------------------------------------------------------
//
void CMPXAppUi::HandleAddFileCompleteL( TInt aErr )
    {
    MPX_DEBUG2("CMPXAppUi::HandleAddFileCompleteL(%d) entering", aErr);

    iCommonUiHelper->DismissWaitNoteL();

    // close collectionhelper since it is not needed now
    if ( iCollectionHelper )
        {
        iCollectionHelper->Close();
        iCollectionHelper = NULL;
        }

    if ( !aErr )
        {
        if (( iSaveMode != EMPXAssignRingtone ) &&
            ( iSaveMode != EMPXAssignContact ))
            {
            iCommonUiHelper->DisplayConfirmNoteL( R_MPX_SAVED_TO_COLLECTION_NOTE );
            }
        }
    else if ( KErrNotSupported == aErr )
        {
        iCommonUiHelper->DisplayConfirmNoteL( R_MPX_SAVED_NOTE );
        }
    else if( iIsUsbOnGoing && (aErr != KErrNone) )
        {
        // Kill mpx embedded mode so usb can start
        //
        MPX_DEBUG1("CMPXAppUi::HandleAddFileCompleteL usb ongoing");
        iSaveMode = EMPXExitAppAfterSave;
        }
    else
        {
        iCommonUiHelper->HandleErrorL( aErr );
        }

    if ( iView )
        {
        iView->HandleCommandL( EMPXCmdSaveComplete );
        }

    HandlePostSaveOperationL();

    MPX_DEBUG1("CMPXAppUi::HandleAddFileCompleteL() exiting" );
    }

// ---------------------------------------------------------------------------
// From MProgressDialogCallback.
// Get's called when a dialog is dismissed.
// ---------------------------------------------------------------------------
//
void CMPXAppUi::DialogDismissedL( TInt aButtonId )
    {
    MPX_DEBUG1("CMPXAppUi::DialogDismissedL");

// Cover UI start
//#ifdef __COVER_DISPLAY
    if ( iCommandInitiator )
        {
        iCommandInitiator->CancelCommand( KMediatorSecondaryDisplayDomain,
                                    KMPlayerNoteCategory,
                                    EMPlayerNoteSaveOneItem );
        }
//#endif
// Cover UI end

    if ( aButtonId == EAknSoftkeyCancel && iView )
        {
        iView->HandleCommandL( EMPXCmdSaveCancelled );
        if ( iSaveMode != EMPXSaveModeIdle )
            {
            iCancelFileMan = ETrue;
            if( iSaveHelper )
                {
                iSaveHelper->Cancel();
                }
            iSaveMode = EMPXSaveModeIdle;
            }
        }
    MPX_DEBUG1("CMPXAppUi::DialogDismissedL");
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::NotifyFileManOperation
// -----------------------------------------------------------------------------
//
MFileManObserver::TControl CMPXAppUi::NotifyFileManOperation()
    {
    MPX_DEBUG1("CMPXAppUi::NotifyFileManOperation(): entering");
    // this code is run in CFileMan working thread.
    if( iCancelFileMan )
        {
        MPX_DEBUG1("CMPXAppUi::NotifyFileManOperation(): ECancel");
        return MFileManObserver::ECancel;
        }
    return MFileManObserver::EContinue;
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::DoUpdatePluginRefreshedTimeL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::DoUpdatePluginRefreshedTimeL()
    {
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KMPXCommandIdUpdateRefreshTime );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );

    // Update Music
    cmd->SetTObjectValueL<TUid>( KMPXCommandGeneralCollectionId, iMusicCollectionId );
    iCollectionUtility->Collection().CommandL( *cmd );

    // Update Podcast
    if( !iDisablePodcasting )
        {
        cmd->SetTObjectValueL<TUid>( KMPXCommandGeneralCollectionId, iPodcastCollectionId );
        iCollectionUtility->Collection().CommandL( *cmd );
        }
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::HandlePropertyL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::HandlePropertyL(TMPXPlaybackProperty aProperty, TInt aValue, TInt aError)
    {
    if (aError == KErrNone)
        {
        switch (aProperty)
            {
        case EPbPropertyVolume:
            {
            iCurrentVolume = aValue;
            }
            break;

        case EPbPropertyMaxVolume:
            {
            iMaxVolume = aValue;
            }
            break;

        default:
            break;
            }
        }
    else
        {
        // How to handle error?
        }
    }


// -----------------------------------------------------------------------------
// CMPXAppUi::HandleSubPlayerNamesL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::HandleSubPlayerNamesL(
    TUid /*aPlayer*/,
    const MDesCArray* /*aSubPlayers*/,
    TBool /*aComplete*/,
    TInt /*aError*/)
    {

    }

// -----------------------------------------------------------------------------
// CMPXAppUi::HandleMediaL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::HandleMediaL(
    const CMPXMedia& /*aProperties*/,
    TInt /*aError*/)
    {

    }
// -----------------------------------------------------------------------------
// CMPXAppUi::IsActiveView
// -----------------------------------------------------------------------------
//
TBool CMPXAppUi::IsActiveView(TInt aViewType )
    {
    return (iCurrentViewType.iUid == aViewType ) || ((iCurrentViewType.iUid == KMPXPluginTypeWaitNoteDialogUid ) && (iPreviousViewType.iUid == aViewType));
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::ReOpenPathL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::ReOpenPathL()
    {
    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL ( );
    CleanupStack::PushL ( path );
    if ( path->Levels ( )> 1 )
        {
        path->Back ( );
        iCollectionUtility->Collection().OpenL ( *path );
        }
    CleanupStack::PopAndDestroy ( path );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::HandleServerAppExit
// -----------------------------------------------------------------------------
//
void CMPXAppUi::HandleServerAppExit(TInt /*aReason*/)
    {
        RunAppShutter();
    }
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
// -----------------------------------------------------------------------------
// CMPXAppUi::IsBrowserClosedL
// -----------------------------------------------------------------------------
//
TBool CMPXAppUi::IsBrowserClosedL(
    const TDesC8& aParams )
    {
    TBool ret(EFalse);
    CBufFlat* paramBuf = CBufFlat::NewL( /*anExpandSize*/1 );
    CleanupStack::PushL( paramBuf );
    paramBuf->InsertL( 0, aParams );
    RBufReadStream readStream( *paramBuf );

    CAiwGenericParamList* genParList = CAiwGenericParamList::NewLC( readStream );

    TInt index = 0;
    const TAiwGenericParam* paramTerminate = genParList->FindFirst( index,
        EGenericParamTerminate, EVariantTypeTInt32 );
    TInt32 terminateID = 0;
    if ( paramTerminate )
        {
        paramTerminate->Value().Get( terminateID );
        }

    if ( terminateID )
        {
        ret = ETrue;
        }

    CleanupStack::PopAndDestroy( genParList );
    CleanupStack::PopAndDestroy( paramBuf );

    return ret;
    }

// ---------------------------------------------------------
// Bring Browser application to the foreground.
// ---------------------------------------------------------
//
TBool CMPXAppUi::BringUpBrowserL()
    {
    TBool browserFound(EFalse);
    RWsSession& wsSession=iEikonEnv->WsSession();
    CAknTaskList* taskList = CAknTaskList::NewL( wsSession );
    TApaTask task = taskList->FindRootApp( TUid::Uid(KMPXBrowserUid) );
    delete taskList;
    if( task.Exists() )
        {
        task.BringToForeground();
        browserFound = ETrue;
        }
    return browserFound;
    }

#endif // RD_BROWSER_PROGRESSIVE_DOWNLOAD

// -----------------------------------------------------------------------------
// CMPXAppUi::SuppressCollectionView
// -----------------------------------------------------------------------------
//
TBool CMPXAppUi::SuppressCollectionView()
    {
#ifdef RD_BROWSER_PROGRESSIVE_DOWNLOAD
            return ( iPdPbvStartedByBrowser || iStandAlone );
#else
            return ( iStandAlone );
#endif
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::SetVolumeL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::SetVolumeL( const TInt aVolume )
    {
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdSetVolume );
    cmd->SetTObjectValueL<TUint>( KMPXCommandPlaybackGeneralData, aVolume );
    
    MMPXPlaybackUtility* activePbu = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer );
    CleanupClosePushL( *activePbu );
    activePbu->CommandL( *cmd );
    CleanupStack::PopAndDestroy( activePbu );
    
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::MuteVolumeL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::MuteVolumeL()
    {
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdMuteVolume );
    
    MMPXPlaybackUtility* activePbu = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer );
    CleanupClosePushL( *activePbu );
    activePbu->CommandL( *cmd );
    CleanupStack::PopAndDestroy( activePbu );
    
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::UnMuteVolumeL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::UnMuteVolumeL()
    {
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, EPbCmdUnMuteVolume );
    
    MMPXPlaybackUtility* activePbu = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer );
    CleanupClosePushL( *activePbu );
    activePbu->CommandL( *cmd );
    CleanupStack::PopAndDestroy( activePbu );
    
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::UnMuteVolume
// -----------------------------------------------------------------------------
//
TInt CMPXAppUi::MPXCmdForPbCmd(
    TMPXPlaybackCommand aCommand )
    {
    TInt cmd( KErrNotFound );

    switch ( aCommand )
        {
        case EPbCmdStop:
            {
            cmd = EMPXCmdStop;
            break;
            }
        case EPbCmdPlay:
            {
            cmd = EMPXCmdPlay;
            break;
            }
        case EPbCmdPause:
            {
            cmd = EMPXCmdPause;
            break;
            }
        case EPbCmdPlayPause:
            {
            cmd = EMPXCmdPlayPause;
            break;
            }
        case EPbCmdNext:
            {
            cmd = EMPXCmdNext;
            break;
            }
        case EPbCmdPrevious:
            {
            cmd = EMPXCmdPrevious;
            break;
            }
        case EPbCmdStartSeekForward:
            {
            cmd = EMPXCmdSeekForward;
            break;
            }
        case EPbCmdStartSeekBackward:
            {
            cmd = EMPXCmdSeekBackward;
            break;
            }
        case EPbCmdStopSeeking:
            {
            cmd = EMPXCmdStopSeeking;
            break;
            }
        default:
            {
            // Command not found
            break;
            }
        }
    return cmd;
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::CheckAvailableMemoryL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::CheckAvailableMemoryL( TInt aNeedMemory )
    {
    TMemoryInfoV1Buf info;
    UserHal::MemoryInfo(info);
    if( info().iFreeRamInBytes < aNeedMemory )
        {
        HBufC* title =
            StringLoader::LoadLC( R_MPX_NO_MEMORY_INFO );
        CAknGlobalNote* infoNote = CAknGlobalNote::NewLC();
        infoNote->ShowNoteL(
            EAknGlobalInformationNote,
            title->Des() );
        CleanupStack::PopAndDestroy( infoNote );
        CleanupStack::PopAndDestroy( title );
        Exit();
        return;
        }
    }


// -----------------------------------------------------------------------------
// CMPXAppUi::CheckUpdatesComplete
// from base class MIAUpdateObserver
// -----------------------------------------------------------------------------
//
void CMPXAppUi::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    MPX_DEBUG2("CMPXAppUi::CheckUpdatesComplete(UpdatesComplete: %d) entering", aErrorCode);
    if ( aErrorCode == KErrNone )
        {
            MPX_DEBUG2("CMPXAppUi::CheckUpdatesComplete(AvailableUpdates: %d)", aAvailableUpdates);
            if ( aAvailableUpdates > 0 )
                {
                // There were some updates available. Let's ask if the user wants to update them.
                iUpdate->UpdateQuery();
                }
            else
                {
                // No updates available. CIAUpdate object could be deleted
                CleanUpdateParams();
                }
        }
        MPX_DEBUG1( "CMPXAppUi::CheckUpdatesComplete() exiting" );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::UpdateComplete
// from base class MIAUpdateObserver
// -----------------------------------------------------------------------------
//
void CMPXAppUi::UpdateComplete( TInt /*aErrorCode*/, CIAUpdateResult* aResultDetails )
    {
    MPX_DEBUG1( "CMPXAppUi::UpdateComplete() entering" );
    // The update process that the user started from IAUpdate UI is now completed.
    if(aResultDetails)
        {
        delete aResultDetails; // Ownership was transferred, so this must be deleted by the client
        aResultDetails = NULL;
        }

    // We do not need the client-server session anymore, let's delete the object to close the session
    CleanUpdateParams();
    MPX_DEBUG1( "CMPXAppUi::UpdateComplete() exiting" );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::UpdateQueryComplete
// from base class MIAUpdateObserver
// -----------------------------------------------------------------------------
//
void CMPXAppUi::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {
    MPX_DEBUG2("CMPXAppUi::UpdateQueryComplete(QueryComplete: %d) entering", aErrorCode);
    if ( aErrorCode == KErrNone )
        {
        MPX_DEBUG2("CMPXAppUi::UpdateQueryComplete(UpdateNow: %d) ", aUpdateNow);
        if ( aUpdateNow )
            {
            // User choosed to update now, so let's launch the IAUpdate UI.
            iUpdate->ShowUpdates( *iParameters );
            }
        else
            {
            // The answer was 'Later'. CIAUpdate object could be deleted
            CleanUpdateParams();
            }
         }
    MPX_DEBUG1( "CMPXAppUi::UpdateQueryComplete() exiting" );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::UpdateQueryComplete
// -----------------------------------------------------------------------------
//
void CMPXAppUi::CleanUpdateParams()
    {
    MPX_DEBUG1( "CMPXAppUi::CleanUpdateParams() entering" );
    if(iUpdate)
        {
        delete iUpdate;
        iUpdate = NULL;
        }
    if(iParameters)
        {
        delete iParameters;
        iParameters = NULL;
        }
    MPX_DEBUG1( "CMPXAppUi::CleanUpdateParams() exiting" );
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::CheckUpdatesL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::CheckUpdatesL()
    {
    MPX_DEBUG1( "CMPXAppUi::CheckUpdatesL() entering" );
    if ( FeatureManager::FeatureSupported( KFeatureIdIAUpdate ) )
        {
        MPX_DEBUG1( "CMPXAppUi::CheckUpdatesL() IAD Update supported" );

        TRAP_IGNORE( iUpdate = CIAUpdate::NewL( *this ) );

        MPX_DEBUG1( "CMPXAppUi::CheckUpdatesL() IAD Update Client Created" );

        if ( iUpdate )
            {
            MPX_DEBUG1( "CMPXAppUi::CheckUpdatesL() creating IAD Update paramentes" );
            iParameters = CIAUpdateParameters::NewL();

            // Search for updates using SIS package UID
            iParameters->SetUid( TUid::Uid( KMPXMusicPlayerSISPkgUid ) );

            //mpx.exe to be started after update is finished
            iParameters->SetCommandLineExecutableL( KMPXMusicPlayerExec );

            //check the updates
            iUpdate->CheckUpdates( *iParameters );
            }
        }
    MPX_DEBUG1( "CMPXAppUi::CheckUpdatesL() exiting" );
    }

#ifdef FF_OOM_MONITOR2_COMPONENT
// -----------------------------------------------------------------------------
// CMPXAppUi::SetMpxOomPriorityL
// -----------------------------------------------------------------------------
//
void CMPXAppUi::SetMpxOomPriorityL( ROomMonitorSession::TOomPriority aPriority )
    {
    if( aPriority != iOomPriority )
        {
        ROomMonitorSession monitorSession;
        User::LeaveIfError ( monitorSession.Connect() );
        // No leaving code after this point, so no need to use cleanup stack
        monitorSession.SetOomPriority( aPriority );
        iOomPriority = aPriority;
        monitorSession.Close(); // CLOSE THE OOM SESSION....
        }
    }

// -----------------------------------------------------------------------------
// CMPXAppUi::CheckAvailableMemoryByOomL
// -----------------------------------------------------------------------------
//
void  CMPXAppUi::CheckAvailableMemoryByOomL( TInt aNeedMemory )
    {
    ROomMonitorSession oomMonitor;
    User::LeaveIfError( oomMonitor.Connect() );
    // No leaving code after this point, so no need to use cleanup stack
    TInt errorCode = oomMonitor.RequestFreeMemory( aNeedMemory );
    oomMonitor.Close();

    if ( errorCode != KErrNone )
        {
        // NOT ENOUGH MEMORY ....Show Information note and exit out..
        HBufC* title = StringLoader::LoadLC( R_MPX_NO_MEMORY_INFO );
        CAknGlobalNote* infoNote = CAknGlobalNote::NewLC();
        infoNote->ShowNoteL(
            EAknGlobalInformationNote,
            title->Des() );
        CleanupStack::PopAndDestroy( infoNote );
        CleanupStack::PopAndDestroy( title );
        Exit();
        }
    }
#endif // FF_OOM_MONITOR2_COMPONENT

// End of File
