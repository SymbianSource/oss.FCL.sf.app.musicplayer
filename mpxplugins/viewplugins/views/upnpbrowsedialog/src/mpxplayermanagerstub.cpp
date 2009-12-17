/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Winscw player manager stub
*
*/


// INCLUDE FILES
#include    <e32std.h>

#include "mpxplayermanagerstub.h"
#include <mpxplaybackobserver.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::CMPXPlayerManagerStub
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXPlayerManagerStub::CMPXPlayerManagerStub()
    : CActive(EPriorityStandard)
    {
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::ConstructL()
    {
    // Add this active object to the scheduler.
    CActiveScheduler::Add(this);
    }

// Destructor
CMPXPlayerManagerStub::~CMPXPlayerManagerStub()
    {
    delete iSubPlayersNamesArray;
    Cancel();
    }    

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::RunL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::RunL()
    {
    delete iSubPlayersNamesArray;
    iSubPlayersNamesArray = NULL;
    iSubPlayersNamesArray = new (ELeave) CDesCArrayFlat(4);
    
    iSubPlayersNamesArray->AppendL(_L("Subplayer1"));
    iSubPlayersNamesArray->AppendL(_L("Local"));
    iSubPlayersNamesArray->AppendL(_L("Subplayer3"));
    iSubPlayersNamesArray->AppendL(_L("Subplayer4"));
    
    iCallback->HandleSubPlayerNamesL( iUid, iSubPlayersNamesArray, 
                                      ETrue, KErrNone);
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::DoCancel
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::GetPlayerTypesL
// -----------------------------------------------------------------------------
//    
void CMPXPlayerManagerStub::GetPlayerTypesL(RArray<TMPXPlaybackPlayerType>& /*aTypes*/)
    {
    }
    
// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::PlayerTypeDisplayNameL
// -----------------------------------------------------------------------------
//
HBufC* CMPXPlayerManagerStub::PlayerTypeDisplayNameL(TMPXPlaybackPlayerType /*aType*/)
    {
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::GetPlayerListL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::GetPlayerListL(RArray<TUid>& /*aPlayers*/)
    {
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::GetPlayerListL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::GetPlayerListL(RArray<TUid>& /*aPlayers*/,
                           TMPXPlaybackPlayerType /*aType*/)
    {
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::SubPlayerNamesL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::SubPlayerNamesL(MMPXPlaybackCallback& aCallback,
                             TUid aPlayer)
    {
    iCallback = &aCallback;
    iUid = aPlayer;
    if (!IsActive())
        {
        iStatus = KRequestPending;
        SetActive();
        }
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::SelectPlayersL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::SelectPlayersL(TMPXPlaybackPlayerType /*aType*/)
    {
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::SelectSubPlayerL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::SelectSubPlayerL(TUid /*aPlayer*/, TInt /*aSubPlayerIndex*/)
    {
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::SelectPlayerL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::SelectPlayerL(TUid /*aPlayer*/)
    {
    }

// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::ClearSelectPlayersL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::ClearSelectPlayersL()
    {
    }
    
// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::GetSelectionL
// -----------------------------------------------------------------------------
//
void CMPXPlayerManagerStub::GetSelectionL(TMPXPlaybackPlayerType& /*aType*/,
                           TUid& /*aPlayer*/,
                           TInt& /*aSubPlayerIndex*/,
                           HBufC*& /*aSubPlayerName*/)
    {
    }
    
// -----------------------------------------------------------------------------
// CMPXPlayerManagerStub::CurrentPlayer
// -----------------------------------------------------------------------------
//
MMPXPlayer* CMPXPlayerManagerStub::CurrentPlayer()
    {
    return NULL;
    }
        
// ----------------------------------------------------------------------------
// Returns current sub player name
// ----------------------------------------------------------------------------
//     
HBufC* CMPXPlayerManagerStub::GetSelectedSubPlayerNameL(TInt /*aSubPlayerIndex*/)
    {
    return NULL; 
    }        

//  End of File  
