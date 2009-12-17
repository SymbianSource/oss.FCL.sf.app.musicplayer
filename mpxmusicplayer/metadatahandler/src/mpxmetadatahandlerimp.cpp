/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of metadata handler
*
*/


#include <bldvariant.hrh>
#include <remconinterfaceselector.h>

#include <bautils.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxplaybackmessage.h>
#include <data_caging_path_literals.hrh>
#include <mpxmedia.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxuser.h>
#include <mpxlog.h>
#include <playerinformationtarget.h>
#include <remcongroupnavigationtarget.h>
#include <utf.h>
#include <remcon/avrcpspec.h>
#include <mpxmediamusicdefs.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <EqualizerConstants.h>
#include <centralrepository.h>
#include "mpxmetadatahandler.h"
#include "mpxmetadatahandlerimp.h"

// CONSTANTS
const TInt KMPXOneSecInMilliSecs( 1000 );

// Definition of remotely controllable application settings
// The commented settings are not used here, included for reference.
_LIT( KMPlayerSettingsDefs, "mplayeravrcpsettings.rsc" );
const TInt KAvrcpEqualizerMode = 0x01;
const TInt KAvrcpRepeatMode = 0x02;
const TInt KAvrcpShuffleMode = 0x03;
//const TInt KAvrcpScanMode = 0x04;

const TInt KAvrcpOff = 0x01;
const TInt KAvrcpOn = 0x02;

// Off                      0x01
//const TInt KAvrcpSingleTrackRepeat = 0x02;
//const TInt KAvrcpAllTracksRepeat = 0x03;
//const TInt KAvrcpGroupRepeat = 0x04;

// Off                      0x01
const TInt KAvrcpAllTracksShuffle = 0x02;
//const TInt KAvrcpGroupShuffle = 0x03;

// Extended settings defined for Music Player
// Note that these are also defined in mplayeravrcpsettings.rss
const TInt KAvrcpBassBoostMode = 0x80;
const TInt KAvrcpStereoWideningMode = 0x81;

const TUid KCRUidMusicPlayerSettings = {0x101FFCDC};
const TUint32 KMPlayerEqPresetId = 0x00000001;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CMPXMetaDataHandlerImp::CMPXMetaDataHandlerImp()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::ConstructL(CRemConInterfaceSelector &aInterfaceSelector)
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::ConstructL(CRemConInterfaceSelector)");

    iPlayerInformationTarget = CPlayerInfoTarget::NewL(aInterfaceSelector, 
                                                iPlayerCapabilitiesObserver, 
                                                iPlayerApplicationSettingsObserver, 
                                                iPlayerEventsObserver, *this);
    iMediaInfoTarget = CRemConMediaInformationTarget::NewL( aInterfaceSelector, *this );

    iGroupNavigationTarget = CRemConGroupNavigationApiTarget::NewL( aInterfaceSelector, *this );

    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );

    MMPXSource* s = iPlaybackUtility->Source();
    if ( s )
        {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        (void) attrs.Append( KMPXMediaGeneralTitle );
        (void) attrs.Append( KMPXMediaGeneralDuration );
        (void) attrs.Append( KMPXMediaGeneralId );
        (void) attrs.Append( KMPXMediaMusicArtist );
        (void) attrs.Append( KMPXMediaMusicAlbum );
        (void) attrs.Append( KMPXMediaMusicGenre );
        s->MediaL( attrs.Array(), *this, NULL );
        CleanupStack::PopAndDestroy( &attrs );
        }

    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TParse parse;   
    parse.Set( KMPlayerSettingsDefs, &KDC_RESOURCE_FILES_DIR, NULL );
    
    TFileName resourceFile( parse.FullName() );

    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( fs, resourceFile );
    CResourceFile* playerSettingsResourceFile = CResourceFile::NewLC( fs, resourceFile, 0, 0 );
    PlayerApplicationSettingsResourceInit::DefineAttributesL( *iPlayerApplicationSettingsObserver, *playerSettingsResourceFile );
    CleanupStack::PopAndDestroy( playerSettingsResourceFile );
    CleanupStack::PopAndDestroy( &fs );

        // Adding two optional events, mandatory ERegisterNotificationPlaybackStatusChanged 
        // and ERegisterNotificationTrackChanged are added by SOS.
    User::LeaveIfError( iPlayerCapabilitiesObserver->AddEvent(ERegisterNotificationPlaybackPosChanged));
    User::LeaveIfError( iPlayerCapabilitiesObserver->AddEvent(ERegisterNotificationPlayerApplicationSettingChanged));

    iEqPresetListener = CEqualizerPresetChangeListener::NewL(*this);
    iLastEqPresetId = iEqPresetListener->GetCurrentPresetL();
    iEqPresetListener->StartL();
    iTrackNumber = 0;
    iColId.iUid = -1;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
