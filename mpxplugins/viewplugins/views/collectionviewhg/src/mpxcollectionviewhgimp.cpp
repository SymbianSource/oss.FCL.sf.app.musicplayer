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
* Description:  Implementation of Collection view
*
*/

// INCLUDE FILES
#include <bldvariant.hrh>
#include <f32file.h>
#include <eikmenub.h>
#include <coeutils.h>
#include <aknlists.h>
#include <aknViewAppUi.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <aknnavilabel.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <textresolver.h>
#include <aknnotewrappers.h>
#include <aknnavi.h>
#include <hlplch.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <avkon.mbg>
#include <sendui.h>
#include <CMessageData.h>
#include <centralrepository.h>
#include <MProfileEngine.h>
#include <AknDlgShut.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE
#include <aknmessagequerydialog.h>
#include <gfxtranseffect/gfxtranseffect.h>    
#include <akntranseffect.h>

#include <upnpcopycommand.h>
#include <AiwServiceHandler.h>  //Copy to remote feature

#include <mediarecognizer.h>
#include <featmgr.h>
#include <AknMediatorFacade.h>
#include <MediatorCommandInitiator.h>
#include <MediatorDomainUIDs.h>

#include <mplayersecondarydisplayapi.h>
#include <data_caging_path_literals.hrh>
#include <layoutmetadata.cdl.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxcollectionmessage.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxcollectionviewhg.rsg>
#include <mpxcollectionviewhg.mbg>
#include <mpxcommonui.rsg>
#include <mpxcollectionutility.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmedia.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionplaylist.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediaarray.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxviewpluginmanager.h>
#include <mpxviewplugin.h>
#ifdef BACKSTEPPING_INCLUDED
#include <mpxbacksteppingutility.h>
#endif // BACKSTEPPING_INCLUDED
#include <mpxcollectionopenutility.h>

#include <mpxfindinmusicshop.h>
#include <mpxfindinmusicshopcommon.h>  // KFindInMShopKeyInValid

// cenrep key need to be checked whether Mass Storage or MTP is connected
#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>
#include <mtpprivatepskeys.h>
#include <mpxappui.hrh>
#include <mpxinternalcrkeys.h>
#include <mpxtlshelper.h>
#include <mpxuser.h>
#include "mpxcollectionviewhglistboxarray.h"
#include "mpxcommoncontainer.hrh"
#include "mpxcommonuihelper.h"
#include "mpxcollectionviewhgcontainer.h"
#include "mpxcollectionviewhg.hrh"
#include "mpxcollectionviewhg.hlp.hrh"
#include "mpxcollectionviewhgimp.h"
#include "mpxviewprivatepskeys.h"
#include "mpxlog.h"


// CONSTANTS
_LIT( KMPXCollectionRscPath, "mpxcollectionviewhg.rsc" );

const TInt KMilliSecondsToSeconds( 1000 );
const TInt KMPXReorderNaviPaneGranularity( 2 );
const TInt KMPXMaxBufferLength( 160 );
const TInt KMPXMaxTimeLength( 36 );
const TInt KMPXDurationDisplayResvLen( 10 );
const TInt KMPXPlaylistExportRetry( 10 );
const TInt KMPXDirectionUp( -1 );
const TInt KMPXDirectionDown( 1 );
const TInt KMPXErrDataNotReady( -8000 );
const TInt KMPXErrDataNotReadyCacheCommand( -8001 );
const TInt KMPXArrayGranularity( 100 );
const TInt KMusicCollectionMenuLevel( 2 );

_LIT( KMPXCollDetailsItemsFormat, "%S\t%S" );

_LIT( KCategoryEmpty, "" );

const TInt KIncrementalDelayNone = 0;
const TInt KIncrementalDelayHalfSecond = 500000;
const TInt KIncrementalFetchBlockSize = 400;
const TInt KIncrementalDirectionCount = 8;
const TInt KWaitNoteImpUid = 0x101FFC6C; // refresh wait note UID
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
const TInt KMusicMenuPodcastMenuItemIndex = 4; // podcast menu item index
#define KPodcastCollectionUid 0x101FFC3C
#endif

const TInt KMaxIntLen( 10 );
_LIT8( KMVPrefix, "MVviewID:" );
const TInt KMVPrefixLen( 9 );

// Music setting
const TUid KCRUidMPXMPSettings = {0x101FFCDC};
const TUint32 KMPXMusicStoreUID = 0x00000003;
const TUint32 KOperatorMusicStore = 0x00000004;
const TUint32 KOperatorMusicStoreType = 0x00000005;
const TUint32 KOperatorMusicStoreDisplayName = 0x00000006;
const TUint32 KOperatorMusicStoreNativeUid = 0x00000007;
const TUint32 KOperatorMusicStoreJavaName = 0x00000008;
const TUint32 KOperatorMusicStoreWebPage = 0x00000009;
const TUint32 KOperatorMusicStoreURI = 0x0000000A;
const TUint32 KEducatingUserURI = 0x0000000D;
const TUint32 KEducatingPopupState = 0x0000000E;

const TInt KJavaMusicShopType( 1 );
const TInt KUIDMaxLength = 8;
const TInt KMPXMaxHistoryLength( 255 );

#define KProgressDownloadUid  0x10207BCD

// Music collection browse type
const TInt KMPXCollectionArtistAlbum( 3 );
const TInt KMPXCollectionGenre( 5 );

_LIT(KEmptyTitle, " ");

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewHgImp* CMPXCollectionViewHgImp::NewL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::NewL" );
    CMPXCollectionViewHgImp* self = CMPXCollectionViewHgImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewHgImp* CMPXCollectionViewHgImp::NewLC()
    {
    CMPXCollectionViewHgImp* self = new ( ELeave ) CMPXCollectionViewHgImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCollectionViewHgImp::~CMPXCollectionViewHgImp()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::~CMPXCollectionViewHgImp" );

    if ( iCoverDisplay )
        {
        delete iCommandInitiator;
        }

    // Cleanup observer, does not panic if not found
    //
    AppUi()->RemoveViewDeactivationObserver( this );

    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    if ( iViewUtility )
        {
        iViewUtility->RemoveObserver( this );
        iViewUtility->Close();
        }

    if ( iCollectionUiHelper )
        {
        iCollectionUiHelper->Close();
        }

    if ( iCollectionHelper )
        {
        iCollectionHelper->Close();
        }

    if ( iProfileEngine )
        {
        iProfileEngine->Release();
        }

    if ( iUpnpFrameworkSupport )
        {
        MPX_DEBUG1(_L("CMPXCollectionViewHgImp::~CMPXCollectionViewHgImp Detaching 'Copy to external' menu service..."));
        if ( iServiceHandler )
            {
            iServiceHandler->DetachMenu( R_MPX_COLLECTION_VIEW_MENU_1,
                                     R_MPX_UPNP_COPY_TO_EXTERNAL_MENU_INTEREST );
        iServiceHandler->DetachMenu( R_MPX_USE_AS_CASCADE,
                                 R_MPX_AIW_ASSIGN_INTEREST );
            delete iServiceHandler;
            iServiceHandler = NULL;
            }
        iPlayersList.Close();
        delete iSubPlayerName;
        }
    if ( iUpnpCopyCommand )
        {
        delete iUpnpCopyCommand;
        }

    if ( iServiceHandler )
        {
        iServiceHandler->DetachMenu( R_MPX_USE_AS_CASCADE,
                                 R_MPX_AIW_ASSIGN_INTEREST );
        delete iServiceHandler;
        }
    if ( iMediaRecognizer )
        {
        delete iMediaRecognizer;
        }

#ifdef BACKSTEPPING_INCLUDED
    if( iBackSteppingUtility )
        {
        iBackSteppingUtility->Close();
        }
#endif // BACKSTEPPING_INCLUDED

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    if( iTitleWait )
	    {
	    delete iTitleWait;
		}
    delete iUserPlaylists;
    delete iCommonUiHelper;
    delete iSendUi;
    delete iTitle;
    delete iDuration;
    delete iOriginalTitle;
    delete iOriginalDuration;
    delete iNewName;
    delete iBottomIndex;
    delete iCurrentCba;
    delete iIncrementalOpenUtil;
    delete iCachedSelectionIndex;
    FeatureManager::UnInitializeLib();
    delete iOperatorMusicStoreName ;
    if ( iOperatorMusicStoreURI )
        {
        delete iOperatorMusicStoreURI;
        }
    if ( iEducateUserURI )
        {
        delete iEducateUserURI;
        }
    if (iRepository)
        {
        delete iRepository;
        }
    if (iStoredAlbum)
        delete iStoredAlbum;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewHgImp::CMPXCollectionViewHgImp() :
    iLastDepth( 1 ),
    iPlayIndex( KErrNotFound ),
    iSetMediaLCount( KErrNotFound ),
    iCurrentHighlightedIndex( KErrNotFound ),
    iCachedCommand( KErrNotFound ),
    iNoteType( EMPXNoteNotDefined ),
	iFirstIncrementalBatch( ETrue ),
	iDialogDismissed( EFalse ),
	iTitleWait( NULL )
	{
    MPX_FUNC( "CMPXCollectionViewHgImp::CMPXCollectionViewHgImp" );
    iUsingNokiaService = EFalse;
#ifdef __ENABLE_MSK
    iCurrentMskId = KErrNotFound;
    iShowContextMenu = EFalse;
    iCollectionCacheReady = ETrue;
#endif // __ENABLE_MSK
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
    iPodcast = EFalse;
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU
    iInAlbumArtDialog = EFalse;

    iMarkedAll = EFalse;
    // grab the current process priority
    RProcess proc;
    iPriority = proc.Priority();
    iGoToNowPlaying = EFalse; 
    }

void CMPXCollectionViewHgImp::HandleStatusPaneSizeChange()
    {
	MPX_FUNC( "CMPXCollectionViewHgImp::HandleStatusPaneSizeChange" );
	CMPXCollectionViewHg::HandleStatusPaneSizeChange();
	if ( iContainer )
        {
		iContainer->SetRect( ClientRect() );
		UpdateTitlePaneL();
        }
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::ConstructL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::ConstructL" );

// initialize FeatureManager
    FeatureManager::InitializeLibL();

    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        iCoverDisplay = ETrue;
        iCommandInitiator = CMediatorCommandInitiator::NewL( NULL );
        }
    else
        {
        iCoverDisplay = EFalse;
        }


    CCoeEnv* coeEnv( iEikonEnv );
    TParse parse;
    parse.Set( KMPXCollectionRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    BaseConstructL( R_MPX_COLLECTION_VIEW );

    iIsEmbedded = iEikonEnv->StartedAsServerApp();

    // create a new collection utility bound to the default context.
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );
    iViewUtility = MMPXViewUtility::UtilityL();
    iViewUtility->AddObserverL( this );
    iBottomIndex = new (ELeave) CArrayFixFlat<TInt>( 1 );

    iCommonUiHelper = CMPXCommonUiHelper::NewL( iCollectionUtility );
    iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();

    // Monitor for view activation
    AppUi()->AddViewActivationObserverL( this );

    if (!iUpnpCopyCommand )
        {
        MPX_TRAPD ( error, iUpnpCopyCommand = CUpnpCopyCommand::NewL() );
		    if ( error == KErrNone )
            {
            iUpnpFrameworkSupport = ETrue;
            iServiceHandler = CAiwServiceHandler::NewL();
            MPX_DEBUG1("CMPXCollectionViewHgImp::ConstructL() Attaching 'Copy to external' menu service...");
            MPX_TRAP( iErrorAttachCopyMenu, iServiceHandler->AttachMenuL( R_MPX_COLLECTION_VIEW_MENU_1,
                                                   R_MPX_UPNP_COPY_TO_EXTERNAL_MENU_INTEREST ) );
            if ( iErrorAttachCopyMenu == KErrNotSupported )
                {
                // when CCoeEnv is not available
                User::Leave( iErrorAttachCopyMenu );
                }
            MPX_DEBUG2( "CMPXCollectionViewHgImp::ConstructL(): attach Copy menu error: %d", iErrorAttachCopyMenu );
            }
        else
            {
            iUpnpFrameworkSupport = EFalse;
            iUpnpCopyCommand = NULL;
            }
        }
    iMediaRecognizer = CMediaRecognizer::NewL();
    if ( iServiceHandler == NULL)
        {
        iServiceHandler = CAiwServiceHandler::NewL();
        }
    MPX_DEBUG1("CMPXCollectionViewHgImp::ConstructL() Attaching 'use tone as' menu service...");
    MPX_TRAP( iErrorAttachAssignMenu, iServiceHandler->AttachMenuL( R_MPX_USE_AS_CASCADE,
                                               R_MPX_AIW_ASSIGN_INTEREST ) );


    TInt flags( 0 );
    CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
    repository->Get( KMPXMPLocalVariation, flags );
    delete repository;
    iGoToMusicShopOptionHidden =
        !static_cast<TBool>( flags & KMPXEnableGoToMusicShopOption );
    iUsingNokiaService =
        static_cast<TBool>( flags & KMPXEnableFindInMusicShopOption );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::ConstructL(): iUsingNokiaService: %d", iUsingNokiaService );
    iDisablePodcasting = flags&KMPXDisablePodcastingOption ? ETrue : EFalse;

#ifdef _DEBUG
    iExitOptionHidden = EFalse;
#else // _DEBUG
    iExitOptionHidden = iCommonUiHelper->ExitOptionHiddenL() && !iIsEmbedded;
#endif // _DEBUG

    iBottomIndex->AppendL( 0 );
    iProfileEngine = CreateProfileEngineL();

    _LIT_SECURITY_POLICY_C1(KMPlayerRemoteReadPolicy, ECapabilityReadUserData);
    _LIT_SECURITY_POLICY_C1(KMPlayerRemoteWritePolicy, ECapabilityWriteUserData);

    if (iGoToMusicShopOptionHidden)
        {
        iMusicStoreUID = 0;
        }
    else
        {
        // Get music store uid from cenrep
        //
        TBuf8< KUIDMaxLength > musicStoreUID;
        TRAP_IGNORE(
            {
            CRepository* repository = CRepository::NewL( KCRUidMPXMPSettings );
            repository->Get( KMPXMusicStoreUID, musicStoreUID );
            delete repository;
            repository = NULL;
            } );

        GetUint32Presentation( iMusicStoreUID, musicStoreUID, 0 );
        if (iMusicStoreUID == 0)
            {
            iGoToMusicShopOptionHidden = ETrue;
            }
        }
    MPX_DEBUG2("CMPXCollectionViewHgImp::ConstructL musicStoreUID = %x", iMusicStoreUID);

    TInt retval(KErrNone);
    // P/S key for music shop
    retval = RProperty::Define( TUid::Uid(iMusicStoreUID),
                                KMShopCategoryId,
                                RProperty::EInt,
                                KMPlayerRemoteReadPolicy,
                                KMPlayerRemoteWritePolicy );

    if( retval != KErrAlreadyExists)
        {
        RProperty::Set( TUid::Uid(iMusicStoreUID),
                        KMShopCategoryId,
                        KFindInMShopKeyInValid );  // initialize Find In Musicshop was not called


        RProperty::Define(  TUid::Uid(iMusicStoreUID),
                            KMShopCategoryName,
                            RProperty::ELargeText,
                            KMPlayerRemoteReadPolicy,
                            KMPlayerRemoteWritePolicy );
        }

    iCachedSelectionIndex = new ( ELeave )CArrayFixFlat<TInt>( KMPXArrayGranularity );
    iIncrementalOpenUtil = CMPXCollectionOpenUtility::NewL( this );

#ifdef BACKSTEPPING_INCLUDED
    // Initialize the Back Stepping Service Utility with the MPX Music Player
    iBackSteppingUtility = MMPXBackSteppingUtility::UtilityL();
    iBackSteppingUtility->InitializeL(
        TUid::Uid( KMusicPlayerAppUidConstant ) );
    iActivateBackStepping = EFalse;
#endif //BACKSTEPPING_INCLUDED

    iIsAddingToPlaylist = EFalse;
   
    //Get educating user URI and popup state from cenrep
    iEducateUserURI = HBufC16::NewL( KMPXMaxHistoryLength );
    TPtr16 educatingURI = iEducateUserURI->Des();
    iRepository = CRepository::NewL( KCRUidMPXMPSettings );
    iRepository->Get( KEducatingUserURI, educatingURI );
    iRepository->Get( KEducatingPopupState, iEducatingPopupState);
        
       // Get music store information from cenrep
   //
   TBuf8< KUIDMaxLength > operatorMusicStoreUID;

   iOperatorMusicStoreName = HBufC16::NewL( KMPXMaxHistoryLength );
   TPtr operatorMusicStoreMenuOption = iOperatorMusicStoreName->Des();

   HBufC16* musicStoreJavaName = HBufC16::NewLC( KMPXMaxHistoryLength );
   TPtr operatorMusicStoreJavaName = musicStoreJavaName->Des();

   TRAP_IGNORE(
    {
    CRepository* musicshoprepository = CRepository::NewL( KCRUidMPXMPSettings );
    musicshoprepository->Get( KOperatorMusicStore, iOperatorMusicStore );
    if(iOperatorMusicStore)
        {
        musicshoprepository->Get( KOperatorMusicStoreType, iOperatorMusicStoreType );
        musicshoprepository->Get( KOperatorMusicStoreDisplayName, operatorMusicStoreMenuOption );
        if (iOperatorMusicStoreType == KJavaMusicShopType)
            {
            musicshoprepository->Get( KOperatorMusicStoreJavaName, operatorMusicStoreJavaName );
            }
        else
            {
            musicshoprepository->Get( KOperatorMusicStoreNativeUid, operatorMusicStoreUID );
            GetUint32Presentation( iOperatorNativeMusicStoreUID, operatorMusicStoreUID, 0 );

            musicshoprepository->Get(KOperatorMusicStoreWebPage, iMusicStoreWebPage);
            iOperatorMusicStoreURI = HBufC16::NewL( KMPXMaxHistoryLength );
            TPtr16 operatorMusicStoreuri = iOperatorMusicStoreURI->Des();
            musicshoprepository->Get(KOperatorMusicStoreURI, operatorMusicStoreuri);

            }
        }
    delete musicshoprepository;
    musicshoprepository = NULL;
    } );

    if(iOperatorMusicStoreType == KJavaMusicShopType)
        {
            TApaAppInfo appInfo;
            RApaLsSession apaSession;
            User::LeaveIfError( apaSession.Connect() );
            CleanupClosePushL( apaSession );
            User::LeaveIfError( apaSession.GetAllApps() );

            while ( apaSession.GetNextApp( appInfo ) == KErrNone )
                {
                if(appInfo.iFullName.Right(8).Compare(_L(".fakeapp")) == 0)
                    {
                    TApaAppCaption appname;
                    appname = appInfo.iCaption ;
                    if (!appInfo.iCaption.Compare(operatorMusicStoreJavaName))
                        {
                        iOperatorMusicStoreUID = appInfo.iUid;
                        }
                    }
                }
            CleanupStack::PopAndDestroy(&apaSession); // close apaSession
        }
    //else
    //    {
    //    GetUint32Presentation( iMusicStoreUID, operatorMusicStoreUID, 0 );
    //    }
    CleanupStack::PopAndDestroy(musicStoreJavaName);

    // P/S key for usb unblocking
    RProperty::Define( KMPXViewPSUid,
            KMPXUSBUnblockingPSStatus,
            RProperty::EInt,
            KMPlayerRemoteReadPolicy,
            KMPlayerRemoteWritePolicy );

    TInt usbStatus;
    RProperty::Get(KPSUidUsbWatcher, KUsbWatcherSelectedPersonality, usbStatus);

    TInt mtpStatus = EMtpPSStatusUninitialized;
    RProperty::Get( KMtpPSUid, KMtpPSStatus, mtpStatus);
    
    MPX_DEBUG2("CMPXCollectionViewHgImp::ConstructL, mtpstatus = %d", mtpStatus);

    // Whenever usb/mtp is connected
    if ( usbStatus == KUsbPersonalityIdMS
                || usbStatus == KUsbPersonalityIdPTP
                || usbStatus == KUsbPersonalityIdPCSuite 
                || mtpStatus != EMtpPSStatusUninitialized )
        {
        RProperty::Set( KMPXViewPSUid,
        		        KMPXUSBUnblockingPSStatus,
                        EMPXUSBUnblockingPSStatusActive);
        }
    else
        {
        RProperty::Set( KMPXViewPSUid, 
        		        KMPXUSBUnblockingPSStatus,
                        EMPXUSBUnblockingPSStatusUninitialized );
        }
    } 

// ---------------------------------------------------------------------------
// Delete the selected items in TBone View
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DeleteSelectedTBoneItemsL(TInt aCommand)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DeleteSelectedTBoneItemsL" );
     // if reorder mode is on, or something is currently deleting, disable delete
    TBool isIgnore( iContainer->IsInReorderMode() || iIsDeleting );

    if ( !isIgnore  && iCollectionReady )
        {
        CMPXCommonListBoxArrayBase* listboxArray( iContainer->ListBoxArray() );

        HBufC* promptTxt( NULL );
        HBufC* waitNoteText( NULL );
        TInt waitNoteCBA( R_AVKON_SOFTKEYS_EMPTY );
        
        // Create a copy of collection path
        CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
        CleanupStack::PushL( path );

        if ( iContainer->IsTBoneView() )
            {
             //get the media object of the selected track in TBone View
            CMPXMedia* albumTrack = iContainer->SelectedItemMediaL();
            
            TMPXGeneralType mediaType(
                    albumTrack->ValueTObjectL<TMPXGeneralType>(
                            KMPXMediaGeneralType ) );
            TMPXGeneralCategory mediaCategory(
                    albumTrack->ValueTObjectL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory ) );

            if ( mediaType == EMPXItem && mediaCategory == EMPXSong )
                {
                // tracks level in Tbone View
                TMPXItemId trackId = albumTrack->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                const TDesC& trackTitle( albumTrack->ValueText( KMPXMediaGeneralTitle ) );
                // create the item path to delete

                if ( 2 == path->Levels() )
                    {
                    path->Back();
                    path->AppendL(3);
                    }
                 else if ( path->Levels() == 3 )
					{
					path->Back();
					}
				 else if (path->Levels() == 4)
					{
					path->Back();
					path->Back();
					}

                TInt currentIndex( iContainer->CurrentLbxItemIndex() );
                const CMPXMediaArray& albums = listboxArray->MediaArray();
                CMPXMedia* album( albums.AtL( currentIndex ) );
                const TMPXItemId albumId = album->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);

                path->AppendL(albumId);
                path->AppendL(trackId);
                 
                MPX_DEBUG_PATH(*path);

                waitNoteText = StringLoader::LoadLC(
                    R_MPX_QTN_ALBUM_WAITING_DELETING, trackTitle );
                promptTxt = StringLoader::LoadLC(
                    R_MPX_QTN_QUERY_COMMON_CONF_DELETE,
                    trackTitle );

                iConfirmationDlg = CAknQueryDialog::NewL(
                CAknQueryDialog::EConfirmationTone );

                waitNoteCBA = R_MPX_COLLECTION_WAITNOTE_SOFTKEYS_EMPTY_STOP;
                
                iConfirmationDlg->SetPromptL( *promptTxt );
                CleanupStack::PopAndDestroy( promptTxt );
                TBool performDelete(EFalse);
                if(iCachedCommand == aCommand)
                   {
                   performDelete = ETrue;
                   }
                if (!performDelete)
                    {
                    if ( iConfirmationDlg->ExecuteLD( R_MPX_CUI_DELETE_CONFIRMATION_QUERY ) )
                        {
                        performDelete = ETrue;
                        }
                    }
                if (performDelete)
                    {
                    HandleCommandL( EMPXCmdIgnoreExternalCommand );
                    MPX_PERF_START_EX( MPX_PERF_SHOW_WAITNOTE );
                    if(iCachedCommand != aCommand)
                        {
                        iIsWaitNoteCanceled = EFalse;
                        StartDeleteWaitNoteL();
                        TPtr buf = waitNoteText->Des();
                        UpdateProcessL( buf );
                        }
                    if ( !iIsWaitNoteCanceled )
                        {
                        iIsDeleting = ETrue;
                        iCollectionUiHelper->DeleteL( *path, this );
                        }
                    else if( iContainer )
                        {
                        // delete was canceled before it even began, clear marked items
                        iContainer->ClearLbxSelection();
                        }
                    iIsWaitNoteCanceled = EFalse;
        
                    if(iContainer->FindBoxVisibility())
                        {
                        iContainer->EnableFindBox(EFalse);
                        }

                    // delete songs list to update T-bone view after deleting a song
                    album->Delete( KMPXMediaArrayContents );
                    }
                iConfirmationDlg = NULL;
                CleanupStack::PopAndDestroy( waitNoteText );                
                }
            }
        CleanupStack::PopAndDestroy( path );
        }
    }


// ---------------------------------------------------------------------------
// Delete the selected items
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DeleteSelectedItemsL(TInt aCommand)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DeleteSelectedItemsL" );

    // if reorder mode is on, or something is currently deleting, disable delete
    TBool isIgnore( iContainer->IsInReorderMode() || iIsDeleting );

    CMPXCommonListBoxArrayBase* listboxArray(
        iContainer->ListBoxArray() );
    const CMPXMedia& containerMedia( listboxArray->ContainerMedia() );
    TInt currentIndex( iContainer->CurrentLbxItemIndex() );

    // Marked indicies
    // cannot use the caches indexes since this can be reached by pressing the cancel key
    const CArrayFix<TInt>* array (
        iContainer->CurrentSelectionIndicesL() ); // not owned
    TInt arrayCount( array->Count() );

    if ( !isIgnore )
        {
        if ( iContainer->CurrentListItemCount() == 0 )
            {
            // list is empty
            isIgnore = ETrue;
            }
        if ( !isIgnore &&
            containerMedia.IsSupported( KMPXMediaGeneralNonPermissibleActions ) )
            {
            TMPXGeneralNonPermissibleActions attr(
                containerMedia.ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions ) );
            if ( attr & EMPXWrite )
                {
                isIgnore = ETrue;
                }
            }
        if ( !isIgnore )
            {
            const CMPXMedia& media( listboxArray->MediaL( currentIndex ) );
            if ( ( arrayCount == 0 || arrayCount == 1 ) &&
                ( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId ) )
                {
                // not marked, or only 1 item is marked
                // and the highlighted item is not yet available
                isIgnore = ETrue;
                }
            else if ( media.IsSupported( KMPXMediaGeneralNonPermissibleActions ) )
                {
                TMPXGeneralNonPermissibleActions attr(
                    media.ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                        KMPXMediaGeneralNonPermissibleActions ) );
                if ( attr & EMPXWrite )
                    {
                    isIgnore = ETrue;
                    }
                }
            }
        }

    if ( !isIgnore )
        {
        // Create a copy of collection path
        CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
        CleanupStack::PushL( path );
        
        if ( 2 == path->Levels() )
             {
             path->Back();
             path->AppendL(3);
             
             const CMPXMediaArray& albums = listboxArray->MediaArray();
             
             RArray<TMPXItemId> ids;
             CleanupClosePushL(ids);

             TInt albumCount = albums.Count();
              for (TInt i=0; i<albumCount; ++i)
                  {
                  CMPXMedia* album = albums.AtL(i);
                  const TMPXItemId id = album->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                  ids.AppendL(id);
                  }
              path->AppendL(ids.Array()); // top level items
              ids.Reset();
              path->Set(currentIndex);
              CleanupStack::PopAndDestroy(&ids);             
             }
        MPX_DEBUG_PATH(*path);
        
        HBufC* promptTxt( NULL );
        HBufC* waitNoteText( NULL );
        TInt waitNoteCBA( R_AVKON_SOFTKEYS_EMPTY );
        MPX_DEBUG2( "CMPXCollectionViewHgImp::DeleteSelectedItemsL delete array count = %d", arrayCount );

        TMPXGeneralType containerType(
            containerMedia.ValueTObjectL<TMPXGeneralType>(
                KMPXMediaGeneralType ) );
        TMPXGeneralCategory containerCategory(
            containerMedia.ValueTObjectL<TMPXGeneralCategory>(
                KMPXMediaGeneralCategory ) );

        if ( arrayCount > 1 )
            {
            if ( containerType == EMPXItem && containerCategory == EMPXPlaylist )
                {
                // playlist view
                waitNoteText = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_NOTE_REMOVING_MANY );
                promptTxt = StringLoader::LoadLC(
                    R_MPX_QTN_MUS_QUERY_CONF_REMOVE_MANY,
                    arrayCount );
                }
            else
                {
                waitNoteText = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_DEL_SONGS_WAIT_NOTE );
                promptTxt = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_DEL_SONGS_QUERY,
                    arrayCount );
                }
            waitNoteCBA = R_MPX_COLLECTION_WAITNOTE_SOFTKEYS_EMPTY_STOP;
            iConfirmationDlg = CAknQueryDialog::NewL(
                CAknQueryDialog::EConfirmationTone );
            if ( iCoverDisplay )
                {
                if ( containerType == EMPXItem && containerCategory == EMPXPlaylist )
                    {
                    iConfirmationDlg->PublishDialogL(
                        EMPlayerQueryRemoveTracks,
                        KMPlayerNoteCategory);
                    iWaitNoteId = EMPlayerNoteRemovingMany;
                    }
                else
                    {
                    iConfirmationDlg->PublishDialogL(
                        EMPlayerQueryDeleteTracks,
                        KMPlayerNoteCategory);
                    iWaitNoteId = EMPlayerNoteDeletingMany;
                    }
                CAknMediatorFacade* covercl(
                    AknMediatorFacade( iConfirmationDlg ) );
                if ( covercl )
                    {
                    covercl->BufStream().WriteInt32L( arrayCount );
                    }
                }
            if(iCollectionReady || aCommand == EMPXCmdCommonDelete )
                {
            for ( TInt i = 0; i < arrayCount; i++ )
                {
                MPX_DEBUG2( "CMPXCollectionViewHgImp::DeleteSelectedItemsL delete array index = %d", array->At( i ) );
                path->SelectL( array->At(i) );
                    }
                }
            }
        else
            {
            // not marked, process the highlighted item
            const CMPXMedia& media( listboxArray->MediaL(
                ( arrayCount > 0 ) ? array->At( 0 ) : currentIndex ) );
            const TDesC& title( media.ValueText( KMPXMediaGeneralTitle ) );

            TMPXGeneralType type(
                media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
            TMPXGeneralCategory category(
                media.ValueTObjectL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory ) );

            if ( containerType == EMPXItem && containerCategory == EMPXPlaylist )
                {
                // playlist view
                waitNoteText = StringLoader::LoadLC(
                    R_MPX_QTN_MUS_NOTE_REMOVING_TRACK, title );
                promptTxt = StringLoader::LoadLC(
                    R_MPX_QTN_MUS_QUERY_CONF_REMOVE_TRACK,
                    title );
                }
            else if ( containerType == EMPXItem && containerCategory == EMPXArtist
                && type == EMPXItem && category == EMPXAlbum && currentIndex == 0 )
                {
                // deleting first entry in artist/album view
                const TDesC& containerTitle(
                    containerMedia.ValueText( KMPXMediaGeneralTitle ) );
                waitNoteText = StringLoader::LoadLC(
                    R_MPX_QTN_MUS_QUERY_CONF_DELETE_ALL, containerTitle );
                waitNoteCBA = R_MPX_COLLECTION_WAITNOTE_SOFTKEYS_EMPTY_STOP;
                promptTxt = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_QUERY_CONF_DELETE_GROUP,
                    containerTitle );
                }
            else
                {
                if ( ( type == EMPXItem && category == EMPXSong ) ||
                    ( type == EMPXItem && category == EMPXPlaylist ) )
                    {
                    // tracks level, or deleting a playlist
                    waitNoteText = StringLoader::LoadLC(
                        R_MPX_QTN_ALBUM_WAITING_DELETING, title );
                    promptTxt = StringLoader::LoadLC(
                        R_MPX_QTN_QUERY_COMMON_CONF_DELETE,
                        title );
                    }
                else
                    {
                    // deleting a group
                    if ( ( currentIndex == ( iContainer->CurrentListItemCount() - 1 ) ) &&
                        title.Compare( KNullDesC ) == 0 )
                        {
                        // check for unknown entry
                        // if it's the last entry, and it's null text
                        // load "unknown" text to display in prompt
                        waitNoteText = StringLoader::LoadLC(
                            R_MPX_QTN_MUS_QUERY_CONF_DELETE_ALL_UNKNOWN);
                        promptTxt = StringLoader::LoadLC(
                            R_MPX_QTN_NMP_QUERY_CONF_DELETE_GROUP_UNKNOWN);
                        }
                    else
                        {
                        waitNoteText = StringLoader::LoadLC(
                            R_MPX_QTN_MUS_QUERY_CONF_DELETE_ALL, title );
                        promptTxt = StringLoader::LoadLC(
                            R_MPX_QTN_NMP_QUERY_CONF_DELETE_GROUP, title );
                        }
                    waitNoteCBA = R_MPX_COLLECTION_WAITNOTE_SOFTKEYS_EMPTY_STOP;
                    }
                }
            iConfirmationDlg = CAknQueryDialog::NewL(
                CAknQueryDialog::EConfirmationTone );
            if ( iCoverDisplay )
                {
                if ( containerType == EMPXItem && containerCategory == EMPXPlaylist )
                    {
                    iConfirmationDlg->PublishDialogL(
                        EMPlayerQueryRemoveTrack,
                        KMPlayerNoteCategory);
                    iWaitNoteId = EMPlayerNoteRemovingTrack;
                    }
                else if ( containerType == EMPXItem && containerCategory == EMPXArtist
                    && type == EMPXItem && category == EMPXAlbum && currentIndex == 0 )
                    {
                    iConfirmationDlg->PublishDialogL(
                        EMPlayerQueryDeleteCategory,
                        KMPlayerNoteCategory);
                    iWaitNoteId = EMPlayerNoteDeletingSingle;
                    }
                else
                    {
                    if ( ( type == EMPXItem && category == EMPXSong ) ||
                        ( type == EMPXItem && category == EMPXPlaylist ) )
                        {
                        // tracks level
                        iConfirmationDlg->PublishDialogL(
                            EMPlayerQueryDeleteCategory,
                            KMPlayerNoteCategory);
                        iWaitNoteId = EMPlayerNoteDeletingSingle;
                        }
                    else
                        {
                        iConfirmationDlg->PublishDialogL(
                            EMPlayerQueryDeleteTrack,
                            KMPlayerNoteCategory);
                        iWaitNoteId = EMPlayerNoteDeletingSingle;
                        }
                    }
                CAknMediatorFacade* covercl(
                    AknMediatorFacade( iConfirmationDlg ) );
                if ( covercl )
                    {
                    covercl->BufStream() << title;
                    }
                iItemTitle.Set( title );
                }
