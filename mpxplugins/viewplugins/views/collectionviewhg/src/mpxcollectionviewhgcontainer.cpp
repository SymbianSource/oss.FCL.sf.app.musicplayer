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
* Description:  Implementation of Collection view container
*
*/


// INCLUDE FILES
#include <e32cmn.h>
#include <e32math.h>
#include <s32file.h>
#include <gulicon.h>
#include <hlplch.h>
#include <StringLoader.h>
#include <aknview.h>
#include <aknappui.h>
#include <AknIconArray.h>
#include <aknlists.h>
#include <aknPopup.h>
#include <akncontext.h>
#include <AknsBasicBackgroundControlContext.h>
#include <data_caging_path_literals.hrh>

#include <mpxlog.h>
#include <mpxuser.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxconstants.h>
#include <mpxmediageneraldefs.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxpodcastdefs.h>
#include <mpxcommonuihelper.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <mpxplaybackutility.h>
#include <mpxcollectionviewhg.rsg>
#include <mpxcollectionviewhg.mbg>
#include <mpxappui.hrh>
#include <mpxmusicplayerviewplugin.hrh>

#include <ganes/HgDoubleGraphicListFlat.h>
#include <ganes/HgSingleGraphicList.h>
#include <ganes/HgSingleTextListWithIcon.h>
#include <ganes/HgVgItem.h>
#include <ganes/HgItem.h>
#include <ganes/HgScrollerWithTitle.h>
#include <ganes/HgVgMediaWall.h>

#include <layoutmetadata.cdl.h>
#include <AknLayout2ScalableDef.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <akntranseffect.h>                 // For transition effects
#include <gfxtranseffect/gfxtranseffect.h>  // For transition effects

#include "mpxcommoncontainer.hrh"
#include "mpxcollectionviewhg.hrh"
#include "mpxcollectionviewhglistboxarray.h"
#include "mpxcollectionviewhg.hlp.hrh"
#include "mpxcollectionviewhgcontainer.h"
#include "mpxcollectionviewhgplaylisthelper.h"
#include "mpxcollectionviewhgtnloader.h"
#include "mpxcbahandler.h"
#include "mpxselectedalbumhandler.h"
#include "mpxpopuplist.h"
#include "mpxcollectionviewhgswitchbuffer.h"

// CONSTANTS
_LIT( KMPXCollectionViewRscPath, "mpxcollectionviewhg.rsc" );
_LIT( KMPXCollectionViewIconFile, "mpxcollectionviewhg.mbm" );
const TInt KMPXListSelectionCount = 5;
const TInt KMPXMinSecSeparatorIndex = 2;
const TInt KOneHourInSeconds = 3600;
const TInt KMPXTimeIndicatorLength = 16;
#ifdef __WINS__
const TInt KMPXListBufferSize = 20;
#else
const TInt KMPXListBufferSize = 400;
#endif
// Added by Harri. Buffer size has to be smaller with mediawall, otherwise IVE3 runs out of memory
#ifdef __WINS__
const TInt KMPXListBufferSizeWithMediaWall = 20;
#else
const TInt KMPXListBufferSizeWithMediaWall = 40;
#endif
const TInt KMPXMaxFileLength = 256;
const TReal KIconFactor = 0.7;

const TInt KThumbLoadingPauseForTransition(500000);

_LIT( KMPXDash, " - " );
#ifdef HG_MP_LOC_AVAILABLE
_LIT( KMPXSpace, " ");
#endif //HG_MP_LOC_AVAILABLE
_LIT( KMPXZeroDurationMark, "--");

_LIT( KSong, " song" );
_LIT( KSongs, " songs" );
_LIT( KEpisode, " episode" );
_LIT( KEpisodes, " episodes" );
_LIT( KAlbum, " album" );
_LIT( KAlbums, " albums" );
_LIT( KSongDash, " song - " );
_LIT( KSongsDash, " songs - " );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewHgContainer* CMPXCollectionViewHgContainer::NewL(
    MEikCommandObserver* aView,
    MEikListBoxObserver* aObserver,
    MMPXSelectedAlbumHandler* aSelectedAlbumHandler,
    TBool aIsEmbedded)
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::NewL" );
    CMPXCollectionViewHgContainer* self =
        new ( ELeave ) CMPXCollectionViewHgContainer(
                aView, aObserver, aSelectedAlbumHandler, aIsEmbedded);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewHgContainer::CMPXCollectionViewHgContainer(
    MEikCommandObserver* aView,
    MEikListBoxObserver* aObserver,
    MMPXSelectedAlbumHandler* aSelectedAlbumHandler,
    TBool aIsEmbedded) :
	iView( aView ),
	iListBoxObserver( aObserver ),
	iReady( EFalse ),
	iIsEmbedded(aIsEmbedded),
	iCurrentDefaultIcon( EMPXDefaultIconNotSet ),
	iContext( EContextUnknown ),
	iTopIndex( 0 ),
	iBottomIndex( KErrNotFound ),
	iAlbumIndex(KErrNotFound),
	iSelectedAlbumIndex(KErrNotFound),
	iRestoredAlbumIndex(KErrNotFound),
	iSelectedAlbumHandler (aSelectedAlbumHandler),
	iOpenAlbumTracks(EFalse),
	iShuffleItem(0),
	iSetEmptyTextNeeded(EFalse),
	iPopupListRect(TRect(0,0,0,0)),
	iPreviousViewId(TUid::Uid(0))
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCollectionViewHgContainer::~CMPXCollectionViewHgContainer()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::~CMPXCollectionViewHgContainer" );
    delete iSongTitle;
    delete iAlbumTitle;

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }
    delete iListSelections;
    delete iListBoxArray;
    delete iIconArray;
    delete iMediaWall;
    delete iMwListWidget;
    delete iListWidget;
    delete iOrigIcon;
    delete iCommonUiHelper;
    CancelTNRequest();
    iThumbnailReqMap.Close();
    delete iThumbnailManager;
    delete iBgContext;
    delete iSelectedMediaInAlbumView;
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    if ( iCollectionUiHelper)
         {
         iCollectionUiHelper->Close();
         }

    if ( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }

    delete iPlaylist;
    delete iPlaylistHelper;

    if( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        }
    delete iAsyncCallBack;

    iCoeEnv->RemoveForegroundObserver( *this );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ConstructL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ConstructL" );

    CCoeEnv* coeEnv( iEikonEnv );
    TParse parse;
    parse.Set( KMPXCollectionViewRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    iAlbumTitle = StringLoader::LoadL( R_MPX_COLLECTION_DETAILS_ONE_ALBUM );
    iSongTitle = StringLoader::LoadL( R_MPX_COLLECTION_DETAILS_ONE_SONG );

    if ( !iCollectionUiHelper )
        {
        iCollectionUiHelper = CMPXCollectionHelperFactory:: NewCollectionUiHelperL();
        }
    iCollectionUtility = MMPXCollectionUtility::NewL( NULL, KMcModeDefault );
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );

    iPlaylistHelper = CMPXCollectionViewHgPlaylistHelper::NewL(iPlaybackUtility, iCollectionUiHelper);

    TRect mainPane( 0,0,0,0 );
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgScreen, mainPane, ETrue);

    iBgContext->SetRect(((CAknAppUi*)iCoeEnv->AppUi())->ApplicationRect());

    // Common methods used for 2-line list support
    iCommonUiHelper = CMPXCommonUiHelper::NewL();
    iListSelections = new (ELeave) CArrayFixFlat<TInt>( KMPXListSelectionCount );

    iThumbnailManager = CMpxTNLoader::NewL( *this, EAudioListThumbnailSize );

    iImageSize = CHgDoubleGraphicListFlat::PreferredImageSize();
    iListBoxArray = static_cast<CMPXCollectionViewListBoxArray*>(
        CreateListBoxArrayL() );

    CreateIconArrayL();

    iIsForeground = ETrue;
    iCoeEnv->AddForegroundObserverL( *this );

    TCallBack callback(CMPXCollectionViewHgContainer::AsyncCallback, this);
    iAsyncCallBack = new (ELeave) CAsyncCallBack( CActive::EPriorityStandard );
    iAsyncCallBack->Set(callback);

    GfxTransEffect::Enable();

    CreateWindowL();
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CMPXCollectionViewHgContainer::MopSupplyObject( TTypeUid aId )
	{
    MPX_FUNC( "CMPXCollectionViewHgContainer::MopSupplyObject" );
    if( aId.iUid == MAknsControlContext::ETypeId && iBgContext )
        {
        return MAknsControlContext::SupplyMopObject(aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject(aId);
	}

// ---------------------------------------------------------------------------
// Sets playback status
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetPlaybackStatusByIdL(
    TMPXItemId aId, TMPXPlaybackState aStatus, TInt aIndex)
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetPlaybackStatusByIdL" );
    if ( iListBoxArray )
        {
        TInt index( iListBoxArray->SetPlaybackStatusByIdL( aId, aStatus, aIndex ) );
        if ( index != KErrNotFound )
            {
            // Update the previous item as not playing anymore
            DrawLbxItemL( index );
            }
        index = iListBoxArray->GetPlaybackIndex();
        if ( index != KErrNotFound )
            {
            // Update new item as playing
            DrawLbxItemL( index );
            }
        }
    }

// ---------------------------------------------------------------------------
// Get playback index
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::PlaybackIndex()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::PlaybackIndex" );
    return ( iListBoxArray ? iListBoxArray->GetPlaybackIndex() : KErrNotFound );
    }

// ---------------------------------------------------------------------------
// Activate reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ActivateReorderMode( TBool aEnable )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ActivateReorderMode" );
    if ( iListBoxArray )
        {
        iReorder = aEnable;
        iListBoxArray->ActivateReorderMode( iReorder );
        }
    }

// ---------------------------------------------------------------------------
// Set reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetReorderGrabbedMode(
    TBool aEnable, TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetReorderGrabbedMode" );
    if ( iListBoxArray )
        {
        iListBoxArray->SetReorderGrabbedMode( aEnable, aIndex );
        }
    }

// ---------------------------------------------------------------------------
// Determines if it's currently in reorder mode
// ---------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgContainer::IsInReorderMode()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::IsInReorderMode" );
    return iReorder;
    }

// ---------------------------------------------------------------------------
// Move current reorder index
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::MoveReorderIndex( TInt aDirection )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::MoveReorderIndex" );
    if ( iListBoxArray )
        {
        iListBoxArray->MoveReorderIndex( aDirection );
        }
    }

// ---------------------------------------------------------------------------
// set current reorder index
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetReorderIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetReorderIndex" );
    if ( iListBoxArray )
        {
        iListBoxArray->SetReorderIndex( aIndex );
        }
    }

// ---------------------------------------------------------------------------
// Confirm reorder
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ConfirmReorderL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ConfirmReorderL" );
    if ( iListBoxArray )
        {
        iListBoxArray->ConfirmReorderL();
        }
    }

// ---------------------------------------------------------------------------
// Cancel reorder
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::CancelReorder()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CancelReorder" );
    TInt originalIndex( GetOriginalIndex() );
    if ( iListBoxArray )
        {
        iListBoxArray->CancelReorder();
        }
    SetLbxCurrentItemIndexAndDraw( originalIndex );
    }

// ---------------------------------------------------------------------------
// Gets original index of currently selected item
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::GetOriginalIndex()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::GetOriginalIndex" );
    TInt index( KErrNotFound );
    if ( iListBoxArray )
        {
        index = iListBoxArray->GetOriginalIndex();
        }
    return index;
    }

// -----------------------------------------------------------------------------
// Update Titlepane icon to reorder image
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::UpdateReorderTitleIconL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::UpdateReorderTitleIconL" );
    // For status changes
    CEikStatusPane* sp = iAvkonAppUi->StatusPane();
    TParse parse;
    parse.Set( KMPXCollectionViewIconFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );

    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );

    AknsUtils::CreateIconLC(
        skin,
        KAknsIIDNone,
        bitmap,
        mask,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_ctx_reordersong,
        EMbmMpxcollectionviewhgQgn_graf_mup_ctx_reordersong_mask );

    CEikImage* newIcon = new ( ELeave ) CEikImage();
    CleanupStack::PushL( newIcon );
    newIcon->SetPicture( bitmap, mask );
    newIcon->SetPictureOwnedExternally( EFalse );
    iContextPane = static_cast<CAknContextPane*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidContext ) ) );
    if ( iOrigIcon )
        {
        delete iOrigIcon;
        }
    iOrigIcon = iContextPane->SwapPicture( newIcon );
    CleanupStack::Pop( 3 ); // bitmap, mask, newIcon
    }

// -----------------------------------------------------------------------------
// Update Titlepane icon to use original image
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::RestoreOriginalTitleIconL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::RestoreOriginalTitleIconL" );
    if ( iOrigIcon )
        {
        CEikImage* newIcon = iContextPane->SwapPicture( iOrigIcon );
        delete newIcon;
        iOrigIcon = NULL;
        }
    }

// -----------------------------------------------------------------------------
// Creates listbox array
// -----------------------------------------------------------------------------
//
CMPXCommonListBoxArrayBase* CMPXCollectionViewHgContainer::CreateListBoxArrayL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CreateListBoxArrayL" );
    CMPXCollectionViewListBoxArray* array = CMPXCollectionViewListBoxArray::NewL();
    array->ConstructListBoxArrayL();
    array->SetObserver( this );
    return array;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCollectionViewHgContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXCollectionViewHgContainer::HandleKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    if ( aKeyEvent.iCode == EKeyUpArrow ||
        aKeyEvent.iCode == EKeyPrevious )
        {
        iView->ProcessCommandL( EMPXCmdUpArrow );
        }
    else if ( aKeyEvent.iCode == EKeyDownArrow ||
            aKeyEvent.iCode == EKeyNext )
        {
        iView->ProcessCommandL( EMPXCmdDownArrow );
        }
    TKeyResponse response = EKeyWasNotConsumed;

    if( iReady )
        {
        const CMPXMedia& containerMedia(iListBoxArray->ContainerMedia() );
        TMPXGeneralType containerType(
            containerMedia.ValueTObjectL<TMPXGeneralType>(
                KMPXMediaGeneralType ) );
        TMPXGeneralCategory containerCategory(
            containerMedia.ValueTObjectL<TMPXGeneralCategory>(
                KMPXMediaGeneralCategory ) );

        // Hashkey mark/unmark works in all song's view and other tracks view,
        // not on other category views
        if( containerCategory == EMPXSong ||
            (( containerCategory == EMPXPlaylist ||
            containerCategory == EMPXAlbum  ||
            containerCategory == EMPXGenre ||
            containerCategory == EMPXComposer ) &&
            containerType == EMPXItem ) ||
            aKeyEvent.iScanCode != EStdKeyHash )
            {
            response = HandleKeyEventL( aKeyEvent, aType );
            }
        }
    else
        {
        response = HandleKeyEventL( aKeyEvent, aType );
        MPX_DEBUG2( "CMPXCollectionViewHgContainer::OfferKeyEventL commoncontainer offerkey res = %d", response);
        }

    if ( aKeyEvent.iCode == EKeyUpArrow ||
        aKeyEvent.iCode == EKeyPrevious )
        {
        iView->ProcessCommandL( EMPXCmdUpArrowAfterListboxHandled );
        }
    else if ( aKeyEvent.iCode == EKeyDownArrow ||
            aKeyEvent.iCode == EKeyNext )
        {
        iView->ProcessCommandL( EMPXCmdDownArrowAfterListboxHandled );
        }
    return response;
    }


