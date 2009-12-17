/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of winscw player manager stub
*
*/


#ifndef CMPXPLAYERMANAGERSTUB_H
#define CMPXPLAYERMANAGERSTUB_H

// INCLUDES
#include <e32base.h>
#include <mpxplaybackutility.h>

// FORWARD DECLARATIONS

/**
* CMPXPlayerManagerStub
*
* Stub for player manager in Winscw
* @since S60 3.1
*/
NONSHARABLE_CLASS(CMPXPlayerManagerStub) : public CActive, public MMPXPlayerManager
    {
public:
    /**
    * C++ default constructor.
    */
    CMPXPlayerManagerStub();

    /**
    * Symbian OS constructor.
    */
    void ConstructL();

    /**
    * Destructor.
    */
    virtual ~CMPXPlayerManagerStub();
    
public: // New functions

    /**
    *  Typically, a client will obtain the list of all the (non-duplicated)
    *  player 'types' in the system from GetPlayerTypesL and present localized
    *  type names (e.g. "Local","Home network", if possible, else if 
    *  non-standard type,should use PlayerTypeDisplayNameL to get text from 
    *  player).
    *
    *  User should be given opportunity to choose IF there is more than one 
    *  type available, or if there is one type but more than one sub players,
    *  the user could be presented with the sub player list.
    *
    *  @param aTypes the list of player types
    */
    virtual void GetPlayerTypesL(RArray<TMPXPlaybackPlayerType>& aTypes); 
    
    /**
    *  Returns display name for custom types; standard types should be 
    *  recognized by client and localized - calling this method would then not
    *  be required
    *
    *  @param aType playback type
    */
    virtual HBufC* PlayerTypeDisplayNameL(TMPXPlaybackPlayerType aType);
    
    /**
    *  Get the list of UIDs of all players
    *
    *  @param aPlayers All the players' uids in the system
    */
    virtual void GetPlayerListL(RArray<TUid>& aPlayers); 

    /**
    *  Get the list of UIDs of players with the specific type 
    * 
    *  @param aPlayers All the players' uids with the same type
    *  @param aType the type of players
    */
    virtual void GetPlayerListL(RArray<TUid>& aPlayers,
                               TMPXPlaybackPlayerType aType);

    /**
    *  Get the list of sub player names, Async
    * 
    *  @param aCallback sub players returned in call back interface 
    *  @param aPlayer UID of the player
    */
    virtual void SubPlayerNamesL(MMPXPlaybackCallback& aCallback,
                                 TUid aPlayer); 
    
    /**
    *  Client selects all players with the type aType, the specific player used
    *  then being resolved on the basis of the content. 
    *
    *  @param aType the type of player
    */
    virtual void SelectPlayersL(TMPXPlaybackPlayerType aType);
    
    /**
    *  Client selects specific player aPlayer and sub player aSubPlayerIndex 
    *  from array returned by SubPlayerNamesL
    *
    *  @param aPlayer the UID of player
    *  @param aSubPlayerIndex the index of the sub player
    */
    virtual void SelectSubPlayerL(TUid aPlayer, TInt aSubPlayerIndex);
    
    /**
    *  Client selects a specific player 
    *
    *  @param aPlayer the UID of the player
    */
    virtual void SelectPlayerL(TUid aPlayer);
    
    /**
    *  Clears all selection criteria. Essentially the same as 
    *  SelectPlayersL(EPbLocal).
    */
    virtual void ClearSelectPlayersL();
    
    /**
    *  Retreives the current selection: KNullUid and KErrNotFound are possible
    *  return values for aPlayer and aSubPlayerIndex respectively if none are
    *  explicitly selected
    *
    *  @param aType player type
    *  @param aPlayer the UID of player
    *  @param aSubPlayerIndex index of the subplayer
    */
    virtual void GetSelectionL(TMPXPlaybackPlayerType& aType,
                               TUid& aPlayer,
                               TInt& aSubPlayerIndex,
                               HBufC*& aSubPlayerName);
    
    /**
    *  The current player, may change with each song, NULL if none found
    *
    *  @return object of current player
    */
    virtual MMPXPlayer* CurrentPlayer(); 
    
    /**
    *  From MMPXPlayerManager
    *  Get the name of the current sub player
    * 
    *  @param aSubPlayerIndex the index of the sub player 
    *  @return friendly name of the sub player
    */
    virtual HBufC* GetSelectedSubPlayerNameL(TInt aSubPlayerIndex);    
    
private:
    /**
    * @see CActive::RunL()
    */
    void RunL();

    /**
    * @see CActive::DoCancel()
    */
    void DoCancel();

private: // Data
    MMPXPlaybackCallback* iCallback;
    CDesCArrayFlat* iSubPlayersNamesArray;
    TUid iUid;
    };
          
#endif // CMPXPLAYERMANAGERSTUB_H
            
// End of File
