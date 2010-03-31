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
* Description:  Listbox array for podcast collection view container
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <gulicon.h>
#include <AknsSkinInstance.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
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
#include <mpxpodcastcollectionview.mbg>
#include <mpxpodcastdefs.h> // Podcast specific
#include <mpxplaybackframeworkdefs.h>
#include <mpxpodcastcollectionview.rsg>
#include <mpxuser.h>
#include <mpxcommonuihelper.h>
#include "mpxcommoncontainer.hrh"
#include "mpxpodcastcollectionview.hrh"
#include "mpxcommonlistboxarrayobserver.h"
#include "mpxpodcastcollectionviewlistboxarray.h"

// CONSTANTS
_LIT( KMPXCommonListboxFormatString, "%d\t%S" );
_LIT( KMPXTab, "\t" );
_LIT( KMPXPodcastCollectionViewIconFile, "mpxpodcastcollectionview.mbm" );

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewListBoxArray* CMPXPodcastCollectionViewListBoxArray::NewL()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewListBoxArray::NewL" );
    CMPXPodcastCollectionViewListBoxArray* self =
        new ( ELeave ) CMPXPodcastCollectionViewListBoxArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::CMPXCommonListBoxArrayBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXPodcastCollectionViewListBoxArray::CMPXPodcastCollectionViewListBoxArray() :
    CMPXCommonListBoxArrayBase()
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CMPXPodcastCollectionViewListBoxArray::~CMPXPodcastCollectionViewListBoxArray()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewListBoxArray::~CMPXPodcastCollectionViewListBoxArray" );
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewListBoxArray::ConstructL()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewListBoxArray::ConstructL" );
    iMMCDrive = CMPXCommonUiHelper::MMCDriveNumber();
    }

// ---------------------------------------------------------------------------
// Sets playback status
// ---------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewListBoxArray::SetPlaybackStatusByIdL(
    TMPXItemId aId, TMPXPlaybackState aStatus )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewListBoxArray::SetPlaybackStatusById" );
    if ( iMedia )
        {
        ASSERT( iMedia );
        TInt index( KErrNotFound );
        TInt count( iMedia->Count() );

        for ( TInt i = 0; i < count; i++ )
            {
            CMPXMedia* entry( iMedia->AtL( i ) );
            if( entry->IsSupported( KMPXMediaGeneralId ) )
                {
                TMPXItemId id(
                    entry->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

                if ( id == aId )
                    {
                    index = i;
                    break;
                    }

                }
            }

        return SetPlaybackStatusByIndex( index, aStatus );
        }

    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Gets the index for the song that's currently playing
// ---------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewListBoxArray::GetPlaybackIndex() const
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::GetPlaybackIndex" );
    return iIndex;
    }

