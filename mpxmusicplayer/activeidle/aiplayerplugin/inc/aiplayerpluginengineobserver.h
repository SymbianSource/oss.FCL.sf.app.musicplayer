/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_AIPLAYERPLUGINENGINEOBSERVER_H
#define M_AIPLAYERPLUGINENGINEOBSERVER_H

#include <e32std.h>

/**
 *  Player state.
 */
enum TMPlayerState
    {
    EMPlayerStatePlaying,
    EMPlayerStatePaused,
    EMPlayerStateSeeking,
    EMPlayerStateOther
    };

/**
 *  Observer interface to be implemented by a client of AiPlayerPluginEngine.
 *
 *  @lib aiplayerplugin2.lib
 *  @since S60 5.0
 */
class MAiPlayerPluginEngineObserver
    {
public:

    /**
     * Called when active player instance's state changes
     *
     * @since S60 5.0
     * @param aState Active player state
     * @see TMPlayerState
     */
    virtual void PlayerStateChanged( TMPlayerState aState ) = 0;

    /**
     * Called when track info changes (i.e. track changes or current track's
     * info is updated) in the active player instance.
     *
     * @since S60 5.0
     * @param aTitle Current track's title.
     * @param aArtist Current track's artist.
     */
    virtual void TrackInfoChanged( const TDesC& aTitle, const TDesC& aArtist ) = 0;

    /**
     * Called each second during playback.
     *
     * @since S60 5.0
     * @param aPosition Current playback position in seconds.
     */
    virtual void PlaybackPositionChanged( TInt aPosition ) = 0;

    /**
     * Called when the player volume is changed.
     *
     * @since S60 5.0
     * @param aVolume volume level.
     */
    virtual void VolumeChanged( TInt aVolume ) = 0;
    };


#endif // ?M_AIPLAYERPLUGINENGINEOBSERVER_H
