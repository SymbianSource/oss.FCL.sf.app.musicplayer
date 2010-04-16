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
* Description: CAudioEqualizerUtility stub for testing mpmpxframeworkwrapper
*
*/

#ifndef CAUDIOEQUALIZERUTILITY_H
#define CAUDIOEQUALIZERUTILITY_H

//  INCLUDES
#include <e32base.h>
#include <e32svr.h>
#include <AudioEqualizerBase.h>
#include <centralrepository.h>
#include <AudioEqualizerUtilityData.h>

// FORWARD DECLARATIONS
class CMdaAudioPlayerUtility;
class CRepository;


// CLASS DECLARATION

/**
*  This is the Equalizer Utility class for managing audio equalizer presets.
*
*  @lib AudioEqualizerUtility.lib
*  @since 3.0
*/

class CAudioEqualizerUtility : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Factory function for creating the audio equalizer utility object.
        * @since 3.0
        * @param aUtility A reference to a audio player utility
        * @return pointer to CAudioEqualizerUtility object
        */
        static CAudioEqualizerUtility* NewL(CMdaAudioPlayerUtility& aUtility);

         /**
        * Destructor.
        */
        virtual ~CAudioEqualizerUtility();

        /**
        * Apply the Preset Index value
        * @since 3.0
        * @param aPreset Preset Value
        */
        void ApplyPresetL(TInt aPreset);

        /**
        * Disable the Equalizer
        * @since 3.0
        */
        void DisableEqualizerL();

        /**
        * Retrieves a Preset with the given index from the Central Repository
        * @since 3.0
        * @param aPresetIndex Index of the Preset in the Array
        */
        const TDesC& GetPresetL(TInt aPresetIndex);

        /**
        * Get all the Presets defined in the Central Repository
        * @since 3.0
        * @return Array of all Presets.
        */
        TArray<TEfAudioEqualizerUtilityPreset> Presets();

    private:

        /**
        * C++ default constructor.
        */
        CAudioEqualizerUtility();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        
        /**
        * Second Phase Costructor for CMdaAudioPlayerUtility
        * @since 3.0
        * @param aUtility CMdaAudioPlayerUtility Referece
        */
        void ConstructL(CMdaAudioPlayerUtility& aUtility);


        /**
        * Function of Updating the Preset Array for the User
        * @since 3.0
        * @param aUtility MCustomInterface Referece
        */
        void UpdatePresetArray(RArray<TEfAudioEqualizerUtilityPreset>& aPresetArray);

    private: // Data

        // Array for Storing the Current State of the Repository
        // which has been compacted after the transaction
        // with the Central Repository
        RArray<TEfAudioEqualizerUtilityPreset> iPresetArray;

   };

#endif      // CAUDIOEQUALIZERUTILITY_H
// End of File
