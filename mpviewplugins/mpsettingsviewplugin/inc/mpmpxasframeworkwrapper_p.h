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
* Description: Framework wraper for Audio Settings - Private.
*
*/

#ifndef MPMPXASFRAMEWORKWRAPPER_P_H
#define MPMPXASFRAMEWORKWRAPPER_P_H

#include <mpxaudioeffectproperties.h>

class MMPXPlaybackUtility;
class CMPXAudioEffectProperties;

class MpMpxAsFrameworkWrapperPrivate 
{
public:
    explicit MpMpxAsFrameworkWrapperPrivate(); 
    virtual ~MpMpxAsFrameworkWrapperPrivate();
    
    
public:
    
    void init();
    void setBalance(int balance);
    void setLoudness(bool mode);
    int  balance();
    bool loudness();
            
private:
    
    void doInitL();
    void doSetBalanceL(int balance);
    void doSetLoudnessL(bool mode);
    int  balanceL();
    bool loudnessL();
             
private:
    MMPXPlaybackUtility           *iPlaybackUtility; // owned
    CMPXAudioEffectProperties     *iAudioEffectProperties; // owned
};

#endif /*MPMPXASFRAMEWORKWRAPPER_P_H*/
