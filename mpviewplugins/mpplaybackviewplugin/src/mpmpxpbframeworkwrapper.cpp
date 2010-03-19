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
* Description: Framework wraper for Playback.
*
*/

#include "mpmpxpbframeworkwrapper.h"
#include "mpmpxpbframeworkwrapper_p.h"
#include "mptrace.h"

/*!
    \class MpMpxPbFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
 Constructs the utility wrapper.
 */
MpMpxPbFrameworkWrapper::MpMpxPbFrameworkWrapper( QObject *parent )
    : QObject(parent)
{
    d_ptr = new MpMpxPbFrameworkWrapperPrivate(this );
    d_ptr->init();
}

/*!
 Destructs the utility wrapper.
 */
MpMpxPbFrameworkWrapper::~MpMpxPbFrameworkWrapper()
{
    delete d_ptr;
}

/*!
 Returns pointer to MpPlaybackData, which is the playback data.
 */
MpPlaybackData *MpMpxPbFrameworkWrapper::playbackData()
{
    return d_ptr->playbackData();
}

/*!
 Slot to handle a play pause.
 */
void MpMpxPbFrameworkWrapper::playPause()
{
    d_ptr->playPause();
}

/*!
 Slot to handle a stop.
 */
void MpMpxPbFrameworkWrapper::stop()
{
    d_ptr->stop();
}

/*!
 Slot to handle a skeep forward.
 */
void MpMpxPbFrameworkWrapper::skipForward()
{
    d_ptr->skipForward();
}

/*!
 Slot to handle a skeep backwards.
 */
void MpMpxPbFrameworkWrapper::skipBackward()
{
    d_ptr->skipBackward();
}

/*!
 Slot to handle a request to change \a position.
 */
void MpMpxPbFrameworkWrapper::setPosition( int position )
{
    d_ptr->setPosition( position );
}

/*!
 Slot to handle a request to change shuffle \a mode.
 */
void MpMpxPbFrameworkWrapper::setShuffle( bool mode )
{
    d_ptr->setShuffle( mode );
}

/*!
 Slot to handle a request to change repeat \a mode.
 */
void MpMpxPbFrameworkWrapper::setRepeat( bool mode )
{
    d_ptr->setRepeat( mode );
}

