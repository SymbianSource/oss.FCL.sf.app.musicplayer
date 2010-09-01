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
* Description:  Button state manager for playback view
*
*/


// INCLUDE FILES
#include <aknbutton.h>
#include <mpxcommonplaybackview.rsg>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <AknsDrawUtils.h>
#include <barsread.h>
#include <AknDef.h>
#include <featmgr.h>
#include <aknconsts.h>
#include "mpxbuttonmanager.h"
#include "mpxplaybackviewlayoutinterface.h"
#include "mpxcommonplaybackview.hrh"
#include "mpxlog.h"

// CONSTANTS


struct TStateAndCommandIds
    {
    TInt iState;
    TInt iEnterCommand;
    TInt iExitCommand;
    TInt iLongEnterCommand;
    TInt iLongExitCommand;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXButtonManager::CMPXButtonManager
// -----------------------------------------------------------------------------
//
CMPXButtonManager::CMPXButtonManager(
    MMPXButtonCmdObserver* aObserver,
    MMPXPlaybackViewLayout* aLayout )
    : iObserver(aObserver),
      iLayout(aLayout),
      iEnabled(ETrue)
    {
    MPX_FUNC("CMPXButtonManager::CMPXButtonManager()");
    }

// -----------------------------------------------------------------------------
// CMPXButtonManager::NewL
// -----------------------------------------------------------------------------
//
CMPXButtonManager* CMPXButtonManager::NewL(
    MMPXButtonCmdObserver* aObserver,
    MMPXPlaybackViewLayout* aLayout,
    const CCoeControl &aContainer)
    {
    MPX_FUNC("CMPXButtonManager::NewL()");
    CMPXButtonManager* self = new (ELeave) CMPXButtonManager(
        aObserver,
        aLayout);

    CleanupStack::PushL(self);
    //self->ConstructL(aRect, aContainer);
    self->ConstructL(aContainer);
    CleanupStack::Pop();
    return self;
    }
// -----------------------------------------------------------------------------
// CMPXButtonManager::~CMPXButtonManager
// -----------------------------------------------------------------------------
//
CMPXButtonManager::~CMPXButtonManager()
    {
    MPX_FUNC("CMPXButtonManager::~CMPXButtonManager()");
//    iButtons.ResetAndDestroy();
    iAvkonButtons.ResetAndDestroy();
    if (iLongKeyTimer)
        {
        iLongKeyTimer->Cancel();
        delete iLongKeyTimer;
        }
    }

// -----------------------------------------------------------------------------
// CMPXButtonManager::ConstructL (overloaded for avkon button rocker)
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::ConstructL(const CCoeControl &aContainer)
    {
    MPX_FUNC("CMPXButtonManager::ConstructL()");
    iIsLongPress = EFalse;
    iVolumeInRocker = (!FeatureManager::FeatureSupported( KFeatureIdSideVolumeKeys ));

    if( AknLayoutUtils::PenEnabled() )
        {
            iIsTouchUi = ETrue;
        }
    else
        {
            iIsTouchUi = EFalse;
        }

    if (!iVolumeInRocker)
        {
            iStopInRocker = ETrue;
        }
    else
        {
            iStopInRocker = EFalse;
        }

    if( !iIsTouchUi)
        {
        CreateRockerAvkonButtonsL(aContainer);
        }

    ActivateL();

    iLongKeyTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    }

// -----------------------------------------------------------------------------
// CMPXButtonManager::UpdateButtonStates
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::UpdateButtonStates( TMPXPlaybackState aState )
    {
    MPX_DEBUG2("CMPXButtonManager::UpdateButtonStates(%d) entering", aState);
    if( !iIsTouchUi && !iIsSeeking )
        {
        if (aState == EPbStatePlaying)
            {
            // Playing
            if (iAvkonButtons[EButtonPlay]->IsVisible())
                {
                iAvkonButtons[EButtonPlay]->MakeVisible(EFalse);
                }
            if (!iAvkonButtons[EButtonPause]->IsVisible())
                {
                iAvkonButtons[EButtonPause]->MakeVisible(ETrue);
                }
            }
        else
            {
            // Not playing
            if (!iAvkonButtons[EButtonPlay]->IsVisible())
                {
                iAvkonButtons[EButtonPlay]->MakeVisible(ETrue);
                }
            if (iAvkonButtons[EButtonPause]->IsVisible())
                {
                iAvkonButtons[EButtonPause]->MakeVisible(EFalse);
                }
            }

        if (iAvkonButtons[EButtonPlay]->IsVisible())
            {
            if (aState == EPbStateNotInitialised &&
                !iAvkonButtons[EButtonPlay]->IsDimmed())
                {
                iAvkonButtons[EButtonPlay]->SetDimmed(ETrue);
                }
            if (aState != EPbStateNotInitialised &&
                iAvkonButtons[EButtonPlay]->IsDimmed())
                {
                iAvkonButtons[EButtonPlay]->SetDimmed(EFalse);
                }
            }

        for (TInt i = 0; i< iRockerAvkonButtonCount; i++)
            {
            iAvkonButtons[i]->SetCurrentState(0, ETrue);
            }
        }

    MPX_DEBUG1("CMPXButtonManager::UpdateButtonStates() exiting");
    }


// -----------------------------------------------------------------------------
// CMPXButtonManager::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXButtonManager::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG3("CMPXButtonManager::OfferKeyEventL(%d, %d) entering", aKeyEvent.iScanCode, aType);

    TKeyResponse response( EKeyWasNotConsumed );
    response = SetAvkonButtonStateL(aKeyEvent, aType);
    MPX_DEBUG2("CMPXButtonManager::OfferKeyEventL() exiting %d", response);
    return response;
    }

