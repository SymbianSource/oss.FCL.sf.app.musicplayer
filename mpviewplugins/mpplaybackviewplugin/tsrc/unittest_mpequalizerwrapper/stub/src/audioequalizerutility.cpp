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
* Description:   Stub Utility to Access the Equalizer Effect
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "stub/inc/audioequalizerutility.h"
#include "mptrace.h"


// ============================ MEMBER FUNCTIONS ===============================

CAudioEqualizerUtility::CAudioEqualizerUtility()
	{
    }


// Destructor
CAudioEqualizerUtility::~CAudioEqualizerUtility()
    {
		iPresetArray.Close();
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ConstructL(CMdaAudioPlayerUtility& /*aUtility*/)
    {
		UpdatePresetArray(iPresetArray);
	}

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAudioEqualizerUtility* CAudioEqualizerUtility::NewL(CMdaAudioPlayerUtility& aUtility)
    {
		CAudioEqualizerUtility* self = new( ELeave ) CAudioEqualizerUtility;
		CleanupStack::PushL( self );
		self->ConstructL(aUtility);
		CleanupStack::Pop(self);
		return self;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::ApplyPresetL()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::ApplyPresetL(
    TInt /*aPresetIndex*/)
    {
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::GetPresetL()
// Gets the Preset from the Central Repository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CAudioEqualizerUtility::GetPresetL(
    TInt aPresetIndex)
    {
    TX_ENTRY

		if(aPresetIndex > (iPresetArray.Count() - 1) || aPresetIndex < 0 )
		{
			User::Leave(KErrArgument);
		}

		return iPresetArray[aPresetIndex].iPresetName;

    TX_EXIT
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::DisablePresetL()
// Disable the Preset
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CAudioEqualizerUtility::DisableEqualizerL()
{
}

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::Presets()
// Applies the Preset to the Equalizer Object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TArray<TEfAudioEqualizerUtilityPreset> CAudioEqualizerUtility::Presets()
    {
		return iPresetArray.Array();
    }

// -----------------------------------------------------------------------------
// CAudioEqualizerUtility::UpdatePresetArray(RArray<TDesC>& aPresetArray,const RArray<TEfAudioEqualizerUtilityPreset> &aFullPresetArray)
// Creates the Descriptor Array of Names of the predifined presets
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAudioEqualizerUtility::UpdatePresetArray(RArray<TEfAudioEqualizerUtilityPreset>& aPresetArray)
	{

		aPresetArray.Reset();
		
		TEfAudioEqualizerUtilityPreset	tempPreset1;		
		tempPreset1.iPresetDescriptionKey = 0x10;
		tempPreset1.iPresetName = _L("Bass");
		tempPreset1.iPresetNameKey = 0x0A;
		aPresetArray.Append(tempPreset1);
		
        TEfAudioEqualizerUtilityPreset  tempPreset2;        
        tempPreset2.iPresetDescriptionKey = 0x20;
        tempPreset2.iPresetName = _L("Classical");
        tempPreset2.iPresetNameKey = 0x0B;
        aPresetArray.Append(tempPreset2);

        TEfAudioEqualizerUtilityPreset  tempPreset3;        
        tempPreset3.iPresetDescriptionKey = 0x30;
        tempPreset3.iPresetName = _L("Pop");
        tempPreset3.iPresetNameKey = 0x0C;
        aPresetArray.Append(tempPreset3);
        
        TEfAudioEqualizerUtilityPreset  tempPreset4;        
        tempPreset4.iPresetDescriptionKey = 0x40;
        tempPreset4.iPresetName = _L("Jazz");
        tempPreset4.iPresetNameKey = 0x0D;
        aPresetArray.Append(tempPreset4);

        TEfAudioEqualizerUtilityPreset  tempPreset5;        
        tempPreset5.iPresetDescriptionKey = 0x50;
        tempPreset5.iPresetName = _L("Rock");
        tempPreset5.iPresetNameKey = 0x0E;
        aPresetArray.Append(tempPreset5);
	}

//  End of File