// ---------------------------------------------------------------------------
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCollectionViewHgContainer::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG5( "CMPXCollectionViewHgContainer::HandleKeyEventL(iCode=%d, iScanCode=%d, iModifiers=0x%x, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iModifiers, aType );

    TKeyResponse response( EKeyWasNotConsumed );
    if ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter )
        {
            SaveSelectedAlbumItemL(iAlbumIndex);
        // Handle enter key pressed
        iView->ProcessCommandL( EMPXCmdCommonEnterKey );
        }
    if ( iListWidget )
        {
        iListWidget->OfferKeyEventL( aKeyEvent, aType );
        }
    else if( iMediaWall )
        {
        iMediaWall->OfferKeyEventL( aKeyEvent, aType );
        }
    if ( response == EKeyWasNotConsumed &&
         aType == EEventKey &&
         aKeyEvent.iCode == EKeyBackspace && iView )
        {
        iView->ProcessCommandL( EMPXCmdCommonDelete );
        response = EKeyWasConsumed;
        }

    return response;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleHelpContext" );
    aContext.iMajor = KAppUidMusicPlayerX;

    const CMPXMedia& containerMedia(iListBoxArray->ContainerMedia() );
    TMPXGeneralType containerType(
        containerMedia.ValueTObjectL<TMPXGeneralType>(
            KMPXMediaGeneralType ) );
    TMPXGeneralCategory containerCategory(
        containerMedia.ValueTObjectL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory ) );
    MPX_DEBUG3( "CMPXCollectionViewHgContainer::HandleHelpContext containertype = %d, containercategory = %d", containerType, containerCategory );

    switch ( containerCategory )
        {
        case EMPXCollection:
            {
            // music menu view
            aContext.iContext = KMusHlpLibMainView;
            break;
            }
        case EMPXPlaylist:
            {
            if (iIsEmbedded)
                {
                aContext.iContext = KMusHlpEmbeddedPlaylistTracksView;
                }
            else
                {
                if ( containerType == EMPXGroup )
                    {
                    // playlist view
                    aContext.iContext = KMusHlpPlaylistView;
                    }
                else if ( containerType == EMPXItem )
                    {
                    // playlist tracks view
                    aContext.iContext = KMusHlpPlaylistTracksView;
                    }
                else
                    {
                    // default to music main view help context
                    aContext.iContext = KMusHlpLibMainView;
                    }
                }
            break;
            }
        case EMPXAlbum:
        case EMPXGenre:
        case EMPXComposer:
            {
            if ( containerType == EMPXGroup )
                {
                // album, genre, composer view
                aContext.iContext = KMusHlpCategoryView;
                }
            else if ( containerType == EMPXItem )
                {
                // album, genre, composer tracks view
                aContext.iContext = KMusHlpTracksView;
                }
            else
                {
                // default to music main view help context
                aContext.iContext = KMusHlpLibMainView;
                }
            break;
            }
        case EMPXArtist:
            {
            // artist view
            // artist/album view
            aContext.iContext = KMusHlpCategoryView;
            break;
            }
        case EMPXSong:
            {
            // all songs view
            aContext.iContext = KMusHlpTracksView;
            break;
            }
        default:
            {
            // default to music main view help context
            aContext.iContext = KMusHlpLibMainView;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleResourceChange( TInt aType )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleResourceChange" );
    CCoeControl::HandleResourceChange( aType );

    TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
    TRAP_IGNORE(
        if ( aType == KAknsMessageSkinChange )
            {
            delete iIconArray;
            iIconArray = NULL;
            CreateIconArrayL();

            if( iCurrentViewType == EMPXViewMediawall || iCurrentViewType == EMPXViewTBone )
                {
            	iMediaWall->SetDefaultIconL(NULL);
                iMediaWall->EnableScrollBufferL(*this, KMPXListBufferSize, KMPXListBufferSize/4);
                }
            else
                {
                CHgScroller* list = CurrentListWidget();
                if( list )
                    {
                    list->SetDefaultIconL(NULL);
                    list->EnableScrollBufferL(*this, KMPXListBufferSizeWithMediaWall, KMPXListBufferSizeWithMediaWall/4);
                    }
                }
			SetDetailIconShuffleL();
            }
        else if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            iLayoutSwitch = ETrue;

            if ( iCurrentViewType == EMPXViewMediawall )
                {
                if ( iDialog )
                    {
                    iDialog->CancelPopup();
                    }
                // make cba visible so that visible screen area is calculated correctly in list view
                if( iCbaHandler )
                    iCbaHandler->ChangeCbaVisibility( ETrue );
                }
            iSetEmptyTextNeeded = ETrue;


            TRect clientRect = ((CAknView*)iView)->ClientRect();
            SetRect( clientRect );
            iBgContext->SetRect(((CAknAppUi*)iCoeEnv->AppUi())->ApplicationRect());

            // call HandleLbxItemAdditionL
            HandleLbxItemAdditionL();
            }
        );
    if(iMediaWall)
        iMediaWall->HandleResourceChange( aType );
    if(iListWidget)
        iListWidget->HandleResourceChange( aType );
    if(iMwListWidget)
        iMwListWidget->HandleResourceChange( aType );
    }


// ---------------------------------------------------------------------------
// From CCoeControl
// Draw this application's view to the screen
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::Draw(const TRect& /*aRect*/) const
    {
    if ( iContext == EContextUnknown )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
        AknsDrawUtils::DrawBackground( skin, cc, this, SystemGc(),
                TPoint(0,0), Rect(), KAknsDrawParamDefault );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Provide control input capabilities
// ---------------------------------------------------------------------------
//
TCoeInputCapabilities CMPXCollectionViewHgContainer::InputCapabilities() const
	{
	switch( iCurrentViewType )
        {
        case EMPXViewMediawall:
        case EMPXViewTBone:
            {
            return iMediaWall->InputCapabilities();
            }
        case EMPXViewList:
            {
            return iListWidget->InputCapabilities();
            }
        default:
            break;
        }
	return CCoeControl::InputCapabilities();
	}

// ---------------------------------------------------------------------------
// From MCoeControlObserver
// Handle control event
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,
    TCoeEvent /*aEventType*/ )
    {
    }
// ---------------------------------------------------------------------------
// From MMPXViewContainer
// Returns the indicators for the specified item within the view container
// ---------------------------------------------------------------------------
//
RArray<TInt> CMPXCollectionViewHgContainer::IndicatorsL( TInt aIndex )
    {
    return iListBoxArray->IndicatorIconIndicesL( aIndex );
    }

// ---------------------------------------------------------------------------
// From MMPXCommonListBoxArrayObserver
// Handle listbox array events.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleListBoxArrayEventL(
    TMPXCommonListBoxArrayEvents aEvent )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleListBoxArrayEventL" );

    if ( aEvent == MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventIconArrayChange )
        {
        delete iIconArray;
        iIconArray = NULL;
        CreateIconArrayL();
        }
    else if ( aEvent == MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventMediaArrayChange )
        {

		// Update list widget with new items in the media array.
        // We start the update from last valid media item index.
		if ( iListWidget )
			{
			ProvideDataWithoutThumbnailsL(iListBoxArray->MediaArray(), iLastValidMediaItemIndex);


			if ( iLastValidMediaItemIndex >= iRequestStart && iLastValidMediaItemIndex >= iRequestEnd )
				{

				 Request(iRequestStart, iRequestEnd, EHgBufferReset );
				}
			iListWidget->RefreshScreen(iListWidget->FirstIndexOnScreen());
			}
        }
    }

// ---------------------------------------------------------------------------
// Content is ready.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ContentIsReady( TBool aReady )
    {
    iReady = aReady;
    }


void CMPXCollectionViewHgContainer::SetCbaHandler( MMpxCbaHandler* aCbaHandler )
    {
    iCbaHandler = aCbaHandler;
    }

// -----------------------------------------------------------------------------
// Creates icon array, populates iIconArray to be used by listbox
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::CreateIconArrayL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CreateIconArrayL" );
    if ( iListBoxArray )
        {
        iIconArray = iListBoxArray->CreateIconArrayL();
        }
    SetDefaultIconL();
    }

// -----------------------------------------------------------------------------
// Returns a pointer to the listboxarray
// -----------------------------------------------------------------------------
//
CMPXCommonListBoxArrayBase* CMPXCollectionViewHgContainer::ListBoxArray() const
    {
    return iListBoxArray;
    }

// -----------------------------------------------------------------------------
// Return number of listbox items shown in the current listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::CurrentListItemCount() const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CurrentListItemCount" );
    TInt count = 0;

    switch( iCurrentViewType )
        {
        case EMPXViewMediawall:
        case EMPXViewTBone:
            {
            count = iMediaWall->ItemCount();
            break;
            }
        case EMPXViewList:
            {
            count = iListWidget->ItemCount();
            break;
            }
        default:
            break;
        }

    return count - iShuffleItem;
    }


// -----------------------------------------------------------------------------
// Return total number of listbox items in the listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::TotalListItemCount() const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::TotalListItemCount" );
    TInt count = 0;
    if ( iListBoxArray )
        {
        count = iListBoxArray->MediaArray().Count();
        }
    return count;
    }


// -----------------------------------------------------------------------------
// Gets top listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::TopLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::TopLbxItemIndex" );
    return iTopIndex;
    }


// -----------------------------------------------------------------------------
// Gets bottom listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::BottomLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::BottomLbxItemIndex" );
    return iBottomIndex;
    }


// -----------------------------------------------------------------------------
// Gets current listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::CurrentLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CurrentLbxItemIndex" );
	TInt index( KErrNotFound );

    const TInt count = CurrentListItemCount();

    if ( count <= 0 )
        return index;

    switch( iCurrentViewType )
        {
        case EMPXViewMediawall:
        case EMPXViewTBone:
            {
            index = MediaIndex(iMediaWall->SelectedIndex());
            break;
            }
        case EMPXViewList:
            {
            index = MediaIndex(iListWidget->SelectedIndex());
            break;
            }
        default:
            break;
        }

    return index;
    }


// -----------------------------------------------------------------------------
// Get current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
const CArrayFix<TInt>* CMPXCollectionViewHgContainer::CurrentSelectionIndicesL() const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CurrentSelectionIndicesL" );

    // TODO, check if selection is available only with views that have only list.
    if ( iListWidget )
		{
		iListSelections->Reset();
		RArray<TInt> selectionArray;
        CleanupClosePushL( selectionArray );
		iListWidget->GetMarkedItemsL(selectionArray);
		int count = selectionArray.Count();
		for( int i = 0; i < count; i++ )
			{
			iListSelections->AppendL( selectionArray[i] );
			}
        CleanupStack::PopAndDestroy( &selectionArray );
		}
    return iListSelections;
    }


// -----------------------------------------------------------------------------
// Set current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetCurrentSelectionIndicesL(
    CArrayFix<TInt>* aIndices ) const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetCurrentSelectionIndicesL" );
    TInt count = aIndices->Count();
    if ( count > 0 && iListWidget )
        {
        for( TInt i = 0; i < count; i++ )
            {
            iListWidget->Mark( (*aIndices)[i] );
            }
        iListWidget->RefreshScreen( (*aIndices)[0] );
        }
    }


// -----------------------------------------------------------------------------
// Set top listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetLbxTopItemIndex( TInt /*aIndex*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetLbxTopItemIndex" );
    }


// -----------------------------------------------------------------------------
// Set current listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetLbxCurrentItemIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetLbxCurrentItemIndex" );

    // TODO, check if we need to support this with other view types also
    // David: This needs to be supported for Mediawall and Tbone
    if ( iListWidget && iCurrentViewType == EMPXViewList )
        {
        iListWidget->SetSelectedIndex( aIndex );
        }
    }


// -----------------------------------------------------------------------------
// Set current listbox item index and highlight it.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetLbxCurrentItemIndexAndDraw(
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetLbxCurrentItemIndexAndDraw" );
	SetLbxCurrentItemIndex(aIndex);
    }


// -----------------------------------------------------------------------------
// Clear listbox selection.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ClearLbxSelection()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ClearLbxSelection" );
    if ( iListWidget )
        {
        iListWidget->UnMarkAll();
        }
    }

// -----------------------------------------------------------------------------
// Sets the text for an empty list box
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetLbxEmptyTextL( const TDesC& aText )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetLbxEmptyTextL" );

    if ( iListWidget && aText != KNullDesC )
        {
        iListWidget->SetEmptyTextL( aText );
        }
    else if ( iMediaWall && aText != KNullDesC )
        {
        iMediaWall->SetEmptyTextL( aText );
        }
    }

// -----------------------------------------------------------------------------
// Draws a list box item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::DrawLbxItemL( TInt aIndex )
    {
    MPX_DEBUG2( "CMPXCollectionViewHgContainer::DrawLbxItem (aIndex=%d)", aIndex );

    CHgScroller* list = CurrentListWidget();

	TInt listIndex = aIndex + iShuffleItem;
    if( list && listIndex < list->ItemCount() )
        {
        CHgItem* item = &list->ItemL(listIndex);
        SetDetailIndicatorL(item, aIndex);
        list->RefreshScreen(listIndex);
        }
    }


// -----------------------------------------------------------------------------
// Set Find box's focus.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetFindBoxFocus( TBool /*aFocus*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetFindBoxFocus" );
    }


// -----------------------------------------------------------------------------
// Handle listbox item addition.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleLbxItemAdditionL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleLbxItemAdditionL" );
    SetCollectionContextL();

	// When the collection is reopened after coming from playback view,
	// it is reopened in Album-Tracks level (EContextItemAlbum).
	// We don't display its content in that context but inturn reopen the
	// collection at one level up (Albums) and display it in T-bone view.
	// T-Bone view is just a list of albums presented differently.
    if ( iContext == EContextItemAlbum )
		{
		iCollectionUtility->Collection().BackL();
		iOpenAlbumTracks = ETrue;
		return;
		}

    iLastValidMediaItemIndex = 0;

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    TInt count( mediaArray.Count() );
    MPX_DEBUG2( "CMPXCollectionViewHgContainer::HandleLbxItemAdditionL count=%d", count);

    HandleLbxItemRemovalL();
    TViewType prevViewType = iCurrentViewType;

    ResolveCurrentViewType( count );

    iThumbnailReqMap.Reset();
    if (ShuffleItemPresent())
        iShuffleItem = 1;
    else
    	iShuffleItem = 0;

    count = count + iShuffleItem;
    iThumbnailReqMap.ReserveL(count );
    for ( TInt i = 0; i < count ; i++ )
        {
        iThumbnailReqMap.Append( EFalse );
        }

    iThumbnailManager->CancelAll();

    if( prevViewType == iCurrentViewType && iContext == iPrevContext )
        {
        ResizeListL( mediaArray, count );
        }
    else
        {
        // MediaWall sets the default icon in the construction phase.
        iDefaultIconSet = EFalse;

        if( !iLayoutSwitch  )
            {
            // We only need to use this for one transition coming
            // from playback view.
            TBool pbv = (iPreviousViewId == TUid::Uid(KMPXPluginTypePlaybackUid)) ? ETrue : EFalse;
            if ( pbv )
                {
            	iPreviousViewId = TUid::Uid(0);
                }
            BeginFullScreenAnimation( pbv );
            }

        switch (iCurrentViewType)
            {
            case EMPXViewMediawall:
                {
                TMPXPlaybackState pbState( iPlaybackUtility->StateL() );
                if ( pbState == EPbStatePlaying || pbState == EPbStatePaused || pbState == EPbStateStopped )
                    {
                    RestoreSelectedAlbumItemL(mediaArray);
                    }
                PrepareMediaWallL(mediaArray, count);
                iOpenAlbumTracks = EFalse;
                break;
                }
            case EMPXViewTBone:
                {
                RestoreSelectedAlbumItemL(mediaArray);
                PrepareMediaWallWithListL( mediaArray, count );
                // We need to adjust the CBA for this view.
                if( iCbaHandler )
                    {
                    iCbaHandler->UpdateCba();
                    }
                break;
                }
            case EMPXViewList:
                {
                PrepareListL(mediaArray, count);
                iOpenAlbumTracks = EFalse;
                break;
                }
            default:
                User::Leave( KErrGeneral );
                break;
            }
        }

    DrawableWindow()->SetOrdinalPosition( -1 );

    CleanPrevView();
    iPrevViewType = iCurrentViewType;
    iPrevContext = iContext;
    if ( iSetEmptyTextNeeded )
        {
        LoadAndSetEmptyTextL();
        iSetEmptyTextNeeded = EFalse;
        }
    if( !iDefaultIconSet )
        {
        SetDefaultIconL();
        }

    iLayoutSwitch = EFalse;
    }

// -----------------------------------------------------------------------------
// Handle listbox item addition, preserving the current display index
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleLbxItemAdditionPreserveIndexL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleLbxItemAdditionPreserveIndexL" );
    HandleLbxItemAdditionL();
    }


// -----------------------------------------------------------------------------
// Handle listbox item removal.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleLbxItemRemovalL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleLbxItemRemovalL" );
    iTopIndex = 0;
    iBottomIndex = KErrNotFound;
    if ( iContext != iPrevContext )
        {
        CancelTNRequest();
        iThumbnailReqMap.Close();

	    }
    }


// -----------------------------------------------------------------------------
// Show/hide find box.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetFindBoxVisibilityL( TBool /*aIsVisible*/ )
    {
    }


// -----------------------------------------------------------------------------
// Determine find box visibility
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgContainer::FindBoxVisibility()
    {
    return EFalse;
    }


// -----------------------------------------------------------------------------
// Restore the PopupCBA of find box
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::RestoreFindBoxPopupCBA()
    {
    }


