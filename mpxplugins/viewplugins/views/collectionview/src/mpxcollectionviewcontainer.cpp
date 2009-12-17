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
#include <hlplch.h>
#include <aknappui.h>
#include <akncontext.h>
#include <AknsUtils.h>  // AknsUtils
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>
#include <eikspane.h>
#include <eiktxlbx.h>
#include <mpxcollectionview.mbg>
#include <mpxplaybackframeworkdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmedia.h>
#include <mpxconstants.h>
#include <mpxlog.h>
#include <eikon.hrh>
#include <mpxuser.h>

#include "mpxcollectionview.hrh"
#include "mpxcollectionviewlistboxarray.h"
#include "mpxcollectionview.hlp.hrh"
#include "mpxcollectionviewcontainer.h"

// CONSTANTS
_LIT( KMPXCollectionViewIconFile, "mpxcollectionview.mbm" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewContainer* CMPXCollectionViewContainer::NewL(
    MEikCommandObserver* aView,
    MEikListBoxObserver* aObserver,
    TBool aIsEmbedded)
    {
    MPX_FUNC( "CMPXCollectionViewContainer::NewL" );
    CMPXCollectionViewContainer* self =
        new ( ELeave ) CMPXCollectionViewContainer(aView, aObserver, aIsEmbedded);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewContainer::CMPXCollectionViewContainer(
    MEikCommandObserver* aView,
    MEikListBoxObserver* aObserver,
    TBool aIsEmbedded) 
    : iView( aView )
    , iListBoxObserver( aObserver )
    , iReady( EFalse )
    , iIsEmbedded(aIsEmbedded)    
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCollectionViewContainer::~CMPXCollectionViewContainer()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::~CMPXCollectionViewContainer" );
    delete iCommonContainer;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::ConstructL()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::ConstructL" );
    iCommonContainer = CMPXCommonContainerFactory::NewL();
    iCommonContainer->SetViewContainer( this );
    iCommonContainer->SetCommandObserver( iView );
    iCommonContainer->SetListBoxObserver( iListBoxObserver );
    iCommonContainer->ConstructContainerL( MMPXCommonContainer::EMPXCommonContainerMusic );
    iLbxArray = static_cast<CMPXCollectionViewListBoxArray*>(
        CreateListBoxArrayL() );
    iCommonContainer->SetListBoxArrayL( iLbxArray );
    iCommonContainer->HandleListBoxArrayEventL(
        MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventIconArrayChange );
    }

// ---------------------------------------------------------------------------
// Sets playback status
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::SetPlaybackStatusByIdL(
    TMPXItemId aId, TMPXPlaybackState aStatus, TInt aIndex)
    {
    MPX_FUNC( "CMPXCollectionViewContainer::SetPlaybackStatusByIdL" );
    if ( iLbxArray && iCommonContainer )
        {
        TInt index( iLbxArray->SetPlaybackStatusByIdL( aId, aStatus, aIndex ) );
        if ( index != KErrNotFound )
            {
            // Update the previous item as not playing anymore
            iCommonContainer->DrawLbxItem( index );
            }
        index = iLbxArray->GetPlaybackIndex();
        if ( index != KErrNotFound )
            {
            // Update new item as playing
            iCommonContainer->DrawLbxItem( index );
            }
        }
    }

// ---------------------------------------------------------------------------
// Get playback index
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewContainer::PlaybackIndex()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::PlaybackIndex" );
    return ( iLbxArray ? iLbxArray->GetPlaybackIndex() : KErrNotFound );
    }

// ---------------------------------------------------------------------------
// Activate reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::ActivateReorderMode( TBool aEnable )
    {
    MPX_FUNC( "CMPXCollectionViewContainer::ActivateReorderMode" );
    if ( iLbxArray )
        {
        iReorder = aEnable;
        iLbxArray->ActivateReorderMode( iReorder );
        }
    }

// ---------------------------------------------------------------------------
// Set reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::SetReorderGrabbedMode(
    TBool aEnable, TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewContainer::SetReorderGrabbedMode" );
    if ( iLbxArray )
        {
        iLbxArray->SetReorderGrabbedMode( aEnable, aIndex );
        }
    }

// ---------------------------------------------------------------------------
// Determines if it's currently in reorder mode
// ---------------------------------------------------------------------------
//
TBool CMPXCollectionViewContainer::IsInReorderMode()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::IsInReorderMode" );
    return iReorder;
    }

// ---------------------------------------------------------------------------
// Move current reorder index
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::MoveReorderIndex( TInt aDirection )
    {
    MPX_FUNC( "CMPXCollectionViewContainer::MoveReorderIndex" );
    if ( iLbxArray )
        {
        iLbxArray->MoveReorderIndex( aDirection );
        }
    }

// ---------------------------------------------------------------------------
// set current reorder index
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::SetReorderIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewContainer::SetReorderIndex" );
    if ( iLbxArray )
        {
        iLbxArray->SetReorderIndex( aIndex );
        }
    }

// ---------------------------------------------------------------------------
// Confirm reorder
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::ConfirmReorderL()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::ConfirmReorderL" );
    if ( iLbxArray )
        {
        iLbxArray->ConfirmReorderL();
        }
    }