//#endif //__COVER_DISPLAY
// Cover UI end
            if(iCollectionReady || aCommand == EMPXCmdCommonDelete )
                {
            path->SelectL( arrayCount > 0 ? array->At( 0 ) : currentIndex );
                }
            }
        iConfirmationDlg->SetPromptL( *promptTxt );
        CleanupStack::PopAndDestroy( promptTxt );
        TBool performDelete(EFalse);
        if(iCachedCommand == aCommand)
            {
            performDelete = ETrue;
            }
        if (!performDelete)
            {
        if ( iConfirmationDlg->ExecuteLD( R_MPX_CUI_DELETE_CONFIRMATION_QUERY ) )
                {
                performDelete = ETrue;
                }
            }
        if (performDelete)
            {
            HandleCommandL( EMPXCmdIgnoreExternalCommand );
// Cover UI start
//#ifdef __COVER_DISPLAY
        if ( iCoverDisplay )
            {
            InitiateWaitDialogL();
            }
//#endif // __COVER_DISPLAY
// Cover UI end
            MPX_PERF_START_EX( MPX_PERF_SHOW_WAITNOTE );
            if(iCachedCommand != aCommand)
                {
                iIsWaitNoteCanceled = EFalse;
                StartDeleteWaitNoteL();
				TPtr buf = waitNoteText->Des();
				UpdateProcessL( buf );
                }

                if ( !iIsWaitNoteCanceled )
                    {
                    iIsDeleting = ETrue;
                    iCollectionUiHelper->DeleteL( *path, this );
                    iFirstIndexOnScreen = iContainer->FirstIndexOnScreen();               
                    }
                else if( iContainer )
                    {
                    // delete was canceled before it even began, clear marked items
                    iContainer->ClearLbxSelection();
                    }
                iIsWaitNoteCanceled = EFalse;


            if(iContainer->FindBoxVisibility())
                {
                iContainer->EnableFindBox(EFalse);
                }
            }
        iConfirmationDlg = NULL;
        CleanupStack::PopAndDestroy( waitNoteText );
        CleanupStack::PopAndDestroy( path );
        }
    }

// ---------------------------------------------------------------------------
// Initiates wait dialog
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::InitiateWaitDialogL()
    {
    if ( iCommandInitiator )
        {
        HBufC8* filename8 = HBufC8::NewLC( KMaxFileName );
        filename8->Des().Copy( iItemTitle );
        HBufC8* data = HBufC8::NewLC(KMaxFileName);

        TPtr8 dataPtr = data->Des();
        RDesWriteStream str(dataPtr);

        TPtrC8 b(KNullDesC8);
        b.Set(*filename8);

        str << b;
        str.CommitL();

        iCommandInitiator->IssueCommand( KMediatorSecondaryDisplayDomain,
                                    KMPlayerNoteCategory,
                                    iWaitNoteId,
                                    TVersion ( 0,0,0 ),
                                    *data );

        CleanupStack::PopAndDestroy( data );
        CleanupStack::PopAndDestroy( filename8 );
        }
    }

// ---------------------------------------------------------------------------
// Updates list box
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::UpdateListBoxL(
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool aComplete)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::UpdateListBox" );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::UpdateListBox aIndex = %d", aIndex );
    if ( iContainer )
        {
        CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
        MPX_DEBUG_PATH(*cpath);

        CleanupStack::PushL( cpath );

        TInt topIndex = 0;
        if (iContainer->IsInReorderMode())
            {
            TInt currentItem( iContainer->CurrentLbxItemIndex() );
            TInt currentBottomIndex( iContainer->BottomLbxItemIndex() );
            topIndex = iContainer->CalculateTopIndex( currentBottomIndex ) ;
            }

        CMPXCommonListBoxArrayBase* array( iContainer->ListBoxArray() );
        array->ResetMediaArrayL();
        array->AppendMediaL( aEntries );
        iCollectionCacheReady = ETrue;
        //keep the marked item indicies
        const CArrayFix<TInt>* markedList = iContainer->CurrentSelectionIndicesL();
		if ( iFirstIncrementalBatch )
            {
            iContainer->HandleLbxItemAdditionL();
            iFirstIncrementalBatch = EFalse;
            }
		else
			{
			iContainer->HandleListBoxArrayEventL(
			MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventMediaArrayChange );
			}

        if (aComplete)
            {
            iFirstIncrementalBatch = ETrue;
            }
        //mark all again after list box item addition
        if (iMarkedAll)
            {
            iContainer->HandleMarkableListProcessCommandL(EAknMarkAll);
            if (aComplete)
                {
                iMarkedAll = EFalse;
                }
            }
		   //mark again individual items after list box item addition
        else if ( markedList )
            {
		       TInt markedItemCount( markedList->Count() );
		       if ( markedItemCount >0 )
                {
                iContainer->SetCurrentSelectionIndicesL( const_cast<CArrayFix<TInt>*>(markedList) );
                }
            }
        UpdatePlaybackStatusL();

        if ( iBackOneLevel || iPossibleJump )
            {
            if ( cpath->Levels() == iLastDepth )
                {
                iLastDepth--;
                iContainer->SetLbxTopItemIndex(
                    iContainer->CalculateTopIndex(
                    iBottomIndex->At( iLastDepth ) ) );
                iContainer->SetLbxCurrentItemIndexAndDraw( aIndex );
                iBottomIndex->Delete( iLastDepth );
                }
            else
                {
                MPX_DEBUG1( "CMPXCollectionViewHgImp::UpdateListBox Invalid history, rebuilding" );
                // invalid path, rebuild with all 0. possibily caused by a jump in views
                iBottomIndex->Reset();
                iLastDepth = cpath->Levels();
                for ( TInt i = 0; i < iLastDepth; i++ )
                    {
                    iBottomIndex->AppendL( 0 );
                    }
                iContainer->SetLbxCurrentItemIndexAndDraw( aIndex );
                }
            iPossibleJump = EFalse;
            iBackOneLevel = EFalse;
            }
        else
            {
            if ( iAddingSong )
                {
                // always highlight the last item when a track
                // is added in add tracks view
                TInt currentItemCount =
                    iContainer->CurrentListItemCount();
                if ( currentItemCount > 0 )
                    {
                    iContainer->
                        SetLbxCurrentItemIndexAndDraw( currentItemCount - 1 );
                    }
                }
            else if ( iCurrentHighlightedIndex > KErrNotFound &&
                ( iCurrentHighlightedIndex <
                iContainer->CurrentListItemCount() ) )
                {
                if (iContainer->IsInReorderMode())
                    {
                    iContainer->SetLbxTopItemIndex( topIndex );
                    }
                iContainer->
                    SetLbxCurrentItemIndexAndDraw( iCurrentHighlightedIndex );
                iCollectionUtility->Collection().CommandL(
                    EMcCmdSelect, iCurrentHighlightedIndex );
                iCurrentHighlightedIndex = KErrNotFound;
                }
            else if ( ( aIndex > 0 )
                && ( aIndex < iContainer->CurrentListItemCount() ) )
                {
				iContainer->SetLbxCurrentItemIndexAndDraw( aIndex );
                }
            else
                {
                iContainer->SetLbxCurrentItemIndexAndDraw( 0 );
                }
            }
        if (!aComplete || cpath->Levels() == KMusicCollectionMenuLevel || iContainer->IsInReorderMode())
            {
            iContainer->EnableFindBox(EFalse);
            }
        else
            {
            iContainer->EnableFindBox(ETrue);
            }

        CleanupStack::PopAndDestroy( cpath );
        }
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleError( TInt aError )
    {
    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleError(%d)", aError );
    ASSERT( aError );

    // only display error message if collection view is in the foreground
    if ( iContainer )
        {
        TRAP_IGNORE( iCommonUiHelper->HandleErrorL( aError ) );
        }
    }

// ---------------------------------------------------------------------------
// Updates the title pane
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::UpdateTitlePaneL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::UpdateTitlePaneL" );
    // Set title

    if ( iViewUtility->ActiveViewType() ==
        TUid::Uid( KMPXPluginTypeCollectionUid ) ||
        ( iViewUtility->ActiveViewType() ==
         TUid::Uid( KMPXPluginTypeWaitNoteDialogUid ) && iContainer ) )
        {
        CAknTitlePane* title( NULL );

        TRAP_IGNORE(
            {
            title = static_cast<CAknTitlePane*>
                ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
            } );

        if ( title )
            {
            if ( iTitle )
                {
                title->SetTextL( *iTitle );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates playback status indicator
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgImp::UpdatePlaybackStatusL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::UpdatePlaybackStatusL" );
    TInt ret( KErrNotFound );  // default selection
    if ( iContainer )
        {
        TMPXPlaybackState pbState( EPbStateNotInitialised );
        TInt selectedIndex( KErrNotFound );
        TMPXItemId selectedItemId( KMPXInvalidItemId );
        CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
        CleanupStack::PushL( cpath );

        MMPXSource* source( iPlaybackUtility->Source() );
        if ( source )
            {
            CMPXCollectionPlaylist* playlist( source->PlaylistL() );
            if ( playlist )
                {
                CleanupStack::PushL( playlist );
                CMPXCollectionPath* pbPath(
                    CMPXCollectionPath::NewL( playlist->Path() ) );
                CleanupStack::PushL( pbPath );
                TInt playbackPathCount( pbPath->Levels() );
                if ( cpath->Levels() == playbackPathCount )
                    {
                    // not comparing the index
                    TBool isEqual( ETrue );
                    for ( TInt i = 0; i < playbackPathCount - 1; i++ )
                        {
                        if ( cpath->Id( i ) != pbPath->Id( i ) )
                            {
                            isEqual = EFalse;
                            break;
                            }
                        }
                    if ( isEqual )
                        {
                        selectedIndex = pbPath->Index();
                        selectedItemId = pbPath->Id();
                        ret = selectedIndex;
                        pbState = iPlaybackUtility->StateL();
                        }
                    }
                CleanupStack::PopAndDestroy( pbPath );
                CleanupStack::PopAndDestroy( playlist );
                }
            }
        CleanupStack::PopAndDestroy( cpath );
        if ( selectedIndex != KErrNotFound )
            {
            if ( iContainer->IsInReorderMode() )
                {
                // in reorder mode, need to pass unique ID to list box
                // to search for now playing song
                iContainer->SetPlaybackStatusByIdL( selectedItemId, pbState );
                }
            else
                {
                // Fixed for EBLI-7AG8ZN, the Playlist in the Engine is updated
                // later than UI (because of Inc Open), so index does not work
                iContainer->SetPlaybackStatusByIdL( selectedItemId, pbState, selectedIndex );
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Start a refreshing note
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::StartWaitNoteL( TWaitNoteType aNoteType )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::StartWaitNoteL" );
    TUid waitnoteId = TUid::Uid( KMPXPluginTypeWaitNoteDialogUid );
    TUid activeView = iViewUtility->ActiveViewType();
    if( !iIsEmbedded && activeView != waitnoteId )
        {
        TPckg<TWaitNoteType> note = aNoteType;
        HBufC* arg = MPXUser::AllocL( note );
        CleanupStack::PushL( arg );
        RProcess proc;
        iPriority = proc.Priority();
        proc.SetPriority( EPriorityHigh );
        iNoteType = aNoteType;
        iViewUtility->ActivateViewL( waitnoteId, arg );
        CleanupStack::PopAndDestroy( arg );
        }
    }

// ---------------------------------------------------------------------------
// Start a Wait note
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::StartDeleteWaitNoteL()
    {
    iWaitDialog = new (ELeave) CAknWaitDialog(
        (REINTERPRET_CAST(CEikDialog**, &iWaitDialog)),
        ETrue);
    iWaitDialog->PrepareLC(R_MPX_WAIT_NOTE);

    iWaitDialog->SetCallback(this);
    iWaitDialog->RunLD();
    }

// ---------------------------------------------------------------------------
// Update the Progress note
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::UpdateProcessL( const TDesC& aProgressText )
    {
    if ( iWaitDialog )
        {
        iWaitDialog->SetTextL(aProgressText);
        }
    }

// ---------------------------------------------------------------------------
// Close waitnote dialog
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::CloseWaitNoteL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::CloseWaitNoteL" );
    if( !iUSBOnGoing && !iIsEmbedded)
        {
        RProcess proc;
        proc.SetPriority( iPriority );
        // Fix for ESLU-7CFEPF, try to close the WaitNote even if it's not the current view
        TRAP_IGNORE(
            CMPXViewPlugin* pi =
                iViewUtility->ViewPluginManager().PluginL( TUid::Uid(KWaitNoteImpUid) );
            pi->DeactivateView();
            );
        }
    }


// -----------------------------------------------------------------------------
// Change the navi pane
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::UpdateReorderNaviPaneL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::UpdateReorderNaviPaneL" );
    delete iDuration;
    iDuration = NULL;

    // Set playlist counter text to pos/size by default
    CArrayFixFlat<TInt>* params =
        new ( ELeave ) CArrayFixFlat<TInt>( KMPXReorderNaviPaneGranularity );
    CleanupStack::PushL( params );
    // Position of selection in index
    params->AppendL( iContainer->CurrentLbxItemIndex() + 1 );
    // Total Number of items in lbx
    params->AppendL( iContainer->TotalListItemCount() );
    iDuration = StringLoader::LoadL(
        R_MPX_CUI_POSITION_COUNTER_TXT, *params );
    CleanupStack::PopAndDestroy( params );
    }

// ---------------------------------------------------------------------------
// Activates reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::ActivateReorderGrabbedModeL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::ActivateReorderGrabbedModeL" );
    iIsGrabbed = ETrue;
    TInt currentItem( iContainer->CurrentLbxItemIndex() );
    iContainer->EnableFindBox( EFalse );
    iContainer->SetReorderGrabbedMode( ETrue, currentItem );
	// mark the grabbed item
    iContainer->MarkGrabedItemL( currentItem );
    if ( !iContainer->IsInReorderMode() )
        {
        iContainer->UpdateReorderTitleIconL();

        if ( iTitle )
            {
            delete iOriginalTitle;
            iOriginalTitle = NULL;
            iOriginalTitle = iTitle->AllocL();
            delete iTitle;
            iTitle = NULL;
            }
        iTitle = StringLoader::LoadL( R_QTN_NMP_TITLE_REORDER_LIST );
        UpdateTitlePaneL();

        if ( iDuration )
            {
            delete iOriginalDuration;
            iOriginalDuration = NULL;
            iOriginalDuration = iDuration->AllocL();
            delete iDuration;
            iDuration = NULL;
            }
        iContainer->ActivateReorderMode( ETrue );
        SetNewCbaL( R_MPX_CUI_REORDER_CANCEL_CBA );
        }
    iContainer->SetLbxCurrentItemIndexAndDraw( currentItem );
    UpdateReorderNaviPaneL();
    }

// ---------------------------------------------------------------------------
// Deactivates reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DeactivateReorderGrabbedModeL( TBool aExit )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DeactivateReorderGrabbedModeL" );
    iIsGrabbed = EFalse;
    delete iCurrentCba;
    iCurrentCba = NULL;

    iContainer->SetReorderGrabbedMode( EFalse );
    if ( aExit )
        {
        CEikButtonGroupContainer* cba = Cba();
        if ( cba )
            {
            cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
            cba->DrawDeferred();

#ifdef __ENABLE_MSK
            // set middle softkey label display
            UpdateMiddleSoftKeyDisplayL( R_QTN_MSK_PLAY );
#endif // __ENABLE_MSK

            }

        iContainer->RestoreOriginalTitleIconL();
        delete iTitle;
        iTitle = NULL;
        if ( iOriginalTitle )
            {
            iTitle = iOriginalTitle->AllocL();
            delete iOriginalTitle;
            iOriginalTitle = NULL;
            }
        UpdateTitlePaneL();

        delete iDuration;
        iDuration = NULL;
        if ( iOriginalDuration )
            {
            iDuration = iOriginalDuration->AllocL();
            delete iOriginalDuration;
            iOriginalDuration = NULL;
            }
        iContainer->ActivateReorderMode( EFalse );
        iContainer->EnableFindBox( ETrue );
        }
    iHandlingKeyEvent = ETrue;
    iContainer->HandleLbxItemAdditionPreserveIndexL();
    }

// -----------------------------------------------------------------------------
// Change the button group
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::SetNewCbaL( TInt aResId )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SetNewCbaL" );
    if ( !iCurrentCba )
        {
        iCurrentCba = CEikButtonGroupContainer::NewL(
            CEikButtonGroupContainer::ECba,
            CEikButtonGroupContainer::EHorizontal,
            this,
            aResId, *iContainer );
        }
    else
        {
        iCurrentCba->SetCommandSetL( aResId );
        iCurrentCba->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// Display the details dialog
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DisplayDetailsDialogL( MDesC16Array& aDataArray,
    TInt aDialogResourceId, TInt aTitleResourceId )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DisplayDetailsDialogL" );
    CAknSingleHeadingPopupMenuStyleListBox* list =
        new ( ELeave ) CAknSingleHeadingPopupMenuStyleListBox;
    CleanupStack::PushL( list );
    CAknPopupList* popupList = CAknPopupList::NewL(
        list, R_AVKON_SOFTKEYS_OK_EMPTY,
        AknPopupLayouts::EMenuGraphicHeadingWindow);
    CleanupStack::PushL( popupList );
    list->ConstructL( popupList, EAknListBoxViewerFlags );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto);

    // Enable Marquee
    static_cast<CEikFormattedCellListBox*>( list )->ItemDrawer()->ColumnData()->
        EnableMarqueeL( ETrue );

    // Retrieve heading array
    CDesCArrayFlat* headingsArray(
        iCoeEnv->ReadDesCArrayResourceL( aDialogResourceId ) );
    CleanupStack::PushL( headingsArray );

    // Item array combines heading array and data array
    CDesCArrayFlat* itemArray =
        new ( ELeave ) CDesCArrayFlat( headingsArray->Count() );
    CleanupStack::PushL( itemArray );

    TInt count = headingsArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        HBufC* item = HBufC::NewLC(
            headingsArray->MdcaPoint( i ).Length() +
            aDataArray.MdcaPoint( i ).Length() +
            KMPXDurationDisplayResvLen );

        TPtrC tempPtr1 = headingsArray->MdcaPoint( i );
        TPtrC tempPtr2 = aDataArray.MdcaPoint( i );

        item->Des().Format( KMPXCollDetailsItemsFormat,
                            &tempPtr1,
                            &tempPtr2 );

        itemArray->AppendL( *item );
        CleanupStack::PopAndDestroy( item );
        }

    CleanupStack::Pop( itemArray );
    CleanupStack::PopAndDestroy( headingsArray );

    // Set list items
    CTextListBoxModel* model = list->Model();
    model->SetOwnershipType( ELbmOwnsItemArray );
    model->SetItemTextArray( itemArray );

    // Set title
    HBufC* title = StringLoader::LoadLC( aTitleResourceId );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    // Show popup list
    CleanupStack::Pop( popupList );
    popupList->ExecuteLD();
    CleanupStack::PopAndDestroy( list );
    }

// -----------------------------------------------------------------------------
// Display collection details
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoShowCollectionDetailsL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::ShowCollectionDetailsL" );
    CDesCArrayFlat* dataArray =
        new ( ELeave ) CDesCArrayFlat( EMPXCollectionDetailsCount );
    CleanupStack::PushL( dataArray );

    // number of items
    HBufC* dataToAppend = HBufC::NewLC( KMPXMaxBufferLength );
    TInt songsCount( 0 );
    if ( aMedia.IsSupported( KMPXMediaColDetailNumberOfItems ) )
        {
        songsCount = aMedia.ValueTObjectL<TInt>( KMPXMediaColDetailNumberOfItems );
        }
    TPtr ptr( dataToAppend->Des() );
    ptr.AppendNum( songsCount );
    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    // total duration
    TInt duration( 0 );
    if ( aMedia.IsSupported( KMPXMediaColDetailDuration ) )
        {
        duration = aMedia.ValueTObjectL<TInt>( KMPXMediaColDetailDuration );
        }
    if ( duration > 0 )
        {
        dataToAppend = iCommonUiHelper->DisplayableDurationInTextL( duration/KMilliSecondsToSeconds );
        CleanupStack::PushL( dataToAppend );
        ptr.Set( dataToAppend->Des() );
        AknTextUtils::LanguageSpecificNumberConversion( ptr );
        dataArray->AppendL( ptr );
        CleanupStack::PopAndDestroy( dataToAppend );
        }
    else
        {
        dataArray->AppendL( KNullDesC );
        }

#ifdef RD_MULTIPLE_DRIVE
    RFs& fileSession( iCoeEnv->FsSession() );
    TDriveList driveList;
    TInt driveCount(0);
    TInt64 freePhoneMemory(KErrNotFound);
    TInt64 freeInternalMemory(KErrNotFound);
    TInt64 freeExternalMemory(KErrNotFound);
    TVolumeInfo volInfo;

    // Get all visible drives
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives(
           fileSession, driveList, driveCount ) );
    MPX_DEBUG2 ("CMPXCollectionViewHgImp::ShowCollectionDetailsL - driveCount = %d", driveCount);

    for( TInt driveNum = EDriveA; driveNum <= EDriveZ; driveNum++ )
        {
        if (driveList[driveNum])
            {
            // Get the volume information
            if ( fileSession.Volume( volInfo, driveNum ) == KErrNone )
                {
                // Get the drive status
                TUint driveStatus(0);
                DriveInfo::GetDriveStatus( fileSession, driveNum, driveStatus );

                // Add up free memory depending on memory location
                if ( driveNum == EDriveC )
                    {
                    if ( freePhoneMemory == KErrNotFound )
                        {
                        freePhoneMemory = 0;
                        }
                    freePhoneMemory += volInfo.iFree;
                    }
                else if ( driveStatus & DriveInfo::EDriveInternal )
                    {
                    if ( freeInternalMemory == KErrNotFound )
                        {
                        freeInternalMemory = 0;
                        }
                    freeInternalMemory += volInfo.iFree;
                    }
                else if ( driveStatus &
                    (DriveInfo::EDriveRemovable | DriveInfo::EDriveRemote) )
                    {
                    if ( freeExternalMemory == KErrNotFound )
                        {
                        freeExternalMemory = 0;
                        }
                    freeExternalMemory += volInfo.iFree;
                    }
                }
            }
        }

    // phone memory free
    if ( freePhoneMemory != KErrNotFound )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(
            freePhoneMemory, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr);
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_MEMORY_UNAVAILABLE );
        ptr.Set(dataToAppend->Des());
        }

    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    // internal memory (mass storage) free
    if ( freeInternalMemory != KErrNotFound )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(
            freeInternalMemory, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr);
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_MEMORY_UNAVAILABLE );
        ptr.Set(dataToAppend->Des());
        }

    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    // removable/remote (memory card) memory free
    if ( freeExternalMemory != KErrNotFound )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(
            freeExternalMemory, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr);
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_CARD_UNAVAILABLE_ITEM );
        ptr.Set(dataToAppend->Des());
        }
    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );
#else
    // phone memory free
    HBufC* driveLetter = StringLoader::LoadLC( R_MPX_COLLECTION_PHONE_MEMORY_ROOT_PATH );
    RFs& fileSession( iCoeEnv->FsSession() );
    TVolumeInfo volInfo;
    TDriveUnit driveUnit( *driveLetter ); // Get the drive
    CleanupStack::PopAndDestroy( driveLetter );
    if ( KErrNone == fileSession.Volume( volInfo, ( TInt )driveUnit ) )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL( volInfo.iFree, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr);
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_CARD_UNAVAILABLE_ITEM );
        ptr.Set(dataToAppend->Des());
        }
    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    // memory card
    driveLetter = StringLoader::LoadLC( R_MPX_COLLECTION_MEMORY_CARD_ROOT_PATH );
    driveUnit = *driveLetter; // Get the drive
    CleanupStack::PopAndDestroy( driveLetter );
    if ( fileSession.Volume( volInfo, ( TInt )driveUnit ) == KErrNone )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL( volInfo.iFree, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr);
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_CARD_UNAVAILABLE_ITEM );
        ptr.Set(dataToAppend->Des());
        }
    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );
#endif // RD_MULTIPLE_DRIVE

    // last refreshed
    TInt64 lastRefreshed( 0 );
    if ( aMedia.IsSupported( KMPXMediaColDetailLastRefreshed ) )
        {
        lastRefreshed = aMedia.ValueTObjectL<TInt64>( KMPXMediaColDetailLastRefreshed );
        }
    TTime time( lastRefreshed );
    dataToAppend = HBufC::NewLC(
        KMPXMaxTimeLength + KMPXDurationDisplayResvLen );
    HBufC* format( StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO ) );
    TPtr modDatePtr( dataToAppend->Des() );
    time.FormatL( modDatePtr, *format );
    CleanupStack::PopAndDestroy( format );
    AknTextUtils::LanguageSpecificNumberConversion( modDatePtr );
    dataArray->AppendL( modDatePtr );
    CleanupStack::PopAndDestroy( dataToAppend );

    DisplayDetailsDialogL( *dataArray,
        R_MPX_CUI_COLLECTION_DETAILS_HEADINGS,
        R_MPX_QTN_NMP_TITLE_COLLECTION_DETAILS );
    CleanupStack::PopAndDestroy( dataArray );
    }

// -----------------------------------------------------------------------------
// Display playlist details
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoShowPlaylistDetailsL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::ShowPlaylistDetailsL" );
    CDesCArrayFlat* dataArray =
        new ( ELeave ) CDesCArrayFlat( EMPXPlaylistDetailsCount );
    CleanupStack::PushL( dataArray );

    // playlist name
    if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
        {
        dataArray->AppendL( aMedia.ValueText( KMPXMediaGeneralTitle ) );
        }
    else if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
        {
        TParsePtrC fileDetail( aMedia.ValueText( KMPXMediaGeneralUri ) );
        dataArray->AppendL( fileDetail.Name() );
        }
    else
        {
        dataArray->AppendL( KNullDesC );
        }

    // number of songs
    TInt songsCount( 0 );
    if ( aMedia.IsSupported( KMPXMediaGeneralCount ) )
        {
        songsCount = aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralCount );
        }
    HBufC* dataToAppend = HBufC::NewLC( KMPXMaxBufferLength );
    TPtr ptr( dataToAppend->Des() );
    ptr.AppendNum( songsCount );
    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    // Duration
    TInt duration( 0 );
    if ( aMedia.IsSupported( KMPXMediaGeneralDuration ) )
        {
        duration = aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralDuration );
        }
    if ( duration > 0 )
        {
        dataToAppend = iCommonUiHelper->DisplayableDurationInTextL( duration/KMilliSecondsToSeconds );
        CleanupStack::PushL( dataToAppend );
        ptr.Set( dataToAppend->Des() );
        AknTextUtils::LanguageSpecificNumberConversion( ptr );
        dataArray->AppendL( ptr );
        CleanupStack::PopAndDestroy( dataToAppend );
        }
    else
        {
        dataArray->AppendL( KNullDesC );
        }

    // Location
    TUint flags( aMedia.ValueTObjectL<TUint>( KMPXMediaGeneralFlags ) );
    TUint isVirtual( ( flags ) & ( KMPXMediaGeneralFlagsIsVirtual ) );
    if ( !isVirtual )
        {
        if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
            {
            dataArray->AppendL( aMedia.ValueText( KMPXMediaGeneralUri ) );
            }
        else
            {
            dataArray->AppendL( KNullDesC );
            }
        }

    // last refreshed
    TInt64 lastMod( 0 );
    if ( aMedia.IsSupported( KMPXMediaGeneralDate ) )
        {
        lastMod = aMedia.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
        }
    TTime time( lastMod );
    dataToAppend = HBufC::NewLC(
        KMPXMaxTimeLength + KMPXDurationDisplayResvLen );
    HBufC* format = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
    ptr.Set( dataToAppend->Des() );
    time.FormatL( ptr, *format );
    CleanupStack::PopAndDestroy( format );
    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    dataToAppend = HBufC::NewLC(
        KMPXMaxTimeLength + KMPXDurationDisplayResvLen );
    format = StringLoader::LoadLC( R_QTN_TIME_USUAL );
    ptr.Set( dataToAppend->Des() );
    time.FormatL( ptr, *format );
    CleanupStack::PopAndDestroy( format );
    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    TInt headingResource( isVirtual ?
        R_MPX_CUI_PLAYLIST_DETAILS_HEADINGS_WITHOUT_URI :
        R_MPX_CUI_PLAYLIST_DETAILS_HEADINGS_WITH_URI );
    DisplayDetailsDialogL( *dataArray, headingResource,
        R_MPX_QTN_NMP_TITLE_PLAYLIST_DETAILS );
    CleanupStack::PopAndDestroy( dataArray );
    }


