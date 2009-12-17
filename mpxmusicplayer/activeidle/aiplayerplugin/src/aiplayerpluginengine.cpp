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
* Description:  Music Player stautus observer
*
*/


#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxlog.h>

#include "aiplayerpluginengine.h"

const TInt KMPXOneSecInMilliSecs( 1000 );
const TInt KVolumeLevelMin = 0;
const TInt KVolumeLevelMax = 10;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAiPlayerPluginEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::ConstructL()
    {
    MPX_DEBUG1("CAiPlayerPluginEngine::ConstructL");
    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::NewL( KPbModeActivePlayer, this );
    
    TMPXPlaybackState state( EPbStateNotInitialised ); 
    
    // Not fatal error if fetching the playback state fails
    TRAP_IGNORE( state = iPlaybackUtility->StateL() );
    
    if( state != EPbStateNotInitialised &&
        state != EPbStateInitialising )
    	{
    	// Playback is already ongoing. We aren't going to receive EMediaChanged
    	// for the current song so we need manually update the media info   
    	RequestMediaL();
    	}
    }

// ---------------------------------------------------------------------------
// CAiPlayerPluginEngine::NewL
// ---------------------------------------------------------------------------
//
CAiPlayerPluginEngine* CAiPlayerPluginEngine::NewL( MAiPlayerPluginEngineObserver& aObserver )
    {
    MPX_DEBUG1("CAiPlayerPluginEngine::NewL");
    CAiPlayerPluginEngine* self = new ( ELeave ) CAiPlayerPluginEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAiPlayerPluginEngine::CAiPlayerPluginEngine
// ---------------------------------------------------------------------------
//
CAiPlayerPluginEngine::CAiPlayerPluginEngine( MAiPlayerPluginEngineObserver& aObserver )
    : iObserver( &aObserver ),
      iPlaybackUtility(NULL),
      iTitle(NULL),
      iArtist(NULL),
      iMaxVolume(KVolumeLevelMax),
      iVolume(KErrNotFound),
      iPosition(KErrNotFound),
      iDuration(KErrNotFound)
    {
    }

// ---------------------------------------------------------------------------
// CAiPlayerPluginEngine::~CAiPlayerPluginEngine
// ---------------------------------------------------------------------------
//
CAiPlayerPluginEngine::~CAiPlayerPluginEngine()
    {
    MPX_DEBUG1("CAiPlayerPluginEngine::~CAiPlayerPluginEngine");
    if ( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }
    delete iTitle;
    delete iArtist;
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
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
void CAiPlayerPluginEngine::HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError )
    {
    DoHandlePropertyL( aProperty, aValue, aError );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::HandleMediaL( const CMPXMedia& aMedia, TInt aError )
    {
    MPX_DEBUG1("CAiPlayerPluginEngine::HandleMediaL");
    if ( KErrNone == aError )
        {
		delete iTitle;
		iTitle = NULL;
        if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
            {
            iTitle = (aMedia.ValueText( KMPXMediaGeneralTitle )).AllocL();
            }
        else if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
            {
            TParsePtrC filePath( aMedia.ValueText( KMPXMediaGeneralUri ) );
            iTitle = (filePath.Name()).AllocL();
            }
		delete iArtist;
		iArtist = NULL;
		iArtist = (aMedia.ValueText( KMPXMediaMusicArtist )).AllocL();

        iObserver->TrackInfoChanged( *iTitle, *iArtist );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle completion of a asynchronous command
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::HandlePlaybackCommandComplete( CMPXCommand* /*aCommandResult*/, TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// Get the current state of the active player
// ---------------------------------------------------------------------------
//
TMPlayerState CAiPlayerPluginEngine::PlayerState()
    {
    MPX_DEBUG1("CAiPlayerPluginEngine::PlayerState");
    TMPXPlaybackState state( EPbStateNotInitialised );
    TRAP_IGNORE( state = iPlaybackUtility->StateL() );
    return MapState( state );
    }

// ---------------------------------------------------------------------------
// CAiPlayerPluginEngine::Title
// ---------------------------------------------------------------------------
//
const TDesC& CAiPlayerPluginEngine::Title()
    {
    if ( iTitle )
        {
        return *iTitle;
        }
     else
        {
        return KNullDesC;
        }
    }

// ---------------------------------------------------------------------------
// CAiPlayerPluginEngine::Artist
// ---------------------------------------------------------------------------
//
const TDesC& CAiPlayerPluginEngine::Artist()
    {
    if ( iArtist )
        {
        return *iArtist;
        }
     else
        {
        return KNullDesC;
        }
    }

// ----------------------------------------------------
// CAiPlayerPluginEngine::Position
// ----------------------------------------------------
//
TInt CAiPlayerPluginEngine::Position()
    {
    return iPosition;
    }

// ----------------------------------------------------
// CAiPlayerPluginEngine::Duration
// ----------------------------------------------------
//
TInt CAiPlayerPluginEngine::Duration()
    {
    return iDuration;
    }

// ----------------------------------------------------
// CAiPlayerPluginEngine::Volume
// ----------------------------------------------------
//
TInt CAiPlayerPluginEngine::Volume()
    {
    return iVolume;
    }

// ----------------------------------------------------
// CAiPlayerPluginEngine::SetVolumeL
// ----------------------------------------------------
//
void CAiPlayerPluginEngine::SetVolumeL( TInt aValue )
    {
	MPX_DEBUG2("CAiPlayerPluginEngine::SetVolumeL(%d)", aValue );
    if ( aValue == iVolume )
        {
        return;
        }
    if ( aValue < KVolumeLevelMin || aValue > KVolumeLevelMax )
        {
        User::Leave(KErrArgument);
        }
    else
        {
       	// iVolume will be updated upon callback DoHandlePropertyL
        iPlaybackUtility->SetL( EPbPropertyVolume, MapToMpxVolume(aValue) ) ;
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    MPX_DEBUG1("CAiPlayerPluginEngine::DoHandlePlaybackMessageL");

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        MPX_DEBUG2("CAiPlayerPluginEngine::DoHandlePlaybackMessageL(%d)", event );
        switch ( event )
            {
            case TMPXPlaybackMessage::EPropertyChanged:
                {
                TInt error( KErrNone );
                DoHandlePropertyL(
                    aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ),
                    aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ),
                    error );
                break;
                }
            case TMPXPlaybackMessage::EStateChanged:
                {
				TMPXPlaybackState state( aMessage.ValueTObjectL<TMPXPlaybackState>( KMPXMessageGeneralType ));
                MPX_DEBUG2("CAiPlayerPluginEngine::HandlePlaybackMessageL - EStateChanged(%d)", state);

                DoHandleStateChangedL( state );
                break;
                }
            case TMPXPlaybackMessage::EMediaChanged:
            case TMPXPlaybackMessage::EPlaylistUpdated:
                {
                iPlaybackUtility->PropertyL( *this, EPbPropertyPosition );
                iPlaybackUtility->PropertyL( *this, EPbPropertyDuration );
                RequestMediaL();
                break;
                }
            case TMPXPlaybackMessage::ECommandReceived:
                {
                MPX_DEBUG2("CAiPlayerPluginEngine::HandlePlaybackMessageL - ECommandReceived(%d)",
                    aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
                break;
                }
            case TMPXPlaybackMessage::EActivePlayerChanged:
                {
                MPX_DEBUG3("CAiPlayerPluginEngine::HandlePlaybackMessageL - EActivePlayerChanged(%d, %d)",
                    aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ), 
                    aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
                iPlaybackUtility->PropertyL( *this, EPbPropertyPosition );
                iPlaybackUtility->PropertyL( *this, EPbPropertyDuration );
                iPlaybackUtility->PropertyL( *this, EPbPropertyMaxVolume );
                iPlaybackUtility->PropertyL( *this, EPbPropertyVolume );
                DoHandleStateChangedL( iPlaybackUtility->StateL() );
                // refresh media property
                RequestMediaL();
                break;
                }
            case TMPXPlaybackMessage::EDownloadStateChanged:
                {
                iPlaybackUtility->PropertyL( *this, EPbPropertyPosition );
                iPlaybackUtility->PropertyL( *this, EPbPropertyDuration );
                RequestMediaL();
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
void CAiPlayerPluginEngine::DoHandlePropertyL( TInt aProperty, TInt aValue, TInt aError )
    {
    MPX_DEBUG4("CAiPlayerPluginEngine::DoHandlePropertyL - Property(%d); Value(%d); Error(%d)", aProperty, aValue, aError );
    if ( KErrNone == aError )
        {
        switch ( aProperty	)
            {
            case EPbPropertyPosition:
                {
				iPosition = aValue / KMPXOneSecInMilliSecs;
                iObserver->PlaybackPositionChanged( iPosition );
                break;
                }
            case EPbPropertyDuration:
                {
				iDuration = aValue / KMPXOneSecInMilliSecs;
                break;
                }
            case EPbPropertyMaxVolume:
                {
                iMaxVolume = aValue;
                break;
                }
            case EPbPropertyVolume:
                {
                TInt volume = MapToAiVolume(aValue);
				if ( iVolume != volume )
					{
					iVolume = volume;
					iObserver->VolumeChanged( iVolume );
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
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::DoHandleStateChangedL( TMPXPlaybackState aState )
    {
    TMPlayerState state = MapState( aState );
    MPX_DEBUG3("CAiPlayerPluginEngine::DoHandleStateChangedL - State mapped from (%d) to (%d)", aState, state );
    iObserver->PlayerStateChanged(state);
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::HandleErrorL( TInt aError )
    {
	MPX_DEBUG2("CAiPlayerPluginEngine::HandleErrorL(%d)", aError );
    }

// ---------------------------------------------------------------------------
// Map states from TMPXPlaybackState to TMPlayerState
// ---------------------------------------------------------------------------
//
TMPlayerState CAiPlayerPluginEngine::MapState( TMPXPlaybackState aState )
    {
	TMPlayerState state = EMPlayerStateOther;
    switch ( aState )
        {
        case EPbStatePlaying:
            state = EMPlayerStatePlaying;
            break;
        case EPbStatePaused:
            state = EMPlayerStatePaused;
            break;
        case EPbStateSeekingForward:
        case EPbStateSeekingBackward:
            state = EMPlayerStateSeeking;
            break;
        default:
            break;
        }
    return state;
    }

// ---------------------------------------------------------------------------
// Maps volume from MPX player to AI volume.
// ---------------------------------------------------------------------------
//
TInt CAiPlayerPluginEngine::MapToAiVolume( TInt aVolume )
    {
	TInt volume;
	if ( iMaxVolume == KVolumeLevelMax )
		{
		// No need to translate
		volume = aVolume;
		}
	else if ( aVolume == KVolumeLevelMin )
		{
		volume = KVolumeLevelMin;
		}
	else if ( aVolume == iMaxVolume )
		{
		volume = KVolumeLevelMax;
		}
	else
		{
		volume = (TInt) ((aVolume * KVolumeLevelMax) / iMaxVolume);
		}
	return volume;
    }

// ---------------------------------------------------------------------------
// Maps volume from AI to MPX player volume.
// ---------------------------------------------------------------------------
//
TInt CAiPlayerPluginEngine::MapToMpxVolume( TInt aVolume )
    {
	TInt volume;
	if ( iMaxVolume == KVolumeLevelMax )
		{
		// No need to translate
		volume = aVolume;
		}
	else if ( aVolume == KVolumeLevelMin )
		{
		volume = KVolumeLevelMin;
		}
	else if ( aVolume == KVolumeLevelMax )
		{
		volume = iMaxVolume;
		}
	else
		{
		volume = (TInt) ((aVolume * iMaxVolume) / KVolumeLevelMax);
		}
	return volume;
    }

// ---------------------------------------------------------------------------
// Requests Media.
// ---------------------------------------------------------------------------
//
void CAiPlayerPluginEngine::RequestMediaL()
    {
    MMPXSource* s = iPlaybackUtility->Source();
    if ( s )
        {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        attrs.Append( KMPXMediaGeneralUri );
        attrs.Append( KMPXMediaGeneralTitle );
        attrs.Append( KMPXMediaMusicArtist );
        s->MediaL( attrs.Array(), *this );
        CleanupStack::PopAndDestroy( &attrs );
        }
    }
//  End of File