// -----------------------------------------------------------------------------
// Calculate the top index of the visible items
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::CalculateTopIndex( TInt /*aBottomIndex*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CalculateTopIndex" );
    return 0;
    }

// ---------------------------------------------------------------------------
// Enable/disable find box
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::EnableFindBox( TBool /*aEnable*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::EnableFindBox" );
    }


// ---------------------------------------------------------------------------
// Enable/disable marking
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::EnableMarking( TBool /*aEnable*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::EnableMarking" );
    }

// ---------------------------------------------------------------------------
// Custom handling of commands for markable lists.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleMarkableListProcessCommandL(
    TInt aCommand )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleMarkableListProcessCommandL" );

	if ( iListWidget )
		{
		switch ( aCommand )
			{
			case EAknCmdMark:
				iListWidget->Mark(iListWidget->SelectedIndex());
				break;
			case EAknCmdUnmark:
				iListWidget->UnMark(iListWidget->SelectedIndex());
			    break;
			case EAknMarkAll:
				iListWidget->MarkAll();
				break;
			case EAknUnmarkAll:
				iListWidget->UnMarkAll();
				break;
			}
        iListWidget->RefreshScreen(iListWidget->SelectedIndex());
		}
    }


// ---------------------------------------------------------------------------
// Custom handling of menu pane for markable lists
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleMarkableListDynInitMenuPane(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleMarkableListDynInitMenuPane" );

    if ( aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST )
        {
        TInt currentItem = CurrentLbxItemIndex();
        if ( currentItem == KErrNotFound )
            {
            aMenuPane->SetItemDimmed( EAknCmdEditListMenu, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdEditListMenu, EFalse );
            }
        }
    else if ( aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST_IMPLEMENTATION )
        {
        TRAP_IGNORE(CurrentSelectionIndicesL());
        TInt currentItem = CurrentLbxItemIndex();
        if ( currentItem != KErrNotFound )
            {
            TBool currentItemMarked = EFalse;
            for( int i = 0; i < iListSelections->Count(); i++ )
                {
                if ( currentItem == iListSelections->At(i) )
                    {
                    currentItemMarked = ETrue;
                    break;
                    }
                }

            TInt count = 0;
            if( iListWidget )
                {
                count = iListWidget->ItemCount();
                }
            else if ( iMediaWall )
                {
                count = iMediaWall->ItemCount();
                }
            aMenuPane->SetItemDimmed( EAknCmdMark, currentItemMarked );
            aMenuPane->SetItemDimmed( EAknCmdUnmark, !currentItemMarked );
            aMenuPane->SetItemDimmed( EAknMarkAll, iListSelections->Count() == count );
            aMenuPane->SetItemDimmed( EAknUnmarkAll, iListSelections->Count() == 0 );
            }
        }
    }




// ---------------------------------------------------------------------------
// Resize the list when contents of media array changed.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ResizeListL(const CMPXMediaArray& aMediaArray, TInt aCount)
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ResizeListL" );
    TInt mediaCount = aMediaArray.Count();

    if( iListWidget )
        {
        TRect clientRect = ((CAknView*)iView)->ClientRect();
        TInt mediaIndex = MediaIndex(iListWidget->SelectedIndex());
        mediaIndex = ( KErrNotFound == mediaIndex ) ? iListWidget->FirstIndexOnScreen() : mediaIndex;
        mediaIndex = ( mediaIndex >= 0 && (mediaIndex < (mediaCount)) ) ? mediaIndex : (mediaCount - 1);

        TInt prevItemCount = iListWidget->ItemCount();

        iListWidget->InitScreenL( clientRect );
        iListWidget->Reset();
        if ( aCount )
            {
            // enable scroll buffering now as it has not been enabled when empty list was constructed
            if ( !prevItemCount )
                {
                iListWidget->EnableScrollBufferL( *this, KMPXListBufferSize, KMPXListBufferSize/4 );
                }
            iListWidget->ResizeL( aCount );
            ProvideDataWithoutThumbnailsL(aMediaArray);
            iListWidget->SetSelectedIndex( mediaIndex + iShuffleItem );
            }
        else
            {
            iListWidget->RefreshScreen(0);
            }
        }
    else if( iMediaWall )
        {
        // In case of mediawall components we switch to different view type if orientation changes
        // so there is no need to set new client rect for mediawall.
        TInt mediaIndex = MediaIndex(iMediaWall->SelectedIndex());

        // Correct the array index if it is out of range
        // This case may happen when last album or album beside selected album is deleted
		// mediaIndex -1 is valid and it represents shuffle item

        if ( mediaIndex >= mediaCount || mediaIndex < -1)
            {
            mediaIndex = mediaCount - 1; // last item of the list
            }

        //in case of shuffle item is seleted (mediaIndex -1), there is no need to save it.
        if( (iSelectedAlbumIndex != mediaIndex) && ( mediaIndex != -1) )
            {
             iSelectedAlbumIndex = mediaIndex;
             SaveSelectedAlbumItemL( iSelectedAlbumIndex );
            }

        iMediaWall->Reset();
        if ( aCount )
            {
            iMediaWall->ResizeL( aCount );
            ProvideDataWithoutThumbnailsMwL(aMediaArray);

            iMediaWall->SetSelectedIndex( mediaIndex + iShuffleItem);
            if ( iCurrentViewType == EMPXViewTBone )
                {
                OpenAlbumL( mediaIndex );
                }
            }
        else
            {
            iMediaWall->RefreshScreen(0);
            }
        }
    }

// ----------------------------------------------------------------------------
// Prepare view for displaying data in list format.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::PrepareListL(const CMPXMediaArray& aMediaArray, TInt aCount)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::PrepareListL");

    if (!((CAknAppUi*)iCoeEnv->AppUi())->StatusPane()->IsVisible())
        ((CAknAppUi*)iCoeEnv->AppUi())->StatusPane()->MakeVisible(ETrue);

    if( iCbaHandler )
        iCbaHandler->UpdateCba();

    TRect clientRect = ((CAknView*)iView)->ClientRect();
    iThumbnailManager->SetSizeL( EAudioListThumbnailSize );
    iImageSize = CHgDoubleGraphicListFlat::PreferredImageSize();

    if( !iListWidget )
        {
        iListWidget = CHgDoubleGraphicListFlat::NewL (
            clientRect,
            aCount,
            NULL,
            NULL );
        iListWidget->SetMenuProviderL(this);
        iListWidget->SetSelectionObserver(*this);
        // TODO. check if this is correct for all lists
        iListWidget->ClearFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
        iListWidget->SetFocus(ETrue);
        iListWidget->SetScrollBarTypeL( CHgScroller::EHgScrollerLetterStripLite );
        iListWidget->DrawableWindow()->SetOrdinalPosition( -1 );
        iListWidget->DrawableWindow()->SetFaded(((CAknAppUi*)iCoeEnv->AppUi())->IsFaded(), RWindowTreeNode::EFadeIncludeChildren);
        ProvideDataWithoutThumbnailsL(aMediaArray);
        }
    else
        {
        iListWidget->Reset();
        iListWidget->ResizeL( aCount );
        iListWidget->SetFocus(ETrue);
        ProvideDataWithoutThumbnailsL(aMediaArray);
        iListWidget->MakeVisible(ETrue);
        iListWidget->InitScreenL(clientRect);
        }

    if ( (iAlbumIndex >= 0) && (iAlbumIndex < aCount) )
        {
        iListWidget->SetSelectedIndex( iAlbumIndex + iShuffleItem );
        }
    else if ( KErrNotFound == iAlbumIndex )
        {
        if ( iSelectedAlbumIndex >= 0 && iSelectedAlbumIndex < aCount)
            {
            iListWidget->SetSelectedIndex( iSelectedAlbumIndex + iShuffleItem );
            }
        else
            {
            iListWidget->SetSelectedIndex( iListWidget->FirstIndexOnScreen() + iShuffleItem );
            }
        }

    // TODO. Define here in which views we need to have buffering enabled in the list
    if( ( ( iContext == EContextGroupAlbum ) ||
        ( iContext == EContextGroupArtist ) ||
        ( iContext == EContextGroupSong ) ||
        ( iContext == EContextItemGenre ) ||
        ( iContext == EContextItemPlaylist ) ) &&
        // Check if the list is empty, Enable scroll buffer won't call requst if list is empty
        ( iListWidget->ItemCount() != 0 ) )
        {
        MPX_DEBUG1("CMPXCollectionViewHgContainer::PrepareListL - EnableScrollBufferL");
        iListWidget->EnableScrollBufferL(*this, KMPXListBufferSize, KMPXListBufferSize/4);
        }
    else
        {
        EndFullScreenAnimation();
        iListWidget->RefreshScreen(0);
        }
    iDefaultIconSet = EFalse;
    }

// ----------------------------------------------------------------------------
// Prepare view for displaying data in media wall with list
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::PrepareMediaWallWithListL(const CMPXMediaArray& aMediaArray, TInt aCount)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::PrepareMediaWallWithListL");

	// Now create the media wall & the list

    ((CAknAppUi*)iCoeEnv->AppUi())->StatusPane()->MakeVisible(EFalse);
    iThumbnailManager->SetSizeL( EAudioGridThumbnailSize );

    TRect clientRect = ((CAknView*)iView)->ClientRect();

    TAknLayoutRect mediawallLayout;
    mediawallLayout.LayoutRect( clientRect, AknLayoutScalable_Apps::cf0_flow_pane(0) );

    TAknLayoutRect listLayout;
    listLayout.LayoutRect( clientRect, AknLayoutScalable_Apps::listscroll_cf0_pane(0) );

    TRect mediaWallRect = mediawallLayout.Rect();
    TRect mwListRect = listLayout.Rect();

    // get front rectange from layout
    TAknLayoutRect frontRect;
    frontRect.LayoutRect( mediaWallRect, AknLayoutScalable_Apps::cf0_flow_pane_g1(0) );
	iImageSize = frontRect.Rect().Size();

    if( iMediaWall )
        {
        delete iMediaWall;
        iMediaWall = 0;
        delete iMwListWidget;
        iMwListWidget = 0;
        }

    if ( !iMediaWall )
		{
		iMediaWall = CHgVgMediaWall::NewL (
				mediaWallRect,
				aCount,
				CHgVgMediaWall::EHgVgMediaWallStyleCoverflowTBonePortrait,
				EFalse,
				this,
				DefaultIconL() );

		if( !iIsForeground )
		    iMediaWall->HandleLosingForeground();

		iMediaWall->SetMopParent(this);
		iMediaWall->EnableScrollBufferL(*this, KMPXListBufferSizeWithMediaWall, KMPXListBufferSizeWithMediaWall/4);
        iMediaWall->SetSelectionObserver(*this);
		ProvideDataWithoutThumbnailsMwL(aMediaArray);

		// Create the list to be used with Media wall view
		iMwListWidget = CHgSingleTextListWithIcon::NewL(mwListRect, 0, NULL, NULL);
		iMwListWidget->SetMenuProviderL(this);
		iMwListWidget->ClearFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
		iMwListWidget->SetScrollBarTypeL( CHgScroller::EHgScrollerScrollBar );
		iMwListWidget->SetSelectionObserver(*this);
        }
	else
	    {
        // Lets just reuse the mediawall item and resize it.
	    if( iMediaWall->Style() != CHgVgMediaWall::EHgVgMediaWallStyleCoverflowTBonePortrait )
	        iMediaWall->ChangeStyleL( CHgVgMediaWall::EHgVgMediaWallStyleCoverflowTBonePortrait, mediaWallRect, ETrue );

	    iMediaWall->EnableScrollBufferL(*this, KMPXListBufferSizeWithMediaWall, KMPXListBufferSizeWithMediaWall/4);
        iMediaWall->MakeVisible( ETrue );
        if ( !iMwListWidget )
            {
            iMwListWidget = CHgSingleTextListWithIcon::NewL(mwListRect, 0, NULL, NULL);
            iMwListWidget->SetMenuProviderL(this);
            iMwListWidget->ClearFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
            iMwListWidget->SetScrollBarTypeL( CHgScroller::EHgScrollerScrollBar );
            iMwListWidget->SetSelectionObserver(*this);
            }
        else
            {
            iMwListWidget->MakeVisible( ETrue );
            iMwListWidget->SetFocus( ETrue );
            iMwListWidget->SetRect( mwListRect );
            iMwListWidget->DrawNow();
            }
	    }

    if( iTranstionType != EMPXTranstionNotDefined )
        {
        iMediaWall->SetFlags( CHgVgMediaWall::EHgVgMediaWallDrawToWindowGC );
        }

    iMediaWall->SetOpeningAnimationType( CHgVgMediaWall::EHgVgOpeningAnimationNone );

    if ( iAlbumIndex == KErrNotFound )
        {
        iMediaWall->SetSelectedIndex( iRestoredAlbumIndex );
        // If we are about to start a transition animation we should draw only
        // when we have received first screen of thumbs in Refresh function.
        if( iTranstionType == EMPXTranstionNotDefined )
            iMediaWall->RefreshScreen(iRestoredAlbumIndex);
        OpenAlbumL(iRestoredAlbumIndex);
      }
    else
        {
        iMediaWall->SetSelectedIndex( iAlbumIndex );
        // If we are about to start a transition animation we should draw only
        // when we have received first screen of thumbs in Refresh function.
        if( iTranstionType == EMPXTranstionNotDefined )
            iMediaWall->RefreshScreen(iAlbumIndex);
        OpenAlbumL(iAlbumIndex);
       }
    iMwListWidget->RefreshScreen(0);
	iDefaultIconSet = ETrue;
    }

// ----------------------------------------------------------------------------
// Prepare view for displaying data in Media wall format.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::PrepareMediaWallL(const CMPXMediaArray& aMediaArray, TInt aCount)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::PrepareMediaWallL");
   	TRect appRect = ((CAknAppUi*)iCoeEnv->AppUi())->ApplicationRect();
	((CAknAppUi*)iCoeEnv->AppUi())->StatusPane()->MakeVisible(EFalse);
	iThumbnailManager->SetSizeL( EAudioFullScreenThumbnailSize );
	if( iCbaHandler )
	    iCbaHandler->UpdateCba();
    // get front rectange from layout
    TAknLayoutRect frontRect;
    frontRect.LayoutRect( appRect, AknLayoutScalable_Apps::cf0_flow_pane_g1(0) );
	iImageSize = frontRect.Rect().Size();

	CMPXCollectionViewHgSwitchBuffer* switchBuffer(NULL);

    if( iListWidget && (iPrevContext == EContextGroupAlbum || iPrevContext == EContextItemAlbum ) )
        {
        switchBuffer = CMPXCollectionViewHgSwitchBuffer::CreateBufferLC( *iListWidget );
        }
    else if ( iMediaWall )
        {
        switchBuffer = CMPXCollectionViewHgSwitchBuffer::CreateBufferLC( *iMediaWall );
        switchBuffer->SetIndexOffset(1);
        }
    if( iMediaWall )
        {
        delete iMediaWall;
        iMediaWall = 0;
        delete iMwListWidget;
        iMwListWidget = 0;
        }

	if (!iMediaWall)
	    {
        iMediaWall = CHgVgMediaWall::NewL(
                appRect,
                aCount,
                CHgVgMediaWall::EHgVgMediaWallStyleCoverflowFullScreen,
                ETrue,
                this,
                DefaultIconL() );

        if( !iIsForeground )
            iMediaWall->HandleLosingForeground();

        iMediaWall->SetMopParent(this);
        iMediaWall->SetSelectionObserver(*this);
        iMediaWall->SetObserver( this ); // softkey visibility event observer
        // Check if the list is empty, Enable scroll buffer won't call requst if list is empty
        if( iMediaWall->ItemCount() != 0 )
            {
        		iMediaWall->EnableScrollBufferL(
                *this,
                KMPXListBufferSizeWithMediaWall,
                KMPXListBufferSizeWithMediaWall / 4);
            }
        else
            {
            EndFullScreenAnimation();
            iMediaWall->RefreshScreen( 0 );
            }
        // We want softkeys to be drawn on top of mediawall, thats we need to manipulate window pos
        iMediaWall->DrawableWindow()->SetOrdinalPosition( -1 );
        if(((CAknAppUi*)iCoeEnv->AppUi())->IsFaded())
            {
            iMediaWall->DrawableWindow()->SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
            iMediaWall->SetFlags( CHgVgMediaWall::EHgVgMediaWallFaded );
            }
        ProvideDataWithoutThumbnailsMwL(aMediaArray);
        if( iAlbumIndex == KErrNotFound )
            iMediaWall->SetSelectedIndex( iRestoredAlbumIndex + iShuffleItem );
        else
            iMediaWall->SetSelectedIndex( iAlbumIndex + iShuffleItem );
	    }
	else
	    {
	    if( iMediaWall->Style() != CHgVgMediaWall::EHgVgMediaWallStyleCoverflowFullScreen )
	        iMediaWall->ChangeStyleL( CHgVgMediaWall::EHgVgMediaWallStyleCoverflowFullScreen, appRect, ETrue );

	    iMediaWall->MakeVisible( ETrue );
        iMediaWall->SetFocus( ETrue );
		// Check if the list is empty, Enable scroll buffer won't call requst if list is empty
        if( iMediaWall->ItemCount() != 0 )
            {
        		iMediaWall->EnableScrollBufferL(
                *this,
                KMPXListBufferSizeWithMediaWall,
                KMPXListBufferSizeWithMediaWall / 4);
            }
        else
            {
            EndFullScreenAnimation();
            iMediaWall->RefreshScreen( 0 );
            }
        if( iAlbumIndex == KErrNotFound )
            iMediaWall->SetSelectedIndex( iRestoredAlbumIndex + iShuffleItem );
        else
            iMediaWall->SetSelectedIndex( iAlbumIndex + iShuffleItem );
	    }

    if( iTranstionType != EMPXTranstionNotDefined )
        {
        iMediaWall->SetFlags( CHgVgMediaWall::EHgVgMediaWallDrawToWindowGC );
        }

	if( iPopupListRect == TRect(0,0,0,0) )
		{
	    ResolvePopupListSizeL();
		}

    iMediaWall->SetOpenedItemRect( iPopupListRect );
    iMediaWall->SetOpeningAnimationType( CHgVgMediaWall::EHgVgOpeningAnimationZoomToFront );

    if( switchBuffer )
        {
        switchBuffer->FillFromBufferL( *iMediaWall );
        CleanupStack::PopAndDestroy( switchBuffer );
        switchBuffer = NULL;
        }
	iDefaultIconSet = ETrue;
    }

// ----------------------------------------------------------------------------
// Add shuffle list item to the top of the list.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::SetShuffleItemToListL(CHgScroller* aScroller )
    {
    CHgItem* item = CHgItem::NewL();
    HBufC* shuffleText = StringLoader::LoadLC(
        R_MPX_TBONE_SHUFFLE );
    item->SetTitleL( *shuffleText );
    CleanupStack::PopAndDestroy( shuffleText );

    CGulIcon* icon = (*iIconArray)[EMPXClvIconShuffle];
    CFbsBitmap* bitmap = icon->Bitmap();
    CFbsBitmap* mask = icon->Mask();

    // Icon needs to be bigger!
    TSize size(35, 35);
    AknIconUtils::SetSize( bitmap, size );
    AknIconUtils::SetSize( mask, size );

    CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
    iconCopy->SetBitmapsOwnedExternally(ETrue);
    item->SetIcon( iconCopy );
    aScroller->SetItem( item, 0 );
    }

// ----------------------------------------------------------------------------
// Retreive the current list widget
// ----------------------------------------------------------------------------
CHgScroller* CMPXCollectionViewHgContainer::CurrentListWidget()
    {
    CHgScroller* current = NULL;
    switch (iCurrentViewType)
        {
        case EMPXViewTBone:
            {
            current = iMwListWidget;
            break;
            }
        case EMPXViewList:
            {
            current = iListWidget;
            break;
            }
        default:
            break;
        }

    return current;
    }

// ----------------------------------------------------------------------------
// Check if the current view is TBoneView.
// ----------------------------------------------------------------------------
TBool CMPXCollectionViewHgContainer::IsTBoneView()
    {
    TBool tBoneView = EFalse;

    if( EMPXViewTBone == iCurrentViewType || iOpenAlbumTracks )
        tBoneView = ETrue;

    return tBoneView;
    }

// ----------------------------------------------------------------------------
// Resolve the current view type based on the browsing context
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::ResolveCurrentViewType( TInt aCount )
    {
    iCurrentViewType = EMPXViewUnknown;
    TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
    switch (iContext)
        {
        case EContextGroupAlbum:
            {
            if( landscapeOrientation )
                iCurrentViewType = EMPXViewMediawall;
            else
            	{
				// In case we want to display the albums in T-Bone view
				// we change the view type and context appropriately.
				if ( iOpenAlbumTracks )
					{
                	iCurrentViewType = EMPXViewTBone;
                	iContext = EContextItemAlbum;
					}
				else
					{
					iCurrentViewType = EMPXViewList;
					}
				}
            break;
            }
        case EContextItemAlbum:
        case EContextItemArtist:
            {
            if( landscapeOrientation )
                iCurrentViewType = EMPXViewMediawall;
            else
                iCurrentViewType = EMPXViewTBone;
            break;
            }
        default:
            iCurrentViewType = EMPXViewList;
            break;
        }

    // if tbone view is empty, switch back to Albums list or meidawall view
    if( ( iCurrentViewType == EMPXViewTBone ) && ( aCount < 1 ) )
        {
        if( landscapeOrientation )
         iCurrentViewType = EMPXViewMediawall;
        else
         iCurrentViewType = EMPXViewList;
        }

    }

// ----------------------------------------------------------------------------
// Clears the previous view content.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::CleanPrevView()
    {
    if( iCurrentViewType == iPrevViewType )
        return;

    switch (iPrevViewType)
        {
        case EMPXViewMediawall:
            {
            if( iCurrentViewType != EMPXViewTBone )
                {
                delete iMediaWall;
                iMediaWall = 0;
                }
            break;
            }
        case EMPXViewTBone:
            {
            if( iCurrentViewType != EMPXViewMediawall )
                {
                delete iMediaWall;
                iMediaWall = 0;
                }
            delete iMwListWidget;
            iMwListWidget = 0;
            break;
            }
        case EMPXViewList:
            {
            delete iListWidget;
            iListWidget = 0;
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Check if the selected item is a song.
// ---------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgContainer::IsSelectedItemASong()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::IsSelectedItemASong" );

	TBool res(EFalse);
    if ( iContext == EContextItemAlbum ||
         iContext == EContextGroupSong ||
         iContext == EContextItemGenre )
        {
    	CHgScroller* listWidget = CurrentListWidget();
    	if ( !listWidget )
			{
			return EFalse;
			}
		if ( listWidget->SelectedIndex() == 0 &&
			 listWidget->ItemCount() > 1 )
			{
			res = EFalse;
			}
		else
			{
			res = ETrue;
			}
        }
    return res;
    }


// -----------------------------------------------------------------------------
// Retreives the selected item's media.
// -----------------------------------------------------------------------------
//
CMPXMedia* CMPXCollectionViewHgContainer::SelectedItemMediaL()
    {
    MPX_FUNC( "CMPXCollectionViewHgImp::SelectedItemMediaL" );
    CMPXMedia* song = NULL;
    const CMPXMediaArray& albums = iListBoxArray->MediaArray();
    CMPXMedia* album( albums.AtL( iSelectedAlbumIndex ) );
    const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    CHgScroller* listWidget = CurrentListWidget();
    if (listWidget && songs)
        {
		// Tracks list may have a shuffle item inserted so account for that.
        if (listWidget->ItemCount() > 1)
            {
            song = songs->AtL(listWidget->SelectedIndex()-1);
            }
        else
            {
            song = songs->AtL(listWidget->SelectedIndex());
            }
        }
    return song;
    }


// ---------------------------------------------------------------------------
// Handle item command
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleItemCommandL( TInt aCommand )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleItemCommandL" );
    MPX_DEBUG3("CMPXCollectionViewHgContainer::HandleItemCommandL iCurrentViewType = %d, iContext = %d", iCurrentViewType, iContext);
    if( aCommand == EMPXCmdPlay )
        {
        TInt index = CurrentLbxItemIndex();
        if (!ShufflePlayAllL(index))
            {
            switch( iContext )
                {
                case EContextGroupAlbum:
                case EContextItemAlbum:
                    {
                    iSelectedAlbumIndex = index;
                    SaveSelectedAlbumItemL(index);
                    // Open first song of album & playlist for entire album is created.
                    PlayAlbumL(index);
                    break;
                    }
                case EContextGroupPlaylist:
                    {
                    PlayPlaylistL(index);
					break;
                    }
                case EContextGroupGenre:
                    {
                    PlayGenreL(index);
					break;
                    }
                default:
                    break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// From MHgSelectionObserver
// Handle Item Selection
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleSelectL( TInt /*aIndex*/ )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleSelect" );

    }

