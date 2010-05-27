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
* Description: Framework wraper for Playback Utility.
*
*/


#include "mpmpxplaybackframeworkwrapper.h"
#include "mpmpxplaybackframeworkwrapper_p.h"
#include "mptrace.h"

/*!
    \class MpMpxPlaybackFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
 Constructs the utility wrapper.
 */
MpMpxPlaybackFrameworkWrapper::MpMpxPlaybackFrameworkWrapper( TUid hostUid, QObject *parent )
    : QObject(parent)
{
    d_ptr = new MpMpxPlaybackFrameworkWrapperPrivate(this );
    d_ptr->init( hostUid );
}

/*!
 Destructs the utility wrapper.
 */
MpMpxPlaybackFrameworkWrapper::~MpMpxPlaybackFrameworkWrapper()
{
    delete d_ptr;
}

/*!
 Returns pointer to MpPlaybackData, which is the playback data.
 */
MpPlaybackData *MpMpxPlaybackFrameworkWrapper::playbackData()
{
    return d_ptr->playbackData();
}

/*!
 Slot to handle a play from Uri aFilename
 */
void MpMpxPlaybackFrameworkWrapper::play( QString aFilename )
{
    d_ptr->play( aFilename );
}

/*!
 Slot to handle a play from file handle
 */
void MpMpxPlaybackFrameworkWrapper::play( const XQSharableFile& file)
{
    d_ptr->play( file );
}

/*!
 Slot to handle a play pause.
 */
void MpMpxPlaybackFrameworkWrapper::playPause()
{
    d_ptr->playPause();
}

/*!
 Slot to handle a stop.
 */
void MpMpxPlaybackFrameworkWrapper::stop()
{
    d_ptr->stop();
}

/*!
 Slot to handle a skip forward.
 */
void MpMpxPlaybackFrameworkWrapper::skipForward()
{
    d_ptr->skipForward();
}

/*!
 Slot to handle seek forward.
 */
void MpMpxPlaybackFrameworkWrapper::startSeekForward()
{
    d_ptr->startSeekForward();
}

/*!
 Slot to handle stop seeking.
 */
void MpMpxPlaybackFrameworkWrapper::stopSeeking()
{
    d_ptr->stopSeeking();
}

/*!
 Slot to handle a skip backwards.
 */
void MpMpxPlaybackFrameworkWrapper::skipBackward()
{
    d_ptr->skipBackward();
}

/*!
 Slot to handle seek backwards.
 */

void MpMpxPlaybackFrameworkWrapper::startSeekBackward()
{
    d_ptr->startSeekBackward();
}

/*!
 Slot to handle a request to change \a position.
 */
void MpMpxPlaybackFrameworkWrapper::setPosition( int position )
{
    d_ptr->setPosition( position );
}

/*!
 Slot to handle a request to change shuffle \a mode.
 */
void MpMpxPlaybackFrameworkWrapper::setShuffle( bool mode )
{
    d_ptr->setShuffle( mode );
}

/*!
 Slot to handle a request to change repeat \a mode.
 */
void MpMpxPlaybackFrameworkWrapper::setRepeat( bool mode )
{
    d_ptr->setRepeat( mode );
}

/*!
 Change \a balance.
 */
void MpMpxPlaybackFrameworkWrapper::setBalance( int balance )
{
    d_ptr->setBalance( balance );
}

/*!
 Send command to apply audio effects.
 */
void MpMpxPlaybackFrameworkWrapper::applyAudioEffects()
{
    d_ptr->applyAudioEffects();
}

/*!
 Send command to apply equalizer.
 */
void MpMpxPlaybackFrameworkWrapper::applyEqualizer()
{
    d_ptr->applyEqualizer();
}