// -----------------------------------------------------------------------------
// CMPXButtonManager::SetParentRect
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::SetParentRect(
    const TRect& aRect )
    {
    iParentRect = aRect;
    }

// -----------------------------------------------------------------------------
// CMPXButtonManager::SetEnabled
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::SetEnabled( TBool aEnabled )
    {
    MPX_DEBUG2("CMPXButtonManager::SetEnabled(%d) entering", aEnabled);
    iEnabled = aEnabled;
    MPX_DEBUG1("CMPXButtonManager::SetEnabled() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXButtonManager::HandleForegroundLostL
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::HandleForegroundLostL()
    {
    MPX_FUNC( "CMPXButtonManager::HandleForegroundLostL" );
    iIsSeeking = EFalse;

 	if (iIsLongPress)
		{
		iIsLongPress = EFalse;
		iObserver->HandleButtonCommandL(EMPXPbvCmdEndSeek);
		}

	if(iLongKeyTimer)
		{
		iLongKeyTimer->Cancel();
		}
    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::CreateRockerAvkonButtonsL
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::CreateRockerAvkonButtonsL(const CCoeControl &aContainer)
    {
    TResourceReader reader;
    iRockerAvkonButtonCount = 0;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, R_MPX_PBV_ROCKER_AVKON_BUTTONS );
    TInt numButtons( reader.ReadInt16() );

    CCoeControl::SetContainerWindowL(aContainer);

    for (TInt i = 0; i < numButtons; i++ )
        {
        CAknButton* aknButton = CAknButton::NewL();
        CleanupStack::PushL( aknButton );
        aknButton->SetContainerWindowL(*this);
        aknButton->SetParent( this );
        aknButton->ConstructFromResourceL( reader );
        aknButton->SetObserver( this );
        aknButton->SetLongPressInterval(1000);//KAknKeyboardRepeatInitialDelay / 1000);
        aknButton->SetKeyRepeatInterval(0, KAknKeyboardRepeatInitialDelay / 1000);
        aknButton->ActivateL();
        iAvkonButtons.AppendL( aknButton );
        CleanupStack::Pop( aknButton );
        iRockerAvkonButtonCount++;
        }

    CleanupStack::PopAndDestroy(); // reader;

    TResourceReader reader2;
    if (iVolumeInRocker)
        {
        CCoeEnv::Static()->CreateResourceReaderLC( reader2, R_MPX_PBV_ROCKER_VOLUME_AVKON_BUTTONS );
        numButtons = reader2.ReadInt16();
        for (TInt i = 0; i < numButtons; i++ )
            {
            CAknButton* aknButton = CAknButton::NewL();
            CleanupStack::PushL( aknButton );
            aknButton->SetContainerWindowL( *this );
            aknButton->SetParent( this );
            aknButton->ConstructFromResourceL( reader2 );
            aknButton->SetObserver( this );
            aknButton->SetLongPressInterval(1000);//KAknKeyboardRepeatInitialDelay / 1000);
            aknButton->SetKeyRepeatInterval(0, KAknKeyboardRepeatInitialDelay / 1000);
            aknButton->ActivateL();
            iAvkonButtons.AppendL( aknButton );
            CleanupStack::Pop( aknButton );
            iRockerAvkonButtonCount++;
            }
        }
    else
        {
        CCoeEnv::Static()->CreateResourceReaderLC( reader2, R_MPX_PBV_ROCKER_STOP_AVKON_BUTTON );
        CAknButton* aknButton = CAknButton::NewL();
        CleanupStack::PushL( aknButton );
        aknButton->SetContainerWindowL( *this );
        aknButton->SetParent( this );
        aknButton->ConstructFromResourceL( reader2 );
        aknButton->SetObserver( this );
        aknButton->SetLongPressInterval(1000);//KAknKeyboardRepeatInitialDelay / 1000);
        aknButton->SetKeyRepeatInterval(0, KAknKeyboardRepeatInitialDelay / 1000);
        aknButton->ActivateL();
        iAvkonButtons.AppendL( aknButton );
        CleanupStack::Pop( aknButton );
        iRockerAvkonButtonCount++;
        }

    CleanupStack::PopAndDestroy(); // reader2;

    }

// -----------------------------------------------------------------------------
// CMPXCommonPlaybackViewContainer::SetAvkonButtonState
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXButtonManager::SetAvkonButtonStateL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {

    TKeyResponse response = EKeyWasNotConsumed;

    if (aKeyEvent.iScanCode == EStdKeyLeftArrow || aKeyEvent.iCode == EKeyLeftArrow)
        {    
        MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL LeftArrow" );
        if (aType == EEventKeyDown)
            {
            if (!iIsTouchUi)
                {
                iAvkonButtons[EButtonRewind]->SetCurrentState(1, ETrue); 
                }
            iLongKeyTimer->Cancel();
            iLongKeyTimer->Start(KAknKeyboardRepeatInitialDelay,
                                 KAknKeyboardRepeatInitialDelay,
                                 TCallBack(TimerCallback, this));
            iLongPressCommandId = EMPXPbvCmdSeekBackward;
            iIsSeeking = ETrue;
            }
        else if (aType == EEventKeyUp)
            {
            if (!iIsTouchUi)
                {
                iAvkonButtons[EButtonRewind]->SetCurrentState(0, ETrue);
                }
            iLongKeyTimer->Cancel();
            if (iIsLongPress)
               {
               MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL LeftArrow long press" );
               iIsLongPress = EFalse;
               iObserver->HandleButtonCommandL(EMPXPbvCmdEndSeek);
               }
            else
               {
               MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL LeftArrow short press" );
               iObserver->HandleButtonCommandL(EMPXPbvCmdPreviousListItem);
               }
            iIsLongPress = EFalse;
            response = EKeyWasConsumed;
            iIsSeeking = EFalse;
            }
        }
    else if (aKeyEvent.iScanCode == EStdKeyRightArrow || aKeyEvent.iCode == EKeyRightArrow)
        {
        if (aType == EEventKeyDown)
            {
	    	    if (!iIsTouchUi)
	    	        {
                iAvkonButtons[EButtonFastForward]->SetCurrentState(1, ETrue);
                }
            iLongKeyTimer->Cancel();
            iLongKeyTimer->Start(KAknKeyboardRepeatInitialDelay,
                                 KAknKeyboardRepeatInitialDelay,
                                 TCallBack(TimerCallback, this));
            iLongPressCommandId = EMPXPbvCmdSeekForward;
            iIsSeeking = ETrue;
            }
            else if (aType == EEventKeyUp)
                {
                if (!iIsTouchUi)
                    {
                    iAvkonButtons[EButtonFastForward]->SetCurrentState(0, ETrue);    
                    }
                iLongKeyTimer->Cancel();
                if (iIsLongPress)
                    {
                    MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL RightArrow long press" );
                    iIsLongPress = EFalse;
                    iObserver->HandleButtonCommandL(EMPXPbvCmdEndSeek);
                    }
                else
                    {
				            MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL RightArrow short press" );
                   iObserver->HandleButtonCommandL(EMPXPbvCmdNextListItem);
                   }
                iIsLongPress = EFalse;
                iIsSeeking = EFalse;
                response = EKeyWasConsumed;
                }
        }
    else if (aKeyEvent.iScanCode == EStdKeyDevice3 || aKeyEvent.iCode == EKeyDevice3
            || aKeyEvent.iScanCode == EStdKeyEnter || aKeyEvent.iCode == EKeyEnter)
   		{
   		MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL EStdKeyDevice3 or EStdKeyEnter pressed" );
        if (!iIsTouchUi)
            {
            if (aType == EEventKeyDown)
                {
                iKeyDownEventReceived = ETrue;
    
                if (iAvkonButtons[EButtonPause]->IsVisible())
                    {
                    iAvkonButtons[EButtonPause]->SetCurrentState(1, ETrue);
                    }
                else
                    {
                    iAvkonButtons[EButtonPlay]->SetCurrentState(1, ETrue);
                    }
                }
            else if (aType == EEventKeyUp)
                {
                if ( iKeyDownEventReceived )
                    {
                    if (iAvkonButtons[EButtonPause]->IsVisible())
                        {
                        iAvkonButtons[EButtonPause]->SetCurrentState(0, ETrue);
                        iObserver->HandleButtonCommandL(EMPXPbvCmdPause);
                        response = EKeyWasConsumed;
                        }
                    else
                        {
                        iAvkonButtons[EButtonPlay]->SetCurrentState(0, ETrue);
                        iObserver->HandleButtonCommandL(EMPXPbvCmdPlay);
                        response = EKeyWasConsumed;
                        }
                    }
                iKeyDownEventReceived= EFalse;
                }
            }
        else  //iIsTouchUi
            {
            if (aType == EEventKeyDown)
                {
                iKeyDownEventReceived = ETrue;
                }
            else if (aType == EEventKeyUp)
                {
                if ( iKeyDownEventReceived )
                    {
                    iObserver->HandleButtonCommandL(EMPXPbvCmdPlayPause );
                    response = EKeyWasConsumed;
                    }
                iKeyDownEventReceived = EFalse;
                }
            }	
   		}
    else if (aKeyEvent.iScanCode == EStdKeyDownArrow || aKeyEvent.iCode == EKeyDownArrow)
        {
        MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL EStdKeyDownArrow pressed" );
        if (iVolumeInRocker || iStopInRocker)
           {	
           if (!iIsTouchUi)
               {
            if (aType == EEventKeyDown)
                {
                iAvkonButtons[EButtonVolumeDown]->SetCurrentState(1, ETrue);
                }
            else if (aType == EEventKeyUp)
                {
                iAvkonButtons[EButtonVolumeDown]->SetCurrentState(0, ETrue);
                if(iVolumeInRocker)
                    {
                    iObserver->HandleButtonCommandL(EMPXPbvCmdDecreaseVolume);
                    }
                else
                    {
                    iObserver->HandleButtonCommandL(EMPXPbvCmdStop);
                    }
                response = EKeyWasConsumed;
                    }
               }
		       else //iIsTouchUi
		           {
               if (iVolumeInRocker && aType == EEventKeyUp )
			             {            
                   iObserver->HandleButtonCommandL(EMPXPbvCmdDecreaseVolume);
                   }
               else if (iStopInRocker && aType == EEventKeyUp)
                   {                 		
                   iObserver->HandleButtonCommandL(EMPXPbvCmdStop);
                   }
               response = EKeyWasConsumed; 
               }
		        }
		    }
    else if (aKeyEvent.iScanCode == EStdKeyUpArrow || aKeyEvent.iCode == EKeyUpArrow)
        {
        MPX_DEBUG1( "CMPXButtonManager::SetAvkonButtonStateL EStdKeyUpArrow pressed" );
        if(iVolumeInRocker)
            {
	    	    if (!iIsTouchUi)
                {
                if (aType == EEventKeyDown)
                    {
                    iAvkonButtons[EButtonVolumeUp]->SetCurrentState(1, ETrue);
                    }
                else if (aType == EEventKeyUp)
                    {
                	  iAvkonButtons[EButtonVolumeUp]->SetCurrentState(0, ETrue);     		
                	  iObserver->HandleButtonCommandL(EMPXPbvCmdIncreaseVolume);
                	  response = EKeyWasConsumed;
                	  }
                }
            else //iIsTouchUi
                {
                if (aType == EEventKeyUp)
              	    {
                    iObserver->HandleButtonCommandL(EMPXPbvCmdIncreaseVolume);
                	  response = EKeyWasConsumed;   
                	  }	
                }
           }
        }
      return response;
    }

// -----------------------------------------------------------------------------
// CMPlayerPlaybackContainer::TimerCallback
// -----------------------------------------------------------------------------
//
TInt CMPXButtonManager::TimerCallback(TAny* aPtr)
    {
    //HandleLongPress();
    MPX_TRAPD( err, static_cast<CMPXButtonManager*>(aPtr)->HandleLongPressL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CMPlayerPlaybackContainer::HandleLongPress
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::HandleLongPressL()
    {
    iObserver->HandleButtonCommandL(iLongPressCommandId);
    iLongKeyTimer->Cancel();
    iIsLongPress = ETrue;
    }

// -----------------------------------------------------------------------------
// void CMPXCommonPlaybackViewContainer::HandleControlEventL
// ----------------------------------------------------------------------------
//
void CMPXButtonManager::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
    {

    if( AknLayoutUtils::PenEnabled() )
        {
        switch( aEventType )
            {
            case EEventStateChanged:
                {
                if (iVolumeInRocker)
                {
                if ( aControl == iAvkonButtons[EButtonVolumeUp] )
                    {
                    iAvkonButtons[EButtonVolumeUp]->SetCurrentState(0, ETrue);
                    iObserver->HandleButtonCommandL(EMPXPbvCmdIncreaseVolume);
                    }
                else if ( aControl == iAvkonButtons[EButtonVolumeDown] )
                    {
                    iAvkonButtons[EButtonVolumeDown]->SetCurrentState(0, ETrue);
                    iObserver->HandleButtonCommandL(EMPXPbvCmdDecreaseVolume);
                    }

                }

                break;
                }
            case CAknButton::ELongPressEvent:
                {
                if ( aControl == iAvkonButtons[EButtonFastForward] )
                    {
                    iIsLongPress = ETrue;
                    iAvkonButtons[EButtonFastForward]->SetCurrentState(0, ETrue);
                    iObserver->HandleButtonCommandL(EMPXPbvCmdSeekForward);
                    }
                else if ( aControl == iAvkonButtons[EButtonRewind] )
                    {
                    iIsLongPress = ETrue;
                    iAvkonButtons[EButtonRewind]->SetCurrentState(0, ETrue);
                    iObserver->HandleButtonCommandL(EMPXPbvCmdSeekBackward);
                    }
                break;
                }
            case MCoeControlObserver::EEventRequestExit:
                {
                if ( aControl == iAvkonButtons[EButtonPlay] )
                    {
                    iAvkonButtons[EButtonPlay]->SetCurrentState(0, ETrue);
                    iObserver->HandleButtonCommandL(EMPXPbvCmdPlay);
                    }
                else if ( aControl == iAvkonButtons[EButtonPause] )
                    {
                    iAvkonButtons[EButtonPause]->SetCurrentState(0, ETrue);
                    iObserver->HandleButtonCommandL(EMPXPbvCmdPause);
                    }
                else if ( aControl == iAvkonButtons[EButtonFastForward] )
                    {
                    if (iIsLongPress)
                        {
                        iIsLongPress = EFalse;
                        iAvkonButtons[EButtonFastForward]->SetCurrentState(0, ETrue);
                        iObserver->HandleButtonCommandL(EMPXPbvCmdEndSeek);
                        }
                    else
                        {
                        iAvkonButtons[EButtonFastForward]->SetCurrentState(0, ETrue);
                        iObserver->HandleButtonCommandL(EMPXPbvCmdNextListItem);
                        }
                    }
                else if ( aControl == iAvkonButtons[EButtonRewind] )
                    {
                    if (iIsLongPress)
                        {
                        iIsLongPress = EFalse;
                        iAvkonButtons[EButtonRewind]->SetCurrentState(0, ETrue);
                        iObserver->HandleButtonCommandL(EMPXPbvCmdEndSeek);
                        }
                    else
                        {
                        iAvkonButtons[EButtonRewind]->SetCurrentState(0, ETrue);
                        iObserver->HandleButtonCommandL(EMPXPbvCmdPreviousListItem);
                        }
                    }
                else if ( !iVolumeInRocker)
                    {
                    if(aControl == iAvkonButtons[EButtonVolumeDown])
                        {
                        iAvkonButtons[EButtonVolumeDown]->SetCurrentState(0, ETrue);
                        iObserver->HandleButtonCommandL(EMPXPbvCmdStop);
                        }
                    }
                break;
                }
            default:
                break;
            }
        }

    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// ---------------------------------------------------------------------------
//
TInt CMPXButtonManager::CountComponentControls() const
    {
    return iRockerAvkonButtonCount;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXButtonManager::ComponentControl( TInt aIndex ) const
    {
    ASSERT( aIndex < iRockerAvkonButtonCount );
    return iAvkonButtons[aIndex];
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXButtonManager::HandleResourceChange( TInt aType )
    {

    if ( aType == KAknsMessageSkinChange )
        {
        for (TInt i = 0; i< iRockerAvkonButtonCount; i++)
            {
            iAvkonButtons[i]->HandleResourceChange( aType );
            }

        }
    }
// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a size change
// ---------------------------------------------------------------------------
//
void CMPXButtonManager::SizeChanged()
    {
    iParentRect = Rect();
    TRect rockerRect = iLayout->ButtonLayout(iParentRect);
    TRect controlButtonRect;
    for (TInt i = 0; i< iRockerAvkonButtonCount; i++)
        {
        controlButtonRect = iLayout->ButtonLayout(rockerRect, i);
        iAvkonButtons[i]->SetRect( controlButtonRect );
        }
    }

void CMPXButtonManager::Draw(CWindowGc& /*aGc*/, const CCoeControl& /*aControl*/, const TRect& /*aRect*/) const
    {
	// Do Nothing
    }

// -----------------------------------------------------------------------------
// CMPXButtonManager::RestoreButtonStates
// -----------------------------------------------------------------------------
//
void CMPXButtonManager::RestoreButtonStates( TMPXPlaybackState aState )
    {
    MPX_DEBUG2("CMPXButtonManager::RestoreButtonStates(%d) entering", aState);    
    if( !iIsTouchUi)
        {
        if (aState == EPbStatePlaying)
            {
            // Playing
            if (!iAvkonButtons[EButtonFastForward]->IsDimmed())
                {
                iAvkonButtons[EButtonFastForward]->SetDimmed(ETrue);
                }
            if (!iAvkonButtons[EButtonRewind]->IsDimmed())
                {
                iAvkonButtons[EButtonRewind]->SetDimmed(ETrue);
                }
            }
        }
    MPX_DEBUG1("CMPXButtonManager::ResoreButtonStates() exiting");
    }
//  End of File
