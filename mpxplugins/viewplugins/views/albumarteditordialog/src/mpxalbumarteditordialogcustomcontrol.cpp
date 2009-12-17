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
* Description:  Music Player album art editor dialog implementation
*
*/


// INCLUDE FILES
#include <AknUtils.h>
#include <eiklabel.h>
#include <StringLoader.h>
#include <mpxalbumarteditordialog.rsg>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <mpxlog.h>

#include "mpxalbumarteventobserver.h"
#include "mpxalbumarteditordialogcustomcontrol.h"

// CONSTANTS
const TInt KMPXLabelCount = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogCustomControl*
    CMPXAlbumArtEditorDialogCustomControl::NewLC(
    const CCoeControl& aParent,
    const TRect &aRect,
    MMPXAlbumArtEventObserver* aObserver )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::NewLC" );
    CMPXAlbumArtEditorDialogCustomControl* self =
        new ( ELeave ) CMPXAlbumArtEditorDialogCustomControl();
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aRect, aObserver );
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogCustomControl*
    CMPXAlbumArtEditorDialogCustomControl::NewL(
    const CCoeControl& aParent,
    const TRect &aRect,
    MMPXAlbumArtEventObserver* aObserver )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::NewL" );
    CMPXAlbumArtEditorDialogCustomControl* self = NewLC(
        aParent, aRect, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CMPXAlbumArtEditorDialogCustomControl::~CMPXAlbumArtEditorDialogCustomControl()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::~CMPXAlbumArtEditorDialogCustomControl" );
    delete iImage;
    delete iLabel;
    delete iBackground;
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::CMPXAlbumArtEditorDialogCustomControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogCustomControl::CMPXAlbumArtEditorDialogCustomControl() :
    iLabel( NULL ), iObserver( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::ConstructL(
    const CCoeControl& aParent,
    const TRect &aRect,
    MMPXAlbumArtEventObserver* aObserver )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::ConstructL" );
    SetMopParent( const_cast < CCoeControl* >( &aParent ) );
    CreateWindowL();

    iImage = new ( ELeave ) CEikImage();
    iImage->SetContainerWindowL( *this );

    iLabel = new ( ELeave ) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetTextL( _L( "" ) );
    iLabel->MakeVisible ( EFalse );

    iObserver = aObserver;

    iMode = EMPXAlbumArtEditorDialogBlank;
    SetRect( aRect );
    HandleSkinChangedL();
    ActivateL();
    MakeVisible( ETrue );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::UpdateAlbumArt
// Updates new image to CEikImage
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::UpdateAlbumArt(
    TMPXAlbumArtEditorDialogMode aMode,
    CFbsBitmap* aNewAlbumArt )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::UpdateAlbumArt" );
    switch ( aMode )
        {
        case EMPXAlbumArtEditorDialogAlbumArt:
            {
            iImage->SetPictureOwnedExternally( EFalse) ;
            iImage->SetNewBitmaps( aNewAlbumArt, NULL );
            iImage->SetSize( iImage->MinimumSize() );
            iImage->SetPictureOwnedExternally( EFalse );
            iImage->SetAlignment( EHCenterVCenter );
            iImage->MakeVisible( ETrue );

            TRect myRect = Rect();
            TSize bSize = aNewAlbumArt->SizeInPixels();
            AknLayoutUtils::LayoutImage(
                iImage, myRect, EColor256,
                ( myRect.Width() - bSize.iWidth ) / 2 ,
                ( myRect.Height() - bSize.iHeight ) / 2,
                NULL, NULL, bSize.iWidth, bSize.iHeight );

            // layout border
            TInt tX = ( myRect.Width() - bSize.iWidth ) / 2;
            TInt tY = ( myRect.Height() - bSize.iHeight ) / 2;
            iBorder.iTl.iX = tX - 1;
            iBorder.iTl.iY = tY - 1;
            iBorder.iBr.iX = tX + bSize.iWidth + 1;
            iBorder.iBr.iY = tY + bSize.iHeight + 1;

            TRAP_IGNORE( SetModeL( EMPXAlbumArtEditorDialogAlbumArt ) );
            break;
            }
        default:
            {
            TRAP_IGNORE( SetModeL( aMode ) );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::SetModeL
// Displays image in a specific mode, either Album Art/No Album Art or Various
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::SetModeL(
    TMPXAlbumArtEditorDialogMode aMode )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::SetModeL" );
    iMode = aMode;
    switch( iMode )
        {
        case EMPXAlbumArtEditorDialogAlbumArt:
            {
            iLabel->MakeVisible( EFalse );
            iImage->MakeVisible( ETrue );
            DrawDeferred();
            break;
            }
        case EMPXAlbumArtEditorDialogNoAlbumArt:
            {
            if(iImage->Bitmap())
                {
                iImage->MakeVisible( EFalse );
                }
            iLabel->MakeVisible( EFalse );

            HBufC* labeltext = StringLoader::LoadLC(
                R_MPX_CUI_ALBUM_ART_NO_ART_TEXT  );
            iLabel->SetTextL( *labeltext );
            CleanupStack::PopAndDestroy( labeltext );

            iLabel->MakeVisible( ETrue );
            DrawDeferred();
            break;
            }
        case EMPXAlbumArtEditorDialogBlank:
            iImage->MakeVisible( EFalse );
            break;
        default: // some error
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::HandleSkinChangedL
// updates skin color
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::HandleSkinChangedL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::HandleSkinChangedL" );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb color = KRgbBlack;
    // Color is not updated if it not found from the skin
    AknsUtils::GetCachedColor(
        skin,
        color,
        KAknsIIDQsnTextColors,
        EAknsCIQsnTextColorsCG6 );

    AknLayoutUtils::OverrideControlColorL(
        *iLabel,
        EColorLabelText,
        color );
    }

// ---------------------------------------------------------------------------
// From MMPXAlbumArtUtilObserver
// Notify that extraction of album art started.
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::ExtractAlbumArtStarted()
    {
    // Do Nothing
    }

// ---------------------------------------------------------------------------
// From MMPXAlbumArtUtilObserver
// Album art is extracted from file and ready to use.
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::ExtractAlbumArtCompleted(
    CFbsBitmap* aBitmap,
    TInt aErr )
    {
    MPX_DEBUG3( "CMPXAlbumArtEditorDialogCustomControl::ExtractAlbumArtCompleted(bitmap:0x%x, err:%d)",
        aBitmap, aErr );

    if ( !aErr )
        {
        UpdateAlbumArt( EMPXAlbumArtEditorDialogAlbumArt, aBitmap );
        }
    else
        {
        // error case, do not display album art
        UpdateAlbumArt( EMPXAlbumArtEditorDialogNoAlbumArt );
        }
    if ( iObserver )
        {
        TRAP_IGNORE (
            iObserver->HandleAlbumArtEventL(
                EMPXAlbumArtEventExtractCompleted, aErr ) );
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::Draw
// Draws to the display
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::Draw( const TRect& aRect ) const
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::Draw" );
    CWindowGc& gc = SystemGc();
    if ( iBackground )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsDrawUtils::DrawBackground(
            skin,
            iBackground,
            this,
            gc,
            aRect.iTl,
            aRect,
            KAknsDrawParamDefault );
        }
    else
        {
        gc.Clear( aRect );
        }
    gc.SetClippingRect( aRect );
    switch( iMode )
        {
        case EMPXAlbumArtEditorDialogAlbumArt:
            {
            gc.SetBrushColor( KRgbBlack );
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.DrawRect( iBorder );
            break;
            }
        case EMPXAlbumArtEditorDialogBlank:
            {
            break;
            }
        default: // either various of no album art
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::CountComponentControls
// Returns the number of components in the container.
// -----------------------------------------------------------------------------
//
TInt CMPXAlbumArtEditorDialogCustomControl::CountComponentControls() const
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::CountComponentControls" );
    return KMPXLabelCount;
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::ComponentControl
// Returns the control given the index.
// -----------------------------------------------------------------------------
//
CCoeControl* CMPXAlbumArtEditorDialogCustomControl::ComponentControl(
    TInt aIndex ) const
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::ComponentControl" );
    switch ( aIndex )
        {
        case 0:
            return iImage;
        case 1:
            return iLabel;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogCustomControl::SizeChanged
// Resizes controls.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogCustomControl::SizeChanged()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogCustomControl::SizeChanged" );
    delete iBackground;
    iBackground = NULL;
    TRect appRect = iEikonEnv->EikAppUi()->ApplicationRect();
    MPX_TRAPD( error,
        iBackground = CAknsBasicBackgroundControlContext::NewL(
            KAknsIIDQsnBgAreaMain, appRect, EFalse ) );
    if ( error == KErrNone && iLabel )
        {
        TRect coeRect = Rect();
        AknLayoutUtils::LayoutLabel( iLabel, coeRect,
            R_MPX_CUI_ALBUM_ART_LAYOUT );
        TRAP_IGNORE( HandleSkinChangedL() );
        }
     }

//  End of File
