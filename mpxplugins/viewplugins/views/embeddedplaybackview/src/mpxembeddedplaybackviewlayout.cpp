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
* Description:  Layout and graphic factory for Playback View
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
#include <mpxembeddedplaybackview.rsg>
#include <mpxembeddedplaybackview.mbg>
#include <centralrepository.h>
#include <mpxinternalcrkeys.h>
#include <mpxlbxextendedfeatures.h>
#include <mpxuser.h>
#include "mpxembeddedplaybackviewlayout.h"
#include "mpxlog.h"

using namespace AknLayoutScalable_Apps;

// CONSTANTS
_LIT( KMPXPodcastPlaybackViewIconFile, "mpxembeddedplaybackview.mbm" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewLayout* CMPXEmbeddedPlaybackViewLayout::NewL()
    {
    CMPXEmbeddedPlaybackViewLayout* self = CMPXEmbeddedPlaybackViewLayout::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewLayout* CMPXEmbeddedPlaybackViewLayout::NewLC()
    {
    CMPXEmbeddedPlaybackViewLayout* self = new ( ELeave ) CMPXEmbeddedPlaybackViewLayout();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewLayout::~CMPXEmbeddedPlaybackViewLayout()
    {
    // Do nothing
    if(iCommonUiHelper)
        {
        delete iCommonUiHelper;
        iCommonUiHelper = NULL;
        }
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXEmbeddedPlaybackViewLayout::CMPXEmbeddedPlaybackViewLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXEmbeddedPlaybackViewLayout::ConstructL()
    {
    MPX_FUNC( "CMPXEmbeddedPlaybackViewLayout::ConstructL" );
    iCommonUiHelper = CMPXCommonUiHelper::NewL();
    }

// ---------------------------------------------------------------------------
// Get the bitmap and mask for a given indicator.
// ---------------------------------------------------------------------------
//
CGulIcon* CMPXEmbeddedPlaybackViewLayout::GetIndicatorIconMaskL(
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
            if( iCommonUiHelper &&
                iCommonUiHelper->IsHostPodcastingAppL())
                {
                return AknsUtils::CreateGulIconL(
                    skin,
                    KAknsIIDNone,
                    iconFile,
                    EMbmMpxembeddedplaybackviewQgn_indi_mup_default_album_pcast,
                    EMbmMpxembeddedplaybackviewQgn_indi_mup_default_album_pcast_mask );
                }
            else
                {
                return CMPXCommonPlaybackViewLayout::GetIndicatorIconMaskL(
                aIndicator );
                }
            }
        default:
            {
            return CMPXCommonPlaybackViewLayout::GetIndicatorIconMaskL(
                aIndicator );
            }
        }
    }
//  End of File
