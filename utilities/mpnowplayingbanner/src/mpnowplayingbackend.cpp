/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Music Player now playing widget backend.
*
*/

#include "mpnowplayingbackend.h"
#include "mpnowplayingbackend_p.h"
#include "mptrace.h"

/*!
    \internal
    \class MpNowPlayingBackEnd
    \brief Wraps the playback utility and provides separation for QT and Symbian code.

    This class wraps the playback utility on the MPX Framework to provide a simple 
    interface and separation of QT and Symbian code.
*/

/*!
    \internal
    \fn MpNowPlayingBackEnd::stateUpdate( SimplifiedPlayerState state )

    MpNowPlayingBackEndPrivate emits this signal when the simplified playback 
    state changes.
*/

/*!
    \internal
    \fn MpNowPlayingBackEnd::titleChanged( const QString &text )

    MpNowPlayingBackEndPrivate emits this signal when there is a new song
    playing and the title becomes available.
*/

/*!
    \internal
    \fn MpNowPlayingBackEnd::artistChanged( const QString &text )

    MpNowPlayingBackEndPrivate emits this signal when there is a new song
    playing and the artist name becomes available.
*/

/*!
    \internal
    \enum SimplifiedPlayerState

    This enum defines a set of simplified playback states.
 */

/*!
 \internal
 Constructs the now playing back end.
 */
MpNowPlayingBackEnd::MpNowPlayingBackEnd( long int playerId )
    :d_ptr( new MpNowPlayingBackEndPrivate( playerId, this ) )
{
    TX_ENTRY_ARGS( "Player ID =" << playerId )
    TX_EXIT
}

/*!
 \internal
 Destructs the now playing back end.
 */
MpNowPlayingBackEnd::~MpNowPlayingBackEnd()
{
    TX_ENTRY
    delete d_ptr;
    TX_EXIT
}

/*!
 \internal
 Updates the widget with the latest playback information.
 */
void MpNowPlayingBackEnd::update()
{
    TX_ENTRY
    d_ptr->update();
    TX_EXIT
}

    
/*!
 \internal
 Sends a play/pause command.
 */
void MpNowPlayingBackEnd::playPause()
{
    TX_ENTRY
     d_ptr->playPause();
    TX_EXIT
}
