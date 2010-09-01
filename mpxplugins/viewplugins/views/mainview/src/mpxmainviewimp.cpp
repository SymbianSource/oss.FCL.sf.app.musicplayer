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
* Description:  Implementation of Main view
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <eikfrlb.h>
#include <eikfrlbd.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <akntitle.h>
#include <textresolver.h>
#include <aknnotewrappers.h>
#include <aknnavi.h>
#include <hlplch.h>
#include <bautils.h>
#include <eikspmod.h>
#include <eikclb.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <bldvariant.hrh>
#include <layoutmetadata.cdl.h>
#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include <mplayersecondarydisplayapi.h>

#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <mpxcollectionutility.h>
#include <mpxplaybackmessage.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxviewpluginmanager.h>

#include <mpxcollectionplaylist.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxmainview.rsg>
#include <mpxlog.h>
#include <mpxcollectionpath.h>
#include <mpxviewplugin.h>
#include <mpxcollectionplugin.hrh>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxuser.h>
#include "mpxcommonuihelper.h"
#include "mpxalbumartutil.h"
#include "mpxmainview.hrh"
#include "mpxmainviewimp.h"
#include "mpxmainviewcontainer.h"
#include <mpxappui.hrh>

// CONSTANTS
_LIT( KMPXMainRscPath, "mpxmainview.rsc" );
_LIT( KMPXTab, "\t" );
_LIT( KMPXSpace, " " );

const TInt KWaitNoteImpUid = 0x101FFC6C;

