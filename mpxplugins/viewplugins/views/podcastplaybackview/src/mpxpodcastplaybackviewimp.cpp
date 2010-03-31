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
* Description:  Implementation of Podcast playback view
*
*/



// INCLUDE FILES
#include <StringLoader.h>
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <mpxpodcastplaybackview.rsg>
#include <mpxcommonplaybackview.rsg>
#include <featmgr.h>

#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionplaylist.h>
#include <mpxmedia.h>
#include <mpxviewutility.h>
#include <mpxviewpluginmanager.h>
#include <mpxmediamusicdefs.h>
#include <mpxpodcastdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxplaybackutility.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxuser.h>
#include <mpxappui.hrh>
#include "mpxcommonplaybackview.hrh"
#include "mpxpodcastplaybackviewimp.h"
#include "mpxpodcastplaybackviewcontainer.h"
#include "mpxpodcastplaybackviewlayout.h"
#include "mpxpodcastplaybackview.hrh"
#include "mpxlog.h"

#include "mpxcommonplaybackview.hrh"

#include <akntoolbar.h>
#include <aknbutton.h>
// CONSTANTS
_LIT( KMPXPodcastPlaybackRscPath, "mpxpodcastplaybackview.rsc" );
const TInt KPodcastingCollectionID = 0x101FFC3C;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewImp::CMPXPodcastPlaybackViewImp()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::ConstructL()
    {
    MPX_FUNC( "CMPXPodcastPlaybackViewImp::ConstructL" );

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXPodcastPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iPodcastResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    CMPXCommonPlaybackViewImp::ConstructL();

    BaseConstructL( R_MPX_PODCAST_PLAYBACK_VIEW );
    if(AknLayoutUtils::PenEnabled())
    {
    if ( Toolbar() )
        {
        iToolbar = Toolbar();
        iToolbar->SetToolbarObserver( this );
        }
    }

    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewImp* CMPXPodcastPlaybackViewImp::NewL()
    {
    CMPXPodcastPlaybackViewImp* self = CMPXPodcastPlaybackViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewImp* CMPXPodcastPlaybackViewImp::NewLC()
    {
    CMPXPodcastPlaybackViewImp* self = new ( ELeave ) CMPXPodcastPlaybackViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPodcastPlaybackViewImp::~CMPXPodcastPlaybackViewImp()
    {
    MPX_DEBUG1( "CMPXPodcastPlaybackViewImp::~CMPXPodcastPlaybackViewImp entering" );

    delete iPlaybackViewLayout;

    if ( iPodcastResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iPodcastResourceOffset );
        }

    MPX_DEBUG1( "CMPXPodcastPlaybackViewImp::~CMPXPodcastPlaybackViewImp exiting" );
    }

// ---------------------------------------------------------------------------
// From base class CMPXCommonPlaybackViewImp
//
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXPodcastPlaybackViewImp::DoHandlePropertyL" );
    MPX_DEBUG4( "HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    if ( KErrNone == aError )
        {
        switch ( aProperty  )
            {
            case EPbPropertyPosition:
                {
                if (iPosition == 0 && aValue != 0)
                    {
                    MMPXSource* s = iPlaybackUtility->Source();
                    if (s)
                        {
                        // Trick to make the song played
                        CMPXCollectionPlaylist* playlist = s->PlaylistL();
                        CleanupStack::PushL(playlist);
                        if ( playlist )
                            {
                            CMPXMedia* media = CMPXMedia::NewL();
                            CleanupStack::PushL(media);
                            media->SetTObjectValueL( KMPXMediaGeneralType,
                                                     EMPXItem );
                            media->SetTObjectValueL( KMPXMediaGeneralCategory,
                                                     EMPXSong );
                            media->SetTObjectValueL( KMPXMediaGeneralCollectionId,
                                                     playlist->Path().Id( CMPXCollectionPath::ECollectionUid ).iId1);

                            media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId , playlist->Path().Id() );
                            media->SetTObjectValueL( KMPXMediaGeneralLastPlaybackPosition, aValue );
                            playlist->SetL( *media );
                            CleanupStack::PopAndDestroy(media);
                            }
                        CleanupStack::PopAndDestroy(playlist);
                        }
                    }
                iPosition = aValue;
                CMPXCommonPlaybackViewImp::DoHandlePropertyL(aProperty, aValue, aError);
                break;
                }
            default:
                {
                CMPXCommonPlaybackViewImp::DoHandlePropertyL(aProperty,
                    aValue,
                    aError);
                break;
                }
            }
        }
    else
        {
        HandleErrorL( aError );
        }
    }

// ---------------------------------------------------------------------------
// Updates track info field.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::UpdateTrackInfoL(
    const CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXPodcastPlaybackViewImp::UpdateTrackInfo" );

    CMPXCommonPlaybackViewImp::UpdateTrackInfoL( aMedia );

    if( iContainer && !iSwitchingView )
        {
        if ( aMedia )
            {
            // Set ETextArtist field as Podcast Title
            if ( aMedia->IsSupported(KMPXMediaMusicAlbum))
                {
                HBufC* titleText = aMedia->ValueText(KMPXMediaMusicAlbum).AllocL();
                CleanupStack::PushL(titleText);
                if ( titleText->Length() > 0)
                    {
                    iContainer->UpdateLabelL( ETextArtist, *titleText );
                    }
                else
                    {
                    HBufC* unknownText =
                        StringLoader::LoadLC( R_MPX_QTN_NMP_UNKNOWN_TITLE );
                    iContainer->UpdateLabelL(
                        ETextArtist, *unknownText );
                    CleanupStack::PopAndDestroy( unknownText );
                    }
                CleanupStack::PopAndDestroy( titleText );
                }
            else
                {
                HBufC* unknownText =
                        StringLoader::LoadLC( R_MPX_QTN_NMP_UNKNOWN_TITLE );
                iContainer->UpdateLabelL(
                    ETextArtist, *unknownText );
                CleanupStack::PopAndDestroy( unknownText );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    switch( aResourceId )
        {
        case R_MPX_PODCAST_PLAYBACK_VIEW_MENU1:
            {
            break;
            }
        case R_MPX_PLAYBACK_VIEW_MENU:
            {
            CMPXCommonPlaybackViewImp::DynInitMenuPaneL( aResourceId, aMenuPane );

            // dim the options that aren't applicable
            // to podcast collection
            aMenuPane->SetItemDimmed(
                    EMPXPbvCmdGotoMusicMenu,
                    ETrue );
            aMenuPane->SetItemDimmed(
                    EMPXPbvCmdAddToPlaylist,
                    ETrue );
            aMenuPane->SetItemDimmed(
                EMPXPbvCmdUseAsCascade,
                ETrue );
            aMenuPane->SetItemDimmed(
                    EMPXPbvCmdRepeat,
                    ETrue );
            aMenuPane->SetItemDimmed(
                    EMPXPbvCmdRandom,
                    ETrue );
            aMenuPane->SetItemDimmed(
                    EMPXPbvCmdSongDetails,
                    ETrue );

			// Check if FM Transmitter is supported
			if ( !FeatureManager::FeatureSupported( KFeatureIdFmtx ) )
				{
				aMenuPane->SetItemDimmed(
					EMPXPbvCmdFMTransmitter,
					ETrue );
				}
            break;
            }
        default:
            {
            // Call base clase to handle most cases
            CMPXCommonPlaybackViewImp::DynInitMenuPaneL(
                aResourceId,
                aMenuPane );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXPodcastPlaybackViewImp::Id() const
    {
    return TUid::Uid( KMPXPodcastPlaybackViewId );
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXPodcastPlaybackViewImp::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EMPXPbvCmdGoToPodcastMenu:
            {
            // Handle Goto main podcast menu
            CMPXCollectionPath* cpath = iCollectionUiHelper->PodCastMenuPathL();
            CleanupStack::PushL( cpath );
            MPX_DEBUG_PATH (*cpath);
            iCollectionUtility->Collection().OpenL( *cpath );
            CleanupStack::PopAndDestroy( cpath );

            // Restore status pane
            if ( StatusPane()->CurrentLayoutResId() !=
                 R_AVKON_STATUS_PANE_LAYOUT_USUAL)
                {
                StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
                }
            break;
            }
        case EAknSoftkeyBack:
            {
            MMPXSource* s = iPlaybackUtility->Source();
            TBool handled = EFalse;
            if(s == NULL)
                {
                CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL(path);
                TInt levels = path->Levels();
                if(levels == 3)
                    {
                    const TMPXItemId& iid0 = path->Id(0);
                    const TMPXItemId& iid1 = path->Id(1);
                    if(iid0 == KPodcastingCollectionID && iid1 == 4) //EBrowseNotPlayed
                        {
                        path->Back();
                        iViewUtility->PushDefaultHistoryL();
                        iCollectionUtility->Collection().OpenL(*path);
                        handled = ETrue;
                        }
                    }
                CleanupStack::PopAndDestroy(path);
                }
            if(!handled)
                {
                CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
                }
            break;
            }
        case EMPXPbvCmdPodcastDetails:
            {
            CMPXCommonPlaybackViewImp::LaunchFileDetailsDialogL();
			break;
            } 
        default:
            {
            CMPXCommonPlaybackViewImp::HandleCommandL( aCommand );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    MPX_FUNC( "CMPXPodcastPlaybackViewImp::DoActivateL()" );

    if ( !iContainer )
        {
        if ( !iPlaybackViewLayout )
            {
            iPlaybackViewLayout = CMPXPodcastPlaybackViewLayout::NewL();
            }
        iContainer = new ( ELeave ) CMPXPodcastPlaybackViewContainer(
            this,
            this,
            iPlaybackViewLayout );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }

    CMPXCommonPlaybackViewImp::DoActivateL( aPrevViewId , aCustomMessageId, aCustomMessage );
    UpdateMiddleSoftKeyDisplayL();
    }

// -----------------------------------------------------------------------------
// CMPXPodcastPlaybackViewImp::PrepareStatusPaneForExitingViewL
// -----------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::PrepareStatusPaneForExitingViewL()
    {
    if ( StatusPane()->CurrentLayoutResId() !=
         R_AVKON_STATUS_PANE_LAYOUT_USUAL)
        {
        StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }
    }
// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Used to change toolbar settings before toolbar becomes visible
// -----------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar)
    {
    if (aResourceId == R_MPX_PBV_TOUCH_TOOLBAR)
        {
        // Put initializations here to take effect before toolbar is shown
        UpdateToolbar();
        aToolbar->UpdateBackground();
       }
    }

// -----------------------------------------------------------------------------
// From MAknToolbarObserver
// Handles toolbar events
// -----------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::OfferToolbarEventL( TInt aCommandId )
    {
    TInt commandId = aCommandId;
    TInt eventModifiers = iToolbar->EventModifiers();
    if ( eventModifiers == CAknToolbar::ELongPress )
        {
        if ( aCommandId == EMPXPbvCmdNextListItem )
            {
            commandId = EMPXPbvCmdSeekForward;
            }
        else if ( aCommandId == EMPXPbvCmdPreviousListItem )
            {
            commandId = EMPXPbvCmdSeekBackward;
            }
        }
    else if ( eventModifiers == CAknToolbar::ELongPressEnded )
        {
        if ( aCommandId == EMPXPbvCmdNextListItem || aCommandId == EMPXPbvCmdPreviousListItem )
            {
            commandId = EMPXPbvCmdEndSeek;
            }
        }

    CAknView::ProcessCommandL( commandId );
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_DEBUG1( "CMPXPodcastPlaybackViewImp::DoHandlePlaybackMessageL");

    TMPXMessageId id( *aMessage.Value<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( *aMessage.Value<TInt>( KMPXMessageGeneralType ) );
        TInt data( *aMessage.Value<TInt>( KMPXMessageGeneralData ) );
        switch ( *aMessage.Value<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EStateChanged:
                UpdateMiddleSoftKeyDisplayL();
                UpdateToolbar();
                break;

            default:
                break;
            }
        }
    CMPXCommonPlaybackViewImp::DoHandlePlaybackMessageL (aMessage);
    }

// ---------------------------------------------------------------------------
// Updates the middle toolbar button
// ---------------------------------------------------------------------------
//
void CMPXPodcastPlaybackViewImp::UpdateToolbar()
    {
    MPX_FUNC("CMPXPodcastPlaybackViewImp::UpdateToolbar");

    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( iToolbar )
            {
            CAknButton* pausePlayControl;
            pausePlayControl = (CAknButton*)(iToolbar->ComponentControl( 1 ));
            MMPXSource* s = iPlaybackUtility->Source();
            if ( s )
                {
                CMPXCollectionPlaylist* playlist = s->PlaylistL();
                if ( playlist )
                    {
                    CleanupStack::PushL( playlist );
                    if ( playlist->Count() > 0 && pausePlayControl )
                        {
                        TMPXPlaybackState state = iPlaybackUtility->StateL();
                        if ((state == EPbStateInitialising) || (state == EPbStatePlaying))
                           {
                           pausePlayControl->SetCurrentState(0, ETrue);
                           }
                        else
                           {
                           pausePlayControl->SetCurrentState(1, ETrue);
                           }
                        }
                    CleanupStack::PopAndDestroy( playlist );
                    }
                }
            }
        }
    }

//  End of File
