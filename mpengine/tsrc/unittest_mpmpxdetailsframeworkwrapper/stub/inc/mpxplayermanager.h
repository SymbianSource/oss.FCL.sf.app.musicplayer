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


#ifndef MPXPLAYERMANAGER_H
#define MPXPLAYERMANAGER_H

#include <e32base.h>
#include <e32cmn.h>
#include <mpxplaybackplugin.hrh>

class MMPXPlayer;
class MMPXPlaybackCallback;

class MMPXPlayerManager
{
public:
    MMPXPlayerManager();
    ~MMPXPlayerManager();
    
public: // MMPXPlayerManager
    void GetPlayerTypesL(RArray<TMPXPlaybackPlayerType>& aTypes);
    HBufC* PlayerTypeDisplayNameL(TMPXPlaybackPlayerType aType);
    void GetPlayerListL(RArray<TUid>& aPlayers);
    void GetPlayerListL(RArray<TUid>& aPlayers, TMPXPlaybackPlayerType aType);
    void SubPlayerNamesL(MMPXPlaybackCallback& aCallback, TUid aPlayer);
    void SelectPlayersL(TMPXPlaybackPlayerType aType);
    void SelectSubPlayerL(TUid aPlayer, TInt aSubPlayerIndex);
    void SelectPlayerL(TUid aPlayer);
    void ClearSelectPlayersL();
    void GetSelectionL(TMPXPlaybackPlayerType& aType,
                                   TUid& aPlayer,
                                   TInt& aSubPlayerIndex,
                                   HBufC*& aSubPlayerName);
    MMPXPlayer* CurrentPlayer();
};

#endif // MPXPLAYERMANAGER_H
