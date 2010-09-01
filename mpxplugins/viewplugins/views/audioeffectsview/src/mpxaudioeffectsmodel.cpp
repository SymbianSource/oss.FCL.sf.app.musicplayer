/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Engine class that uses the Effects API and saves changes.
*
*/


// INCLUDE FILES
#include  <s32file.h>
#include  <s32std.h>

#include  "mpxaudioeffectproperties.h"
#include  <mpxaudioeffectsview.rsg>
#include  <mpxplaybackutility.h>
#include  <mpxlog.h>

#include  "mpxaudioeffects.hrh"
#include  "mpxaudioeffectsmodel.h"

// CONSTANTS
const TInt KAudioEffectsCustomCommand = 0x101FFC02;

// ================= MEMBER FUNCTIONS =======================
   
// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMPXAudioEffectsModel* CMPXAudioEffectsModel::NewL(
    MMPXPlaybackUtility& aPlaybackUtility)
    {
    CMPXAudioEffectsModel* self = new( ELeave ) 
                                        CMPXAudioEffectsModel(aPlaybackUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::CMPXAudioEffectsModel()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMPXAudioEffectsModel::CMPXAudioEffectsModel(MMPXPlaybackUtility& aPlaybackUtility)
    :iPlaybackUtility(aPlaybackUtility)
    {
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::~CMPXAudioEffectsModel()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMPXAudioEffectsModel::~CMPXAudioEffectsModel()
    {
    if (iProp)
        {
        delete iProp;
        }
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::ConstructL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsModel::ConstructL()
    {
    iProp = new(ELeave) CMPXAudioEffectProperties();   
    MPX_TRAPD( err, iProp->LoadFromFileL() ); //Loads saved settings if available
    if( err != KErrNone )
        {
        MPX_DEBUG1("CMPXAudioEffectsModel::ConstructL -- load error");
        iProp->Reset();
        iBalance = 0;
        iReverb = 0;
        iStereo = EFalse;
        iBassBoost = EFalse;
        iLoudness = EFalse;
#if 1 // VCPCC_MOD
        iLoudness=EFalse; 
#endif // VCPCC_MOD   
        MPX_DEBUG1("CMPXAudioEffectsModel::ConstructL -- save to file");
        iProp->SaveToFileL();        
        }
    else
        {
        iBalance = iProp->Balance();
        iReverb = iProp->Reverb();
        iStereo = iProp->Stereo();
        iBassBoost = iProp->BassBoost(); 
        iLoudness = iProp->Loudness();   
        }
    }


// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::SetBalanceL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsModel::SetBalanceL()
    {
    iProp->SetBalance(iBalance);
    MPX_DEBUG1("CMPXAudioEffectsModel::SetBalanceL -- save to file");
    iProp->SaveToFileL();

    iPlaybackUtility.CommandL( EPbApplyEffect, KAudioEffectsCustomCommand );
    }
  
  
// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::SetReverberationL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsModel::SetReverberationL()
    {
    iProp->SetReverb(iReverb);
    MPX_DEBUG1("CMPXAudioEffectsModel::SetReverberationL -- reverb save to file");
    iProp->SaveToFileL();
    
    iPlaybackUtility.CommandL( EPbApplyEffect, KAudioEffectsCustomCommand );
    }
  

// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::SetStereoWideningL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsModel::SetStereoWideningL()
    {
    iProp->SetStereo(iStereo);
    MPX_DEBUG1("CMPXAudioEffectsModel::SetStereoWideningL -- set stereo save to file");
    iProp->SaveToFileL();
    
    iPlaybackUtility.CommandL( EPbApplyEffect, KAudioEffectsCustomCommand );
    }


// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::SetBassBoostL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsModel::SetBassBoostL()
    {
#if 0 // VCPCC_MOD    
    iProp->SetBassBoost(iBassBoost);
    iProp->SaveToFileL();

    iPlaybackUtility.CommandL( EPbApplyEffect, KAudioEffectsCustomCommand );
#endif // VCPCC_MOD   
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsModel::SetLoudnessL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsModel::SetLoudnessL()
    {
    iProp->SetLoudness(iLoudness);
    
    MPX_DEBUG1("CMPXAudioEffectsModel::SetLoudnessL -- save to file");
    iProp->SaveToFileL();
    
    iPlaybackUtility.CommandL( EPbApplyEffect, KAudioEffectsCustomCommand );
    }

// End of file





