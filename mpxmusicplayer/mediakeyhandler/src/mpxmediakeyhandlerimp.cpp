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
* Description:  Implementation of media key handler
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <remconcoreapitarget.h>
#include <remconcoreapitargetobserver.h>
#include <remconinterfaceselector.h>
#include <aknconsts.h>
#include <AknUtils.h>
#include <eikenv.h>
#include <bautils.h>
#include <StringLoader.h>
#include <apgwgnam.h>
#include <utf.h>

#ifdef UPNP_INCLUDED
#include <upnpcopycommand.h>  
#endif


#include <aknnotewrappers.h>  // AknErrorNote
#include <AknVolumePopup.h>

#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <data_caging_path_literals.hrh>
#include <mpxmediakeyhandler.rsg>
#include <mpxmedia.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxuser.h>
#include <mpxmediakeyhandler.h>
#include <mpxlog.h>
#include <layoutmetadata.cdl.h>
#include <mpxconstants.h>

#include "mpxmetadatahandler.h"
#include <mpxplaybackframeworkdefs.h>
#include <sounddevice.h>
#include "mpxmediakeyhandlerimp.h"
#include "mpxremconkeyresponse.h"
#include "mpxnotifierdialog.h"


// CONSTANTS
const TInt KFirstTimerExpiryInterval( 1 ); // Expire immediately
const TInt KTimerExpiryInterval( KAknStandardKeyboardRepeatRate );
const TInt KMPXOneSecInMilliSecs( 1000 );
const TInt KMPXMinVolume(0);
const TInt KMPXMaxVolume(100);		// Max volume used in volume popup
const TInt KMPXVolumeSteps(1);
const TInt KTenStepsVolume = 10;   
const TInt KTwentyStepsVolume = 20;
const TInt KVolumePopupSynchInterval = 300000; // 300 ms
const TInt KFilterEventInterval( 6000 ); //6ms
const TRemConCoreApiOperationId KSupportedCoreFeatures[] = {
        ERemConCoreApiVolumeUp,
        ERemConCoreApiVolumeDown,
        ERemConCoreApiPlay,
        ERemConCoreApiStop,
        ERemConCoreApiPause,
        ERemConCoreApiRewind,
        ERemConCoreApiFastForward,
        ERemConCoreApiForward,
        ERemConCoreApiBackward,
        ERemConCoreApiPausePlayFunction
};