// ---------------------------------------------------------------------------
// Handles Upnp menus from DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleDynInitUpnpL(
    TInt aResourceId,
    CEikMenuPane& aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleDynInitUpnpL()" );

    if ( !IsUpnpVisibleL() )
        {
        GetCurrentPlayerDetails();

        if ( iCurrentPlayerType == EPbLocal )
            {
            aMenuPane.SetItemDimmed(
                EMPXCmdUpnpPlayVia,
                ETrue);
            }
        aMenuPane.SetItemDimmed(
            EMPXCmdUPnPAiwCmdCopyToExternalCriteria,
            ETrue);
        }
    else
        {
        MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleDynInitUpnpL(): UPnP visible and media" );
        TInt currentItem( iContainer->CurrentLbxItemIndex() );

        const CMPXMedia& media =
            (iContainer->ListBoxArray())->MediaL( currentItem );
        TMPXGeneralType type( EMPXNoType );
        if (media.IsSupported(KMPXMediaGeneralType))
            {
            type =
                media.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);
            }

        if ( ( type == EMPXItem ) ||
             ( type == EMPXGroup ) )
            {
            MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleDynInitUpnpL(): folder or song" );
            if ( iErrorAttachCopyMenu != KErrNone )
                {
                MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleDynInitUpnpL(): error, Dim Copy" );
                aMenuPane.SetItemDimmed(
                    EMPXCmdUPnPAiwCmdCopyToExternalCriteria,
                    ETrue);
                }
            else
                {
                if (iServiceHandler->HandleSubmenuL(aMenuPane))
                    {
                    return;
                    }

                if ( iServiceHandler->IsAiwMenu(aResourceId))
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleDynInitUpnpL(): Aiw menu" );
                    CAiwGenericParamList& in = iServiceHandler->InParamListL();

                    iServiceHandler->InitializeMenuPaneL(aMenuPane, aResourceId, EMPXCmdUPnPAiwCmdCopyToExternalCriteria, in);
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleDynInitUpnpL(): after InitializeMenuPaneL" );
                    }
                }
            }
         else
            {
            aMenuPane.SetItemDimmed(
                EMPXCmdUpnpPlayVia,
                ETrue);
            aMenuPane.SetItemDimmed(
                EMPXCmdUPnPAiwCmdCopyToExternalCriteria,
                ETrue);
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::IsUpnpVisible
// Checks if UPnP access point is defined
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgImp::IsUpnpVisibleL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::IsUpnpVisibleL" );
 	  TBool returnValue = EFalse;

   	if ( iUpnpCopyCommand  && iUpnpFrameworkSupport)
        {
        returnValue = iUpnpCopyCommand->IsAvailableL();
        }
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::AddPlayersNamesToMenuL
// Taken from Gallery upnp support implementation
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::AddPlayersNamesToMenuL( CEikMenuPane& aMenuPane )
    {
    RArray<TMPXPlaybackPlayerType> playerTypes;
    CleanupClosePushL(playerTypes);

    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
    manager.GetPlayerTypesL(playerTypes);

    GetCurrentPlayerDetails();

    TInt countPlayersTypes( playerTypes.Count() );
    TInt countLocalType(0);
    TInt countRemoteType(0);

    if ( countPlayersTypes >= 1 )
        {
        iPlayersList.Close();

        for ( TInt i = 0; i < countPlayersTypes; i++ )
            {
            MPX_DEBUG3( "playerTypes[%d]: %d", i, playerTypes[i] );
            switch (playerTypes[i])
                {
                case EPbLocal:
                    {
                    // we only show 1 specific type once in the menu
                    if ( countLocalType == 0 )
                        {
                        AddPlayerNameToMenuL( aMenuPane,
                                            EMPXCmdUpnpPlayViaLocal,
                                            manager,
                                            playerTypes[i] );

                        countLocalType++;
                        }
                    break;
                    }
                case EPbRemote:
                    {
                    // we only show 1 specific type once in the menu
                    if ( countRemoteType == 0 )
                        {
                        AddPlayerNameToMenuL( aMenuPane,
                                            EMPXCmdUpnpPlayViaRemotePlayer,
                                            manager,
                                            playerTypes[i] );
                        countRemoteType++;
                        }
                    break;
                    }
                default:
                    {
                    // default case is handled in the next loop, we want
                    // to add any "other" player at the end of the list
                    break;
                    }
                }
            } // for loop

        for ( TInt i = 0; i < countPlayersTypes; i++ )
            {
            MPX_DEBUG3( "playerTypes[%d]: %d", i, playerTypes[i] );
            // EPbLocal and EPbRemote were already added to the
            // submenu in the previous loop
            if ( playerTypes[i] != EPbLocal &&
                 playerTypes[i] != EPbRemote )
                {
                // EPbUnknown or else
                TBool alreadyInTheList( EFalse );

                for ( TInt j = 0; j < i; j++)
                    {
                    // if the new playertype is already in the list
                    if ( playerTypes[i] == playerTypes[j] )
                        {
                        alreadyInTheList = ETrue;
                        break;
                        }
                    }

                if ( !alreadyInTheList )
                    {
                    HBufC* buf = manager.PlayerTypeDisplayNameL(playerTypes[i]);

                    if ( buf )
                        {
                        CleanupStack::PushL(buf);
                        AddPlayerNameToMenuL( aMenuPane,
                                            EMPXCmdUpnpPlayViaRemotePlayer,
                                            manager,
                                            playerTypes[i],
                                            *buf );
                        CleanupStack::PopAndDestroy(buf);
                        }
                    // else we don't do anything. other player type is not
                    // currently supported. Implementation will be finalized
                    // when the requirement is defined.
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(&playerTypes);

    if ( iPlayersList.Count() <= 1 )
        {
        aMenuPane.SetItemDimmed( EMPXCmdUpnpPlayViaRemotePlayer,
                                  ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::AddPlayerNameToMenuL
// Taken from Gallery upnp support implementation
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::AddPlayerNameToMenuL( CEikMenuPane& aMenuPane,
                                                TInt aCommandId,
                                                MMPXPlayerManager& aPlayerManager,
                                                TMPXPlaybackPlayerType& aPlayerType,
                                                const TDesC& aMenuText )
    {
    RArray<TUid> players;
    CleanupClosePushL(players);
    aPlayerManager.GetPlayerListL(players, aPlayerType);

    // For now we only keep the first player we find
    // Local player is always the first one in the list
    // Ui spec limitation
    MPX_DEBUG2( "players[0]: %d", players[0].iUid );
    if ( aCommandId == EMPXCmdUpnpPlayViaLocal )
        {
        iPlayersList.Insert(players[0], 0);
        }
    else if ( ( aCommandId == EMPXCmdUpnpPlayViaRemotePlayer ) &&
              (iPlayersList.Count() > 1) )
        {
        iPlayersList.Insert(players[0], 1);
        }
    else
        {
        iPlayersList.AppendL(players[0]);
        }
    CleanupStack::PopAndDestroy(&players);

    // if the player is of unknown type
    if ( ( aCommandId != EMPXCmdUpnpPlayViaRemotePlayer ) &&
         ( aCommandId != EMPXCmdUpnpPlayViaLocal ) )
        {
        CEikMenuPaneItem::SData menuItem;
        menuItem.iText.Copy( aMenuText );
        menuItem.iCascadeId = 0;
        menuItem.iFlags = EEikMenuItemRadioEnd;

        menuItem.iCommandId = aCommandId + (iPlayersList.Count() - 1);
        aCommandId = menuItem.iCommandId;

        aMenuPane.AddMenuItemL( menuItem );
        }

    if ( iCurrentPlayerType == aPlayerType )
        {
        aMenuPane.SetItemButtonState( aCommandId, EEikMenuItemSymbolOn );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::GetCurrentPlayerDetails
// Retrieves the current player name and type
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::GetCurrentPlayerDetails()
    {
    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
    TUid currentlyUsedPlayer;
    TInt currentlyUsedSubPlayer;
    iCurrentPlayerType = EPbLocal;
    delete iSubPlayerName;
    iSubPlayerName = NULL;
    TRAP_IGNORE( manager.GetSelectionL( iCurrentPlayerType,
                                        currentlyUsedPlayer,
                                        currentlyUsedSubPlayer,
                                        iSubPlayerName));
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::SelectNewPlayerL
// Selects a new player for audio playback
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::SelectNewPlayerL( TInt aCommand )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SelectNewPlayerL()" );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::SelectNewPlayerL(%d)", aCommand );

    TInt errorSelectPlayer( KErrCancel );

    switch (aCommand)
        {
        case EMPXCmdUpnpPlayViaLocal:
            {
            // if Local is not already the current player, select it
            if ( iCurrentPlayerType != EPbLocal )
                {
                MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
                MPX_TRAP( errorSelectPlayer, manager.ClearSelectPlayersL() );
                }
            else // no need to reselect it but always start playback
                {
                errorSelectPlayer = KErrNone;
                }
            break;
            }
        default:
            {
            if ( aCommand >= EMPXCmdUpnpPlayViaRemotePlayer )
                {
                HBufC* buf = HBufC::NewLC( KMaxUidName );
                buf->Des().AppendNum(
                     (iPlayersList[aCommand -
                                   EMPXCmdUpnpPlayViaLocal]).iUid );

                // ActivateViewL leaves with KErrCancel if the dialog is
                // cancelled
                // ActivateViewL leaves with KErrInUse if the selected player
                // is already the active player
                MPX_TRAP(errorSelectPlayer, iViewUtility->ActivateViewL(
                                TUid::Uid( KMPXPluginTypeUPnPBrowseDialogUid ),
                                buf ));
                CleanupStack::PopAndDestroy( buf );
                }
            break;
            }
        }

    GetCurrentPlayerDetails();

    // If InUse, the player selected is already the current one, we start playback anyway
    // contrary to Playback view
    if ( ( errorSelectPlayer == KErrNone ) ||
         ( errorSelectPlayer == KErrInUse ) )
        {
        // Starts playback by calling OpenL() for the selected track
        CMPXCommonListBoxArrayBase* array = iContainer->ListBoxArray();
        if ( array && ( iContainer->TotalListItemCount() > 0 ) )
            {
            MPX_DEBUG1( "CMPXCollectionViewHgImp::SelectNewPlayerL() before MediaL()" );

            TInt currentListBoxItemIndex(
                iContainer->CurrentLbxItemIndex() );

            if ( currentListBoxItemIndex >= 0 )
                {
                const CMPXMedia& media = array->MediaL( currentListBoxItemIndex );

                TMPXGeneralType type( EMPXNoType );
                TMPXGeneralCategory category( EMPXNoCategory );

                if ( media.IsSupported( KMPXMediaGeneralType ) )
                    {
                    type = media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
                    }

                if ( media.IsSupported( KMPXMediaGeneralCategory ) )
                    {
                    category = media.ValueTObjectL<TMPXGeneralCategory>(
                                    KMPXMediaGeneralCategory );
                    }

                MPX_DEBUG2( "CMPXCollectionViewHgImp::SelectNewPlayerL() currentIndex %d", currentListBoxItemIndex );

                // Needed to reset the status of iPreservedState
                iPlaybackUtility->CommandL(EPbCmdResetPreserveState);

                if ( ( type == EMPXItem ) && ( category == EMPXSong ) )
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::SelectNewPlayerL() type is EMPXItem" );
                    if(iCollectionCacheReady)
                        {
                        iCollectionUtility->Collection().OpenL( currentListBoxItemIndex );
						iFirstIncrementalBatch = ETrue;
                        }
                    }
                else if ( ( type == EMPXItem ) && ( category == EMPXPlaylist ) )
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::SelectNewPlayerL() category is EMPXPlaylist" );
                    // if we want to play a playlist, we need to find out
                    // if it's empty first
                    ASSERT( iCurrentFindAllLOp == EMPXOpFindAllLIdle );

                    TMPXItemId id(
                        media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                    CMPXMedia* entry = CMPXMedia::NewL();
                    CleanupStack::PushL( entry );
                    entry->SetTObjectValueL<TMPXGeneralType>(
                        KMPXMediaGeneralType, EMPXGroup );
                    entry->SetTObjectValueL<TMPXGeneralCategory>(
                        KMPXMediaGeneralCategory, EMPXSong );
                    entry->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralId, id );
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL( attrs );
                    attrs.Append( KMPXMediaGeneralId );
                    iCollectionUtility->Collection().FindAllL(
                                            *entry, attrs.Array(), *this );
                    iCurrentFindAllLOp = EMPXOpFindAllLUpnpPlayback;

                    CleanupStack::PopAndDestroy( &attrs );
                    CleanupStack::PopAndDestroy( entry );
                    }
                else // if a folder is selected
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::SelectNewPlayerL() type is EMPXGroup" );
                    if(iCollectionCacheReady)
                        {
                        iCollectionUtility->Collection().OpenL( currentListBoxItemIndex,
                                                            EMPXOpenPlaylistOnly);
						iFirstIncrementalBatch = ETrue;
                        }
                    }
                }
            }
        }
    else if ( errorSelectPlayer != KErrCancel )
        {
        User::LeaveIfError(errorSelectPlayer);
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::CopySelectedItemsToRemoteL
// Copies selected file(s) to remote player
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::CopySelectedItemsToRemoteL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::CopySelectedItemsToRemoteL()" );

    TInt index( iContainer->CurrentLbxItemIndex() );
    if ( index >= 0 )
        {
        CMPXCommonListBoxArrayBase* array = iContainer->ListBoxArray();
        const CMPXMedia& media = array->MediaL( index );

        TMPXGeneralType type( EMPXNoType );
        TMPXGeneralCategory category( EMPXNoCategory );

       if ( media.IsSupported( KMPXMediaGeneralType ) )
            {
            type =
                media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
            }
        if ( media.IsSupported( KMPXMediaGeneralCategory ) )
            {
            category =
                media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
            }

        CMPXCollectionPath* cpath =
                              iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( cpath );

        if ( ( type == EMPXItem ) && ( category == EMPXSong ) )
            {
            const CArrayFix<TInt>* arrayIndex =
                iContainer->CurrentSelectionIndicesL();
            TInt arrayCount( arrayIndex->Count() );

            cpath->ClearSelection();
            if ( arrayCount > 0 )
                {
                for (TInt index=0; index < arrayCount; index++)
                    {
                    cpath->SelectL(arrayIndex->At(index));
                    }
                }
            else
                {
                cpath->Set( index );
                }

            // Ask for the list of selected song paths: will return in HandleMedia()
            ASSERT( iCurrentMediaLOp == EMPXOpMediaLIdle );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append( KMPXMediaGeneralUri );
            iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
            iCurrentMediaLOp = EMPXOpMediaLCopyToRemote;
            CleanupStack::PopAndDestroy( &attrs );
            }
        else // if a folder is selected
            {
            TMPXItemId id(0);
            if ( media.IsSupported( KMPXMediaGeneralId ) )
                {
                id = media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                }

            RArray<TInt> supportedIds;
            CleanupClosePushL(supportedIds);
            supportedIds.AppendL(KMPXMediaIdGeneral);

            CMPXMedia* entry = CMPXMedia::NewL(supportedIds.Array());
            CleanupStack::PushL(entry);
            entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXGroup );

            entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );

            entry->SetTObjectValueL( KMPXMediaGeneralId, id );

            if ( category == EMPXAlbum )
                {
                const CMPXMedia& containerMedia = array->ContainerMedia();
                TMPXGeneralCategory containerCategory( EMPXNoCategory );
                if ( containerMedia.IsSupported( KMPXMediaGeneralCategory ) )
                    {
                    containerCategory = containerMedia.ValueTObjectL<TMPXGeneralCategory>(
                                                KMPXMediaGeneralCategory );
                    }

                if ( containerCategory == EMPXArtist )
                    {
                    // artist/album level, need to specify artist ID in container ID
                    TMPXItemId containerId =
                        containerMedia.ValueTObjectL<TMPXItemId>(
                            KMPXMediaGeneralId );
                    entry->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralContainerId, containerId );
                    }
                }

            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append( KMPXMediaGeneralUri );

            ASSERT( iCurrentFindAllLOp == EMPXOpFindAllLIdle );
            iCollectionUtility->Collection().FindAllL( *entry,
                                                       attrs.Array(),
                                                       *this );
            iCurrentFindAllLOp = EMPXOpFindAllLUpnp;

            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( &supportedIds );
            }
        CleanupStack::PopAndDestroy( cpath );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::DoHandleCopyToRemoteL
// Handle call back from collectionframework for Copy to Remote command
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoHandleCopyToRemoteL(
    const CMPXMedia& aMedia, TBool aComplete )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoHandleCopyToRemoteL()");

    CMPXMediaArray* mediaArray( NULL );
    TInt fileCount( 0 );

    if ( aMedia.IsSupported( KMPXMediaArrayContents ) )
        {
        mediaArray =
            aMedia.Value<CMPXMediaArray>( KMPXMediaArrayContents );
        if ( mediaArray )
            {
            fileCount = mediaArray->Count();
            }
        }
    MPX_DEBUG2("CMPXCollectionViewHgImp::DoHandleCopyToRemoteL Entry count = %d", fileCount);

    // Change this to Global will be needed if FindAll() is called
    // repeatedly until aComplete is ETrue (Inc9?)
    CAiwGenericParamList* aiwCopyInParamList = CAiwGenericParamList::NewLC();

    // multiple files or folder
    if ( fileCount > 0 )
        {
        if ( mediaArray )
            {
            for ( TInt i = 0; i < fileCount; i++ )
                {
                CMPXMedia* media( mediaArray->AtL( i ) );

                const TDesC& location =
                    media->ValueText( KMPXMediaGeneralUri );
                MPX_DEBUG3( "CMPXCollectionViewHgImp::DoHandleCopyToRemote %d filePath = %S", i, &location );

                TAiwVariant path( location );
                TAiwGenericParam fileParameter( EGenericParamFile,
                                                path );
                aiwCopyInParamList->AppendL( fileParameter );
                }
            }
        }
    else // single file
        {
        TMPXGeneralType type( EMPXNoType );

        if ( aMedia.IsSupported( KMPXMediaGeneralType ) )
            {
            type = aMedia.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
            }

        if ( type == EMPXItem )
            {
            const TDesC& location =
                aMedia.ValueText( KMPXMediaGeneralUri );
            TAiwVariant path( location );
            MPX_DEBUG2( "CMPXCollectionViewHgImp::DoHandleCopyToRemote filePath = %S", &location );

            TAiwGenericParam fileParameter( EGenericParamFile, path );
            aiwCopyInParamList->AppendL( fileParameter );
            }
        }

    if ( ( aiwCopyInParamList->Count() > 0 ) &&
         aComplete )
        {
        CAiwGenericParamList* outParamList = CAiwGenericParamList::NewLC();
        // do not allow the copy dialog to be dismissed by external command
        HandleCommandL( EMPXCmdIgnoreExternalCommand );
        iServiceHandler->ExecuteMenuCmdL(
                  EMPXCmdUPnPAiwCmdCopyToExternalCriteria, /* command id */
                  *aiwCopyInParamList,   /* in parameters */
                  *outParamList,          /* out parameters */
                  0,                      /* options */
                  NULL );                 /* call back */
        HandleCommandL( EMPXCmdHandleExternalCommand );

        CleanupStack::PopAndDestroy( outParamList );
        }

    // Clear selection
    if( iContainer )
        {
        iContainer->ClearLbxSelection();
        }

    CleanupStack::PopAndDestroy( aiwCopyInParamList );
    }

// -----------------------------------------------------------------------------
// Find playlists
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgImp::FindPlaylistsL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::FindPlaylistsL" );
    TInt entriesCount( 0 );
    delete iUserPlaylists;
    iUserPlaylists = NULL;
    iUserPlaylists = iCommonUiHelper->FindPlaylistsL();

    if ( iUserPlaylists )
        {
        const CMPXMediaArray* mediaArray =
            iUserPlaylists->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));

        entriesCount = mediaArray->Count();
        MPX_DEBUG2( "CMPXCollectionViewHgImp::FindPlaylistsL Entry count = %d", entriesCount );
        }
    else
        {
        MPX_DEBUG1( "CMPXCollectionViewHgImp::FindPlaylistsL Error getting playlist" );
        User::Leave( KErrNotFound );
        }
    return entriesCount;
    }

// -----------------------------------------------------------------------------
// Save the playlist after reorder
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::SaveCurrentPlaylistL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SaveCurrentPlaylistL" );
    CMPXCommonListBoxArrayBase* listboxArray =
        iContainer->ListBoxArray();
    TInt currentIndex( iContainer->CurrentLbxItemIndex() );
    TInt origIndex( iContainer->GetOriginalIndex() );
    const CMPXMedia& media( listboxArray->MediaL( origIndex ) );
    TMPXItemId id( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    const CMPXMedia& containerMedia = listboxArray->ContainerMedia();
    TMPXItemId playlistId( containerMedia.ValueTObjectL<TMPXItemId>(
        KMPXMediaGeneralId ) );
    MPX_DEBUG4( "CMPXCollectionViewHgImp::SaveCurrentPlaylistL song 0x%x is moved from %d to %d",
         id.iId1, origIndex, currentIndex );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::SaveCurrentPlaylistL playlist ID 0x%x", playlistId.iId1 );
    iIsSavingReorderPlaylist = ETrue;
    iCollectionUiHelper->ReorderPlaylistL(
        playlistId, id, origIndex, currentIndex, this );
    iCurrentHighlightedIndex = currentIndex;
    }

// -----------------------------------------------------------------------------
// Handles rename operation complete
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleRenameOpCompleteL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleRenameOpCompleteL" );
    iSetMediaLCount = KErrNotFound;
    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleRenameOpCompleteL dismiss wait note" );
    iCommonUiHelper->DismissWaitNoteL();
    HandleCommandL( EMPXCmdHandleExternalCommand );
    if ( iInvalidFileExist )
        {
        // there are invalid files, diplay info note
        HBufC* string = StringLoader::LoadLC(
            R_MPX_COLLECTION_NOTE_RENAME_WITH_INVALID, *iNewName );
        iCommonUiHelper->DisplayInfoNoteL( *string );
        CleanupStack::PopAndDestroy( string );
        }
    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleRenameOpCompleteL calling OpenL" );
    // path changed messages were ignored during rename, need to
    // manually refresh the screen
    DoIncrementalOpenL();
    }

// -----------------------------------------------------------------------------
// checks if send option should be shown
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgImp::SendOptionVisibilityL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SendOptionVisibilityL" );
    TBool isHidden( iContainer->TotalListItemCount() < 1 );

    if ( !isHidden )
        {
        CMPXCommonListBoxArrayBase* baseArray =
            iContainer->ListBoxArray();
        CMPXCollectionViewListBoxArray* array =
            static_cast<CMPXCollectionViewListBoxArray*>( baseArray );
        TInt selectionCount( 0 );
        iSelectionIndexCache = iContainer->CurrentSelectionIndicesL();
        if ( iSelectionIndexCache )
            {
            selectionCount = iSelectionIndexCache->Count();
            }

        if ( selectionCount > 0 )
            {
            TBool isValid( EFalse );
            for ( TInt i = 0; i < selectionCount && !isValid; i++ )
                {
                // multiple selection
                isValid = !array->IsItemBrokenLinkL(
                    iSelectionIndexCache->At( i ) );
                isValid = isValid &&
                    !array->IsItemCorruptedL(
                    iSelectionIndexCache->At( i ));
                }
            // if anything is valid, display the item
            isHidden = !isValid;
            }
        else
            {
            //single selection
            TInt currentIndex( iContainer->CurrentLbxItemIndex() );
            if( currentIndex > KErrNotFound )
                {   
                isHidden = array->IsItemBrokenLinkL( currentIndex );
                isHidden = isHidden || array->IsItemCorruptedL( currentIndex );
                }
			else
                {
                isHidden = ETrue;
                }	
            }
        }

    return isHidden;
    }

// -----------------------------------------------------------------------------
// Checks if Set as ringtone option should be shown
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgImp::SetAsRingToneOptionVisibilityL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SetAsRingToneOptionVisibilityL" );
    TBool isHidden( iContainer->TotalListItemCount() < 1 );

    if ( !isHidden )
        {
        CMPXCollectionViewListBoxArray* array =
            static_cast<CMPXCollectionViewListBoxArray*>(
            iContainer->ListBoxArray() );
        const CMPXMedia& media =
            array->MediaL( iContainer->CurrentLbxItemIndex() );

        // if item is not yet fetched from database, assume that it's valid
        if ( !( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ==
            KMPXInvalidItemId ) )
            {
            CMPXMedia* criteria = CMPXMedia::NewL();
            CleanupStack::PushL( criteria );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL( attrs );
            attrs.Append( KMPXMediaGeneralUri );
            attrs.Append( KMPXMediaDrmProtected );
            attrs.Append( KMPXMediaDrmCanSetAutomated );
            criteria->SetTObjectValueL<TMPXGeneralType>(
                KMPXMediaGeneralType,
                media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
            criteria->SetTObjectValueL<TMPXGeneralCategory>(
                KMPXMediaGeneralCategory,
                media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ) );
            if ( media.IsSupported( KMPXMediaGeneralCollectionId ) )
                {
                criteria->SetTObjectValueL<TUid>(
                    KMPXMediaGeneralCollectionId,
                    media.ValueTObjectL<TUid>( KMPXMediaGeneralCollectionId ) );
                }
            else
                {
                CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
                CleanupStack::PushL( path );
                criteria->SetTObjectValueL<TUid>(
                    KMPXMediaGeneralCollectionId,
                    TUid::Uid( path->Id( 0 ) ) );
                CleanupStack::PopAndDestroy( path );
                }
            criteria->SetTObjectValueL<TMPXItemId>(
                KMPXMediaGeneralId,
                media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

            CMPXMedia* songInfo = iCollectionUtility->Collection().FindAllL( *criteria,
                                                             attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PushL( songInfo );

            const CMPXMediaArray* mediaArray(
                songInfo->Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
            User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));

            if ( mediaArray->Count() > 0 )
                {
                CMPXMedia* si( mediaArray->AtL( 0 ) );
                if ( si->IsSupported( KMPXMediaDrmProtected ) )
                    {
                    if ( si->ValueTObjectL<TBool>( KMPXMediaDrmProtected ) &&
                        si->IsSupported( KMPXMediaDrmCanSetAutomated ) )
                        {
                        // if it's drm protected, check if
                        // KMPXMediaDrmCanSetAutomated
                        isHidden = !( si->ValueTObjectL<TBool>( KMPXMediaDrmCanSetAutomated ) );
                        }
                    }
                }
            CleanupStack::PopAndDestroy( songInfo );
            CleanupStack::PopAndDestroy( criteria );
            }
        }
    return isHidden;
    }

// -----------------------------------------------------------------------------
// Checks if file details option should be shown
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgImp::FileDetailsOptionVisibilityL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::FileDetailsOptionVisibilityL" );
    TInt selectionCount( 0 );
    if ( iSelectionIndexCache)
        {
        selectionCount = iSelectionIndexCache->Count();
        }
    TBool isHidden( iContainer->TotalListItemCount() < 1 ||
            selectionCount );
    if ( !isHidden )
        {
        CMPXCollectionViewListBoxArray* array =
            static_cast<CMPXCollectionViewListBoxArray*>(
            iContainer->ListBoxArray() );

        TInt currentItem = iContainer->CurrentLbxItemIndex();
        if ( currentItem > KErrNotFound )
            {
            const CMPXMedia& media = array->MediaL( currentItem );

            if ( media.IsSupported( KMPXMediaGeneralFlags ) )
                {
                TUint flags( media.ValueTObjectL<TUint>( KMPXMediaGeneralFlags ) );
                if ( ( flags ) & ( KMPXMediaGeneralFlagsIsMissingDetails ) )
                    {
                    isHidden = ETrue;
                    }
                }
            }
        else
            {
            isHidden = ETrue;
            }

        }
    return isHidden;
    }



// -----------------------------------------------------------------------------
// Handle send command in TBone view.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoSendTBoneListItemL(TMPXItemId aContainerId)
    {

    MPX_FUNC( "CMPXCollectionViewHgImp::DoSendTBoneListItemL" );
    
    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );
    
    CMPXMedia* albumTrack = iContainer->SelectedItemMediaL();
    TMPXItemId trackId = albumTrack->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    // create the item path to send

    if ( path->Levels() == 3 ) //TBone album level
        {
        path->Back();  
        }
    else if (path->Levels() == 4) //TBone Song Node level
        {
        path->Back();
        path->Back();
        }
    path->AppendL(aContainerId);
    path->AppendL(trackId); 

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL(attrs);
    attrs.Append(
        TMPXAttribute( KMPXMediaIdGeneral,
            EMPXMediaGeneralUri | EMPXMediaGeneralSize |
            EMPXMediaGeneralCollectionId |
            EMPXMediaGeneralFlags | EMPXMediaGeneralId |
            EMPXMediaGeneralType | EMPXMediaGeneralCategory ) );
    iCurrentMediaLOp = EMPXOpMediaLSend;
    iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( path );
}




// -----------------------------------------------------------------------------
// Handle send command.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoSendL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoSendL" );


    CMPXCommonListBoxArrayBase* listboxArray(
        iContainer->ListBoxArray() );
    TInt currentIndex( iContainer->CurrentLbxItemIndex() );
    const CMPXMedia& media( listboxArray->MediaL(
        iContainer->CurrentLbxItemIndex() ) );

    // Marked indicies
    TInt arrayCount( iSelectionIndexCache->Count() );

    if ( ( arrayCount == 0 || arrayCount == 1 ) &&
        ( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId ) )
        {
        // not marked, or only 1 item is marked
        // and the highlighted item is not yet available
        MPX_DEBUG1( "CMPXCollectionViewHgImp::DoSendL item not yet fetched" );
        }
    else
        {
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( path );

        MPX_DEBUG2( "CMPXCollectionViewHgImp::DoSendL array count = %d", arrayCount );
        if ( arrayCount > 1 )
            {
            // if more than 1 selected
            for ( TInt i = 0; i < arrayCount; i++ )
                {
                MPX_DEBUG2( "CMPXCollectionViewHgImp::DoSendL array index = %d", iSelectionIndexCache->At( i ) );
                path->SelectL( iSelectionIndexCache->At(i) );
                }
            }
        else
            {
            // if only 1 item is marked, treat it as single selection
            path->Set( ( arrayCount == 1 ) ?
                iSelectionIndexCache->At( 0 ) : iContainer->CurrentLbxItemIndex() );
            }

        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        attrs.Append(
            TMPXAttribute( KMPXMediaIdGeneral,
                EMPXMediaGeneralUri | EMPXMediaGeneralSize |
                EMPXMediaGeneralCollectionId |
                EMPXMediaGeneralFlags | EMPXMediaGeneralId |
                EMPXMediaGeneralType | EMPXMediaGeneralCategory ) );
        iCurrentMediaLOp = EMPXOpMediaLSend;
        iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
        CleanupStack::PopAndDestroy( &attrs );
        CleanupStack::PopAndDestroy( path );
        }
    }

// -----------------------------------------------------------------------------
// Handle send playlist command
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoSendPlaylistL( TMPXItemId aItemId )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoSendPlaylistL" );
    // export the playlist to file system, and send the playlist
    // when the operation is completed
    HBufC* defaultDrive = iCommonUiHelper->DefaultDriveLC();
    TFileName defaultPath;
    defaultPath.Append( *defaultDrive );
    RFs& fileSession = iCoeEnv->FsSession();
    TBool folderExist = BaflUtils::FolderExists( fileSession, defaultPath );
    TInt count( 0 );
    // use time stamp as the directory name for export, so that
    // it won't overwrite any existing files.  It will search for the first
    // available folder that is not currently in the file system
    while ( folderExist )
        {
        if ( count++ > KMPXPlaylistExportRetry )
            {
            MPX_DEBUG1( "CMPXCollectionViewHgImp::DoSendPlaylistL maximum retry count reached, leaving" );
            User::Leave( KErrNotReady );
            }
        defaultPath.Copy( *defaultDrive );
        TTime timeStamp;
        timeStamp.UniversalTime();
        defaultPath.AppendNum( timeStamp.Int64() );
        defaultPath.Append( KPathDelimiter );
        folderExist = BaflUtils::FolderExists( fileSession, defaultPath );
        }
    CleanupStack::PopAndDestroy( defaultDrive );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::DoSendPlaylistL creating path = %S", &defaultPath );
    BaflUtils::EnsurePathExistsL( fileSession, defaultPath );
    // playlist is sent in the call back of this function
    iCollectionUiHelper->ExportPlaylistL( aItemId, defaultPath, this );
    }

