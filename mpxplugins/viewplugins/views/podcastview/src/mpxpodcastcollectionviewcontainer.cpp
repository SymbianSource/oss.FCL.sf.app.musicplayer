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
* Description:  Implementation of Podcast Collection view container
*
*/


// INCLUDE FILES
#include <hlplch.h>
#include <aknappui.h>
#include <akncontext.h>
#include <AknIconUtils.h>
#include <data_caging_path_literals.hrh>
#include <eikspane.h>
#include <eiktxlbx.h>
#include <mpxcollectionview.mbg>
#include <mpxplaybackframeworkdefs.h>
#include <mpxconstants.h>
#include <mpxlog.h>
#include <eikon.hrh>

#include "mpxpodcastcollectionview.hrh"
#include "mpxpodcastcollectionviewlistboxarray.h"
#include "mpxpodcastcollectionview.hlp.hrh"
#include "mpxpodcastcollectionviewcontainer.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewContainer* CMPXPodcastCollectionViewContainer::NewL(
    MEikCommandObserver* aView,
    MEikListBoxObserver* aObserver )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::NewL" );
    CMPXPodcastCollectionViewContainer* self =
        new ( ELeave ) CMPXPodcastCollectionViewContainer(aView, aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewContainer::CMPXPodcastCollectionViewContainer(
    MEikCommandObserver* aView,
    MEikListBoxObserver* aObserver )
    : iView( aView )
    , iListBoxObserver( aObserver )
    {
    iCurrentView = EMPXMainPodcastMenuView;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewContainer::~CMPXPodcastCollectionViewContainer()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::~CMPXPodcastCollectionViewContainer" );
    delete iCommonContainer;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewContainer::ConstructL()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::ConstructL" );
    iCommonContainer = CMPXCommonContainerFactory::NewL();
    iCommonContainer->SetViewContainer( this );
    iCommonContainer->SetCommandObserver( iView );
    iCommonContainer->SetListBoxObserver( iListBoxObserver );
    iCommonContainer->ConstructContainerL(MMPXCommonContainer::EMPXCommonContainerPodcast);
    iLbxArray = static_cast<CMPXPodcastCollectionViewListBoxArray*>(
        CreateListBoxArrayL() );
    iCommonContainer->SetListBoxArrayL( iLbxArray );
    iCommonContainer->HandleListBoxArrayEventL(
        MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventIconArrayChange );
    }

// ---------------------------------------------------------------------------
// Sets playback status
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewContainer::SetPlaybackStatusByIdL(
    TMPXItemId aId, TMPXPlaybackState aStatus )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::SetPlaybackStatusByIdL" );
    if ( iLbxArray && iCommonContainer )
        {
        TInt index( iLbxArray->SetPlaybackStatusByIdL( aId, aStatus ) );
        if ( index != KErrNotFound )
            {
            // Update the previous item as not playing anymore
            iCommonContainer->DrawLbxItemL( index );
            }
        index = iLbxArray->GetPlaybackIndex();
        if ( index != KErrNotFound )
            {
            // Update new item as playing
            iCommonContainer->DrawLbxItemL( index );
            }
        }
    }

// ---------------------------------------------------------------------------
// Get playback index
// ---------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewContainer::PlaybackIndex()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::PlaybackIndex" );
    return ( iLbxArray ? iLbxArray->GetPlaybackIndex() : KErrNotFound );
    }

// ---------------------------------------------------------------------------
// Sets the current view to be used to determine the correct help context
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewContainer::SetCurrentView(
    const TMPXPodcastView& currentView )
    {
    iCurrentView = currentView;
    }

// -----------------------------------------------------------------------------
// Creates listbox array
// -----------------------------------------------------------------------------
//
CMPXCommonListBoxArrayBase* CMPXPodcastCollectionViewContainer::CreateListBoxArrayL()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::CreateListBoxArrayL" );
    CMPXPodcastCollectionViewListBoxArray* array = CMPXPodcastCollectionViewListBoxArray::NewL();
    array->ConstructListBoxArrayL();
    array->SetObserver( this );
    return array;
    }

// ---------------------------------------------------------------------------
// From MMPXViewContainer
// Handles the retrieval of the view container's help context.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewContainer::HandleHelpContext(
    TCoeHelpContext& aContext ) const
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::HandleHelpContext" );
    aContext.iMajor = KAppUidMusicPlayerX;

    switch(iCurrentView)
        {
        case EMPXMainPodcastMenuView:
            {
            aContext.iContext = KMUS_HLP_PODCAST_MENU_VIEW;
            break;
            }
        case EMPXPublishDateView:
            {
            aContext.iContext = KMUS_HLP_PODCAST_PUBLISH_DATE_CAT_VIEW;
            break;
            }
        case EMPXTitlesView:
            {
            aContext.iContext = KMUS_HLP_PODCAST_MENU_VIEW;
            break;
            }
        case EMPXEpisodesView:
            {
            aContext.iContext = KMUS_HLP_PODCAST_EPISODES_VIEW;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXViewContainer
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXPodcastCollectionViewContainer::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    ASSERT( iCommonContainer );
    MPX_DEBUG4( "CMPXPodcastCollectionViewContainer::HandleKeyEventL(iCode=%d, iScanCode=%d, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    if ( aKeyEvent.iCode == EKeyUpArrow  ||
        aKeyEvent.iCode == EKeyPrevious )
        {
        iView->ProcessCommandL( EMPXCmdUpArrow );
        }
    else if ( aKeyEvent.iCode == EKeyDownArrow ||
        aKeyEvent.iCode == EKeyNext)
        {
        iView->ProcessCommandL( EMPXCmdDownArrow );
        }

    TKeyResponse response = EKeyWasNotConsumed;
    // Hashkey mark/unmark only works in episodes view
    if( iCurrentView == EMPXEpisodesView ||
        aKeyEvent.iScanCode != EStdKeyHash )
        {
        response = iCommonContainer->HandleKeyEventL( aKeyEvent, aType );
        }

    return response;
    }

// ---------------------------------------------------------------------------
// From MMPXViewContainer
// Returns the indicators for the specified item within the view container
// Only used when using Hitchcock container.
// ---------------------------------------------------------------------------
//
RArray<TInt> CMPXPodcastCollectionViewContainer::IndicatorsL( TInt aIndex )
    {
        return iLbxArray->IndicatorIconIndicesL( aIndex );
    }

// -----------------------------------------------------------------------------
// Returns common container
// -----------------------------------------------------------------------------
//
MMPXCommonContainer* CMPXPodcastCollectionViewContainer::Common()
    {
    return iCommonContainer;
    }

// ---------------------------------------------------------------------------
// From MMPXCommonListBoxArrayObserver
// Handle listbox array events.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewContainer::HandleListBoxArrayEventL(
    TMPXCommonListBoxArrayEvents aEvent )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewContainer::HandleListBoxArrayEventL" );
    ASSERT( iCommonContainer );
    iCommonContainer->HandleListBoxArrayEventL( aEvent );
    }

//  End of File
