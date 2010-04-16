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
* Description: Wrapper for equalizer utility - private implementation.
*
*/

#ifndef MPEQUALIZERWRAPPER_P_H
#define MPEQUALIZERWRAPPER_P_H

// System includes
#include <e32base.h>
#include <mdaaudiosampleplayer.h>

// FORWARD DECLARATIONS
class MMPXPlaybackUtility;
class CAudioEqualizerUtility;

class MpEqualizerWrapperPrivate : public CBase,
                                          public MMdaAudioPlayerCallback
    {
public:

    MpEqualizerWrapperPrivate( MpEqualizerWrapper *wrapper );
    virtual ~MpEqualizerWrapperPrivate();

    void init();
    void applyPreset( int preset );
	int activePreset();
    void disableEqualizer();
    QStringList presetNames();

private:

    void DoInitL();
    void GetPresetId( TInt& aPresetId );
    TInt GetEngineIndexL(const TInt preset) const; 
    void HandlePresetActivation( TInt aActivatedPreset );
    void MapcInitComplete(
        TInt aError, const TTimeIntervalMicroSeconds& aDuration );
    void MapcPlayComplete( TInt aError );
    
private: 

    MpEqualizerWrapper *q_ptr;    
    CMdaAudioPlayerUtility* iMdaPlayUtility;
    MMPXPlaybackUtility* iMpxPlaybackUtility;    
    CAudioEqualizerUtility* iEqualizerUtility;

    TInt iCurrentPresetIndex;
    };

#endif // MPEQUALIZERWRAPPER_P_H

//End of File