// -----------------------------------------------------------------------------
// Get array of indicator icon indices at the specified view index
// Only used when using Hitchcock container.
// -----------------------------------------------------------------------------
//
RArray<TInt> CMPXPodcastCollectionViewListBoxArray::IndicatorIconIndicesL(
    TInt aIndex )
    {
    RArray<TInt> iconIndices;
    TInt index( MapListBoxIndexToArrayIndex( aIndex ) );

    CMPXMedia* entry( iMedia->AtL( index ) );

    TMPXPodcastType type = EMPXPodcastNoType;
    TMPXPodcastCategory category = EMPXUnknown;
    if ( entry->IsSupported( KMPXMediaPodcastType ) )
        {
        type = entry->ValueTObjectL<TMPXPodcastType>( KMPXMediaPodcastType );
        }
    if ( entry->IsSupported( KMPXMediaPodcastCategoryGroup ) )
        {
        category = entry->ValueTObjectL<TMPXPodcastCategory>( KMPXMediaPodcastCategoryGroup );
        }
    TUint flags(0);
    if ( entry->IsSupported( KMPXMediaGeneralFlags ) )
        {
        flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
        }

    MPX_DEBUG4( "CMPXPodcastCollectionViewListBoxArray::IndicatorIconIndicesL (%d) Type: %d, Category: %d", aIndex, type, category );


    // playback status icon
    if ( iIndex == index )
        {
        switch ( iPbState )
            {
            case EPbStatePlaying:
                {
                iconIndices.Append( EMPXPodClvIconPlay );
                break;
                }
            case EPbStatePaused:
                {
                iconIndices.Append( EMPXPodClvIconPause );
                break;
                }
            default:
                {
                // other playback states, not handled
                break;
                }
            }
        }

    if ( type == EMPXPodcastItem )
        {
        TBool checkFlag(ETrue);
        switch( category )
            {
            case EMPXEpisode:
                {
                iconIndices.Append( EMPXPodClvIconEpisode );
                break;
                }
            case EMPXNew:
                {
                if ( entry->IsSupported(KMPXMediaPodcastIsPlaying) &&
                     entry->ValueTObjectL<TBool>(KMPXMediaPodcastIsPlaying) )
                    {
                    iconIndices.Append( EMPXPodClvIconPartplay );
                    }
                else if ( iIndex == aIndex  &&
                          (iPbState == EPbStatePlaying || iPbState == EPbStatePaused) )
                    {
                    entry->SetTObjectValueL<TBool>(KMPXMediaPodcastIsPlaying, ETrue);
                    iconIndices.Append( EMPXPodClvIconPartplay );
                    }
                else
                    {
                    iconIndices.Append( EMPXPodClvIconNew );
                    }

                break;
                }
            case EMPXPartlyPlayed:
                {
                iconIndices.Append( EMPXPodClvIconPartplay );
                break;
                }
            case EMPXCompletelyPlayed:
                {
                iconIndices.Append( EMPXPodClvIconCompplay );
                break;
                }
            case EMPXPodcastFile:
                {
                checkFlag = EFalse;
                iconIndices.Append( EMPXPodClvIconFolder );
                break;
                }
            default:
                {
                checkFlag = EFalse;
                iconIndices.Append( EMPXPodClvIconUnknowFile );
                break;
                }
            }

            if ( checkFlag )
                {
                if ( flags & KMPXMediaGeneralFlagsIsCorrupted )
                    {
                    iconIndices.Append( EMPXPodClvIconCorrupt );
                    }
                else if ( flags & KMPXMediaGeneralFlagsIsInvalid )
                    {
                    iconIndices.Append( EMPXPodClvIconBroken );
                    }
                else if ( flags & KMPXMediaGeneralFlagsIsDrmLicenceInvalid )
                    {
                    iconIndices.Append( EMPXPodClvIconBroken );
                    }
                // else do nothing
                }
        }

    TInt driveNum = ( ( flags ) & ( KMPXMediaGeneralFlagsDriveInfo ) );
    if ( driveNum == iMMCDrive )
        {
        // The MMC icon is displayed in the first Indicator slot
        iconIndices.Append( EMPXPodClvIconMMC );
        }

    return iconIndices;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::MdcaPoint
// indexes into a descriptor array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CMPXPodcastCollectionViewListBoxArray::MdcaPoint( TInt aIndex ) const
    {
    TPtrC item( KNullDesC );
    TRAP_IGNORE( item.Set( DoMdcaPointL( aIndex ) ) );
    return item;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::CreateIconArrayL
// creates icon array.
// -----------------------------------------------------------------------------
//
CAknIconArray* CMPXPodcastCollectionViewListBoxArray::CreateIconArrayL()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewListBoxArray::CreateIconArrayL" );
    TParse mbmFileName;
    mbmFileName.Set( KMPXPodcastCollectionViewIconFile,
                     &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( mbmFileName.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    // Prepare icon array for listbox
    CAknIconArray* iconArray = new ( ELeave ) CAknIconArray( 21 ); // magic: array granularity

    // Update the enums when this list is updated
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMarkedAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG13,
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
        KAknsIIDQgnGrafMupLstPdcAdded,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_added,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_added_mask );  //1
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcAuto,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_auto,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_auto_mask );   //2
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcCateg,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_categ,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_categ_mask );  //3
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcCompplay,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_compplay,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_compplay_mask );  //4
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcDate,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_date,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_date_mask );  //5
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcEpisode,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_episode,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_episode_mask );  //6
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcNew,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_new,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_new_mask );  //7
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcNoplay,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_noplay,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_noplay_mask );  //8
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcPartplay,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_partplay,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_partplay_mask );  //9
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstPdcTitle,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_title,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_pdc_title_mask );  //10
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnMenuFolderApps,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_menu_folder_apps,
        EMbmMpxpodcastcollectionviewQgn_menu_folder_apps_mask );  //11
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnPropUnknown,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_prop_unknown,
        EMbmMpxpodcastcollectionviewQgn_prop_unknown_mask );  //12
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupPlay,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_npv_icon_play,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_npv_icon_play_mask ); //13
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupPause,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_npv_icon_pause,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_npv_icon_pause_mask ); // 14
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupForwAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_indi_mup_forw_add,
        EMbmMpxpodcastcollectionviewQgn_indi_mup_forw_add_mask );  // 15
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMupRewAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_indi_mup_rew_add,
        EMbmMpxpodcastcollectionviewQgn_indi_mup_rew_add_mask ); // 16
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_indi_mmc_add,
        EMbmMpxpodcastcollectionviewQgn_indi_mmc_add_mask ); // 17
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstBrokenpodc,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_brokenpodc,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_brokenpodc_mask );  // 18
    AppendColorIconToArrayL( iconArray,
        KAknsIIDQgnGrafMupLstCorruptpodc,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_corruptpodc,
        EMbmMpxpodcastcollectionviewQgn_graf_mup_lst_corruptpodc_mask );   // 19

    return iconArray;
    }

