/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Engine which interfaces to MPX framework.
*
*/


#ifndef C_AIPLAYERPLUGINENGINE_H
#define C_AIPLAYERPLUGINENGINE_H

#include <e32base.h>
#include <mpxplaybackobserver.h>

#include "aiplayerpluginengineobserver.h"

class MMPXPlaybackUtility;


/**
 *  Music Player Player Plugin Engine.
 *  This class provides interface to MPX framework.
 *
 *  @lib aiplayerplugin2.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CAiPlayerPluginEngine ) : public CBase,
                                             public MMPXPlaybackObserver,
                                             public MMPXPlaybackCallback
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CAiPlayerPluginEngine* NewL( MAiPlayerPluginEngineObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CAiPlayerPluginEngine();

    /**
     * From MMPXPlaybackObserver
     * See mpxplaybackobserver.h for detailed description.
     * @since S60 5.0
     */
    void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

    /**
     * From MMPXPlaybackCallback
     * See mpxplaybackobserver.h for detailed description.
     * @since S60 5.0
     */
    void HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );
    void HandleSubPlayerNamesL( TUid aPlayer, const MDesCArray* aSubPlayers, TBool aComplete, TInt aError );
    void HandleMediaL( const CMPXMedia& aMedia, TInt aError );
    void HandlePlaybackCommandComplete( CMPXCommand* aCommandResult, TInt aError );

public:

    /**
     * Get the current state of the active player.
     *
     * @return Active player state
     * @see TMPlayerState
     */
    TMPlayerState PlayerState();

    /**
     * Get the title of the currently playing song.
     *
     * @return Song title
     */
    const TDesC& Title();

    /**
     * Get the artist of the currently playing song.
     *
     * @return Artist
     */
    const TDesC& Artist();

    /**
     * Get the current position.
     *
     * @return Current position.
     */
    TInt Position();

    /**
     * Get the duration of the currently playing song.
     *
     * @return Song duration.
     */
    TInt Duration();

    /**
     * Get the current volume.
     *
     * @return Current volume.
     */
    TInt Volume();

    /**
     * Set volume.
     *
     * @param aVolume Volume level
     */
    void SetVolumeL( TInt aVolume );

private:

    /**
     * C++ default constructor.
     */
    CAiPlayerPluginEngine( MAiPlayerPluginEngineObserver& aObserver );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     *  Handle playback message
     *
     *  @param aMessage playback message
     */
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );

    /**
     *  Handle playback property
     *
     *  @param aProperty the property
     *  @param aValue the value of the property
     *  @param aError error code
     */
    void DoHandlePropertyL( TInt aProperty, TInt aValue, TInt aError );

    /**
     *  Handle playback state changed.
     *
     *  @param aState New Playback state
     */
    void DoHandleStateChangedL( TMPXPlaybackState aState );

    /**
     * Displays error notes.
     *
     * @param aError Error ID to be handled.
     */
    void HandleErrorL( TInt aError );

    /**
     *  Map states from TMPXPlaybackState to TMPlayerState
     *
     *  @param aState State in TMPXPlaybackState format
     *  @return State in TMPlayerState format, or KErrNotFound
     *          if the state can't be mapped
     */
    TMPlayerState MapState( TMPXPlaybackState aState );

    /**
     *  Map volume from MPX player to AI volume level.
     *
     *  @param aVolume Volume level from MPX player
     *  @return Volume level for AI.
     */
    TInt MapToAiVolume( TInt aVolume );

    /**
     *  Map volume from AI to MPX player volume level.
     *
     *  @param aVolume Volume level from AI
     *  @return Volume level for MPX player.
     */
    TInt MapToMpxVolume( TInt aVolume );
    
    /**
     *  Request media.
     */
    void RequestMediaL();

private:    // Data

    MAiPlayerPluginEngineObserver* 	iObserver; 			// Not owned
    MMPXPlaybackUtility* 			iPlaybackUtility; 	// Owned

    HBufC*		iTitle;
    HBufC*		iArtist;
    TInt		iMaxVolume;
    TInt		iVolume;
    TInt		iPosition;
    TInt		iDuration;
    };

#endif  // C_CAiPlayerPluginEngine_H

// End of File
