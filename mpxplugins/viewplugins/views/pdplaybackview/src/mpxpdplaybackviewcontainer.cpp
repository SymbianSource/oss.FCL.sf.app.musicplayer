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
* Description:  Implementation of Progressive Download Playback view's container.
*
*/



// INCLUDE FILES

#include <AknDef.h>
#include <gulicon.h>

#include "mpxconstants.h"
#include "mpxpdplaybackview.hlp.hrh"
#include "mpxplaybackviewinfolabel.h"
#include "mpxpdplaybackviewcontainer.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxlog.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewContainer::CMPXPdPlaybackViewContainer(
    MEikCommandObserver* aCommandObserver,
    MMPXLayoutSwitchObserver* aLayoutObserver,
    MMPXPlaybackViewLayout* aLayout )
    {
    iCommandObserver = aCommandObserver;
    iLayoutObserver = aLayoutObserver;
    iMode = EUninitialized;
    iLayout = aLayout;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewContainer::ConstructL( const TRect& aRect )
    {
    MPX_DEBUG1( "CMPXPdPlaybackViewContainer::ConstructL() entering" );
    
    CMPXCommonPlaybackViewContainer::ConstructL( aRect );

    MPX_DEBUG1( "CMPXPdPlaybackViewContainer::ConstructL() exiting" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPdPlaybackViewContainer::~CMPXPdPlaybackViewContainer()
    {
    }

// ---------------------------------------------------------------------------
// CMPXPdPlaybackViewContainer::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	//Handles the touch events, except for sliderpane.
	if ( (!iSliderPaneRect.Contains(aPointerEvent.iPosition) ) )
		{
		CMPXCommonPlaybackViewContainer::HandlePointerEventL(aPointerEvent);
		}
	}

// ---------------------------------------------------------------------------
// Handles when the download position has changed
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewContainer::HandleDownloadPositionChanged(
    TInt aDownloadedBytes, 
    TInt aTotalBytes )
    {
    MPX_DEBUG3("CMPXPdPlaybackViewContainer::HandleDownloadPositionChanged(%d, %d) entering", 
                aDownloadedBytes, 
                aTotalBytes);
    
    if (aTotalBytes)
        {
        TReal t = aTotalBytes;
        iDownloadRatio = aDownloadedBytes / t;
        
        RefreshProgressBar();
        }    
    
    MPX_DEBUG1("CMPXPdPlaybackViewContainer::HandleDownloadPositionChanged() exiting");    
    }

// ---------------------------------------------------------------------------
// Redraw part of the screen.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewContainer::RedrawRect(
    const TRect& aRect) const
    {
    MPX_DEBUG1("CMPXPdPlaybackViewContainer::RedrawRect() entering");
    

 CMPXCommonPlaybackViewContainer::RedrawRect( aRect );
    
    // Always show progress bars in progressive download
    CWindowGc& gc = SystemGc();
    DrawIndicator( gc, aRect, iSliderBackgroundRect, iSliderBackground );
    DrawIndicator( gc, 
                   aRect,
                   TRect(iDownloadSliderRect.iTl,
                         iDownloadSlider->Bitmap()->SizeInPixels()),
                   iDownloadSlider );
    DrawIndicator( gc, 
                   aRect, 
                   TRect(iPlaybackSliderRect.iTl,
                         iPlaybackSlider->Bitmap()->SizeInPixels()),
                   iPlaybackSlider );
    
    MPX_DEBUG1("CMPXPdPlaybackViewContainer::RedrawRect() exiting");    
    }

// ---------------------------------------------------------------------------
// Sets main pane mode.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewContainer::SetMode( TMPXPbvPlaybackMode aMode )
    {
    MPX_DEBUG2("CMPXPdPlaybackViewContainer::SetMode(%d) entering", aMode);
    
    CMPXCommonPlaybackViewContainer::SetMode( aMode );

    if ( ENoTracksMode != aMode )
        {
        iLabels[ETextArtist]->MakeVisible( ETrue );
        iLabels[ETextTrack]->MakeVisible( ETrue );

        // Make sure download label is visible all the time
        iLabels[ETextDownloadState]->MakeVisible( ETrue );

        // Make sure album art is visible all the time
        iDisplayAlbumArt = ETrue;
        }
    
    MPX_DEBUG1("CMPXPdPlaybackViewContainer::SetMode() exiting");    
    }
    
// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewContainer::UpdateLayout()
    {
    MPX_DEBUG1( "CMPXPdPlaybackViewContainer::UpdateLayout() entering" );
    
    CMPXCommonPlaybackViewContainer::UpdateLayout();

    MPX_DEBUG1( "CMPXPdPlaybackViewContainer::UpdateLayout() exiting" );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXPdPlaybackViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG4( "CMPXPdPlaybackViewContainer::OfferKeyEventL(iCode=%d, iScanCode=%d, aType=%d)", 
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );
    // No action for rocker left and right keys in progress download view
    if (aKeyEvent.iScanCode == EStdKeyLeftArrow || 
    	aKeyEvent.iScanCode == EStdKeyRightArrow ) 
    	{
    	return EKeyWasConsumed;
    	}
    return CMPXCommonPlaybackViewContainer::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXPdPlaybackViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;  
    aContext.iContext = KMUS_HLP_EMBED_MUSIC_SHOP; 
    }


//  End of File