// ---------------------------------------------------------------------------
// From MHgSelectionObserver
// Handle Item Selection
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleSelectL( TInt aIndex, CCoeControl* aControl )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleSelect" );

    TInt index(MediaIndex(aIndex));

    if( iCurrentViewType == EMPXViewTBone && iSelectedAlbumIndex != index )
        {
        if ( aControl == iMediaWall )
            {
            iSelectedAlbumIndex = index;
            iAlbumIndex = index;
            OpenAlbumL(index);
            }
        }
    else if( iCurrentViewType == EMPXViewMediawall )
        {
        iSelectedAlbumIndex = index;
        iAlbumIndex = index;
        }
    }


// ---------------------------------------------------------------------------
// From MHgSelectionObserver
// Handle Item opened
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleOpenL( TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleOpenL" );

    TInt index(MediaIndex(aIndex));

    // ganes list components still uses this version of the HandleOpen
    if ( iContext == EContextItemAlbum  )
		{
        SaveSelectedAlbumItemL(iSelectedAlbumIndex);
		UpdatePathAndOpenL(index);
		}
    else if ( iContext == EContextGroupAlbum )
        {
        SaveSelectedAlbumItemL(index);
        iAlbumIndex = index;
        // Check if shuffle play all was selected.
        if (!ShufflePlayAllL(index))
            {
			// To open the selected album.
            PrepareTboneViewL();
            }
        }
    else if ( iContext == EContextGroupSong || iContext == EContextItemPlaylist || iContext == EContextItemGenre )
        {
        // Check if shuffle play all was selected.
        if (!ShufflePlayAllL(index))
            {
            // To open the selected album.
            iView->ProcessCommandL( EMPXCmdCommonEnterKey );
            }
        }
    else
		{
		iView->ProcessCommandL( EMPXCmdCommonEnterKey );
		}
	}

// ---------------------------------------------------------------------------
// From MHgSelectionObserver
// Handle Item opened
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleOpenL( TInt aIndex, CCoeControl* aControl )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::HandleOpenL" );

    TInt index(MediaIndex(aIndex));

    if( iContext == EContextGroupAlbum && index >= 0 )
        {
        iAlbumIndex = index;
        iSelectedAlbumIndex = index;
        SaveSelectedAlbumItemL(index);
        }

    if ( iContext == EContextItemAlbum  )
        {
        if( iCurrentViewType == EMPXViewTBone )
            {
            iMediaWall->SetFlags( CHgVgMediaWall::EHgVgMediaWallDrawToWindowGC );
            iMediaWall->DrawNow();
            }
        if ( aControl == iMediaWall )
            {
			// If album was opened, we will play all tracks in the album.
            SaveSelectedAlbumItemL(index);
            // Open first song of album & playlist for entire album is created.
            UpdatePathAndOpenL(0, ETrue);
            }
        else if ( aControl == iMwListWidget )
            {
			// If item in list was opened, we only play the selected one.
            SaveSelectedAlbumItemL(iSelectedAlbumIndex);
           // Open the selected song of album
            UpdatePathAndOpenL(index);
            }
        }
    else if ( iContext == EContextGroupAlbum  )
        {
        if ( aControl == iMediaWall )
            {
			// Check if shuffle play all was selected.
			if (!ShufflePlayAllL(index))
				{
	            OpenAlbumL(index);
				}
            }
        }
    else
        {
        iView->ProcessCommandL( EMPXCmdCommonEnterKey );
        }
    }

// -----------------------------------------------------------------------------
// Handle media wall event
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleMediaWallEvent(
	TInt aEvent,
	CHgVgMediaWall* /*aSender*/)
    {
    switch( aEvent )
        {
        case CHgVgMediaWall::EHgVgMediaWallEventRequestShowSoftkeys:
            {
            if( iCbaHandler )
                iCbaHandler->ChangeCbaVisibility( ETrue );
            break;
            }
        case CHgVgMediaWall::EHgVgMediaWallEventRequestHideSoftkeys:
            {
            if( iCbaHandler )
                iCbaHandler->ChangeCbaVisibility( EFalse );
            break;
            }
        default:
            break;
        }
    }
// -----------------------------------------------------------------------------
// Provides the data to the model
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::Request(
	TInt aBufferStart,
	TInt aBufferEnd,
	THgScrollDirection aDirection )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::Request" );
   	MPX_DEBUG3( "CMPXCollectionViewHgContainer::Request aBufferStart = %d, aBufferEnd = %d",
            aBufferStart, aBufferEnd );

	// This should not happen but if not handled, bad things will happen.
	if ( aBufferStart < 0 || aBufferEnd < 0 )
   	    return;

    if ( aBufferStart - iShuffleItem > iLastValidMediaItemIndex ||
         aBufferEnd - iShuffleItem > iLastValidMediaItemIndex )
		{
		// We cannot provide data for the requested range at this time.
		// Remember the request so we can handle it when there's enough data.
		iRequestStart = aBufferStart;
    	iRequestEnd = aBufferEnd;
    	return;
		}

    iDirection = aDirection;

    for ( TInt i = aBufferStart; i <= aBufferEnd; i++ )
        {
        iThumbnailReqMap[ i ] = EFalse;
        }


    if ( aDirection == EHgBufferReset  || aDirection == EHgBufferScrollDown || aDirection == EHgBufferScrollUp)
        {
        // For index range in visible-area
        TRAP_IGNORE(ProvideDataIntersectL(aBufferStart, aBufferEnd));
        // For index range elsewhere
        TRAP_IGNORE(ProvideDataDifferenceL(aBufferStart, aBufferEnd));
       }
    else
        {
        TRAP_IGNORE(ProvideDataL(aBufferStart, aBufferEnd));
        }

    // When in main collection view, list can be refresh
    // without waiting on thumbnails since this list doesn't
    // require thumbnails.
    if ( iCurrentViewType == EMPXViewList && (
	     iContext == EContextGroupCollection ||
         iContext == EContextGroupPlaylist ||
         iContext == EContextGroupGenre ||
         iContext == EContextGroupComposer ) ||
         iContext == EContextGroupArtist )
        {
        iListWidget->RefreshScreen( aBufferStart );
        }
    iTopIndex = aBufferStart;
    iBottomIndex = aBufferEnd;
    }

// -----------------------------------------------------------------------------
// Check if request range also covers the visible area and provide data
// to it first.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ProvideDataIntersectL(
	TInt aBufferStart,
	TInt aBufferEnd )
    {
    TInt firstOnScreen  = 0;
    TInt lastOnScreen = 0;
    if( iMediaWall )
        {
        firstOnScreen = iMediaWall->FirstIndexOnScreen();
        lastOnScreen = firstOnScreen + iMediaWall->ItemsOnScreen();
        }
    else
        {
        firstOnScreen = iListWidget->FirstIndexOnScreen();
        lastOnScreen = firstOnScreen + iListWidget->ItemsOnScreen();
        }

    TInt startIndex = Max( firstOnScreen, aBufferStart );
    TInt endIndex = Min( lastOnScreen, aBufferEnd );

    ProvideDataForRangeL( startIndex, endIndex );
    }

// -----------------------------------------------------------------------------
// Handles request range outside of visible area.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ProvideDataDifferenceL(
	TInt aBufferStart,
	TInt aBufferEnd)
    {
    TInt firstOnScreen  = 0;
    TInt lastOnScreen = 0;
    if( iMediaWall )
        {
        firstOnScreen = iMediaWall->FirstIndexOnScreen();
        lastOnScreen = firstOnScreen + iMediaWall->ItemsOnScreen();
        }
    else
        {
        firstOnScreen = iListWidget->FirstIndexOnScreen();
        lastOnScreen = firstOnScreen + iListWidget->ItemsOnScreen();
        }
    if (aBufferStart == firstOnScreen && aBufferEnd == lastOnScreen)
        return;

    // Start block
    ProvideDataForRangeL( aBufferStart, firstOnScreen-1 );

    // End block
    ProvideDataForRangeL( lastOnScreen+1, aBufferEnd );

    }


