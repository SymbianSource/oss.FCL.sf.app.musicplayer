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
* Description: mpxplaybackutility stub for testing mpmpxframeworkwrapper
*
*/

#include "stub/inc/mpxplayermanager.h"
#include "mptrace.h"

MMPXPlayerManager::MMPXPlayerManager()
{
    TX_ENTRY_ARGS("STUB")    
}

MMPXPlayerManager::~MMPXPlayerManager()
{
    TX_ENTRY_ARGS("STUB")
}

void MMPXPlayerManager::GetPlayerTypesL(RArray<TMPXPlaybackPlayerType>& aTypes)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

HBufC* MMPXPlayerManager::PlayerTypeDisplayNameL(TMPXPlaybackPlayerType aType)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
    return 0;
}

void MMPXPlayerManager::GetPlayerListL(RArray<TUid>& aPlayers)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlayerManager::GetPlayerListL(RArray<TUid>& aPlayers, TMPXPlaybackPlayerType aType)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlayerManager::SubPlayerNamesL(MMPXPlaybackCallback& aCallback, TUid aPlayer)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlayerManager::SelectPlayersL(TMPXPlaybackPlayerType aType)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlayerManager::SelectSubPlayerL(TUid aPlayer, TInt aSubPlayerIndex)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlayerManager::SelectPlayerL(TUid aPlayer)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlayerManager::ClearSelectPlayersL()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

void MMPXPlayerManager::GetSelectionL(TMPXPlaybackPlayerType& aType,    
                   TUid& /*aPlayer*/,
                   TInt& /*aSubPlayerIndex*/,
                   HBufC*& /*aSubPlayerName*/)
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")    
}

MMPXPlayer* MMPXPlayerManager::CurrentPlayer()
{
    TX_ENTRY_ARGS("STUB WARNING! The unit test required to implement this. Currently empty imp")
    return 0;
}