// -----------------------------------------------------------------------------
// Handle call back from collectionframework for send command
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoHandleSendL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoHandleSendL" );
    TInt invalidFileCount( 0 );
    TInt maxSize( 0 );
    CMessageData* messageData = CMessageData::NewLC();

    TInt fileCount( 0 );
    if( aMedia.IsSupported( KMPXMediaArrayContents ) )
        {
        const CMPXMediaArray* mediaArray =
            aMedia.Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));
        fileCount = mediaArray->Count();

        }
    MPX_DEBUG2( "CMPXCollectionViewHgImp::DoHandleSendL Entry count = %d", fileCount );

    if ( fileCount > 0 )
        {
        const CMPXMediaArray* mediaArray =
            aMedia.Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));

        CMPXMedia* media( NULL );
        for ( TInt i = 0; i < fileCount; i++ )
            {
            media = mediaArray->AtL( i );
            TUint flags( media->ValueTObjectL<TUint>( KMPXMediaGeneralFlags ) );
            const TDesC& location = media->ValueText( KMPXMediaGeneralUri );

            if ( !( flags & KMPXMediaGeneralFlagsIsInvalid ) &&
                ConeUtils::FileExists( location ) )
                {
                TInt fileSize( 0 );
                if ( media->IsSupported( KMPXMediaGeneralSize ) )
                    {
                    fileSize = media->ValueTObjectL<TInt>( KMPXMediaGeneralSize );
                    }
                else
                    {
                    RFs& fileSession = iCoeEnv->FsSession();
                    TEntry pl;
                    fileSession.Entry( location, pl );
                    fileSize = pl.iSize;
                    }
                if ( fileSize > maxSize )
                    {
                    maxSize = fileSize;
                    }
                messageData->AppendAttachmentL( location );
                }
            else
                {
                UpdateDatabaseFlagL( KErrNotFound, *media,
                    KMPXMediaGeneralFlagsIsInvalid, ETrue, EFalse );
                invalidFileCount++;
                }
            }
        }
    else
        {
        // single file
        fileCount = 1;
        TUint flags = aMedia.ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
        const TDesC& location = aMedia.ValueText( KMPXMediaGeneralUri );

        if ( !( flags & KMPXMediaGeneralFlagsIsInvalid ) &&
            ConeUtils::FileExists( location ) )
            {
            TInt fileSize( 0 );
            if ( aMedia.IsSupported( KMPXMediaGeneralSize ) )
                {
                fileSize = aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralSize );
                }
            else
                {
                RFs& fileSession = iCoeEnv->FsSession();
                TEntry pl;
                fileSession.Entry( location, pl );
                fileSize = pl.iSize;
                }
            if ( fileSize > maxSize )
                {
                maxSize = fileSize;
                }
            messageData->AppendAttachmentL( location );
            }
        else
            {
            UpdateDatabaseFlagL( KErrNotFound, aMedia,
                KMPXMediaGeneralFlagsIsInvalid, ETrue, EFalse );
            invalidFileCount++;
            }
        }
    TBool confirmOk( EFalse );
    MPX_DEBUG3( "CMPXCollectionViewHgImp::DoHandleSendL Invalid Count = %d, Total Count = %d", invalidFileCount, fileCount );
    if ( invalidFileCount )
        {
        if ( invalidFileCount == fileCount )
            {
            // all files are invalid
            iCommonUiHelper->DisplayInfoNoteL( ( invalidFileCount == 1 ) ?
                R_MPX_COLLECTION_INFO_FILE_NOT_FOUND :
                R_MPX_COLLECTION_SEND_ALL_INVALID_SONGS_NOTE_TXT );
            }
        else
            {
            // Show confirmation query if invalid file found
            HBufC* queryTxt = StringLoader::LoadLC(
                R_MPX_COLLECTION_QUERY_SEND_INVALID_SONGS_TXT );
            CAknQueryDialog* query = CAknQueryDialog::NewL(
                CAknQueryDialog::EConfirmationTone );
            CleanupStack::PushL( query );
            query->PublishDialogL( EMPlayerNoteSendInvalidSongs,
                                KMPlayerNoteCategory);
            CleanupStack::Pop( query );
            confirmOk = query->ExecuteLD(
                R_MPX_COLLECTION_GENERIC_CONFIRMATION_QUERY,
                *queryTxt );
            CleanupStack::PopAndDestroy( queryTxt );
            }
        }

    if ( !invalidFileCount || confirmOk )
        {
        if ( !iSendUi )
            {
            iSendUi = CSendUi::NewL();
            }
        TSendingCapabilities capabilities(
            0,
            maxSize,
            TSendingCapabilities::ESupportsAttachments );
        // ignore commands from external clients so that the
        // sending is not interrupted
        HandleCommandL( EMPXCmdIgnoreExternalCommand );
        // send ui leaves with error 0 if canceled, have to be trapped
        MPX_TRAPD( err, iSendUi->ShowQueryAndSendL( messageData, capabilities ) );
        if ( err != KErrNone )
            {
            HandleError( err );
            }
        HandleCommandL( EMPXCmdHandleExternalCommand );
        }

    // Clear selection
    iContainer->ClearLbxSelection();
    CleanupStack::PopAndDestroy( messageData );
    }

// -----------------------------------------------------------------------------
// Handle rename command
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoHandleRenameL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoHandleRenameL" );
    CMPXCommonListBoxArrayBase* listboxArray(
        iContainer->ListBoxArray() );
    TInt currentIndex( iContainer->CurrentLbxItemIndex() );
    const CMPXMedia& currentMedia( listboxArray->MediaL( currentIndex ) );
    TMPXGeneralCategory category(
        currentMedia.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ) );
    switch ( category )
        {
        case EMPXPlaylist:
            {
            // rename playlist
            iCurrentMediaLOp = EMPXOpMediaLRenamePlaylist;
            CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( path );
            path->Set( iContainer->CurrentLbxItemIndex() );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append(
                TMPXAttribute( KMPXMediaIdGeneral,
                    EMPXMediaGeneralUri | EMPXMediaGeneralId |
                    EMPXMediaGeneralTitle ) );
            iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( path );
            break;
            }
        case EMPXAlbum:
        case EMPXArtist:
        case EMPXGenre:
        case EMPXComposer:
            {
            delete iNewName;
            iNewName = NULL;
            iNewName = HBufC::NewL( KMaxFileName );
            TPtr ptr( iNewName->Des() );

            // Set default text as current selected item text
            const CMPXMediaArray& mediaAry = listboxArray->MediaArray();
            CMPXMedia* origMedia( mediaAry.AtL( currentIndex ) );
            const TDesC& title = origMedia->ValueText( KMPXMediaGeneralTitle );
            ptr.Append( title.Left(KMaxFileName) );

            CAknTextQueryDialog* query = CAknTextQueryDialog::NewL( ptr );
            CleanupStack::PushL( query );
            query->SetMaxLength( KMaxFileName );
            HBufC* promptTxt = StringLoader::LoadLC( R_MPX_QTN_NMP_PRMPT_NEW_NAME );
            query->SetPromptL( *promptTxt );
            query->SetPredictiveTextInputPermitted( ETrue );
            CleanupStack::PopAndDestroy( promptTxt );
            query->PublishDialogL(
                    EMPlayerQueryPrmptNewName,
                    KMPlayerNoteCategory);
            CleanupStack::Pop( query );
            if ( query->ExecuteLD( R_MPX_CUI_RENAME_QUERY ) )
                {
                TMPXItemId id(
                    currentMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

                CMPXMedia* media = CMPXMedia::NewL();
                CleanupStack::PushL( media );
                media->SetTObjectValueL<TMPXGeneralType>(
                    KMPXMediaGeneralType, EMPXGroup );
                media->SetTObjectValueL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory, EMPXSong );
                media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );

                switch ( category )
                    {
                    case EMPXAlbum:
                        {
                        iCurrentFindAllLOp = EMPXOpFindAllLRenameAlbum;
                        // check if it's album or artist/album
                        const CMPXMedia& containerMedia = listboxArray->ContainerMedia();
                        TMPXGeneralCategory containerCategory(
                            containerMedia.ValueTObjectL<TMPXGeneralCategory>(
                             KMPXMediaGeneralCategory ) );
                        if ( containerCategory == EMPXArtist )
                            {
                            // artist/album level, need to specify artist ID in container ID
                            TMPXItemId containerId(
                                containerMedia.ValueTObjectL<TMPXItemId>(
                                    KMPXMediaGeneralId ) );
                            media->SetTObjectValueL<TMPXItemId>(
                                KMPXMediaGeneralContainerId, containerId );
                            }
                        break;
                        }
                    case EMPXArtist:
                        {
                        iCurrentFindAllLOp = EMPXOpFindAllLRenameArtist;
                        break;
                        }
                    case EMPXGenre:
                        {
                        iCurrentFindAllLOp = EMPXOpFindAllLRenameGenre;
                        break;
                        }
                    case EMPXComposer:
                        {
                        iCurrentFindAllLOp = EMPXOpFindAllLRenameComposer;
                        break;
                        }
                    default:
                        {
                        ASSERT( 0 );
                        break;
                        }
                    }
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL( attrs );
                attrs.Append(
                    TMPXAttribute( KMPXMediaIdGeneral,
                        EMPXMediaGeneralId | EMPXMediaGeneralUri |
                        EMPXMediaGeneralCollectionId |
                        EMPXMediaGeneralType | EMPXMediaGeneralCategory | EMPXMediaGeneralFlags ) );
                if ( category == EMPXAlbum || category == EMPXArtist )
                    {
                    attrs.Append( TMPXAttribute( KMPXMediaIdMusic,
                        EMPXMediaMusicAlbumArtFileName ) );
                    }
                iCollectionUtility->Collection().FindAllL( *media, attrs.Array(), *this );
                CleanupStack::PopAndDestroy( &attrs );
                CleanupStack::PopAndDestroy( media );
                }
            break;
            }
        default:
            {
            // shouldn't reach here
            ASSERT( 0 );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// Prepares media object for selected items
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::PrepareMediaForSelectedItemsL( CMPXMedia& aMedia, TMPXGeneralCategory aContainerCategory, TMPXGeneralType aContainerType  )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::PrepareMediaForSelectedItemsL" );
    CMPXMediaArray* mediaArray( CMPXMediaArray::NewL() );
    CleanupStack::PushL( mediaArray );

    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( path );
    TMPXItemId id( path->Id( 0 ) );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::PrepareMediaForSelectedItemsL collection ID = 0x%x", id.iId1 );
    CleanupStack::PopAndDestroy( path );
    
    //support for TBone view add to playlist
    if ( iContainer->IsTBoneView() )
        {
        CMPXMedia* media = iContainer->SelectedItemMediaL();
        if ( media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId )
           {
           // leave with special error code, this error
           // should not trigger any error message, the operation should
           // be terminated silently
           User::Leave( KMPXErrDataNotReady );
           }
        else
           {
           CMPXMedia* entry( PopulateMediaLC( *media, id ) );
           mediaArray->AppendL( entry );
           CleanupStack::Pop( entry );
           }      	
       }

    else
        {
        TInt count( iSelectionIndexCache->Count() );   
        CMPXCommonListBoxArrayBase* listBoxArray =   
            iContainer->ListBoxArray(); 
        if ( count > 0 )
            {
                            
            if ( count == 1 )
                {
                // marked one item, if it's not valid, ignore the command
                const CMPXMedia& listBoxMedia = listBoxArray->MediaL( iSelectionIndexCache->At(0) );
                if ( listBoxMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId )
                    {
                    // leave with special error code, this error
                    // should not trigger any error message, the operation should
                    // be terminated silently
                    User::Leave( KMPXErrDataNotReady );
                    }
                
                }
                for ( TInt i = 0; i < count; i++ )
                    {
                    const CMPXMedia& listBoxMedia = listBoxArray->MediaL( iSelectionIndexCache->At(i) );
                    if ( listBoxMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId )
                        {
                        // item not ready, cache this command and execute
                        // it when the data becomes valid
                        User::Leave( KMPXErrDataNotReadyCacheCommand );
                        }
                    CMPXMedia* entry( PopulateMediaLC( listBoxMedia, id ) );
                    mediaArray->AppendL( entry );
                    CleanupStack::Pop( entry );
                    }
            }
       
        else
            {
            const CMPXMedia& listBoxMedia = listBoxArray->MediaL(
                iContainer->CurrentLbxItemIndex() );
            if ( listBoxMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId )
                {
                // leave with special error code, this error
                // should not trigger any error message, the operation should
                // be terminated silently
                User::Leave( KMPXErrDataNotReady );
                }
            else
                {
                CMPXMedia* entry( PopulateMediaLC( listBoxMedia, id ) );
                mediaArray->AppendL( entry );
                CleanupStack::Pop( entry );
                }
            }
       }
 
       aMedia.SetTObjectValueL( KMPXMediaGeneralCollectionId, id );
       aMedia.SetCObjectValueL( KMPXMediaArrayContents, mediaArray );
       aMedia.SetTObjectValueL( KMPXMediaArrayCount, mediaArray->Count() );
       iNumSongAddedToPlaylist = mediaArray->Count();
       CleanupStack::PopAndDestroy( mediaArray );
    }

// -----------------------------------------------------------------------------
// Populates media object with information needed for save to
// existing playlist
// -----------------------------------------------------------------------------
//
CMPXMedia* CMPXCollectionViewHgImp::PopulateMediaLC(
    const CMPXMedia& aMedia, TMPXItemId aCollectionId )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::PopulateMediaLC" );
    CMPXMedia* entry = CMPXMedia::NewL();
    CleanupStack::PushL( entry );
    entry->SetTextValueL(
        KMPXMediaGeneralTitle,
        aMedia.ValueText( KMPXMediaGeneralTitle ) );
    entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXItem);
    entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXSong );
    entry->SetTObjectValueL( KMPXMediaGeneralId,
        aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    entry->SetTObjectValueL( KMPXMediaGeneralCollectionId, aCollectionId );
    return entry;
    }

// -----------------------------------------------------------------------------
// Handle rename playlist command
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoHandleRenamePlaylistL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoHandleRenamePlaylistL" );
    const TDesC& uri = aMedia.ValueText( KMPXMediaGeneralUri );
    TParsePtrC fn( uri );
#ifdef _DEBUG
    TPtrC ptr( fn.NameAndExt() );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::DoHandleRenamePlaylistL Original Uri = %S", &ptr );
#endif
    HBufC* playlistName = HBufC::NewLC( KMaxFileName );
    TPtr playlistNamePtr = playlistName->Des();
    const TDesC& title = aMedia.ValueText( KMPXMediaGeneralTitle );
    TInt ret = iCommonUiHelper->LaunchRenameDialogL(
        title, playlistNamePtr, fn.DriveAndPath() );
    if ( ret )
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL<TMPXGeneralType>(
            KMPXMediaGeneralType, EMPXItem );
        media->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXPlaylist );
        TMPXItemId id( aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
        TParsePtrC newFn( *playlistName );
        media->SetTextValueL( KMPXMediaGeneralTitle, newFn.NameAndExt() );
        iCollectionUiHelper->RenameL( *media, this );
        CleanupStack::PopAndDestroy( media );
        }
    CleanupStack::PopAndDestroy( playlistName );
    }

// ---------------------------------------------------------------------------
// Set/clears the flags for item in database
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::UpdateDatabaseFlagL(
    TInt aIndex, const CMPXMedia& aMedia, TUint aFlag,
    TBool aSet, TBool aEnableInfoDialog )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::UpdateDatabaseFlagL" );
    MPX_DEBUG3( "CMPXCollectionViewHgImp::UpdateDatabaseFlagL aFlag = 0x%x, aSet = %d", aFlag, aSet );
    CMPXCommonListBoxArrayBase* array(0);
    if (iContainer)
    	array = iContainer->ListBoxArray();
    	
    ASSERT( array );

    // set the item as invalid
    TUint flags( 0 );
    if ( aSet )
        {
        flags = KMPXMediaGeneralFlagsSetOrUnsetBit;
        }
    flags |= aFlag;

    MPX_DEBUG2( "CMPXCollectionViewHgImp::UpdateDatabaseFlagL flag after 0x%x", flags );
    CMPXMedia* entry = CMPXMedia::NewL();
    CleanupStack::PushL( entry );
    if ( aIndex > KErrNotFound )
        {
        MPX_DEBUG2( "CMPXCollectionViewHgImp::UpdateDatabaseFlagL using aIndex = %d", aIndex );
        const CMPXMedia& media = array->MediaL( aIndex );
        TMPXItemId id( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        TMPXGeneralType type(
            media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
        TMPXGeneralCategory category(
            media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ) );
        entry->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
        entry->SetTObjectValueL<TMPXGeneralType>(
            KMPXMediaGeneralType, type );
        entry->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, category );
        }
    else
        {
        // try to look for the info inside aMedia
        if ( aMedia.IsSupported( KMPXMediaGeneralId ) )
            {
            TMPXItemId id( aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            entry->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
            }
        if ( aMedia.IsSupported( KMPXMediaGeneralType ) )
            {
            TMPXGeneralType type(
                aMedia.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
            entry->SetTObjectValueL<TMPXGeneralType>(
                KMPXMediaGeneralType, type );
            }
        if ( aMedia.IsSupported( KMPXMediaGeneralCategory ) )
            {
            TMPXGeneralCategory category(
                aMedia.ValueTObjectL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory ) );
            entry->SetTObjectValueL<TMPXGeneralCategory>(
                KMPXMediaGeneralCategory, category );
            }
        }
    TUid collectionId( aMedia.ValueTObjectL<TUid>( KMPXMediaGeneralCollectionId ) );
    entry->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, collectionId );
    entry->SetTObjectValueL<TUint>( KMPXMediaGeneralFlags, flags );

    // Update the collection via CommandL
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KMPXCommandIdCollectionSet );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );
    TUid colId( entry->ValueTObjectL<TUid>(KMPXMediaGeneralCollectionId) );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, entry );
    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );

    CleanupStack::PopAndDestroy( entry );
    if ( aEnableInfoDialog && KMPXMediaGeneralFlagsIsInvalid == aFlag )
        {
        iCommonUiHelper->DisplayInfoNoteL( R_MPX_COLLECTION_INFO_FILE_NOT_FOUND );
        }
    }

// ---------------------------------------------------------------------------
// Get duration of current view
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::GetDurationL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::GetDurationL" );
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    cpath->Back();  // Container
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL(attrs);
    iCurrentMediaLOp = EMPXOpMediaLGetContainerInfo;
    attrs.Append( KMPXMediaGeneralDuration );
    iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( cpath );
    }

// ---------------------------------------------------------------------------
// Handles file check result
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoHandleFileCheckResultL(
    TUid aViewToActivate, const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleFileCheckResultL" );
    const TDesC& location = aMedia.ValueText( KMPXMediaGeneralUri );
    if ( location.Length() == 0 || ConeUtils::FileExists( location ) )
        {
        HandleListBoxEventL( NULL, EEventItemClicked );   
        HBufC* buf = HBufC::NewLC( 5 ); // magic number, array granularity
        buf->Des().AppendNum( iContainer->CurrentLbxItemIndex() );
        // Activate view via View Framework
        iViewUtility->ActivateViewL( aViewToActivate, buf );
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {
        // mark database entry for this item as invalid
        UpdateDatabaseFlagL( iContainer->CurrentLbxItemIndex(),
            aMedia, KMPXMediaGeneralFlagsIsInvalid, ETrue );
        }
    }

#ifdef __ENABLE_MSK
// ---------------------------------------------------------------------------
// Updates the middle softkey display
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::UpdateMiddleSoftKeyDisplayL( TInt aMskId )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::UpdateMiddleSoftKeyDisplayL" );
    CEikButtonGroupContainer* cba = Cba();
	// We only update middle softkey for non-touch UI
    if ( cba && !AknLayoutUtils::PenEnabled() )
        {
        if ( !iContainer->CurrentListItemCount() ) // list is empty
            {
            // remove middle softkey label
            iCommonUiHelper->RemoveMiddleSoftKeyLabel( *cba );
            MPX_DEBUG1( "CMPXCollectionViewHgImp::UpdateMiddleSoftKeyDisplayL - remove label, list is empty" );
            }
        else if ( iContainer->CurrentSelectionIndicesL()->Count() ) // marked items
            {
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnPropMskMenu,
                KAknsIIDQsnComponentColors,
                EAknsCIQsnComponentColorsCG13,
                bitmap,
                mask,
                AknIconUtils::AvkonIconFileName(),
                EMbmAvkonQgn_prop_msk_menu,
                EMbmAvkonQgn_prop_msk_menu_mask,
                KRgbBlack);

            CleanupStack::PushL(bitmap);
            CleanupStack::PushL(mask);

            // set middle softkey icon
            iCommonUiHelper->SetMiddleSoftKeyIconL(*cba, bitmap, mask);

            CleanupStack::PopAndDestroy(2, bitmap);
            MPX_DEBUG1("CMPXCollectionViewHgImp::UpdateMiddleSoftKeyDisplayL - items marked, show icon");
            }
        else
            {
            // set middle softkey label
            iCommonUiHelper->SetMiddleSoftKeyLabelL(
                *cba,
                aMskId,
                EAknSoftkeyForwardKeyEvent );
            iCurrentMskId = aMskId;
            MPX_DEBUG1( "CMPXCollectionViewHgImp::UpdateMiddleSoftKeyDisplayL - label displayed" );
            }
        cba->DrawDeferred();
        }
    }
#endif // __ENABLE_MSK


// ---------------------------------------------------------------------------
// Handles the completion of adding a playlist event
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleAddCompletedL(
    CMPXMedia* aPlaylist, TInt aError )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleAddCompletedL" );
    iAddingToNewPlaylist = EFalse;
    iDialogDismissed = EFalse;
    iCommonUiHelper->DismissWaitNoteL();
    HandleCommandL( EMPXCmdHandleExternalCommand );
    if ( aError == KErrNone )
        {
        // Show confirmation note
        if( aPlaylist->IsSupported( KMPXMediaGeneralId ) )
            {
            iPlaylistId = aPlaylist->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            }
        MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleAddCompletedL iPlaylistId = 0x%x", iPlaylistId.iId1 );
        if ( iCurrentPlaylistOp != EMPXOpPLCreating )
            {
            if( !iTitleWait )
                {
                delete iTitleWait;
                iTitleWait = NULL;
                }
            iTitleWait = aPlaylist->ValueText( KMPXMediaGeneralTitle ).AllocL();
            if( iDialogDismissed )
                {
                ShowAddedItemsDialogL();
                }
            }
        else // iCurrentPlaylistOp == EMPXOpPLCreating
            {
            // Re-open the collection, because we cannot
            // depend on the change events that could be a race condition
            // against this HandleAddCompleteL() callback!
            //
            DoIncrementalOpenL();
            }
        }
    else
        {
        HandleError( aError );
        }

    delete aPlaylist;
    if ( iContainer )
        {
        iContainer->ClearLbxSelection();
        }
    }

// ---------------------------------------------------------------------------
// Shows the added items dialog
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::ShowAddedItemsDialogL()
    {
    HBufC* confirmTxt( NULL );
    
    TMPlayerSecondaryDisplayNote noteId( EMPlayerNoteNone );
    if ( iNumSongAddedToPlaylist > 1 )
        {
        
        confirmTxt = StringLoader::LoadLC(
            R_MPX_QTN_MUS_MULTIPLE_TRACKS_ADDED_TO_PL,
            iTitleWait->Des(), iNumSongAddedToPlaylist );
        noteId = EMPlayerNoteAddManySongToPlaylist;
        }
    else
        {
        confirmTxt = StringLoader::LoadLC(
            R_MPX_QTN_MUS_NOTE_TRACK_ADDED_TO_PL, iTitleWait->Des() );
        noteId = EMPlayerNoteAddSongToPlaylist;
        }
    
    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote();

    note->PublishDialogL(
        noteId,
        KMPlayerNoteCategory );

    if ( iCoverDisplay )
        {
        CAknMediatorFacade* covercl = AknMediatorFacade( note );
        if ( covercl )
            {
            covercl->BufStream() << iTitleWait;
            if ( iNumSongAddedToPlaylist > 1 )
                {
                covercl->BufStream().WriteInt32L( iNumSongAddedToPlaylist );
                }
            }
        }
    note->ExecuteLD( *confirmTxt );
    CleanupStack::PopAndDestroy( confirmTxt );
	if( iTitleWait )
	    {
	    delete iTitleWait;
	    iTitleWait = NULL;
		}
    }

// -----------------------------------------------------------------------------
// Handles the completion of setting a media event.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleSetCompletedL( CMPXMedia* aMedia, TInt aError )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleSetCompletedL" );
    delete aMedia;
    if ( aError == KErrNone )
        {
        // save successful, set flag so that it can't be saved again
        MPXTlsHelper::SetNeedSave( EFalse );
        }
    else
        {
        if (iContainer->IsInReorderMode())
            {
            DeactivateReorderGrabbedModeL(ETrue);
            }
        HandleError( aError );
        }
    if ( iIsSavingReorderPlaylist )
        {
        // change messages are ignored during save command,
        // when save is complete, it should open the collection again to refresh
        // display data
        DoIncrementalOpenL();
        iIsSavingReorderPlaylist = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// Handle playback message
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoHandlePlaybackMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        switch ( event )
            {
            case TMPXPlaybackMessage::EActivePlayerChanged:
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXCollectionViewHgImp::HandlePlaybackMessageL - EStateChanged(%d)", type );
                UpdatePlaybackStatusL();
                break;
                }
            case TMPXPlaybackMessage::EError:
                {
                if ( iCommonUiHelper )
                    {
                    iCommonUiHelper->DismissWaitNoteL();
                    HandleCommandL( EMPXCmdHandleExternalCommand );
                    }
                if ( iIsDeleting )
                    {
                    iCollectionUiHelper->Cancel();
                    iIsDeleting = EFalse;
                    // if delete is interrupted, reopen
                    DoIncrementalOpenL();
                    }
                MPX_DEBUG2( "CMPXCollectionViewHgImp::HandlePlaybackMessageL Error = %d", data );
                break;
                }
            case TMPXPlaybackMessage::EPlayerUnavailable:
                {
                if ( iUpnpFrameworkSupport )
                    {
                    if ( data == KErrNotFound )
                        {
                        if ( iViewUtility->ActiveViewType() ==
                            TUid::Uid( KMPXPluginTypeCollectionUid ) )
                            {
                            if ( iSubPlayerName )
                                {
                                HBufC* dialogText = StringLoader::LoadLC( R_MPX_COLLECTION_NOTE_REMOTE_CONNECTION_FAILED,
                                                                          *iSubPlayerName );
                                CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                                errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                                errNote->ExecuteLD( *dialogText );
                                CleanupStack::PopAndDestroy( dialogText );
                                }
                            }
                        }
                    }
                // else we don't show any error note
                break;
                }
            default:
                {
                // ignore other messages
                break;
                }
            }
        // Fix EBLI-7CFE4D press select key very quick cause System Error KErrArgument
        if (event != TMPXPlaybackMessage::ECommandReceived)
            {
            iHandlingKeyEvent = EFalse;
            }
        }
    else
        {
        iHandlingKeyEvent = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoHandleCollectionMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        MPX_DEBUG4( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL Event = %d, Type() = %d, data = %d",
            event, type, data );

        // Only handle collection messages while collection view is in focus
        // and not transitioning views
        if( iContainer )
            {

            if ( event == TMPXCollectionMessage::EPathChanged &&
                 type == EMcPathChangedByOpen &&
                 data == EMcContainerOpened
                 )
                { // Open new entries
                MPX_DEBUG1( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL calling OpenL EMcPathChangedByOpen" );
                if ( iContainer->IsInReorderMode() )
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL canceling reorder mode" );
                    // cancel reorder mode if path has changed
                    ProcessCommandL( EMPXCmdReorderDone );
                    }
                if( iContainer->CurrentSelectionIndicesL()->Count() )
                    {
                    HandleCommandL( EAknUnmarkAll );
                    }

                // Incremental Open when browsing to the next level
                DoIncrementalOpenL();
                }
            else if ( event == TMPXCollectionMessage::EPathChanged &&
                      type == EMcPathChangedByCollectionChange )
                {
                // Re-Fetch entries only if we are in playlist
                // creation idle stage. Ignore change messages during playlist op
                // because there are race conditions and we don't know
                // which handle open is actually opening the playlist!
                //
                // don't call openL if it's currently deleting
                // don't call openL if saving after reorder
                // don't call openL if renaming
                if( iCurrentPlaylistOp == EMPXOpPLIdle  && !iIsDeleting
                    && !iIsSavingReorderPlaylist && iSetMediaLCount == KErrNotFound )
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL calling OpenL EMcPathChangedByCollectionChange" );
                    if( iContainer->CurrentSelectionIndicesL()->Count() )
                        {
                        HandleCommandL( EAknUnmarkAll );
                        }
                    // dismiss find pane if it's visible
                    if( iContainer->FindBoxVisibility() )
                        {
                        iContainer->EnableFindBox( EFalse );
                        }
                    DoIncrementalOpenL();
                    }
                else
                    {
                    MPX_DEBUG4( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL PathChange Message Ignored iIsDeleting = %d, iCurrentPlaylistOp = %d, iIsSavingReorderPlaylist = %d", iIsDeleting, iCurrentPlaylistOp, iIsSavingReorderPlaylist );
                    MPX_DEBUG2( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL PathChange Message Ignored iSetMediaLCount = %d", iSetMediaLCount );
                    }
                }
            else if ( event == TMPXCollectionMessage::EPathChanged &&
                 type == EMcPathChangedByOpen &&
                 data == EMcItemOpened)
                {
                // opened a song, revert back one level when collection view
                // is activated again
                iIncrementalOpenUtil->Stop();
                iBackOneLevel = ETrue;
                }
            else if( event == TMPXCollectionMessage::EBroadcastEvent )
                {
                MPX_DEBUG2( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL - broadcast type = %d", type );
                if ( type == EMcMsgDiskRemoved &&
                     iViewUtility->ActiveViewType() != TUid::Uid(KMPXPluginTypeWaitNoteDialogUid) )
                    {
                    AknDialogShutter::ShutDialogsL( *CEikonEnv::Static() );
                    }
                if ( iConfirmationDlg && ( type == EMcMsgUSBMTPStart || type == EMcMsgUSBMassStorageStart ) )
                    {
                    TKeyEvent kEvent;
                    kEvent.iCode = EKeyEscape;
                    kEvent.iScanCode = EStdKeyEscape;
                    iConfirmationDlg->OfferKeyEventL( kEvent, EEventKey );
                    }

                }
        else if ( event == TMPXCollectionMessage::EFocusChanged && iContainer && iCoverDisplay )
            {
            if ( data != KErrNotFound &&
                 data < iContainer->CurrentListItemCount() &&
                 !iIgnoreNextFocusChangedMessage )
                {
                MPX_DEBUG2( "CMPXCollectionViewHgImp::DoHandleCollectionMessageL - EFocusChanged to %d", data );
                iContainer->SetLbxCurrentItemIndexAndDraw( data );
                }
            iIgnoreNextFocusChangedMessage = EFalse;
            }
            } // if iContainer

        // System events need to be checked regardless of iContainer
        //
        if( event == TMPXCollectionMessage::EBroadcastEvent )
            {
            if((type == EMcMsgUSBMassStorageStart ) || (type == EMcMsgUSBMTPEnd) || (type == EMcMsgRefreshEnd))
                {
                iCollectionCacheReady = EFalse;
                }
            if(type == EMcMsgRefreshEnd)
                {
                TInt songAdded = aMessage.ValueTObjectL<TInt>(KMPXMessageGeneralData);
                MPX_DEBUG2("EMcMsgRefreshEnd, songAdded = %d", songAdded);
                if (iPopuponRefresh)
                    {   
                    iPopuponRefresh =(songAdded > KErrNone)? ETrue:EFalse;
                    }
     
	            if( NeedToShowEducatingDialog())
                    {  
                    EducatingUserDialog();
                    iPopuponRefresh = EFalse;
                    } 
                } 
            // USB flags
            //
	        CEikMenuBar* menuBar( MenuBar() );
	        TBool IsUSBEvent( EFalse );
            if( type == EMcMsgUSBMassStorageStart || type == EMcMsgUSBMTPStart )
                {
                iUSBOnGoing = ETrue;
		        IsUSBEvent = ETrue;
                }
            else if( type == EMcMsgUSBMassStorageEnd || type == EMcMsgUSBMTPEnd )
                {
                iUSBOnGoing = EFalse;
		        IsUSBEvent = ETrue;
                RProperty::Set( KMPXViewPSUid,
                                KMPXUSBUnblockingPSStatus,
                                EMPXUSBUnblockingPSStatusUninitialized );
                }
            else if( type == EMcMsgUSBMTPNotActive )
                {
		        IsUSBEvent = ETrue;
                RProperty::Set( KMPXViewPSUid,
                                KMPXUSBUnblockingPSStatus,
                                EMPXUSBUnblockingPSStatusActive );
		        }
            if( IsUSBEvent && menuBar && menuBar->IsDisplayed() )
            	{
            	menuBar->StopDisplayingMenuBar();
            	}

            if ( iContainer && iIsEmbedded && type == EMcMsgUSBMassStorageEnd )
            	{
            	DoIncrementalOpenL();
            	}
            } // if event == EBroadcastEvent
        } // if ( KMPXMessageGeneral == id )
    }

// ---------------------------------------------------------------------------
// CMPXCollectionViewHgImp::DoIncrementalOpenL
// Start the incremental fetching algorithm
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoIncrementalOpenL( TBool aShowWaitDlg )
    {
     MPX_FUNC( "CMPXCollectionViewHgImp::DoIncrementalOpenL" );
    // Show the opening note if > 1.5 seconds
    if( aShowWaitDlg )
        {
        //StartWaitNoteL( EMPXOpeningNote );
        }

    // Cancel any reads
    iIncrementalOpenUtil->Stop();

    // Start the read
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    TArray<TMPXAttribute> ary = attrs.Array();
    iIncrementalOpenUtil->SetDelay( KIncrementalDelayNone );
    iIncrementalOpenUtil->StartL( ary, KIncrementalFetchBlockSize,
                                  KErrNotFound, CMPXCollectionOpenUtility::EFetchDown );
    iIncrementalOpenUtil->SetDelay( KIncrementalDelayHalfSecond );
    CleanupStack::PopAndDestroy( &attrs );

    if (iContainer)
     {	 
     // Default empty text
     iContainer->SetLbxEmptyTextL( KNullDesC );
     iContainer->DrawDeferred();
     }
    iFirstIncrementalBatch = ETrue;
    }

// -----------------------------------------------------------------------------
// Start wait note for delayed action due to the items not ready
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::StartDelayedActionL(
    TInt aCommand, TDesC& aNote,TBool aSkipDisplay  )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::StartDelayedActionL" );
    HandleCommandL( EMPXCmdIgnoreExternalCommand );
    if(!aSkipDisplay)
        {
    	iCommonUiHelper->ShowWaitNoteL( aNote, R_AVKON_SOFTKEYS_EMPTY, ETrue, this );
        }

    iCachedCommand = aCommand;
    const CArrayFix<TInt>* array (
        iContainer->CurrentSelectionIndicesL() ); // not owned
    TInt arrayCount( array->Count() );
    iCachedSelectionIndex->Reset();
    for ( TInt i = 0; i < arrayCount; i++ )
        {
        iCachedSelectionIndex->AppendL( array->At( i ) );
        }
    }

