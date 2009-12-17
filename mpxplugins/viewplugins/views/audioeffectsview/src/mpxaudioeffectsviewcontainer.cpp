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
* Description:  Container class.
*
*/



// INCLUDE FILES
#include <hlplch.h>
#include <akntitle.h>
#include <eikspane.h>
#include <mpxaudioeffectsview.rsg>
#include <mpxconstants.h>

#include "mpxaudioeffectsview.hlp.hrh"
#include "mpxaudioeffectsitems.h"
#include "mpxaudioeffectsviewcontainer.h"
#include "mpxaudioeffectsviewimp.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXAudioEffectsViewContainer::CMPXAudioEffectsViewContainer()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// Symbian OS 2nd phase constructor.  Creates a Window for the controls, which
// it contains. Constructs a label and adds it to the window, which it then
// activates.
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsViewContainer::ConstructL(
    const TRect& aRect,
    CMPXAudioEffectsModel* aModel )
    {
    CreateWindowL();
    SetRect( aRect );

    iModel = aModel;

    //inserting title
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
        ( iAvkonViewAppUi->StatusPane()->ControlL(
            TUid::Uid( EEikStatusPaneUidTitle ) ) );

    HBufC* title =
        CEikonEnv::Static()->AllocReadResourceL( R_AUDIOEFFECT_CAPTION_TEXT );
    titlePane->SetText( title );   // Ownership transferred

    //inserting setting list
    iSettingItems = new ( ELeave ) CMPXAudioEffectsItems( iModel );
    iSettingItems->SetContainerWindowL( *this );
    iSettingItems->ConstructFromResourceL( R_AUDIOEFFECTS_SETTING_ITEM_LIST );
    // Layout setting list box
    SizeChanged();
    ActivateL();
    }

// -----------------------------------------------------------------------------
// Destructor.  Frees up memory for the settings list.
// -----------------------------------------------------------------------------
//
CMPXAudioEffectsViewContainer::~CMPXAudioEffectsViewContainer()
    {
    delete iSettingItems;
    }

// -----------------------------------------------------------------------------
// Asks the setting list to change the currently selected item
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsViewContainer::ChangeSelectedItemL()
    {
    if ( iSettingItems )
        {
        iSettingItems->ChangeSelectedItemL();
        }
    }

// -----------------------------------------------------------------------------
// From CCoeControl
// Called by the framework in compound controls
// -----------------------------------------------------------------------------
//
TInt CMPXAudioEffectsViewContainer::CountComponentControls() const
    {
    return 1; // return number of controls inside this container
    }

// -----------------------------------------------------------------------------
// From CCoeControl
// Called by the framework in compound controls
// -----------------------------------------------------------------------------
//
CCoeControl* CMPXAudioEffectsViewContainer::ComponentControl(
    TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iSettingItems;
            }
        default:
            {
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// From CCoeControl
// Process a key event
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXAudioEffectsViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    // VC_PCC_MOD: Do not merge changes in this function, Music Player uses 
    // the volumne popup instead of navi pane
    //
    if ( iSettingItems )
        {
        return iSettingItems->OfferKeyEventL( aKeyEvent, aType );
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Responds to changes to the size and position of the contents of this
// control.
// ---------------------------------------------------------------------------
//
void CMPXAudioEffectsViewContainer::SizeChanged()
    {
    if ( iSettingItems )
        {
        iSettingItems->SetRect( Rect() );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXAudioEffectsViewContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
        }
    }

// -----------------------------------------------------------------------------
// From CCoeControl
// Gets Help
// -----------------------------------------------------------------------------
//
void CMPXAudioEffectsViewContainer::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KAppUidMusicPlayerX;
    aContext.iContext = KMusHlpSoundCheckView;
    }

// End of File
