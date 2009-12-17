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
* Description:  Listbox array for collection view container
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <aknconsts.h>
#include <AknIconArray.h>
#include <avkon.mbg>
#include <f32file.h>
#include <data_caging_path_literals.hrh>
#include <mpxmedia.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediaarray.h>
#include <mpxlog.h>
#include <mpxcollectionviewhg.mbg>
#include <mpxcollectionpath.h>  // TMPXItemId
#include <mpxplaybackframeworkdefs.h>
#include <mpxcollectionviewhg.rsg>
#include <mpxuser.h>

#include <mpxcommonuihelper.h>
#include "mpxcollectionviewhg.hrh"
#include "mpxcommoncontainer.hrh"
#include "mpxcommonlistboxarrayobserver.h"
#include "mpxcollectionviewhglistboxarray.h"

// CONSTANTS
_LIT( KMPXCommonListboxFormatString, "%d\t%S" );
_LIT( KMPXTab, "\t" );
_LIT( KMPXCollectionViewIconFile, "mpxcollectionviewhg.mbm" );

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXCollectionViewListBoxArray* CMPXCollectionViewListBoxArray::NewL()
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::NewL" );
    CMPXCollectionViewListBoxArray* self =
        new ( ELeave ) CMPXCollectionViewListBoxArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewListBoxArray::CMPXCollectionViewListBoxArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXCollectionViewListBoxArray::CMPXCollectionViewListBoxArray() :
    CMPXCommonListBoxArrayBase()
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CMPXCollectionViewListBoxArray::~CMPXCollectionViewListBoxArray()
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::~CMPXCollectionViewListBoxArray" );
    delete iUnknown;
    delete iMediaOwned;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::ConstructL()
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::ConstructL" );
    iUnknown = StringLoader::LoadL(R_MPX_QTN_MP_UNKNOWN );
    iMMCDrive = CMPXCommonUiHelper::MMCDriveNumber();
    }

