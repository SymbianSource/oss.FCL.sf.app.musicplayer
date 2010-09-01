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
* Description:  Implementation of CMPXLbxExtendedFeatures
*
*/


// INCLUDE FILES
#include <eiktxlbx.h>
#include <eiktxlbm.h>
#include <aknconsts.h>
#include <aknsoundsystem.h>
#include <aknViewAppUi.h>
#include <aknnavide.h>
#include <aknnavilabel.h>
#include <eikspane.h>
#include <aknappui.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileTones.h>
#include <TProfileToneSettings.h>
#include <AudioPreference.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxsubscription.h>
#include <mpxuser.h>
#include <mpxcommonui.rsg>
#include <mpxcollectionutility.h>
#include "mpxinternalcrkeys.h"
#include "mpxlbxextendedfeatures.h"
#include "mpxlog.h"

// CONSTANTS
const TInt KMPXDefaultKeyRepeatInitDelay = KAknKeyboardRepeatInitialDelay;
const TInt KMPMicroSecPerMilliSec = 1000;
const TInt KSoundInterval = 1000000;  // 1 second
const TInt KMaxVolumeLevel = 3;
const TInt KSpeedScrollSetIndexInterval = 5;

_LIT( KMPXCommonUiRscPath, "mpxcommonui.rsc" );
//_LIT8( KPhoneNormalBeepSequence, "\x00\x11\x0A\x71\x06\x0B" );
_LIT8( KPhoneFastBeepSequence, "\x00\x11\x0A\x76\x06\x0B" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::CMPXLbxExtendedFeatures
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXLbxExtendedFeatures::CMPXLbxExtendedFeatures(
    CEikTextListBox* aLbx,
    TBool aEnableSetIndex ) :
    iLbx( aLbx ),
    iKeyStatus( EMPXKeyOther ),
    iScrollingState( EMPXScrollingIdle ),
    iNaviEnabled( ETrue ),
    iKeysoundEnabled( ETrue ),
    iEnableSetIndex( aEnableSetIndex )
    {
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::ConstructL()
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::ConstructL" );

    CCoeEnv* coeEnv = CEikonEnv::Static();
    TParse parse;
    parse.Set( KMPXCommonUiRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    // connect to window server for scrolling speed setting
    iWsSession = CEikonEnv::Static()->WsSession();
    TUid naviPaneUid;
    naviPaneUid.iUid = EEikStatusPaneUidNavi;
    iNaviPane =
        static_cast<CAknNavigationControlContainer*>
        ( iAvkonViewAppUi->StatusPane()->ControlL( naviPaneUid ) );

    GetInfoFromCenRepL();

    iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
    //Create label to change text in Navi pane
    iNaviLabel = static_cast<CAknNaviLabel*>
            ( iNaviDecorator->DecoratedControl() );

    iTimer = CPeriodic::NewL( CActive::EPriorityIdle );

    iToneUtility = CMdaAudioToneUtility::NewL( *this, NULL );
    iToneUtility->SetPriority(
        KAudioPriorityKeyPress,
        static_cast<TMdaPriorityPreference>( KAudioPrefKeyPressNonDTMF ) );
    iToneUtility->SetRepeats( 1,
        TTimeIntervalMicroSeconds( KSoundInterval ) );

    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );

#ifdef __USE_MESSAGE_SUBSCRIPTION
    // Subscribe to only a few messages from collection utility
    CMPXSubscription* subscription( CMPXSubscription::NewL() );
    CleanupStack::PushL( subscription );
    CMPXSubscriptionItem* subItem1( CMPXSubscriptionItem::NewL() );
    CleanupStack::PushL( subItem1 );
    subItem1->SetTObjectValueL( KMPXMessageGeneralId, KMPXMessageGeneral );
    subItem1->SetTObjectValueL( KMPXMessageGeneralEvent, TMPXCollectionMessage::EPathChanged );
    subItem1->SetTObjectValueL( KMPXMessageGeneralType, EMcPathChangedByOpen );
    subscription->AddItemL( *subItem1 );
    iCollectionUtility->Collection().AddSubscriptionL( *subscription );
    CleanupStack::PopAndDestroy( subItem1 );
    CleanupStack::PopAndDestroy( subscription );
#endif
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXLbxExtendedFeatures* CMPXLbxExtendedFeatures::NewL(
    CEikTextListBox* aLbx, TBool aEnableSetIndex )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::NewL" );
    __ASSERT_DEBUG( aLbx, User::Panic( _L("NewL"), KErrArgument ) );

    CMPXLbxExtendedFeatures* self =
        new ( ELeave ) CMPXLbxExtendedFeatures( aLbx, aEnableSetIndex );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXLbxExtendedFeatures::~CMPXLbxExtendedFeatures()
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::~CMPXLbxExtendedFeatures" );
    delete iTimer;

    iWsSession.SetKeyboardRepeatRate(
        KAknKeyboardRepeatInitialDelay, KAknStandardKeyboardRepeatRate );
    iWsSession.Flush();

    if ( !iKeysoundEnabled )
        {
        // Enable keysound if it's not already enabled
        iAvkonViewAppUi->KeySounds()->PopContext();
        }

    if ( iNaviDecorator )
        {
        delete iNaviDecorator;
        }

    if ( iResourceOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResourceOffset );
        }

    if( iToneUtility )
        {
        if( iToneUtility->State() == EMdaAudioToneUtilityPlaying )
            {
            iToneUtility->CancelPlay();
            }
        else
            {
            iToneUtility->CancelPrepare();
            }
        delete iToneUtility;
        }

    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::HandleLbxKeyEventL
// Handle key events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CMPXLbxExtendedFeatures::HandleLbxKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::HandleLbxKeyEventL" );
    TKeyResponse keyResponse = EKeyWasNotConsumed;

    if ( iSpeedScrollEnabled )
        {
        //MPX_DEBUG2( "CMPXLbxExtendedFeatures::HandleLbxKeyEventL EventType: %d", aType );
        if ( aType == EEventKeyDown )
            {
            MPX_PERF_CHECKPT("Key down issued")
            // pass it to listbox to handle
            keyResponse = iLbx->OfferKeyEventL( aKeyEvent, aType );
            keyResponse = HandleEventKeyDownL( aKeyEvent );
            iFirstKeyEvent = ETrue;
            }
        else if ( aType == EEventKeyUp )
            {
            // pass it to listbox to handle
            keyResponse = iLbx->OfferKeyEventL( aKeyEvent, aType );
            keyResponse = HandleEventKeyUpL( aKeyEvent );
            }
        else if ( aType == EEventKey )
            {
            if ( ( aKeyEvent.iScanCode == EStdKeyLeftArrow ) ||
                ( aKeyEvent.iScanCode == EStdKeyRightArrow ) )
                {
                keyResponse = iLbx->OfferKeyEventL(
                              aKeyEvent, aType );
                }
            else if ( !CheckBoundaryCases( iKeyStatus ) )
                {
                if ( !iSkipEvent )
                    {
                    keyResponse = iLbx->OfferKeyEventL(
                        aKeyEvent, aType );

                    if ( ( ( aKeyEvent.iScanCode == EStdKeyUpArrow ) ||
                         ( aKeyEvent.iScanCode == EStdKeyDownArrow ) ) &&
                         iEnableSetIndex )
                        {
                        // Get current listbox item index
                        TInt currentItem = iLbx->CurrentItemIndex();
                        if ( currentItem != KErrNotFound )
                            {
                            SetIndexToCollectionL( currentItem );
                            }
                        }
                    }
                else
                    {
                    // Simulate the key event to activate the updated scrolling speed
                    iWsSession.SimulateKeyEvent(aKeyEvent);
                    keyResponse = EKeyWasConsumed;
                    }
                }
            else
                {
                // bump effect
                if ( iFirstKeyEvent )
                    {
                    iSkipEvent = ETrue;
                    keyResponse = iLbx->OfferKeyEventL(
                        aKeyEvent, aType );

                    if ( ( ( aKeyEvent.iScanCode == EStdKeyUpArrow ) ||
                         ( aKeyEvent.iScanCode == EStdKeyDownArrow ) ) &&
                         iEnableSetIndex )
                        {
                        TInt currentItem = iLbx->CurrentItemIndex();
                        if ( currentItem != KErrNotFound )
                            {
                            SetIndexToCollectionL( currentItem );
                            }
                        }

                    iKeyStatus = EMPXKeyDown;
                    if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
                        {
                        iKeyStatus = EMPXKeyUp;
                        }

                    // reset scrolling speed to normal
                    iScrollingState = EMPXScrollingIdle;
                    UpdateScrollingSpeed( iKeyStatus );
                    iScrollingState = EMPXScrolling1stBufferTime;

                    // after crossing the boundary, start timer again
                    StopTimer();
                    StartTimerL();
                    }
                EnableKeySoundL( EFalse );
                keyResponse = EKeyWasConsumed;
                MPX_PERF_CHECKPT("End of list reaches - bump effect");
                }
            iFirstKeyEvent = EFalse;
            iSkipEvent = EFalse;
            }
        else
            {
            // Other TEventCode; do nothing
            }
        } // end if iSpeedScrollEnabled
    else // iSpeedScrollEnabled is false
        {
        // pass it to listbox to handle
        keyResponse = iLbx->OfferKeyEventL( aKeyEvent, aType );

        // Get current listbox item index
        if ( ( ( aKeyEvent.iScanCode == EStdKeyUpArrow ) ||
             ( aKeyEvent.iScanCode == EStdKeyDownArrow ) ) &&
             iEnableSetIndex )
            {
            TInt currentItem = iLbx->CurrentItemIndex();
            if ( currentItem != KErrNotFound )
                {
                SetIndexToCollectionL( currentItem );
                }
            }
        }

    return keyResponse;
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::EnableSpeedScroll
// Enable/disable speed scroll
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXLbxExtendedFeatures::EnableSpeedScrollL( TBool aIsEnable )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::EnableSpeedScrollL" );
    iSpeedScrollEnabled = aIsEnable;

    if ( !aIsEnable )
        {
        Reset();
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::SpeedNaviUpdating
// Enable or disable speedscroll specific navi labelling
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXLbxExtendedFeatures::SpeedNaviUpdating( TBool aNaviEnable )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::SpeedNaviUpdating" );
    iNaviEnabled = aNaviEnable;
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::Reset
// Reset everything to default (eg. scrolling speed)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::Reset()
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::Reset" );
    StopTimer();

    // Reset all states to idle
    iKeyStatus = EMPXKeyOther;
    iScrollingState = EMPXScrollingIdle;

    // Revert to original speed
    UpdateScrollingSpeed( iKeyStatus );
    TRAP_IGNORE( EnableKeySoundL( ETrue ) );
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::EnableAutoLoop
// Enable/disable auto loop
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::EnableAutoLoop( TBool aIsEnable )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::EnableAutoLoop" );
    iAutoLoopEnabled = aIsEnable;
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::StartTimer
// Start the timer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::StartTimerL()
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::StartTimerL" );
    TInt bufferTime = 0;
    TBool enabled = EFalse;

    switch ( iScrollingState )
        {
        case EMPXScrolling1stBufferTime:
            {
            // Enable 1st buffer timer
            bufferTime = iFirstBufferTime;
            enabled = ETrue;
            break;
            }
        case EMPXScrolling2ndBufferTime:
            {
            // Enable 2nd buffer timer
            bufferTime = iSecondBufferTime;
            enabled = ETrue;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if ( enabled )
        {
        StopTimer();

        iTimer->Start( TTimeIntervalMicroSeconds32(
                            bufferTime * KMPMicroSecPerMilliSec ),
                       TTimeIntervalMicroSeconds32(
                            bufferTime * KMPMicroSecPerMilliSec ),
                       TCallBack( TimerCallBack, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::StopTimer
// Stop the timer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::StopTimer()
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::StopTimer" );
    if ( iTimer && iTimer->IsActive() )
        {
        iTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::TimerCallBack
// Callback function for the timer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXLbxExtendedFeatures::TimerCallBack( TAny* aPtr )
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::TimerCallBack" );
    CMPXLbxExtendedFeatures *p = ( CMPXLbxExtendedFeatures* )aPtr;
    p->BufferTimerHandler();
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::BufferTimerHandler
// Change scrolling status, called by TimerCallBack()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::BufferTimerHandler()
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::BufferTimerHandler" );
    StopTimer();
    switch ( iScrollingState )
        {
        case EMPXScrolling1stBufferTime:
            {
            iScrollingState = EMPXScrolling1stSpeed;
            // Goto first speed
            UpdateScrollingSpeed( iKeyStatus );
            TRAP_IGNORE( EnableKeySoundL( EFalse ) );
            break;
            }
        default:
            {
            // Reset all states if timeout
            Reset();
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::UpdateScrollingSpeed
// Update and return scrolling speed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::UpdateScrollingSpeed( TMPXKeyStatus aKeyStatus )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::UpdateScrollingSpeed" );
    TInt rate = 0;
    TInt repeatInitDelay = 0;
    TBool updated = EFalse;

    switch ( iScrollingState )
        {
        case EMPXScrolling1stSpeed:
            {
            MPX_DEBUG1( ">>>CMPXLbxExtendedFeatures::UpdateScrollingSpeed rate changed: first speed" );
            repeatInitDelay = iFirstSpeed * KMPMicroSecPerMilliSec;
            rate = iFirstSpeed * KMPMicroSecPerMilliSec;
            updated = ETrue;

            iSkipPlaying = EFalse;
            // TO-DO: timing problem with CMdaAudioToneUtility in S60 3.0,
            // enable this may crash, should be fixed in 3.1?
            iToneUtility->PrepareToPlayDesSequence( KPhoneFastBeepSequence() );
            break;
            }
        case EMPXScrolling2ndSpeed:
            {
            MPX_DEBUG1( ">>>CMPXLbxExtendedFeatures::UpdateScrollingSpeed rate changed: second speed" );
            repeatInitDelay = iSecondSpeed * KMPMicroSecPerMilliSec;
            rate = iSecondSpeed * KMPMicroSecPerMilliSec;
            updated = ETrue;

            iSkipPlaying = EFalse;
            // TO-DO: timing problem with CMdaAudioToneUtility in S60 3.0,
            // enable this may crash, should be fixed in 3.1?
            iToneUtility->CancelPrepare();
            iToneUtility->PrepareToPlayDesSequence( KPhoneFastBeepSequence() );
            break;
            }
        case EMPXScrollingIdle:
            {
            MPX_DEBUG1( ">>>CMPXLbxExtendedFeatures::UpdateScrollingSpeed rate changed: idle" );
            repeatInitDelay = KMPXDefaultKeyRepeatInitDelay;
            rate = iFirstSpeed * KMPMicroSecPerMilliSec;
            updated = ETrue;

            if( iToneUtility->State() == EMdaAudioToneUtilityPlaying )
                {
                MPX_DEBUG1( ">>>CMPXLbxExtendedFeatures::UpdateScrollingSpeed rate changed: idle cancel play" );
                iSkipPlaying = EFalse;
                iToneUtility->CancelPlay();
                }

            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if ( updated )
        {
        if( !CheckBoundaryCases( aKeyStatus ) )
            {
            TRAP_IGNORE( SetNaviSpeedTextL( EFalse ) );
            }
        // Change scrolling speed
        iWsSession.SetKeyboardRepeatRate( repeatInitDelay, rate );
        iWsSession.Flush();
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::CheckBoundaryCases
// Check whether boundary is reached
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMPXLbxExtendedFeatures::CheckBoundaryCases(
    TMPXKeyStatus aKeyStatus )
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::CheckBoundaryCases" );
    __ASSERT_DEBUG( iLbx, User::Panic( _L("CheckBoundaryCases"), KErrArgument ) );

    TBool reached = EFalse;

    if ( !iAutoLoopEnabled )
        {
        // Get total number of items in the listbox
        TInt totalCount = iLbx->Model()->NumberOfItems();

        // Get current listbox item index
        TInt currentItem = iLbx->CurrentItemIndex();

        //CHECK BOUNDARIES
        if ( aKeyStatus == EMPXKeyUp && currentItem == 0 )
            {
            reached = ETrue;
            }
        if ( aKeyStatus == EMPXKeyDown && ( currentItem == totalCount - 1 ) )
            {
            reached = ETrue;
            }
        }

    if( reached )
        {
        // boundary reached, stop all keytones
        if( iToneUtility->State() == EMdaAudioToneUtilityPlaying )
            {
            MPX_DEBUG1( ">>>CMPXLbxExtendedFeatures::CheckBoundaryCases cancel play" );
            iSkipPlaying = ETrue;
            iToneUtility->CancelPlay();
            }

        TRAP_IGNORE( SetNaviSpeedTextL( reached ) );
        }

    return reached;
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::GetInfoFromCenRepL
// Get info (eg. scrolling speed) from the cenrep
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::GetInfoFromCenRepL()
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::GetInfoFromCenRepL" );
    CRepository* repository = CRepository::NewLC( KCRUidMPXMPExtendedFeatures );

    User::LeaveIfError( repository->Get( KMPXMPSpeedScrollFirBuffTime,
        iFirstBufferTime ) );
    User::LeaveIfError( repository->Get( KMPXMPSpeedScrollSecBuffTime,
        iSecondBufferTime ) );
    User::LeaveIfError( repository->Get( KMPXMPSpeedScrollFirSpeed,
        iFirstSpeed ) );
    User::LeaveIfError( repository->Get( KMPXMPSpeedScrollSecSpeed,
        iSecondSpeed ) );

    CleanupStack::PopAndDestroy(); // repository
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::SetNaviSpeedTextL
// Update navi pane label for speed scrolling.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::SetNaviSpeedTextL( TBool boundaryReached )
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::SetNaviSpeedTextL" );
    if ( iNaviEnabled )
        {
        if( iScrollingState == EMPXScrolling2ndSpeed
            && !boundaryReached )
            {
            HBufC* speedFast =
                StringLoader::LoadLC( R_MPX_CUI_SCROLL_SPEED_FAST );
            iNaviLabel->SetTextL( *speedFast );
            CleanupStack::PopAndDestroy();

            iNaviPane->PushL( *iNaviDecorator );
            iNaviPane->DrawNow();
            }
        // Boundary or idle
        else
            {
            iNaviPane->Pop( iNaviDecorator );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::HandleEventKeyDownL
// Handles key down event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXLbxExtendedFeatures::HandleEventKeyDownL(
    const TKeyEvent& aKeyEvent )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::HandleEventKeyDown" );

    TKeyResponse keyResponse = EKeyWasNotConsumed;
    // Stop the timer
    StopTimer();
    iSkipPlaying = EFalse;

    // if not up or down arrow key, don't start timer
    if ( ( aKeyEvent.iScanCode == EStdKeyUpArrow ) ||
        ( aKeyEvent.iScanCode == EStdKeyDownArrow ) )
        {
        keyResponse = EKeyWasConsumed;

        // current key event
        // used to compare with previous key event
        TMPXKeyStatus currentKeyStatus = EMPXKeyDown;

        if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
            {
            currentKeyStatus = EMPXKeyUp;
            }

        if ( CheckBoundaryCases( currentKeyStatus ) )
            {
            EnableKeySoundL( EFalse );
            }
        else
            {
            EnableKeySoundL( ETrue );
            }

        if ( iKeyStatus == currentKeyStatus )
            {
            switch ( iScrollingState )
                {
                case EMPXScrolling2ndBufferTime:
                    {
                    iScrollingState = EMPXScrolling2ndSpeed;
                    iIndexCount = 0; // reset counter
                    // Goto second speed
                    UpdateScrollingSpeed( iKeyStatus );
                    EnableKeySoundL( EFalse );
                    break;
                    }
                default:
                    {
                    // Do nothing
                    break;
                    }
                }
            }
        else // if current key is opposite from previous
            {
            switch ( iScrollingState )
                {
                case EMPXScrolling1stSpeed:
                case EMPXScrolling2ndBufferTime:
                case EMPXScrolling2ndSpeed:
                    {
                    // Need to activate the updated scrolling speed
                    iSkipEvent = ETrue;
                    break;
                    }
                default:
                    {
                    break;
                    }
                }

            // Reset to idle state
            Reset();
            // Update key status
            iKeyStatus = currentKeyStatus;
            // Start the 1st buffer timer for next key
            iScrollingState = EMPXScrolling1stBufferTime;
            StartTimerL();
            }
        }
    else
        {
        // Reset everything to idle state
        Reset();
        }
    return keyResponse;
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::HandleEventKeyUpL
// Handles key down event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXLbxExtendedFeatures::HandleEventKeyUpL(
    const TKeyEvent& aKeyEvent )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::HandleEventKeyUp" );
    TKeyResponse keyResponse = EKeyWasNotConsumed;
    iSkipPlaying = EFalse;

    // if not up or down arrow key, don't reset timer
    if ( ( aKeyEvent.iScanCode == EStdKeyUpArrow ) ||
        ( aKeyEvent.iScanCode == EStdKeyDownArrow ) )
        {
        switch ( iScrollingState )
            {
            case EMPXScrolling2ndSpeed:
                {
                // leaving speed scrolling, set index
                SetIndexToCollectionL( iLbx->CurrentItemIndex(), ETrue );
                } // fall through
            case EMPXScrolling1stSpeed:
                {
                // Enable 2nd buffer timer
                iScrollingState = EMPXScrolling2ndBufferTime;
                StartTimerL();
                EnableKeySoundL( ETrue );
                break;
                }
            default:
                {
                // Reset all states for other scrolling states
                Reset();
                break;
                }
            }
        keyResponse = EKeyWasConsumed;
        }
    return keyResponse;
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::EnableKeySoundL
// Enables/Disables Keysound
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::EnableKeySoundL( TBool aEnable )
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::EnableKeySoundL" );
    //MPX_DEBUG2( "CMPXLbxExtendedFeatures::EnableKeySoundL: aEnable: %d", aEnable );

    if ( aEnable )
        {
        if ( !iKeysoundEnabled )
            {
            MPX_DEBUG1( "CMPXLbxExtendedFeatures::EnableKeySoundL: aEnable: pop" );
            iKeysoundEnabled = ETrue;
            // Enable keysound if it's not already enabled
            iAvkonViewAppUi->KeySounds()->PopContext();
            MPX_DEBUG2( "CMPXLbxExtendedFeatures::EnableKeySoundL: aEnable: %d", aEnable );
            }
        }
    else
        {
        if ( iKeysoundEnabled )
            {
            MPX_DEBUG1( "CMPXLbxExtendedFeatures::EnableKeySoundL: aEnable: push" );
            iKeysoundEnabled = EFalse;
            // Disable keysound if it's enabled
            iAvkonViewAppUi->KeySounds()->PushContextL(
                R_MPX_CUI_UP_DOWN_KEYS_SILENT );
            MPX_DEBUG2( "CMPXLbxExtendedFeatures::EnableKeySoundL: aEnable: %d", aEnable );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::SetIndexToCollectionL
// Send a select index command to collection async
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::SetIndexToCollectionL(
    TInt aIndex, TBool aForceSet )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::SetIndexToCollectionL" );
    // In case we are speed scrolling, we may flood w/ many unfinished selects
    //
    TBool okToSet( ETrue );
    if ( !aForceSet && iScrollingState == EMPXScrolling2ndSpeed )
        {
        iIndexCount++;
        if ( iIndexCount < KSpeedScrollSetIndexInterval )
            {
            okToSet = EFalse;
            }
        else
            {
            iIndexCount = 0;
            }
        }
    if ( okToSet )
        {
        MPX_DEBUG1( "CMPXLbxExtendedFeatures::SetIndexToCollectionL ok to set" );
        CMPXCommand* command = CMPXCommand::NewL();
        CleanupStack::PushL( command );
        command->SetTObjectValueL<TMPXCommandId>( KMPXCommandGeneralId,
                                                  KMPXCommandIdCollectionSelect );
        command->SetTObjectValueL( KMPXCommandCollectionSelectIndex, aIndex );

        iCollectionUtility->Collection().CommandL( *command );
        CleanupStack::PopAndDestroy( command );
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::DoHandleCollectionMessageL
// -----------------------------------------------------------------------------
void CMPXLbxExtendedFeatures::DoHandleCollectionMessageL(
    CMPXMessage* aMessage,
    TInt /*aError*/ )
    {
    MPX_FUNC("CMPXLbxExtendedFeatures::DoHandleCollectionMessageL()");

    TMPXMessageId id( aMessage->ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage->ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage->ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        if ( event == TMPXCollectionMessage::EPathChanged &&
             type == EMcPathChangedByOpen )
            {
            iCollectionUtility->Collection().CancelRequest();
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::MatoPrepareComplete
//
// Once tone initialization is complete, set tone settings and start playing
// the tone.
//
// There's no fallback mechanism for tone player
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::MatoPrepareComplete( TInt aError )
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::MatoPrepareComplete" );
    MPX_DEBUG2( "CMPXLbxExtendedFeatures::MatoPrepareComplete START: aError = %d", aError );

    MProfileEngine* profileEngine = NULL;
    MProfile* profile = NULL;
    TInt profileVolume = 0;

    MPX_TRAPD( err, profileEngine = CreateProfileEngineL() );
    if( err )
        {
        profileEngine = NULL;
        }

    if( profileEngine )
        {
        TRAP_IGNORE( profile = profileEngine->ActiveProfileL() );

        if( profile )
            {
            const TProfileToneSettings& setting =
                profile->ProfileTones().ToneSettings();
            profileVolume = ( setting.iRingingType == EProfileRingingTypeSilent ?
                0 : setting.iKeypadVolume );
            profile->Release();
            }
        profileEngine->Release();
        }

    TInt toneVolume = iToneUtility->MaxVolume() *
        profileVolume / KMaxVolumeLevel;
    iToneUtility->SetVolume( toneVolume );

    MPX_DEBUG1( "CMPXLbxExtendedFeatures::MatoPrepareComplete start playing" );
    if ( KErrNone == aError )
        {
        iSkipPlaying = EFalse;
        iToneUtility->Play();
        }
    }

// -----------------------------------------------------------------------------
// CMPXLbxExtendedFeatures::MatoPlayComplete
// -----------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::MatoPlayComplete( TInt aError )
    {
    //MPX_FUNC( "CMPXLbxExtendedFeatures::MatoPlayComplete" );
    MPX_DEBUG4( "CMPXLbxExtendedFeatures::MatoPlayComplete START: iScrollingState = %d, iSkipPlaying = %d, aError = %d",
        iScrollingState, iSkipPlaying, aError);
    if ( ( KErrNone == aError ) && ( !iSkipPlaying ) )
        {
        iToneUtility->Play();
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handle collection message.
// ---------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::HandleCollectionMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::HandleCollectionMessage" );
    TRAP_IGNORE( DoHandleCollectionMessageL( aMessage, aError ));
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d a folder
// ---------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::HandleOpenL(
    const CMPXMedia& /*aEntries*/,
    TInt /*aIndex*/,
    TBool /*aComplete*/,
    TInt /*aError*/ )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::HandleOpenL" );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d an item. Client typically responds by
// 'playing' the item
// ---------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::HandleOpenL 2" );
    // Do Nothing: playback/fetch client should handle this stage
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle media properties
// ---------------------------------------------------------------------------
//
void CMPXLbxExtendedFeatures::HandleCollectionMediaL(
    const CMPXMedia& /*aMedia*/,
    TInt /*aError*/)
    {
    MPX_FUNC( "CMPXLbxExtendedFeatures::HandleCollectionMediaL" );
    }

// End of File
