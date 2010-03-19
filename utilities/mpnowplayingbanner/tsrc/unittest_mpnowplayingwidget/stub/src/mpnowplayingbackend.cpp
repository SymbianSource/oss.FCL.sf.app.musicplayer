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
* Description: mpxplaybackutility stub for testing MpNowPlayingBackEnd
*
*/

#include "stub/inc/mpnowplayingbackend.h"


int gUpdateCounter = 0;
int gPlayPauseCounter = 0;


/*!
 Returns gUpdateCounter.
 gUpdateCounter counts the number of MpNowPlayingBackEnd::update calls.
 */
int MpNowPlayingBackEnd::getUpdateCounter()
{
    return gUpdateCounter;
}

/*!
 Resets gUpdateCounter to zero.
 */
void MpNowPlayingBackEnd::resetUpdateCounter()
{
    gUpdateCounter = 0;
}

/*!
 Returns gPlayPauseCounter.
 gPlayPauseCounter counts the number of MpNowPlayingBackEnd::playPause calls.
 */
int MpNowPlayingBackEnd::getPlayPauseCounter()
{
    return gPlayPauseCounter;
}

/*!
 Resets gPlayPauseCounter to zero.
 */
void MpNowPlayingBackEnd::resetPlayPauseCounter()
{
    gPlayPauseCounter = 0;
}

/*!
 Causes signal stateUpdate to be emitted.
 */
void MpNowPlayingBackEnd::triggerStateUpdate(SimplifiedPlayerState state)
{
    emit stateUpdate(state);
}

/*!
 Causes signal titleChanged to be emitted.
 */
void MpNowPlayingBackEnd::triggerTitleChanged(const QString &text)
{
    emit titleChanged(text);
}

/*!
 Causes signal artistChanged to be emitted.
 */
void MpNowPlayingBackEnd::triggerArtistChanged(const QString &text)
{
    emit artistChanged(text);
}

/*!
 Stub function.
 */
MpNowPlayingBackEnd::MpNowPlayingBackEnd( long int playerId )
{
    Q_UNUSED(playerId);
}

/*!
 Stub function.
 */
MpNowPlayingBackEnd::~MpNowPlayingBackEnd()
{
}

/*!
 Stub function.
 */
void MpNowPlayingBackEnd::update()
{
    gUpdateCounter++;
}

/*!
 Stub function.
 */
void MpNowPlayingBackEnd::playPause()
{
    gPlayPauseCounter++;
}

