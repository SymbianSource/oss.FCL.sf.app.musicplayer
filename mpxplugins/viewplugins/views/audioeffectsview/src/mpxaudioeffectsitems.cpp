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
* Description:  Setting List
*
*/


// INCLUDE FILES 

#include <aknnotewrappers.h>
#include <aknsettingpage.h> 
#include <aknslidersettingpage.h> 
#include <eikfrlbd.h> 
#include <eikfrlb.h> 
#include <mpxlog.h>

#include <mpxaudioeffectsview.rsg>
#include <avkon.rsg>
#include <StringLoader.h>

// User Includes
#include "mpxaudioeffectsitems.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::CMPXAudioEffectsItems()
// Constructor
// -----------------------------------------------------------------------------
//  
CMPXAudioEffectsItems::CMPXAudioEffectsItems(CMPXAudioEffectsModel* aModel)
    {
    iModel = aModel;
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::~CMPXAudioEffectsItems()
// Destructor
// -----------------------------------------------------------------------------
//
CMPXAudioEffectsItems::~CMPXAudioEffectsItems()
    {
    }


// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::SizeChanged()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsItems::SizeChanged()
    {
    if( ListBox() ) 
        {
        ListBox()->SetRect( Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::CreateSettingItemL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAknSettingItem* CMPXAudioEffectsItems::CreateSettingItemL(TInt aIdentifier)
    {
    CAknSettingItem* settingItem = NULL;
    
    switch (aIdentifier)
        {
        case EEqualizerSettingItem: // Not used
            settingItem = new (ELeave) CAknTextSettingItem(aIdentifier, 
                                                         iModel->iPresetName);
            break;
        case EBalanceSettingItem:
            settingItem = new (ELeave) CAknSliderSettingItem(aIdentifier, 
                                                             iModel->iBalance);
            break;
        case EReverbSettingItem:
            settingItem = new ( ELeave ) CAknEnumeratedTextPopupSettingItem(
                                                    aIdentifier,iModel->iReverb);
            break;
        case EStereoWideningSettingItem:
            settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, 
                                                                  iModel->iStereo);
            break;
        case ELoudnessItem:
            settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier, 
                                                                  iModel->iLoudness);
          break;
        }
    return settingItem;
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::ChangeSelectedItemL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsItems::ChangeSelectedItemL()
    {
    EditItemL(ListBox()->CurrentItemIndex(), ETrue);  
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::EditItemL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsItems::EditItemL(TInt aIndex, TBool aCalledFromMenu)
    {
    if (aIndex==0) //balance
        {
        TInt oldValue = iModel->iBalance;

        CAknSliderSettingPage* dlg = 
                new(ELeave) CAknSliderSettingPage(R_BALANCE_SETTING_PAGE, 
                                                  iModel->iBalance);
        dlg->SetSettingId(EBalanceSettingItem);
        dlg->SetSettingPageObserver(this);
        SetBalanceTextL(dlg);
        dlg->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
          
        MPX_TRAPD( err, iModel->SetBalanceL() );
        if(err != KErrNone)// the slider button will return to the previous
                           // position if an error accures
            {
            iModel->iBalance = oldValue;
            TRAP_IGNORE(iModel->SetBalanceL()); // update value in file
            }
            
        // Redraw the text box correctly
        CAknSliderSettingItem* settingItem = (CAknSliderSettingItem*) 
                                                 SettingItemArray()->At(0);
        settingItem->LoadL();
        settingItem->UpdateListBoxTextL();
        }
    else if (aIndex==1) //Loundness
        {
        TBool oldLoudnessValue = iModel->iLoudness;
        CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
        (*SettingItemArray())[aIndex]->StoreL(); //activates change in listbox
        MPX_TRAPD( err, iModel->SetLoudnessL() );    //calls engine to change API

        if (err != KErrNone) //change in UI will not accure if API is not changed
            {
            CAknSettingItemList::EditItemL( aIndex, EFalse );
            iModel->iLoudness = oldLoudnessValue;
            TRAP_IGNORE(iModel->SetLoudnessL());
            }
        }
    else if (aIndex==2)//stereo widening
        {
        TBool oldStereoValue = iModel->iStereo;
        CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
        (*SettingItemArray())[aIndex]->StoreL(); //activates change in listbox
        MPX_TRAPD( err, iModel->SetStereoWideningL() ); //calls engine to change API

        if (err != KErrNone) //change in UI will not accure if API is not 
                             //changed(=error in API)
            {
            CAknSettingItemList::EditItemL( aIndex, EFalse );
            iModel->iStereo = oldStereoValue;
            TRAP_IGNORE(iModel->SetStereoWideningL()); // update value in file 
            }
        }
    DrawDeferred();
    }


// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::HandleSettingPageEventL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsItems::HandleSettingPageEventL(
    CAknSettingPage* aSettingPage,
    TAknSettingPageEvent aEventType)
    {
    TInt id = aSettingPage->SettingId();

    if (aEventType == EEventSettingChanged)
        {
        switch (id)
            {
            case EBalanceSettingItem:
                {
                CAknSliderSettingPage* slider = 
                            static_cast<CAknSliderSettingPage*>(aSettingPage);
                iModel->iBalance = slider->SliderControl()->Value();
                iModel->SetBalanceL();
                SetBalanceTextL( slider );
                break;
                }
            default:
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAudioEffectsItems::SetBalanceTextL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsItems::SetBalanceTextL( CAknSliderSettingPage* slider )
    {
    HBufC* title = NULL;
    
    switch ( iModel->iBalance )
        {
        
        case -100:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_LEFT100 );
            break;
            }
        case -75:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_LEFT75 );
            break;
            }    
        case -50:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_LEFT50 );
            break;
            }
        case -25:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_LEFT25 );
            break;
            }
        case 0:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_CENTER );
            break;
            }
        case 25:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_RIGHT25 );
            break;
            }    
        case 50:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_RIGHT50 );
            break;
            }
        case 75:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_RIGHT75 );
            break;
            }    
        case 100:
            {
            title = StringLoader::LoadLC( R_AUDIOEFFECT_BALANCE_RIGHT100 );
            break;
            }                    
        default: 
        break;
        }
        
    if( title )
        {
        slider->SetSettingTextL( *title );
        slider->DrawNow();
        CleanupStack::PopAndDestroy( title );
        }
    }

// End of file
