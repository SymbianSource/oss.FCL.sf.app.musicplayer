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
* Description:  Implementation of Audio effects view
*
*/



// INCLUDE FILES
#include <bautils.h>
#include <eikmenub.h>
#include <AknUtils.h>
#include <hlplch.h>
#include <data_caging_path_literals.hrh>
#include <avkon.rsg>
#include <mpxuser.h>

#include <mpxplaybackutility.h>
#include <mpxaudioeffectsview.rsg>
#include <mpxviewutility.h>
#include "mpxaudioeffectengine.h"

#include "mpxaudioeffectsmodel.h"
#include "mpxaudioeffectsviewcontainer.h"
#include "mpxaudioeffectsviewimp.h"
#include "mpxaudioeffects.hrh"


// CONSTANTS
_LIT( KMPXAudioEffectsRscPath, "mpxaudioeffectsview.rsc" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXAudioEffectsViewImp::CMPXAudioEffectsViewImp()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXAudioEffectsViewImp::ConstructL()
    {
    //get resource file
    TParse parse;
    parse.Set( KMPXAudioEffectsRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile(
        CCoeEnv::Static()->FsSession(), resourceFile );
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( resourceFile );
    BaseConstructL( R_AUDIOEFFECTS_SETTINGSLIST_VIEW );

    // Get the playback utility instance from engine
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );

    iModel = CMPXAudioEffectsModel::NewL( *iPlaybackUtility );
        
    iViewUtility = MMPXViewUtility::UtilityL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXAudioEffectsViewImp* CMPXAudioEffectsViewImp::NewL()
    {
    CMPXAudioEffectsViewImp* self = CMPXAudioEffectsViewImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXAudioEffectsViewImp* CMPXAudioEffectsViewImp::NewLC()
    {
    CMPXAudioEffectsViewImp* self = new ( ELeave ) CMPXAudioEffectsViewImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXAudioEffectsViewImp::~CMPXAudioEffectsViewImp()
    {
    delete iModel;

    if ( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }

    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }

    if ( iViewUtility )
        {
        iViewUtility->Close();
        }

    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsViewImp::ChangeSelectedItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsViewImp::ChangeSelectedItemL()
    {
    if( iContainer )
        {
        iContainer->ChangeSelectedItemL();
        }
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsViewImp::Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CMPXAudioEffectsViewImp::Id() const
    {
    return TUid::Uid( KMPXAudioEffectsViewId );
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsViewImp::HandleCommandL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsViewImp::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case ESettingsListCmdChange:
            {
            ChangeSelectedItemL();
            break;
            }
        case EAknCmdHelp:
        case ESettingsListCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(),
            AppUi()->AppHelpContextL());
            break;
            }
        case EAknSoftkeyBack:
            {
            iViewUtility->ActivatePreviousViewL();
            break;
            }
        default:
            {
            // Pass the command to AppUi to handle.
            AppUi()->HandleCommandL( aCommand );
            break;
            }
         }
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsViewImp::DoActivateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsViewImp::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    StatusPane()->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
    iPreviousView = aPrevViewId;

    if (!iContainer)
        {
        iContainer = new ( ELeave ) CMPXAudioEffectsViewContainer();
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect(), iModel );
        AppUi()->AddToStackL( *this, iContainer );
        }
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsViewImp::DoDeactivate
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsViewImp::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXAudioEffectsViewImp::ProcessCommandL( TInt aCommand )
    {
    MPX_DEBUG2("ProcessCommandL::ProcessCommandL(%d): entering", aCommand);
    if ( aCommand == EAknCmdExit )
        {
        HandleCommandL( aCommand );
        }
    CAknView::ProcessCommandL( aCommand );
    MPX_DEBUG1("ProcessCommandL::ProcessCommandL(): exiting");
    }

// End of File