// ---------------------------------------------------------------------------
// Cancel reorder
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::CancelReorder()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::CancelReorder" );
    TInt originalIndex( GetOriginalIndex() );
    if ( iLbxArray )
        {
        iLbxArray->CancelReorder();
        }
    iCommonContainer->SetLbxCurrentItemIndexAndDraw( originalIndex );
    }

// ---------------------------------------------------------------------------
// Gets original index of currently selected item
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewContainer::GetOriginalIndex()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::GetOriginalIndex" );
    TInt index( KErrNotFound );
    if ( iLbxArray )
        {
        index = iLbxArray->GetOriginalIndex();
        }
    return index;
    }

// -----------------------------------------------------------------------------
// Update Titlepane icon to reorder image
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::UpdateReorderTitleIconL()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::UpdateReorderTitleIconL" );
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
        EMbmMpxcollectionviewQgn_graf_mup_ctx_reordersong,
        EMbmMpxcollectionviewQgn_graf_mup_ctx_reordersong_mask );

    CEikImage* newIcon = new ( ELeave ) CEikImage();
    CleanupStack::PushL( newIcon );
    newIcon->SetPicture( bitmap, mask );
    newIcon->SetPictureOwnedExternally( EFalse );
    iContextPane = static_cast<CAknContextPane*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidContext ) ) );
    iOrigIcon = iContextPane->SwapPicture( newIcon );
    CleanupStack::Pop( 3 ); // bitmap, mask, newIcon
    }

// -----------------------------------------------------------------------------
// Update Titlepane icon to use original image
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::RestoreOriginalTitleIconL()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::RestoreOriginalTitleIconL" );
    if ( iOrigIcon )
        {
        CEikImage* newIcon = iContextPane->SwapPicture( iOrigIcon );
        delete newIcon;
        }
    }

// -----------------------------------------------------------------------------
// Creates listbox array
// -----------------------------------------------------------------------------
//
CMPXCommonListBoxArrayBase* CMPXCollectionViewContainer::CreateListBoxArrayL()
    {
    MPX_FUNC( "CMPXCollectionViewContainer::CreateListBoxArrayL" );
    CMPXCollectionViewListBoxArray* array = CMPXCollectionViewListBoxArray::NewL();
    array->ConstructListBoxArrayL();
    array->SetObserver( this );
    return array;
    }

// -----------------------------------------------------------------------------
// Returns common container
// -----------------------------------------------------------------------------
//
MMPXCommonContainer* CMPXCollectionViewContainer::Common()
    {
    ASSERT( iCommonContainer );
    return iCommonContainer;
    }

// ---------------------------------------------------------------------------
// From MMPXViewContainer
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCollectionViewContainer::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    ASSERT( iCommonContainer );
    MPX_DEBUG4( "CMPXCollectionViewContainer::HandleKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
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
        const CMPXMedia& containerMedia(
            iCommonContainer->ListBoxArray()->ContainerMedia() );
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
            response = iCommonContainer->HandleKeyEventL( aKeyEvent, aType );
            }
        }
    else
        {
        response = iCommonContainer->HandleKeyEventL( aKeyEvent, aType );
        MPX_DEBUG2( "CMPXCollectionViewContainer::OfferKeyEventL commoncontainer offerkey res = %d", response);
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
// From MMPXViewContainer
// Handle the retrieval of the view container's help context.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::HandleHelpContext(
    TCoeHelpContext& aContext ) const
    {
    ASSERT( iCommonContainer );
    MPX_FUNC( "CMPXCollectionViewContainer::HandleHelpContext" );
    aContext.iMajor = KAppUidMusicPlayerX;

    const CMPXMedia& containerMedia(
        iCommonContainer->ListBoxArray()->ContainerMedia() );
    TMPXGeneralType containerType(
        containerMedia.ValueTObjectL<TMPXGeneralType>(
            KMPXMediaGeneralType ) );
    TMPXGeneralCategory containerCategory(
        containerMedia.ValueTObjectL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory ) );
    MPX_DEBUG3( "CMPXCollectionViewContainer::HandleHelpContext containertype = %d, containercategory = %d", containerType, containerCategory );

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
// From MMPXViewContainer
// Returns the indicators for the specified item within the view container
// ---------------------------------------------------------------------------
//
RArray<TInt> CMPXCollectionViewContainer::IndicatorsL( TInt aIndex )
    {
    return iLbxArray->IndicatorIconIndicesL( aIndex );
    }

// ---------------------------------------------------------------------------
// From MMPXCommonListBoxArrayObserver
// Handle listbox array events.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::HandleListBoxArrayEventL(
    TMPXCommonListBoxArrayEvents aEvent )
    {
    MPX_FUNC( "CMPXCollectionViewContainer::HandleListBoxArrayEventL" );
    ASSERT( iCommonContainer );
    iCommonContainer->HandleListBoxArrayEventL( aEvent );
    }

// ---------------------------------------------------------------------------
// Content is ready.
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewContainer::ContentIsReady( TBool aReady )
    {
    iReady = aReady;
    }

//  End of File