// -----------------------------------------------------------------------------
// Generic data provider for the requested range.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ProvideDataForRangeL( TInt aBufferStart, TInt aBufferEnd )
    {
    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    TInt index(0);

    if(iDirection == MHgScrollBufferObserver::EHgBufferScrollUp )
        {
        for( TInt i = aBufferEnd; i >= aBufferStart; i-- )
			{
			// Just get the exiting item and update the fields + icon.
			CHgItem* item = NULL;
			if( iMediaWall )
				item = &iMediaWall->ItemL(i);
			else
				item = &iListWidget->ItemL(i);

			index = MediaIndex(i);
			if ( index >= 0 )
				{
				CMPXMedia* currentMedia( mediaArray.AtL( index ) );
				AddThumbnailToDisplayedItemL( item, currentMedia, index );
				}
			else
				{
				if ( iCurrentViewType == EMPXViewMediawall ||
					 iCurrentViewType == EMPXViewList && (i-iShuffleItem) == -1 )
					SetDetailIconShuffleL();
				iThumbnailReqMap[i] = ETrue;
				RefreshNoThumbnailL(i);
				}
			}
		}
	else
		{
		for( TInt i = aBufferStart; i <= aBufferEnd; i++ )
			{
			// Just get the exiting item and update the fields + icon.
			CHgItem* item = NULL;
			if( iMediaWall )
				item = &iMediaWall->ItemL(i);
			else
				item = &iListWidget->ItemL(i);

			index = MediaIndex(i);
			if ( index >= 0 )
				{
				CMPXMedia* currentMedia( mediaArray.AtL( index ) );
				AddThumbnailToDisplayedItemL( item, currentMedia, index );
				}
			else
				{
				if ( iCurrentViewType == EMPXViewMediawall ||
					 iCurrentViewType == EMPXViewList && (i-iShuffleItem) == -1 )
					SetDetailIconShuffleL();
				iThumbnailReqMap[i] = ETrue;
				RefreshNoThumbnailL(i);
				}
			}
		}
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::ProvideDataWithouThumbnailsL
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ProvideDataWithoutThumbnailsL(
	const CMPXMediaArray& aMediaArray,
	TInt aStartIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ProvideDataWithoutThumbnailsL" );

	if ( ShuffleItemPresent() )
		{
		CHgItem* item = &iListWidget->ItemL(0);
		SetShuffleItemTextL(item, NULL);
		}

	TInt mediaCount = aMediaArray.Count();

    for ( TInt i = aStartIndex; i < mediaCount ; i++ )
        {
        CMPXMedia* currentMedia( aMediaArray.AtL( i ) );

		if ( currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ==
			KMPXInvalidItemId )
			{
			break;
			}
        // Just get the exiting item and update the fields + icon.
        CHgItem* item = &iListWidget->ItemL(i+iShuffleItem);
		SetTitleL( item, currentMedia );
		AddDetailToDisplayedItemL( item, currentMedia, i );
		iLastValidMediaItemIndex = i;
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::ProvideDataWithoutThumbnailsMwL
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ProvideDataWithoutThumbnailsMwL(
	const CMPXMediaArray& aMediaArray,
	TInt aStartIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ProvideDataWithoutThumbnailsMwL" );

	if ( ShuffleItemPresent() )
		{
		CHgVgItem* item = &iMediaWall->ItemL(0);
		SetShuffleItemTextL(item, NULL);
		}

	TInt mediaCount = aMediaArray.Count();

    for ( TInt i = aStartIndex; i < mediaCount ; i++ )
        {
        CMPXMedia* currentMedia( aMediaArray.AtL( i ) );

		if ( currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ==
			KMPXInvalidItemId )
			{
			break;
			}
        // Just get the exiting item and update the fields + icon.
        CHgVgItem* item = &iMediaWall->ItemL(i+iShuffleItem);

		SetTitleL( item, currentMedia );
		AddDetailToDisplayedItemL( item, currentMedia, i );
		iLastValidMediaItemIndex = i;
        }
    }
// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::ProvideDataL
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ProvideDataL( TInt aStart, TInt aEnd )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ProvideDataL" );
   	MPX_DEBUG3( "CMPXCollectionViewHgContainer::ProvideDataL aStart = %d, aEnd = %d",
            aStart, aEnd );
   	if ( aStart < 0 ) aStart = 0;
   	if ( aEnd < 0 ) aEnd = 0;

	TInt index(0);
	const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    for ( TInt i = aStart; i <= aEnd ; i++ )
        {
        // Just get the exiting item and update the fields + icon.
        CHgItem* item = NULL;
        if( iCurrentViewType == EMPXViewMediawall || iCurrentViewType == EMPXViewTBone )
            item = &iMediaWall->ItemL(i);
        else
            item = &iListWidget->ItemL(i);

        index = MediaIndex(i);
        if ( index >= 0 )
            {
            CMPXMedia* currentMedia( mediaArray.AtL( index ) );
            AddThumbnailToDisplayedItemL( item, currentMedia, index );
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::Release
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::Release(TInt aReleaseStart, TInt aReleaseEnd)
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::Release" );

	TInt temp = aReleaseStart - iShuffleItem;
	TInt start =  temp >=0 ? temp : 0 ;
	TInt end = aReleaseEnd - iShuffleItem;

    for(; start <= end; ++start)
        {
        MPX_DEBUG2( "-->CMPXCollectionViewHgContainer::Release aReleaseStart = %d", start );
        iThumbnailManager->CancelThumb( start );
        }
    }

// ---------------------------------------------------------------------------
// Refresh for item without thumbnail
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::RefreshNoThumbnailL(TInt aDisplayIndex)
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::RefreshNoThumbnail" );

    RefreshL(aDisplayIndex);
    }
// ---------------------------------------------------------------------------
// Refresh list as needed.
// This function tries to determine the best time to refresh the screen as to
// avoid too many redraws. In some cases, multiple refresh is unavoidable.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::RefreshL(TInt aDisplayIndex)
    {
    if( !iIsForeground )
        {
        return;
        }

    MPX_FUNC( "CMPXCollectionViewHgContainer::Refresh" );

	TInt mediaCount = iListBoxArray->MediaArray().Count();
	TInt displayCount = mediaCount + iShuffleItem;

	TInt firstOnScreen = 0;
	TInt lastOnScreen =  0;

    TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
    CHgScroller* listWidget = CurrentListWidget();

    if( iCurrentViewType == EMPXViewTBone || iCurrentViewType == EMPXViewMediawall )
        {
        firstOnScreen = iMediaWall->FirstIndexOnScreen();
        lastOnScreen = firstOnScreen + iMediaWall->ItemsOnScreen();
        }
    else
        {
        firstOnScreen = listWidget->FirstIndexOnScreen();
        lastOnScreen = firstOnScreen + listWidget->ItemsOnScreen();
        }

	if( firstOnScreen < 0 )
	    firstOnScreen = 0;
	if (lastOnScreen > (displayCount - 1))
		lastOnScreen = displayCount - 1;

	// Refresh screen if the item is within view and no more thumbnails expected.
	if ( aDisplayIndex >= firstOnScreen && aDisplayIndex <= lastOnScreen )
		{
		TBool canRefresh(ETrue);
		for (TInt i= firstOnScreen; i <= lastOnScreen; i++)
			{
			if ( !iThumbnailReqMap[i] )
				{
				canRefresh = EFalse;
				break;
				}
			}
        if ( canRefresh )
            {
            if( iCurrentViewType == EMPXViewTBone || iCurrentViewType == EMPXViewMediawall )
                {
                if( iTranstionType != EMPXTranstionNotDefined )
                    {
                    // This will tricker the transtition animation
                    EndFullScreenAnimation();
                    iMediaWall->DrawNow();
                    }
                else
                    {
                    iMediaWall->RefreshScreen( firstOnScreen );
                    }
                }
            else
                {
                if( iTranstionType != EMPXTranstionNotDefined )
                    {
                    // This will tricker the transtition animation
                    // Use DrawNow since RefreshScreen uses DrawDeferred and we want to start
                    // the animation immediately.
                    EndFullScreenAnimation();
                    listWidget->DrawNow();
                    }
                else
                    {
                    listWidget->RefreshScreen( firstOnScreen );
                    }
                }
            }
		}
    }

// ----------------------------------------------------------------------------
// Get the default icon for the current browsing context.
// ----------------------------------------------------------------------------
CGulIcon* CMPXCollectionViewHgContainer::DefaultIconL()
    {

    TInt iconIndex( EMPXClvIconEmpty );

    switch( iContext )
        {
        case EContextGroupArtist:
        case EContextItemArtist:
            {
            iconIndex = EMPXClvIconArtist;
            break;
            }
        case EContextGroupAlbum:
        case EContextItemAlbum:
            {
            iconIndex = 27; // default album art in mediawall
            break;
            }
        case EContextGroupPodcast:
        case EContextItemPodcast:
            {
            iconIndex = EMPXPodClvIconTitle; // Podcast default icon
            break;
            }
        case EContextGroupPlaylist:
            {
            iconIndex = EMPXClvIconPlaylist;
            break;
            }
        case EContextGroupGenre:
            {
            iconIndex = EMPXClvIconGenre;
            break;
            }
        case EContextGroupComposer:
            {
            iconIndex = EMPXClvIconComposer;
            break;
            }
        case EContextItemComposer:
        case EContextGroupSong:
        case EContextItemSong:
        case EContextItemPlaylist:
        case EContextItemGenre:
        case EContextGroupCollection:
        case EContextUnknown:
        default:
            {
            // no default icon for main menu items
            break;
            }
        }

    CGulIcon* icon = (*iIconArray)[iconIndex];
    CFbsBitmap* bitmap = icon->Bitmap();
    CFbsBitmap* mask = icon->Mask();

    TSize size(iImageSize.iWidth, iImageSize.iHeight);
    AknIconUtils::SetSize( bitmap, size );
    AknIconUtils::SetSize( mask, size );

    CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
    iconCopy->SetBitmapsOwnedExternally(ETrue);
    return iconCopy;
    }

// -----------------------------------------------------------------------------
// Sets default icon to the HgList
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDefaultIconL()
	{

	TDefaultIcon defaultIcon( EMPXDefaultIconEmpty );
	TInt iconIndex( EMPXClvIconEmpty );

	switch( iContext )
		{
		case EContextGroupArtist:
		case EContextItemArtist:
			{
			defaultIcon = EMPXDefaultIconArtist;
			iconIndex = EMPXClvIconArtist;
			break;
			}
		case EContextGroupAlbum:
		case EContextItemAlbum:
            {
            defaultIcon = EMPXDefaultIconAlbum;
            iconIndex = EMPXClvIconAlbum;
			break;
			}
		case EContextGroupPodcast:
		case EContextItemPodcast:
			{
			defaultIcon = EMPXDefaultTitleIconPodcasts;
			iconIndex = EMPXPodClvIconTitle; // Podcast default icon
			break;
			}
		case EContextGroupPlaylist:
			{
			defaultIcon = EMPXDefaultIconPlaylist;
			iconIndex = EMPXClvIconPlaylist;
			break;
			}
		case EContextGroupGenre:
			{
			defaultIcon = EMPXDefaultIconGenre;
			iconIndex = EMPXClvIconGenre;
			break;
			}
		case EContextGroupComposer:
			{
			defaultIcon = EMPXDefaultIconComposer;
			iconIndex = EMPXClvIconComposer;
			break;
			}
		case EContextGroupSong:
        case EContextItemPlaylist:
        case EContextItemGenre:
			{
			defaultIcon = EMPXDefaultIconSongs;
			iconIndex = EMPXClvIconSongs;
			break;
			}
		case EContextItemComposer:
		case EContextItemSong:
		case EContextGroupCollection:
		case EContextUnknown:
		default:
			{
			// no default icon for main menu items
			break;
			}
		}

	CGulIcon* icon = (*iIconArray)[iconIndex];
	CFbsBitmap* bitmap = icon->Bitmap();
	CFbsBitmap* mask = icon->Mask();

	TSize size(iImageSize.iWidth, iImageSize.iHeight);
	AknIconUtils::SetSize( bitmap, size );
	AknIconUtils::SetSize( mask, size );

	CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
	iconCopy->SetBitmapsOwnedExternally(ETrue);
	CleanupStack::PushL( iconCopy );
	if ( iMediaWall )
		{
		iMediaWall->SetDefaultIconL(iconCopy);
		}
	else if ( iListWidget )
		{
		iListWidget->SetDefaultIconL(iconCopy);
		}
	else
		{
		delete iconCopy;
		}
	CleanupStack::Pop( iconCopy );
	iCurrentDefaultIcon = defaultIcon;
	}

// -----------------------------------------------------------------------------
// Sets default icon to list item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDefaultIconL(TInt aIndex)
	{

	TDefaultIcon defaultIcon( EMPXDefaultIconNotSet );
	TInt iconIndex( EMPXClvIconNone );

	switch( iContext )
		{

		case EContextGroupAlbum:
			{
			defaultIcon = EMPXDefaultIconAlbum;
			iconIndex = EMPXClvIconAlbum;
			break;
			}
		case EContextGroupPodcast:
		case EContextItemPodcast:
			{
			defaultIcon = EMPXDefaultTitleIconPodcasts;
			iconIndex = EMPXPodClvIconTitle; // Podcast default icon
			break;
			}
		case EContextItemComposer:
		case EContextGroupSong:
		case EContextItemSong:
		case EContextItemPlaylist:
		case EContextItemGenre:
		case EContextItemAlbum:
			{
			defaultIcon = EMPXDefaultIconSongs;
			iconIndex = EMPXClvIconSongs;
			break;
			}
		default:
			{
			break;
			}
		}

	CGulIcon* icon = (*iIconArray)[iconIndex];
	CFbsBitmap* bitmap = icon->Bitmap();
	CFbsBitmap* mask = icon->Mask();

	TSize size(iImageSize.iWidth, iImageSize.iHeight);
	AknIconUtils::SetSize( bitmap, size );
	AknIconUtils::SetSize( mask, size );

	CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
	iconCopy->SetBitmapsOwnedExternally(ETrue);
	CleanupStack::PushL( iconCopy );

	// TODO, fix this to use currentviewtype
	if ( iMediaWall && defaultIcon != EMPXDefaultIconNotSet )
		{
        iMediaWall->ItemL(aIndex).SetIcon(iconCopy);
		}
	else if ( iListWidget && defaultIcon != EMPXDefaultIconNotSet )
		{
        iListWidget->ItemL(aIndex).SetIcon(iconCopy);
		}
	else
		{
		delete iconCopy;
		}
	CleanupStack::Pop( iconCopy );
	}


// -----------------------------------------------------------------------------
// Adds detail to the list item at the specified index
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::AddDetailToDisplayedItemL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::AddDetailToDisplayedItemL" );

    switch ( iContext )
        {
        case EContextGroupCollection:
            {
            SetDetailIconL( aVisualItem, aIndex );
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupArtist:
            {
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupAlbum:
        	{
            SetDetailAlbumL( aVisualItem, aMedia );
        	//SetDetailArtistL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupPlaylist:
            {
            SetDetailDurationL( aVisualItem, aMedia );
            SetDetailIndicatorL( aVisualItem, aIndex );
          //  SetDetailIconL( aVisualItem, EMPXDefaultIconPlaylist ); // playlist icon
            break;
            }
        case EContextItemAlbum:
            {
            SetDetailAlbumL( aVisualItem, aMedia );
            //SetDetailArtistL( aVisualItem, aMedia );
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
        case EContextGroupSong:
        case EContextItemPlaylist:
        case EContextItemGenre:
        case EContextItemComposer:
        	{
            SetDetailArtistL( aVisualItem, aMedia );
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
        case EContextGroupPodcast:
            {
            SetDetailCountL( aVisualItem, aMedia );
          //  SetDetailIconL( aVisualItem, EMPXDefaultTitleIconPodcasts );
            break;
            }
        case EContextGroupGenre:
            {
          //  SetDetailIconL( aVisualItem, EMPXDefaultIconGenre ); //genre icon
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupComposer:
            {
           // SetDetailIconL( aVisualItem, EMPXDefaultIconComposer ); //composer icon
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextItemArtist:
        	{
            SetDetailCountL(aVisualItem, aMedia);
            break;
            }
        case EContextItemSong:
        	{
            SetDetailAlbumL( aVisualItem, aMedia );
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
        case EContextItemPodcast:
        	{
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
		}
	}

// -----------------------------------------------------------------------------
// Adds thumbnail to the list item at the specified index
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::AddThumbnailToDisplayedItemL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::AddThumbnailToDisplayedItemL" );


    switch ( iContext )
        {
        case EContextGroupCollection:
             {

             SetDetailIconL( aVisualItem, aIndex );
             break;
             }
        case EContextGroupAlbum:
        case EContextGroupSong:
        case EContextItemPlaylist:
        case EContextItemGenre:
        case EContextItemComposer:
        case EContextItemArtist:
        case EContextItemSong:
        case EContextItemPodcast:
        case EContextItemAlbum:
            {
            SetDetailThumbnailL( aMedia, aIndex );
            break;
            }

        }
    }

// -----------------------------------------------------------------------------
// Set title to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetTitleL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetTitleL" );


    if ( iContext == EContextGroupAlbum ||  iContext == EContextItemAlbum )
        {
        if ( aMedia->IsSupported( KMPXMediaMusicArtist ) )
            {
            const TDesC& title = aMedia->ValueText( KMPXMediaMusicArtist );
            if ( title.Compare( KNullDesC ) != 0 )
                {
                aVisualItem->SetTitleL( title );
                }
            else
                {
                HBufC* unknownText = NULL;
                if ( iContext == EContextGroupAlbum )
                    {
                    TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
                    if ( landscapeOrientation )
                        {
                        unknownText =
                            StringLoader::LoadLC( R_MPX_MEDIAWALL_ARTIST_UNKNOWN );
                        }
                    else
                        {
                        unknownText =
                            StringLoader::LoadLC( R_MPX_MP_LIST_ARTIST_UNKNOWN );
                        }
                    }
                else
                    {
                    unknownText =
                        StringLoader::LoadLC( R_MPX_MEDIAWALL_ARTIST_UNKNOWN );
                    }
                aVisualItem->SetTitleL( *unknownText );
                CleanupStack::PopAndDestroy( unknownText );
                }
            }
        }
    else
        {
        if ( aMedia->IsSupported( KMPXMediaGeneralTitle ) )
            {
            const TDesC& title = aMedia->ValueText( KMPXMediaGeneralTitle );
            if ( title.Compare( KNullDesC ) != 0 )
                {
                aVisualItem->SetTitleL( title );
                }
            else
                {
                if ( iContext == EContextGroupGenre )
                    {
                    HBufC* unknownText =
                        StringLoader::LoadLC( R_MPX_GENRE_UNKNOWN );
                    aVisualItem->SetTitleL( *unknownText );
                    CleanupStack::PopAndDestroy( unknownText );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Set song title to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetSongTitleL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetSongTitleL" );

    if ( aMedia->IsSupported( KMPXMediaGeneralTitle ) )
        {
        const TDesC& title = aMedia->ValueText( KMPXMediaGeneralTitle );
        if ( title.Compare( KNullDesC ) != 0 )
            {
            aVisualItem->SetTitleL( title );
            }
        else
            {
            HBufC* unknownText =
                StringLoader::LoadLC( R_MPX_COLLECTION_UNKNOWN );
            aVisualItem->SetTitleL( *unknownText );
            CleanupStack::PopAndDestroy( unknownText );
            }
        }
    }

// -----------------------------------------------------------------------------
// Set Detail - Count to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDetailCountL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetDetailCountL" );
	if ( aMedia->IsSupported( KMPXMediaGeneralCount ) )
		{
		TInt count( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount ) );

        if ( iContext == EContextGroupGenre )
            {
            HBufC* numSongsText = NULL;
            if ( count > 1 || count == 0 )
                {
                numSongsText = StringLoader::LoadLC( R_MPX_MUSIC_NUM_SONGS, count );
                }
            else
                {
                numSongsText = StringLoader::LoadLC( R_MPX_MUSIC_ONE_SONG );
                }
            TPtr ptr = numSongsText->Des();
            AknTextUtils::LanguageSpecificNumberConversion( ptr );
            aVisualItem->SetTextL( ptr );
            CleanupStack::PopAndDestroy( numSongsText );
            }
        }
    }

// -----------------------------------------------------------------------------
// Set Detail - Artist to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDetailArtistL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetDetailArtistL" );

    if ( aMedia->IsSupported( KMPXMediaMusicArtist ) )
        {
        // AK - Needs to be localized
        TBuf<KMPXMaxFileLength> detailText;
        const TDesC& artist = aMedia->ValueText( KMPXMediaMusicArtist );
        if ( artist.Compare( KNullDesC ) != 0 )
            {
            detailText.Copy(artist.Left(detailText.MaxLength()));
            aVisualItem->SetTextL( detailText );
            }
        else
            {
            HBufC* unknownText =
                StringLoader::LoadLC( R_MPX_SONGS_ARTIST_UNKNOWN );
            aVisualItem->SetTextL( *unknownText );
            CleanupStack::PopAndDestroy( unknownText );
            }
        }
    }

// -----------------------------------------------------------------------------
// Set Detail - Album to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDetailAlbumL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetDetailAlbumL" );

    TMPXGeneralCategory category( EMPXNoCategory );
    if ( aMedia->IsSupported( KMPXMediaGeneralCategory ) )
        {
        category = aMedia->ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
        }

    if ( aMedia->IsSupported( KMPXMediaMusicAlbum ) && category != EMPXCommand )
        {
        TBuf<KMPXMaxFileLength> detailText;
        const TDesC& album = aMedia->ValueText( KMPXMediaMusicAlbum );
        if ( album.Compare( KNullDesC ) != 0 )
            {
            detailText.Copy( album.Left(detailText.MaxLength()));
            aVisualItem->SetTextL( detailText );
            }
        else
            {
            HBufC* unknownText = NULL;
            if ( iContext == EContextGroupAlbum )
                {
                TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
                if ( landscapeOrientation )
                    {
                    unknownText =
                        StringLoader::LoadLC( R_MPX_MEDIAWALL_ALBUM_UNKNOWN );
                    }
                else
                    {
                    unknownText =
                        StringLoader::LoadLC( R_MPX_MP_LIST_ALBUM_UNKNOWN );
                    }
                }
            else
                {
                unknownText =
                    StringLoader::LoadLC( R_MPX_MEDIAWALL_ALBUM_UNKNOWN );
                }
            aVisualItem->SetTextL( *unknownText );
            CleanupStack::PopAndDestroy( unknownText );
            }
        }
    }


// -----------------------------------------------------------------------------
// Set Detail - Count + Duration to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDetailDurationL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetDetailDurationL" );
    if ( iContext == EContextGroupPlaylist &&
         aMedia->IsSupported( KMPXMediaGeneralDuration ) &&
	     aMedia->IsSupported( KMPXMediaGeneralCount ) )
		{
		TBuf<KMPXMaxFileLength> detailText;
		TInt count( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount ) );
   		TInt duration = aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration );

        UpdateTimeIndicatorsL(detailText, duration);
        HBufC* numSongsDurationText = NULL;
    	if ( count > 1 || count == 0 )
    	    {
    	    numSongsDurationText = StringLoader::LoadLC( R_MPX_MUSIC_NUM_SONGS_DURATION, detailText, count );
    	    }
    	else
    	    {
    	    numSongsDurationText = StringLoader::LoadLC( R_MPX_MUSIC_ONE_SONG_DURATION, detailText );
    	    }
        TPtr ptr = numSongsDurationText->Des();
        AknTextUtils::LanguageSpecificNumberConversion( ptr );
        aVisualItem->SetTextL( ptr );
        CleanupStack::PopAndDestroy( numSongsDurationText );
        }
    else if ( iContext == EContextItemPodcast &&
         aMedia->IsSupported( KMPXMediaGeneralDuration ) &&
         aMedia->IsSupported( KMPXMediaGeneralSize ) )
        {
        TBuf<KMPXMaxFileLength> detailText;
        TInt duration = aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration );
        UpdateTimeIndicatorsL(detailText, duration);
        detailText.Append( KMPXDash() );

        TInt size( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralSize ) );
        HBufC* sizeBuf = iCommonUiHelper->UnitConversionL( size, ETrue );
        TPtr sizePtr( sizeBuf->Des() );
        detailText.Append( sizePtr );
        delete sizeBuf;

        aVisualItem->SetTextL( detailText );
        }
    }


// -----------------------------------------------------------------------------
// Format the duration to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::UpdateTimeIndicatorsL(
    TDes& aBuf,
    TInt aDuration )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::UpdateTimeIndicatorsL" );
    TInt durationInSeconds = aDuration;
    if ( durationInSeconds > 0 )
        {
        durationInSeconds /= 1000;   // MilliSeconds to second conversion
        }
    else if ( durationInSeconds < 0 )
        {
        durationInSeconds = 0;
        }
    if ( durationInSeconds == 0 )
        {
        TLocale locale;
        TBuf<KMPXTimeIndicatorLength> pos;
        TChar separator = locale.TimeSeparator( KMPXMinSecSeparatorIndex );
        aBuf.Append(KMPXZeroDurationMark);
        aBuf.Append( separator );
        aBuf.Append(KMPXZeroDurationMark);
        }
    else
        {
        CMPXCommonUiHelper::TMPXDuratDisplayMode durationMode = CMPXCommonUiHelper::EMPXDuratAuto;
        if ( durationInSeconds > KOneHourInSeconds )
            {
            durationMode = CMPXCommonUiHelper::EMPXDuratHMS;
            }
        // Convert ellapsed time to texts
        HBufC* position = iCommonUiHelper->DisplayableDurationL( durationInSeconds, durationMode );
        aBuf.Append(position->Des());
        delete position;
        }
    }


// -----------------------------------------------------------------------------
// Set Detail - Indicator to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDetailIndicatorL(
    CHgItem* aVisualItem,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetDetailIndicatorL" );
    // Get icon indices
    RArray<TInt> iconIndices = iListBoxArray->IndicatorIconIndicesL( aIndex );
    CleanupClosePushL( iconIndices );

    const TInt iconCount = iconIndices.Count();
    TInt flags = aVisualItem->Flags() & CHgItem::EHgItemFlagMarked;
    aVisualItem->ClearFlags(aVisualItem->Flags());
    if ( iconCount > 0 )
    	{
        TInt iconId = 0;
		for ( TInt iconIndex = 0; iconIndex < iconCount; iconIndex++ )
			{
			iconId = iconIndices[iconIndex];
			if ( !iPodcastContext )
			    {
                switch (iconId)
                    {
                    case EMPXClvIconMMC:
                        {
                        flags |= CHgItem::EHgItemFlagsMmc;
                        break;
                        }
                    case EMPXClvIconPlay:
                        {
                        flags |= CHgItem::EHgItemFlagsPlaybackIndicator;
                        break;
                        }
                    case EMPXClvIconPause:
                        {
                        flags |= CHgItem::EHgItemFlagsPauseIndicator;
                        break;
                        }
                    case EMPXClvIconBrokenTrack:
                        {
                        flags |= CHgItem::EHgItemFlagsBrokenTrackIndicator;
                        break;
                        }
                    case EMPXClvIconCorruptTrack:
                        {
                        flags |= CHgItem::EHgItemFlagsCorruptedTrackIndicator;
                        break;
                        }
                    case EMPXClvIconDrmExpired:
                        {
                        flags |= CHgItem::EHgItemFlagsDrmRightsExpired;
                        break;
                        }
                    case EMPXClvIconBrokenPlaylist:
                        {
                        flags |= CHgItem::EHgItemFlagsBrokenPlaylistIndicator;
                        break;
                        }
                    }
			    }
            else // podcast icons
                {
                switch (iconId)
                    {
                    case EMPXPodClvIconMMC:
                        {
                        flags |= CHgItem::EHgItemFlagsMmc;
                        break;
                        }
                    case EMPXPodClvIconPlay:
                        {
                        flags |= CHgItem::EHgItemFlagsPlaybackIndicator;
                        break;
                        }
                    case EMPXPodClvIconPause:
                        {
                        flags |= CHgItem::EHgItemFlagsPauseIndicator;
                        break;
                        }
                    case EMPXPodClvIconBroken:
                        {
                        flags |= CHgItem::EHgItemFlagsBrokenTrackIndicator;
                        break;
                        }
                    case EMPXPodClvIconCorrupt:
                        {
                        flags |= CHgItem::EHgItemFlagsCorruptedTrackIndicator;
                        break;
                        }
                    case EMPXPodClvIconNew:
                        {
                        flags |= CHgItem::EHgItemFlagsNewPodcastIndicator;
                        break;
                        }
                    case EMPXPodClvIconCompplay:
                        {
                        flags |= CHgItem::EHgItemFlagsCompletelyPlayedPodcastIndicator;
                        break;
                        }
                    case EMPXPodClvIconPartplay:
                        {
                        flags |= CHgItem::EHgItemFlagsPlayedPodcastIndicator;
                        break;
                        }
                    }
                }
			}
		}
    aVisualItem->SetFlags( flags );
    CleanupStack::PopAndDestroy(&iconIndices);
    }


// -----------------------------------------------------------------------------
// Set Detail - Icon to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDetailIconL(
    CHgItem* aVisualItem,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetDetailIconL" );

    TInt iconIndex = aIndex;
    switch ( aIndex )
        {
        case 0:
            iconIndex = EMPXClvIconArtist;
            break;
        case 1:
            iconIndex = EMPXClvIconAlbum;
            break;
        case 2:
            iconIndex = EMPXClvIconPlaylist;
            break;
        case 3:
            iconIndex = EMPXClvIconSongs;
            break;
        case 4:
            iconIndex = EMPXClvIconPodcasts;
            break;
        case 5:
            iconIndex = EMPXClvIconGenre;
            break;
        case 6:
            iconIndex = EMPXClvIconComposer;
            break;
        case 7: //EMPXDefaultTitleIconPodcasts
            iconIndex = 10;
            break;
		}

    CGulIcon* icon = (*iIconArray)[iconIndex];

	CFbsBitmap* bitmap = icon->Bitmap();
	CFbsBitmap* mask = icon->Mask();

	TSize size(iImageSize.iWidth, iImageSize.iHeight);
    if ( iContext == EContextGroupCollection )
        { // shrink smaller to look same as in Antriksh
        size.iWidth = iImageSize.iWidth * KIconFactor;
        size.iHeight = iImageSize.iHeight * KIconFactor;
        }

    AknIconUtils::SetSize( bitmap, size );
    AknIconUtils::SetSize( mask, size );

    CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
	iconCopy->SetBitmapsOwnedExternally(ETrue);
    aVisualItem->SetIcon( iconCopy );
    }

