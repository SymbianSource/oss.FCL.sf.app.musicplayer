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
* Description:  Layout and graphic factory for Podcast Playback View
*
*/



// INCLUDE FILES
#include <AknsUtils.h>
#include <AknUtils.h>
#include <barsread.h>
#include <gulicon.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include <mpxuser.h>

#include <mpxpodcastplaybackview.mbg>
#include "mpxpodcastplaybackviewlayout.h"
#include "mpxlog.h"


// CONSTANTS
_LIT( KMPXPodcastPlaybackViewIconFile, "mpxpodcastplaybackview.mbm" );


using namespace AknLayoutScalable_Apps;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewLayout* CMPXPodcastPlaybackViewLayout::NewL()
    {
    CMPXPodcastPlaybackViewLayout* self = CMPXPodcastPlaybackViewLayout::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewLayout* CMPXPodcastPlaybackViewLayout::NewLC()
    {
    CMPXPodcastPlaybackViewLayout* self = new ( ELeave ) CMPXPodcastPlaybackViewLayout();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewLayout::~CMPXPodcastPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewLayout::CMPXPodcastPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewLayout::ConstructL()
    {
    MPX_FUNC( "CMPXPodcastPlaybackViewLayout::ConstructL" );
    InitCommonLayoutL();
    }

// ---------------------------------------------------------------------------
// Get the bitmap and mask for a given indicator.
// ---------------------------------------------------------------------------
//
CGulIcon* CMPXPodcastPlaybackViewLayout::GetIndicatorIconMaskL(
    TMPXPbvIndicator aIndicator )
    {
    TParse parse;
    parse.Set( KMPXPodcastPlaybackViewIconFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    switch ( aIndicator )
        {
        case EAlbumArtArea:
            {
            return AknsUtils::CreateGulIconL(
                skin,
                KAknsIIDQgnIndiMupDefaultAlbumPcast,
                iconFile,
                EMbmMpxpodcastplaybackviewQgn_indi_mup_default_album_pcast,
                EMbmMpxpodcastplaybackviewQgn_indi_mup_default_album_pcast_mask );
            }
        default:
            {
            return CMPXCommonPlaybackViewLayout::GetIndicatorIconMaskL(
                aIndicator );
            }
        }
    }

//  End of File