_LIT( KMPXMediaKeyHandlerRscPath, "mpxmediakeyhandler.rsc" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXMediaKeyHandlerImp::CMPXMediaKeyHandlerImp(
    MMPXMediaKeyHandlerObserver* aObserver ) :
    iObserver( aObserver ),
    iEnable( ETrue ),
    iCurrentVol( KErrNotFound ),
    iVolumeSteps(KMPXMaxVolume),
    iPreviousVol( KErrNotFound )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::ConstructL(
	TMPXMediaKeyPopupFlags aFlags )
    {
	ASSERT( iObserver );
    CCoeEnv* coeEnv = CEikonEnv::Static();
    TParse parse;
    parse.Set( KMPXMediaKeyHandlerRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    // Register to remote control framework
    iInterfaceSelector = CRemConInterfaceSelector::NewL();

    // For AVRCP 1.4 support, pass the core features to the target API. 
    TInt entrySize = sizeof( KSupportedCoreFeatures[0] );
    TRemConCoreApiOperationId* entries = (TRemConCoreApiOperationId*) &KSupportedCoreFeatures[0];
    TInt count = sizeof( KSupportedCoreFeatures ) / entrySize;
    RArray<TRemConCoreApiOperationId> coreFeatures( entrySize, entries, count );
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this, coreFeatures );

    // For handling AVRCP 1.3 metadata
    iMetaDataHandler = MMPXMetaDataHandler::NewL(*iInterfaceSelector);
    
    // For AVRCP 1.4 support, make the player named. 
    HBufC* avsrcname = StringLoader::LoadLC( R_MPX_AVSRC_NAME_TEXT );
    HBufC8* avsrcname8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( avsrcname->Des() );
    CleanupStack::PushL( avsrcname8 );
    iInterfaceSelector->OpenTargetL(ERemConAudioPlayer, ERemConNoSubType, *avsrcname8);
    CleanupStack::PopAndDestroy( avsrcname8 );
    CleanupStack::PopAndDestroy( avsrcname );
    iResponseHandler = CMPXRemConKeyResponse::NewL( *iCoreTarget );

    // Timer for implementing repeat
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );
    // get the current Volume and Max Volume
    iPlaybackUtility->PropertyL(*this, EPbPropertyVolume);
    iPlaybackUtility->PropertyL(*this, EPbPropertyMute);
    CMMFDevSound* devsound = CMMFDevSound::NewL();	
    MPX_DEBUG2( "CMPXMediaKeyHandlerImp::Constructdevsound->MaxVolume()%d",devsound->MaxVolume());
    iVolumeSteps = KTenStepsVolume; //Default 10 steps
    if ( devsound && devsound->MaxVolume() >= KTwentyStepsVolume ) 
        {
         // set 20-steps volume
         iVolumeSteps = KTwentyStepsVolume;
        }    
    delete devsound;
    if ( aFlags & EDisplayVolumePopup )
        {
        // Popup volume control
        iVolPopup = CAknVolumePopup::NewL(NULL, ETrue);
        iVolPopup->SetObserver(this);
        iVolPopup->SetRange(KMPXMinVolume, iVolumeSteps);
        iVolPopup->SetStepSize(KMPXVolumeSteps);
        // Ensure that initial value is set to popup. If not initialized, CAknVolumePopup::Value() returns 0 
        // but the control appears unmuted and allows volume to be decreased out of range (panics also with Avkon 6 in UDEB)
        iVolPopup->SetValue( 0 );
        
        HBufC* popupText = StringLoader::LoadLC( R_MPX_VOLUME_POPUP_TEXT );
        iVolPopup->SetTitleTextL( *popupText );
        CleanupStack::PopAndDestroy( popupText );
        }

    if ( aFlags & EDisplayMediaPopup )
        {
        // Playback popup
        iPlaybackPopup = CMPXNotifierDialog::NewL();
        }

    MMPXSource* s = iPlaybackUtility->Source();
    if ( s )
        {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        attrs.Append( KMPXMediaGeneralTitle );
        s->MediaL( attrs.Array(), *this );
        CleanupStack::PopAndDestroy( &attrs );
        }
    else
        {
        // No current track.
        // This is constructed so that the first popup will display
        // it doesn't hold anything.
        iTrackTitle = HBufC::NewL( 1 );
        }
#ifdef UPNP_INCLUDED
    if (!iUpnpCopyCommand )
        {
        MPX_TRAPD ( error, iUpnpCopyCommand = CUpnpCopyCommand::NewL() );   
		    if ( error == KErrNone )
            {
            iUpnpFrameworkSupport = ETrue;
            }
        else
            {
            iUpnpFrameworkSupport = EFalse;
			iUpnpCopyCommand = NULL;
            }
        }
#endif
    iVolumePopupSynchTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iFilterEventTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
MMPXMediaKeyHandler* CMPXMediaKeyHandlerImp::NewL(
    TMPXMediaKeyPopupFlags aFlags,
    MMPXMediaKeyHandlerObserver* aObserver )
    {
    CMPXMediaKeyHandlerImp* self =
        new(ELeave)CMPXMediaKeyHandlerImp( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aFlags );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMediaKeyHandlerImp::~CMPXMediaKeyHandlerImp()
    {
	if ( iFilterEventTimer )
		{
	    iFilterEventTimer->Cancel();
        delete iFilterEventTimer;
		}
    if ( iVolumePopupSynchTimer )
        {
        iVolumePopupSynchTimer->Cancel();
        delete iVolumePopupSynchTimer;
        }
    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    delete iMetaDataHandler;

    delete iResponseHandler;
    delete iInterfaceSelector;

    if( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }

    if ( iResourceOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResourceOffset );
        }

    delete iTrackTitle;
    delete iVolPopup;
    delete iPlaybackPopup;
#ifdef UPNP_INCLUDED
    if ( iUpnpCopyCommand)
        {
        delete iUpnpCopyCommand;
        }
#endif
    }

// ---------------------------------------------------------------------------
// Filter and send command to playback utility.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::FilterAndSendCommand(
    TMPXPlaybackCommand aCommandId )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::FilterAndSendCommand" );
    TRAP_IGNORE( DoFilterAndSendCommandL( aCommandId ) );
    }