// -----------------------------------------------------------------------------
// Set Detail - Thumbnail to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetDetailThumbnailL(
    CMPXMedia* aMedia,
    TInt aIndex )
    {
	  MPX_FUNC("CMPXCollectionViewHgContainer::SetDetailThumbnailL(CMPXMedia* aMedia,TInt aIndex)");
    TMPXItemId currentId( aMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

    if ( currentId == KMPXInvalidItemId )
        {
        return;
        }

	if ( aMedia->IsSupported( KMPXMediaMusicAlbumArtFileName ) )
		{
		const TDesC& album = aMedia->ValueText( KMPXMediaMusicAlbumArtFileName );
		#ifdef _DEBUG
		RDebug::RawPrint(album);
		#endif
		if ( album.Compare( KNullDesC ) != 0 )
			{
			iThumbnailManager->LoadThumbL( aIndex , album);
			}
		else
			{
			//no albumart
			// Default album art has already been set earlier.
    		iThumbnailReqMap[aIndex+iShuffleItem] = ETrue;
    		RefreshNoThumbnailL(aIndex);
			}
		}
	else
	    {
		// no album art supported
		// Default album art has already been set earlier.
	    if ( iCurrentViewType == EMPXViewMediawall ||
	         iCurrentViewType == EMPXViewList && (aIndex-iShuffleItem) == -1 )
	        SetDetailIconShuffleL(); // BUG: temporarily called here because default icon is shown if SetDefaultIconL is called after.
    	iThumbnailReqMap[aIndex+iShuffleItem] = ETrue;
        RefreshNoThumbnailL(aIndex);
	    }
    }

// ----------------------------------------------------------------------------
// Set the shuffle item icon
// ----------------------------------------------------------------------------

void CMPXCollectionViewHgContainer::SetDetailIconShuffleL()
    {

	if (iShuffleItem)
		{
		CGulIcon* icon = (*iIconArray)[EMPXClvIconShuffle];
		CFbsBitmap* bitmap = icon->Bitmap();
		CFbsBitmap* mask = icon->Mask();

		TSize size(CHgDoubleGraphicListFlat::PreferredImageSize());
		CHgItem* item = NULL;
		if( iCurrentViewType == EMPXViewMediawall || iCurrentViewType == EMPXViewTBone )
			{
			item = &iMediaWall->ItemL(0);
			TRect appRect = ((CAknAppUi*)iCoeEnv->AppUi())->ApplicationRect();
			// get front rectange from layout
			TAknLayoutRect frontRect;
			frontRect.LayoutRect( appRect, AknLayoutScalable_Apps::cf0_flow_pane_g1(0) );
			size = frontRect.Rect().Size();
			}
		else if( iCurrentViewType == EMPXViewList )
			{
			item = &iListWidget->ItemL(0);
			}
		else
			{
			User::Leave( KErrNotSupported );
			}

		if ( item )
			{
			AknIconUtils::SetSize( bitmap, size );
			AknIconUtils::SetSize( mask, size );

			CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
			iconCopy->SetBitmapsOwnedExternally(ETrue);
			item->SetIcon( iconCopy );
			}
		}



    }

// ----------------------------------------------------------------------------
// Set the shuffle item text
// ----------------------------------------------------------------------------

TBool CMPXCollectionViewHgContainer::SetShuffleItemTextL(CHgItem* aItem, CMPXMedia* aMedia)
    {

	TBool res(EFalse);

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    const TInt count( mediaArray.Count() );

    if ( count > 1 && iContext == EContextGroupAlbum )
        {
        TBool landscapeOrientation = Layout_Meta_Data::IsLandscapeOrientation();
        if ( landscapeOrientation )
            {
            HBufC* shuffleText = StringLoader::LoadLC(
                R_MPX_MEDIAWALL_TITLE_SHUFFLE_ALL );
            aItem->SetTitleL( *shuffleText );
            CleanupStack::PopAndDestroy( shuffleText );
            }
        else
            {
            HBufC* shuffleText = StringLoader::LoadLC(
                R_MPX_SHUFFLE );
            aItem->SetTitleL( *shuffleText );
            CleanupStack::PopAndDestroy( shuffleText );
            }

        // We can try to set icon too.
        SetDetailIconShuffleL();
        res = ETrue;
        }

    if ( count > 1 &&
        iContext == EContextGroupSong ||
        iContext == EContextGroupGenre ||
        iContext == EContextItemPlaylist ||
        iContext == EContextItemGenre ||
        iContext == EContextItemSong )
        {
        HBufC* shuffleText = StringLoader::LoadLC(
            R_MPX_SHUFFLE );
        aItem->SetTitleL( *shuffleText );
        CleanupStack::PopAndDestroy( shuffleText );

        // We can try to set icon too.
        SetDetailIconShuffleL();
        res = ETrue;
        }

    return res;
    }

// -----------------------------------------------------------------------------
// Sets the current category.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetCollectionContextL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetCollectionContextL" );

    const CMPXMedia& media = iListBoxArray->ContainerMedia();

    if ( NULL == &media )
        {
        MPX_DEBUG1( "CMPXCollectionViewHgContainer::SetCollectionContextL NO media");
        User::Leave(KErrNotFound);
        }

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
    MPX_DEBUG3( "CMPXCollectionViewHgContainer::SetCollectionContextL container type = %d, category = %d",
            containerType, containerCategory );

    iPodcastContext = EFalse;
    TMPXPodcastCategory podcastCategory( EMPXUnknown );
    if ( media.IsSupported( KMPXMediaPodcastType ) )
        {
        iPodcastContext = ETrue;
        if ( media.IsSupported( KMPXMediaPodcastCategoryGroup ) )
            {
            podcastCategory = media.ValueTObjectL<TMPXPodcastCategory>( KMPXMediaPodcastCategoryGroup );
            }
        }
    MPX_DEBUG3( "CMPXCommonContainerHitchcockImp::SetCollectionContextL container podcast = %d, category = %d",
            iPodcastContext, podcastCategory );

    iPrevContext = iContext;

    if( iListWidget )
        {
        iListWidget->ClearFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
        }
    if ( !iPodcastContext )
        {
        if ( containerType == EMPXGroup )
            {
            switch (containerCategory)
                {
                case EMPXCollection:
                    iContext = EContextGroupCollection;
                    if( iListWidget )
                        {
                        iListWidget->SetFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
                        }
                    break;
                case EMPXArtist:
                    iContext = EContextGroupArtist;
                    break;
                case EMPXAlbum:
                    iContext = EContextGroupAlbum;
                    break;
                case EMPXPlaylist:
                    iContext = EContextGroupPlaylist;
                    break;
                case EMPXSong:
                    iContext = EContextGroupSong;
                    break;
                case EMPXGenre:
                    iContext = EContextGroupGenre;
                    break;
                case EMPXComposer:
                    iContext = EContextGroupComposer;
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
                case EMPXArtist:
                    iContext = EContextItemArtist;
                    break;
                case EMPXAlbum:
                    iContext = EContextItemAlbum;
                    break;
                case EMPXPlaylist:
                    iContext = EContextItemPlaylist;
                    break;
                case EMPXSong:
                    iContext = EContextItemSong;
                    break;
                case EMPXGenre:
                    iContext = EContextItemGenre;
                    break;
                case EMPXComposer:
                    iContext = EContextItemComposer;
                    break;
                default:
                    User::Leave(KErrNotSupported);
                    break;
                }
            }
         else if ( containerType == EMPXOtherType )
            {

			if (containerCategory == EMPXAlbum)
				{
				//iContext = EContextItemAlbum2;
				}

            }
        }
    else
        {
        if ( (containerType == EMPXGroup) && (podcastCategory == EMPXTitle) )
            {
            iContext = EContextGroupPodcast;
			if( iListWidget )
				{
				iListWidget->SetFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
				}
			}
        else if ( (containerType == EMPXItem) && (podcastCategory == EMPXEpisode) )
            {
            iContext = EContextItemPodcast;
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }



void CMPXCollectionViewHgContainer::TNReadyL(TInt aError, CFbsBitmap* aBitmap, CFbsBitmap* /*aMask*/, TInt aIndex)
    {
   TInt displayIndex = aIndex+iShuffleItem;
   if ( aError == KErrNone )
        {
        const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
        CMPXMedia* currentMedia( mediaArray.AtL( aIndex ) );
        if ( currentMedia->IsSupported( KMPXMediaGeneralId ) )
            {
            if ( aBitmap )
                {
                CGulIcon* icon = CGulIcon::NewL(aBitmap, NULL);

                switch (iCurrentViewType)
                    {
                    case EMPXViewMediawall:
                    case EMPXViewTBone:
                        {
                        iMediaWall->ItemL(displayIndex).SetIcon(icon);
                        break;
                        }
                    case EMPXViewList:
                        {
                        iListWidget->ItemL(displayIndex).SetIcon(icon);
                        break;
                        }
                    default:
                        break;
                    }


                }
            }
        }
    else
        {
        //no albumart supported
        //iThumbnailReqMap[displayIndex] = ETrue;
        //RefreshL(displayIndex);
        }

	iThumbnailReqMap[displayIndex] = ETrue;
	RefreshL(displayIndex);


    }

// ----------------------------------------------------------------------------
// Clears the TN request if any
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::CancelTNRequest()
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::CancelTNRequest");

    }


// ----------------------------------------------------------------------------
// Construct a collection path with the current selection and open the
// path to start playback.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::UpdatePathAndOpenL(TInt aIndex, TBool aSelectAll)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::UpdatePathAndOpenL");

	RArray<TMPXItemId> ids;
	CleanupClosePushL(ids);
    const CMPXMediaArray& albums = iListBoxArray->MediaArray();
    CMPXMedia* album( albums.AtL( iSelectedAlbumIndex ) );

	if( album->IsSupported(KMPXMediaArrayContents) )
		{

		// Get the current path
		CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
		CleanupStack::PushL( cpath );

        MPX_DEBUG_PATH(*cpath);

		if ( 2 == cpath->Levels())
            {
            cpath->Back();
            cpath->AppendL(3);
            }
        else if (cpath->Levels() == 3)
			{
			// go back one level before amending path with new levels
			cpath->Back();
			}

        else if  (cpath->Levels() == 4)
			{
			// go back 2 levels before appending new level
			cpath->Back();
			cpath->Back();
			}


		TInt albumCount = albums.Count();
		for (TInt i=0; i<albumCount; ++i)
			{
			CMPXMedia* album = albums.AtL(i);
			const TMPXItemId id = album->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
			ids.AppendL(id);
			}
		cpath->AppendL(ids.Array()); // top level items
		cpath->Set(iSelectedAlbumIndex);
		ids.Reset();

		const CMPXMediaArray* songs = album->Value<CMPXMediaArray>(KMPXMediaArrayContents);
		User::LeaveIfNull(const_cast<CMPXMediaArray*>(songs));
		TInt count = songs->Count();
		for (TInt i=0; i<count; ++i)
			{
			CMPXMedia* song = songs->AtL(i);
			const TMPXItemId id = song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
			ids.AppendL(id);
			}

		cpath->AppendL(ids.Array()); // top level items

		if ( aSelectAll ) // Plays all songs without shuffle
			{
			cpath->Set(0);
			}
		else
			{
			if ( count > 1 ) // We need to adjust in case aIndex is the shuffle item
				{
				if ( aIndex == 0 ) // Plays all songs shuffled
					{
					TTime time;
					time.UniversalTime();
					TInt64 seed = time.Int64();
					TInt randIndex = Math::Rand(seed) % count;
					cpath->Set(randIndex);
					iPlaybackUtility->SetL( EPbPropertyRandomMode, ETrue );
					}
				else
					{
					cpath->Set(aIndex-1); // actual selection
					}
				}
			else
				{
				cpath->Set(aIndex);
				}
			}

		MPX_DEBUG_PATH(*cpath);

		TMPXPlaybackState pbState( iPlaybackUtility->StateL() );
		if ( pbState == EPbStatePlaying || pbState == EPbStatePaused )
			{
			if ( IsPlayingCurrentIndexL(cpath) )
				{
				if ( pbState == EPbStatePaused )
					{
					iPlaybackUtility->CommandL( EPbCmdPlay );
					}
                iView->ProcessCommandL( EMPXCmdGoToNowPlaying );
				}
			else
			    {
            	iCollectionUtility->Collection().OpenL(*cpath);
			    }
			}
		else
		    {
            iCollectionUtility->Collection().OpenL(*cpath);
		    }

		CleanupStack::PopAndDestroy( cpath );


		}
	CleanupStack::PopAndDestroy(&ids);
	}


