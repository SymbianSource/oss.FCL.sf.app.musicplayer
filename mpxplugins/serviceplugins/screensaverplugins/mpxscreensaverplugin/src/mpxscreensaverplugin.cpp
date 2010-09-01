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
* Description:  Implementation of Screen Saver Plugin
*
*/


// INCLUDE FILES

#include <eikenv.h>
#include <bautils.h>
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileName.h>
#include <Profile.hrh> // profile ID
#include <StringLoader.h>

#include <mpxscreensaverplugin.rsg>
#include <mpxscreensaverplugin.mbg>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxlog.h>
#include <mpxuser.h>

#include "mpxscreensaverplugin.hrh"
#include "mpxscreensaverplugincontainer.h"
#include "mpxscreensaverplugin.h"

// Constants
const TInt KTimerInterval = 30000000; // 30 seconds
const TInt KTimerForceNextUpdateInterval = 1000; // 1 milli second

_LIT( KResourceFileName, "mpxscreensaverplugin.rsc" );

// ---------------------------------------------------------------------------
// CMPXScreenSaverPlugin::NewL
// ---------------------------------------------------------------------------
//
CMPXScreenSaverPlugin* CMPXScreenSaverPlugin::NewL()
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::NewL" );
    CMPXScreenSaverPlugin* self = new ( ELeave ) CMPXScreenSaverPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::~CMPXScreenSaverPlugin
