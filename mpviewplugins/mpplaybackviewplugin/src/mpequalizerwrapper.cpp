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
* Description: Wrapper for equalizer utility.
*
*/

// System includes
#include <qstringlist>
#include <qdebug>

// User includes
#include "mpequalizerwrapper.h"
#include "mpequalizerwrapper_p.h"
#include "mptrace.h"

/*!
    \class MpEqualizerWrapper
    \ingroup musicplayer
    \brief Wrapper for equalizer utility.
    \since 10.1
    
    Playback wrapper provides Qt style interface to the audio equalizer
    utilities. Its implementation is hidden using private class data pattern.
    
    This class defines several APIs that are needed from \a MpPlaybackView
    and other components in \a mpplaybackviewplugin.

    \sa MpEqualizerWrapperPrivate
*/

/*!
    \fn MpEqualizerWrapper::equalizerReady()

    This signal will be emitted when Audio Eqalizer is initialized. This is
    when all APIs, like \a applyPreset, are ready to be used.
    
    \sa MpEqualizerWrapperPrivate::MapcInitComplete
*/

/*!
    Constructs a new MpEqualizerWrapper with \a parent and initializes
    private imaplementation.

*/
MpEqualizerWrapper::MpEqualizerWrapper( QObject *parent )
    : QObject(parent)
{
    TX_ENTRY

    d_ptr = new MpEqualizerWrapperPrivate(this);
    d_ptr->init();

    TX_EXIT
}

/*!
    Destructs the class and its private imaplementation.
    
 */
MpEqualizerWrapper::~MpEqualizerWrapper()
{
    TX_LOG

    delete d_ptr;
}

/*!
    Apply the preset by giving preset index. The index is starting with 0
    which if the "Off". The command then relays to its private implementation.

 */
void MpEqualizerWrapper::applyPreset( int preset )
{
    TX_ENTRY_ARGS( "Preset: " << preset );
    
	d_ptr->applyPreset(preset);

    TX_EXIT
}

/*!
    Disabling the eqaulizer. The command then relays to its private implementation.

 */
void MpEqualizerWrapper::disableEqualizer()
{
    TX_ENTRY

    d_ptr->disableEqualizer();

    TX_EXIT
}

/*!
    Returning currectly activated preset. The command then relays to its private 
    implementation. -1 will be returned if the adaptation is not ready or
    no available preset.

 */
int MpEqualizerWrapper::activePreset()
{
    TX_LOG
    return d_ptr->activePreset();
}

/*!
    Returning the list of availale preset names. The command then relays to its 
    private implementation. 

 */
 QStringList MpEqualizerWrapper::presetNames()
{
    TX_ENTRY_ARGS( "Names=" << d_ptr->presetNames());

    return d_ptr->presetNames();
}

 //End of File