// ----------------------------------------------------------------------------
// Construct a collection path with the current selection and construct a
// playlist from path to start playback.
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::UpdatePathAndOpenPlaylistL(
	 const CMPXMedia& aFindResult )
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::UpdatePathAndOpenPlaylistL");

	TInt selectedIndex(0);
    CHgScroller* listWidget = CurrentListWidget();
	if ( iContext == EContextGroupPlaylist ||
	     iContext == EContextGroupGenre && listWidget )
		{
		selectedIndex = MediaIndex(listWidget->SelectedIndex());
		}
	else
		{
		return;
		}

	RArray<TMPXItemId> ids;
	CleanupClosePushL(ids);

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();

	// Get the current path
	CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
	CleanupStack::PushL( cpath );

	MPX_DEBUG_PATH(*cpath);

    if ( 2 == cpath->Levels())
        {
        cpath->Back();
        cpath->AppendL(3);
        }
	else if (cpath->Levels() == 3)
		{
		// go back one level before amending path with new levels
		cpath->Back();
		}

	TInt mediaCount = mediaArray.Count();
	for (TInt i=0; i<mediaCount; ++i)
		{
		CMPXMedia* mediaItem = mediaArray.AtL(i);
		const TMPXItemId id = mediaItem->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
		ids.AppendL(id);
		}
	cpath->AppendL(ids.Array()); // top level items
	cpath->Set(selectedIndex); // set the selected item
	ids.Reset();

	CMPXMediaArray* songArray(const_cast<CMPXMediaArray*>( aFindResult.Value<CMPXMediaArray>(
							KMPXMediaArrayContents ) ) );
	User::LeaveIfNull( songArray );

	TInt count = songArray->Count();
	for (TInt i=0; i<count; ++i)
		{
		CMPXMedia* song = songArray->AtL(i);
		const TMPXItemId id = song->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
		ids.AppendL(id);
		}

	cpath->AppendL(ids.Array()); // top level items of songs
	cpath->Set(0); // select 1st song

	MPX_DEBUG_PATH(*cpath);

	TMPXPlaybackState pbState( iPlaybackUtility->StateL() );
	if ( pbState == EPbStatePlaying || pbState == EPbStatePaused )
		{
		if ( IsPlayingCurrentIndexL(cpath) )
			{
			if ( pbState == EPbStatePaused )
				{
				iPlaybackUtility->CommandL( EPbCmdPlay );
				}
			iView->ProcessCommandL( EMPXCmdGoToNowPlaying );
			}
		else
			{
			iPlaylistHelper->InitPlaylistL(*cpath, EFalse);
			}
		}
	else
		{
		iPlaylistHelper->InitPlaylistL(*cpath, EFalse);
		}

	CleanupStack::PopAndDestroy( cpath );

	// We need to restore the status pane before switching to playback view.
	if (!((CAknAppUi*)iCoeEnv->AppUi())->StatusPane()->IsVisible())
		((CAknAppUi*)iCoeEnv->AppUi())->StatusPane()->MakeVisible(ETrue);

	CleanupStack::PopAndDestroy(&ids);
	}


// ----------------------------------------------------------------------------
// Handle opening the album to show songs
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::OpenAlbumL(TInt aIndex)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::OpenAlbumL");

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    CMPXMedia* album( mediaArray.AtL( aIndex ) );
    if( album->IsSupported(KMPXMediaArrayContents) )
        {
        // We've previously fetched the songs for this album so
        // all we do now is populate the list with the song titles.
        if( iCurrentViewType == EMPXViewMediawall )
            {
            ShowAlbumSongsDialogL(*album);
            }
        else
            {
            ShowAlbumSongsL(*album);
            }
        }
    else
        {
        // We'll find the songs for the selected album
        iFindOp = EMPXOpenAlbum;
        FindAlbumSongsL(aIndex);
        }

    }

// ----------------------------------------------------------------------------
// Handle preparing playback of the selected album
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::PlayAlbumL(TInt aIndex)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::PlayAlbumL");

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    CMPXMedia* album( mediaArray.AtL( aIndex ) );
    if( album->IsSupported(KMPXMediaArrayContents) )
        {
        // We've previously fetched the songs for this album so
        // all we do now is construct a playlist for the album
        UpdatePathAndOpenL(0, ETrue);
        }
    else
        {
        // We'll find the songs for the selected album
        iFindOp = EMPXPlayAlbum;
        FindAlbumSongsL(aIndex);
        }
    }

// ----------------------------------------------------------------------------
// Handle preparing playback of the selected playlist
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::PlayPlaylistL(TInt aIndex)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::PlayPlaylistL");

	// We'll find the songs for the selected playlist
	iFindOp = EMPXPlayPlaylist;
	FindPlaylistSongsL(aIndex);
    }

// ----------------------------------------------------------------------------
// Handle preparing playback of the selected genre
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::PlayGenreL(TInt aIndex)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::PlayGenreL");

	// We'll find the songs for the selected genre
	iFindOp = EMPXPlayGenre;
	FindGenreSongsL(aIndex);
    }

// ----------------------------------------------------------------------------
// Check if shuffle item is selected and if so, initialize shuffle playback.
// ----------------------------------------------------------------------------
TBool CMPXCollectionViewHgContainer::ShufflePlayAllL(TInt aIndex)
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::ShufflePlayAllL");

    TBool shuffle(EFalse);

    if ( aIndex == -1 )
        {
        shuffle = ETrue;
        if ( iContext == EContextItemPlaylist || iContext == EContextItemGenre )
            {
            CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cpath );
            iPlaylistHelper->InitPlaylistL(*cpath, shuffle);
            CleanupStack::PopAndDestroy( cpath );
            }
        else
            {
            iPlaylistHelper->InitPlaylistL(shuffle);
            }
        }

    return shuffle;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::FindAlbumSongsL
// Find all the songs for the selected album index.
// If songs are already in the media array, we don't need to fetch them again.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::FindAlbumSongsL(
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::FindAlbumSongsL" );
    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    CMPXMedia* album( mediaArray.AtL( aIndex ) );

    // Fetch the songs for the selected album
    TMPXItemId albumId = album->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    CMPXMedia* findCriteria = CMPXMedia::NewL();
    CleanupStack::PushL( findCriteria );
    findCriteria->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXGroup );
    findCriteria->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXSong );
    findCriteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, albumId );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( TMPXAttribute( KMPXMediaIdGeneral,
                                 EMPXMediaGeneralTitle |
                                 EMPXMediaGeneralId |
                                 EMPXMediaGeneralType |
                                 EMPXMediaGeneralCategory |
                                 EMPXMediaGeneralFlags ) );
    attrs.Append( KMPXMediaMusicAlbumTrack );


    iCollectionUtility->Collection().FindAllL( *findCriteria, attrs.Array(), *this );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( findCriteria );

    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::FindPlaylistSongsL
// Find all the songs for the selected playlist index.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::FindPlaylistSongsL(
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::FindPlaylistSongsL" );
    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    CMPXMedia* playlist( mediaArray.AtL( aIndex ) );

    // Fetch the songs for the selected album
    TMPXItemId playlistId = playlist->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    CMPXMedia* findCriteria = CMPXMedia::NewL();
    CleanupStack::PushL( findCriteria );
    findCriteria->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXGroup );
    findCriteria->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXSong );
    findCriteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, playlistId );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( TMPXAttribute( KMPXMediaIdGeneral,
                                 EMPXMediaGeneralTitle |
                                 EMPXMediaGeneralId |
                                 EMPXMediaGeneralType |
                                 EMPXMediaGeneralCategory |
                                 EMPXMediaGeneralFlags ) );

    iCollectionUtility->Collection().FindAllL( *findCriteria, attrs.Array(), *this );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( findCriteria );

    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::FindGenreSongsL
// Find all the songs for the selected genre index.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::FindGenreSongsL(
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::FindPlaylistSongsL" );
    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    CMPXMedia* genre( mediaArray.AtL( aIndex ) );

    // Fetch the songs for the selected genre
    TMPXItemId genreId = genre->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    CMPXMedia* findCriteria = CMPXMedia::NewL();
    CleanupStack::PushL( findCriteria );
    findCriteria->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, EMPXGroup );
    findCriteria->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, EMPXSong );
    findCriteria->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, genreId );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( TMPXAttribute( KMPXMediaIdGeneral,
                                 EMPXMediaGeneralTitle |
                                 EMPXMediaGeneralId |
                                 EMPXMediaGeneralType |
                                 EMPXMediaGeneralCategory |
                                 EMPXMediaGeneralFlags ) );

    iCollectionUtility->Collection().FindAllL( *findCriteria, attrs.Array(), *this );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( findCriteria );

    }


// ---------------------------------------------------------------------------
// From MMPXCollectionFindObserver
// Handle callback for "find" operation
// Songs are saved back into the media array for subsequent use.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::HandleFindAllL(
    const CMPXMedia& aResults,
    TBool /*aComplete*/,
    TInt aError )
    {
    MPX_DEBUG2( "-->CMPXCollectionViewHgContainer::HandleFindAllL aError = %d", aError );

    if ( aError == KErrNone )
		{
		if ( iFindOp == EMPXOpenAlbum || iFindOp == EMPXPlayAlbum )
			{
			CMPXMediaArray* songArray(const_cast<CMPXMediaArray*>( aResults.Value<CMPXMediaArray>(
							KMPXMediaArrayContents ) ) );
			User::LeaveIfNull( songArray );

			// save the songs to the album so that we don't need to find them again
			// if the same album is selected again.
			if ( songArray->Count() )
				{
				const CMPXMediaArray& albumsArray = iListBoxArray->MediaArray();
				CMPXMedia* albumMedia( albumsArray.AtL( iSelectedAlbumIndex ) );
				albumMedia->SetCObjectValueL(KMPXMediaArrayContents, songArray);
				albumMedia->SetTObjectValueL<TInt>(KMPXMediaArrayCount, songArray->Count());
				}

			if ( iFindOp == EMPXOpenAlbum )
			    {
                if( iCurrentViewType == EMPXViewMediawall )
                    {
                    ShowAlbumSongsDialogL( aResults );
                    }
                else
                    {
                    ShowAlbumSongsL( aResults );
                    }
			    }
			else if ( iFindOp == EMPXPlayAlbum )
			    {
		        PlayAlbumL(iSelectedAlbumIndex);
			    }
			iFindOp = EMPXNoOp;
			}
		else if ( iFindOp == EMPXPlayPlaylist || iFindOp == EMPXPlayGenre )
			{
			iFindOp = EMPXNoOp;
			UpdatePathAndOpenPlaylistL(aResults);
			}
		}
	else
		{
		// We don't do anything if there's error
		return;
		}
    }

// ---------------------------------------------------------------------------
// Check if the current playing song is the same as the selected song.
// ---------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgContainer::IsPlayingCurrentIndexL(CMPXCollectionPath* aPath)
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::CurrentPlaybackIndexL" );
    TBool ret( EFalse );

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
	        MPX_DEBUG_PATH(*aPath);
	        MPX_DEBUG_PATH(*pbPath);

			TInt playbackPathCount( pbPath->Levels() );
			if ( aPath->Levels() == playbackPathCount )
				{
				TBool isEqual( ETrue );
				for ( TInt i = 0; i < playbackPathCount - 1; i++ )
					{
					if ( aPath->Id( i ) != pbPath->Id( i ) )
						{
						isEqual = EFalse;
						break;
						}
					}
				if ( isEqual )
					{
					if (pbPath->Index() == aPath->Index() )
						ret = ETrue;
					}
				}
			CleanupStack::PopAndDestroy( pbPath );
			CleanupStack::PopAndDestroy( playlist );
			}
		}

    return ret;
    }

// -----------------------------------------------------------------------------
// Shows the album songs in songs dialog.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ShowAlbumSongsDialogL( const CMPXMedia& aResults )
    {
    CAknSingleGraphicPopupMenuStyleListBox* listBox = new ( ELeave ) CAknSingleGraphicPopupMenuStyleListBox;
    CleanupStack::PushL( listBox );

    CAknPopupList* dialog = CAknPopupList::NewL(listBox, R_MPX_COLLECTION_ALBUMSONGS_LIST_CBA,
            AknPopupLayouts::EDynMenuWindow );

	CleanupStack::PushL( dialog );

    listBox->ConstructL( dialog,
            EAknListBoxSelectionList | EAknListBoxScrollBarSizeExcluded  );


    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto );

    // Set title
    if ( iSelectedMediaInAlbumView && iSelectedMediaInAlbumView->IsSupported( KMPXMediaMusicAlbum ) )
        {
        const TDesC& album = iSelectedMediaInAlbumView->ValueText( KMPXMediaMusicAlbum );
            if ( album.Compare( KNullDesC ) != 0 )
            {
            dialog->SetTitleL( album );
            }
        else
            {
            HBufC* unknownText =
                StringLoader::LoadLC( R_MPX_MEDIAWALL_POPUP_TITLE_UNKNOWN );
            dialog->SetTitleL( *unknownText );
            CleanupStack::PopAndDestroy( unknownText );
            }
        }

    CMPXMediaArray* songArray(const_cast<CMPXMediaArray*>( aResults.Value<CMPXMediaArray>(
            KMPXMediaArrayContents ) ) );
    User::LeaveIfNull( songArray );
    TInt songCount = songArray->Count();

    CDesC16ArrayFlat* songList = new (ELeave) CDesC16ArrayFlat(songCount);

    if ( songCount > 1 )
        {
        HBufC* shuffleText = StringLoader::LoadLC(
            R_MPX_MEDIAWALL_DIALOG_SHUFFLE );
        // Make room for 3 more formatting characters.
		HBufC* finalText = HBufC::NewLC(shuffleText->Length() + 3);
		TPtr ptr = finalText->Des();
		// This tells the list to use icon at index 0 of array index.
		ptr.Append(_L("0\t"));
		ptr.Append(shuffleText->Des());
        songList->AppendL( *finalText );
        CleanupStack::PopAndDestroy( finalText );
        CleanupStack::PopAndDestroy( shuffleText );
		CAknIconArray* iconArray = new( ELeave ) CAknIconArray( 1 );
		CleanupStack::PushL( iconArray );
		// Prepare icon array.
		CGulIcon* icon = (*iIconArray)[EMPXClvIconShuffle];
		CGulIcon* iconCopy = CGulIcon::NewL(icon->Bitmap(), icon->Mask());
		iconCopy->SetBitmapsOwnedExternally(ETrue);
		iconArray->AppendL(iconCopy);

		listBox->ItemDrawer()->ColumnData()->SetIconArrayL( iconArray );
		CleanupStack::Pop(); // iconArray
        }

    for ( TInt i = 0; i < songCount; i++ )
        {
        // Just get the exiting item and update the fields + icon.
        CMPXMedia* currentMedia( songArray->AtL( i ) );

        if ( currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ==
            KMPXInvalidItemId )
            {
            break;
            }
        if ( currentMedia->IsSupported( KMPXMediaGeneralTitle ) )
            {
            const TDesC& title = currentMedia->ValueText( KMPXMediaGeneralTitle );
			HBufC* finalText = HBufC::NewLC(title.Length() + 2);
			TPtr ptr = finalText->Des();
            ptr.Append(_L("\t"));
            ptr.Append(title);
            songList->AppendL( *finalText );
        	CleanupStack::PopAndDestroy( finalText );
            }
        }

    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( songList );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( dialog );
    iDialog = dialog;
    TBool play( EFalse );
    GfxTransEffect::Deregister( iDialog );
    GfxTransEffect::Register( iDialog, KAppUidMusicPlayerX, EFalse );
    // Trap ExecuteLD to make sure iDialog always get reset.
    TRAP_IGNORE( play = iDialog->ExecuteLD() );
    iDialog = NULL;
    if( play && iMediaWall )
        {
        iMediaWall->SetFlags( CHgVgMediaWall::EHgVgMediaWallDrawToWindowGC );
        iMediaWall->DrawNow();
        TInt index = listBox->CurrentItemIndex();
        // If item in list was opened, we only play the selected one.
        SaveSelectedAlbumItemL(iSelectedAlbumIndex);
        // Open the selected song of album
        UpdatePathAndOpenL(index);
        }
    else if( !iLayoutSwitch && iMediaWall )
        {
        // Close mediawall "flip animation"
        iMediaWall->StartOpeningAnimationL( EFalse );
        }

    CleanupStack::PopAndDestroy( listBox );
    }