// -----------------------------------------------------------------------------
// Appends media array to the current array
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewListBoxArray::AppendMediaL( const CMPXMedia& aMedia )
    {
    MPX_FUNC( "CMPXCollectionViewListBoxArray::AppendMediaL" );
    CMPXCommonListBoxArrayBase::AppendMediaL( aMedia );
    if ( iMedia )
        {
        TInt entriesCount = iMedia->Count();
        if ( entriesCount > 0 )
            {
            CMPXMedia* origMedia( iMedia->AtL( entriesCount - 1 ) );
            if ( origMedia->ValueText(KMPXMediaGeneralTitle ).Length() == 0 )
                {
                HBufC* text = StringLoader::LoadLC(
                    R_MPX_PODCAST_UNKNOWN );
                origMedia->SetTextValueL(KMPXMediaGeneralTitle,
                    *text );
                CleanupStack::PopAndDestroy( text );
                }
            }
        }
    if ( iContainerMedia )
        {
        if ( iContainerMedia->IsSupported(KMPXMediaGeneralTitle) )
            {
            if ( iContainerMedia->ValueText(KMPXMediaGeneralTitle).Length() == 0 )
                {
                HBufC* text = StringLoader::LoadLC(
                    R_MPX_TITLE_UNKNOWN_PODCAST );
                iContainerMedia->SetTextValueL(KMPXMediaGeneralTitle,
                    *text );
                CleanupStack::PopAndDestroy( text );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Checks if the current item is a broken link
// -----------------------------------------------------------------------------
//
TBool CMPXPodcastCollectionViewListBoxArray::IsItemBrokenLinkL( TInt aIndex )
    {
    TBool ret( EFalse );
    if ( iMedia )
        {
        CMPXMedia* entry( iMedia->AtL( aIndex ) );
        TUint flags( 0 );
        if(entry->IsSupported(KMPXMediaGeneralFlags))
            {
            flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
            }

        ret = ( ( flags ) & ( KMPXMediaGeneralFlagsIsInvalid ) );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// Checks if the current item is corrupted
// -----------------------------------------------------------------------------
//
TBool CMPXPodcastCollectionViewListBoxArray::IsItemCorruptedL( TInt aIndex )
    {
    TBool ret( EFalse );
    if ( iMedia )
        {
        CMPXMedia* entry( iMedia->AtL( aIndex ) );
        TUint flags( 0 );
        if(entry->IsSupported( KMPXMediaGeneralFlags))
            {
            flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
            }
        ret = ( ( flags ) & ( KMPXMediaGeneralFlagsIsCorrupted ) );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMPXCommonListBoxArrayBase::DoMdcaPointL
// indexes into a descriptor array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CMPXPodcastCollectionViewListBoxArray::DoMdcaPointL( TInt aIndex ) const
    {
    MPX_FUNC( "CMPXPodcastCollectionViewListBoxArray::DoMdcaPointL" );
    MPX_DEBUG3( "CMPXPodcastCollectionViewListBoxArray::DoMdcaPointL media count = %d aIndex = %d", MdcaCount(), aIndex );
    TInt index( MapListBoxIndexToArrayIndex( aIndex ) );

    CMPXMedia* entry( iMedia->AtL( index ) );

    if( entry->IsSupported( KMPXMediaGeneralTitle ))
        {
        const TDesC& title = entry->ValueText(KMPXMediaGeneralTitle);
        TMPXPodcastType type = EMPXPodcastNoType;
        TMPXPodcastCategory category = EMPXUnknown;

        if (entry->IsSupported(KMPXMediaPodcastType))
            {
            type =
                entry->ValueTObjectL<TMPXPodcastType>(KMPXMediaPodcastType);

            }
        if (entry->IsSupported(KMPXMediaPodcastCategoryGroup))
            {
            category =
                    entry->ValueTObjectL<TMPXPodcastCategory>(KMPXMediaPodcastCategoryGroup);
            }
        TUint flags(0);
        if( entry->IsSupported( KMPXMediaGeneralFlags ) )
            {
            flags = entry->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
            }

        MPX_DEBUG3( "CMPXPodcastCollectionViewListBoxArray::MdcaPoint (%d) Name: %S", aIndex, &title );
        MPX_DEBUG4( "CMPXPodcastCollectionViewListBoxArray::MdcaPoint (%d) Type: %d, Category: %d", aIndex, type, category );
        TInt iconID;
        if ( type == EMPXPodcastGroup )
            {
            switch( category )
                {
                case EMPXAll:
                    {
                    iconID = EMPXPodClvIconEpisode;
                    break;
                    }
                case EMPXRecentlyAdded:
                case EMPXNotYetPlayed:
                    {
                    iconID = EMPXPodClvIconAuto;
                    break;
                    }
                case EMPXPubDate:
                    {
                    iconID = EMPXPodClvIconDate;
                    break;
                    }
                case EMPXTitle:
                    {
                    iconID = EMPXPodClvIconTitle;
                    break;
                    }
                case EMPXPodcastFile:
                    {
                    iconID = EMPXPodClvIconFolder;
                    break;
                    }
                default:
                    {
                    iconID = EMPXPodClvIconUnknowFile;
                    break;
                    }
                } // Switch
            }
        else if ( type == EMPXPodcastItem )
            {
            TBool checkFlag(ETrue);

            switch( category )
                {
                case EMPXEpisode:
                    {
                    iconID = EMPXPodClvIconEpisode;
                    break;
                    }
                case EMPXNew:
                    {
                    if(entry->IsSupported(KMPXMediaPodcastIsPlaying) &&
                       entry->ValueTObjectL<TBool>(KMPXMediaPodcastIsPlaying))
                        {
                        iconID = EMPXPodClvIconPartplay;
                        }
                    else if(iIndex == aIndex  &&
                           (iPbState == EPbStatePlaying ||
                            iPbState == EPbStatePaused))
                        {
                        entry->SetTObjectValueL<TBool> (
                            KMPXMediaPodcastIsPlaying, ETrue);
                        iconID = EMPXPodClvIconPartplay;
                        }
                    else
                        {
                        iconID = EMPXPodClvIconNew;
                        }

                    break;
                    }
                case EMPXPartlyPlayed:
                    {
                    iconID = EMPXPodClvIconPartplay;
                    break;
                    }
                case EMPXCompletelyPlayed:
                    {
                    iconID = EMPXPodClvIconCompplay;
                    break;
                    }
                case EMPXPodcastFile:
                    {
                    checkFlag = EFalse;
                    iconID = EMPXPodClvIconFolder;
                    break;
                    }
                default:
                    {
                    checkFlag = EFalse;
                    iconID = EMPXPodClvIconUnknowFile;
                    break;
                    }
                } // Switch

                if(checkFlag)
                    {
                    if ( flags & KMPXMediaGeneralFlagsIsCorrupted )
                        {
                        iconID = EMPXPodClvIconCorrupt;
                        }
                    else if ( flags & KMPXMediaGeneralFlagsIsInvalid )
                        {
                        iconID = EMPXPodClvIconBroken;
                        }
                    else if ( flags & KMPXMediaGeneralFlagsIsDrmLicenceInvalid )
                        {
                        iconID = EMPXPodClvIconBroken;
                        }
                    // else do nothing
                    }
            }
        else
            {
            iconID = EMPXPodClvIconUnknowFile;
            }

        TPtrC item;

        TPtr ptr = iItem->Des();
        ptr.Zero();
        ptr.Format( KMPXCommonListboxFormatString, iconID, &title );

        // playback status icon
        if ( iIndex == index )
            {
            if ( iPbState == EPbStatePlaying )
                {
                ptr.Append( KMPXTab );
                ptr.AppendNum( EMPXPodClvIconPlay );
                }
            else if ( iPbState == EPbStatePaused )
                {
                ptr.Append( KMPXTab );
                ptr.AppendNum( EMPXPodClvIconPause );
                }
            else if ( iPbState == EPbStateSeekingForward )
                {
                ptr.Append( KMPXTab );
                ptr.AppendNum( EMPXPodClvIconForward );
                }
            else if ( iPbState == EPbStateSeekingBackward )
                {
                ptr.Append( KMPXTab );
                ptr.AppendNum( EMPXPodClvIconRewind );
                }
            else
                {
                // Do nothing
                }
            }

        // mmc icon
        TInt driveNum = ( ( flags ) & ( KMPXMediaGeneralFlagsDriveInfo ) );
        if( driveNum == iMMCDrive )
            {
            ptr.Append( KMPXTab );
            ptr.AppendNum( EMPXPodClvIconMMC );
            }

        item.Set( *iItem );
        return item;
        }
    else
        {
        TPtrC item( KNullDesC );
        return item;
        }
    }

// -----------------------------------------------------------------------------
// Append icon to array
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewListBoxArray::AppendColorIconToArrayL(
    CAknIconArray* aArray,
    const TAknsItemID& aID,
    const TAknsItemID& aColorId,
    TInt aColorIndex,
    const TDesC& aMbmFile,
    TInt aBitmapId,
    TInt aMaskId )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

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
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );
    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop( icon );
    }

// -----------------------------------------------------------------------------
// Maps list box index to array index in reorder mode
// -----------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewListBoxArray::MapListBoxIndexToArrayIndex(
    TInt aIndex ) const
    {
    TInt index = aIndex;

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

// ---------------------------------------------------------------------------
// Sets playback status
// ---------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewListBoxArray::SetPlaybackStatusByIndex(
    TInt aIndex, TMPXPlaybackState aStatus )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewListBoxArray::SetPlaybackStatusByIndex" );
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