// ---------------------------------------------------------------------------
// Sets playback status
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewListBoxArray::SetPlaybackStatusByIdL(
    TMPXItemId aId, TMPXPlaybackState aStatus, TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::SetPlaybackStatusByIdL" );
    // fix crash when click back too quick after play a song
    // aIndex < 0 means no icon to place, iMedia->Count() == 0 means no song, don't search
    if ( iMedia == NULL || aIndex < 0 || iMedia->Count() == 0)
        {
        return SetPlaybackStatusByIndex( KErrNotFound, aStatus );
        }

    TInt index( KErrNotFound );
    TInt count( iMedia->Count() );
    // Search start from aIndex, then aIndex-1, aIndex+1, aIndex-2, aIndex+2
    // until the boundary reach then search for the rest
    if ( aIndex > count - 1 ) // remove song before the last
        {
        aIndex = count - 1;
        }
    CMPXMedia* origMedia( iMedia->AtL( aIndex ) );
    TMPXItemId id( origMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    if ( id == aId )
        {
        index = aIndex;
        }
    else
        {
        TInt upBound( ( count - 1 ) - aIndex );
        TInt loBound( aIndex - 0 );
        TInt biBound( Min( upBound, loBound ) );
        for ( TInt i = 1; i <= biBound; i++ )
            {
            CMPXMedia* entry( iMedia->AtL( aIndex - i ) );
            TMPXItemId id( entry->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            if ( id == aId )
                {
                index = aIndex - i;
                break;
                }

            entry = iMedia->AtL( aIndex + i );
            id = entry->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            if ( id == aId )
                {
                index = aIndex + i;
                break;
                }
            }
        if ( index == KErrNotFound && upBound != loBound ) // still not found search for the rest
            {
            TInt begin( ( loBound > upBound ) ? 0 : aIndex + biBound + 1 );
            TInt end( ( loBound>upBound ) ? aIndex - biBound : count );
            for ( TInt i = begin; i < end; i++ )
                {
                CMPXMedia* entry( iMedia->AtL( i ) );
                TMPXItemId id( entry->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                if ( id == aId )
                    {
                    index = i;
                    break;
                    }
                }
            }
        }
    return SetPlaybackStatusByIndex( index, aStatus );
    }

// ---------------------------------------------------------------------------
// Activates reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::ActivateReorderMode( TBool aEnable )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::ActivateReorderMode" );
    iReorderActivated = aEnable;
    if ( !iMediaOwned && aEnable )
        {
        // make a copy of the current array so that any modification to the array
        // does not pollute the master copy
        MPX_TRAPD( error, iMediaOwned = CMPXMediaArray::NewL() );
        if ( error == KErrNone )
            {
            TInt count( iMedia->Count() );
            TRAP_IGNORE(
                for ( TInt i = 0; i < count; i++ )
                    {
                    CMPXMedia* entry( iMedia->AtL( i ) );
                    iMediaOwned->AppendL( *entry );
                    } );
            // no need to delete iMedia, we don't own it
            // iMedia might get reset before we have a chance to delete it
            // keep a copy of the pointer so that we can delete
            iMedia = iMediaOwned;
            }
        }
    }

// ---------------------------------------------------------------------------
// Set reorder mode
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::SetReorderGrabbedMode(
    TBool aEnable, TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::SetReorderGrabbedMode" );
    iReorder = aEnable;
    iOriginalIndex = aIndex;
    iMovedIndex = aIndex;
    if ( aEnable && aIndex == iIndex )
        {
        // if the item currently being moved is now playing
        // manually move the icon after it's complete, and before the result
        // is saved to db
        iMoveNowPlaying = ETrue;
        }
    else
        {
        iMoveNowPlaying = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Move current reorder index
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::MoveReorderIndex( TInt aDirection )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::MoveReorderIndex" );
    if ( iReorder )
        {
        if ( ( aDirection == 1 ) && ( iMovedIndex < MdcaCount() - 1 ) )
            {
            iMovedIndex++;
            }
        else if ( ( aDirection == -1 ) && ( iMovedIndex > 0 ) )
            {
            iMovedIndex--;
            }
        }
    }

// ---------------------------------------------------------------------------
// Set current reorder index
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::SetReorderIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::SetReorderIndex" );
    iMovedIndex = aIndex;
    }

// ---------------------------------------------------------------------------
// Confirm reorder
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::ConfirmReorderL()
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::ConfirmReorderL" );
    CMPXMedia* origMedia( iMedia->AtL( iOriginalIndex ) );
    CMPXMedia* entry( CMPXMedia::NewL( *origMedia ) );
    iMedia->Remove( iOriginalIndex );
    iMedia->Insert( entry, iMovedIndex ); // iMedia takes ownership
    if ( iMoveNowPlaying )
        {
        iIndex = iMovedIndex;
        iMoveNowPlaying = EFalse;
        }
    iOriginalIndex = KErrNotFound;
    iMovedIndex = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Cancel reorder
// ---------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::CancelReorder()
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::CancelReorder" );
    iOriginalIndex = KErrNotFound;
    iMovedIndex = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Gets original index of currently selected item
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewListBoxArray::GetOriginalIndex() const
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::GetOriginalIndex" );
    return iOriginalIndex;
    }

// ---------------------------------------------------------------------------
// Gets the index for the song that's currently playing
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewListBoxArray::GetPlaybackIndex() const
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::GetPlaybackIndex" );
    return iIndex;
    }

// -----------------------------------------------------------------------------
// Indicates if Unknown entry exists in this view
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewListBoxArray::IsCategoryUnknownExist() const
    {
    return iIsUnknownExist;
    }

