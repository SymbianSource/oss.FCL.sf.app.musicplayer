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
* Description: Wrapper for mpx framework utilities.
*
*/

#include "mpengine.h"
#include "mptrace.h"


MpEngine::MpEngine()
    : mBalance(0),
    mLoudness(0)
{
    TX_LOG_ARGS("Stub")
}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{
    TX_ENTRY_ARGS("Stub")
}

// Audio Effects related
/*!
 Destructs music player engine.
 */
int MpEngine::balance()
{
    return mBalance;
}

/*!
 Destructs music player engine.
 */
bool MpEngine::loudness()
{
    return mLoudness;
}


//slots:
/*!
 Destructs music player engine.
 */
void MpEngine::setBalance( int balance )
{
     mBalance=balance;
}

/*!
 Destructs music player engine.
 */
void MpEngine::setLoudness( bool mode )
{
    mLoudness=mode;
}
