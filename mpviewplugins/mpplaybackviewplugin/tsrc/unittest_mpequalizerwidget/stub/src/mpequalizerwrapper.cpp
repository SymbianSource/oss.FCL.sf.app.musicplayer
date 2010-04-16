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
#include "mptrace.h"

// User includes
#include "stub/inc/mpequalizerwrapper.h"

/*!
    Constructs a new MpEqualizerWrapper with \a parent 
*/
MpEqualizerWrapper::MpEqualizerWrapper(QObject *parent)
    : mActivePreset(-1),
      mApplyPresetCount(0),
      mDisableEquqlizerCount(0)
{   
    Q_UNUSED(parent);
    TX_ENTRY
    TX_LOG_ARGS( "Stub wrapper" );
    TX_EXIT
}

/*!
    Destructs the class and its private imaplementation.
    
    \sa MpEqualizerWrapperPrivate
 */
MpEqualizerWrapper::~MpEqualizerWrapper()
{
    TX_ENTRY
    TX_LOG_ARGS( "Stub wrapper" );
    TX_EXIT
}

/*!
    Apply the preset by giving preset index. The index is starting with 0
    which if the "Off". The command then relays to its private implementation.

    \sa MpEqualizerWrapperPrivate
 */
void MpEqualizerWrapper::applyPreset( int preset )
{
    mApplyPresetCount++;
    mActivePreset = preset;
}

/*!
    Disabling the eqaulizer. The command then relays to its private implementation.

    \sa MpEqualizerWrapperPrivate
 */
void MpEqualizerWrapper::disableEqualizer()
{
    mDisableEquqlizerCount++;
    mActivePreset = -1;
}

/*!
    Returning currectly activated preset. The command then relays to its private 
    implementation. -1 will be returned if the adaptation is not ready or
    no available preset.

    \sa MpEqualizerWrapperPrivate
 */
int MpEqualizerWrapper::activePreset()
{
    return mActivePreset;
}

/*!
    Returning the list of availale preset names. The command then relays to its 
    private implementation. 

    \sa MpEqualizerWrapperPrivate
 */
QStringList MpEqualizerWrapper::presetNames()
{
    QStringList presets;
    presets << "Bass Booster" << "Classic" << "Jazz" << "Pop" << "Rock" ;
    return presets;
}

 //End of File