// -----------------------------------------------------------------------------
// Checks if the current item is a broken link
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewListBoxArray::IsItemBrokenLinkL( TInt aIndex ) const
    {
    TBool ret( EFalse );
    if ( iMedia )
        {
        CMPXMedia* entry( iMedia->AtL( aIndex ) );
        TUint flags( 0 );
        if( entry->IsSupported( KMPXMediaGeneralFlags ) )
            {
            flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
            ret = ( ( flags ) & ( KMPXMediaGeneralFlagsIsInvalid ) );
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// Checks if the current item is corrupted
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewListBoxArray::IsItemCorruptedL( TInt aIndex ) const
    {
    TBool ret( EFalse );
    if ( iMedia )
        {
        CMPXMedia* entry( iMedia->AtL( aIndex ) );
        TUint flags( 0 );
        if( entry->IsSupported( KMPXMediaGeneralFlags ) )
            {
            flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
            ret = ( ( flags ) & ( KMPXMediaGeneralFlagsIsCorrupted ) );
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewListBoxArray::IndicationIconIndices
// Get array of indicator icon indices at the specified view index
// Only used when using Hitchcock container.
// -----------------------------------------------------------------------------
//
RArray<TInt> CMPXCollectionViewListBoxArray::IndicatorIconIndicesL(
    TInt aIndex )
    {
    RArray<TInt> iconIndices;

    TInt index( GetArrayIndexForListBoxIndex( aIndex ) );
    CMPXMedia* entry( iMedia->AtL( index ) );

    TMPXGeneralType type( EMPXNoType );
    if ( entry->IsSupported( KMPXMediaGeneralType ) )
        {
        type = entry->ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
        }
    TMPXGeneralCategory category( EMPXNoCategory );
    if ( entry->IsSupported( KMPXMediaGeneralCategory ) )
        {
        category = entry->ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
        }
    TUint flags(0);
    if ( entry->IsSupported( KMPXMediaGeneralFlags ) )
        {
        flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
        }

    // reorder icon
    if ( ( iReorder ) && ( aIndex == iMovedIndex ) )
        {
        iconIndices.Append( GetReorderIcon() );
        }
    else if ( !iReorderActivated )
        {
        // playback status icon
        if ( iIndex == index )
            {
            switch ( iPbState )
                {
                case EPbStatePlaying:
                    {
                    iconIndices.Append( EMPXClvIconPlay );
                    break;
                    }
                case EPbStatePaused:
                    {
                    iconIndices.Append( EMPXClvIconPause );
                    break;
                    }
                default:
                    {
                    // other playback states, not handled
                    break;
                    }
                }
            }
        }

    if ( category == EMPXSong && type == EMPXItem )
        {
        if ( flags & KMPXMediaGeneralFlagsIsInvalid )
            {
            iconIndices.Append( EMPXClvIconBrokenTrack );
            }
        else if ( flags & KMPXMediaGeneralFlagsIsCorrupted )
            {
            iconIndices.Append( EMPXClvIconCorruptTrack );
            }
        else if ( flags & KMPXMediaGeneralFlagsIsDrmLicenceInvalid )
            {
            iconIndices.Append( EMPXClvIconDrmExpired );
            }
        }
    else if ( category == EMPXPlaylist )
        {
        if ( flags & KMPXMediaGeneralFlagsIsInvalid )
            {
            iconIndices.Append( EMPXClvIconBrokenPlaylist );
            }
        }

    TInt driveNum = ( ( flags ) & ( KMPXMediaGeneralFlagsDriveInfo ) );
    if ( driveNum == iMMCDrive )
        {
        // The MMC icon is displayed in the first Indicator slot
        iconIndices.Append( EMPXClvIconMMC );
        }

    return iconIndices;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewListBoxArray::MdcaPoint
// indexes into a descriptor array.
// -----------------------------------------------------------------------------
//
TPtrC CMPXCollectionViewListBoxArray::MdcaPoint( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXCollectionViewListBoxArray::MdcaPoint" );
    //MPX_DEBUG2( "CMPXCollectionViewListBoxArray::MdcaPoint media count %d", iMedia->Count() );
    TPtrC item( KNullDesC );
    TRAP_IGNORE( item.Set( GetMdcaPointL( aIndex ) ) );
    return item;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewListBoxArray::CreateIconArrayL
// creates icon array.
// -----------------------------------------------------------------------------
//
CAknIconArray* CMPXCollectionViewListBoxArray::CreateIconArrayL()
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::CreateIconArrayL" );
    TParse mbmFileName;
    mbmFileName.Set( KMPXCollectionViewIconFile,
                     &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( mbmFileName.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
    // Prepare icon array for listbox
    CAknIconArray* iconArray = new ( ELeave ) CAknIconArray( 27 ); // magic: array granularity
#else
    // Prepare icon array for listbox
    CAknIconArray* iconArray = new ( ELeave ) CAknIconArray( 26 ); // magic: array granularity
#endif

    // Update the enums when this list is updated
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMarkedAdd,
        KAknsIIDNone,
        EAknsMinorNone,
        KAvkonBitmapFile,
        EMbmAvkonQgn_indi_marked_add,
        EMbmAvkonQgn_indi_marked_add_mask);
   AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropEmpty,
        KAknsIIDNone,
        EAknsMinorNone,
        KAvkonBitmapFile,
        EMbmAvkonQgn_prop_empty,
        EMbmAvkonQgn_prop_empty_mask);
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupSongs,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_songs,
        EMbmMpxcollectionviewhgQgn_prop_mup_songs_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupArtist,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_artist,
        EMbmMpxcollectionviewhgQgn_prop_mup_artist_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupAlbum,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_album,
        EMbmMpxcollectionviewhgQgn_prop_mup_album_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupPlaylist,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_playlist,
        EMbmMpxcollectionviewhgQgn_prop_mup_playlist_mask);
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupPlaylist,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_userplayl,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_userplayl_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupPlaylistAuto,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_playlist_auto,
        EMbmMpxcollectionviewhgQgn_prop_mup_playlist_auto_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupGenre,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_genre,
        EMbmMpxcollectionviewhgQgn_prop_mup_genre_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupComposer,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_composer,
        EMbmMpxcollectionviewhgQgn_prop_mup_composer_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnMenuFolderApps,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_menu_folder_apps,
        EMbmMpxcollectionviewhgQgn_menu_folder_apps_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropMupAudio,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_mup_audio,
        EMbmMpxcollectionviewhgQgn_prop_mup_audio_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_corrupttrack,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_corrupttrack_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_brokentrack,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_brokentrack_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_brokenplayl,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_brokenplayl_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_drm_exp,
        EMbmMpxcollectionviewhgQgn_graf_mup_lst_drm_exp_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropUnknown,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_prop_unknown,
        EMbmMpxcollectionviewhgQgn_prop_unknown_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_indi_mmc_add,
        EMbmMpxcollectionviewhgQgn_indi_mmc_add_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupPlay,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_npv_icon_play,
        EMbmMpxcollectionviewhgQgn_graf_mup_npv_icon_play_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupPause,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_npv_icon_pause,
        EMbmMpxcollectionviewhgQgn_graf_mup_npv_icon_pause_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupForwAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_indi_mup_forw_add,
        EMbmMpxcollectionviewhgQgn_indi_mup_forw_add_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupRewAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_indi_mup_rew_add,
        EMbmMpxcollectionviewhgQgn_indi_mup_rew_add_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_adtl_reorder_down,
        EMbmMpxcollectionviewhgQgn_graf_mup_adtl_reorder_down_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_adtl_reorder_up,
        EMbmMpxcollectionviewhgQgn_graf_mup_adtl_reorder_up_mask );
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_adtl_reorder_updown,
        EMbmMpxcollectionviewhgQgn_graf_mup_adtl_reorder_updown_mask );
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupDlstPodcast,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_dlst_podcast,
        EMbmMpxcollectionviewhgQgn_graf_mup_dlst_podcast_mask );
#endif
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupRandom,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxcollectionviewhgQgn_graf_mup_npv_icon_shuffle,
        EMbmMpxcollectionviewhgQgn_graf_mup_npv_icon_shuffle_mask );

    return iconArray;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewListBoxArray::AppendMediaL
// Appends media array to the current array
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::AppendMediaL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::AppendMediaL" );
    if ( iMediaOwned )
        {
        delete iMediaOwned;
        iMediaOwned = NULL;
        }
    CMPXCommonListBoxArrayBase::AppendMediaL( aMedia );
    iIsUnknownExist = EFalse;

    if ( iMedia )
        {
        TInt entriesCount( iMedia->Count() );
        if ( entriesCount > 0 )
            {
            // check if the last entry is "unknown" entry
            // if the last entry has a length of 0, then it's
            // "unknown" category and needs to be localized
            CMPXMedia* origMedia( iMedia->AtL( entriesCount - 1 ) );
            if ( origMedia->ValueText(
                KMPXMediaGeneralTitle ).Length() == 0 )
                {
                iIsUnknownExist = ETrue;
                }
            }
        }
    if ( iContainerMedia )
        {
        if ( iContainerMedia->IsSupported( KMPXMediaGeneralTitle ) )
            {
            if ( iContainerMedia->ValueText(
                KMPXMediaGeneralTitle ).Length() == 0 )
                {
                TMPXGeneralCategory containerCategory(
                    iContainerMedia->ValueTObjectL<TMPXGeneralCategory>(
                        KMPXMediaGeneralCategory ) );
                HBufC* text( NULL );
                switch ( containerCategory )
                    {
                    case EMPXArtist:
                        {
                        text = StringLoader::LoadLC(
                            R_MPX_QTN_NMP_UNKNOWN_ARTIST );
                        break;
                        }
                    case EMPXAlbum:
                        {
                        text = StringLoader::LoadLC(
                            R_MPX_QTN_MUS_TITLE_UNKNOWN_ALBUM );
                        break;
                        }
                    case EMPXGenre:
                        {
                        text = StringLoader::LoadLC(
                            R_MPX_QTN_MUS_TITLE_UNKNOWN_GENRE );
                        break;
                        }
                    case EMPXComposer:
                        {
                        text = StringLoader::LoadLC(
                            R_MPX_QTN_MUS_TITLE_UNKNOWN_COMPOSER );
                        break;
                        }
                    default:
                        {
                        text = StringLoader::LoadLC(
                            R_MPX_QTN_MP_UNKNOWN );
                        break;
                        }
                    }
                iContainerMedia->SetTextValueL(
                    KMPXMediaGeneralTitle, *text );
                CleanupStack::PopAndDestroy( text );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Append icon to array
// -----------------------------------------------------------------------------
//
void CMPXCollectionViewListBoxArray::AppendColorIconToArrayL(
    CAknIconArray* aArray,
    const TAknsItemID& aID,
    const TAknsItemID& aColorId,
    TInt aColorIndex,
    const TDesC& aMbmFile,
    TInt aBitmapId,
    TInt aMaskId )
    {
    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );

    CFbsBitmap* bitmap( NULL );
    CFbsBitmap* mask( NULL );

    if ( aColorId == KAknsIIDNone )
        {
        // do not use theme color, use the default color from the file
        AknsUtils::CreateIconLC( skin, aID,
            bitmap, mask, aMbmFile, aBitmapId, aMaskId );
        }
    else
        {
        // use theme color
        AknsUtils::CreateColorIconLC( skin, aID, aColorId, aColorIndex,
            bitmap, mask, aMbmFile, aBitmapId, aMaskId, KRgbBlack );
        }

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop( 2 ); // mask, bitmap

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );
    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop( icon );
    }

// -----------------------------------------------------------------------------
// Maps list box index to array index in reorder mode
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionViewListBoxArray::GetArrayIndexForListBoxIndex(
    TInt aIndex ) const
    {
    TInt index( aIndex );

    if ( ( iReorder ) && ( iOriginalIndex != iMovedIndex ) )
        {
        if ( iMovedIndex > iOriginalIndex )
            {
            // moved down
            if ( aIndex < iOriginalIndex )
                {
                // do nothing
                }
            else if ( aIndex > iMovedIndex )
                {
                // do nothing
                }
            else if ( aIndex < iMovedIndex )
                {
                // between original index and moved index
                index = aIndex + 1;
                }
            else if ( aIndex == iMovedIndex )
                {
                index = iOriginalIndex;
                }
            }
        else
            {
            // moved up
            if ( aIndex > iOriginalIndex )
                {
                // do nothing
                }
            else if ( aIndex < iMovedIndex )
                {
                // do nothing
                }
            else if ( aIndex > iMovedIndex )
                {
                // between original index and moved index
                index = aIndex - 1;
                }
            else if ( aIndex == iMovedIndex )
                {
                index = iOriginalIndex;
                }
            }
        }

    return index;
    }

// -----------------------------------------------------------------------------
// Get reorder icon
// -----------------------------------------------------------------------------
//
TBool CMPXCollectionViewListBoxArray::GetReorderIcon() const
    {
    TInt icon( EMPXClvIconNone );

    // Determine the icon to display
    if ( iReorder )
        {
        if ( iMovedIndex == 0 )
            {
            icon = EMPXClvIconReorderDown;
            }
        else if ( MdcaCount() - 1 == iMovedIndex )
            {
            icon = EMPXClvIconReorderUp;
            }
        else
            {
            icon = EMPXClvIconReorderUpDown;
            }
        }
    return icon;
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewListBoxArray::GetMdcaPointL
// indexes into a descriptor array.
// -----------------------------------------------------------------------------
//
TPtrC CMPXCollectionViewListBoxArray::GetMdcaPointL( TInt aIndex ) const
    {
    TInt index( GetArrayIndexForListBoxIndex( aIndex ) );
    TPtrC item( KNullDesC );
    CMPXMedia* entry( iMedia->AtL( index ) );
    if( entry->IsSupported( KMPXMediaGeneralTitle ) )
        {
        TPtrC title( entry->ValueText( KMPXMediaGeneralTitle ) );
        if ( IsCategoryUnknownExist() && aIndex== MdcaCount() - 1 && title.Length()==0 )
            {
            title.Set( *iUnknown );
            }

        TMPXGeneralType type( EMPXNoType );
        if( entry->IsSupported( KMPXMediaGeneralType ) )
            {
            type = entry->ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
            }
        TMPXGeneralCategory category( EMPXNoCategory );
        if( entry->IsSupported( KMPXMediaGeneralCategory ) )
            {
            category = entry->ValueTObjectL<TMPXGeneralCategory>(
                        KMPXMediaGeneralCategory );
            }
        TUint flags(0);
        if( entry->IsSupported( KMPXMediaGeneralFlags ) )
            {
            flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
            }
        //MPX_DEBUG3( "CMPXCollectionViewListBoxArray::MdcaPoint (%d) Name: %S", aIndex, &title );
        //MPX_DEBUG4( "CMPXCollectionViewListBoxArray::MdcaPoint (%d) Type: %d, Category: %d", aIndex, type, category );
        TInt iconID( EMPXClvIconNone );
        switch( category )
            {
            case EMPXSong:
                {
                if ( type == EMPXGroup )
                    {
                    iconID = EMPXClvIconSongs;
                    }
                else
                    {
                    TUint flags(0);
                    if( entry->IsSupported( KMPXMediaGeneralFlags ) )
                        {
                        flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
                        }
                    if ( flags & KMPXMediaGeneralFlagsIsInvalid )
                        {
                        iconID = EMPXClvIconBrokenTrack;
                        }
                    else if ( flags & KMPXMediaGeneralFlagsIsCorrupted )
                        {
                        iconID = EMPXClvIconCorruptTrack;
                        }
                    else if ( flags & KMPXMediaGeneralFlagsIsDrmLicenceInvalid )
                        {
                        iconID = EMPXClvIconDrmExpired;
                        }
                    else
                        {
                        iconID = EMPXClvIconAudio;
                        }
                    }
                break;
                }
            case EMPXArtist:
                {
                iconID = EMPXClvIconArtist;
                break;
                }
            case EMPXAlbum:
                {
                iconID = EMPXClvIconAlbum;
                if ( entry->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ==
                    iContainerMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) )
                    {
                    // if id == containerid, "all" under artist/album
                    iconID = EMPXClvIconSongs;
                    }
                break;
                }
            case EMPXPlaylist:
                {
                if ( flags & KMPXMediaGeneralFlagsIsInvalid )
                    {
                    iconID = EMPXClvIconBrokenPlaylist;
                    }
                else
                    {
                    if ( type == EMPXGroup )
                        {
                        iconID = EMPXClvIconPlaylist;
                        }
                    else
                        {
                        iconID = EMPXClvIconUserPlaylist;
                        }
                    if ( type == EMPXItem )
                        {
                        if ( entry->IsSupported(
                            KMPXMediaGeneralNonPermissibleActions ) )
                            {
                            // check for auto playlist and display a different icon
                            TMPXGeneralNonPermissibleActions attr(
                                entry->ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                                    KMPXMediaGeneralNonPermissibleActions ) );
                            if ( attr & EMPXWrite )
                                {
                                iconID = EMPXClvIconPlaylistAuto;
                                }
                            }
                        }
                    }
                break;
                }
            case EMPXGenre:
                {
                iconID = EMPXClvIconGenre;
                break;
                }
            case EMPXComposer:
                {
                iconID = EMPXClvIconComposer;
                break;
                }
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
            case EMPXPodcast:
                {
                iconID = EMPXClvIconPodcasts;
                break;
                }
#endif
            case EMPXFile:
                {
                // a group of files is a folder
                iconID = EMPXClvIconFolder;
                break;
                }

            default:
                {
                // unknown folder icon
                iconID = EMPXClvIconUnknowFile;
                break;
                }
            }
        TPtr ptr = iItem->Des();
        ptr.Zero();

        ptr.Format( KMPXCommonListboxFormatString, iconID, &title );

        iconID = EMPXClvIconNone;
        // reorder icon
        if ( ( iReorder ) && ( aIndex == iMovedIndex ) )
            {
            iconID = GetReorderIcon();
            }
        else if ( !iReorderActivated )
            {
            // playback status icon
            if ( iIndex == index )
                {
                switch ( iPbState )
                    {
                    case EPbStatePlaying:
                        {
                        iconID = EMPXClvIconPlay;
                        break;
                        }
                    case EPbStatePaused:
                        {
                        iconID = EMPXClvIconPause;
                        break;
                        }
                    case EPbStateSeekingForward:
                        {
                        iconID = EMPXClvIconForward;
                        break;
                        }
                    case EPbStateSeekingBackward:
                        {
                        iconID = EMPXClvIconRewind;
                        break;
                        }
                    default:
                        {
                        // other playback states, not handled
                        break;
                        }
                    }
                }
            }
        else
            {
            // Do nothing
            }

        if ( iconID != EMPXClvIconNone )
            {
            ptr.Append( KMPXTab );
            ptr.AppendNum( iconID );
            }

        // mmc icon
        TInt driveNum = ( ( flags ) & ( KMPXMediaGeneralFlagsDriveInfo ) );
        if ( driveNum == iMMCDrive )
            {
            ptr.Append( KMPXTab );
            ptr.AppendNum( EMPXClvIconMMC );
            }

        item.Set( *iItem );
        }
    return item;
    }

// ---------------------------------------------------------------------------
// Sets playback status
// ---------------------------------------------------------------------------
//
TInt CMPXCollectionViewListBoxArray::SetPlaybackStatusByIndex(
    TInt aIndex, TMPXPlaybackState aStatus )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::SetPlaybackStatusByIndex" );
    TInt ret( iIndex );
    iIndex = aIndex;

    // Not to display seeking icons for seeking mode
    if ( aStatus != EPbStateSeekingForward &&
        aStatus != EPbStateSeekingBackward )
        {
        iPbState = aStatus;
        }
    return ret;
    }

//  End of File