MMPXMetaDataHandler* CMPXMetaDataHandlerImp::NewL(CRemConInterfaceSelector &aInterfaceSelector)
    {
    CMPXMetaDataHandlerImp* self = new(ELeave)CMPXMetaDataHandlerImp();
    CleanupStack::PushL( self );
    self->ConstructL(aInterfaceSelector);
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMetaDataHandlerImp::~CMPXMetaDataHandlerImp()
    {
    if( iEqPresetListener )
        {
        iEqPresetListener->Stop();
        delete iEqPresetListener;
        }
    if( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    delete iTrackTitle;
    delete iArtist;
    delete iAlbum;
    delete iGenre;
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::DoHandlePlaybackMessageL(CMPXMessage)");

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
                TMPXPlaybackProperty property( static_cast<TMPXPlaybackProperty>( type ) );
                TInt error( KErrNone );
                DoHandlePropertyL( property, data, error );
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2( "CMPXMetaDataHandlerImp::HandlePlaybackMessageL - EStateChanged(%d)", type );

                TMPXPlaybackState state = static_cast<TMPXPlaybackState>( type );
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
                    (void) attrs.Append( KMPXMediaGeneralTitle );
                    (void) attrs.Append( KMPXMediaGeneralDuration );
                    (void) attrs.Append( KMPXMediaGeneralId );
                    (void) attrs.Append( KMPXMediaMusicArtist );
                    (void) attrs.Append( KMPXMediaMusicAlbum );
                    (void) attrs.Append( KMPXMediaMusicGenre );
                    MPX_DEBUG1( "CMPXMetaDataHandlerImp::HandlePlaybackMessageL Media changed, calling MediaL to refresh" );
                    s->MediaL( attrs.Array(), *this, NULL );
                    CleanupStack::PopAndDestroy( &attrs );
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
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::DoHandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::DoHandlePropertyL" );
    MPX_DEBUG4( "CMPXMetaDataHandlerImp::HandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );

    TInt attr = 0;
    TInt val = KAvrcpOff;
    if ( KErrNone == aError )
        {
        switch ( aProperty )
            {
            case EPbPropertyPosition:
                {
                iPlaybackPosition = aValue / KMPXOneSecInMilliSecs;
                    // AVRCP 1.3 - inform remote device of the new position 
                    // ( Todo: check that we don't call too often)
                iPlayerEventsObserver->SetPlaybackPosition(aValue); // aPlaybackPosInMicroSeconds is converted to 32 bit value
                break;
                }
            case EPbPropertyRepeatMode:
                {
                attr = KAvrcpRepeatMode; // Repeat mode status
                    // TMPlayerRepeatMode values are in the same order as AVRCP 1.3, 
                    // but starting at 0 instead of 1.
                val = (TInt) aValue + 1;
                break;
                }
            case EPbPropertyRandomMode:
                {
                attr = KAvrcpShuffleMode;    // Shuffle on/off status
                TBool random = aValue;
                if( random )
                    {
                    val = KAvrcpAllTracksShuffle;
                    }
                break;
              }
            default:
                {
                break;
                }
            }
        if( attr )
            {
            iPlayerApplicationSettingsObserver->SetAttributeL( attr, val );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle media properties.
// Notes: The client is responsible for delete the object of aProperties.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::DoHandleMediaL(
    const CMPXMedia& aMedia, TInt aError )
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::DoHandleMediaL" );

    // Delete old title no matter there's error or not
    delete iTrackTitle;
    iTrackTitle = NULL;
    delete iArtist;
    iArtist = NULL;
    delete iAlbum;
    iAlbum = NULL;
    delete iGenre;
    iGenre = NULL;
    TInt lastTrackNumber = -1;
    if ( KErrNone == aError )
        {
        // Keep track of the current track's title
        iTrackTitle = aMedia.ValueText( KMPXMediaGeneralTitle ).AllocL();
        TInt *playingTime = aMedia.Value<TInt>( KMPXMediaGeneralDuration );
        if(playingTime)
            {
            iPlayingTime = *playingTime; // In milliseconds.
            }
        MMPXSource* s = iPlaybackUtility->Source();
        if ( s )
            {
            CMPXCollectionPlaylist* pl = s->PlaylistL();
            if( pl )
                {
                lastTrackNumber = pl->Path().Index() + 1; // What if PlaylistItemCount is zero?
                delete pl;
                }
            }

        iArtist = aMedia.ValueText( KMPXMediaMusicArtist ).AllocL();
        iAlbum = aMedia.ValueText( KMPXMediaMusicAlbum ).AllocL();
        iGenre = aMedia.ValueText( KMPXMediaMusicGenre ).AllocL();

        TUid colId = aMedia.ValueTObjectL<TUid>(KMPXMediaGeneralId);

        
        MPX_DEBUG2( "CMPXMetaDataHandlerImp::DoHandleMediaL - Track title(%S)", iTrackTitle);
        MPX_DEBUG4( "CMPXMetaDataHandlerImp::DoHandleMediaL - Artist(%S); Album(%S); Genre(%S)", iArtist, iAlbum, iGenre );
        MPX_DEBUG4( "CMPXMetaDataHandlerImp::DoHandleMediaL - Playing time(%d); Track number(%d); UID(%d)", iPlayingTime, lastTrackNumber, colId.iUid);
                
        if ( colId.iUid != iColId.iUid || lastTrackNumber != iTrackNumber )
            {
            iColId = colId;
            iTrackNumber = lastTrackNumber;
            iPlayerEventsObserver->TrackChanged(iColId.iUid, iPlayingTime);
            }
        else
            {
            MPX_DEBUG1( "CMPXMetaDataHandlerImp::DoHandleMediaL DUPLICATE Trace Change Received" );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::DoHandleStateChangedL(
    TMPXPlaybackState aState )
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::DoHandleStateChangedL" );
    switch ( aState )
        {
        case EPbStateStopped:
            {
            iPlayerState = aState;
            iPlayerEventsObserver->PlaybackStatusChanged(MPlayerEventsObserver::EStopped);
            break;
            }
        case EPbStatePlaying:
            iPlayerState = aState;
            // Start the time if needed, otherwise, update content if visible
            iPlayerEventsObserver->PlaybackStatusChanged(MPlayerEventsObserver::EPlaying);
            break;
        case EPbStatePaused:
            {
            iPlayerState = aState;
            // Start the time if needed, otherwise, update content if visible
            iPlayerEventsObserver->PlaybackStatusChanged(MPlayerEventsObserver::EPaused);
            break;
            }
        case EPbStateSeekingForward:
            {
            iPlayerEventsObserver->PlaybackStatusChanged(MPlayerEventsObserver::EFwdSeek);
            break;
            }
        case EPbStateSeekingBackward:
            {
            iPlayerEventsObserver->PlaybackStatusChanged(MPlayerEventsObserver::ERevSeek);
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
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::HandlePlaybackMessage(
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
void CMPXMetaDataHandlerImp::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    TRAP_IGNORE( DoHandlePropertyL( aProperty, aValue, aError ) );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle sub player names.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::HandleSubPlayerNamesL(
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
void CMPXMetaDataHandlerImp::HandleMediaL(
    const CMPXMedia& aMedia, TInt aError )
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::HandleMediaL" );
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ) );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionPlaylistObserver
// Handle collection playlist change.
// Notes: aError values:
// KErrNotFound - Playlist is updated, current item removed
// KErrNone - Playlist is updated, current item is valid
// KErrEof - Playlist is updated, current item removed and reached to the end of playlist
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::HandleCollectionPlaylistChange(TInt aError)
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::HandleCollectionPlaylistChange" );

    if( aError )
        {
            // Update the media item.
        MMPXSource* s = iPlaybackUtility->Source();
        if ( s )
            {
            RArray<TMPXAttribute> attrs;
                // Failing the next operations here is not fatal.
            (void) attrs.Append( KMPXMediaGeneralTitle );
            (void) attrs.Append( KMPXMediaGeneralDuration );
            (void) attrs.Append( KMPXMediaGeneralId );
            (void) attrs.Append( KMPXMediaMusicArtist );
            (void) attrs.Append( KMPXMediaMusicAlbum );
            (void) attrs.Append( KMPXMediaMusicGenre );
                // Ignore the leave. If this occurs, we just don't get 
                // an updated media information for time being.
            TRAP_IGNORE(s->MediaL( attrs.Array(), *this, NULL ));
            attrs.Close();
            }
        }
    }

// ---------------------------------------------------------------------------
// From MPlayerApplicationSettingsNotify
// This is called when the controller has changed a setting
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::MpasnSetPlayerApplicationValueL(
    const RArray<TInt>& aAttributeID, const RArray<TInt>& aAttributeValue )
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::MpasnSetPlayerApplicationValueL" );

    for( TInt i = 0; i < aAttributeID.Count(); i++ )
        {
        switch( aAttributeID[ i ] )
            {
            case KAvrcpEqualizerMode:
                {
                TInt equalizer = aAttributeValue[ i ] == KAvrcpOff ? KEqualizerPresetNone : iLastEqPresetId;
                iEqPresetListener->ChangePresetL(equalizer);
//                iLastEqPresetId = equalizer;
                break;
                }
            case KAvrcpRepeatMode:
                {
                    // TMPlayerRepeatMode values are in the same order 
                    // as AVRCP 1.3, but starting at 0 instead of 1.
                TInt repeat = aAttributeValue[ i ] - 1;
                iPlaybackUtility->SetL(EPbPropertyRepeatMode, repeat);
                break;
                }
            case KAvrcpShuffleMode:
                {
                TBool random = aAttributeValue[ i ] == KAvrcpOff ? EFalse : ETrue;
                iPlaybackUtility->SetL(EPbPropertyRandomMode, random);
                break;
                }
            case KAvrcpBassBoostMode:
                {
                TBool boost = aAttributeValue[ i ] == KAvrcpOff ? EFalse : ETrue;
//                iSettingModel->SetBassBoostL( boost );
                break;
                }
            case KAvrcpStereoWideningMode:
                {
                TBool widening = aAttributeValue[ i ] == KAvrcpOff ? EFalse : ETrue;
//                iSettingModel->SetStereoWideningL( widening );
                break;
                }
            default:
                {
                // Leaving results in sending an error back to the controller.
                User::Leave( KErrNotSupported );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// From MRemConMediaInformationTargetObserver
// For each element in aAttributeList the client should respond by calling 
// CRemConMediaInformationTarget::AttributeValue(). After all attributes have 
// been supplied the client should call CRemConMediaInformationTarget::Completed().
// @param aAttributeList A list of TAttributeID requested by the controller
// -----------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::MrcmitoGetCurrentlyPlayingMetadata( 
    TMediaAttributeIter& aAttributeIter )
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::MrcmitoGetCurrentlyPlayingMetadata" );
    const TInt KMaxMediaAttrLen = 50;   // Replace with suitable value
    TBuf8<KMaxMediaAttrLen> attrBuf;
    TMediaAttributeId attrId;
    aAttributeIter.Start();
    while( aAttributeIter.Next( attrId ) )
        {
        TInt err = KErrNotFound;
        HBufC* detail = NULL;
        TInt64 val = 0;
        switch( attrId )
            {
            case ETitleOfMedia:
                {
                detail = iTrackTitle;
                break;
                }
            case ENameOfArtist:
                {
                detail = iArtist;
                break;
                }
            case ENameOfAlbum:
                {
                detail = iAlbum;
                break;
                }
            case ETrackNumber:
                {
                val = iTrackNumber;
                break;
                }
            case ENumberOfTracks:
                {
                MMPXSource* s = iPlaybackUtility->Source();
                if ( s )
                    {
                    CMPXCollectionPlaylist* pl = NULL;
                    TRAP(err, pl = s->PlaylistL());
                    if( !err && pl )
                        {
                        val = pl->Count(); // What if PlaylistItemCount is zero?
                        }
                    delete pl;
                    }
                break;
                }
            case EGenre:
                {
                detail = iGenre;
                break;
                }
            case EPlayingTime:
                {
                // No need to check for boundaries, assuming the file length < 10^15 seconds
                val = iPlayingTime; // If playing time does not exist, what then?
                break;
                }
            default:
                break;
            }
        if( val )
            {
            // 'val' and 'detail' are mutually exclusive. We still set detail to 
            // null, to avoid the possiblility that "delete detail" will delete 
            // a member variable if NewL fails.
            detail = NULL;
            TRAP_IGNORE( detail = HBufC::NewL( KMaxMediaAttrLen ) );
            if( detail )
                {
                detail->Des().NumUC( val );
                }
            }
        if( detail )
            {
            if( CnvUtfConverter::ConvertFromUnicodeToUtf8( attrBuf, *detail ) )
                {
                attrBuf.Zero();     // Check if this generates the right response
                }
            }
        if( val )
            {
            delete detail;
            detail = NULL;
            }
        iMediaInfoTarget->AttributeValue( attrId, attrBuf );
        }
    iMediaInfoTarget->Completed();
    }



// ---------------------------------------------------------------------------
// From class MRemConGroupNavigationTargetObserver.
// A 'Next Group' command has been received.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::MrcgntoNextGroup(TRemConCoreApiButtonAction aButtonAct)
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::MrcgntoNextGroup" );
    (void) aButtonAct;
    // Not supported yet, return an error.
    TRequestStatus* status = NULL;
    iGroupNavigationTarget->NextGroupResponse( status, KErrAvrcpMetadataInvalidParameter );
    }

// ---------------------------------------------------------------------------
// From class MRemConGroupNavigationTargetObserver.
// A 'Previous Group' command has been received.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::MrcgntoPreviousGroup(TRemConCoreApiButtonAction aButtonAct)
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::MrcgntoPreviousGroup" );
    (void) aButtonAct;
    // Not supported yet, return an error.
    TRequestStatus* status = NULL;
    iGroupNavigationTarget->PreviousGroupResponse( status, KErrAvrcpMetadataInvalidParameter );
    }


// ---------------------------------------------------------------------------
// From class MEqualizerPresetChangeListenerObserver
// Callback for receiving changes in the equalizer preset settings.
// ---------------------------------------------------------------------------
//
void CMPXMetaDataHandlerImp::EqualizerPresetChangedL(TInt aNewPreset)
    {
    MPX_FUNC( "CMPXMetaDataHandlerImp::EqualizerPresetChanged" );
    TInt val = KAvrcpOff;
    if( aNewPreset != KEqualizerPresetNone)
      {
      iLastEqPresetId = aNewPreset; // Store last used preset
      val = KAvrcpOn;
      }
    iPlayerApplicationSettingsObserver->SetAttributeL( KAvrcpEqualizerMode, val );
    }


// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CEqualizerPresetChangeListener::CEqualizerPresetChangeListener(MEqualizerPresetChangeListenerObserver& aObserver)
:   CActive(EPriorityStandard), 
    iObserver(aObserver)
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::CEqualizerPresetChangeListener" );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CEqualizerPresetChangeListener::ConstructL()
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::ConstructL" );
    CActiveScheduler::Add( this );
    iRepository = CRepository::NewL(KCRUidMusicPlayerSettings);
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CEqualizerPresetChangeListener* CEqualizerPresetChangeListener::NewL(MEqualizerPresetChangeListenerObserver& aObserver)
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::NewL" );
    CEqualizerPresetChangeListener* self = new(ELeave)CEqualizerPresetChangeListener( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CEqualizerPresetChangeListener::~CEqualizerPresetChangeListener()
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::~CEqualizerPresetChangeListener" );
    Stop();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
// Get the current equalizer preset ID
// ---------------------------------------------------------------------------
//
TInt CEqualizerPresetChangeListener::GetCurrentPresetL()
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::GetCurrentPresetL" );
    TInt preset;
    User::LeaveIfError(iRepository->Get(KMPlayerEqPresetId, preset));
    return preset;
    }

// ---------------------------------------------------------------------------
// Set the equalizer preset
// ---------------------------------------------------------------------------
//
void CEqualizerPresetChangeListener::ChangePresetL(TInt aNewPreset)
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::ChangePresetL" );
    User::LeaveIfError(iRepository->Set(KMPlayerEqPresetId, aNewPreset));
    }

// ---------------------------------------------------------------------------
// Start listening to equalizer preset changes
// ---------------------------------------------------------------------------
//
void CEqualizerPresetChangeListener::StartL()
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::StartL" );
    User::LeaveIfError(iRepository->NotifyRequest(KMPlayerEqPresetId, iStatus));
    SetActive();
    }

// ---------------------------------------------------------------------------
// Stop listening to equalizer preset changes
// ---------------------------------------------------------------------------
//
void CEqualizerPresetChangeListener::Stop()
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::Stop" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Called by the active scheduler when the request has been completed.
// ---------------------------------------------------------------------------
//
void CEqualizerPresetChangeListener::RunL()
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::RunL" );
    iObserver.EqualizerPresetChangedL(GetCurrentPresetL());
    StartL();
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Called by the active scheduler when the request has been cancelled.
// ---------------------------------------------------------------------------
//
void CEqualizerPresetChangeListener::DoCancel()
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::DoCancel" );
    iRepository->NotifyCancel(KMPlayerEqPresetId);
    delete iRepository;
    iRepository = NULL;
    }

// ---------------------------------------------------------------------------
// From class CActive.
// Called by the active scheduler when an error in RunL has occurred.
// ---------------------------------------------------------------------------
//
TInt CEqualizerPresetChangeListener::RunError( TInt aError )
    {
    MPX_FUNC( "CEqualizerPresetChangeListener::RunError" );
    (void) aError;
    // In case of an exception in RunL, re-subscribe to Central Repository, 
    // and ignore the return value. If it fails we just won't be sending 
    // equalizer updates anymore.
    if( !iRepository->NotifyRequest( KMPlayerEqPresetId, iStatus ) )
        {
        SetActive();
        }
    return KErrNone;
    }