// ----------------------------------------------------------------------------
//
CMPXScreenSaverPlugin::~CMPXScreenSaverPlugin()
    {
    MPX_DEBUG1( "CMPXScreenSaverPlugin::~CMPXScreenSaverPlugin START" );
    if ( iResourceOffset != 0 )
        {
        iEikEnv->DeleteResourceFile( iResourceOffset );
        }
    if ( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }

    if ( iProfileEngine )
        {
        iProfileEngine->Release();
        }

    if ( iContainer )
        {
        delete iContainer;
        }

    delete iDisplayName;

    MPX_DEBUG1( "CMPXScreenSaverPlugin::~CMPXScreenSaverPlugin END" );
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::InitializeL
// -----------------------------------------------------------------------------
//
TInt CMPXScreenSaverPlugin::InitializeL( MScreensaverPluginHost *aHost )
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::InitializeL" );
    if ( !aHost )
        {
        return KErrCancel;
        }
    iScreenSaverHost = aHost;
    iScreenSaverHost->OverrideStandardIndicators();
    iScreenSaverHost->UseRefreshTimer( ETrue );
    iScreenSaverHost->SetRefreshTimerValue( KTimerInterval );

    CreateContainerL();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::Draw
// -----------------------------------------------------------------------------
//
TInt CMPXScreenSaverPlugin::Draw( CWindowGc& aGc )
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::Draw" );

    TInt status = KErrNone;
    TIndicatorPayload result;

    // EScreensaverEventStarting event should come before draw
    // iContainer should be available
    // Check for missed calls
    status = iScreenSaverHost->GetIndicatorPayload(
        EScreensaverIndicatorIndexNewMissedCalls, result );
    MPX_DEBUG2( "CMPXScreenSaverPlugin::Draw status = %d", status );
    MPX_DEBUG4( "CMPXScreenSaverPlugin::Draw EScreensaverIndicatorIndexNewMissedCalls iType = %d, iInteger = %d, iIsDisplayed = %d", result.iType, result.iInteger, result.iIsDisplayed );
    if ( KErrNone == status)
        {
        iContainer->SetParam( MPXScreenSaverPluginParamMissedCall,
            result.iInteger );
        }

    // Check for new messages
    status = iScreenSaverHost->GetIndicatorPayload(
        EScreensaverIndicatorIndexNewMessages, result );
    MPX_DEBUG2( "CMPXScreenSaverPlugin::Draw status = %d", status );
    MPX_DEBUG4( "CMPXScreenSaverPlugin::Draw EScreensaverIndicatorIndexNewMessages iType = %d, iInteger = %d, iIsDisplayed = %d", result.iType, result.iInteger, result.iIsDisplayed );
    if ( KErrNone == status )
        {
        iContainer->SetParam( MPXScreenSaverPluginParamNewMessage,
            result.iInteger );
        }

    // Check for new instant messages
    status = iScreenSaverHost->GetIndicatorPayload(
        EScreensaverIndicatorIndexChatMessage, result );
    MPX_DEBUG2( "CMPXScreenSaverPlugin::Draw status = %d", status );
    MPX_DEBUG4( "CMPXScreenSaverPlugin::Draw EScreensaverIndicatorIndexChatMessage iType = %d, iInteger = %d, iIsDisplayed = %d", result.iType, result.iInteger, result.iIsDisplayed );
    if ( KErrNone == status )
        {
        iContainer->SetParam( MPXScreenSaverPluginParamInstantMessage,
            result.iIsDisplayed ? 1: 0 );
        }

    // Check for new email
    status = iScreenSaverHost->GetIndicatorPayload(
        EScreensaverIndicatorIndexEmail, result );
    MPX_DEBUG2( "CMPXScreenSaverPlugin::Draw status = %d", status );
    MPX_DEBUG4( "CMPXScreenSaverPlugin::Draw EScreensaverIndicatorIndexEmail iType = %d, iInteger = %d, iIsDisplayed = %d", result.iType, result.iInteger, result.iIsDisplayed );
    if ( KErrNone == status )
        {
        iContainer->SetParam( MPXScreenSaverPluginParamEmail,
            result.iIsDisplayed ? 1: 0 );
        }

    // Check for new voice messages
    status = iScreenSaverHost->GetIndicatorPayload(
        EScreensaverIndicatorIndexVoicemail, result );
    MPX_DEBUG2( "CMPXScreenSaverPlugin::Draw status = %d", status );
    MPX_DEBUG4( "CMPXScreenSaverPlugin::Draw EScreensaverIndicatorIndexVoicemail iType = %d, iInteger = %d, iIsDisplayed = %d", result.iType, result.iInteger, result.iIsDisplayed );
    if ( KErrNone == status )
        {
        iContainer->SetParam( MPXScreenSaverPluginParamVoiceMessage,
            result.iIsDisplayed ? 1: 0 );
        }

    // Check for keyguard
    status = iScreenSaverHost->GetIndicatorPayload(
        EScreensaverIndicatorIndexKeyGuardState, result );
    MPX_DEBUG2( "CMPXScreenSaverPlugin::Draw status = %d", status );
    MPX_DEBUG4( "CMPXScreenSaverPlugin::Draw EScreensaverIndicatorIndexKeyGuardState iType = %d, iInteger = %d, iIsDisplayed = %d", result.iType, result.iInteger, result.iIsDisplayed );
    if ( KErrNone == status )
        {
        iContainer->SetParam( MPXScreenSaverPluginParamKeyguard,
            result.iIsDisplayed ? 1: 0 );
        }

    TRAP_IGNORE( iContainer->DrawL( aGc, iPlaybackState ) );

    if ( iTimerChanged )
        {
        MPX_DEBUG1( "CMPXScreenSaverPlugin::Draw Revert back to original timer" );
        iScreenSaverHost->SetRefreshTimerValue( KTimerInterval );
        iScreenSaverHost->UseRefreshTimer( ETrue );
        iTimerChanged = EFalse;
        }

    return status;
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::Name
// -----------------------------------------------------------------------------
//
const TDesC16& CMPXScreenSaverPlugin::Name() const
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::Name" );
    if ( iDisplayName )
        {
        return *iDisplayName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::Capabilities
// -----------------------------------------------------------------------------
//
TInt CMPXScreenSaverPlugin::Capabilities()
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::Capabilities" );
    return EScpCapsNone;
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::PluginFunction
// -----------------------------------------------------------------------------
//
TInt CMPXScreenSaverPlugin::PluginFunction(
            TScPluginCaps /*aFunction*/,
            TAny* /*aParam*/)
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::PluginFunction" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::HandleScreensaverEventL
// -----------------------------------------------------------------------------
//
TInt CMPXScreenSaverPlugin::HandleScreensaverEventL(
            TScreensaverEvent aEvent,
            TAny* /*aData*/ )
    {
    MPX_DEBUG2( "-->CMPXScreenSaverPlugin::HandleScreensaverEventL event = %d", aEvent );

    if ( aEvent == EScreensaverEventDisplayChanged )
        {
        if ( iContainer )
            {
            iContainer->RefreshDisplayInfo();
            }
        }
    else if ( aEvent == EScreensaverEventStarting )
        {
        if ( !iContainer )
            {
            CreateContainerL();
            }
        iContainer->RequestTimeout( 0 );
        MProfile* profile = iProfileEngine->ActiveProfileL();
        if( profile )
            {
            if ( iProfileEngine->ActiveProfileId() != EProfileGeneralId )
                {
                // active profile is not general profile
                const MProfileName& profileName = profile->ProfileName();
                TRAP_IGNORE( iContainer->SetProfileNameL( profileName.Name(), ETrue ) );
                }
            else
                {
                TRAP_IGNORE( iContainer->SetProfileNameL( KNullDesC, EFalse ) );
                }
            iContainer->SetParam( MPXScreenSaverPluginParamIsSilent,
                profile->IsSilent() );
            profile->Release();
            }
        profile = NULL;
        }
    else if ( aEvent == EScreensaverEventStopping )
        {
        delete iContainer;
        iContainer = NULL;
        }
    MPX_DEBUG1( "<--CMPXScreenSaverPlugin::HandleScreensaverEventL" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::CMPXScreenSaverPlugin
// -----------------------------------------------------------------------------
//
CMPXScreenSaverPlugin::CMPXScreenSaverPlugin():
    iPlaybackState( EPbStateNotInitialised )
    {
    }

// -----------------------------------------------------------------------------
// CMPXScreenSaverPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::ConstructL()
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::ConstructL" );
    iEikEnv = CEikonEnv::Static();
    TParse lParse;
    lParse.Set( KResourceFileName, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile( lParse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );

    BaflUtils::NearestLanguageFile( iEikEnv->FsSession(), resourceFile );
    iResourceOffset = iEikEnv->AddResourceFileL( resourceFile );

    iPlaybackUtility = MMPXPlaybackUtility::NewL( KPbModeActivePlayer, this );
    iProfileEngine = CreateProfileEngineL();
    iDisplayName = StringLoader::LoadL( R_MPX_SCREENSAVER_DISPLAY_NAME );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::HandlePropertyL(
    TMPXPlaybackProperty /* aProperty */,
    TInt /* aValue */,
    TInt /* aError */ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media properties.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ) );
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::DoHandlePlaybackMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXScreenSaverPlugin::DoHandlePlaybackMessageL - EStateChanged(%d)", type );

                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );
                DoHandleStateChangedL( state );
                break;
                }
            case TMPXPlaybackMessage::EMediaChanged:
                {
                MPX_DEBUG2( "CMPXScreenSaverPlugin::DoHandlePlaybackMessageL - EMediaChanged(%d)", type );
                MMPXSource* s( iPlaybackUtility->Source() );
                if ( s )
                    {
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL(attrs);
                    attrs.Append( KMPXMediaGeneralTitle );
                    s->MediaL( attrs.Array(), *this );
                    CleanupStack::PopAndDestroy( &attrs );
                    }
                break;
                }
            case TMPXPlaybackMessage::EActivePlayerChanged:
                {
                MPX_DEBUG3( "CMPXScreenSaverPlugin::DoHandlePlaybackMessageL - Active player changed(%d, %d)", type, data );
                if ( type && data )
                    {
                    // binded to active player, change state to active
                    DoHandleStateChangedL( EPbStatePlaying );
                    // refresh media property
                    MMPXSource* s = iPlaybackUtility->Source();
                    if ( s )
                        {
                        RArray<TMPXAttribute> attrs;
                        CleanupClosePushL(attrs);
                        attrs.Append( KMPXMediaGeneralTitle );
                        s->MediaL( attrs.Array(), *this );
                        CleanupStack::PopAndDestroy( &attrs );
                        }
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle media properties.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::DoHandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::DoHandleMediaL" );
    if ( iContainer && KErrNone == aError)
        {
        iContainer->SetSongPropertiesL( aMedia );
        // force a redraw after title update
        iScreenSaverHost->SetRefreshTimerValue( KTimerForceNextUpdateInterval );
        iScreenSaverHost->UseRefreshTimer( ETrue );
        iTimerChanged = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::DoHandleStateChangedL(
    TMPXPlaybackState aState )
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::DoHandleStateChangedL" );
    iPlaybackState = aState;

    // force a redraw after play state change
    iScreenSaverHost->SetRefreshTimerValue( KTimerForceNextUpdateInterval );
    iScreenSaverHost->UseRefreshTimer( ETrue );
    iTimerChanged = ETrue;
    }

// ---------------------------------------------------------------------------
// create container.
// ---------------------------------------------------------------------------
//
void CMPXScreenSaverPlugin::CreateContainerL()
    {
    MPX_FUNC( "CMPXScreenSaverPlugin::CreateContainerL" );

    iContainer = new ( ELeave )
        CMPXScreenSaverPluginContainer( iScreenSaverHost );
    iContainer->ConstructL();

    iPlaybackState = iPlaybackUtility->StateL();

    // get current playing song (if any)
    MMPXSource* s = iPlaybackUtility->Source();
    if ( s )
        {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        attrs.Append( KMPXMediaGeneralTitle );
        s->MediaL( attrs.Array(), *this );
        CleanupStack::PopAndDestroy( &attrs );
        }
    }

// End Of file.

