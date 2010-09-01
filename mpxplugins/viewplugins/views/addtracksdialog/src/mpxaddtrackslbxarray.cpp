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
* Description:  Listbox array for add tracks dialog
*
*/


// INCLUDE FILES
#include <AknsSkinInstance.h>
#include <gulicon.h>
#include <AknIconArray.h>
#include <AknUtils.h>
#include <AknsUtils.h>

#include <data_caging_path_literals.hrh>
#include <mpxaddtracksdialog.mbg>
#include <mpxlog.h>
#include <mpxmediageneraldefs.h>
#include <mpxuser.h>

#include "mpxaddtracksdialog.hrh"
#include "mpxaddtracksdialogcommon.h"
#include "mpxaddtrackslistmodel.h"
#include "mpxaddtrackslbxarray.h"

// CONSTANTS
const TInt KMPXLbxIconsCount =
    CMPXAddTracksLbxArray::EMPXATLbxIconsCount;

const TInt KMaxTitleLength = 261;  // 255 + 6 for tab
_LIT( KMPXTab, "\t" );
_LIT( KMPXIndent, "  " );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXAddTracksLbxArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPXAddTracksLbxArray* CMPXAddTracksLbxArray::NewL(
    CMPXAddTracksListModel* aModel )
    {
    MPX_FUNC( "CMPXAddTracksLbxArray::NewL" );
    CMPXAddTracksLbxArray* self =
        new (ELeave) CMPXAddTracksLbxArray( aModel );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CMPXAddTracksLbxArray::~CMPXAddTracksLbxArray()
    {
    MPX_DEBUG1( "CMPXAddTracksLbxArray::~CMPXAddTracksLbxArray START" );
    delete iItem;
    MPX_DEBUG1( "CMPXAddTracksLbxArray::~CMPXAddTracksLbxArray END" );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksLbxArray::CreateIconArrayL
// Create listbox icons array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CMPXAddTracksLbxArray::CreateIconArrayL()
    {
    MPX_FUNC( "CMPXAddTracksLbxArray::CreateIconArrayL" );
    TParse parse;
    parse.Set( KMPXAddTracksBmpFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    CAknIconArray* icons = new ( ELeave ) CAknIconArray( KMPXLbxIconsCount );
    CleanupStack::PushL(icons);

    // Create expand icon
    AppendColorIconToArrayL(
        icons,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_expd_track,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_expd_track_mask );

    // Create collapse icon
    AppendColorIconToArrayL(
        icons,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_colpsd_track,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_colpsd_track_mask );

    // Create expand icon
    AppendColorIconToArrayL(
        icons,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_expd_artist,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_expd_artist_mask );

    // Create collapse icon
    AppendColorIconToArrayL(
        icons,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_colpsd_artist,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_colpsd_artist_mask );

    // Create empty icon
    AppendColorIconToArrayL(
        icons,
        KAknsIIDNone,
        KAknsIIDNone,
        EAknsMinorNone,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_empty,
        EMbmMpxaddtracksdialogQgn_graf_mup_lst_empty_mask );

    // Create selection confirm animation icon 1
    AppendColorIconToArrayL(
        icons,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG13,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr01,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr01_mask );

    // Create selection confirm animation icon 2
    AppendColorIconToArrayL(
        icons,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG13,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr02,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr02_mask );

    // Create selection confirm animation icon 3
    AppendColorIconToArrayL(
        icons,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG13,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr03,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr03_mask );

    // Create selection confirm animation icon 4
    AppendColorIconToArrayL(
        icons,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG13,
        iconFile,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr04,
        EMbmMpxaddtracksdialogQgn_graf_mup_adtl_chmrk_fr04_mask );

    // Create icon for MMC
    AppendColorIconToArrayL(
        icons,
        KAknsIIDQgnIndiMmcAdd,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG13,
        iconFile,
        EMbmMpxaddtracksdialogQgn_indi_mmc_add,
        EMbmMpxaddtracksdialogQgn_indi_mmc_add_mask );

    CleanupStack::Pop( icons );

    return icons;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksLbxArray::MdcaCount
// Returns the number of descriptor elements in a descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksLbxArray::MdcaCount() const
    {
    MPX_FUNC( "CMPXAddTracksLbxArray::MdcaCount" );
    return iModel->TracksListCount();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksLbxArray::MdcaPoint
// Indexes into a descriptor array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CMPXAddTracksLbxArray::MdcaPoint( TInt aIndex ) const
    {
    //MPX_FUNC( "CMPXAddTracksLbxArray::MdcaPoint" );
    TPtrC item;

    TPtr ptr = iItem->Des();
    ptr.Zero();

    TInt iconIndex;
    TBool indent = EFalse;
    TInt trackType = EMPXNoType;
    MPX_TRAPD( error, trackType = iModel->TrackTypeL( aIndex ) );

    if ( error == KErrNone && trackType == EMPXGroup )
        {
        TInt category = EMPXSong;
        TRAP_IGNORE( category = iModel->TrackCategoryL(aIndex));
        if ( aIndex == 0 && category != EMPXArtist)
            {
            iconIndex = EMPXATLbxIconTrackExpand;
            if ( iModel->IsExpandedIndex( aIndex ) )
                {
                iconIndex = EMPXATLbxIconTrackCollapse;
                }
            }
        else
            {
            iconIndex = EMPXATLbxIconArtistExpand;
            if ( iModel->IsExpandedIndex( aIndex ) )
                {
                iconIndex = EMPXATLbxIconArtistCollapse;
                }
            }
        }
    else
        {
        // Indent if tracks
        iconIndex = EMPXATLbxIconEmpty;
        indent = ETrue;
        }

    ptr.AppendNum( iconIndex );
    ptr.Append( KMPXTab );
    if ( indent )
        {
        ptr.Append( KMPXIndent );
        }
    ptr.Append( iModel->TrackName( aIndex ) );

    TInt animIconIndex = iModel->AnimationIconNum( aIndex );
    if(animIconIndex != 0)
        {
        ptr.Append( KMPXTab );
        ptr.AppendNum( animIconIndex );
        }

    TBool isRemovable( EFalse );
    isRemovable = iModel->IsOnRemovableDrive( aIndex );
    if ( isRemovable )
        {
        ptr.Append( KMPXTab );
        ptr.AppendNum( EMPXATLbxIconMMC );
        }

    item.Set( *iItem );
    return item;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksLbxArray::CMPXAddTracksLbxArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXAddTracksLbxArray::CMPXAddTracksLbxArray(
    CMPXAddTracksListModel* aModel ) :
    iModel( aModel )
    {
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksLbxArray::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPXAddTracksLbxArray::ConstructL()
    {
    MPX_FUNC( "CMPXAddTracksLbxArray::ConstructL" );
    // iItem length cannot be defined dynamically as MdcaPoint is const function
    iItem = HBufC::NewL( KMaxTitleLength ) ;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksLbxArray::AppendIconToArrayL
// Load icon (with mask) and append it to an icon array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksLbxArray::AppendColorIconToArrayL(
    CAknIconArray* aArray,
    const TAknsItemID& aID,
    const TAknsItemID& aColorId,
    TInt aColorIndex,
    const TDesC& aMbmFile,
    TInt aBitmapId,
    TInt aMaskId )
    {
    MPX_FUNC( "CMPXAddTracksLbxArray::AppendColorIconToArrayL" );
    __ASSERT_DEBUG( aArray != NULL,
        User::Panic( _L( "AppendColorIconToArrayL" ), KErrArgument ) );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

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
    icon->SetBitmapsOwnedExternally(EFalse);
    CleanupStack::Pop( 2 ); // bitmap, mask
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );
    CleanupStack::Pop( icon );
    }

//  End of File
