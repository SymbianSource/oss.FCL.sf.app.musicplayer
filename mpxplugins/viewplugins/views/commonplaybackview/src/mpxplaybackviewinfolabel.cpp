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
* Description:  Text label for Playback view
*
*/



// INCLUDE FILES
#include    <AknsSkinInstance.h>
#include    <AknsDrawUtils.h>
#include    <AknsUtils.h>
#include    <AknMarqueeControl.h>
#include    <AknUtils.h>
#include    <AknLayoutFont.h>

#include    "mpxplaybackviewinfolabel.h"

// CONSTANTS
#ifdef _DEBUG
_LIT( KPanicCat, "CMPXPlaybackViewInfoLabel" );
#endif

// MODULE DATA STRUCTURES
enum TPanicCode
    {
    ENoPanic = 0,
    EAlreadyScrolling,
    ENoTextSet,
    EFontNotSet
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewInfoLabel::CMPXPlaybackViewInfoLabel()
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewInfoLabel::ConstructL(TBool aEnableMarqueeSupport)
    {
    if ( aEnableMarqueeSupport )
        {
        iMarquee = CAknMarqueeControl::NewL();
        TCallBack callback( RedrawCallback, this );
        iMarquee->SetRedrawCallBack( callback );
        iMarquee->SetLoops( 1 );
        iMarquee->SetContainerWindowL( *this );
        }
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPlaybackViewInfoLabel::~CMPXPlaybackViewInfoLabel()
    {
    delete iFullText;
    delete iMarquee;
    iBackground = NULL;
    }

// ---------------------------------------------------------------------------
// Set background bitmap
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewInfoLabel::SetBackground(
    MAknsControlContext* aBackground )
    {
    iBackground = aBackground;
    }

// ---------------------------------------------------------------------------
// Redraw rectangle
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewInfoLabel::RedrawRect( const TRect& aRect ) const
    {
    if ( aRect.Intersects( Rect() ) )
        {
        CWindowGc& gc = SystemGc();
        if ( iBackground )
            {
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
            AknsDrawUtils::Background( skin, iBackground, gc, Rect() );
            }

        CGraphicsContext::TTextAlign align=iAlignment.TextAlign();

        // Get baseline for current font
        TInt baselineOffset = 0;
        if ( iMarquee )
            {
            const CAknLayoutFont* layoutFont =
                CAknLayoutFont::AsCAknLayoutFontOrNull( Font() );
            if ( layoutFont )
                {
                baselineOffset= layoutFont->TextPaneTopToBaseline();
                }
            }

        gc.UseFont( Font() );
        gc.SetPenColor( iEikonEnv->ControlColor( EColorLabelText,*this ) );

        // Draw marquee, if supported and necessary
        if ( !iMarquee || !iMarquee->IsMarqueeOn() || iMarquee->DrawText(
            gc,              // aGc
            Rect(),          // aTextRect
            FullText(),      // aText
            baselineOffset,  // aBaselineOffset
            align,           // aAlign
            *Font() ) )      // aFont
            {
            // Marquee is not enabled or all the loops have been executed
            // -> the text needs to be drawn here
            CEikLabel::Draw( aRect );
            }
        }
    }

// ---------------------------------------------------------------------------
// Start marquee scrolling if supported and needed.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewInfoLabel::StartMarquee()
    {
    if ( iMarquee )
        {
        TBool need = NeedToScroll();
        if ( need && !iMarquee->IsMarqueeOn() )
            {
            iMarquee->Reset();
            }
        iMarquee->EnableMarquee( need );
        if ( need )
            {
            iMarquee->Start();
            }
        }
    }

// ---------------------------------------------------------------------------
// Stop marquee scrolling.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewInfoLabel::StopMarquee()
    {
    if ( iMarquee )
        {
        iMarquee->EnableMarquee( EFalse );
        iMarquee->Stop();
        }
    }

// ---------------------------------------------------------------------------
// Get the full unclipped text for the label
// ---------------------------------------------------------------------------
//
const TDesC& CMPXPlaybackViewInfoLabel::FullText() const
    {
    if ( iFullText )
        {
        return *iFullText;
        }
    else
        {
        return *Text();
        }
    }

// ---------------------------------------------------------------------------
// Set label text and clip if necessary.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewInfoLabel::SetLabelTextL( const TDesC& aText )
    {
    if ( iMarquee )
        {
        delete iFullText;
        iFullText = NULL;
        iFullText = aText.AllocL();
        }

    // magic: allocate 3 chars more (make sure "..." fits).
    HBufC* buf = HBufC::NewLC( aText.Length() + 3 );
    *buf = aText;
    TPtr ptr( buf->Des() );
    AknTextUtils::ClipToFit( ptr,
                             *Font(),
                             Rect().Width() );
    SetTextL( *buf );
    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------------------------
// Check if scrolling is needed.
// ---------------------------------------------------------------------------
//
TBool CMPXPlaybackViewInfoLabel::NeedToScroll() const
    {
    __ASSERT_DEBUG( iFont, User::Panic( KPanicCat, EFontNotSet ) );

    TBool need( EFalse );
    // We don't scroll text more than 250 chars in length.
    if ( iFullText )
    	{
    	if ( iFullText->Length() > 250 )
			{
			return need;
			}
		}
    if ( iFullText && iFont->TextWidthInPixels( *iFullText ) > Size().iWidth )
        {
        need = ETrue;
        }
    return need;
    }

// ---------------------------------------------------------------------------
// Redraw callback for marquee control
// ---------------------------------------------------------------------------
//
TInt CMPXPlaybackViewInfoLabel::RedrawCallback( TAny* aPtr )
    {
    CMPXPlaybackViewInfoLabel* self =
        static_cast<CMPXPlaybackViewInfoLabel*>( aPtr );
    CWindowGc& gc = self->SystemGc();
    gc.Activate( *self->DrawableWindow() );
    self->Draw( self->Rect() );
    gc.Deactivate();

    return static_cast<TBool>( ETrue ); // True -> draw again
    }

// ---------------------------------------------------------------------------
// Draws the control.
// ---------------------------------------------------------------------------
//
void CMPXPlaybackViewInfoLabel::Draw( const TRect& aRect ) const
    {
    RedrawRect( aRect );
    }

// ---------------------------------------------------------------------------
// Gets the number of controls contained in a compound control.
// ---------------------------------------------------------------------------
//
TInt CMPXPlaybackViewInfoLabel::CountComponentControls() const
    {
    TInt childCount = 0;
    if ( iMarquee )
        {
        childCount++;
        }
    return childCount;
    }

// ---------------------------------------------------------------------------
// Gets an indexed component of a compound control.
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXPlaybackViewInfoLabel::ComponentControl(
    TInt aIndex ) const
    {
    ASSERT( aIndex == 0 ); // only 1 control
    return iMarquee;
    }

//  End of File
