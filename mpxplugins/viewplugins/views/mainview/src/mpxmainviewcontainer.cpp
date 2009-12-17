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
* Description:  Implementation of main view's container.
*
*/



// INCLUDE FILES
#include <barsread.h>
#include <StringLoader.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <aknconsts.h>
#include <aknlists.h>
#include <avkon.mbg>
#include <eikclbd.h>
#include <data_caging_path_literals.hrh>
#include <mpxmainview.rsg>
#include <mpxmainview.mbg>
#include <mpxconstants.h>
#include <mpxlog.h>
#include <mpxuser.h>
#include "mpxmainviewcontainer.h"
#include "mpxmainview.hlp.hrh"
#include "mpxmainview.hrh"
#include "mpxmainviewimp.h"

// CONSTANTS
_LIT( KMPXMainViewIconFile, "mpxmainview.mbm" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXMainViewContainer::CMPXMainViewContainer(
    CMPXMainViewImp* aView ) :
    iView( aView )
    {
    // pass
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXMainViewContainer::ConstructL( const TRect& aRect )
    {
    MPX_FUNC( "CMPXMainViewContainer::ConstructL()" );

    SetMopParent( iView );

    CreateWindowL();

    SetRect( aRect );

    CreateListBoxL();

    ActivateL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMainViewContainer::~CMPXMainViewContainer()
    {
    MPX_DEBUG1( "-->CMPXMainViewContainer::~CMPXMainViewContainer" );
    delete iListBox;
    MPX_DEBUG1( "<--CMPXMainViewContainer::~CMPXMainViewContainer" );
    }

// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXMainViewContainer::UpdateLayout()
    {
    MPX_FUNC( "CMPXMainViewContainer::UpdateLayout" );
    }

// -----------------------------------------------------------------------------
// Returns a pointer to the listbox
// -----------------------------------------------------------------------------
//
CEikTextListBox* CMPXMainViewContainer::ListBox()
    {
    return iListBox;
    }

// ---------------------------------------------------------------------------
// Create list box
// ---------------------------------------------------------------------------
//
void CMPXMainViewContainer::CreateListBoxL()
    {
    MPX_FUNC( "CMPXMainViewContainer::CreateListBox" );

    iListBox = new ( ELeave ) CAknDoubleLargeStyleListBox();
    iListBox->SetContainerWindowL( *this );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC( rReader, R_MPX_MAIN_VIEW_LBX );
    // construct's iListBox from resource file
    iListBox->ConstructFromResourceL( rReader );
    CleanupStack::PopAndDestroy();

    // Enable Marquee
    CEikFormattedCellListBox* eikListbox =
        static_cast<CEikFormattedCellListBox*>( iListBox );
    eikListbox->EnableExtendedDrawingL();
    eikListbox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
    eikListbox->ItemDrawer()->FormattedCellData()->SetMarqueeParams(
        KMaxTInt, 6, 1000000, 100000); // Magic: loop forever, other params as default

    iListBox->SetListBoxObserver( iView );
    iListBox->View()->SetListEmptyTextL( KNullDesC );

    SizeChanged();
    iListBox->ActivateL();

    // List box icons
    TParse mbmFileName;
    mbmFileName.Set( KMPXMainViewIconFile,
                     &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( mbmFileName.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    // Prepare icon array for listbox
    CAknIconArray* icons = new ( ELeave )
        CAknIconArray( 2 ); // magic: array granularity
    CleanupStack::PushL( icons );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CGulIcon* icon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDNone,
        iconFile,
        EMbmMpxmainviewQgn_indi_mup_default_album,
        EMbmMpxmainviewQgn_indi_mup_default_album_mask );
    icons->AppendL( icon );

    icon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDNone,
        iconFile,
        EMbmMpxmainviewQgn_graf_mup_npv_defalbumart_transp,
        EMbmMpxmainviewQgn_graf_mup_npv_defalbumart_transp_mask );
    icons->AppendL( icon );

    AppendIconToArrayL( icons,
        KAknsIIDQgnIndiMupPlay,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxmainviewQgn_graf_mup_npv_icon_play,
        EMbmMpxmainviewQgn_graf_mup_npv_icon_play_mask );

    AppendIconToArrayL( icons,
        KAknsIIDQgnIndiMupPause,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG26,
        iconFile,
        EMbmMpxmainviewQgn_graf_mup_npv_icon_pause,
        EMbmMpxmainviewQgn_graf_mup_npv_icon_pause_mask );

    icon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDNone,
        iconFile,
        EMbmMpxmainviewQgn_indi_mup_default_album_pcast,
        EMbmMpxmainviewQgn_indi_mup_default_album_pcast_mask );
    icons->AppendL( icon );

    static_cast<CEikFormattedCellListBox*>( iListBox )->ItemDrawer()->
        FormattedCellData()->SetIconArray( icons );
    CleanupStack::Pop( icons );
    }

// -----------------------------------------------------------------------------
// Append icon to array
// -----------------------------------------------------------------------------
//
void CMPXMainViewContainer::AppendIconToArrayL(
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

    AknsUtils::CreateColorIconLC(
        skin,           // aInstance
        aID,     // aID
        aColorId,        // aColorID
        aColorIndex,     // aColorIndex
        bitmap,            // aBitmap
        mask,            // aMask
        aMbmFile, // aFilename
        aBitmapId,   // aFileBitmapId
        aMaskId,     // aFileBitmapId
        KRgbBlack );     // aDefaultColor

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(2); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Responds to changes to the size and position of the contents of this
// control.
// ---------------------------------------------------------------------------
//
void CMPXMainViewContainer::SizeChanged()
    {
    MPX_FUNC( "CMPXMainViewContainer::SizeChanged" );

    if ( iListBox )
        {
        AknLayoutUtils::LayoutControl( iListBox, Rect(),
            AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// ---------------------------------------------------------------------------
//
TInt CMPXMainViewContainer::CountComponentControls() const
    {
    MPX_FUNC( "CMPXMainViewContainer::CountComponentControls" );
    TInt count( 0 );

    if ( iListBox )
        {
        count++;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXMainViewContainer::ComponentControl( TInt aIndex ) const
    {
    MPX_FUNC( "CMPXMainViewContainer::ComponentControl" );
    CCoeControl* control = NULL;
    switch ( aIndex )
        {
        case 0:
            {
            control = iListBox; // Returns the pointer to listbox object.
            break;
            }
        default:
            {
            // Exception : Returns NULL. SHOULD NEVER HAPPEN!
            break;
            }
        }
    return control;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXMainViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXMainViewContainer::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    TKeyResponse response = EKeyWasNotConsumed;

    if ( iListBox )
        {
        response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXMainViewContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXMainViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;
    aContext.iContext = KMUS_HLP_LIB_LIBRARY_VIEW;
    }

//  End of File