// ---------------------------------------------------------------------------
// From MProgressDialogCallback
// Callback method. Get's called when a dialog is dismissed
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DialogDismissedL( TInt aButtonId )
    {
    iDialogDismissed = ETrue;
    MPX_FUNC( "CMPXCollectionViewHgImp::DialogDismissedL" );
    if ( iCommandInitiator )
        {
        iCommandInitiator->CancelCommand( KMediatorSecondaryDisplayDomain,
                                      KMPlayerNoteCategory,
                                      iWaitNoteId );
        }

    switch ( aButtonId )
        {
        case EAknSoftkeyCancel:
            {
            iIsWaitNoteCanceled = ETrue;
            if( iIsDeleting )
                {
                iCollectionUiHelper->Cancel();
                iIsDeleting = EFalse;
                // if delete is interrupted, reopen
                }
            else if ( iSetMediaLCount > 0 )
                {
                // cancel the async request
                iCollectionUtility->Collection().CancelRequest();
                iSetMediaLCount = KErrNotFound;

                DoIncrementalOpenL();
                }
            else if ( iIsAddingToPlaylist )
                {
                // cancel incremental adding of songs
                iCommonUiHelper->CancelCollectionOperation();
                iIsAddingToPlaylist = EFalse;
                }
            break;
            }
        case EAknSoftkeyDone:
            {
            // Double check that we should be showing the dialog
            if( iTitleWait )
                {
                ShowAddedItemsDialogL();
                }
            break;
            }
        default:
            {
            // no special handling for other cases
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection message
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleCollectionMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d a folder
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleOpenL(
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool aComplete,
    TInt aError )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleOpenL 4" );
    iHandlingKeyEvent = EFalse;
    if( aError == KErrNone )
        {
        if ( !iContainer )
            {
            // View is not active. Ignore.
            return;
            }
    #ifdef BACKSTEPPING_INCLUDED
        // handle deferred notification when view is in focus but not for view transitions
        if ( iActivateBackStepping )
            {
	        HandleBacksteppingActivation();
		    }
	#endif // BACKSTEPPING_INCLUDED

        iCollectionReady = aComplete;
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        if( iPodcast && !iDisablePodcasting )
            {
            iPodcast = EFalse;
            UpdateListBoxL( aEntries, KMusicMenuPodcastMenuItemIndex, aComplete );
            ( void ) aIndex;
            }
        else
            {
            TInt topIndex = aIndex;
            if ( iFirstIndexOnScreen > 0 )
                {
                topIndex = iFirstIndexOnScreen;
                if ( aComplete )
                    {
                    iFirstIndexOnScreen = 0;
                    }
                }
            UpdateListBoxL( aEntries, topIndex, aComplete );            
            }
#else
        UpdateListBoxL( aEntries, aIndex, aComplete );
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU
        if ( iContainer )
            {
            iContainer->ContentIsReady( ETrue );
            // this has to be done after the list box media is properly
            // updated
            if ( !iContainer->FindBoxVisibility() )
                {
                MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleOpenL find box not visible" );
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
                CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
                CleanupStack::PushL( cpath );

                CMPXCommonListBoxArrayBase* listboxArray(
                    iContainer->ListBoxArray() );
                const CMPXMedia& media( listboxArray->ContainerMedia() );

                TMPXGeneralType containerType( EMPXNoType );
                if ( media.IsSupported( KMPXMediaGeneralType ) )
                    {
                    containerType = media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
                    }

                TMPXGeneralCategory containerCategory( EMPXNoCategory );
                if ( media.IsSupported( KMPXMediaGeneralCategory ) )
                    {
                    containerCategory = media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
                    }
                TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
                CEikButtonGroupContainer* cba = Cba();
                if ( cba && containerType == EMPXGroup && containerCategory == EMPXAlbum && landscapeOrientation )
                    {
                    cba->SetCommandSetL( R_MPX_COLLECTION_TRANSPARENT_CBA );
                    cba->MakeVisible( EFalse );
					cba->DrawDeferred();
                    }
                else if ( cba )
                    {
                    if ( iContainer->IsTBoneView() )
                        {
                        cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
                        }
                    else
                        {
                        cba->SetCommandSetL(
                            ( cpath->Levels() == 3 && !iIsEmbedded ) ?
                            R_MPX_OPTIONS_HIDE_CBA : R_AVKON_SOFTKEYS_OPTIONS_BACK );
                        }
                    cba->MakeVisible( ETrue );
                    cba->DrawDeferred();
                    }

                CleanupStack::PopAndDestroy(cpath);
#else
                CEikButtonGroupContainer* cba = Cba();
                if ( cba )
                    {
                    cba->SetCommandSetL(
                        ( iViewUtility->ViewHistoryDepth() == 1 && !iIsEmbedded ) ?
                        R_AVKON_SOFTKEYS_OPTIONS_EXIT : R_AVKON_SOFTKEYS_OPTIONS_BACK );
                    cba->DrawDeferred();
                    }
#endif
                }
            else
                {
                MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleOpenL find box visible" );
                }

            CMPXCommonListBoxArrayBase* array(
                iContainer->ListBoxArray() );
            const CMPXMedia& containerMedia = array->ContainerMedia();
            if ( containerMedia.IsSupported( KMPXMediaGeneralTitle ) )
                {
                delete iTitle;
                iTitle = NULL;
                iTitle = containerMedia.ValueText( KMPXMediaGeneralTitle ).AllocL();
                }
            if ( !iContainer->IsInReorderMode() &&
                !iAddingSong &&
                !iInAlbumArtDialog &&
                !iInSongDetails && !iContainer->IsTBoneView())
                {
                UpdateTitlePaneL();
                }

            TMPXGeneralType containerType( EMPXNoType );
            if( containerMedia.IsSupported( KMPXMediaGeneralType ) )
                {
                containerType = containerMedia.ValueTObjectL<TMPXGeneralType>(
                                KMPXMediaGeneralType );
                }

            TMPXGeneralCategory containerCategory( EMPXNoCategory );
            if( containerMedia.IsSupported( KMPXMediaGeneralCategory ) )
                {
                containerCategory = containerMedia.ValueTObjectL<TMPXGeneralCategory>(
                                    KMPXMediaGeneralCategory );
                }
            MPX_DEBUG3( "CMPXCollectionViewHgImp::HandleOpenL container type = %d, category = %d", containerType, containerCategory );

            CEikMenuBar* menuBar( MenuBar() );
            TInt resId( KErrNotFound );
#ifdef __ENABLE_MSK
            TInt mskId( R_QTN_MSK_OPEN );
#endif // __ENABLE_MSK
            if ( iIsEmbedded )
                {
                // embedded playlist view
                iContainer->EnableMarking( ETrue );
                menuBar->SetMenuTitleResourceId( R_MPX_COLLECTION_VIEW_MENUBAR_EMBEDDED_PLAYLIST_VIEW );
                resId = R_MPX_VMP_NO_SONGS;
#ifdef __ENABLE_MSK
                mskId = R_QTN_MSK_PLAY;
#endif // __ENABLE_MSK
                }
            else
                {
                if ( containerType == EMPXGroup && containerCategory == EMPXCollection )
                    {
                    // music menu view
                    iContainer->EnableMarking( EFalse );
                    menuBar->SetMenuTitleResourceId( R_MPX_COLLECTION_VIEW_MENUBAR_MUSIC_MENU );
                    }
                else if ( ( containerType == EMPXGroup && containerCategory == EMPXPlaylist ) || // playlist view
                    ( containerType == EMPXGroup && containerCategory == EMPXArtist ) || // artist view
                    ( containerType == EMPXItem && containerCategory == EMPXArtist ) || // artist/album view
                    ( containerType == EMPXGroup && containerCategory == EMPXAlbum ) || // album view
                    ( containerType == EMPXGroup && containerCategory == EMPXGenre ) || // genre view
                    ( containerType == EMPXGroup && containerCategory == EMPXComposer ) ) // composer view
                    {
                    iContainer->EnableMarking( EFalse );
                    menuBar->SetMenuTitleResourceId( R_MPX_COLLECTION_VIEW_MENUBAR_NO_MARKING );
                    switch ( containerCategory )
                        {
                        case EMPXArtist:
                            {
                            if ( containerType == EMPXGroup )
                                {
                                // artist view
                                resId = R_MPX_COLLECTION_ARTIST_LBX_EMPTYTEXT;
                                }
                            else if ( containerType == EMPXItem )
                                {
                                // artist/album view
                                resId = R_MPX_COLLECTION_ALBUM_LBX_EMPTYTEXT;
                                }
                            else
                                {
                                // should not reach here
                                ASSERT( 0 );
                                }
                            break;
                            }
                        case EMPXAlbum:
                            {
                            // album view
                            resId = R_MPX_VMP_NO_ALBUMS;
                            break;
                            }
                        case EMPXGenre:
                            {
                            // genre view
                            resId = R_MPX_VMP_NO_GENRES;
                            break;
                            }
                        case EMPXPlaylist:
                            {
                            // no special handling for playlist
                            break;
                            }
                        default:
                            {
                            // should not reach here
                            ASSERT( 0 );
                            break;
                            }
                        }
                    }
                else
                    {
                    // tracks view
                    iContainer->EnableMarking( EFalse );
                    menuBar->SetMenuTitleResourceId( R_MPX_COLLECTION_VIEW_MENUBAR_NO_MARKING );
                    resId = R_MPX_VMP_NO_SONGS;
#ifdef __ENABLE_MSK
                    mskId = R_QTN_MSK_PLAY;
#endif // __ENABLE_MSK
                    }
                }

            if ( resId != KErrNotFound )
                {
                HBufC* emptyText = StringLoader::LoadLC( resId );
                iContainer->SetLbxEmptyTextL( *emptyText );
                CleanupStack::PopAndDestroy( emptyText );
                iContainer->DrawDeferred();
                }

#ifdef __ENABLE_MSK
            // set middle softkey label display
            UpdateMiddleSoftKeyDisplayL( mskId );
#endif // __ENABLE_MSK

            switch ( iCurrentPlaylistOp )
                {
                case EMPXOpPLCreating:
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleOpenL searching for playlist" );
                    // after creating a new playlist
                    // playlist created callback will always come before this
                    // iPlaylistId should have already been set, check to see if
                    // we're in playlist view
                    if ( iPlaylistId != 0 &&
                        containerType == EMPXGroup && containerCategory == EMPXPlaylist )
                        {
                        const CMPXMediaArray& mediaArray = array->MediaArray();
                        TInt arrayCount( mediaArray.Count() );
                        for ( TInt i = 0; i < arrayCount; i++ )
                            {
                            CMPXMedia* origMedia( mediaArray.AtL( i ) );
                            TMPXItemId plId = origMedia->ValueTObjectL<TMPXItemId>(
                                KMPXMediaGeneralId );
                            if ( plId == iPlaylistId )
                                {
                                MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleOpenL Playlist match found 0x%x", iPlaylistId.iId1 );
                                iCurrentPlaylistOp = EMPXOpPlOpening;
                                iContainer->SetLbxCurrentItemIndex( i );
                                HandleListBoxEventL( NULL, // not used
                                    EEventEnterKeyPressed );
                                break;
                                }

                            // Stay in Creating state as every OpenL()
                            // will be called back with a HandleOpenL()
                            // During playlist creation, there is a race condition
                            // between HandleCollectionMessage() -> Collection change -> OpenL()
                            // and playlist added's  OpenL()
                            }
                        }
                    break;
                    }
                case EMPXOpPlOpening:
                    {
                    // check how many songs are stored in collection
                    iCurrentMediaLOp = EMPXOpMediaLNewPlaylistCollectionSongCount;
                    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
                    CleanupStack::PushL( path );
                    path->Back();
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL( attrs );
                    attrs.Append( KMPXMediaColDetailNumberOfItems );
                    iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
                    CleanupStack::PopAndDestroy( &attrs );
                    CleanupStack::PopAndDestroy( path );
                    iCurrentPlaylistOp = EMPXOpPLIdle;
                    break;
                    }
                case EMPXOpPLIdle: // fall through
                default:
                    {
                    // Update Duration
                    //
                    if ( !iContainer->IsInReorderMode() && !iAddingSong )
                        {
                        GetDurationL();
                        }
                    break;
                    }
                }
            iPlaylistId = 0;
            }
        }
    else
        {
        if ( iContainer )
            {
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
            CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
            CleanupStack::PushL( cpath );

            // media is not valid until the first HandleOpenL call
            CEikButtonGroupContainer* cba = Cba();
            if ( cba )
                {
                cba->SetCommandSetL(
                    ( cpath->Levels() == 2 && !iIsEmbedded ) ?
                    R_AVKON_SOFTKEYS_OPTIONS_BACK : R_AVKON_SOFTKEYS_OPTIONS_BACK );
                cba->DrawDeferred();
                }

            CleanupStack::PopAndDestroy(cpath);
#else
            CEikButtonGroupContainer* cba = Cba();
            if ( cba )
                {
                cba->SetCommandSetL(
                    ( iViewUtility->ViewHistoryDepth() == 1 && !iIsEmbedded ) ?
                    R_AVKON_SOFTKEYS_OPTIONS_EXIT : R_AVKON_SOFTKEYS_OPTIONS_BACK );
                cba->DrawDeferred();
                }

#endif
            }
        if (aError == KErrNotFound)
            {
            HandleCommandL(EAknSoftkeyBack);
            }
        else
            {
            HandleError( aError );
            }
        }
    if ( iIsDeleting )
        {
        // nothing else to delete
        iIsDeleting = EFalse;
        if ( iWaitDialog )
            {
            iWaitDialog->ProcessFinishedL();
            }
        HandleCommandL( EMPXCmdHandleExternalCommand );
        }
    if ( iPlayIndex > KErrNotFound )
        {
        CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
        CleanupStack::PushL( cpath );
        cpath->Set( iPlayIndex );
        iPlayIndex = KErrNotFound;
        iCollectionUtility->Collection().OpenL( *cpath, EMPXOpenPlaylistOnly );
        iFirstIncrementalBatch = ETrue;
        CleanupStack::PopAndDestroy( cpath );
        }
    if ( ( iCachedCommand != KErrNotFound ) && iCollectionReady )
        {
        if((iCachedCommand != EMPXCmdRemove) && (iCachedCommand != EMPXCmdCommonDelete) && (iCachedCommand != EMPXCmdDelete) )
            {
        iCommonUiHelper->DismissWaitNoteL();
        HandleCommandL( EMPXCmdHandleExternalCommand );
            }
        else
            {
            iCachedCommand = EMPXCmdRemove;
            }
        if( iContainer )
            {
            iContainer->SetCurrentSelectionIndicesL( iCachedSelectionIndex );
            }
        HandleCommandL( iCachedCommand );
        iCachedSelectionIndex->Reset();
        iCachedCommand = KErrNotFound;
        }

    // Close the opening wait note, if the current item is valid or all open completes
    if ( iContainer && !aComplete )
        {
        // if the list is empty, it shouldn't go in this path as aComplete should be ETrue
        CMPXCommonListBoxArrayBase* listboxArray(
            iContainer->ListBoxArray() );
        const CMPXMedia& currentMedia( listboxArray->MediaL( aIndex ) );
        }
    else
        {
        if ( EMPXOpeningNote == iNoteType || EMPXImmediatelyOpeningNote == iNoteType )
            {
            CloseWaitNoteL();
            }
        }

    MPX_PERF_CHECKPT("Collection View opened");
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleOpenL 2" );
    iHandlingKeyEvent = EFalse;
    // Do Nothing: playback/fetch client should handle this stage
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle media properties
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleCollectionMediaL" );

    TInt nextMediaLOp( EMPXOpMediaLIdle );
    if ( aError == KErrNone )
        {
        switch ( iCurrentMediaLOp )
            {
            case EMPXOpMediaLGetContainerInfo:
                {
                delete iDuration;
                iDuration = NULL;
                if ( aMedia.IsSupported( KMPXMediaGeneralDuration ) )
                    {
                    TInt duration(
                        aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralDuration ) );
                    if ( duration > 0 )
                        {
                        // only display duration when the list contains something
                        iDuration = iCommonUiHelper->DisplayableDurationInTextL( duration/KMilliSecondsToSeconds );
                        }
                    }
                break;
                }
            case EMPXOpMediaLSend:
                {
                DoHandleSendL( aMedia );
                if ( !iContainer->FindBoxVisibility() )
                    {
                    iContainer->RestoreFindBoxPopupCBA();
                    }
                break;
                }
            case EMPXOpMediaLSongDetailsFileCheck:
                {
                if ( iContainer )
                    {
                    DoHandleFileCheckResultL(
                        TUid::Uid( KMPXPluginTypeMetadataEditorUid ), aMedia );
                    if ( !iContainer->FindBoxVisibility() )
                        {
                        iContainer->RestoreFindBoxPopupCBA();
                        }
                    }
                break;
                }
            case EMPXOpMediaLAlbumArtFileCheck:
                {
                if ( iContainer )
                    {
                    DoHandleFileCheckResultL(
                        TUid::Uid( KMPXPluginTypeAlbumArtEditorUid ), aMedia );

                    if ( !iContainer->FindBoxVisibility() )
                        {
                        iContainer->RestoreFindBoxPopupCBA();
                        }
                    }
                break;
                }
            case EMPXOpMediaLCollectionDetails:
                {
                DoShowCollectionDetailsL( aMedia );
                break;
                }
            case EMPXOpMediaLPlaylistDetails:
                {
                DoShowPlaylistDetailsL( aMedia );
                break;
                }
            case EMPXOpMediaLRenamePlaylist:
                {
                DoHandleRenamePlaylistL( aMedia );
                break;
                }
            case EMPXOpMediaLNewPlaylistCollectionSongCount:
                {
                TInt songsCount( 0 );
                if ( aMedia.IsSupported( KMPXMediaColDetailNumberOfItems ) )
                    {
                    songsCount =
                        aMedia.ValueTObjectL<TInt>( KMPXMediaColDetailNumberOfItems );
                    }
                if ( songsCount > 0 )
                    {
                    // show add songs prompt only if collection contains
                    // at least one song
                    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleCollectionMediaL openning add songs view, collection contains %d songs", songsCount );
                    CAknQueryDialog* confirmationDlg = CAknQueryDialog::NewL(
                        CAknQueryDialog::EConfirmationTone );
                    CleanupStack::PushL( confirmationDlg );
                    HBufC* promptTxt = StringLoader::LoadLC(
                        R_MPX_QTN_NMP_QUERY_ADD_SONGS );
                    confirmationDlg->SetPromptL( *promptTxt );
                    CleanupStack::PopAndDestroy( promptTxt );
                    confirmationDlg->PublishDialogL(
                        EMPlayerQueryAddSongsToPlaylist,
                        KMPlayerNoteCategory);
                    CleanupStack::Pop( confirmationDlg );
                    AppUi()->StopDisplayingMenuBar();
                    if ( confirmationDlg->ExecuteLD( R_MPX_CUI_ADD_SONGS_CONFIRMATION_QUERY ) )
                        {
                        HandleCommandL( EMPXCmdAddSongs );
                        iCurrentMediaLOp = EMPXOpMediaLNewPlaylistCollectionSongCount;
                        }
                    }
#ifdef _DEBUG
                else
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleCollectionMediaL collection empty, not opening add songs view" );
                    }
#endif // _DEBUG
                break;
                }
            case EMPXOpMediaLCopyToRemote:
                {
                if ( iUpnpFrameworkSupport )
                    {
                    DoHandleCopyToRemoteL( aMedia );
                    }
                break;
                }

            case EMPXOpMediaLFindInMusicShopSongDetails:
                {
                const TDesC& location = aMedia.ValueText( KMPXMediaGeneralUri );
                if ( location != KNullDesC && ConeUtils::FileExists( location ) )
                    {
                    HBufC* buf = HBufC::NewLC( 5 ); // magic number to hold index value
                    buf->Des().AppendNum(
                        iContainer->CurrentLbxItemIndex() );
                    // set current list box index
                    SetParamL( buf );
                    CleanupStack::PopAndDestroy( buf );

                    DoGetSongMediaPropertyL();
                    // Set the next operation to handle MediaL completed.
                    nextMediaLOp = EMPXOpMediaLFindInMusicShopMediaLCompleted;
                    }
                else
                    {
                    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                    CleanupStack::PushL( cpath );
                    TInt currentDepth( cpath->Levels() );
                    CleanupStack::PopAndDestroy( cpath );
                    CMPXCommonListBoxArrayBase* listboxArray =
                        iContainer->ListBoxArray();
                    TInt currentIndex =
                        iContainer->CurrentLbxItemIndex();
                    const CMPXMedia& currentMedia = listboxArray->MediaL( currentIndex );
                    TMPXGeneralCategory category(
                        currentMedia.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ) );
                    const CMPXMediaArray& mediaAry( listboxArray->MediaArray() );
                    CMPXMedia* origMedia( mediaAry.AtL( currentIndex ) );
                    const TDesC& title( origMedia->ValueText( KMPXMediaGeneralTitle ) );
                    if ( ( title.Length() > 0 ) && ( title.Length() < KMaxFileName ) )
                        {
                        switch ( category )
                            {
                            case EMPXAlbum:
                                {
                                const CMPXMedia& containerMedia = listboxArray->ContainerMedia();
                                // The Artist/Album View Title shows the Artist name
                                const TDesC& titleArtist =
                                        containerMedia.ValueText( KMPXMediaGeneralTitle );
                                HBufC* unknownArtistText =
                                        StringLoader::LoadLC( R_MPX_QTN_NMP_UNKNOWN_ARTIST );
                                // 4 is the level of Artist and Album View
                                if ( currentDepth == 4 && titleArtist != unknownArtistText->Des() )
                                    {
                                    DoFindInMusicShopL(KCategoryEmpty, titleArtist, title); // Artist and Album View
                                    }
                                else
                                    {
                                    DoFindInMusicShopL(KCategoryEmpty, KCategoryEmpty, title); // Album View
                                    }
                                CleanupStack::PopAndDestroy( unknownArtistText );
                                break;
                                }
                            case EMPXArtist:
                                {
                                DoFindInMusicShopL(KCategoryEmpty, title, KCategoryEmpty);  // Artist only
                                break;
                                }
                            case EMPXSong:
                                {
                                DoFindInMusicShopL(title, KCategoryEmpty, KCategoryEmpty);  // Song only
                                break;
                                }
                            default:
                                {
                                break;
                                }
                            } // switch
                        } // if
                    }  // else
                break;
                }
            case EMPXOpMediaLFindInMusicShopMediaLCompleted:
                {
                // call DoFindInMusicShop when SongName, AlbumName, and ArtistName are
                // available at this point
                DoFindInMusicShopL(
                    aMedia.ValueText( KMPXMediaGeneralTitle ),
                    aMedia.ValueText( KMPXMediaMusicArtist ),
                    aMedia.ValueText( KMPXMediaMusicAlbum ) );
                break;
                }

            default:
                {
                break;
                }
            }
        }
    else
        {
        /* If a Composer,Artist or Album consists single song.
         * On changing the song details(composer+genre or album name+genre)
         * No more record exist corresponding to previous data.
         * In this case no need to show Error Message. */
        if( aError != KErrNotFound)
            {
            HandleError( aError );
            }
        else
            {
            MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleCollectionMediaL ERROR: %d",aError);
            }
        }
    if ( iCurrentMediaLOp == EMPXOpMediaLNewPlaylistCollectionSongCount )
        {
        iCurrentMediaLOp = EMPXOpMediaLGetContainerInfo;
        }
    else
        {
        iCurrentMediaLOp = nextMediaLOp;
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionHelperObserver
// Callback function after executing a command
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleCommandComplete(CMPXCommand* /*aCommandResult*/,
                                                  TInt /*aError*/)
    {
    // Make sure we are doing the right operation
    if( iSetMediaLCount )
        {
        iSetMediaLCount--;
        if ( iSetMediaLCount < 1 )
            {
            TRAP_IGNORE( HandleRenameOpCompleteL() );
            }
        }
    }
// ---------------------------------------------------------------------------
// From MMPXCollectionHelperObserver
// Callback function to signal that adding a file is complete
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleAddFileCompleteL( TInt aErr )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleAddFileCompleteL" );
    if ( aErr != KErrNone )
        {
        HandleError( aErr );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCHelperObserver
// Handles the completion of helper events
// ---------------------------------------------------------------------------
//

void CMPXCollectionViewHgImp::HandleOperationCompleteL( TCHelperOperation aOperation,
                                           TInt aErr,
                                           void* aArgument )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleOperationCompleteL" );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleOperationCompleteL aErr = %d", aErr );
    switch( aOperation )
        {
        case EDeleteOp:
            {
            MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleOperationCompleteL Delete operation" );
            // do not dismiss the wait note until HandleOpenL is called,
            // unless it's error case
            delete iCurrentCba;
            iCurrentCba = NULL;
            if( aErr == KErrInUse )
                {
                if ( !iIsWaitNoteCanceled )
                    {
                    // only dismiss wait note if the wait note is not
                    // canceled
                    if ( iWaitDialog )
                        {
                        iWaitDialog->ProcessFinishedL();
                        }
                    HandleCommandL( EMPXCmdHandleExternalCommand );
                    iIsWaitNoteCanceled = EFalse;
                    }
                if ( iIsDeleting )
                    {
                    HBufC* text = StringLoader::LoadLC(
                        ( iContainer->CurrentSelectionIndicesL()->Count() == 0 ) ?
                        R_MPX_COLLECTION_NOTE_DELETE_FAIL : // Single selection
                        R_MPX_COLLECTION_NOTE_DELETE_FAIL_MULTI_SELECTION ); // Multi selection

                    CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
                    dlg->ExecuteLD( *text );
                    CleanupStack::PopAndDestroy( text );
                    }
                iIsDeleting = EFalse;
                }
            else if ( aErr != KErrNone )
                {
                iIsDeleting = EFalse;
                if ( !iIsWaitNoteCanceled )
                    {
                    if ( iWaitDialog )
                    	{
						iWaitDialog->ProcessFinishedL();
						}
                    HandleCommandL( EMPXCmdHandleExternalCommand );
                    iIsWaitNoteCanceled = EFalse;
                    }
                HandleError( aErr );
                }
            if ( iContainer )
                {
                iContainer->ClearLbxSelection();
                }
            MPX_PERF_END_EX( MPX_PERF_SHOW_WAITNOTE );
            // reopen collection
            DoIncrementalOpenL( !iIsDeleting );
            break;
            }
        case EExportPlaylistOp:
            {
            MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleOperationCompleteL Export playlist operation" );
            if ( aErr == KErrNone )
                {
                HBufC* uri = static_cast<HBufC*>( aArgument );
                CleanupStack::PushL(uri);
                MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleOperationCompleteL playlist exported as %S", uri );

                CMessageData* messageData = CMessageData::NewLC();
                messageData->AppendAttachmentL( *uri );
                RFs& fileSession = iCoeEnv->FsSession();
                TEntry pl;
                TInt err( fileSession.Entry( *uri, pl ) );
                if ( err == KErrNone )
                    {
                    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleOperationCompleteL playlist size = %d", pl.iSize );
                    if ( !iSendUi )
                        {
                        iSendUi = CSendUi::NewL();
                        }
                    TSendingCapabilities capabilities(
                        0,
                        pl.iSize,
                        TSendingCapabilities::ESupportsAttachments );

                    // sendui leaves with error code 0 when canceling sending
                    // via bluetooth, this should be trapped
                    MPX_TRAP( err, iSendUi->ShowQueryAndSendL( messageData, capabilities ) );
                    if ( err != KErrNone )
                        {
                        HandleError( err );
                        }
                    // remove the file then remove the directory
                    err = fileSession.Delete( *uri );
                    if ( err != KErrNone )
                        {
                        HandleError( err );
                        }
                    err = fileSession.RmDir( *uri );
                    if ( err != KErrNone )
                        {
                        HandleError( err );
                        }
                    }
                else
                    {
                    HandleError( err );
                    }
                CleanupStack::PopAndDestroy( 2, uri ); // messageData & uri
                aArgument = NULL;
                }
            else
                {
                HandleError( aErr );
                }
            break;
            }
        case EAddOp:
            {
            HandleAddCompletedL( (CMPXMedia*)aArgument, aErr );
            aArgument = NULL; // ownership transferred
            iIsAddingToPlaylist = EFalse;  // reset flag once incremental adding is finished
            break;
            }
        case ESetOp:
            {
            if ( iAddingToNewPlaylist )
                {
                // if user selects overwrite existing playlist,
                // the OP will be set, and is handled here
                HandleAddCompletedL( (CMPXMedia*)aArgument, aErr );
                }
            else
                {
                HandleSetCompletedL( (CMPXMedia*)aArgument, aErr );
                }
            aArgument = NULL; // ownership transferred
            break;
            }
        case EReorderPlaylistOp:
            {
            HandleSetCompletedL( (CMPXMedia*)aArgument, aErr );
            aArgument = NULL; // ownership transferred
            break;
            }
        case ERenameOp:
            {
            if ( aErr != KErrNone )
                {
                iCommonUiHelper->HandleErrorL( aErr );
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if( aArgument )
        {
        delete ( CBase* )aArgument;
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandlePlaybackMessage" );
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    // Fix EBLI-7CFE4D press select key very quick cause System Error KErrArgument
    else
        {
        // Fix for EJDI-77WCRF Low memory cause MP not resp.
        iHandlingKeyEvent = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandlePropertyL(
    TMPXPlaybackProperty /*aProperty*/,
    TInt /*aValue*/,
    TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media event.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleMediaL(
    const CMPXMedia& /*aMedia*/,
    TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXCollectionViewHgImp::Id() const
    {
    return TUid::Uid( KMPXCollectionViewHgId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleCommandL( TInt aCommand )
    {
    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleCommandL(0x%x) entering", aCommand );

    switch (aCommand)
        {
        case EAknCmdMark:
        case EAknCmdUnmark:
        case EAknMarkAll:
        case EAknUnmarkAll:
            {
            // Custom handling of markable lists
            iContainer->HandleMarkableListProcessCommandL(
                aCommand );
#ifdef __ENABLE_MSK
            // Determine middle soft key labeling
            UpdateMiddleSoftKeyDisplayL( iCurrentMskId );
#endif // __ENABLE_MSK
            if (aCommand == EAknMarkAll && !iCollectionReady)
                {
                iMarkedAll = ETrue;
                }
            if (aCommand == EAknUnmarkAll || aCommand == EAknCmdUnmark)
                {
                iMarkedAll = EFalse;
                }
            break;
            }
        case EMPXCmdGoToNowPlaying:
            {
            iGoToNowPlaying = ETrue;
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        case EMPXCmdGoToAllSongs:
            {
            OpenAllSongsL();
            break;
            }
        case EMPXCmdGoToArtistAlbums:
            {
            OpenArtistAlbumsL();
            break;
            }
        case EMPXCmdGoToPlaylists:
            {
            OpenPlaylistsL();
            break;
            }
        case EMPXCmdGoToGenre:
            {
            OpenGenreL();
            break;
            }
        case EMPXCmdGoToPodcasts:
            {
            OpenPodcastsL();
            break;
            }
        case EMPXCmdPlayItem:
            {
            iContainer->HandleItemCommandL(EMPXCmdPlay);
            break;
            }
        case EMPXCmdCreatePlaylist:
            {
            iCurrentPlaylistOp = EMPXOpPLCreating;
            CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
            CleanupStack::PushL( mediaArray );
            CMPXMedia* media = CMPXMedia::NewL();
            CleanupStack::PushL( media );
            media->SetCObjectValueL( KMPXMediaArrayContents, mediaArray );
            media->SetTObjectValueL( KMPXMediaArrayCount, 0 );
            TInt ret( iCommonUiHelper->CreatePlaylistL( *media, this, this ) );
            if ( !ret )
                {
                // create playlist failed,
                iCurrentPlaylistOp = EMPXOpPLIdle;
                }
            CleanupStack::PopAndDestroy( media );
            CleanupStack::PopAndDestroy( mediaArray );
            break;
            }
        case EMPXCmdAddToNewPlaylist:
        case EMPXCmdAddToSavedPlaylist:
            {
            // flag used only in the case when songs are added incrementally
            iIsAddingToPlaylist = ETrue;
            iNumSongAddedToPlaylist = -1;
      
            CMPXCommonListBoxArrayBase* listboxArray(
                   iContainer->ListBoxArray() );
                
            const CMPXMedia& currentMedia = listboxArray->MediaL(
                iContainer->CurrentLbxItemIndex() );
            
            TMPXGeneralCategory mediaCategory( currentMedia.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ));
                 
            const CMPXMedia& containerMedia = listboxArray->ContainerMedia();
       
            TMPXGeneralType containerType( EMPXNoType );
            if ( containerMedia.IsSupported( KMPXMediaGeneralType ) )
                 {
                 containerType = containerMedia.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
                 }

            TMPXGeneralCategory containerCategory( EMPXNoCategory );
            if ( containerMedia.IsSupported( KMPXMediaGeneralCategory ) )
                 {
                 containerCategory = containerMedia.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
                 }

            if ( mediaCategory == EMPXSong || iContainer->IsTBoneView() )
                {
                CMPXMedia* tracks = CMPXMedia::NewL();
                CleanupStack::PushL( tracks );

                MPX_TRAPD( err, PrepareMediaForSelectedItemsL( *tracks, containerCategory, containerType ) );
                if ( err == KErrNone )
                    {
                    TBool ret = EFalse;
                    if ( aCommand == EMPXCmdAddToNewPlaylist )
                        {
                        iAddingToNewPlaylist = ETrue;
                        MPX_TRAP( err,ret = iCommonUiHelper->CreatePlaylistL(
                                *tracks, this, this ) );
                        if ( err != KErrNone )
                            {
                            HandleError( err );
                            }
                        if ( !ret )
                            {
                            iAddingToNewPlaylist = EFalse;
                            }
                        }
                    else
                        {
                        MPX_TRAP( err, ret = iCommonUiHelper->AddToSavedPlaylistL(
                            *iUserPlaylists, *tracks, this, this ) );
                        if ( err != KErrNone )
                            {
                            HandleError( err );
                            }
                        }
#ifdef __ENABLE_MSK
                     // If Save play list or create play list return True (OK)
                     //  1 - Unselect marks
                     //  2 - Force to refresh the MSK with title "Play"
                     // If Save play list or create play list return False (Cancel)
                     //  1 - let the marks
                     //  2 - let the MSK with the "context menu"
                     if (ret)
                       {
                       iContainer->ClearLbxSelection();
                       UpdateMiddleSoftKeyDisplayL( iCurrentMskId );
                       }
#endif // __ENABLE_MSK
                    }
                else if ( err == KMPXErrDataNotReadyCacheCommand )
                    {
                    // cache this command
                    HBufC* text = StringLoader::LoadLC(
                        R_MPX_QTN_MUS_NOTE_ADDING_TRACKS );
                    StartDelayedActionL( aCommand, *text );
                    CleanupStack::PopAndDestroy( text );
                    }
                else if ( err != KMPXErrDataNotReady )
                    {
                    HandleError( err );
                    }
                else
                    {
                    // KMPXErrDataNotReady, ignore
                    }
                CleanupStack::PopAndDestroy( tracks );
                }
            else
                {
                // on artist/album level
                if ( aCommand == EMPXCmdAddToNewPlaylist )
                    {
                    iAddingToNewPlaylist = ETrue;
                    iCurrentFindAllLOp = EMPXOpFindAllLAlbumArtistAddToNewPlaylist;
                    }
                else
                    {
                    iCurrentFindAllLOp = EMPXOpFindAllLAlbumArtistAddToSavedPlaylist;
                    }
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL( attrs );
                attrs.Append(
                    TMPXAttribute( KMPXMediaIdGeneral,
                    EMPXMediaGeneralTitle | EMPXMediaGeneralId |
                    EMPXMediaGeneralType | EMPXMediaGeneralCategory ) );
                CMPXMedia* criteria = CMPXMedia::NewL();
                CleanupStack::PushL( criteria );
                if ( containerCategory == EMPXArtist )
                    {
                    // artist/album level, need to specify artist ID in container ID
                    TMPXItemId containerId(
                        containerMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                    criteria->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralContainerId, containerId );
                    }
            
                TMPXItemId id(
                    currentMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                criteria->SetTObjectValueL<TMPXGeneralType>(
                    KMPXMediaGeneralType, EMPXGroup );
                criteria->SetTObjectValueL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory, EMPXSong );
                criteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
                iCollectionUtility->Collection().FindAllL(
                    *criteria, attrs.Array(), *this );
                CleanupStack::PopAndDestroy( criteria );
                CleanupStack::PopAndDestroy( &attrs );
                }
            break;
            }
        case EMPXCmdAddSongs:
            {
            iAddingSong = ETrue;
            iViewUtility->ActivateViewL(
                TUid::Uid( KMPXPluginTypeAddSongsEditorUid ) );
            break;
            }
        case EMPXCmdReorder:
            {
            ActivateReorderGrabbedModeL();
            break;
            }
        case EMPXCmdDelete: // fall through
        case EMPXCmdRemove:
            {
            StoreListboxItemIndexL();
            if ( iContainer->IsTBoneView() )
                {
                DeleteSelectedTBoneItemsL(aCommand);
                }
            else
                {
                DeleteSelectedItemsL(aCommand);    
                }
            break;
            }
        case EMPXCmdSend:
            {
            const CMPXMedia& media =
                iContainer->ListBoxArray()->MediaL(
                    iContainer->CurrentLbxItemIndex() );

            TMPXGeneralType type(
                media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
            TMPXGeneralCategory category(
                media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ) );

            if( type == EMPXItem && category == EMPXAlbum && 
                    iContainer->IsTBoneView() )
                {
                const TMPXItemId containerId = media.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                DoSendTBoneListItemL(containerId);
                }
            else if ( iContainer->CurrentSelectionIndicesL()->Count() == 0 &&
                type == EMPXItem &&
                category == EMPXPlaylist )
                {
                // sending a playlist
                DoSendPlaylistL( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                }
            else
                {
                DoSendL();
                }
            break;
            }
        case EMPXCmdSongDetails: // fall through
        case EMPXCmdAlbumArt: // fall through
        case EMPXCmdFindInMusicShop:
            {
            StoreListboxItemIndexL();

            CMPXCommonListBoxArrayBase* listboxArray(
                iContainer->ListBoxArray() );
            const CMPXMedia& media( listboxArray->MediaL(
                iContainer->CurrentLbxItemIndex() ) );
            if ( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId )
                {
                // highlighted item is not yet available
                MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleCommandL item not yet fetched" );
                }
            else
                {
                // need to check if the file exists.  if the file doesn't exist
                // and mediaL is called with drm status, it'll leave
                if ( aCommand == EMPXCmdSongDetails )
                    {
                    iCurrentMediaLOp = EMPXOpMediaLSongDetailsFileCheck;
                    iInSongDetails = ETrue;
                    }
                else if ( aCommand == EMPXCmdAlbumArt )
                    {
                    iInAlbumArtDialog = ETrue;
                    iCurrentMediaLOp = EMPXOpMediaLAlbumArtFileCheck;
                    }
                else if ( aCommand == EMPXCmdFindInMusicShop )
                    {
                    iCurrentMediaLOp = EMPXOpMediaLFindInMusicShopSongDetails;
                    }
                CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL( path );
                
                if ( 2 == path->Levels() )
                     {
                     path->Back();
                     path->AppendL(3);
                     
                     const CMPXMediaArray& albums = listboxArray->MediaArray();
                     
                     RArray<TMPXItemId> ids;
                     CleanupClosePushL(ids);

                     TInt albumCount = albums.Count();
                      for (TInt i=0; i<albumCount; ++i)
                          {
                          CMPXMedia* album = albums.AtL(i);
                          const TMPXItemId id = album->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                          ids.AppendL(id);
                          }
                      path->AppendL(ids.Array()); // top level items
                      ids.Reset();
                      CleanupStack::PopAndDestroy(&ids);             
                     }
                path->Set( iContainer->CurrentLbxItemIndex() );
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL( attrs );
                attrs.Append(
                    TMPXAttribute( KMPXMediaIdGeneral,
                        EMPXMediaGeneralUri | EMPXMediaGeneralCollectionId ) );
                iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
                CleanupStack::PopAndDestroy( &attrs );
                CleanupStack::PopAndDestroy( path );
                }
            break;
            }
        case EMPXCmdGoToMusicShop:
            {
            LaunchMusicShopL();
            break;
            }
        case EMPXCmdGoToNokiaMusicShop:
            {
            LaunchMusicShopL();
            break;
            }
        case EMPXCmdGoToOperatorMusicShop:
            {
            if(iMusicStoreWebPage)
                {
                LaunchOperatorURLMusicShopL();
                }
            else
                {
                if(iOperatorMusicStoreType)
                {
                LaunchOperatorJavaMusicShopL(iOperatorMusicStoreUID);
                }
            else
                {
                LaunchOperatorNativeMusicShopL();
                }
                }
            break;
            }
        case EMPXCmdMusicLibraryDetails:
            {
            iCurrentMediaLOp = EMPXOpMediaLCollectionDetails;
            CMPXCollectionPath* path = iCollectionUiHelper->MusicMenuPathL();
            CleanupStack::PushL( path );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL( attrs );
            attrs.Append( KMPXMediaColDetailAll );
            iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( path );
            break;
            }
        case EMPXCmdRename:
            {
            DoHandleRenameL();
            break;
            }
        case EMPXCmdPlaylistDetails:
            {
            iCurrentMediaLOp = EMPXOpMediaLPlaylistDetails;
            CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( path );
            path->Set( iContainer->CurrentLbxItemIndex() );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL( attrs );
            attrs.Append( TMPXAttribute( KMPXMediaIdGeneral,
                EMPXMediaGeneralCount | EMPXMediaGeneralTitle |
                EMPXMediaGeneralUri | EMPXMediaGeneralDate |
                EMPXMediaGeneralDuration | EMPXMediaGeneralFlags ) );
            iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( path );
            break;
            }
        case EAknSoftkeyBack:
            {
			if (iContainer->OfferCommandL(EAknSoftkeyBack))
				{
				CEikButtonGroupContainer* cba = Cba();
				if (cba)
					{
					cba->SetCommandSetL( R_MPX_OPTIONS_HIDE_CBA );
					cba->MakeVisible( ETrue );
					cba->DrawDeferred();
					}
				break;
				}

            iMarkedAll = EFalse;
            if( iIsDeleting )
            	{
            	break;
            	}

            if ( iIsEmbedded )
                {
                AppUi()->HandleCommandL(
                    ( MPXTlsHelper::NeedSave() && iCommonUiHelper->IsHostMessagingBrowserL() )
                    ? EMPXCmdSaveAndExit : EEikCmdExit );
                }
            else
                {
                if ( !iHandlingKeyEvent )
                    {
                    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                    CleanupStack::PushL( cpath );
                    TInt currentDepth( cpath->Levels() );
                    if ( currentDepth == 2 ) // 2 is the level of music main view
                        {
#ifdef BACKSTEPPING_INCLUDED
                        // let Back Stepping Service handle the event
                        TInt statusInfo( KMPXBackSteppingNotConsumed );
                        if( iViewUtility &&
                            ( iViewUtility->ActiveViewImplementationUid() != KNullUid ) )
                            {
                            TInt viewId( iViewUtility->ActiveViewType().iUid );
                            TBuf8<KMVPrefixLen + KMaxIntLen> buf;
                            buf.Copy( KMVPrefix );
                            buf.AppendNum( viewId );
                            statusInfo = iBackSteppingUtility->HandleBackCommandL( buf );
                            MPX_DEBUG3("CMPXCollectionViewHgImp::HandleCommandL - viewid=0x%x, statusInfo=%d",
                                 viewId, statusInfo );
                            }
                        if ( statusInfo == KMPXBackSteppingNotConsumed )
                            // event not consumed by Back Stepping utility, handle here
                            //
#endif // BACKSTEPPING_INCLUDED
                            {
                            AppUi()->HandleCommandL( aCommand );
                            }
                        }
                    else
                        {
                        if ( currentDepth > 2 && !iBackOneLevel )
                            {
                            iContainer->ClearLbxSelection();
                            iCollectionUtility->Collection().BackL();
                            iFirstIncrementalBatch = ETrue;
                            MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleCommandL Getting container media, currentDepth = %d", currentDepth );
                            cpath->Back();
                            cpath->Back();
                            RArray<TMPXAttribute> attrs;
                            CleanupClosePushL(attrs);
                            iCurrentMediaLOp = EMPXOpMediaLGetContainerInfo;
                            attrs.Append( KMPXMediaGeneralDuration );
                            iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
                            CleanupStack::PopAndDestroy( &attrs );
                            iBackOneLevel = ETrue;
                            }
                        else
                            {
                            MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleCommandL Not getting container media" );
                            }

                        iHandlingKeyEvent = ETrue;
                        }
                    CleanupStack::PopAndDestroy( cpath );
                    }
                else
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleCommandL Busy, ignoring command" );
                    }
                }
            break;
            }
        case EMPXCmdSavePlaylist:
            {
            AppUi()->HandleCommandL( EMPXCmdSaveAndReopen );
            break;
            }
        case EMPXCmdAbout:
            {
            DisplayAboutDlgL();
            break;
            }    
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        case EMPXCmdCancelReorder:
            {
            if ( iContainer && iContainer->IsInReorderMode() )
                {
                TRAP_IGNORE( ProcessCommandL( EMPXCmdReorderCancel ) );
                iCurrentHighlightedIndex = KErrNotFound;
                }
            break;
            }
        case EMPXCmdUpnpPlayViaLocal:
            {
            if ( iUpnpFrameworkSupport )
                {
                SelectNewPlayerL( aCommand );
                }
            break;
            }
        case EMPXCmdUPnPAiwCmdCopyToExternalCriteria:
            {
            if ( iUpnpFrameworkSupport )
                {
                CopySelectedItemsToRemoteL();
                }
            break;
            }

        case EMPXCmdPlay:
        case EMPXCmdForcePlay:
        case EMPXCmdPlayPause:
            {
            TBool isReady( ETrue );
            TMPXPlaybackState state( iPlaybackUtility->StateL() );

            // if it's in paused state, resume playback, no need
            // to check if current highlight is a playlist
            if ( iContainer &&
                aCommand != EMPXCmdForcePlay &&
                state != EPbStatePaused && state != EPbStatePlaying )
                {
                CMPXCommonListBoxArrayBase* listboxArray =
                    iContainer->ListBoxArray();
                if ( iContainer->CurrentListItemCount() > 0 )
                    {
                    TInt currentItem( iContainer->CurrentLbxItemIndex() );
                    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleCommandL currentItem %d", currentItem );
                    if (currentItem == KErrNotFound)
                        {
                        // no item selected
                        // start playing previous playlist
                        if ( iPlaybackUtility->Source() )
                            {
                            iPlaybackUtility->CommandL( EPbCmdPlay );
                            HandleCommandL( EMPXCmdGoToNowPlaying );
                            }
                        isReady = EFalse;
                        } 
                    else 
                        {
                        const CMPXMedia& media = listboxArray->MediaL( currentItem );
                        TMPXGeneralType type(
                            media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
                        TMPXGeneralCategory category(
                            media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ) );
                        if ( category == EMPXPlaylist && type == EMPXItem )
                            {
                            MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleCommandL checking item count in playlist" );
                            isReady = EFalse;
                            TMPXItemId id(
                                media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                            CMPXMedia* entry = CMPXMedia::NewL();
                            CleanupStack::PushL( entry );
                            entry->SetTObjectValueL<TMPXGeneralType>(
                                KMPXMediaGeneralType, EMPXGroup );
                            entry->SetTObjectValueL<TMPXGeneralCategory>(
                                KMPXMediaGeneralCategory, EMPXSong );
                            entry->SetTObjectValueL<TMPXItemId>(
                                KMPXMediaGeneralId, id );
                            iCurrentFindAllLOp = EMPXOpFindAllLPlaylistTracksCount;
                            RArray<TMPXAttribute> attrs;
                            CleanupClosePushL( attrs );
                            attrs.Append( KMPXMediaGeneralId );
                            iCollectionUtility->Collection().FindAllL(
                                *entry, attrs.Array(), *this );
                            CleanupStack::PopAndDestroy( &attrs );
                            CleanupStack::PopAndDestroy( entry );
                            }
                        }
                    }
                else
                    {
                    // current list view is empty, and no song currently playing
                    // ignore the command
                    isReady = EFalse;
                    }
                }
            if ( isReady )
                {
                if ( aCommand == EMPXCmdForcePlay )
                    {
                    aCommand = EMPXCmdPlay;
                    }
                if (iContainer && (EPbStateNotInitialised == state ||
                     EPbStateStopped == state ))
                    {
                    // Needed to reset the status of iPreservedState
                    if ( EPbStateStopped == state )
                        {
                        iPlaybackUtility->CommandL(EPbCmdResetPreserveState);
                        }
                    TInt currentItem(
                        iContainer->CurrentLbxItemIndex() );
                    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleCommandL playing index %d", currentItem );
                    if ( iContainer->IsInReorderMode() )
                        {
                        // if in reorder mode, and in not playing state
                        // drop the reorder target and start playing
                        if ( iIsGrabbed )
                            {
                            ProcessCommandL( EMPXCmdReorderDrop );
                            }
                        ProcessCommandL( EMPXCmdReorderDone );
                        iPlayIndex = currentItem;
                        }
                    else if ( currentItem != KErrNotFound )
                        {
                        if ( iContainer && !AppUi()->IsDisplayingDialog() )
                            {
                            StopDisplayingMenuBar();
                            }
                        iPlayIndex = KErrNotFound;

                        if (iContainer->IsSelectedItemASong())
							{
							if(iCollectionCacheReady)
								{
								iCollectionUtility->Collection().OpenL( currentItem, EMPXOpenPlaylistOnly );
								iFirstIncrementalBatch = ETrue;
								}
							}
						else
							{
							iContainer->HandleItemCommandL(EMPXCmdPlay);
							}

                        }
                    }
                else
                    {
                    AppUi()->HandleCommandL( aCommand );
                    }
                }
            break;
            }
        case EMPXCmdSaveComplete:
            {
            MPXTlsHelper::SetNeedSave( EFalse );
            break;
            }
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        case EMPXCmdRefreshLibrary:
            {
            MPX_PERF_CHECKPT("Refresh collection library");
            // Package the argument from 8bit to 16bit then activate view
            //
            StartWaitNoteL( EMPXRefreshingNote );
            iPlaybackUtility->CommandL( EPbCmdStop );
            iPopuponRefresh = ETrue;
            break;
            }
#endif
        default:
            {
            MPX_PERF_CHECKPT("Handle Collection view unknown command");
            if ( iUpnpFrameworkSupport )
                {
                if ( aCommand >= EMPXCmdUpnpPlayViaRemotePlayer  &&
                     aCommand < EMPXCmdUpnpLastCommandId )
                    {
                    SelectNewPlayerL( aCommand );
                    }
                else
                    {
                    if ( iServiceHandler->ServiceCmdByMenuCmd(aCommand) != KAiwCmdNone )
                        {
                        CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
                        FillAiwParametersL(paramList);
                        iServiceHandler->ExecuteMenuCmdL(aCommand, paramList,
                            iServiceHandler->OutParamListL());
                        }
                    AppUi()->HandleCommandL( aCommand );
                    }
                }
            else
                {
                if ( iServiceHandler->ServiceCmdByMenuCmd(aCommand) != KAiwCmdNone )
                    {
                    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
                    FillAiwParametersL(paramList);
                    iServiceHandler->ExecuteMenuCmdL(aCommand, paramList,
                        iServiceHandler->OutParamListL());
                    }
                AppUi()->HandleCommandL( aCommand );
                }
            break;
            }
        }
    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleCommandL() exiting" );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoActivateL(
    const TVwsViewId& /* aPrevViewId */,
    TUid /* aCustomMessageId */,
    const TDesC8& /* aCustomMessage */ )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoActivateL()" );

    // Add view deactivation observer
    AppUi()->AddViewDeactivationObserverL( this );

    CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
    CleanupStack::PushL( cpath );
    // media is not valid until the first HandleOpenL call
    CEikButtonGroupContainer* cba = Cba();
    if ( cba )
        {
        cba->SetCommandSetL(( cpath->Levels() == 3 && !iIsEmbedded ) ?
            R_MPX_OPTIONS_HIDE_CBA: R_MPX_OPTIONS_BACK_CBA_NO_ACTION );
        cba->DrawDeferred(); 
        }

    // Set status pane layout if switched here directly from another view,
    // such as when using the AnyKey
    StatusPane()->MakeVisible( ETrue );
    StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT);

    if ( !iContainer )
        {
        iContainer = CMPXCollectionViewHgContainer::NewL( this, this, this, iIsEmbedded );
        iContainer->SetCbaHandler( this );
        }
    iContainer->SetMopParent( this );
    AppUi()->AddToStackL( *this, iContainer );
    iContainer->SetRect( ClientRect() );


	UpdateTitlePaneL();


    if ( iViewUtility->PreviousViewType().iUid == KMPXPluginTypePlaybackUid )
        {
		// Set the previous view for animatin purposes.
		iContainer->SetPreviousViewId(TUid::Uid(KMPXPluginTypePlaybackUid));

#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        // If previous view is Playback view, it can only be normal playback view
        // Podcast shouldn't be highlighted in any case.
        iPodcast = EFalse;
#endif

        // return back from playback view and in songs level,
        // highlight the new playing song
        MMPXSource* source( iPlaybackUtility->Source() );

        if ( source )
            {
            CMPXCollectionPlaylist* playlist( source->PlaylistL() );
            if ( playlist )
                {
                // Going from Now Playing View -> Collection, same collection level
                iPossibleJump = ( playlist->Path().Levels() == cpath->Levels() );
                delete playlist;
                }
            }

        if ( cpath->Levels() > 1 )
            {
            // valid path in collection
            MPX_DEBUG_PATH(*cpath);

            DoIncrementalOpenL();
            }
        else
            {
            TUid defaultView( iViewUtility->DefaultViewUid() );
            iViewUtility->ActivateViewL( ( defaultView != KNullUid ) ?
                defaultView : TUid::Uid( KMPXPluginTypeMainUid ) );
            }
        }
    else
        {
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        // If previous view is Collection view, it can only be podcast view
        if ( iViewUtility->PreviousViewType().iUid == KMPXPluginTypeCollectionUid )
            {
            iPodcast = ETrue;
            }
#endif

        MPX_DEBUG_PATH(*cpath);

        DoIncrementalOpenL( cpath->Levels()>KMusicCollectionMenuLevel ? ETrue:EFalse );

        iUpCount = 0;
        iDownCount = 0;
        }
    CleanupStack::PopAndDestroy( cpath );
    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoDeactivate()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoDeactivate" );
    if ( iContainer && iContainer->IsInReorderMode() )
        {
        TRAP_IGNORE( ProcessCommandL( EMPXCmdReorderDone ) );
        iCurrentHighlightedIndex = KErrNotFound;
        }

    if ( iContainer )
        {
        iContainer->ContentIsReady( EFalse );
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    if (iDuration)
        {
        delete iDuration;
        iDuration = NULL;
        }
    if (iTitle)
       {
	   delete iTitle;
       iTitle = NULL;
       }
    // for Avkon calling Deactivate, not manually hiding window contorls, reset iGoToNowPlaying
    iGoToNowPlaying = EFalse;
    // Cleanup view deactivation observer
    AppUi()->RemoveViewDeactivationObserver( this );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleForegroundEventL( TBool aForeground )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleForegroundEventL" );
    if ( aForeground && iOpeningNote )
    	{
        CloseWaitNoteL();
        iOpeningNote = EFalse;
    	}

/* This code is added to avoid CBA flicker which happens after pressing "Now playing" 
   from artist&album view and pressing BACK from Playback view.
   While going to playback view, reset CBA to "Back". */
    if( !aForeground && iViewUtility->ActiveViewType().iUid == KMPXPluginTypePlaybackUid )
        {
        TRAP_IGNORE( CEikButtonGroupContainer* cba = Cba();
        if ( cba )
            {
            cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
            cba->DrawDeferred();
            } );
        }
		
#ifdef BACKSTEPPING_INCLUDED
    iActivateBackStepping = EFalse;
    MPX_DEBUG3("CMPXCollectionViewHgImp::HandleForegroundEventL - is in foreground=%d, this view=0x%x",
        aForeground, Id().iUid );
    if( aForeground &&
        iViewUtility &&
        ( iViewUtility->ActiveViewImplementationUid() != KNullUid ) )
        {
        iActivateBackStepping = ETrue;
        }
#endif // BACKSTEPPING_INCLUDED
        {
        CAknView::HandleForegroundEventL( aForeground );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::HandleBacksteppingActivation
// Handle backstepping activation
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleBacksteppingActivation()
    {
#ifdef BACKSTEPPING_INCLUDED
    TInt viewId( iViewUtility->ActiveViewType().iUid );
    TBuf8<KMVPrefixLen + KMaxIntLen> buf;
    buf.Copy( KMVPrefix );
    buf.AppendNum( viewId );
    TInt statusInfo( KMPXBackSteppingNotConsumed );
    TRAP_IGNORE(
       statusInfo =
        iBackSteppingUtility->ForwardActivationEventL( buf, ETrue );
         );
    MPX_DEBUG3("CMPXCollectionViewHgImp::HandleBacksteppingActivation - viewId=0x%x, statusInfo=%d", viewId, statusInfo );
    iActivateBackStepping = EFalse;
#endif // BACKSTEPPING_INCLUDED
   }



// -----------------------------------------------------------------------------
// checks if Now Playing option should be shown
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgImp::NowPlayingOptionVisibilityL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SendOptionVisibilityL" );
    TBool isHidden( iContainer->TotalListItemCount() < 1 );

	// First check if there's a local playback source
	MMPXSource* source( iPlaybackUtility->Source() );
	if ( source == 0 )
		{
		// If no local playback, check if there's a progressive download playback source.
		MMPXPlaybackUtility* pdPlaybackUtility;
		pdPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid( KProgressDownloadUid ) );
		isHidden = pdPlaybackUtility->Source() ? EFalse : ETrue;
		pdPlaybackUtility->Close();
		}
	else
	    {
        isHidden = EFalse;
	    }

    return isHidden;
    }


// ---------------------------------------------------------------------------
// Handle initialing a music menu pane.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleInitMusicMenuPaneL(
    CEikMenuPane* aMenuPane )
    {
    CMPXCollectionViewListBoxArray* array =
        static_cast<CMPXCollectionViewListBoxArray*>(
        iContainer->ListBoxArray() );
	const CMPXMedia& containerMedia = array->ContainerMedia();

	TMPXGeneralCategory containerCategory(
		containerMedia.ValueTObjectL<TMPXGeneralCategory>(
			KMPXMediaGeneralCategory ) );
			
	TMPXGeneralType containerType(
    	containerMedia.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );			
    
	TInt usbUnblockingStatus;
    RProperty::Get( KMPXViewPSUid,
                    KMPXUSBUnblockingPSStatus,
                    usbUnblockingStatus);
   
   if(usbUnblockingStatus)
       {
       aMenuPane->SetItemDimmed( EMPXCmdRefreshLibrary, ETrue );
       }
   
	switch ( containerCategory )
		{
		case EMPXPlaylist:
			{
			// playlist view
			aMenuPane->SetItemDimmed( EMPXCmdGoToPlaylists, ETrue );
			if(usbUnblockingStatus)
			 {
			 aMenuPane->SetItemDimmed( EMPXCmdGoToMusicShop, ETrue );
			 }
			else          
			 {
             aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, iGoToMusicShopOptionHidden );
             }
	        if (containerType != EMPXGroup ) // EMPXItem -> playlist tracks level
			 {
			  aMenuPane->SetItemDimmed( EMPXCmdGoToArtistAlbums, ETrue );
			  aMenuPane->SetItemDimmed( EMPXCmdGoToAllSongs, ETrue );
			  aMenuPane->SetItemDimmed( EMPXCmdGoToPodcasts, ETrue );
			  aMenuPane->SetItemDimmed( EMPXCmdGoToGenre, ETrue );
			  aMenuPane->SetItemDimmed( EMPXCmdRefreshLibrary, ETrue );
              aMenuPane->SetItemDimmed( EMPXCmdMusicLibraryDetails, ETrue);
			  aMenuPane->SetItemDimmed( EMPXCmdGoToMusicShop, ETrue );
	          aMenuPane->SetItemDimmed( EMPXCmdGoToMultipleMusicShop, ETrue); 
	          aMenuPane->SetItemDimmed( EMPXCmdAbout, ETrue); 
	          }  
			break;
			}
		case EMPXAlbum:
		case EMPXArtist:
			{
			// Artists & Albums view
			aMenuPane->SetItemDimmed( EMPXCmdGoToArtistAlbums, ETrue );
			if(usbUnblockingStatus)
			 {
             aMenuPane->SetItemDimmed( EMPXCmdGoToMusicShop, ETrue );
			 }
			else          
			 {
             aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, iGoToMusicShopOptionHidden );
             }
			if ( iContainer->IsTBoneView() ) //TBone View
			    {
			    aMenuPane->SetItemDimmed( EMPXCmdGoToAllSongs, ETrue );
			    aMenuPane->SetItemDimmed( EMPXCmdGoToPlaylists, ETrue );
			    aMenuPane->SetItemDimmed( EMPXCmdGoToPodcasts, ETrue );
			    aMenuPane->SetItemDimmed( EMPXCmdGoToGenre, ETrue );
			    aMenuPane->SetItemDimmed( EMPXCmdRefreshLibrary, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdMusicLibraryDetails, ETrue); 
                aMenuPane->SetItemDimmed( EMPXCmdAbout, ETrue);
			    }    
			break;
			}
		case EMPXGenre:
			{
			// Genre view
        	aMenuPane->SetItemDimmed( EMPXCmdGoToGenre, ETrue );
			if(usbUnblockingStatus)
			 {
			 aMenuPane->SetItemDimmed( EMPXCmdGoToMusicShop, ETrue );
			 }
			else          
			 {
			 aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, iGoToMusicShopOptionHidden );
             }
			if ( containerType != EMPXGroup ) // EMPXItem -> tracks level
				{
                aMenuPane->SetItemDimmed( EMPXCmdGoToAllSongs, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdGoToArtistAlbums, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdGoToPlaylists, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdGoToPodcasts, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdRefreshLibrary, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdMusicLibraryDetails, ETrue);
				aMenuPane->SetItemDimmed( EMPXCmdGoToMusicShop, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdGoToMultipleMusicShop, ETrue); 
                aMenuPane->SetItemDimmed( EMPXCmdAbout, ETrue);
				}
			
			break;
			}
		case EMPXSong:
			{
			// Song view and Genre Track view
			aMenuPane->SetItemDimmed( EMPXCmdGoToAllSongs, ETrue );
			if(usbUnblockingStatus)
			 {
			 aMenuPane->SetItemDimmed( EMPXCmdGoToMusicShop, ETrue );
			 }
			else          
			 {
             aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, iGoToMusicShopOptionHidden );
			 }
			break;
			}
		default:
			{
			break;
			}
		}
   	}
// ---------------------------------------------------------------------------
// Dynamically initialises a menu pane for the Album context
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DynInitMenuPaneAlbumL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DynInitMenuPaneAlbumL" );
    ASSERT( iContainer && aMenuPane != NULL);
    switch ( aResourceId )
        {
        case R_MPX_COLLECTION_VIEW_MENU_1:
            {
            HandleInitMusicMenuPaneL(aMenuPane);
            aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, NowPlayingOptionVisibilityL() );
            aMenuPane->SetItemDimmed( EMPXCmdFind, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUpnpPlayVia, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdCreatePlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdSend, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdPlayItem, ETrue );
            TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
			if ( !landscapeOrientation )
				{
				TInt usbUnblockingStatus;
				RProperty::Get( KMPXViewPSUid,
								KMPXUSBUnblockingPSStatus,
								usbUnblockingStatus);

				if ( iContainer->CurrentLbxItemIndex() > KErrNotFound && !iContainer->IsTBoneView())
				    {
				    aMenuPane->SetItemDimmed( EMPXCmdPlayItem, EFalse );			    	       
                    if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
                        {
                        aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
                        aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
                        }
                    else
                        {
                         aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, EFalse );
                         aMenuPane->SetItemDimmed( EMPXCmdDelete, EFalse );
                        }
					}
				if ( iContainer->IsSelectedItemASong() && iContainer->IsTBoneView() )
					{
					if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
						{
						aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
						aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
						}
					else
						{
						aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, EFalse );
						aMenuPane->SetItemDimmed( EMPXCmdDelete, EFalse );
						aMenuPane->SetItemDimmed( EMPXCmdSend, EFalse );
						}
					}
				}
					
				//If Operator Music store exist, show the cascade menu with Nokia and Operator music store.
				if ( iOperatorMusicStore )
				    {
				    aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, ETrue);
				    }
				else
				    {
				    aMenuPane->SetItemDimmed(EMPXCmdGoToMultipleMusicShop, ETrue);
				    }

			break;
			}

		case R_MPX_COLLECTION_VIEW_MENU_2:
            {
			aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
 			aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdUseAsCascade, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );
			if ( !Layout_Meta_Data::IsLandscapeOrientation() )
 				{
 				TInt usbUnblockingStatus;
				RProperty::Get( KMPXViewPSUid,
							KMPXUSBUnblockingPSStatus,
							usbUnblockingStatus);

				if (usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive)
					{
					aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue); 
					}
				else if( !iContainer->IsTBoneView()&& (iContainer->CurrentLbxItemIndex() > KErrNotFound))								
			        {  
                    CMPXCollectionViewListBoxArray* array =
                    static_cast<CMPXCollectionViewListBoxArray*>(
                        iContainer->ListBoxArray() );
			        const CMPXMedia& media = array->MediaL( iContainer->CurrentLbxItemIndex() );
			        const TDesC& title( media.ValueText( KMPXMediaGeneralTitle ) );
			        if( title.Length() > 0)
			            {   
			            aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, !iUsingNokiaService );
			            }
			        } 
			    }
		    break;
			}
        case R_MPX_ADD_TO_PL_SUB_MENU:
            {
            aMenuPane->SetItemDimmed( EMPXCmdAddToSavedPlaylist, !FindPlaylistsL() );
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
// Dynamically initialises a menu pane for the Playlist context
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DynInitMenuPanePlaylistL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DynInitMenuPanePlaylistL" );
 
    switch ( aResourceId )
        {
        case R_MPX_COLLECTION_VIEW_MENU_1:
            {
            HandleInitMusicMenuPaneL(aMenuPane);

			aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, NowPlayingOptionVisibilityL() );
			aMenuPane->SetItemDimmed( EMPXCmdFind, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdUpnpPlayVia, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdSend, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );

			TInt usbUnblockingStatus;
			RProperty::Get( KMPXViewPSUid,
							KMPXUSBUnblockingPSStatus,
							usbUnblockingStatus);
			if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
				{
				aMenuPane->SetItemDimmed( EMPXCmdCreatePlaylist, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
				}
			else
				{
				aMenuPane->SetItemDimmed( EMPXCmdCreatePlaylist, EFalse );
				aMenuPane->SetItemDimmed( EMPXCmdDelete, EFalse );
				}

			TInt currentItem( iContainer->CurrentLbxItemIndex() );
			if(currentItem > KErrNotFound )
			    {  
			    CMPXCollectionViewListBoxArray* array =
			    static_cast<CMPXCollectionViewListBoxArray*>(
			            iContainer->ListBoxArray() );
			    const CMPXMedia& media = array->MediaL( currentItem );

			    if ( media.IsSupported( KMPXMediaGeneralNonPermissibleActions ) )
			        {
			        // check for auto playlist, disable delete
			        TMPXGeneralNonPermissibleActions attr(
			                media.ValueTObjectL<TMPXGeneralNonPermissibleActions>(
			                        KMPXMediaGeneralNonPermissibleActions ) );
			        if ( attr & EMPXWrite )
			            {
			            aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
			            }
			        }

			    TInt trackCount (0);
			    if( media.IsSupported(KMPXMediaGeneralCount) )
			        {
			        trackCount = media.ValueTObjectL<TInt>( KMPXMediaGeneralCount );
			        }
			    if( trackCount < 1 )
			        {
			        aMenuPane->SetItemDimmed( EMPXCmdPlayItem, ETrue );
			        }
			    } 
			if ( iOperatorMusicStore )
			    {
			    aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, ETrue);
			    }
			else
			    {
			    aMenuPane->SetItemDimmed(EMPXCmdGoToMultipleMusicShop, ETrue);
			    }

			break;
			}

		case R_MPX_COLLECTION_VIEW_MENU_2:
            {
			aMenuPane->SetItemDimmed( EMPXCmdUseAsCascade, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );

			TInt currentItem( iContainer->CurrentLbxItemIndex() );
            
			if(currentItem > KErrNotFound )
			    {  
			    CMPXCollectionViewListBoxArray* array =
			    static_cast<CMPXCollectionViewListBoxArray*>(
			            iContainer->ListBoxArray() );
			    const CMPXMedia& media = array->MediaL( currentItem );

			    TInt usbUnblockingStatus;
			    RProperty::Get( KMPXViewPSUid,
			            KMPXUSBUnblockingPSStatus,
			            usbUnblockingStatus);
			    if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
			        {
			        aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
			        }
			    else
			        {
			        aMenuPane->SetItemDimmed( EMPXCmdRename, EFalse );
			        }

			    if ( media.IsSupported(
			            KMPXMediaGeneralNonPermissibleActions ) )
			        {
			        // check for auto playlist, disable delete
			        TMPXGeneralNonPermissibleActions attr(
			                media.ValueTObjectL<TMPXGeneralNonPermissibleActions>(
			                        KMPXMediaGeneralNonPermissibleActions ) );
			        if ( attr & EMPXWrite )
			            {
			            aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
			            // TODO: this should be an item specific command.
			            aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );
			            }
			        }
			    if ( array->IsItemBrokenLinkL( currentItem ) ||
			            array->IsItemCorruptedL( currentItem ) )
			        {
			        aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
			        // TODO: this should be an item specific command.
			        aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );
			        }
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
// Dynamically initialises a menu pane for the Genre context
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DynInitMenuPaneGenreL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DynInitMenuPanePlaylistL" );

    switch ( aResourceId )
        {
        case R_MPX_COLLECTION_VIEW_MENU_1:
            {
            HandleInitMusicMenuPaneL(aMenuPane);

            aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, NowPlayingOptionVisibilityL() );
            aMenuPane->SetItemDimmed( EMPXCmdFind, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUpnpPlayVia, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdCreatePlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdSend, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdPlayItem, ETrue );

            if ( iContainer->CurrentLbxItemIndex() > KErrNotFound )
                {
                aMenuPane->SetItemDimmed( EMPXCmdPlayItem, EFalse ); 
                TInt usbUnblockingStatus;
                RProperty::Get( KMPXViewPSUid,
                                KMPXUSBUnblockingPSStatus,
                                usbUnblockingStatus);
                if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
                    {
                    aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
                   }
                else
                    {
                    aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, EFalse );
                    }
                }
            
            if ( iOperatorMusicStore )
                {
                aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, ETrue);
                }
            else
                {
                aMenuPane->SetItemDimmed(EMPXCmdGoToMultipleMusicShop, ETrue);
                }
			break;
			}

		case R_MPX_COLLECTION_VIEW_MENU_2:
            {
			aMenuPane->SetItemDimmed( EMPXCmdUseAsCascade, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
			break;
			}
        case R_MPX_ADD_TO_PL_SUB_MENU:
            {
            aMenuPane->SetItemDimmed( EMPXCmdAddToSavedPlaylist, !FindPlaylistsL() );
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
// Dynamically initialises a menu pane for the Songs context
// Handling for any tracks view except playlist tracks.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DynInitMenuPaneSongsL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DynInitMenuPaneAllSongsL" );
    TInt selectionCount( 0 );
    iSelectionIndexCache = iContainer->CurrentSelectionIndicesL(); // not owned
    switch ( aResourceId )
        {
        case R_MPX_COLLECTION_VIEW_MENU_1:
            {
            HandleInitMusicMenuPaneL(aMenuPane);
            aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, NowPlayingOptionVisibilityL() );
            aMenuPane->SetItemDimmed( EMPXCmdFind, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUpnpPlayVia, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdCreatePlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdSend, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdPlayItem, ETrue );
			TInt usbUnblockingStatus;
			RProperty::Get( KMPXViewPSUid,
								KMPXUSBUnblockingPSStatus,
								usbUnblockingStatus);

			if ( iContainer->IsSelectedItemASong() )
				{
				if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
					{
					aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
					aMenuPane->SetItemDimmed( EMPXCmdSend, SendOptionVisibilityL() );
					aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
					}
				else
					{
					aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, EFalse );
					aMenuPane->SetItemDimmed( EMPXCmdSend, SendOptionVisibilityL() );
					aMenuPane->SetItemDimmed( EMPXCmdDelete, EFalse );
					}
				}
			if ( iOperatorMusicStore )
			    {
			    aMenuPane->SetItemDimmed(EMPXCmdGoToMusicShop, ETrue);
			    }
			else
			    {
			    aMenuPane->SetItemDimmed(EMPXCmdGoToMultipleMusicShop, ETrue);
			    }
			break;
			}

		case R_MPX_COLLECTION_VIEW_MENU_2:
            {

		
			if ( iSelectionIndexCache)
				{
				selectionCount = iSelectionIndexCache->Count();
				}
			if ( selectionCount > 0 )
				{
				// multiple selection
				aMenuPane->SetItemDimmed( EMPXCmdUseAsCascade, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
				}
			else
				{
				aMenuPane->SetItemDimmed( EMPXCmdSongDetails,
				        FileDetailsOptionVisibilityL() );
				aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdUseAsCascade, ETrue );
                aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );

				TInt currentItem( iContainer->CurrentLbxItemIndex() );
				if ( currentItem > KErrNotFound )
					{
					CMPXCollectionViewListBoxArray* array =
						static_cast<CMPXCollectionViewListBoxArray*>(
						iContainer->ListBoxArray() );

					if ( array->IsItemBrokenLinkL( currentItem ) ||
						array->IsItemCorruptedL( currentItem ) )
						{
						aMenuPane->SetItemDimmed( EMPXCmdUseAsCascade, ETrue );
						aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
						aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
						aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
						}

					TInt usbUnblockingStatus;
					RProperty::Get( KMPXViewPSUid,
						KMPXUSBUnblockingPSStatus,
						usbUnblockingStatus);

					if (usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive)
						{
						aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue); 
						}
					else
						{	
					    aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, !iUsingNokiaService );
						}
					}
				}
			aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
			aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );
			if (iServiceHandler->HandleSubmenuL(*aMenuPane))
				{
				return;
				}

			break;
			}
        case R_MPX_ADD_TO_PL_SUB_MENU:
            {
            aMenuPane->SetItemDimmed( EMPXCmdAddToSavedPlaylist, !FindPlaylistsL() );
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
// Dynamically initialises a menu pane for the playlist Songs context
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DynInitMenuPanePlaylistSongsL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DynInitMenuPanePlaylistSongsL" );

    TBool isListEmpty( iContainer->TotalListItemCount() == 0 );

    switch ( aResourceId )
        {
        case R_MPX_COLLECTION_VIEW_MENU_1:
            {
            TInt usbUnblockingStatus;
            RProperty::Get( KMPXViewPSUid,
                    KMPXUSBUnblockingPSStatus,
                    usbUnblockingStatus);
								
            HandleInitMusicMenuPaneL(aMenuPane);
            aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, NowPlayingOptionVisibilityL() );
            aMenuPane->SetItemDimmed( EMPXCmdFind, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUpnpPlayVia, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdCreatePlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdSend, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdPlayItem, ETrue );
            
            if( iContainer->IsInReorderMode() )
                break;
            
			if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
				{
				aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdSend, SendOptionVisibilityL() );
				aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
				}
			else
				{
				TInt selectionCount( 0 );
				iSelectionIndexCache = iContainer->CurrentSelectionIndicesL(); // not owned
				if ( iSelectionIndexCache)
					{
					selectionCount = iSelectionIndexCache->Count();
					}
				// do not display add songs option when marking is on or USB is connected
				if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
					{
					aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
					}
				else
					{
					aMenuPane->SetItemDimmed( EMPXCmdAddSongs, selectionCount > 0 );
					}

				if ( iContainer->CurrentLbxItemIndex() > KErrNotFound )
					{
					aMenuPane->SetItemDimmed( EMPXCmdSend, SendOptionVisibilityL() );
					aMenuPane->SetItemDimmed( EMPXCmdRemove, EFalse );
					if ( iContainer->CurrentListItemCount() > 1 )
					    {
					    aMenuPane->SetItemDimmed( EMPXCmdReorder, EFalse );
					    }
					}
				}
				
			CMPXCollectionViewListBoxArray* array =
            static_cast<CMPXCollectionViewListBoxArray*>(
                    iContainer->ListBoxArray() );
            const CMPXMedia& containerMedia = array->ContainerMedia();

            if ( containerMedia.IsSupported( KMPXMediaGeneralNonPermissibleActions ) )
                {
                // check for auto playlist, disable add, remove and reorder
                TMPXGeneralNonPermissibleActions attr(
                        containerMedia.ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                                KMPXMediaGeneralNonPermissibleActions ) );
                if ( attr & EMPXWrite )
                    {
                    aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
                    aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
                    aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );
                    }
                }
			break;
			}

		case R_MPX_COLLECTION_VIEW_MENU_2:
            {
            aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUseAsCascade, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
            
            if( iContainer->IsInReorderMode() )
                break;
            
			TInt selectionCount( 0 );
			iSelectionIndexCache = iContainer->CurrentSelectionIndicesL(); // not owned
			if ( iSelectionIndexCache)
				{
				selectionCount = iSelectionIndexCache->Count();
				}
			if ( selectionCount > 0 )
				{
				// multiple selection
				aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
				aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
				}
			else
				{
				aMenuPane->SetItemDimmed( EMPXCmdSongDetails,
					FileDetailsOptionVisibilityL() );

				TInt currentItem( iContainer->CurrentLbxItemIndex() );

				if ( currentItem > KErrNotFound )
					{
					CMPXCollectionViewListBoxArray* array =
						static_cast<CMPXCollectionViewListBoxArray*>(
						iContainer->ListBoxArray() );
					if ( array->IsItemBrokenLinkL( currentItem ) ||
						array->IsItemCorruptedL( currentItem ) )
						{
						aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
						}
						
					TInt usbUnblockingStatus;
					RProperty::Get( KMPXViewPSUid,
							KMPXUSBUnblockingPSStatus,
							usbUnblockingStatus);

					if (usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive)
						{
						aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue);
						}
					else
						{		
					    aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, !iUsingNokiaService );
						}
					}
				}
			if (iServiceHandler->HandleSubmenuL(*aMenuPane))
				{
				return;
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
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DynInitMenuPaneL" );

    CMPXCollectionViewListBoxArray* array =
        static_cast<CMPXCollectionViewListBoxArray*>(
        iContainer->ListBoxArray() );
	const CMPXMedia& containerMedia = array->ContainerMedia();

    if ( &containerMedia == NULL )
        {
        // If containerMedia is NULL, Dim all options 
        DimAllOptions(aResourceId, aMenuPane);
        return; 
        } 
		
	TMPXGeneralType containerType(
		containerMedia.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
	TMPXGeneralCategory containerCategory(
		containerMedia.ValueTObjectL<TMPXGeneralCategory>(
			KMPXMediaGeneralCategory ) );

    TInt usbUnblockingStatus;
    RProperty::Get( KMPXViewPSUid,
                    KMPXUSBUnblockingPSStatus,
                    usbUnblockingStatus);

	// We first construct options menu based on collection browsing context.
	// For resource R_MPX_COLLECTION_VIEW_MENU_1, R_MPX_COLLECTION_VIEW_MENU_2.
	if ( containerType == EMPXGroup )
		{
		switch (containerCategory)
			{
			case EMPXCollection:
				break;
			case EMPXArtist:
				break;
			case EMPXAlbum:
			    // Artist & Album view
				DynInitMenuPaneAlbumL(aResourceId, aMenuPane);
				break;
			case EMPXPlaylist:
			    // Playlist view
				DynInitMenuPanePlaylistL(aResourceId, aMenuPane);
				break;
			case EMPXSong:
			    // Songs (All Songs) View
				DynInitMenuPaneSongsL(aResourceId, aMenuPane);
				break;
			case EMPXGenre:
			    // Genre View
				DynInitMenuPaneGenreL(aResourceId, aMenuPane);
				break;
			case EMPXComposer:
				break;
			default:
				User::Leave(KErrNotSupported);
				break;
			}
		}
	else if ( containerType == EMPXItem )
		{
		switch (containerCategory)
			{
			case EMPXSong:
				break;
			case EMPXArtist:
				break;
			case EMPXPlaylist:
			    // Playlist tracks view
				DynInitMenuPanePlaylistSongsL(aResourceId, aMenuPane);
				break;
			case EMPXAlbum:
			case EMPXGenre:
			case EMPXComposer:
			    // Other tracks view
				DynInitMenuPaneSongsL(aResourceId, aMenuPane);
				break;
			default:
				User::Leave(KErrNotSupported);
				break;
			}
		}

	// Handle other menu resources.
    switch ( aResourceId )
        {
        case R_MPX_ADD_TO_PL_SUB_MENU:
            {
            aMenuPane->SetItemDimmed( EMPXCmdAddToSavedPlaylist, !FindPlaylistsL() );
            break;
            }
        case R_MPX_COLLECTION_PLAY_VIA_SUB_MENU:
            {
            if ( iUpnpFrameworkSupport )
                {
                AddPlayersNamesToMenuL( *aMenuPane );
                }
            break;
            }
        case R_MPX_COLLECTION_VIEW_EMBEDDED_PLAYLIST_VIEW:
            {
            // if it's from browser/messaging, display save option if
            // it has not been saved
            aMenuPane->SetItemDimmed( EMPXCmdSavePlaylist,
                ( !iCommonUiHelper->IsHostMessagingBrowserL() || !MPXTlsHelper::NeedSave() ) );
            aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
            break;
            }
        case R_MPX_PLAYBACK_COLLECTION_MUSICSHOP_SUB_MENU:
        	{
        	aMenuPane->SetItemTextL(EMPXCmdGoToOperatorMusicShop,  iOperatorMusicStoreName->Des() );
        	break;
        	}
        case R_AVKON_MENUPANE_MARKABLE_LIST:
            {
            // Mark/Unmark submenu is displayed only if USB is on going
            if (usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive)
                {
                MPX_DEBUG1("CMPXCollectionViewHgImp::DynInitMenuPaneL() dimmed Mark/Unmark submenu");
                aMenuPane->SetItemDimmed(EAknCmdEditListMenu, ETrue);
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if (!(aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST && usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive))
        {
        // Custom handling of menu pane for markable lists
        iContainer->HandleMarkableListDynInitMenuPane(
            aResourceId,
            aMenuPane );
        }
    }

// ---------------------------------------------------------------------------
// From MEikListBoxObserver
// Handles listbox events.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleListBoxEventL" );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleListBoxEventL iLastDepth = %d", iLastDepth );
    MPX_PERF_CHECKPT( "Select collection item to open" );
    if ( iContainer )
        {
        //to keep/change focus on right song in rename/remove
        if ( aEventType == EEventItemSingleClicked )         
            {
            if ( !iHandlingKeyEvent && iCollectionCacheReady )
                {
                CMPXCommonListBoxArrayBase* listboxArray( iContainer->ListBoxArray() );
                TInt currentIndex( iContainer->CurrentLbxItemIndex() );

                CMPXCommand* command = CMPXCommand::NewL();
                CleanupStack::PushL( command );
                command->SetTObjectValueL<TMPXCommandId>( KMPXCommandGeneralId,
                                                          KMPXCommandIdCollectionSelect );
                command->SetTObjectValueL( KMPXCommandCollectionSelectIndex, currentIndex );
                command->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
                iCollectionUtility->Collection().CommandL( *command );
                CleanupStack::PopAndDestroy( command );
                }
            }

        if ( aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked
            || aEventType == EEventItemSingleClicked
           )
            {
            if ( iContainer->IsInReorderMode() )
                {
                ProcessCommandL( iContainer->IsInReorderMode() ?
                    EMPXCmdReorderDrop : EMPXCmdReorderGrab );
                }
		    else if ( !iHandlingKeyEvent )
                {
                TMPXPlaybackState pbState( iPlaybackUtility->StateL() );
                TBool isEqual( EFalse );
                if ( pbState == EPbStatePlaying || pbState == EPbStatePaused )
                    {
                    // in currently playing view, check if current item is playing
                    if ( iContainer->CurrentLbxItemIndex() ==
                        iContainer->PlaybackIndex() )
                        {
                        isEqual = ETrue;
                        }
                    }

                if ( isEqual )
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleListBoxEventL going to now playing view" );
                    if ( pbState == EPbStatePaused )
                        {
                        MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleListBoxEventL resuming playback" );
                        iPlaybackUtility->CommandL( EPbCmdPlay );
                        }
                    HandleCommandL( EMPXCmdGoToNowPlaying );
                    }
                else
                    {
                    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                    CleanupStack::PushL( cpath );
                    if ( cpath->Levels() == ( iLastDepth + 1 ) )
                        {
                        // navigated in one level
                        iLastDepth++;
                        iBottomIndex->AppendL(
                            iContainer->BottomLbxItemIndex() );
                        }
                    else
                        {
                        // invalid path, rebuild with all 0
                        iBottomIndex->Reset();
                        iLastDepth = cpath->Levels();
                        for ( TInt i = 0; i < iLastDepth; i++ )
                            {
                            iBottomIndex->AppendL( 0 );
                            }
                        }
                    CleanupStack::PopAndDestroy( cpath );
                    TInt currentItem( iContainer->CurrentLbxItemIndex() );
                    MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleListBoxEventL Opening Item: %d", currentItem );

                    iHandlingKeyEvent = ETrue;

                    // Needed to reset the status of iPreservedState
                    if ( EPbStateStopped == pbState )
                        {
                        iPlaybackUtility->CommandL(EPbCmdResetPreserveState);
                        }

    #ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
                    // check to see if user has selected podcast item
                    if ( (iLastDepth == KMusicCollectionMenuLevel) &&
                         (currentItem == KMusicMenuPodcastMenuItemIndex) &&
                         !iDisablePodcasting )
                        {
                        // open the podcast collection db plugin
                        iPodcast = ETrue;
                        CMPXCollectionPath* mainPodcastMenu = CMPXCollectionPath::NewL();
                        CleanupStack::PushL( mainPodcastMenu );
                        mainPodcastMenu->AppendL(KPodcastCollectionUid);
                        iCollectionUtility->Collection().OpenL( *mainPodcastMenu );
						iFirstIncrementalBatch = ETrue;
                        CleanupStack::PopAndDestroy( mainPodcastMenu );

                        // activate the podcast collection view
                        RArray<TUid> uids;
                        CleanupClosePushL( uids );
                        uids.AppendL( TUid::Uid( KMPXPluginTypeCollectionUid ) );
                        uids.AppendL( TUid::Uid( KPodcastCollectionUid ));
                        iViewUtility->ActivateViewL(uids);

                        CleanupStack::PopAndDestroy( &uids );
                        }
                    else
    #endif
                        {
                        if(iCollectionCacheReady)
                            {
                            CMPXCommonListBoxArrayBase* listboxArray( iContainer->ListBoxArray() );
                            TInt currentIndex( iContainer->CurrentLbxItemIndex() );
                            const CMPXMedia& media( listboxArray->MediaL( currentIndex ) );
                            TMPXGeneralType type(
                                media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType ) );
                            TMPXGeneralCategory category(
                                media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory ) );
                            if ( type != EMPXItem || category != EMPXSong )
                                {
                                if ( type == EMPXGroup && category== EMPXSong )
                                    {
                                    // dismiss for HGList
                                    //StartWaitNoteL( EMPXImmediatelyOpeningNote );
                                    }
                                else
                                    {
                                    //do not show wait note if opening a song
                                    // dismiss for HGList
                                    //StartWaitNoteL( EMPXOpeningNote );
                                    }
                                }
                            iCollectionUtility->Collection().OpenL( currentItem );
							iFirstIncrementalBatch = ETrue;
                            }
                        }

                    }
                }
            else
                {
                MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleListBoxEventL Busy, ignoring command" );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// From MEikCommandObserver
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::ProcessCommandL(TInt aCommandId)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::ProcessCommandL" );

    switch( aCommandId )
        {
        case EMPXCmdReorderDrop:
            {
            SaveCurrentPlaylistL();
            iContainer->ConfirmReorderL();
            DeactivateReorderGrabbedModeL( ETrue );
            break;
            }
        case EMPXCmdReorderGrab:
            {
            SetNewCbaL( R_MPX_CUI_REORDER_DROP_CANCEL_CBA );
            ActivateReorderGrabbedModeL();
            break;
            }
        case EMPXCmdReorderCancel:
            {
            iCurrentHighlightedIndex = iContainer->GetOriginalIndex();
            MPX_DEBUG2( "CMPXCollectionViewHgImp::ProcessCommandL Setting iCurrentHighlightedIndex = %d", iCurrentHighlightedIndex );
            iContainer->CancelReorder();
            } // Fall through on purpose
        case EMPXCmdReorderDone:
            {
            DeactivateReorderGrabbedModeL( ETrue );
            iHandlingKeyEvent = EFalse;            
            break;
            }
        case EMPXCmdUpArrow:
            {
            iIgnoreNextFocusChangedMessage = ETrue;
            if ( iContainer->IsInReorderMode() )
                {
                iContainer->MoveReorderIndex( KMPXDirectionUp );
                }
            else
                {
                iDownCount = 0;

                if( iUpCount != KErrNotFound )
                    {
                    if( iUpCount++ > KIncrementalDirectionCount )
                        {
                        iIncrementalOpenUtil->SetDirection( CMPXCollectionOpenUtility::EFetchUp );
                        iUpCount = KErrNotFound;
                        }
                    }
                }
            break;
            }
        case EMPXCmdDownArrow:
            {
            iIgnoreNextFocusChangedMessage = ETrue;
            if ( iContainer->IsInReorderMode() )
                {
                iContainer->MoveReorderIndex( KMPXDirectionDown );
                }
            else
                {
                iUpCount = 0;

                if( iDownCount != KErrNotFound )
                    {
                    if( iDownCount++ > KIncrementalDirectionCount  )
                        {
                        iIncrementalOpenUtil->SetDirection( CMPXCollectionOpenUtility::EFetchDown );
                        iDownCount = KErrNotFound;
                        }

                    }
                }
            break;
            }
        case EMPXCmdCommonEnterKey:
            {
            // Handle list box event for enter key pressed
            TInt index( iContainer->CurrentLbxItemIndex() );
            if( index >= 0 )
            	{
                HandleListBoxEventL(
            	    NULL, // ignore
            	    EEventEnterKeyPressed );
            	}
            break;
            }

		case EMPXCmdGoToNowPlaying:
			{
			HandleCommandL( aCommandId );
			break;
			}
        case EMPXCmdUpArrowAfterListboxHandled:
            {
            if ( iContainer->IsInReorderMode() )
                {
                TInt index( iContainer->CurrentLbxItemIndex() );
                if ( index == iContainer->CurrentListItemCount() - 1 )
                    {
                    // if after up arrow is pressed, and end up at the
                    // lowest index.  list box looped, sync with listboxarray needed
                    iContainer->SetReorderIndex( index );
                    iContainer->HandleLbxItemAdditionL();
                    // need to set index and redraw
                    iContainer->SetLbxCurrentItemIndexAndDraw( index );
                    }
                UpdateReorderNaviPaneL();
                }
            break;
            }
        case EMPXCmdDownArrowAfterListboxHandled:
            {
            if ( iContainer->IsInReorderMode() )
                {
                TInt index( iContainer->CurrentLbxItemIndex() );
                if ( index == 0 )
                    {
                    // if after down arrow is pressed, and end up at the
                    // top index.  list box looped, sync with listboxarray needed
                    iContainer->SetReorderIndex( index );
                    iContainer->HandleLbxItemAdditionL();
                    }
                UpdateReorderNaviPaneL();
                }
            break;
            }
        case EMPXCmdCommonDelete:
            {
            if ( !iIsEmbedded )
                {
                StoreListboxItemIndexL();
                // embedded mode doesn't support delete key
                DeleteSelectedItemsL(aCommandId);
                }
            break;
            }
        case EMPXCmdCommonResourceChange:
            {
            iContainer->SetRect( ClientRect() );
            break;
            }
        case EAknSoftkeyEmpty:
            {
            // do nothing.  handled to avoid "unsupported" message
            break;
            }
#ifdef __ENABLE_MSK
        case EMPXCmdCommonUpdateMiddleSoftKey:
            {
            // Determine middle soft key labeling
            UpdateMiddleSoftKeyDisplayL( iCurrentMskId );
            break;
            }
        case EAknSoftkeyContextOptions :
            {
            // set context menu flag
            iShowContextMenu = ETrue;
            // open context menu (by processing "Options" command)
            if ( iContainer )
                {
                iSelectionIndexCache = iContainer->CurrentSelectionIndicesL(); // not owned
                MPX_DEBUG2( "CMPXCollectionViewHgImp::ProcessCommandL count = %d", iSelectionIndexCache->Count() );
                }
            CAknView::ProcessCommandL( EAknSoftkeyOptions );
            break;
            }
#endif // __ENABLE_MSK
        case EAknCmdExit:
			{
            HandleCommandL( aCommandId );
			CAknView::ProcessCommandL( aCommandId );
			break;
			}
		case EAknSoftkeyOptions:
            {
            if ( iContainer )
                {
                iSelectionIndexCache = iContainer->CurrentSelectionIndicesL(); // not owned
                MPX_DEBUG2( "CMPXCollectionViewHgImp::ProcessCommandL count = %d", iSelectionIndexCache->Count() );
                }
            }
        default:
            {
            CAknView::ProcessCommandL( aCommandId );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::LaunchMusicShopL
// Launch music shop application
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::LaunchMusicShopL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::LaunchMusicShopL" );

    if ( iMusicStoreUID != 0)
        {
    TApaTaskList taskList( iCoeEnv->WsSession() );
        TApaTask task = taskList.FindApp( TUid::Uid(iMusicStoreUID) );

    if ( task.Exists() )
        {
        GfxTransEffect::BeginFullScreen( 
        AknTransEffect::EApplicationStart,
            TRect(), 
            AknTransEffect::EParameterType, 
            AknTransEffect::GfxTransParam( TUid::Uid(iMusicStoreUID),        
            AknTransEffect::TParameter::EActivateExplicitContinue ));            
        task.BringToForeground();
        }
    else
        {
        RApaLsSession session;
        if ( KErrNone == session.Connect() )
            {
            CleanupClosePushL( session );
            TThreadId threadId;
                session.CreateDocument( KNullDesC, TUid::Uid(iMusicStoreUID), threadId );
            CleanupStack::PopAndDestroy(&session);
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPlayerCategoryView::DoFindInMusicShopL
// Handle find in music shop event
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoFindInMusicShopL( const TDesC& aSongName,
                                                const TDesC& aArtistName,
                                                const TDesC& aAlbumName )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoFindInMusicShopL" );
    CMPXFindInMShop* finder = CMPXFindInMShop::NewL();
    CleanupStack::PushL( finder );
    HBufC* url = NULL;
    url = finder->CreateSearchURLL( aSongName,
                                     aArtistName,
                                     aAlbumName,
                                     KNullDesC,     // Composer - Not used
                                     KNullDesC );   // Genre - Not used
    CleanupStack::PushL(url);   // the pointer will be poped
    RProperty::Set( TUid::Uid(iMusicStoreUID),
                    KMShopCategoryId,
                    KFindInMShopKeyValid );  // Set Key to Valid

    RProperty::Set( TUid::Uid(iMusicStoreUID),
                    KMShopCategoryName,
                    *url );

    LaunchMusicShopL();


    if(url)
        {
        CleanupStack::PopAndDestroy(url);
        }

    CleanupStack::PopAndDestroy( finder ); // finder
    REComSession::FinalClose();
    }


// ----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::SetParamL
// Set the parameter passed in from viewframework to the current select index
// ----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::SetParamL( const TDesC* aParam )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SetParamL" );
    delete iCurrentSelectedIndex;
    iCurrentSelectedIndex = NULL;
    iCurrentSelectedIndex = aParam->AllocL();    //  aParam = L"0" for 1st item
    }


// -----------------------------------------------------------------------------
// CMPlayerCategoryView::DoGetSongMediaPropertyL
// Handle find in music shop event
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DoGetSongMediaPropertyL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::DoGetSongMediaPropertyL" );

    if ( iCurrentSelectedIndex )
        {
        TLex currentSelectedIndexLex( iCurrentSelectedIndex->Des() );
        TInt lexToInt = NULL;
        if ( currentSelectedIndexLex.Val( lexToInt ) == KErrNone )
            {
            // Get media property for the current song
            CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cpath );

            cpath->Set( lexToInt );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            // to get EMPXMediaGeneralTitle
            attrs.Append( KMPXMediaGeneralTitle );
            // to get EMPXMediaMusicArtist and EMPXMediaMusicAlbum
            attrs.Append( KMPXMediaMusicArtist );
            attrs.Append( KMPXMediaMusicAlbum );

            // Call MediaL and handle callback at HandleCollectionMediaL
            iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( cpath );
            }
        }

    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::HandleFindAllL
// Handle callback for "find" operation
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleFindAllL(
    const CMPXMedia& aResults,
    TBool aComplete,
    TInt aError )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleFindAllL" );
    if ( aError == KErrNone )
        {
        switch ( iCurrentFindAllLOp )
            {
            case EMPXOpFindAllLAlbumArtistAddToNewPlaylist:
                {
                iNumSongAddedToPlaylist = aResults.ValueTObjectL<TInt>(
                    KMPXMediaArrayCount );
                TBool ret = iCommonUiHelper->CreatePlaylistL(
                    aResults, this, this );
                if ( !ret )
                    {
                    iAddingToNewPlaylist = EFalse;
                    }
                break;
                }
            case EMPXOpFindAllLAlbumArtistAddToSavedPlaylist:
                {
                if ( !iUserPlaylists )
                    {
                    MPX_DEBUG1( "CMPXCollectionViewHgImp::HandleFindAllL Null iUserPlaylists" );
                    User::Leave( KErrArgument );
                    }
                iNumSongAddedToPlaylist = aResults.ValueTObjectL<TInt>(
                    KMPXMediaArrayCount );
                MPX_TRAPD( err, iCommonUiHelper->AddToSavedPlaylistL(
                    *iUserPlaylists, aResults, this, this ) );
                if ( err != KErrNone )
                    {
                    HandleError( err );
                    }
                break;
                }
            case EMPXOpFindAllLRenameAlbum:
            case EMPXOpFindAllLRenameArtist:
            case EMPXOpFindAllLRenameGenre:
            case EMPXOpFindAllLRenameComposer:
                {
                const CMPXMediaArray* mediaArray =
                    aResults.Value<CMPXMediaArray>( KMPXMediaArrayContents );
                User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));

                iSetMediaLCount = mediaArray->Count();
                iInvalidFileExist = EFalse;
                TInt invalidItemCount( 0 );
                MPX_DEBUG2( "CMPXCollectionViewHgImp::HandleFindAllL Rename song count = %d", iSetMediaLCount );
                iIsWaitNoteCanceled = EFalse;
                HBufC* waitNoteString( NULL );
                if ( iSetMediaLCount > 1 )
                    {
                    // multiple item
                    waitNoteString = StringLoader::LoadLC(
                        R_MPX_COLLECTION_NOTE_UPDATING_MULTIPLE_ITEMS,
                        iSetMediaLCount );
                    }
                else
                    {
                    // single item
                    waitNoteString = StringLoader::LoadLC(
                        R_MPX_COLLECTION_NOTE_UPDATING_ITEM );
                    }
                HandleCommandL( EMPXCmdIgnoreExternalCommand );
                iCommonUiHelper->ShowWaitNoteL( *waitNoteString,
                    R_MPX_COLLECTION_WAITNOTE_SOFTKEYS_EMPTY_STOP, EFalse, this );
                CleanupStack::PopAndDestroy( waitNoteString );

                TPtrC art( KNullDesC );
                if ( (iCurrentFindAllLOp == EMPXOpFindAllLRenameAlbum || iCurrentFindAllLOp == EMPXOpFindAllLRenameArtist )&&
                        iContainer )
                    {
                    CMPXCommonListBoxArrayBase* listboxArray(
                        iContainer->ListBoxArray() );
                    TInt currentIndex( iContainer->CurrentLbxItemIndex() );
                    const CMPXMedia& currentMedia( listboxArray->MediaL( currentIndex ) );
                    if ( currentMedia.IsSupported( KMPXMediaMusicAlbumArtFileName ) )
                        {
                        art.Set( currentMedia.ValueText( KMPXMediaMusicAlbumArtFileName ) );
                        }
                    }

                CMPXMediaArray* entryArray = CMPXMediaArray::NewL();
                CleanupStack::PushL( entryArray );
                TUid collectionId( KNullUid );
                for ( TInt i = 0; i < iSetMediaLCount && !iIsWaitNoteCanceled; i++ )
                    {
                    CMPXMedia* media( mediaArray->AtL( i ) );
                    TUint flags( 0 );
                    if( media->IsSupported( KMPXMediaGeneralFlags ) )
                        {
                        flags = media->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
                        }
                    const TDesC& songUri =
                        media->ValueText( KMPXMediaGeneralUri );
                    TBool fileExist( ConeUtils::FileExists( songUri ) );
                    if (!(flags & KMPXMediaGeneralFlagsIsInvalid)
                     && !(flags & KMPXMediaGeneralFlagsIsCorrupted)
                     && fileExist )
                        {
                        CMPXMedia* entry = CMPXMedia::NewL();
                        CleanupStack::PushL( entry );
                        entry->SetTextValueL( KMPXMediaGeneralUri, songUri );
                        TMPXItemId id( media->ValueTObjectL<TMPXItemId>(
                                        KMPXMediaGeneralId ) );
                        collectionId = media->ValueTObjectL<TUid>(
                                             KMPXMediaGeneralCollectionId );
                        entry->SetTObjectValueL<TMPXItemId>(
                            KMPXMediaGeneralId, id );
                        entry->SetTObjectValueL<TUid>(
                            KMPXMediaGeneralCollectionId, collectionId );
                        entry->SetTObjectValueL<TMPXGeneralType>(
                            KMPXMediaGeneralType, EMPXItem );
                        entry->SetTObjectValueL<TMPXGeneralCategory>(
                            KMPXMediaGeneralCategory, EMPXSong );

                        TPtrC ptr( songUri );
                        MPX_DEBUG3( "CMPXCollectionViewHgImp::HandleFindAllL setting item id 0x%x, uri %S", id.iId1, &ptr );
                        switch ( iCurrentFindAllLOp )
                            {
                            case EMPXOpFindAllLRenameAlbum:
                                {
                                if ( art.Compare( KNullDesC ) != 0 &&
                                        art.Compare(
                                            media->ValueText( KMPXMediaMusicAlbumArtFileName ) ) == 0 )
                                    {
                                    entry->SetTextValueL(
                                        KMPXMediaMusicAlbumArtFileName,
                                        art );
                                    }
                                entry->SetTextValueL(
                                    KMPXMediaMusicAlbum, *iNewName );
                                break;
                                }
                            case EMPXOpFindAllLRenameArtist:
                                {
                                if ( art.Compare( KNullDesC ) != 0 &&
                                        art.Compare(
                                            media->ValueText( KMPXMediaMusicAlbumArtFileName ) ) == 0 )
                                    {
                                    entry->SetTextValueL(
                                        KMPXMediaMusicAlbumArtFileName,
                                        art );
                                    }
                                entry->SetTextValueL(
                                    KMPXMediaMusicArtist, *iNewName );
                                break;
                                }
                            case EMPXOpFindAllLRenameGenre:
                                {
                                entry->SetTextValueL(
                                    KMPXMediaMusicGenre, *iNewName );
                                break;
                                }
                            case EMPXOpFindAllLRenameComposer:
                                {
                                entry->SetTextValueL(
                                    KMPXMediaMusicComposer, *iNewName );
                                break;
                                }
                            default:
                                {
                                // should not reach here
                                ASSERT( 0 );
                                break;
                                }
                            }
                        if ( (iCurrentFindAllLOp == EMPXOpFindAllLRenameAlbum || iCurrentFindAllLOp == EMPXOpFindAllLRenameArtist) &&
                                art.Compare( KNullDesC ) != 0 &&
                                art.Compare(
                                    media->ValueText( KMPXMediaMusicAlbumArtFileName ) ) == 0 )
                            {
                            entryArray->InsertL( entry, 0 );  // ownership xfer
                            }
                        else
                            {
                            entryArray->AppendL( entry );  // ownership xfer
                            }
                        CleanupStack::Pop( entry );
                        }
                    else
                        {
                        iInvalidFileExist = ETrue;
                        invalidItemCount++;
                        if ( !fileExist )
                            {
                            UpdateDatabaseFlagL( KErrNotFound, *media,
                                KMPXMediaGeneralFlagsIsInvalid, ETrue, EFalse );
                            }
                        }
                    }
                // Package array of items to set
                //
                CMPXMedia* setData = CMPXMedia::NewL();
                CleanupStack::PushL( setData );
                setData->SetTObjectValueL( KMPXMediaGeneralType, EMPXGroup );
                setData->SetCObjectValueL( KMPXMediaArrayContents, entryArray );

                // Generate the command
                //
                CMPXCommand* command = CMPXCommand::NewL();
                CleanupStack::PushL( command );
                command->SetTObjectValueL<TMPXCommandId>( KMPXCommandGeneralId,
                                                          KMPXCommandIdCollectionSet );
                command->SetTObjectValueL<TInt>( KMPXCommandGeneralCollectionId,
                                                 collectionId.iUid);
                command->SetCObjectValueL( KMPXCommandColSetMedia, setData );

                iCollectionUtility->Collection().CommandL( *command );

                // Cleanup
                CleanupStack::PopAndDestroy( command );
                CleanupStack::PopAndDestroy( setData );
                CleanupStack::PopAndDestroy( entryArray );

                iSetMediaLCount = 1;  // 1 async set
                MPX_DEBUG3( "CMPXCollectionViewHgImp::HandleFindAllL valid items = %d, invalid items = %d", iSetMediaLCount, invalidItemCount );
                if ( iSetMediaLCount == 0 )
                    {
                    // if all items are invalid, display note
                    // now instead of waiting for callback
                    HandleRenameOpCompleteL();
                    }
                break;
                }
            case EMPXOpFindAllLPlaylistTracksCount:
                {
                TInt trackCount = aResults.ValueTObjectL<TInt>( KMPXMediaArrayCount );
                MPX_DEBUG2( "CMPXCollectionViewHgImp:: playlist tracks count = %d", trackCount );
                if ( trackCount == 0 )
                    {
                    // empty playlist, display info note
                    HBufC* string = StringLoader::LoadLC(
                        R_MPX_COLLECTION_NOTE_EMPTY_PLAYLIST );
                    iCommonUiHelper->DisplayInfoNoteL( *string );
                    CleanupStack::PopAndDestroy( string );
                    }
                else
                    {
                    HandleCommandL( EMPXCmdForcePlay );
                    }
                break;
                }
            case EMPXOpFindAllLUpnp:
                {
                if ( iUpnpFrameworkSupport )
                    {
                    DoHandleCopyToRemoteL( aResults, aComplete );
                    }
                break;
                }
            case EMPXOpFindAllLUpnpPlayback:
                {
                if ( iUpnpFrameworkSupport )
                    {
                    TInt trackCount = aResults.ValueTObjectL<TInt>( KMPXMediaArrayCount );
                    MPX_DEBUG2( "CMPXCollectionViewHgImp:: playlist tracks count = %d", trackCount );
                    if ( trackCount > 0 )
                        {
                        TInt currentListBoxItemIndex(
                            iContainer->CurrentLbxItemIndex() );
                        if ( currentListBoxItemIndex >= 0 )
                            {
                            GetCurrentPlayerDetails();

                            // Needed to reset the status of iPreservedState
                            if ( EPbStateStopped == iPlaybackUtility->StateL() )
                                {
                                iPlaybackUtility->CommandL(EPbCmdResetPreserveState);
                                }
                            if(iCollectionCacheReady)
                                {
                                iCollectionUtility->Collection().OpenL(
                                                currentListBoxItemIndex,
                                                EMPXOpenPlaylistOnly);
								iFirstIncrementalBatch = ETrue;
                                }
                            }
                        }
                    else
                        {
                        // empty playlist, display info note
                        HBufC* string = StringLoader::LoadLC(
                            R_MPX_COLLECTION_NOTE_EMPTY_PLAYLIST );
                        iCommonUiHelper->DisplayInfoNoteL( *string );
                        CleanupStack::PopAndDestroy( string );
                        }
                    }
                break;
                }
            default:
                {
                // should not reach here
                break;
                }
            }
        }
    else
        {
        HandleError( aError );
        }
    iCurrentFindAllLOp = EMPXOpFindAllLIdle;

    if ( !iUpnpFrameworkSupport )
        {
        // Remove compilation warnings
        (void)aComplete;
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::HandleViewActivation
// Handle view activation
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleViewActivation(
    const TUid& aCurrentViewType,
    const TUid& /*aPreviousViewType*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::HandleViewActivation" );

    if ( aCurrentViewType.iUid == KMPXPluginTypeCollectionUid )
        {
        if( iAddingSong )
            {
            iAddingSong = EFalse;
            TRAP_IGNORE( GetDurationL() );
            }
        if( iInAlbumArtDialog )
            {
            iInAlbumArtDialog = EFalse;
            TRAP_IGNORE( UpdateTitlePaneL() );
            }
        if( iInSongDetails )
            {
            iInSongDetails = EFalse;
            }
        }
     else if ( ( aCurrentViewType.iUid == KMPXPluginTypePlaybackUid ) && ( iGoToNowPlaying == EFalse ) )        
        {
		// It is view switching when launched from other applications
		// hide this view to avoid flickering 
		// since this view is the current active view, it receives this event

		if ( iContainer )
        	{
            iContainer->HideContainerWindow();
   	  
            // set title to blank to avoid title flickering
            CAknTitlePane* title( static_cast<CAknTitlePane*>
                ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ))));
            if ( title )
                {
                title->SetTextL(KEmptyTitle); 
                title->DrawNow();
	            }
		    }
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::HandleViewUpdate
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleViewUpdate(
    TUid /* aViewUid */,
    MMPXViewActivationObserver::TViewUpdateEvent /* aUpdateEvent */,
    TBool /* aLoaded */,
    TInt /* aData */)
    {
    // Do nothing, this should be handled by the AppUI
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::HandleViewActivation
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleViewActivation(const TVwsViewId& aNewlyActivatedViewId,
                                                 const TVwsViewId& /*aViewIdToBeDeactivated */)
    {
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::HandleViewDeactivation
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::HandleViewDeactivation(const TVwsViewId& aViewIdToBeDeactivated,
                                                   const TVwsViewId& /*aNewlyActivatedViewId*/)
    {
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::FillAiwParameters
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::FillAiwParametersL(
    CAiwGenericParamList& aParamList )
    {
    CMPXCommonListBoxArrayBase* array( iContainer->ListBoxArray() );
    const CMPXMedia& media = array->MediaL(
        iContainer->CurrentLbxItemIndex() );
    TPtrC location( media.ValueText(KMPXMediaGeneralUri) );
    TPtrC mimeType( media.ValueText(KMPXMediaGeneralMimeType) );

    if ( !location.Length() )
        {
        if ( media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == KMPXInvalidItemId )
            {
            // highlighted item is not yet available
            MPX_DEBUG1( "CMPXCollectionViewHgImp::FillAiwParametersL item not yet fetched" );
            User::Leave( KMPXErrDataNotReady );
            }
        else
            {
            TMPXItemId id(
                    media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            CMPXMedia* criteria = CMPXMedia::NewL();
            CleanupStack::PushL( criteria );
            criteria->SetTObjectValueL<TMPXGeneralType>(
                KMPXMediaGeneralType, EMPXItem );
            criteria->SetTObjectValueL<TMPXGeneralCategory>(
                KMPXMediaGeneralCategory, EMPXSong );
            criteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );

            RArray<TMPXAttribute> attrs;
            CleanupClosePushL( attrs );
            attrs.Append( KMPXMediaGeneralUri );
            attrs.Append( KMPXMediaGeneralMimeType );

            CMPXMedia* songInfo = iCollectionUtility->Collection().FindAllL( *criteria,
                                                             attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PushL( songInfo );

            const CMPXMediaArray* mediaArray(
                songInfo->Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
            User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));

            if ( mediaArray->Count() > 0 )
                {
                CMPXMedia* si( mediaArray->AtL( 0 ) );
                location.Set( si->ValueText(KMPXMediaGeneralUri) );
                mimeType.Set( si->ValueText(KMPXMediaGeneralMimeType) );
                }
            CleanupStack::PopAndDestroy( songInfo );
            CleanupStack::PopAndDestroy( criteria );
            }
        }

    TAiwVariant varMime(mimeType);
    if ( !mimeType.Length() && ( iMediaRecognizer ) )
        {
        varMime.Set(iMediaRecognizer->MimeTypeL(location));
        }
    TAiwGenericParam paramMime(EGenericParamMIMEType, varMime);
    aParamList.Reset();
    aParamList.AppendL(paramMime);
    TAiwVariant varFile(location);
    TAiwGenericParam paramFile(EGenericParamFile, varFile);
    aParamList.AppendL(paramFile);
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::GetUint32Presentation
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::GetUint32Presentation( TUint32& aResult,
                                                   const TDesC8& aBuf,
                                                   TInt aOffset )
    {
    aResult = 0;
    const TInt KProEngByteLength( 8 );
    const TInt length( aBuf.Length() );
    const TInt maxBitShift( KProEngByteLength * ( length - 1 ) );

    for( TInt i( 0 ); i<length; ++i )
        {
        aResult |= ( aBuf[aOffset + i] <<
                   ( maxBitShift - ( KProEngByteLength * i ) ) );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::LaunchMusicShopL
// Launch music shop application
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::LaunchOperatorJavaMusicShopL(TUid aUid)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::LaunchOperatorMusicShopL" );
    TThreadId threadId;
    RApaLsSession apaSession;
    User::LeaveIfError( apaSession.Connect() );
    CleanupClosePushL( apaSession );
    apaSession.StartDocument(_L(""), aUid, threadId);
    CleanupStack::PopAndDestroy( &apaSession ); // close apaSession
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::LaunchMusicShopL
// Launch music shop application
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::LaunchOperatorNativeMusicShopL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::LaunchMusicShopL" );

    TApaTaskList taskList( iCoeEnv->WsSession() );
    TApaTask task = taskList.FindApp( TUid::Uid(iOperatorNativeMusicStoreUID) );

    if ( task.Exists() )
        {
        GfxTransEffect::BeginFullScreen( 
        AknTransEffect::EApplicationStart,
            TRect(), 
            AknTransEffect::EParameterType, 
            AknTransEffect::GfxTransParam( TUid::Uid(iOperatorNativeMusicStoreUID),        
            AknTransEffect::TParameter::EActivateExplicitContinue ));                           
        task.BringToForeground();
        }
    else
        {
        RApaLsSession session;
        if ( KErrNone == session.Connect() )
            {
            CleanupClosePushL( session );
            TThreadId threadId;
            session.CreateDocument( KNullDesC, TUid::Uid(iOperatorNativeMusicStoreUID), threadId );
            CleanupStack::PopAndDestroy(&session);
            }
        }
    }
// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::LaunchOperatorURLMusicShopL
// Launch music shop URL application
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::LaunchOperatorURLMusicShopL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::LaunchOperatorURLMusicShopL" );
    const TUid KOSSBrowserUidValue = {0x10008D39}; // 0x1020724D for S60 3rd Ed
    TUid id(KOSSBrowserUidValue);
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(id);

    if ( task.Exists() )
        {
        GfxTransEffect::BeginFullScreen( 
        AknTransEffect::EApplicationStart,
            TRect(), 
            AknTransEffect::EParameterType, 
            AknTransEffect::GfxTransParam( id,        
            AknTransEffect::TParameter::EActivateExplicitContinue ));                            
        task.BringToForeground();
        if ( iOperatorMusicStoreURI->Length() != NULL )
            {
            TBuf8<KMPXMaxHistoryLength> tempUrl;
            tempUrl.Copy(iOperatorMusicStoreURI->Des());
            task.SendMessage(TUid::Uid(0), tempUrl);
            }
        }
    else
        {
        RApaLsSession apaLsSession;
        if ( KErrNone == apaLsSession.Connect() )
            {
            CleanupClosePushL( apaLsSession );
            TThreadId threadId;

 			apaLsSession.StartDocument(*iOperatorMusicStoreURI, KOSSBrowserUidValue, threadId);
         	CleanupStack::PopAndDestroy(&apaLsSession);
            }
		}
	}

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::StoreListboxItemIndexL
// Stores the current list box item index.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::StoreListboxItemIndexL()
	{
    TInt currentIndex( iContainer->CurrentLbxItemIndex() );

	CMPXCommand* command = CMPXCommand::NewL();
	CleanupStack::PushL( command );
	command->SetTObjectValueL<TMPXCommandId>( KMPXCommandGeneralId,
											  KMPXCommandIdCollectionSelect );
	command->SetTObjectValueL( KMPXCommandCollectionSelectIndex, currentIndex );
	command->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
	iCollectionUtility->Collection().CommandL( *command );
	CleanupStack::PopAndDestroy( command );
	}


// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::ChangeCbaVisibility
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::ChangeCbaVisibility( TBool aVisible )
    {
    CEikButtonGroupContainer* cba = Cba();
    if( cba )
		{
        cba->MakeVisible( aVisible );
		cba->DrawDeferred();
		}
    }

void CMPXCollectionViewHgImp::UpdateCba()
    {
    TRAP_IGNORE( 
        {
        CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
        CleanupStack::PushL( cpath );
    
        CMPXCommonListBoxArrayBase* listboxArray(
            iContainer->ListBoxArray() );
        const CMPXMedia& media( listboxArray->ContainerMedia() );
    
        TMPXGeneralType containerType( EMPXNoType );
        if ( media.IsSupported( KMPXMediaGeneralType ) )
            {
            containerType = media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
            }
    
        TMPXGeneralCategory containerCategory( EMPXNoCategory );
        if ( media.IsSupported( KMPXMediaGeneralCategory ) )
            {
            containerCategory = media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
            }
        TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
        CEikButtonGroupContainer* cba = Cba();
        if ( cba && containerType == EMPXGroup && containerCategory == EMPXAlbum && landscapeOrientation )
            {
            cba->SetCommandSetL( R_MPX_COLLECTION_TRANSPARENT_CBA );
            cba->MakeVisible( EFalse );
            cba->DrawDeferred();
            }
        else if ( cba )
            {
			if ( iContainer->IsTBoneView() )
				{
            	cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
				}
			else
				{
				cba->SetCommandSetL(
					( cpath->Levels() == 3 && !iIsEmbedded ) ?
					R_MPX_OPTIONS_HIDE_CBA : R_AVKON_SOFTKEYS_OPTIONS_BACK );
				}
            cba->MakeVisible( ETrue );
            cba->DrawDeferred();
            }
        CleanupStack::PopAndDestroy(cpath);
        });
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::OpenAllSongsL
// Open the collection at all songs level.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::OpenAllSongsL()
	{
	CMPXCollectionPath* path = iCollectionUiHelper->MusicAllSongsPathL();
	CleanupStack::PushL( path );
	iCollectionUtility->Collection().OpenL( *path );
	CleanupStack::PopAndDestroy( path );
	}


// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::OpenArtistAlbumsL
// Open the collection at artist & albums level.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::OpenArtistAlbumsL()
	{
	CMPXCollectionPath* path = iCollectionUiHelper->MusicMenuPathL();
	CleanupStack::PushL( path );
	path->AppendL(KMPXCollectionArtistAlbum);
	iCollectionUtility->Collection().OpenL( *path );
	CleanupStack::PopAndDestroy( path );
	}

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::OpenPlaylistsL
// Open the collection at playlists level.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::OpenPlaylistsL()
	{
	CMPXCollectionPath* path = iCollectionUiHelper->MusicPlaylistPathL();
	CleanupStack::PushL( path );
	iCollectionUtility->Collection().OpenL( *path );
	CleanupStack::PopAndDestroy( path );
	}

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::OpenGenreL
// Open the collection at genre level.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::OpenGenreL()
	{
	CMPXCollectionPath* path = iCollectionUiHelper->MusicMenuPathL();
	CleanupStack::PushL( path );
	path->AppendL(KMPXCollectionGenre); // Genre
	iCollectionUtility->Collection().OpenL( *path );
	CleanupStack::PopAndDestroy( path );
	}

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::OpenPodcastsL
// Open the collection at podcasts level.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::OpenPodcastsL()
	{
	CMPXCollectionPath* path = iCollectionUiHelper->PodCastMenuPathL();
	CleanupStack::PushL( path );
	iCollectionUtility->Collection().OpenL( *path );
	CleanupStack::PopAndDestroy( path );
	}



// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::SaveSelectedAlbumL
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::SaveSelectedAlbumL(CMPXMedia &aMedia)
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SaveSelectedAlbumL" );
    if (iStoredAlbum)
		{
		delete iStoredAlbum;
		}
    iStoredAlbum = CMPXMedia::NewL(aMedia);
    }


// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::RestoreSelectedAlbum
// -----------------------------------------------------------------------------
//
const CMPXMedia* CMPXCollectionViewHgImp::RestoreSelectedAlbum ()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::RestoreSelectedAlbum" );

	return iStoredAlbum;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::DisplayAboutDlgL()
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::DisplayAboutDlgL() const
     {
      TBuf<32> version;
      version.Format(_L("%d.%d.%d"), MUSIC_PLAYER_VERSION_MAJOR, MUSIC_PLAYER_VERSION_MINOR, MUSIC_PLAYER_VERSION_WEEK);
      HBufC* aboutContent  = StringLoader::LoadL(R_QTN_MUS_ABOUT_VERSION ,version);
      HBufC* aboutTitle  = StringLoader::LoadLC(R_QTN_MUS_ABOUT_NOTE);
      CleanupStack::PushL(aboutContent );
      CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*aboutContent );
      CleanupStack::PushL(dlg);
      dlg->SetHeaderTextL(*aboutTitle );
      dlg->ExecuteLD(R_MPX_ABOUT_DIALOG);
      CleanupStack::Pop(dlg);
      CleanupStack::PopAndDestroy(2,aboutTitle ); 
     }
	 
// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::EducatingUserDialog
// Display Educating User Dialog
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::EducatingUserDialog ()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::EducatingUserDialog " );
    CAknMessageQueryDialog* query = new (ELeave) CAknMessageQueryDialog(
            CAknMessageQueryDialog::ENoTone);
    CleanupStack::PushL(query);

    HBufC* message= HBufC::NewLC(500); 

    message->Des().Append(_L("<AknMessageQuery Link>")); 
    message->Des().Append(iEducateUserURI->Des());
    message->Des().Append(_L("</AknMessageQuery Link>"));
    HBufC* promptTxt = StringLoader::LoadLC(
            R_QTN_NMP_NOTE_EDUCATING_THE_USER,message->Des() );
    query->SetMessageTextL( promptTxt->Des());
    TCallBack cb( CMPXCollectionViewHgImp::LinkCallback, this );
    query->SetLink( cb );

    CleanupStack::PopAndDestroy( promptTxt );
    CleanupStack::PopAndDestroy( message );
    CleanupStack::Pop( query );
            
    if(query->ExecuteLD( R_MPX_CUI_EDUCATING_THE_USER_QUERY ))
        {
        iRepository->Set( KEducatingPopupState, EMPXShowEducatingPopup );
        iEducatingPopupState = EMPXShowEducatingPopup;
        }
    else
        {
        iRepository->Set( KEducatingPopupState, EMPXDonotShowEducatingPopup );
        iEducatingPopupState = EMPXDonotShowEducatingPopup; 
        } 
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::LaunchEducatingURL
// Launch Educating URL
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgImp::LaunchEducatingURL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::LaunchEducatingURL" );
    const TUid KOSSBrowserUidValue = {0x10008D39}; // 0x1020724D for S60 3rd Ed
    TUid id(KOSSBrowserUidValue);
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(id);
  
    if ( task.Exists() )
        {
        task.BringToForeground();
        if ( iEducateUserURI->Length() != NULL )
            {
            TBuf8<KMPXMaxHistoryLength> tempUrl;
            tempUrl.Copy(iEducateUserURI->Des());
            task.SendMessage(TUid::Uid(0), tempUrl);
            }
        }
    else
        {
        RApaLsSession apaLsSession;
        if ( KErrNone == apaLsSession.Connect() )
            {
            CleanupClosePushL( apaLsSession );
            TThreadId threadId;

            apaLsSession.StartDocument(*iEducateUserURI, KOSSBrowserUidValue, threadId);
            CleanupStack::PopAndDestroy(&apaLsSession);
            }
        }
     }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::LinkCallback
// Call back for Educating User URL 
// -----------------------------------------------------------------------------
// 
TInt CMPXCollectionViewHgImp::LinkCallback(TAny* aPtr)
    {
    CMPXCollectionViewHgImp* ptr = static_cast<CMPXCollectionViewHgImp*>( aPtr );
    TRAPD( err, ptr->LaunchEducatingURL() );
    MPX_DEBUG2( "CMPXCollectionViewHgImp::LinkCallback()-err:%d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgImp::NeedToShowEducatingDialog
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgImp::NeedToShowEducatingDialog()
    {
    if(iEducateUserURI->Length() > 0 && iCollectionReady && //If educating user URI is in cenrep and Collection is Ready
      //(iEducatingPopupState == EMPXInitialEducatingPopup || //If Music Player is launched first time after phone flash 
      (iEducatingPopupState == EMPXShowEducatingPopup && iPopuponRefresh)) //If user pressed yes on "Remind me later" and its manual refresh with some song added
        {
        return ETrue;
        }
    else
        return EFalse;
    }

void CMPXCollectionViewHgImp::DimAllOptions(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    switch ( aResourceId )
        {
        case R_MPX_COLLECTION_VIEW_MENU_1:
            {
            aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdGoToArtistAlbums, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdGoToAllSongs, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdGoToPlaylists, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdGoToGenre, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdGoToPodcasts, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdGoToMusicShop, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdGoToMultipleMusicShop, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdMusicLibraryDetails, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdRefreshLibrary, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAbout, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdFind, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUpnpPlayVia, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdCreatePlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddToPlaylist, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAddSongs, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdReorder, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdSend, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdDelete, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdRemove, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdPlayItem, ETrue );
            break;
            }
        case R_MPX_COLLECTION_VIEW_MENU_2:
            {
            aMenuPane->SetItemDimmed( EMPXCmdRename, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAlbumArt, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdFindInMusicShop, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdPlaylistDetails, ETrue );
            break;
            }
        case R_AVKON_MENUPANE_MARKABLE_LIST:
            {
            aMenuPane->SetItemDimmed(EAknCmdEditListMenu, ETrue);
            break;
            }
        }    
    }
//  End of File
