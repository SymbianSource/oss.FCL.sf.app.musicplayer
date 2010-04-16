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
* Description: Framework wrapper - Private.
*
*/


#ifndef MPMPXDETAILSFRAMEWORKWRAPPER_P_H_
#define MPMPXDETAILSFRAMEWORKWRAPPER_P_H_

#include <qobject>
#include <mpxplaybackobserver.h>

//forward declartions
class MMPXPlaybackUtility;
class MpMpxDetailsFrameworkWrapper;
class CMPXMedia;
class MpSongData;

//class declaration
class MpMpxDetailsFrameworkWrapperPrivate : public MMPXPlaybackObserver,
                                            public MMPXPlaybackCallback
{

public:
    explicit MpMpxDetailsFrameworkWrapperPrivate( MpMpxDetailsFrameworkWrapper *qq );
    virtual ~MpMpxDetailsFrameworkWrapperPrivate();
    
public: //from MMPXPlaybackObserver
    void HandlePlaybackMessage( CMPXMessage *aMessage, TInt aError );
    
public: //from MMPXPlaybackCallback
    void HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError );
    void HandleSubPlayerNamesL( TUid aPlayer, const MDesCArray *aSubPlayers,
                                TBool aComplete, TInt aError );
    void HandleMediaL( const CMPXMedia& aProperties, TInt aError );
    
public:
    MpSongData *songData();
    void retrieveSong();
    
private:
    void constructProviderL();
    void destructProviderL();
    void doRetrieveSongL();
    
private:
    MpMpxDetailsFrameworkWrapper             *q_ptr; //not owned
    MpSongData                               *iSongData; // not owned
    MMPXPlaybackUtility                      *iPlaybackUtility; // owned
};


#endif /* MPMPXDETAILSFRAMEWORKWRAPPER_P_H_ */
