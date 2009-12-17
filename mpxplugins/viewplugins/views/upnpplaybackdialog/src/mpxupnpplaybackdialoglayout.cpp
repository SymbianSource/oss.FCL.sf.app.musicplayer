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
* Description:  Layout and graphic factory for Upnp Playback Dialog
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
#include <mpxcommonplaybackview.mbg>
#include <mpxcommonplaybackview.rsg>
#include <mpxupnpplaybackview.rsg>
#include <mpxinternalcrkeys.h>
#include <mpxlbxextendedfeatures.h>
#include <mpxuser.h>

#include "mpxupnpplaybackdialoglayout.h"
#include "mpxlog.h"

using namespace AknLayoutScalable_Apps;


// CONSTANTS
_LIT(KMPXUpnpPlaybackDialogIconFile, "mpxcommonplaybackview.mbm");


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogLayout* CMPXUPnPPlaybackDialogLayout::NewL()
    {
    CMPXUPnPPlaybackDialogLayout* self = CMPXUPnPPlaybackDialogLayout::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogLayout* CMPXUPnPPlaybackDialogLayout::NewLC()
    {
    CMPXUPnPPlaybackDialogLayout* self = new ( ELeave ) CMPXUPnPPlaybackDialogLayout();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogLayout::~CMPXUPnPPlaybackDialogLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXUPnPPlaybackDialogLayout::CMPXUPnPPlaybackDialogLayout()
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogLayout::ConstructL()
    {
    MPX_FUNC( "CMPXUPnPPlaybackDialogLayout::ConstructL" );
    }

// ---------------------------------------------------------------------------
// Get layout data for an graphical indicator.
// ---------------------------------------------------------------------------
//
TRect CMPXUPnPPlaybackDialogLayout::IndicatorLayout(
    const TRect& aParentRect,
    TMPXPbvIndicator aIndicator )
    {
    TInt resId(KErrNotFound);

    // Get Rocker mapping support flag
    TInt flags( 0 );
    CRepository* repository( NULL );
    MPX_TRAPD( err, repository = CRepository::NewL( KCRUidMPXMPFeatures ));
    if ( KErrNone == err )
        {
        repository->Get( KMPXMPLocalVariation, flags );
        delete repository;
        repository = NULL;
        }

    TBool enableButton( flags & KMPXRockerMappingSupport );
    const TSize screenSize( CCoeEnv::Static()->ScreenDevice()->SizeInPixels() );

    if ( screenSize == TSize( 176,208 ))    //lowest portrait
        {
        switch ( aIndicator )
            {
            case EIconRemotePlayer:
                {
                if ( enableButton )
                    {
                    resId = R_MPX_PBV_REMOTE_INDI_LAYOUT;
                    }
                else
                    {
                    resId = R_MPX_PBV_REMOTE_INDI_LAYOUT_WITHOUT_ROCKER;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( screenSize == TSize( 352,416 ))
        {
        switch ( aIndicator )
            {
            case EIconRemotePlayer:
                {
                resId = R_MPX_PBV_REMOTE_INDI_LAYOUT_DOUBLE_PORTRAIT;
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( screenSize == TSize( 240,320 ))
        {
        switch ( aIndicator )
            {
            case EIconRemotePlayer:
                {
                if ( enableButton )
                    {
                    resId = R_MPX_PBV_REMOTE_INDI_LAYOUT_QVGA_PORTRAIT;
                    }
                else
                    {
                    resId = R_MPX_PBV_REMOTE_INDI_LAYOUT_QVGA_PORTRAIT_WITHOUT_ROCKER;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( screenSize == TSize( 320,240 )) // QVGA landscape
        {
        switch ( aIndicator )
            {
            case EIconRemotePlayer:
                {
                if ( enableButton )
                    {
                    resId = R_MPX_PBV_REMOTE_INDI_LAYOUT_QVGA_LANDSCAPE;
                    }
                else
                    {
                    resId = R_MPX_PBV_REMOTE_INDI_LAYOUT_QVGA_LANDSCAPE_WITHOUT_ROCKER;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }

    TRect rect;
    if ( resId != KErrNotFound )
        {
        TAknLayoutRect res;
        res.LayoutRect( aParentRect, resId );
        rect = res.Rect();
        }
    else
        {
        rect = CMPXCommonPlaybackViewLayout::IndicatorLayout( aParentRect, aIndicator );
        }

    return rect;
    }

// ---------------------------------------------------------------------------
// Get the bitmap and mask for a given indicator.
// ---------------------------------------------------------------------------
//
CGulIcon* CMPXUPnPPlaybackDialogLayout::GetIndicatorIconMaskL(
    TMPXPbvIndicator aIndicator )
    {

    TParse parse;
    parse.Set( KMPXUpnpPlaybackDialogIconFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    switch ( aIndicator )
        {
        case EIconRemotePlayer:
            {
            return CreateColorIconL(
                skin,
                KAknsIIDQgnIndiMupRandom,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG26,
                iconFile,
                EMbmMpxcommonplaybackviewQgn_graf_mup_icon_upnp_remote,
                EMbmMpxcommonplaybackviewQgn_graf_mup_icon_upnp_remote_mask );
            }
        default:
            {
            break;
            }
        }

    return CMPXCommonPlaybackViewLayout::GetIndicatorIconMaskL( aIndicator );
    }

//  End of File