// extra size for buffers
const TInt KMPXDefaultBufferLength = 255;
const TInt KMPXExtraBufferLength = 50;
const TInt KSecondInMilliseconds = 1000;
const TInt KOneHourInSeconds = 3600;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXMainViewImp* CMPXMainViewImp::NewL()
    {
    MPX_FUNC( "CMPXMainViewImp::NewL" );
    CMPXMainViewImp* self = CMPXMainViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXMainViewImp* CMPXMainViewImp::NewLC()
    {
    CMPXMainViewImp* self = new ( ELeave ) CMPXMainViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMainViewImp::~CMPXMainViewImp()
    {
    MPX_DEBUG1( "CMPXMainViewImp::~CMPXMainViewImp entering" );

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
        iViewUtility->Close();
        }

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }

    delete iMPXUtility;
    delete iTitle;
    delete iCommonUiHelper;
    delete iCurPath;
    delete iMediaArray;
    delete iIdle;
    MPX_DEBUG1( "CMPXMainViewImp::~CMPXMainViewImp exiting" );
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXMainViewImp::CMPXMainViewImp()
    {
    MPX_FUNC( "CMPXMainViewImp::CMPXMainViewImp" );
    iInitDBNeeded = EFalse;
    iInitDBCorrupted = EFalse;
    iCurSystemEvent = KErrNotFound;
    iShowPlaybackIndicator = EFalse;
    iColInitialized = EFalse;
    iRefreshingCollection = EFalse;
    iCurPlugin = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXMainViewImp::ConstructL" );

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXMainRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    BaseConstructL( R_MPX_MAIN_VIEW );
    iViewUtility = MMPXViewUtility::UtilityL();
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );
    iCommonUiHelper = CMPXCommonUiHelper::NewL();
    iMPXUtility = CMPXAlbumArtUtil::NewL();
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iAlbumArtIndex = KMPXMainViewDefaultIcon;

    TMPXPlaybackState state = iPlaybackUtility->StateL();
    if( state == EPbStatePlaying ||
        state == EPbStatePaused )
        {
        iShowPlaybackIndicator = ETrue;
        }
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    iTitle = HBufC::NewL( 0 );

#ifdef _DEBUG
    iExitOptionHidden = EFalse;
#else // _DEBUG
    iExitOptionHidden = iCommonUiHelper->ExitOptionHiddenL() &&
        !iEikonEnv->StartedAsServerApp();
#endif // _DEBUG
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleErrorL( TInt aError )
    {
    if ( aError )
        {
        MPX_DEBUG2( "CMPXMainViewImp::HandleErrorL(%d)", aError );

        // TextResolver instance for error resolving.
        CTextResolver* textresolver = CTextResolver::NewLC();
        // Resolve the error text
        const TDesC& text =
            textresolver->ResolveErrorString( aError );

        CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
        dlg->ExecuteLD( text );
        CleanupStack::PopAndDestroy( textresolver );
        }
    }

// ---------------------------------------------------------------------------
// Function to display information notes
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DisplayInfoNoteL( TInt aResourceId )
    {
    MPX_FUNC( "CMPXMainViewImp::DisplayInfoNoteL" );
    HBufC* text = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------------------------
// Function to display confirmation notes
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DisplayConfirmionNoteL( TInt aResourceId )
    {
    MPX_FUNC( "CMPXMainViewImp::DisplayConfirmionNoteL" );
    HBufC* text = StringLoader::LoadLC( aResourceId );
    CAknConfirmationNote* dlg = new ( ELeave ) CAknConfirmationNote( ETrue );
    dlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection message
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleCollectionMessage(
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
void CMPXMainViewImp::HandleOpenL(
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool /*aComplete*/,
    TInt aError )
    {
    MPX_FUNC( "CMPXMainViewImp::HandleOpenL 4" );
    if( aError == KErrNone )
        {
        UpdateListBoxL( aEntries, aIndex );

        UpdatePluginInfo();
        }
    else if ( aError == KErrCorrupt )
        {
        DoHandleCorruptMsgL();
        }
    else
        {
        HandleErrorL( aError );
        }
    MPX_PERF_CHECKPT("MPX Main view displayed");
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d an item. Client typically responds by
// 'playing' the item
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    // Do Nothing: playback/fetch client should handle this stage
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle media properties
// Notes: The client is responsible for delete the object of aMedia
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError)
    {
    MPX_FUNC( "CMPXMainViewImp::HandleCollectionMediaL" );

    if (aError == KErrNoMemory)
        {
        TRAP_IGNORE(
                HandleErrorL( aError );
                AppUi()->HandleCommandL( EEikCmdExit );
        );
        User::Exit(KErrNoMemory);
        return;
        }

    if (aError == KErrDiskFull)
        {
             iOutOfDisk = ETrue;
             CloseWaitNoteL();
             iCurPlugin = KErrNotFound;
             DoRetrieveDetailsL(ETrue);
        }

    if ( aError != KErrCorrupt )
        {

        if(!iOutOfDisk)
            {
            // Check to see if database has been created for this item
            //
            TBool dbCreated = ETrue;
            if( aMedia.IsSupported(KMPXMediaColDetailDBCreated))
                {
                dbCreated = aMedia.ValueTObjectL<TBool>(KMPXMediaColDetailDBCreated);
                MPX_DEBUG2("CMPXMainViewImp::HandleCollectionMediaL dbcreated %i", dbCreated);
                }
            iInitDBNeeded |= !dbCreated;  //lint !e514

            // Check to see if database was corrupted for this item
            //
            TBool dbCorrupted = EFalse;
            if( aMedia.IsSupported(KMPXMediaColDetailDBCorrupted))
                {
                dbCorrupted = aMedia.ValueTObjectL<TBool>(KMPXMediaColDetailDBCorrupted);
                }
            iInitDBCorrupted |= dbCorrupted;
            }

        // Update the list box item, iCurPlugin++ because first item is pb state
        //
        iCurPlugin++;
        UpdateItemCountTotalL( aMedia );


        // Check to see if we need to do initial scan or refresh due to db corruption
        //
        if( iCurPlugin == iCurPath->Count() )
            {
            MPX_DEBUG1("CMPXMainViewImp::HandleCollectionMediaL plugins MediaL loaded");
            if ( !iColInitialized )
                {
                CloseWaitNoteL();
                iColInitialized = ETrue;
                }

            if( (iInitDBNeeded || iInitDBCorrupted) && !iOutOfDisk )
                {
                MPX_DEBUG1("CMPXMainViewImp::HandleCollectionMediaL starting scan");
                TWaitNoteType noteType = iInitDBCorrupted ? EMPXCorruptScanningNote :
                                                            EMPXScanningNote;

                StartWaitNoteL( noteType );
                iInitDBNeeded = EFalse;
                iInitDBCorrupted = EFalse;
                iRefreshingCollection = ETrue;
                iDelayedUsbRefresh = EFalse;
                }
            else if( iDelayedUsbRefresh )
                {
                DoHandleBroadcastMsgL( EMcMsgUSBMassStorageEnd );
                }
            iCurPlugin = KErrNotFound;
            }
        else if( iCurPlugin < iCurPath->Count() )  // Fetch next
            {
            // Set which plugin
            iCurPath->Set(iCurPlugin);

            // Fetch the extra data
            RArray<TMPXAttribute> atts;
            CleanupClosePushL( atts );
            if(!iOutOfDisk)
                {
                atts.Append(KMPXMediaColDetailDBCreated);
                atts.Append(KMPXMediaColDetailDBCorrupted);
                }
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
            atts.Append(KMPXMediaGeneralTitle);
            atts.Append(KMPXMediaGeneralSubTitle);
            if(!iOutOfDisk)
                {
                atts.Append(KMPXMediaGeneralCount);
                }
            atts.Append(KMPXMediaGeneralIcon);
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU

            MPX_DEBUG1("CMPXMainViewImp::HandleCollectionMediaL fetch next");
            // Only fetch next if we haven't been interrupted by another event
            //
            if( iCurSystemEvent == KErrNotFound )
                {
                iCollectionUtility->Collection().MediaL(*iCurPath, atts.Array() );
                }
            else
                {
                // Reset state machine
                iCurPlugin = KErrNotFound;
                }
            CleanupStack::PopAndDestroy( &atts );
            }

        if ( aError != KErrNone ) //lint !e961
            {
            HandleErrorL( aError );
            }
        }
    else
        {
        DoHandleCorruptMsgL();
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_DEBUG1( "CMPXMainViewImp::HandlePropertyL" );
    TRAP_IGNORE( DoHandlePropertyL( aProperty, aValue, aError ) );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle extended media properties.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_DEBUG2( "CMPXMainViewImp::HandleMediaL %i", aError );
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ) );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXMainViewImp::Id() const
    {
    return TUid::Uid( KMPXMainViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleCommandL( TInt aCommand )
    {
    MPX_DEBUG2( "CMPXMainViewImp::HandleCommandL(%d) entering", aCommand );

    switch (aCommand)
        {
        case EMPXCmdGoToNowPlaying:
            {
            // update view history first
            iViewUtility->PushDefaultHistoryL();
            //then open the view first
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        case EMPXMainViewRefresh:
            {
            MPX_PERF_CHECKPT("Refresh collection library");
            // Package the argument from 8bit to 16bit then activate view
            //
            StartWaitNoteL( EMPXRefreshingNote );
            iPlaybackUtility->CommandL( EPbCmdStop );
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(
                iEikonEnv->WsSession(),
                AppUi()->AppHelpContextL() );
            break;
            }
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    MPX_DEBUG1( "CMPXMainViewImp::HandleCommandL() exiting" );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoActivateL(
    const TVwsViewId& /* aPrevViewId */,
    TUid /* aCustomMessageId */,
    const TDesC8& /* aCustomMessage */ )
    {
    MPX_FUNC( "CMPXMainViewImp::DoActivateL()" );
    StatusPane()->MakeVisible( ETrue );
    if ( StatusPane()->CurrentLayoutResId() !=
         R_AVKON_STATUS_PANE_LAYOUT_USUAL)
        {
        StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }

    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CMPXMainViewContainer( this );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }

    // Set title
    CAknTitlePane* title( NULL );

    TRAP_IGNORE(
        {
        title = static_cast<CAknTitlePane*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        } );
    if ( title )
        {
        HBufC* titleText = StringLoader::LoadLC(
            R_MPX_MAIN_VIEW_TITLE );
        title->SetTextL( *titleText );
        CleanupStack::PopAndDestroy( titleText );
        }

    CEikButtonGroupContainer* cbgc = CEikButtonGroupContainer::Current();
    if ( cbgc )
        {
        if ( iViewUtility->ViewHistoryDepth() == 1 )
            {
            cbgc->SetCommandSetL( R_MPX_CUI_SOFTKEYS_OPTIONS_PHONE );
            }
        else
            {
            cbgc->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
            }

#ifdef __ENABLE_MSK
        iCommonUiHelper->SetMiddleSoftKeyLabelL(
            *cbgc,
            R_QTN_MSK_OPEN,
            EAknSoftkeyForwardKeyEvent );
#endif // __ENABLE_MSK

        cbgc->DrawDeferred();
        }

    iAlbumArtIndex = KMPXMainViewTransparent;
    iPlaybackState = iPlaybackUtility->StateL();
    MMPXSource* s = iPlaybackUtility->Source();
    if ( s )
        {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        attrs.Append(
            TMPXAttribute( KMPXMediaIdGeneral,
            EMPXMediaGeneralTitle | EMPXMediaGeneralUri ) );
        attrs.Append(KMPXMediaMusicAlbumArtFileName);
        s->MediaL( attrs.Array(), *this );
        CleanupStack::PopAndDestroy( &attrs );

        CMPXCollectionPlaylist* pl = s->PlaylistL();
        if( pl )
            {
            CleanupStack::PushL( pl );
            iCurPlPlayPosition = pl->Index() + 1; // offset for 0th index
            iCurPlTotalCount   = pl->Count();
            CleanupStack::PopAndDestroy( pl );
            }
        }
    UpdatePlaybackViewStatusL();

    if ( iCollectionUtility )
        {
        if( iCurSystemEvent == EMcMsgUSBMassStorageStart ||
            iCurSystemEvent == EMcMsgUSBMTPStart )
            {
            // Fix grey out problem by re-starting the note
            //
            CMPXViewPlugin* pi =
                    iViewUtility->ViewPluginManager().PluginL( TUid::Uid(KWaitNoteImpUid) );
            pi->DeactivateView();
            DoHandleBroadcastMsgL( iCurSystemEvent );
            iColInitialized = ETrue;
            }
        else if ( !iColInitialized )

            {
            StartWaitNoteL( EMPXOpeningNote );
            }

        // Path Depth can be 2 if rapidly pressing back.
        // In this case, we wait until the collection container opened message
        // is sent
        //
        CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( cPath );
        MPX_DEBUG2("CMPXMainViewImp::DoActivateL path depth %i", cPath->Levels());
        if( cPath->Levels() == 1 )
            {
            iCollectionUtility->Collection().OpenL();
            iPathCorrupted = EFalse;
            }
        else
            {
            iPathCorrupted = ETrue;
            }
        CleanupStack::PopAndDestroy( cPath );
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoDeactivate()
    {
    MPX_FUNC( "CMPXMainViewImp::DoDeactivate" );
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleForegroundEventL( TBool aForeground )
    {
    MPX_FUNC( "CMPXMainViewImp::HandleForegroundEventL" );
    CAknView::HandleForegroundEventL( aForeground );
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXMainViewImp::DynInitMenuPaneL" );
    switch ( aResourceId )
        {
        case R_MPX_MAIN_VIEW_MENU:
            {
            aMenuPane->SetItemDimmed( EAknCmdExit, iExitOptionHidden );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    AknSelectionService::HandleMarkableListDynInitMenuPane(
        aResourceId,
        aMenuPane,
        iContainer->ListBox() );
    }

// ---------------------------------------------------------------------------
// From MEikListBoxObserver
// Handles listbox events.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType)
    {
    MPX_FUNC( "CMPXMainViewImp::HandleListBoxEventL" );

    if ( aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked
#ifdef SINGLE_CLICK_INCLUDED
        || aEventType == EEventItemSingleClicked    
#endif
        )
        {
        TInt currentIndex = iContainer->ListBox()->CurrentItemIndex();
        if ( currentIndex == 0 && iShowPlaybackIndicator ) //lint !e961
            {
            MMPXPlayer* player =
                iPlaybackUtility->PlayerManager().CurrentPlayer();
            TUid pluginUid( KNullUid );
            RArray<TUid> array;
            CleanupClosePushL( array );

            if ( player )
                {
                pluginUid = player->UidL();
                MPX_DEBUG2( "CMPXMainViewImp::HandleListBoxEventL player UID = 0x%x", pluginUid.iUid );
                array.AppendL( pluginUid );
                }
            else
                {
                array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ) );
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
                        TMPXItemId cPathUid = pbPath->Id( 0 );
                        MPX_DEBUG2( "CMPXMainViewImp::HandleListBoxEventL collection uid = 0x%x", cPathUid.iId1 );
                        array.AppendL( TUid::Uid( cPathUid ) );
                        }
                    CleanupStack::PopAndDestroy( pbPath );
                    CleanupStack::PopAndDestroy( playlist );
                    }
                }
            // Forced jump, clear main view from view history
            //
            iViewUtility->PushDefaultHistoryL();
            iViewUtility->ActivateViewL( array );

            CleanupStack::PopAndDestroy( &array );
            }
        else if ( currentIndex > -1 ) //lint !e961
            {
            // offset by 1 for playback view
            TInt offset = iShowPlaybackIndicator ? 1 : 0;
            iCollectionUtility->Collection().OpenL( currentIndex - offset );
            }
        }  //lint !e961
    }

// ---------------------------------------------------------------------------
// From MMPXAlbumArtUtilObserver
// Notify that extraction of album art started.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::ExtractAlbumArtStarted()
    {
    MPX_DEBUG1("CMPXMainViewImp::ExtractAlbumArtStarted()");
    iAlbumArtIndex = KMPXMainViewTransparent;
    }

// ---------------------------------------------------------------------------
// From MMPXAlbumArtUtilObserver
// Album art is extracted from file and ready to use.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::ExtractAlbumArtCompleted(
    CFbsBitmap* aBitmap, TInt aErr )
    {
    TRAP_IGNORE( DoExtractAlbumArtCompletedL( aBitmap, aErr ) );
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXMainViewImp::DoHandlePlaybackMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EPropertyChanged:
                {
                TMPXPlaybackProperty property(
                    static_cast<TMPXPlaybackProperty>( type ) );
                TInt error( KErrNone );

                DoHandlePropertyL( property, data, error );
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXMainViewImp::HandlePlaybackMessageL - EStateChanged(%d)", type );

                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );

                DoHandleStateChangedL( state );
                break;
                }
            case TMPXPlaybackMessage::EMediaChanged:
                {
                if ( iContainer )
                    {
                    MMPXSource* s = iPlaybackUtility->Source();
                    if ( s )
                        {
                        RArray<TMPXAttribute> attrs;
                        CleanupClosePushL(attrs);
                        attrs.Append(
                            TMPXAttribute( KMPXMediaIdGeneral,
                            EMPXMediaGeneralTitle | EMPXMediaGeneralUri ) );
                        attrs.Append(KMPXMediaMusicAlbumArtFileName);
                        s->MediaL( attrs.Array(), *this );
                        CleanupStack::PopAndDestroy( &attrs );

                        CMPXCollectionPlaylist* pl = s->PlaylistL();
                        if( pl )
                            {
                            CleanupStack::PushL( pl );
                            iCurPlPlayPosition = pl->Index() + 1; // offset for 0th index
                            iCurPlTotalCount   = pl->Count();
                            CleanupStack::PopAndDestroy( pl );
                            }
                        }
                    }
                break;
                }
            case TMPXPlaybackMessage::ECommandReceived:
                {
                MPX_DEBUG2( "CMPXMainViewImp - ECommandReceived(%d)", type );
                break;
                }
            case TMPXPlaybackMessage::EActivePlayerChanged:
                {
                MPX_DEBUG3( "CMPXMainViewImp - EActivePlayerChanged(%d, %d)", type, data );
                if ( type && data )
                    {
                    // binded to active player, change state to active
                    DoHandleStateChangedL( EPbStatePlaying );
                    // refresh media property
                    MMPXSource* s = iPlaybackUtility->Source();
                    if ( s )
                        {
                        RArray<TMPXAttribute> attrs;
                        CleanupClosePushL(attrs);
                        attrs.Append(
                            TMPXAttribute( KMPXMediaIdGeneral,
                            EMPXMediaGeneralTitle | EMPXMediaGeneralUri ) );
                        attrs.Append(KMPXMediaMusicAlbumArtFileName);
                        s->MediaL( attrs.Array(), *this );
                        CleanupStack::PopAndDestroy( &attrs );

                        CMPXCollectionPlaylist* pl = s->PlaylistL();
                        if( pl )
                            {
                            CleanupStack::PushL( pl );
                            iCurPlPlayPosition = pl->Index() + 1; // offset for 0th index
                            iCurPlTotalCount   = pl->Count();
                            CleanupStack::PopAndDestroy( pl );
                            }
                        }
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXMainViewImp::DoHandlePropertyL" );
    MPX_DEBUG4( "CMPXMainViewImp::DoHandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    if ( KErrNone == aError )
        {
        switch ( aProperty  )
            {
            case EPbPropertyPosition:
                {
                iPosition = aValue;
                break;
                }
            case EPbPropertyDuration:
                {
                iDuration = aValue;
                break;
                }
            default:
                {
                break;
                }
            }
        UpdatePlaybackViewStatusL();
        }
    }

// ---------------------------------------------------------------------------
// Handle collection messages.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoHandleCollectionMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXCommandHandler::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt op( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        // End of refresh message, update the list box text
        //
        if( event == TMPXCollectionMessage::EBroadcastEvent &&
            op == EMcMsgRefreshEnd )
            {
            MPX_DEBUG1("CMPXMainViewImp::HandleCollectionMessageL - refresh end");
            iRefreshingCollection = EFalse;
            iOutOfDisk = data == KErrDiskFull ? ETrue : EFalse;

            if (!iOutOfDisk)
                {
                ResynchronizeCollectionL();
                }
            else
                {
                iCurPlugin = KErrNotFound;
                iInitDBNeeded = EFalse;
                iInitDBCorrupted = EFalse;
                }

            DoRetrieveDetailsL(iOutOfDisk);
            DoSetDefaultFocusL();
            }
        else if(event == TMPXCollectionMessage::EBroadcastEvent &&
                op == EMcMsgRefreshStart)
            {
            iRefreshingCollection = ETrue;
            }
        // Handle other broadcast messages
        //
        else if( event == TMPXCollectionMessage::EBroadcastEvent )
            {
            MPX_DEBUG1("CMPXMainViewImp::HandleCollectionMessageL - broadcast");
            DoHandleBroadcastMsgL( op );
            }
        // if not currently refreshing, the main view should make sure
        // the collection item counters are always up to date
        else if( event == TMPXCollectionMessage::EItemChanged &&
                 !iRefreshingCollection )
            {
            MPX_DEBUG1("CMPXMainViewImp::HandleCollectionMessageL - item changed");

            if((op == EMPXItemInserted || op == EMPXItemDeleted) &&
               (data >> 28 == EMPXCollection)) //lint !e702 only update if collection has changed
                {
                // refresh since the podcasting collection has been updated
                DoRetrieveDetailsL();
                }
            // else ignore
            }
        // If user rapidly presses back, main view gets activated with a Path of level 2
        // but the path change to level 1 msg comes later
        //
        else if( event == TMPXCollectionMessage::EPathChanged &&
                 op == EMcPathChangedByOpen &&
                 data == EMcContainerOpened )
            {
            CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cPath );
            MPX_DEBUG2("CMPXMainViewImp::HandleCollectionMessageL - Path Changed By Open %i", cPath->Levels() );
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
            if( cPath->Levels() == 1 && iContainer )
                {
                iCurPlugin = KErrNotFound;
                iCollectionUtility->Collection().CancelRequest();
                iCollectionUtility->Collection().OpenL();
                }
#else // __ENABLE_PODCAST_IN_MUSIC_MENU
            if( cPath->Levels() == 2 ) // Music menu
                {
                DoRetrieveDetailsL();
                }
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU

            CleanupStack::PopAndDestroy( cPath );
            }
        else if( event == TMPXCollectionMessage::EFocusChanged &&
                 iContainer )
            {
            TInt offset = iShowPlaybackIndicator ? 1 : 0;
            iContainer->ListBox()->SetCurrentItemIndex( data + offset );
            }
        else if ( event == TMPXCollectionMessage::ECollectionChanged &&
                  iContainer && iPathCorrupted)
            {
            MPX_DEBUG1("CMPXMainViewImp::HandleCollectionMessageL -- Collection changed and Levels 1");
            CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cPath );
            if( cPath->Levels() == 1 )
                {
                iCurPlugin = KErrNotFound;
                iCollectionUtility->Collection().CancelRequest();
                iCollectionUtility->Collection().OpenL();
                iPathCorrupted = EFalse;
                }
            CleanupStack::PopAndDestroy( cPath );
            }
        }
    else if( KMPXMessageIdItemChanged == id )
        {
        // Loop through messages for arrays.
        //
        if (aMessage.IsSupported(KMPXMessageArrayContents))
            {
            const CMPXMessageArray* messageArray =
                        aMessage.Value<CMPXMessageArray>(KMPXMessageArrayContents);
            User::LeaveIfNull(const_cast<CMPXMessageArray*>(messageArray));
            
            for( TInt i=0; i<messageArray->Count(); ++i )
                  {
                  HandleCollectionMessage( (*messageArray)[i], KErrNone );
                  }
            }
        // Single item
        else
            {
            MPX_DEBUG1("CMPXScanningWaitDialog::HandleCollectionMessageL KMPXMessageIdItemChanged");
            TMPXChangeEventType op( aMessage.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
            TMPXItemId data( aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId) );
            if( (op == EMPXItemInserted || op == EMPXItemDeleted) &&
                (data.iId2 >> 28 == EMPXCollection) &&
                !iRefreshingCollection ) //lint !e702 only update if collection has changed
                {
                // refresh since the podcasting collection has been updated
                DoRetrieveDetailsL();
                }
            }
        }//lint !e961
    }

// ---------------------------------------------------------------------------
// Handle media properties.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoHandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXMainViewImp::DoHandleMediaL" );
    delete iTitle;
    iTitle = NULL;
    iTitle = aMedia.ValueText(KMPXMediaGeneralTitle).AllocL();

    if ( KErrNone == aError )
        {
        if ( iContainer )
            {
            TInt err( KErrNone );
            MPX_TRAP( err,
                iMPXUtility->ExtractAlbumArtL(
                    aMedia,
                    *this,
                    TSize( 50, 50 ) ) );
            MPX_DEBUG2( "CMPXMainViewImp::DoHandleMediaPropertiesL err = %d", err );
            if ( err != KErrNone )
                {
                iAlbumArtIndex = KMPXMainViewDefaultIcon;
                }
            }
         else
            {
            if ( aMedia.IsSupported(KMPXMediaMusicAlbumArtFileName))
                {
                iAlbumArtIndex = KMPXMainViewTransparent;
                }
            else
                {
                iAlbumArtIndex = KMPXMainViewDefaultIcon;
                }
            }
        }
    else
        {
        iAlbumArtIndex = KMPXMainViewDefaultIcon;
        }

    // check to see if default podcast album art should be displayed instead
    // of default music album art icon
    if(iAlbumArtIndex == KMPXMainViewDefaultIcon &&
       aMedia.IsSupported(KMPXMediaGeneralCategory) &&
       EMPXPodcast == aMedia.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralCategory))  //lint !e650 !e58
        {
        iAlbumArtIndex = KMPXMainViewDefaultPodcastIcon;
        }

    UpdatePlaybackViewStatusL();
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoHandleStateChangedL(
    TMPXPlaybackState aState )
    {
    MPX_FUNC( "CMPXMainViewImp::DoHandleStateChangedL" );

    // Store a state only if we are not seeking
    //
    if( aState != EPbStateSeekingForward &&
        aState != EPbStateSeekingBackward )
        {
        iPlaybackState = aState;
        }
    UpdatePlaybackViewStatusL();
    }

// ---------------------------------------------------------------------------
// Updates list box
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::UpdateListBoxL(
    const CMPXMedia& aEntries,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXMainViewImp::UpdateListBox" );

    if ( iContainer )
        {
        CDesCArray* array =
            static_cast<CDesCArray*>(
            iContainer->ListBox()->Model()->ItemTextArray() );

        // preserve playback status
        if ( (array->MdcaCount() > 0) && iShowPlaybackIndicator )
            {
            HBufC* dataItem = array->MdcaPoint( 0 ).AllocLC();
            array->Reset();
            array->AppendL( *dataItem );
            CleanupStack::PopAndDestroy( dataItem );
            dataItem = NULL;
            }
        else
            {
            array->Reset();
            }

        delete iMediaArray;
        iMediaArray = NULL;
        CMPXMediaArray* mediaAry( aEntries.Value<CMPXMediaArray>(KMPXMediaArrayContents) );
        User::LeaveIfNull( mediaAry );
        iMediaArray = CMPXMediaArray::NewL( *mediaAry );
        TInt entriesCount = iMediaArray->Count();
        CMPXMedia* entry = NULL;
        for( TInt i=0; i < entriesCount; i++ )
            {
            entry = iMediaArray->AtL( i );
            const TDesC& title = entry->ValueText(KMPXMediaGeneralTitle);
            MPX_DEBUG2( "CMPXMainViewImp::UpdateListBox Name: %S", &title );
            MPX_DEBUG3( "CMPXMainViewImp::UpdateListBox Type: %d, Category: %d",
                        entry->ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType),
                        entry->ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory));
            HBufC* dataItem = HBufC::NewLC( title.Length() + KMPXExtraBufferLength );
            dataItem->Des().Format( _L("0\t%S\t\t\t"), &title );
            array->AppendL( *dataItem );
            CleanupStack::PopAndDestroy( dataItem );
            }
        iContainer->ListBox()->HandleItemAdditionL();
        if ( iViewUtility->PreviousViewType().iUid == KMPXPluginTypeCollectionUid )
            {
            if ( aIndex < iContainer->ListBox()->Model()->NumberOfItems() )
                {
                TInt offset = iShowPlaybackIndicator ? 1 : 0;
                iContainer->ListBox()->SetCurrentItemIndexAndDraw( aIndex + offset );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// Function to update the item count at the current item
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::UpdateItemCountTotalL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXMainViewImp::UpdateItemCountTotalL" );

    if( iContainer )
        {
        // Offset for icons and array position
        TInt offset = iShowPlaybackIndicator ? 0 : 1;

        // Listbox descriptor array
        CDesCArray* array = static_cast<CDesCArray*>(
                            iContainer->ListBox()->Model()->ItemTextArray() );

        // Number of items
        if(!aMedia.IsSupported(KMPXMediaGeneralSubTitle) ||
           !aMedia.IsSupported(KMPXMediaGeneralCount))
            {
            User::Leave(KErrArgument);
            }

        const TDesC& subTitle = aMedia.ValueText(KMPXMediaGeneralSubTitle);
        const TInt numItems = aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralCount);

        HBufC* formattedSubTitle(NULL);

        if(numItems != 1)
            {
            formattedSubTitle = HBufC::NewLC( subTitle.Length() + 7 ); // magic, >>10,000 episodes
            TPtr ptr = formattedSubTitle->Des();
            StringLoader::Format(ptr, subTitle, -1, numItems);
            }
        else
            {
            formattedSubTitle = subTitle.Alloc();
            CleanupStack::PushL(formattedSubTitle);
            }

        // Plugin Title
        const TDesC& title = aMedia.ValueText(KMPXMediaGeneralTitle);

        // Plugin Icon and Mask
        TIconInfo icon;
        icon.bmpfile = KNullDesC;
        CGulIcon* gulicon(NULL);
        TInt iconIndex(0);
        if(aMedia.IsSupported(KMPXMediaGeneralIcon))
            {
            MPX_DEBUG1("CMPXMainViewImp::UpdateItemCountTotalL - Got an Icon");
            icon = aMedia.ValueTObjectL<TIconInfo>(KMPXMediaGeneralIcon);
            // Create the icon
            MAknsSkinInstance* skin( AknsUtils::SkinInstance() );
            CFbsBitmap* bmp(NULL);
            CFbsBitmap* mask(NULL);
            TParse parse;
            parse.Set( icon.bmpfile , &KDC_APP_RESOURCE_DIR, NULL);
            TFileName iconFile( parse.FullName() );
            User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

            AknsUtils::CreateIconLC(
                skin,
                KAknsIIDNone,
                bmp,
                mask,
                iconFile,
                icon.bitmapId,
                icon.maskId );

            gulicon = CGulIcon::NewL( bmp, mask );
            CleanupStack::Pop( 2 ); // bmp, mask

            // Add it to the list
            CArrayPtr<CGulIcon>* icons = static_cast<CEikFormattedCellListBox*>(
                    iContainer->ListBox() )->ItemDrawer()->
                    FormattedCellData()->IconArray();
            icons->AppendL( gulicon );
            iconIndex = icons->Count()-1;
            }

        // Reformat item and insert
        //
        MPX_DEBUG1("CMPXMainViewImp::UpdateItemCountTotalL - Updating item");
        HBufC* dataItem = HBufC::NewLC( title.Length() + formattedSubTitle->Length()
                                        + KMPXExtraBufferLength );
        dataItem->Des().Format( _L("%d\t%S\t%S\t\t"), iconIndex, &title, formattedSubTitle );
        if( array->Count() > (iCurPlugin-offset) )
            {
            array->Delete(iCurPlugin-offset);
            array->InsertL(iCurPlugin-offset, *dataItem);
            }
        CleanupStack::PopAndDestroy( dataItem );
        CleanupStack::PopAndDestroy(formattedSubTitle);

        // Redraw listbox
        iContainer->ListBox()->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// Updates playback view status
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::UpdatePlaybackViewStatusL()
    {
    MPX_FUNC( "CMPXMainViewImp::UpdatePlaybackViewStatusL" );

    TBool oldState( iShowPlaybackIndicator );
    MPX_DEBUG2( "CMPXMainViewImp::UpdatePlaybackViewStatusL iPlaybackState = %d", iPlaybackState );

    if ( iPlaybackState != EPbStateInitialising )
        {
        // do not change state if it's in EPbStateInitialising state
        if( iPlaybackState == EPbStatePlaying ||
            iPlaybackState == EPbStatePaused ||
            iPlaybackState == EPbStateSeekingForward ||
            iPlaybackState == EPbStateSeekingBackward )
            {
            iShowPlaybackIndicator = ETrue;
            }
        else
            {
            // Remove indicator if it was being shown
            //
            if( iContainer && iShowPlaybackIndicator )
                {
                TInt currentIndex( iContainer->ListBox()->CurrentItemIndex() );
                MPX_DEBUG2( "CMPXMainViewImp::UpdatePlaybackViewStatusL currentIndex = %d", currentIndex );
                CDesCArray* array =
                    static_cast<CDesCArray*>(
                    iContainer->ListBox()->Model()->ItemTextArray() );

                if ( array->Count() > 0 )
                    {
                    array->Delete( 0 );
                    }
                iContainer->ListBox()->HandleItemAdditionL();
                if ( currentIndex > 0 )
                    {
                    // to preseve current selection index
                    currentIndex--;
                    MPX_DEBUG2( "CMPXMainViewImp::UpdatePlaybackViewStatusL setting index = %d", currentIndex );
                    iContainer->ListBox()->SetCurrentItemIndexAndDraw( currentIndex );
                    }
                }
            MPX_DEBUG1( "CMPXMainViewImp::UpdatePlaybackViewStatusL dismissing playback indicator" );
            iShowPlaybackIndicator = EFalse;
            }
        }
    else
        {
        MPX_DEBUG1( "CMPXMainViewImp::UpdatePlaybackViewStatusL initializing, state unchanged" );
        }

    // Show the item only if we want to show indicator
    //
    if ( iContainer && iShowPlaybackIndicator )
        {
        CDesCArray* array =
            static_cast<CDesCArray*>(
            iContainer->ListBox()->Model()->ItemTextArray() );

        TInt playStatusIcon = -1;

        if ( iPlaybackState == EPbStatePlaying )
            {
            playStatusIcon = KMPXMainViewPlayIcon;
            }
        else if ( iPlaybackState == EPbStatePaused )
            {
            playStatusIcon = KMPXMainViewPauseIcon;
            }

        TInt duration = iPosition; //lint !e961
        if ( duration > 0 ) //lint !e961
            {
            // convert milliseconds to seconds
            duration = duration / KSecondInMilliseconds;
            }
        CMPXCommonUiHelper::TMPXDuratDisplayMode durationMode =
            CMPXCommonUiHelper::EMPXDuratAuto;
        if ( duration > KOneHourInSeconds )
            {
            durationMode = CMPXCommonUiHelper::EMPXDuratHMS;
            }
        HBufC* stringBuf = iCommonUiHelper->DisplayableDurationL(
            (TInt64)duration, durationMode );
        CleanupStack::PushL( stringBuf );

        // Only remove the old one if we were previously showing the
        // playback indicator
        if ( array->Count() > 0 && oldState)
            {
            array->Delete( 0 );
            }
        HBufC* dataItem;
        if ( iTitle )
            {
            dataItem = HBufC::NewLC( iTitle->Length() + KMPXExtraBufferLength );
            }
        else
            {
            dataItem = HBufC::NewLC( KMPXDefaultBufferLength +
                KMPXExtraBufferLength );
            }
        if ( playStatusIcon > -1 )
            {
            dataItem->Des().AppendNum( (TInt64)iAlbumArtIndex );
            dataItem->Des().Append( KMPXTab );
            dataItem->Des().Append( *iTitle );
            dataItem->Des().Append( KMPXTab );
            dataItem->Des().Append( *stringBuf );
            dataItem->Des().Append( KMPXSpace );
            dataItem->Des().Append( KMPXSpace );

            CArrayFix< TInt >* intArray =
                new ( ELeave ) CArrayFixFlat<TInt>( 2 );  // magic number
            CleanupStack::PushL( intArray );
            intArray->AppendL( iCurPlPlayPosition );
            intArray->AppendL( iCurPlTotalCount );
            HBufC* posString = StringLoader::LoadLC(
                R_MPX_TRACK_POSITION, *intArray );
            dataItem->Des().Append( *posString );
            CleanupStack::PopAndDestroy( posString );
            CleanupStack::PopAndDestroy( intArray );
            dataItem->Des().Append( KMPXTab );

            dataItem->Des().Append( KMPXTab );
            dataItem->Des().AppendNum( (TInt64)playStatusIcon );
            }
        else
            {
            dataItem->Des().AppendNum( (TInt64)iAlbumArtIndex );
            dataItem->Des().Append( KMPXTab );
            dataItem->Des().Append( *iTitle );
            dataItem->Des().Append( KMPXTab );
            dataItem->Des().Append( KMPXTab );
            dataItem->Des().Append( KMPXTab );
            }
        array->InsertL( 0, dataItem->Des() );
        iContainer->ListBox()->HandleItemAdditionL();

        CleanupStack::PopAndDestroy( dataItem );
        CleanupStack::PopAndDestroy( stringBuf );
        }
    }

// ---------------------------------------------------------------------------
// Start a refreshing note
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::StartWaitNoteL( TWaitNoteType aNoteType )
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
// Close waitnote dialog
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::CloseWaitNoteL(TBool aSkipCheckIfActive)
    {
    TUid waitnoteId = TUid::Uid( KMPXPluginTypeWaitNoteDialogUid );
    TUid activeView = iViewUtility->ActiveViewType();
    if(( activeView == waitnoteId ) || (aSkipCheckIfActive))
        {
        CMPXViewPlugin* pi =
            iViewUtility->ViewPluginManager().PluginL( TUid::Uid(KWaitNoteImpUid) );
        pi->DeactivateView();
        }
    }

// ---------------------------------------------------------------------------
// Any processing to handle broadcast events
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoHandleBroadcastMsgL( TInt aEvent )
    {
    MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg<--");
    switch( aEvent )
        {
        case EMcMsgFormatStart:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgFormatStart");

            // If we were in MTP sync, we stay in mtp sync mode
            if( iCurSystemEvent == KErrNotFound )
                {
                // Show a formatting wait note
                iCurSystemEvent = EMcMsgFormatStart;
                iDiskDismountDuringFormat = EFalse;
                iPlaybackUtility->CommandL( EPbCmdStop );

                StartWaitNoteL( EMPXFormatScanningNote );
                }
            break;
            }
        case EMcMsgFormatEnd:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgFormatEnd");
            if( iCurSystemEvent == EMcMsgFormatStart )
                {
                iCurSystemEvent = KErrNotFound;

                if( !iDiskDismountDuringFormat )
                    {
                    MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - Sync db after format");
                    ResynchronizeCollectionL();

                    if( !iRefreshingCollection )
                        {
                        StartWaitNoteL( EMPXScanningNote );
                        iRefreshingCollection = ETrue;
                        }
                    }
                }
            break;
            }
        case EMcMsgDiskRemoved:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgDiskRemoved");
            iPlaybackUtility->CommandL( EPbCmdStop );

            // Main view hasn't been activated
            // Updating the view now causes a blank screen if usb is started
            //
            if( iColInitialized )
                {
                UpdatePluginInfo();
                }

            if( iCurSystemEvent == EMcMsgFormatStart )
                {
                iDiskDismountDuringFormat = ETrue;
                }
            break;
            }
        case EMcMsgDiskInserted:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgDiskInserted");
            // Only show the query if we are not processing a usb event
            // USB dismounts and re-mounts the drive several times
            //
            // Ignore the disk insert during format, after format a fake event will be sent.
            //
            if( iCurSystemEvent != EMcMsgUSBMassStorageStart &&
                iCurSystemEvent != EMcMsgUSBMTPStart &&
                iCurSystemEvent != EMcMsgFormatStart &&
                iColInitialized )
                {
                if( iQueryDialog )
                    {
                    MPX_DEBUG1("Main View -- Dismissing Query");
                    iQueryDialog->DismissQueryL();
                    }

                // Always start a refresh
                //
                ResynchronizeCollectionL();
                if( !iRefreshingCollection )
                    {
                    StartWaitNoteL( EMPXScanningNote );
                    iRefreshingCollection = ETrue;
                    }
                }
            break;
            }
        case EMcMsgUSBMassStorageEnd:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgUSBMassStorageEnd");
            // Show query dialog to ask if they want to refresh
            //
            MPX_DEBUG2( "CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgUSBMassStorageEnd iColInitialized = %d", iColInitialized );
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
            if( iColInitialized || iDelayPluginInfoUpdate )
                {
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU
                CloseWaitNoteL(ETrue);
                iDelayPluginInfoUpdate = EFalse;
                if( iIdle->IsActive() )
                    {
                    iIdle->Cancel();
                    }
                TCallBack cb( &IdleCallback, this );
                iIdle->Start( cb );
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
                }
            else
                {
                iDelayedUsbRefresh = ETrue;
                iCurSystemEvent = KErrNotFound;
                }
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU
            break;
            }
        case EMcMsgUSBMassStorageStart:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgUSBMassStorageStart");
            // Close playback framework and start wait note
            if( iQueryDialog )
                {
                MPX_DEBUG1("Main View -- Dismissing Query");
                iQueryDialog->DismissQueryL();
                }
            iCurSystemEvent = EMcMsgUSBMassStorageStart;
            iPlaybackUtility->CommandL( EPbCmdStop );
            StartWaitNoteL( EMPXUsbEventNote );
            break;
            }
        case EMcMsgUSBMTPStart:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgUSBMTPStart");
            // Close playback framework and start wait note
            if( iQueryDialog )
                {
                MPX_DEBUG1("Main View -- Dismissing Query");
                iQueryDialog->DismissQueryL();
                }
            iCurSystemEvent = EMcMsgUSBMTPStart;
            if( !iColInitialized )
                {
                iColInitialized = ETrue;
                }
            iPlaybackUtility->CommandL( EPbCmdStop );
            StartWaitNoteL( EMPXMTPEventNote );
            break;
            }
        case EMcMsgUSBMTPEnd:
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg EMcMsgUSBMTPEnd");
            iCurSystemEvent = KErrNotFound;
            CloseWaitNoteL(ETrue);
            if( iColInitialized )
                {
                ResynchronizeCollectionL();
                UpdatePluginInfo();
                }
            break;
            }
        case EMcMsgRefreshEnd: // fall through
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgFormatEnd, EMcMsgUSBMTPEnd,EMcMsgRefreshEnd");
            iCurSystemEvent = KErrNotFound;

            if( iColInitialized )
                {
                ResynchronizeCollectionL();
                UpdatePluginInfo();
                }
            break;
            }
        case EMcMsgSystemEventMax:
        case EMcMsgRefreshStart:    // fall through
            {
            MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg - EMcMsgSystemEventMax, EMcMsgRefreshStart");
            break;
            }
        default:
            {
            break;
            }
        }
     MPX_DEBUG1("CMPXMainViewImp::DoHandleBroadcastMsg -->");
    }