// -----------------------------------------------------------------------------
// Shows the album songs in the tbone list.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ShowAlbumSongsL( const CMPXMedia& aAlbum )
    {

	CMPXMediaArray* albumSongs =
		const_cast<CMPXMediaArray*>(aAlbum.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
	TInt songCount = albumSongs->Count();
	TInt listSize = songCount;
	TInt indexAddition = 0;

	CHgScroller* currentList = CurrentListWidget();
	User::LeaveIfNull( currentList );

	if( currentList->ItemCount() > 1 )
		{
		// There might be shuffle icon in the first item so lets reset it.
		currentList->ItemL(0).SetIcon( NULL );
		}
	if( songCount > 1 )
		{
		listSize++;
		indexAddition = 1;
		}
	// Add space for shuffle item if list contains more than one item.
	currentList->ResizeL( listSize );

	for ( TInt i = 0; i < songCount ; i++ )
		{
		// Just get the exiting item and update the fields + icon.
		CMPXMedia* song( albumSongs->AtL( i ) );

		if ( song->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ==
			KMPXInvalidItemId )
			{
			break;
			}
		CHgItem* item = &currentList->ItemL(i+indexAddition);

		// Just get the exiting item and update the fields + icon.
		SetSongTitleL( item, song );
		// TODO, david has removed this line????
//            AddDetailToDisplayedItemL( item, song, i );
		//David: AddDetailToDisplayedItemL( item, currentMedia, i ); is broken
		// will need to be fixed.
		}

		if( songCount > 1 )
			{
			SetShuffleItemToListL( currentList );
			}
		currentList->SetSelectedIndex( 0 );
		currentList->SetFocus(ETrue, EDrawNow);
		currentList->DrawNow();

    }

// ----------------------------------------------------------------------------
// Save the selected album item
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::SaveSelectedAlbumItemL(TInt aIndex)
    {
    if ( iContext == EContextGroupAlbum ||
         iContext == EContextItemArtist ||
         iContext == EContextItemAlbum )
        {
        const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();

        if ( iSelectedMediaInAlbumView )
            {
            delete iSelectedMediaInAlbumView;
            iSelectedMediaInAlbumView = NULL;
            }

		if ( aIndex < 0 ) // We try to save a valid album otherwise restore will not work.
			{
			aIndex = 0;
			}

        iSelectedMediaInAlbumView = CMPXMedia::NewL( *mediaArray.AtL( aIndex ) );

        if ( iSelectedMediaInAlbumView->IsSupported( KMPXMediaGeneralTitle ) )
            {
            const TDesC& title = iSelectedMediaInAlbumView->ValueText( KMPXMediaGeneralTitle );
            if ( title == KNullDesC )
                {
                // Cannot use reference to KNullDesC, must
                // use the literal directly.
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicAlbum, KNullDesC );
                }
            else
                {
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicAlbum, title );
                }
            }

        if ( iContext == EContextItemArtist )
            {
            const CMPXMedia& containerMedia = iListBoxArray->ContainerMedia();
            const TDesC& artist = containerMedia.ValueText( KMPXMediaGeneralTitle );

            HBufC* unknownArtistText =
                    StringLoader::LoadLC( R_MPX_QTN_NMP_UNKNOWN_ARTIST );

            if ( artist != unknownArtistText->Des() )
                {
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicArtist, artist );
                }
            else
                {
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicArtist, KNullDesC );
                }

            CleanupStack::PopAndDestroy( unknownArtistText );
            }

        iSelectedAlbumHandler->SaveSelectedAlbumL(*iSelectedMediaInAlbumView);
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewHgContainer::RestoreSelectedAlbumItemL
// When view is deactivated, all the current information about album selection
// were lost. So we restore the selected album.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::RestoreSelectedAlbumItemL(
    const CMPXMediaArray& aMediaArray )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::RestoreSelectedAlbumItemL" );

	const CMPXMedia* restoredAlbum = iSelectedAlbumHandler->RestoreSelectedAlbum();
	iRestoredAlbumIndex = 0;
	iSelectedAlbumIndex = 0;
	if ( restoredAlbum )
		{
	    TMPXItemId id=restoredAlbum->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);

	    for ( TInt i = 0; i < aMediaArray.Count() ; i++ )
	        {
	        CMPXMedia* currentMedia( aMediaArray.AtL( i ) );

	        if ( (currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) == id) ||
	             (id.iId1 == 0 && currentMedia->ValueText(KMPXMediaGeneralTitle).Compare( restoredAlbum->ValueText(KMPXMediaGeneralTitle) ) == 0 )  )
	            {
	            iRestoredAlbumIndex = i;
	            iSelectedAlbumIndex = i;
	            break;
	            }
	        }
		}
    }

// ----------------------------------------------------------------------------
// Write the album name, artist name and album art in media data to file
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::WriteToStreamFileL( const CMPXMedia* aMedia )
   {
    MPX_FUNC( "CMPXCollectionViewHgContainer::WriteToStreamFileL" );

    CCoeEnv* coeEnv( iEikonEnv );
    RFs& fs = coeEnv->FsSession();
    fs.MkDirAll( iSelectedAlbumItemFileName );
    TParse parsedName;

    fs.Parse( iSelectedAlbumItemFileName, parsedName );
    CFileStore* store( NULL );
    MPX_TRAPD( error, store = CDirectFileStore::ReplaceL(fs,
                                                 parsedName.FullName(),
                                                 EFileWrite); );

    if ( error == KErrNone )
        {
        CleanupStack::PushL( store );
        store->SetTypeL( KDirectFileStoreLayoutUid );

        RStoreWriteStream wstream;
        TStreamId stream = wstream.CreateLC( *store );
        store->SetRootL( stream );

        aMedia->ExternalizeL(wstream);

        wstream.CommitL();
        CleanupStack::PopAndDestroy( &wstream );
        CleanupStack::PopAndDestroy( store );
        }
    else
        {
        fs.Delete( iSelectedAlbumItemFileName );
        }
   }

// ----------------------------------------------------------------------------
// Read the album name, artist name and album art from file
// ----------------------------------------------------------------------------
void CMPXCollectionViewHgContainer::ReadFromStreamFileL( CMPXMedia* aMedia )
    {
    MPX_FUNC("CMPXCollectionViewHgContainer::ReadFromStreamFileL");

    CCoeEnv* coeEnv( iEikonEnv );
    RFs& fs = coeEnv->FsSession();
    fs.MkDirAll( iSelectedAlbumItemFileName );
    TParse parsedName;

    fs.Parse( iSelectedAlbumItemFileName, parsedName );
    CFileStore* store( NULL );
    MPX_TRAPD( error, store = CDirectFileStore::OpenL(fs,
                                                 parsedName.FullName(),
                                                 EFileRead ); );

    if ( error == KErrNone )
        {
        CleanupStack::PushL( store );

        RStoreReadStream stream;
        stream.OpenLC( *store, store->Root() );
        aMedia->InternalizeL(stream);

        CleanupStack::PopAndDestroy( &stream );
        CleanupStack::PopAndDestroy( store );
        }

    }

void CMPXCollectionViewHgContainer::HandleGainingForeground()
    {
    iIsForeground = ETrue;
    }

void CMPXCollectionViewHgContainer::HandleLosingForeground()
    {
    iIsForeground = EFalse;
    }

TInt CMPXCollectionViewHgContainer::AsyncCallback( TAny* aPtr )
    {
    CMPXCollectionViewHgContainer* self = static_cast<CMPXCollectionViewHgContainer*>(aPtr);
    if( self )
        {
        //check if the pointer to ContainerMedia exists
        const CMPXMedia& media = self->iListBoxArray->ContainerMedia();
        if ( NULL == &media )
    	    {
            return KErrNone;
    	    }


        MPX_TRAPD( err, self->HandleLbxItemAdditionL() );
        if ( err != KErrNone )
        	{
            MPX_DEBUG2("CMPXCollectionViewHgContainer::AsyncCallback() return err%d", err );
            return err;
        	}

        if( self->iCbaHandler )
            {
            self->iCbaHandler->UpdateCba();
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Prepare T-bone view
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::PrepareTboneViewL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::PrepareTboneViewL" );

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    TInt mediaCount( mediaArray.Count() );
    HandleLbxItemRemovalL();

    iPrevContext = EContextGroupAlbum;
    iOpenAlbumTracks = ETrue;
	iContext = EContextItemAlbum;
	iCurrentViewType = EMPXViewTBone;

    if (ShuffleItemPresent())
        iShuffleItem = 1;
    else
    	iShuffleItem = 0;

	TInt listCount = mediaCount + iShuffleItem;
    iThumbnailReqMap.Reset();
    iThumbnailReqMap.ReserveL(listCount);
    for ( TInt i = 0; i < listCount; i++ )
        {
        iThumbnailReqMap.Append( EFalse );
        }

    iThumbnailManager->CancelAll();

	// MediaWall sets the default icon in the construction phase.
	iDefaultIconSet = EFalse;

    if( !iLayoutSwitch  )
        {
        BeginFullScreenAnimation();
        }

	RestoreSelectedAlbumItemL(mediaArray);
	PrepareMediaWallWithListL( mediaArray, mediaCount );

    DrawableWindow()->SetOrdinalPosition( -1 );

    CleanPrevView();

    iPrevViewType = iCurrentViewType;

    if( !iDefaultIconSet )
        {
        SetDefaultIconL();
        }
    // We need to adjust the CBA for this view.
	if( iCbaHandler )
		iCbaHandler->UpdateCba();

	iLayoutSwitch = EFalse;
	}

// -----------------------------------------------------------------------------
// Determine if adding shuffle command is needed
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgContainer::ShuffleItemPresent()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::ShuffleItemNeeded" );

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();

	TBool res = EFalse;
	if ( mediaArray.Count() > 1 &&
	     (iContext == EContextGroupAlbum ||
		 iContext == EContextGroupSong ||
		 iContext == EContextItemPlaylist ||
		 iContext == EContextItemGenre ||
		 iContext == EContextItemSong ))
		{
		res = ETrue;
		}

	return res;
    }

// -----------------------------------------------------------------------------
// Handles translation from list index to media array index
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewHgContainer::MediaIndex(TInt aIndex) const
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::MediaIndex" );

	TInt index(aIndex);

	if ( iShuffleItem && aIndex != KErrNotFound )
		{
		index = aIndex - 1;
		}

	return index;
    }


// ---------------------------------------------------------------------------
// Offer command by view, if handled, return ETrue.
// ---------------------------------------------------------------------------
//
TBool CMPXCollectionViewHgContainer::OfferCommandL( TInt aCommand )
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::OfferCommandL" );

    if( aCommand == EAknSoftkeyBack && iContext == EContextItemAlbum )
        {
		iOpenAlbumTracks = EFalse;
		HandleLbxItemAdditionL();
		return ETrue;
		}
	else
		{
		return EFalse;
		}
    }

// ---------------------------------------------------------------------------
// Load and set empty text
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::LoadAndSetEmptyTextL()
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::LoadAndSetEmptyTextL" );
    MPX_DEBUG2( "CMPXCollectionViewHgContainer::LoadAndSetEmptyTextL - iContext %d", iContext );

    TInt resId( 0 );

    switch ( iContext )
        {
        case EContextGroupAlbum:
            {
            resId = R_MPX_VMP_NO_ALBUMS;
            break;
            }
        case EContextGroupGenre:
        case EContextItemGenre:
            {
            resId = R_MPX_VMP_NO_GENRES;
            break;
            }
        case EContextItemSong:
        case EContextGroupSong:
        case EContextItemPlaylist:
            {
            resId = R_MPX_VMP_NO_SONGS;
            break;
            }
        default:
            {
            MPX_DEBUG1( "CMPXCollectionViewHgContainer::LoadAndSetEmptyTextL - some empty text is missing" );
            return;
            }
        }
    HBufC* emptyText = StringLoader::LoadLC( resId );
    SetLbxEmptyTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );
    }

// ---------------------------------------------------------------------------
// Set previous view Uid
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::SetPreviousViewId(TUid aViewUid)
    {
    MPX_FUNC( "CMPXCollectionViewHgContainer::SetPreviousViewId" );

	iPreviousViewId = aViewUid;
    }

// ---------------------------------------------------------------------------
// Prepare and begin fullscreen animation effects
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::BeginFullScreenAnimation(TBool aPrevViewWasPlayback)
    {
    if( iTranstionType != EMPXTranstionNotDefined )
        return;

    iTranstionType = EMPXTranstionToLeft;

    if ( (iPrevContext == EContextUnknown ||
          iPrevContext == EContextItemAlbum ) && aPrevViewWasPlayback )
        {
        iTranstionType = EMPXTranstionToLeft;
        }
    else if( iPrevContext == EContextUnknown )
        {
        // We aren't coming from playback view and prev context is unknown.
        // Musicplayer is propably started so we shouldn't use any animation.
        iTranstionType = EMPXTranstionNotDefined;
        return;
        }
    else if( iContext == EContextItemAlbum ||
             iContext == EContextItemGenre ||
             iContext == EContextItemPlaylist )
        {
        iTranstionType = EMPXTranstionToRight;
        }

    if( iPrevViewType == EMPXViewMediawall || iPrevViewType == EMPXViewTBone )
        {
        iMediaWall->SetFlags( CHgVgMediaWall::EHgVgMediaWallDrawToWindowGC );
        iMediaWall->DrawNow();
		// workaround for NGA animations: includes Media Wall into transition animation.
		iCoeEnv->WsSession().Finish();
		User::After(1000);
        }

    const TInt flags = AknTransEffect::TParameter::EActivateExplicitCancel;
    TRect appRect = ((CAknAppUi*)iCoeEnv->AppUi())->ApplicationRect();
    GfxTransEffect::BeginFullScreen( iTranstionType, appRect,
            AknTransEffect::EParameterType,
            AknTransEffect::GfxTransParam(KAppUidMusicPlayerX, flags));
    }

// ---------------------------------------------------------------------------
// End fullscreen animation effects
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::EndFullScreenAnimation()
    {
    if( iTranstionType != EMPXTranstionNotDefined )
        {
        GfxTransEffect::EndFullScreen();
        iTranstionType = EMPXTranstionNotDefined;
        // TODO: uncomment if really needed.
        //iThumbnailManager->Pause(TTimeIntervalMicroSeconds32(KThumbLoadingPauseForTransition));
        }
    }

// ---------------------------------------------------------------------------
// Resolving the size of the popup list rectangle.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewHgContainer::ResolvePopupListSizeL()
    {
    // Resolve popuplistbox rect that is used to show the album songs
    CAknSinglePopupMenuStyleListBox* listBox = new ( ELeave ) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );

    CMpxPopupList* dialog = CMpxPopupList::NewL(listBox, R_MPX_COLLECTION_ALBUMSONGS_LIST_CBA,
            AknPopupLayouts::EDynMenuWindow );

    CleanupStack::PushL( dialog );

    listBox->ConstructL( dialog,
            EAknListBoxSelectionList | EAknListBoxScrollBarSizeExcluded  );

    // title can be hardcoded because it is not shown to user. Just for the calculations.
    dialog->SetTitleL(_L("Foo"));
    iPopupListRect = dialog->LayoutRect();

    CleanupStack::PopAndDestroy( dialog );
    CleanupStack::PopAndDestroy( listBox );
    }
//  End of File
