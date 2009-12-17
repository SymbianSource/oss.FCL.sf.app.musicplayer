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
* Description:  Audio effects model implementation.
*
*/


#ifndef MPXAUDIOEFFECTSMODEL_H
#define MPXAUDIOEFFECTSMODEL_H

// INCLUDES

// SYSTEM INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <aknViewAppUi.h>

// FORWARD DECLARATIONS
class CAknViewAppUi;
class CMsEqualizerHelper;
class CEikStatusPane;
class CMPXAudioEffectProperties;
class CMPXAudioEffectEngine;
class MMPXPlaybackUtility;

/**
*  CMPXAudioEffectsModel, Model class for audio settings 
*  
*  @lib mpxaudioeffectsview.dll
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(CMPXAudioEffectsModel) : public CBase
    {
public:
    friend class CMPXAudioEffectsItems;

public: // accessors
    
    /**
    * Virtual Destructor
    */
    virtual	~CMPXAudioEffectsModel();
    
    /**
    * Two-phased constructor
    *
    * @param aEngine Audio effect engine 
    * @return CMPXAudioEffectsModel
    */
	static CMPXAudioEffectsModel* NewL(MMPXPlaybackUtility& aEngine);
	
	/**
    * Sets the balance
    */
    void SetBalanceL();
	
	/**
    * Sets the bass boost
    */
	void SetBassBoostL();
	
	/**
    * Sets the stereo widening
    */
	void SetStereoWideningL();
 	
 	/**
    * Sets the reverberation
    */
 	void SetReverberationL();

    /**
    * Sets the loudness
    */
    void SetLoudnessL();

private:

    /**
    *  2nd-phased constructor
    */
    void ConstructL();
    
    /**
    * Constructor
    *
    * aEngine Audio effect engine 
    */
	CMPXAudioEffectsModel(MMPXPlaybackUtility& aEngine);

private: //Data
	
	TInt  iBalance; //(From -100 to 100, 0 = center)
	TInt  iReverb; //(from 1 to 8, 0 = default)
	TBool iStereo; //(default off)
	TBool iBassBoost; //(default off)
    TBuf<32> iPresetName;
    TBool iLoudness; 
    MMPXPlaybackUtility& iPlaybackUtility;  // not own
    CMPXAudioEffectProperties* iProp;
	};

#endif  // MPXAUDIOEFFECTSMODEL_H

// End of File