// ---------------------------------------------------------------------------
// Handle a corrupt message
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoHandleCorruptMsgL()
    {
    // Start the corrupt note. The corrupt note will delete
    // the old databases and restart everything from SCRATCH
    //
    MPX_DEBUG1("CMPXMainViewImp::DoHandleCorruptMsgL <-- starting scan");

    StartWaitNoteL( EMPXCorruptScanningNote );
    iInitDBNeeded = EFalse;
    iInitDBCorrupted = EFalse;
    }

// ---------------------------------------------------------------------------
// Handle a corrupt message
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::UpdatePluginInfo()
    {
    MPX_DEBUG1("CMPXMainViewImp::UpdatePluginInfo");

    // MediaL on the first selection and only if we are
    // not currently handling a system event
    //
    if( iCurSystemEvent == KErrNotFound )
        {
        TRAP_IGNORE( DoRetrieveDetailsL() );
        }
    else if ( !iColInitialized )
        {
        MPX_DEBUG1( "CMPXMainViewImp::UpdatePluginInfo setting iDelayPluginInfoUpdate" );
        iDelayPluginInfoUpdate = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// Resynchronize collection(s)
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::ResynchronizeCollectionL()
    {
    MPX_FUNC( "CMPXMainViewImp::ResynchronizeCollectionL" );

    RArray<TUid> ary;
    CleanupClosePushL( ary );

    // Resynchronize podcast databases
    ary.AppendL( TUid::Uid(EMPXCollectionPluginPodCast) );
    TUid collection = iCollectionUtility->CollectionIDL( ary.Array() );
    TRAP_IGNORE(
        iCollectionUtility->
            Collection().CommandL( EMcCmdCollectionInit,
                                   collection.iUid ) );

    // Resynchronize music collection databases
    ary.Reset();
    ary.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
    collection = iCollectionUtility->CollectionIDL( ary.Array() );
    iCollectionUtility->Collection().CommandL( EMcCmdCollectionInit,
                                               collection.iUid );

    CleanupStack::PopAndDestroy( &ary );
    }

// ---------------------------------------------------------------------------
// Handle an idle callback event
// ---------------------------------------------------------------------------
//
TInt CMPXMainViewImp::IdleCallback( TAny* ptr )
    {
    TRAP_IGNORE( ( (CMPXMainViewImp*) ptr )->HandleIdleEventL() );
    return 0;
    }

// ---------------------------------------------------------------------------
// Handle an idle callback event
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::HandleIdleEventL()
    {
    MPX_FUNC( "CMPXMainViewImp::HandleIdleEventL" );
    TInt event = iDelayedUsbRefresh ? EMcMsgUSBMassStorageStart : iCurSystemEvent;
    switch( event )
        {
        case EMcMsgUSBMassStorageStart:
            {
            MPX_DEBUG1("CMPXMainViewImp::HandleIdleEventL -- Mass Storage dialog");
            iCurSystemEvent = KErrNotFound;
            iDelayedUsbRefresh = EFalse;

            // Send resyn command
            ResynchronizeCollectionL();

            HBufC* title = StringLoader::LoadLC( R_MPX_REFRESH_AFTER_SYNC );
            CAknQueryDialog* query = new( ELeave ) CAknQueryDialog();
            iQueryDialog = query;

            query->PublishDialogL(
                EMPlayerNoteUSBSyncRefresh,
                KMPlayerNoteCategory );

            TInt rtn = query->ExecuteLD( R_MPX_QUERY_YES_NO ,
                                         *title );
            CleanupStack::PopAndDestroy( title );
            iQueryDialog = NULL;  // Dialog destroyed

            if ( rtn == EAknSoftkeyYes )
                {
                StartWaitNoteL( EMPXRefreshingNote );
                }
            else
                {
                MPX_DEBUG1("CMPXMainViewImp::HandleIdleEventL -- refreshing view");
                UpdatePluginInfo();

                // Refresh the UI view in this case
                // @todo, refactor all this handling to its own class
                //
                MPX_DEBUG1("CMPXMainViewImp::HandleIdleEventL() no refresh selected");
                if ( !CEikonEnv::Static()->StartedAsServerApp() )
                    {
                    CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
                    CleanupStack::PushL( cPath );
                    if( cPath->Levels() > 1 &&
                        iViewUtility->ActiveViewType() == TUid::Uid(KMPXPluginTypeCollectionUid) )
                        {
                        MPX_DEBUG1("CMPXMainViewImp::HandleIdleEventL() Refreshing UI");
                        cPath->Back();
                        iCollectionUtility->Collection().OpenL( *cPath );
                        }
                    CleanupStack::PopAndDestroy( cPath );
                    }
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
// Calls MediaL to retrieve item details
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoRetrieveDetailsL(TBool aSkipAttribute)
    {
    MPX_DEBUG2("CMPXMainViewImp::DoRetrieveDetailsL %i", iCurPlugin);
    iDelayPluginInfoUpdate = EFalse;
    // Make sure we don't overlap 2 sets of MediaL()
    //
    if( iCurPlugin == KErrNotFound && iCurSystemEvent == KErrNotFound )
        {
        delete iCurPath;
        iCurPath = NULL;

#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
        iCurPath = iCollectionUtility->Collection().PathL();
#else
        // TEMPORARY FIX, hardcode root level collections
        // This needs to be re-written as a seperate class!
        //
        RArray<TMPXItemId> ids;
        CleanupClosePushL( ids );

        // Root level path
        iCurPath = CMPXCollectionPath::NewL();
        ids.AppendL( TMPXItemId(0x101FFC3A) );
        ids.AppendL( TMPXItemId(0x101FFC3C) );
        iCurPath->AppendL( ids.Array() );

        CleanupStack::PopAndDestroy( &ids );
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU

        if( iCurPath->Levels() == 1 && iCurPath->Count() )
            {
            iCurPath->SetToFirst();
            iCurPlugin = 0;

            // Do a MediaL to re-retrieve details
            //
            RArray<TMPXAttribute> atts;
            CleanupClosePushL( atts );
            if(!aSkipAttribute)
                {
                atts.Append(KMPXMediaColDetailDBCreated);
                atts.Append(KMPXMediaColDetailDBCorrupted);
                }
#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
            atts.Append(KMPXMediaGeneralSubTitle);
            if(!aSkipAttribute)
                {
                atts.Append(KMPXMediaGeneralCount);
                }
            atts.Append(KMPXMediaGeneralTitle);
            atts.Append(KMPXMediaGeneralIcon );
#endif // __ENABLE_PODCAST_IN_MUSIC_MENU
            iCollectionUtility->Collection().MediaL(*iCurPath, atts.Array() );
            CleanupStack::PopAndDestroy( &atts );
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXAlbumArtUtilObserver
// Album art is extracted from file and ready to use.
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoExtractAlbumArtCompletedL(
    CFbsBitmap* aBitmap, TInt aErr )
    {
    MPX_DEBUG3( "CMPXMainViewImp::DoExtractAlbumArtCompleted(bitmap:0x%x, err:%d)",
        aBitmap, aErr );

    if ( !aErr )
        {
        if ( iContainer )
            {
            CGulIcon* icon = CGulIcon::NewL( aBitmap );
            CArrayPtr<CGulIcon>* icons = static_cast<CEikFormattedCellListBox*>(
                iContainer->ListBox() )->ItemDrawer()->
                FormattedCellData()->IconArray();
            icons->AppendL( icon );
            iAlbumArtIndex = icons->Count() - 1;

            MPX_DEBUG2( "CMPXMainViewImp::ExtractAlbumArtCompleted Array icon index = %d", iAlbumArtIndex );
            TRAP_IGNORE(UpdatePlaybackViewStatusL());
            }
        else
            {
            delete aBitmap;
            }
        }
    else
        {
        iAlbumArtIndex = KMPXMainViewDefaultIcon;
        }
    }

// ---------------------------------------------------------------------------
// Sets the default focus
// ---------------------------------------------------------------------------
//
void CMPXMainViewImp::DoSetDefaultFocusL()
    {
    if( iContainer )
        {
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );

        // Select item 0
        //
        cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId,
                                     KMPXCommandIdCollectionSelect);
        cmd->SetTObjectValueL<TInt>( KMPXCommandCollectionSelectIndex, 0 );
        cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

        iCollectionUtility->Collection().CommandL( *cmd );
        CleanupStack::PopAndDestroy( cmd );
        }
    }

//  End of File