// ---------------------------------------------------------------------------
// Help filter and send command to playback utility.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::DoFilterAndSendCommandL(
    TMPXPlaybackCommand aCommandId )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::DoFilterAndSendCommandL" );
    // Only send media key events if the media key component is enabled
    if( iEnable )
        {
        TBool forwardCommand = ETrue;
        if ( iUpnpFrameworkSupport )
            {
            switch ( aCommandId )
                {
                case EPbCmdStartSeekForward:
                case EPbCmdStartSeekBackward:
                    {
                    if ( IsUpnpVisibleL() )
                        {
                        TFileName subPlayerName;
                        TMPXPlaybackPlayerType currentPlayerType = EPbLocal;
                        GetSubPlayerInfoL(subPlayerName, currentPlayerType);

                        if ( currentPlayerType != EPbLocal)
                            {
                            //show error note
                            HBufC* dialogText = StringLoader::LoadLC(R_MPX_MEDIA_KEY_NOT_SUPPORTED);
                            CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                            errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                            errNote->ExecuteLD( *dialogText );
                            CleanupStack::PopAndDestroy( dialogText );
                            forwardCommand = EFalse;
                            }
                        }
                    break;
                    }
                default:
                    {
                    //do nothing
                    break;
                    }
                }
            }

        if ( forwardCommand )
            {
            TInt volume(0);
            TFileName subPlayerName;
            TMPXPlaybackPlayerType currentPlayerType = EPbLocal;
            GetSubPlayerInfoL(subPlayerName, currentPlayerType);
            
        	if ( currentPlayerType == EPbRemote )
                {
        	    volume = iUpnpVolume % ( KPbPlaybackVolumeLevelMax / iVolumeSteps )
            	                    + iCurrentVol * KPbPlaybackVolumeLevelMax / iVolumeSteps;
        	    }
        	else
        	    {
        	    volume = iCurrentVol * KPbPlaybackVolumeLevelMax/iVolumeSteps;
                iPreviousVol = iCurrentVol;
        	    }
            
            if ( volume < KMPXMinVolume )
                {
                volume = KMPXMinVolume;
                }
            if ( volume > KMPXMaxVolume )
                {
                volume = KMPXMaxVolume;
                }
            
            MPX_DEBUG3( "CMPXMediaKeyHandlerImp::DoFilterAndSendCommandL: aCommandId(%d), volume(%d)" , aCommandId, volume );
            iObserver->HandleMediaKeyCommand( aCommandId, volume );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::DoHandlePlaybackMessageL(CMPXMessage)");

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessagePbMediaChanged == id )
        {
        if ( aMessage.IsSupported( KMPXMessagePbMedia ) )
            {
            CMPXMedia* media( aMessage.Value<CMPXMedia>( KMPXMessagePbMedia ) );
            User::LeaveIfNull( media );
            iPlaybackPosition = 0;
            DoHandleMediaL( *media, KErrNone );
            }
        }
    else if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ));
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ));
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EPropertyChanged:
                {
                TMPXPlaybackProperty property(
                    static_cast<TMPXPlaybackProperty>( type ) );
                TInt error( KErrNone );

                DoHandlePropertyL( property, data, error );
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXMediaKeyHandlerImp::HandlePlaybackMessageL - EStateChanged(%d)", type );

                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );
                DoHandleStateChangedL( state );
                break;
                }
            case TMPXPlaybackMessage::EMediaChanged:
                {
                MMPXSource* s = iPlaybackUtility->Source();
                if ( s )
                    {
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL(attrs);
                    attrs.Append( KMPXMediaGeneralTitle );
                    MPX_DEBUG1( "CMPXMediaKeyHandlerImp::HandlePlaybackMessageL Media changed, calling MediaL to refresh" );
                    s->MediaL( attrs.Array(), *this );
                    CleanupStack::PopAndDestroy( &attrs );
                    }
                break;
                }
            case TMPXPlaybackMessage::ECommandReceived:
                {
                MPX_DEBUG2( "CMPXMediaKeyHandlerImp::HandlePlaybackMessageL - ECommandReceived(%d)", type );
                break;
                }
            case TMPXPlaybackMessage::ESkipping:
                {
                iSkipping = ETrue;
                break;
                }
            case TMPXPlaybackMessage::ESkipEnd:
                {
                iSkipping = EFalse;
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
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::DoHandlePropertyL" );
    MPX_DEBUG4( "CMPXMediaKeyHandlerImp::HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    iUpnpVolume = aValue; 
    if ( KErrNone == aError )
        {
        switch ( aProperty	)
            {
            case EPbPropertyPosition:
                {
                iPlaybackPosition = aValue / KMPXOneSecInMilliSecs;
                ShowPlaybackPopupL( EMPXPopupShowIfVisible );
                break;
                }
            case EPbPropertyMute:
            	{
            	iMuted = aValue;
            	StartVolumePopupSynchTimer();
				break;
				}
			case EPbPropertyVolume:
                {
                // Avkon Volume has 10 steps, but the rest of the framework
                // has 100 steps. Need to scale it to 10 steps.
                if ( aValue > 0 )
                    {
                    aValue = aValue * iVolumeSteps;
                    aValue = aValue / KPbPlaybackVolumeLevelMax;
                    if ( aValue > iVolumeSteps )
                        {
                        aValue = iVolumeSteps;
                        }
                    }
                
                //if aValue equal to iPreviousVol, MediaKey should not set Volume again.
                TBool OkToSetVolume = ( aValue != iPreviousVol );
                iPreviousVol = KErrNotFound;
                
                if( iMuted && aValue > 0  ) // unmute
                    {
                    iMuted = EFalse;
                    iCurrentVol = aValue;
                    }
                else if( aValue == 0 ) // mute
                    {
                    if( !iMuted ) 
                        {
                        iMuted = ETrue;
                        }
                    }
                else if ( aValue != iCurrentVol )
                    {
                    if ( aValue != 0 && OkToSetVolume && ( iResponseHandler->iCountOfVolumeCommands == 0 ) )
                        {
                        // if we are processing remcon events we shouldn't change the current volume value                      
                        iCurrentVol = aValue;
                        }
                    }
                
                if ( iCurrentVol == KErrNotFound ) // muted by some other application before launching Music Player
                    {
                    iCurrentVol = aValue;
                    }
                
                if ( OkToSetVolume )
                	{
                    StartVolumePopupSynchTimer();
                	}
                
                // send a command to UI to display Volume bar on device when controlling volume via UPnP  
                if ( IsUpnpVisibleL() && iPlayerState != EPbStateNotInitialised )
                    {
                    TFileName subPlayerName;
                    TMPXPlaybackPlayerType currentPlayerType = EPbLocal;
                    GetSubPlayerInfoL( subPlayerName, currentPlayerType );

                    if ( currentPlayerType != EPbLocal )
                        {
                        iObserver->HandleMediaKeyCommand( EPbCmdSetVolume, iUpnpVolume ); 
                        }            
                    }
                          
                if ( iUpnpFrameworkSupport )
                    {
                    SetVolumePopupTitleL();
                    }
					
				break;
                }
            default:
                {
                break;
                }
            }
        }
    else
        {
        switch ( aProperty	)
            {
            case EPbPropertyVolume:
            case EPbPropertyMute:
                {
                if ( iVolPopup && iShowPopups & EDisplayVolumePopup )
                    {
                    //show error note
                    HBufC* dialogText = StringLoader::LoadLC(R_MPX_VOLUME_KEYS_NOT_SUPPORTED);
                    CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                    errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                    errNote->ExecuteLD( *dialogText );
                    CleanupStack::PopAndDestroy( dialogText );
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
void CMPXMediaKeyHandlerImp::DoHandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::DoHandleMediaL" );

    // Delete old title no matter there's error or not
    if ( iTrackTitle )
        {
        delete iTrackTitle;
        iTrackTitle = NULL;
        }

    if ( KErrNone == aError )
        {
        // Keep track of the current track's title
        iTrackTitle = aMedia.ValueText( KMPXMediaGeneralTitle ).AllocL();

        // Update popup if visible
        if ( iShowPopups & EDisplayMediaPopup )
            {
            MPX_DEBUG1( "CMPXMediaKeyHandlerImp::DoHandleMediaL EMPXPopupTimeout" );
            ShowPlaybackPopupL( EMPXPopupTimeout );
            }
        else
            {
            ShowPlaybackPopupL( EMPXPopupShowIfVisible );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::DoHandleStateChangedL(
    TMPXPlaybackState aState )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::DoHandleStateChangedL" );
    switch ( aState )
        {
        case EPbStateStopped:
            {
            iPlayerState = aState;
            ShowPlaybackPopupL( EMPXPopupShowIfVisible );
            break;
            }
        case EPbStatePlaying:
        case EPbStatePaused:
            {
            iPlayerState = aState;
            // Start the time if needed, otherwise, update content if visible
            if ( iShowPopups & EDisplayMediaPopup )
                {
                MPX_DEBUG1( "CMPXMediaKeyHandlerImp::DoHandleStateChangedL EMPXPopupTimeout" );
                ShowPlaybackPopupL( EMPXPopupTimeout );
                }
            else
                {
                ShowPlaybackPopupL( EMPXPopupShowIfVisibleRestartTimer );
                }
            break;
            }
        default:
            {
            // do nothing
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Callback for timer
// ---------------------------------------------------------------------------
//
TInt CMPXMediaKeyHandlerImp::TimerCallback( TAny* aPtr )
    {
    static_cast<CMPXMediaKeyHandlerImp*>( aPtr )->HandleRepeatEvent();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handle repeat event
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::HandleRepeatEvent()
    {
	UpdateVolume();
    FilterAndSendCommand( iCommandId );
    }

// -----------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::GetSubPlayerInfoL
// Retrieves the current player name and sets the volume popup title
// -----------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::GetSubPlayerInfoL(
    TDes& aSubPlayerName,
    TMPXPlaybackPlayerType& aCurrentPlayerType )
    {
    aCurrentPlayerType = EPbLocal;
    aSubPlayerName = KNullDesC;

//#ifdef __UPNP_FRAMEWORK_2_0_
    if ( iUpnpFrameworkSupport )
        {
        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        TUid currentlyUsedPlayer;
        TInt currentlyUsedSubPlayer;
        HBufC* subPlayerName = NULL;
        TRAP_IGNORE( manager.GetSelectionL( aCurrentPlayerType,
                                            currentlyUsedPlayer,
                                            currentlyUsedSubPlayer,
                                            subPlayerName ) );
        if ( subPlayerName )
            {
            aSubPlayerName = (*subPlayerName);
            delete subPlayerName;
            }
        }
//#endif
    }

// -----------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::SetVolumePopupTitleL
// Retrieves the current player name and sets the volume popup title
// -----------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::SetVolumePopupTitleL()
    {
    if ( iUpnpFrameworkSupport && IsUpnpVisibleL() )
        {
        TFileName subPlayerName;
        TMPXPlaybackPlayerType currentPlayerType = EPbLocal;

        GetSubPlayerInfoL(subPlayerName, currentPlayerType);
        if ( currentPlayerType != EPbLocal)
            {
            iVolPopup->SetTitleTextL( subPlayerName );
            }
        else  // need to restore original title
            {
            HBufC* popupText = StringLoader::LoadLC( R_MPX_VOLUME_POPUP_TEXT );
            iVolPopup->SetTitleTextL( *popupText );
            CleanupStack::PopAndDestroy( popupText );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::IsUpnpVisibleL
// Taken from Gallery upnp support implementation
// -----------------------------------------------------------------------------
//
TBool CMPXMediaKeyHandlerImp::IsUpnpVisibleL()
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::IsUpnpVisible" );
    TBool returnValue = EFalse;
#ifdef UPNP_INCLUDED
   	if ( iUpnpCopyCommand && iUpnpFrameworkSupport ) 	  
   	    {
        returnValue = iUpnpCopyCommand->IsAvailableL();   
        }
#endif
    return returnValue; 
    }

// ---------------------------------------------------------------------------
// Updates the volume and the volume command
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::UpdateVolume()
	{
	// Current volume level is not yet initialised
    if( iCurrentVol < 0 )
        {
        MPX_DEBUG1( "CMPXMediaKeyHandlerImp::UpdateVolume not initialised");
        return;
        }

	if ( iIncreaseVol )
		{
		if ( iMuted )
			{
			iMuted = EFalse;
			iCommandId = EPbCmdUnMuteVolume;
            if ( iCurrentVol == 0 ) // prevent muting again when HandleControlEvent is called next time
                {
                iCurrentVol = 1; // "1" is the first step of 20-step volume
                }
			iVolPopup->SetValue( iCurrentVol );
			}
		else
			{
			iCommandId = EPbCmdSetVolume;
			iCurrentVol = iCurrentVol < iVolumeSteps ? (iCurrentVol + 1) : iCurrentVol;
			iVolPopup->SetValue( iCurrentVol );
			}
		}
	else
		{
		if ( iCurrentVol == 1 )
			{
			iMuted = ETrue;
			iCommandId = EPbCmdMuteVolume;
			iVolPopup->SetValue( 0 );
			}
		else if ( iMuted )	// Muted && volume > 1
			{
			iMuted = EFalse;
			iCommandId = EPbCmdUnMuteVolume;
			iVolPopup->SetValue( iCurrentVol );
			}
		else
			{
			iCommandId = EPbCmdSetVolume;
			iCurrentVol = iCurrentVol - 1;
			iVolPopup->SetValue( iCurrentVol );
			}
		}
	}

// ---------------------------------------------------------------------------
// From MMPXMediaKeyHandler
// Show playback popup.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::ShowPlaybackPopupL(
    TMPXPlaybackPopupModes aMode )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::ShowPlaybackPopupL" );

    // Check to see is it ok to display popup
    if ( !iPlaybackPopup || !iTrackTitle )
        {
        return;
        }

    switch ( aMode )
        {
        case EMPXPopupShowIfVisibleRestartTimer:
        case EMPXPopupTimeout:
            {
            iShowPopups &= ~EDisplayMediaPopup;
            break;
            }
        case EMPXPopupNoTimeout:
            {
            iShowPopups |= EDisplayMediaPopup;
            break;
            }
        case EMPXPopupShowIfVisible:
            {
            if ( iShowPopups & EDisplayMediaPopup )
                {
                aMode = EMPXPopupNoTimeout;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    CMPXNotifierDialog::TMPXPlaybackPopupIcons icon(
        CMPXNotifierDialog::EMPXPopupNoneIcon );
    TMPXPlaybackState playerState( EPbStateNotInitialised );

    // Get current playback state from playback utility
    playerState = iPlaybackUtility->StateL();
    switch ( playerState )
        {
        case EPbStatePlaying:
            {
            icon = CMPXNotifierDialog::EMPXPopupPlayIcon;
            break;
            }
        case EPbStatePaused:
            {
            icon = CMPXNotifierDialog::EMPXPopupPauseIcon;
            break;
            }
        case EPbStateSeekingForward:
            icon = CMPXNotifierDialog::EMPXPopupFFIcon;
            break;
        case EPbStateSeekingBackward:
            {
            icon = CMPXNotifierDialog::EMPXPopupFRIcon;
            break;
            }
        case EPbStateStopped:
            {
            // no icon if in stopped state
            break;
            }
        default:
            {
            // not to display popup in other states
            return;
            }
        }

    if ( iTrackTitle->Length() )
        {
        MPX_DEBUG2( "CMPXMediaKeyHandlerImp::ShowPlaybackPopupL aMode = %d", aMode );
        iPlaybackPopup->ShowInfoPopupL(
            icon, *iTrackTitle, iSkipping ? KErrNotFound : iPlaybackPosition, aMode );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXMediaKeyHandler
// Show Volume popup
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::ShowVolumePopupL()
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::ShowVolumePopupL" );
    if ( iVolPopup )
        {
        iShowPopups |= EDisplayVolumePopup;
        iVolPopup->ShowVolumePopupL();
        }
    }

// ---------------------------------------------------------------------------
// From MMPXMediaKeyHandler
// Dismiss notifier popup.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::DismissNotifier(
    TMPXMediaKeyPopupFlags aFlags )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::DismissNotifier" );
    if ( aFlags & EDisplayMediaPopup && iPlaybackPopup )
        {
        iPlaybackPopup->CancelInfoPopup();
        iShowPopups &= ~EDisplayMediaPopup;
        }

    if ( aFlags & EDisplayVolumePopup && iVolPopup )
        {
        iVolPopup->CloseVolumePopup();
        iShowPopups &= ~EDisplayVolumePopup;
        }
    }

// ---------------------------------------------------------------------------
// From MMPXMediaKeyHandler
// Enable or disable media keys
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::SetEnableMediaKeys( TBool aEnable )
    {
    iEnable = aEnable;
    }

// ---------------------------------------------------------------------------
// From MMPXMediaKeyHandler
// Gets called when orientation change begins
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::NotifyOrientationChangeBegin()
	{
	iPlaybackPopup->NotifyOrientationChangeBegin();
	}

// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::HandlePlaybackMessage(
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
void CMPXMediaKeyHandlerImp::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    TRAP_IGNORE( DoHandlePropertyL( aProperty, aValue, aError ) );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::HandleSubPlayerNamesL" );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media properties.
// Notes: The client is responsible for delete the object of aMedia.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXMediaKeyHandlerImp::HandleMediaL" );
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ) );
    }

// ---------------------------------------------------------------------------
// A 'play' command has been received.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::MrccatoPlay(
    TRemConCoreApiPlaybackSpeed /* aSpeed */,
    TRemConCoreApiButtonAction aButtonAct )
    {
    MPX_DEBUG2( "-->CMPXMediaKeyHandlerImp::MrccatoPlay(aButtonAct=%d)", aButtonAct );

    if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
        ( aButtonAct == ERemConCoreApiButtonPress ) )
        {
        FilterAndSendCommand( EPbCmdPlay );
        }
    iResponseHandler->CompleteAnyKey( ERemConCoreApiPlay );
    MPX_DEBUG1( "<--CMPXMediaKeyHandlerImp::MrccatoPlay");
    }

// ---------------------------------------------------------------------------
// A command has been received.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::MrccatoCommand(
    TRemConCoreApiOperationId aOperationId,
    TRemConCoreApiButtonAction aButtonAct )
    {
    MPX_DEBUG3( "CMPXMediaKeyHandlerImp::MrccatoCommand(aOperationId=%d, aButtonAct=%d)", aOperationId, aButtonAct );

    switch (aOperationId)
        {
        case ERemConCoreApiPausePlayFunction:
            {
            if ( aButtonAct == ERemConCoreApiButtonClick )
                {
                FilterAndSendCommand( EPbCmdPlayPause );
                }
            break;
            }
        case ERemConCoreApiPlay:
            {
            if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
                ( aButtonAct == ERemConCoreApiButtonPress ) )
                {
                FilterAndSendCommand( EPbCmdPlay );
                }
            break;
            }
        case ERemConCoreApiStop:
            {
            if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
                ( aButtonAct == ERemConCoreApiButtonPress ) )
                {
                FilterAndSendCommand( EPbCmdStop );
                }
            break;
            }
        case ERemConCoreApiPause:
            {
            if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
                ( aButtonAct == ERemConCoreApiButtonPress ) )
                {
                FilterAndSendCommand( EPbCmdPause );
                }
            break;
            }
        case ERemConCoreApiRewind:
            {
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    {
                    FilterAndSendCommand( EPbCmdStartSeekBackward );
                    break;
                    }
                case ERemConCoreApiButtonRelease:
                    {
                    FilterAndSendCommand( EPbCmdStopSeeking );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        case ERemConCoreApiFastForward:
            {
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    {
                    FilterAndSendCommand( EPbCmdStartSeekForward );
                    break;
                    }
                case ERemConCoreApiButtonRelease:
                    {
                    FilterAndSendCommand( EPbCmdStopSeeking );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        case ERemConCoreApiBackward:
            {
            if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
                 ( aButtonAct == ERemConCoreApiButtonRelease ) )
                {
                FilterAndSendCommand( EPbCmdPrevious );
                }
            break;
            }
        case ERemConCoreApiForward:
            {
            if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
                 ( aButtonAct == ERemConCoreApiButtonRelease ) )
                {
                FilterAndSendCommand( EPbCmdNext );
                }
            break;
            }
        case ERemConCoreApiVolumeUp:
        case ERemConCoreApiVolumeDown:
            {
            iResponseHandler->iCountOfVolumeCommands++;
            iTimer->Cancel();
            MMPXPlaybackUtility* pbUtil( NULL );
            MPX_TRAPD( err, pbUtil = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer ));
            if( err != KErrNone )
                {
                MPX_DEBUG2( "CMPXMediaKeyHandlerImp::MrccatoCommand ERemConCoreApiVolumeDown leave err%d", err );
                break;
                }
            TMPXPlaybackState playerState( EPbStateNotInitialised );
            TRAP_IGNORE( playerState = pbUtil->StateL());
            pbUtil->Close();
            pbUtil = NULL;

            if( playerState == EPbStatePlaying || IsAppForeground() )
                {
                iIncreaseVol = (aOperationId == ERemConCoreApiVolumeUp ? ETrue: EFalse);
                switch (aButtonAct)
                    {
                    case ERemConCoreApiButtonPress:
                        {
                        //Start Timer
                        iTimer->Start(
                            KFirstTimerExpiryInterval,
                            KTimerExpiryInterval,
                            TCallBack( TimerCallback, this ) );
                        break;
                        }
                    case ERemConCoreApiButtonClick:
                        {
                        UpdateVolume();
                        FilterAndSendCommand( iCommandId );
                        break;
                        }
                    case ERemConCoreApiButtonRelease:
                    default:
                        {
                        break;
                        }
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    iResponseHandler->CompleteAnyKey( aOperationId );
    MPX_DEBUG1( "<--CMPXMediaKeyHandlerImp::MrccatoCommand");
    }

// ---------------------------------------------------------------------------
// A 'tune function' command has been received.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::MrccatoTuneFunction(
    TBool /* aTwoPart */,
    TUint /* aMajorChannel */,
    TUint /* aMinorChannel */,
    TRemConCoreApiButtonAction /* aButtonAct */ )
    {
    iResponseHandler->CompleteAnyKey(
        ERemConCoreApiTuneFunction );
    }

// ---------------------------------------------------------------------------
// A 'select disk function' has been received.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::MrccatoSelectDiskFunction(
    TUint /* aDisk */,
    TRemConCoreApiButtonAction /* aButtonAct */ )
    {
    iResponseHandler->CompleteAnyKey(
        ERemConCoreApiSelectDiskFunction );
    }

// ---------------------------------------------------------------------------
// A 'select AV input function' has been received.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::MrccatoSelectAvInputFunction(
    TUint8 /* aAvInputSignalNumber */,
    TRemConCoreApiButtonAction /* aButtonAct */ )
    {
    iResponseHandler->CompleteAnyKey(
        ERemConCoreApiSelectAvInputFunction );
    }

// ---------------------------------------------------------------------------
// A 'select audio input function' has been received.
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::MrccatoSelectAudioInputFunction(
    TUint8 /* aAudioInputSignalNumber */,
    TRemConCoreApiButtonAction /* aButtonAct */ )
    {
    iResponseHandler->CompleteAnyKey(
        ERemConCoreApiSelectAudioInputFunction );
    }

// ----------------------------------------------------------------------------
// void CMPlayerBaseView::HandleControlEventL
// ----------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
	{
    MPX_FUNC("CMPXMediaKeyHandlerImp::HandleControlEventL");
	if ( !AknLayoutUtils::PenEnabled() )
		{
		return;
		}

	if( (aEventType == EEventStateChanged) && (aControl == iVolPopup) )
		{
		TInt vol = iVolPopup->Value();
		
		MPX_DEBUG4( "CMPXMediaKeyHandlerImp::HandleControlEventL: vol(%d), iCurrentVol(%d), iMuted(%d)", vol, iCurrentVol, iMuted );
		
		if ( vol == iCurrentVol && vol != 0 )
		    {
            if ( iMuted )
                {
                iMuted = EFalse;
                FilterAndSendCommand( EPbCmdUnMuteVolume );
                }
            else
                {
                return;
                }
		    }

		if ( vol == 0 )
			{
			if ( iMuted )
			    {
			    return;
			    }
			else
			    {
			    iMuted = ETrue;
			    FilterAndSendCommand( EPbCmdMuteVolume );
			    }
			}
		else
			{
			if ( iMuted )
				{
				iMuted = EFalse;
				
				if ( iCurrentVol == 0 ) // setting volume to 0 would reapply mute, use slider value instead
				    {
                    iCurrentVol = vol;
				    }
				
				iVolPopup->SetValue( iCurrentVol );
				FilterAndSendCommand( EPbCmdUnMuteVolume );
				}
			else
				{
				iCurrentVol = vol;
				//VolPopup CallBack functon is frequently called, 
				//But Music Palyer deals with the event slowly
				//Thus VolPopup will not display smoothly.
				//So Music Player needs timer to filter some events. 
				if ( !iFilterEventTimer->IsActive() )
					{
				    iFilterEventTimer->Start( KFilterEventInterval, 
				    		            KFilterEventInterval, 
				                        TCallBack( FilterEventTimerCallback, this ) );
					}
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// Check the mpx app is foreground.
// ---------------------------------------------------------------------------
//
TBool CMPXMediaKeyHandlerImp::IsAppForeground()
    {
    TBool isForeground( EFalse );
    // Gets the window group id of the app in foreground
    RWsSession& wsSession = CCoeEnv::Static()->WsSession();
    TInt windowGroupId = wsSession.GetFocusWindowGroup();
    if ( windowGroupId >= 0 )
        {
        TRAP_IGNORE(
            {
            CApaWindowGroupName* wgName = CApaWindowGroupName::NewL(
                wsSession, windowGroupId );

            isForeground = ( wgName->AppUid() == KAppUidMusicPlayerX );
            delete wgName;
            } );
        }

    return isForeground;
    }

// ---------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::VolumePopupSynchTimerCallback
// ---------------------------------------------------------------------------
//
TInt CMPXMediaKeyHandlerImp::VolumePopupSynchTimerCallback( TAny* aPtr )
    {
    static_cast<CMPXMediaKeyHandlerImp*>( aPtr )->DoVolumePopupSynch();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::DoVolumePopupSynch
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::DoVolumePopupSynch()
    {
    MPX_FUNC("CMPXMediaKeyHandlerImp::DoVolumePopupSynch");
    
    iVolumePopupSynchTimer->Cancel();
    
    TInt popupValue = iVolPopup->Value();
    
    MPX_DEBUG4("CMPXMediaKeyHandlerImp::DoVolumePopupSynch: popupValue(%d), iMuted(%d), iCurrentVol(%d)",
               popupValue, iMuted, iCurrentVol );
    
    if ( iMuted )
        {
        if ( popupValue != 0 )
            {
            MPX_DEBUG1("CMPXMediaKeyHandlerImp::DoVolumePopupSynch: popup out of synch (muted)" );
            iVolPopup->SetValue( 0 );
            }
        }
    else
        {
        if ( iCurrentVol != popupValue )
            {
            MPX_DEBUG1("CMPXMediaKeyHandlerImp::DoVolumePopupSynch: popup out of synch" );
            iVolPopup->SetValue( iCurrentVol );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::StartVolumePopupSynchTimer
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::StartVolumePopupSynchTimer()
    {
    iVolumePopupSynchTimer->Cancel();
    iVolumePopupSynchTimer->Start( KVolumePopupSynchInterval,
                                   KVolumePopupSynchInterval,
                                   TCallBack( VolumePopupSynchTimerCallback, this ) );

    }

// ---------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::FilterEventTimerCallback
// ---------------------------------------------------------------------------
//
TInt CMPXMediaKeyHandlerImp::FilterEventTimerCallback( TAny* aPtr )
	{
	static_cast<CMPXMediaKeyHandlerImp*>( aPtr )->DoFilterEventTimer();
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CMPXMediaKeyHandlerImp::FilterEventTimerCallback
// ---------------------------------------------------------------------------
//
void CMPXMediaKeyHandlerImp::DoFilterEventTimer()
	{
	MPX_FUNC("CMPXMediaKeyHandlerImp::DoFilterEventTimer");
	iFilterEventTimer->Cancel();
	FilterAndSendCommand( EPbCmdSetVolume );
	}
// End of File