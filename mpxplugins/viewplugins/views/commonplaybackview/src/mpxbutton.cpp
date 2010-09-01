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
* Description:  Button class.
*
*/



// INCLUDE FILES
#include    <aknappui.h>
#include    <AknIconArray.h>
#include    <aknsoundsystem.h>  // CAknKeySoundSystem
#include    <AknsConstants.h>
#include    <aknconsts.h>
#include    <AknsUtils.h>
#include    <barsread.h>
#include    <gulicon.h>
#include    <coedef.h>
#include    <eiksvdef.h>
#include    <AknsDrawUtils.h>
#include    "mpxbutton.h"
#include    "mpxplaybackviewlayoutinterface.h"
#include    "mpxcommonplaybackview.hrh"
#include    "mpxlog.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXButton::CMPXButton
// -----------------------------------------------------------------------------
//
CMPXButton::CMPXButton(
    TMPXPbvButton aButtonId,
    MMPXButtonCmdObserver* aObserver,
    MMPXPlaybackViewLayout* aLayout )
    : iButtonId( aButtonId ),
      iCurrentStateIndex( KErrNotFound ),
      iObserver( aObserver ), 
      iVisible( ETrue ),
      iLayout( aLayout )
    {
    MPX_FUNC("CMPXButton::CMPXButton()");
    }

// -----------------------------------------------------------------------------
// CMPXButton::NewLC
// -----------------------------------------------------------------------------
//
CMPXButton* CMPXButton::NewLC(
    TMPXPbvButton aButtonId,
    MMPXButtonCmdObserver* aObserver,
    MMPXPlaybackViewLayout* aLayout )
    {
    MPX_FUNC("CMPXButton::NewLC()");
    CMPXButton* self = new (ELeave) CMPXButton(
        aButtonId,
        aObserver,
        aLayout );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXButton::NewL
// -----------------------------------------------------------------------------
//
CMPXButton* CMPXButton::NewL(
    TMPXPbvButton aButtonId,
    MMPXButtonCmdObserver* aObserver,
    MMPXPlaybackViewLayout* aLayout )
    {
    MPX_FUNC("CMPXButton::NewL()");
    CMPXButton* self = CMPXButton::NewLC(
        aButtonId,
        aObserver,
        aLayout );
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CMPXButton::~CMPXButton()
    {
    MPX_FUNC("CMPXButton::~CMPXButton()");
    FreeIcons();
    iButtonArray.ResetAndDestroy();

    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }
    }

// -----------------------------------------------------------------------------
// CMPXButton::ConstructL
// -----------------------------------------------------------------------------
//
void CMPXButton::ConstructL()
    {
    MPX_FUNC("CMPXButton::ConstructL()");
    iKeySoundSystem = static_cast<CAknAppUi*>(
        CEikonEnv::Static()->AppUi())->KeySounds();

    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }


// -----------------------------------------------------------------------------
// CMPXButton::AddStateL
// -----------------------------------------------------------------------------
//
void CMPXButton::AddStateL(
    TInt aState,
    TInt aEnterCommand,
    TInt aExitCommand,
    TInt aLongEnterCommand,
    TInt aLongExitCommand )
    {
    MPX_DEBUG1("CMPXButton::AddStateL() entering");
    TMPXButtonInfo* info = new (ELeave) TMPXButtonInfo;
    CleanupStack::PushL( info );
    info->iState = aState;
    info->iEnterCommand = aEnterCommand;
    info->iExitCommand = aExitCommand;
    info->iLongEnterCommand = aLongEnterCommand;
    info->iLongExitCommand = aLongExitCommand;
    info->iBitmap = LoadIconL( aState );
    iButtonArray.AppendL(info);
    CleanupStack::Pop( info );
    MPX_DEBUG1("CMPXButton::AddStateL() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXButton::Id
// -----------------------------------------------------------------------------
//
TMPXPbvButton CMPXButton::Id() const
    {
    return iButtonId;
    }

// -----------------------------------------------------------------------------
// CMPXButton::MakeVisible
// -----------------------------------------------------------------------------
//
void CMPXButton::MakeVisible(
    TBool aVisible )
    {
    iVisible = aVisible;
    }

// -----------------------------------------------------------------------------
// CMPXButton::IsVisible
// -----------------------------------------------------------------------------
//
TBool CMPXButton::IsVisible() const
    {
    return iVisible;
    }

// -----------------------------------------------------------------------------
// CMPXButton::SetButtonState
// -----------------------------------------------------------------------------
//
void CMPXButton::SetButtonState(
    TInt aState,
    TBool aNotifyExit /*=EFalse=*/,
    TBool aNotifyEnter /*=EFalse*/ )
    {
    MPX_DEBUG4("CMPXButton::SetButtonState(%d, %d, %d): entering", aState, aNotifyExit, aNotifyEnter);
    
    TInt index( KErrNotFound );
    TMPXButtonInfo* info( NULL );
    
    // Find the new state
    for (TInt i = 0; i < iButtonArray.Count(); i++)
        {
        info = iButtonArray[i];
        if ( info->iState == aState )
            {
            index = i;
            }
        }
    
    if ( KErrNotFound != index &&
         iCurrentStateIndex != index )
        {
        if ( aNotifyExit || aNotifyEnter )
            {
            if ( aNotifyExit )
                {
                // Execute the previous state's exit command
                info = iButtonArray[iCurrentStateIndex];
                if ( iIsLongPress )
                    {
                    if ( info->iLongExitCommand != KErrNone )
                        {
                        TRAP_IGNORE(
                            iObserver->HandleButtonCommandL( info->iLongExitCommand ));
                        }
                    }
                else if ( info->iExitCommand != KErrNone )
                    {
                    TRAP_IGNORE(
                        iObserver->HandleButtonCommandL( info->iExitCommand ));
                    }
                // Reset long press flag
                iIsLongPress = EFalse;
                }
                
            info = iButtonArray[index];
            if ( aNotifyEnter )
                {
                // Now execute the new state's enter command
                if ( info->iEnterCommand != KErrNone )
                    {
                    TRAP_IGNORE( 
                        iObserver->HandleButtonCommandL( info->iEnterCommand ));
                    }
                }
                
            // start the timer
            iTimer->Cancel();
            if ( info->iLongEnterCommand != KErrNone )
                {
                iTimer->Start( KAknKeyboardRepeatInitialDelay,
                               KAknKeyboardRepeatInitialDelay,
                               TCallBack(TimerCallback, this ));
                }
            }

        iCurrentStateIndex = index;
        iNeedRedraw = ETrue;
        }
    MPX_DEBUG1("CMPXButton::SetButtonState(): exiting");
    }
    
// -----------------------------------------------------------------------------
// CMPXButton::CurrentState
// -----------------------------------------------------------------------------
//
TInt CMPXButton::CurrentState() const
    {
    MPX_DEBUG1("CMPXButton::CurrentState() entering");
    ASSERT( iCurrentStateIndex < iButtonArray.Count() );
    TInt state( iButtonArray[iCurrentStateIndex]->iState );
    MPX_DEBUG2("CMPXButton::CurrentState() exiting: %d", state);
    return state;
    }

// -----------------------------------------------------------------------------
// CMPXButton::RedrawRect
// -----------------------------------------------------------------------------
//
void CMPXButton::RedrawRect(
    CBitmapContext& aGc, 
    const TRect& aRect ) const
    {
    MPX_DEBUG1("CMPXButton::RedrawRect() entering");
    
    if (iVisible && aRect.Intersects(iRect))
        {
        if (iBackground)
            {
            MPX_DEBUG1("CMPXButton::RedrawRect() redrawing background");
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
            AknsDrawUtils::DrawBackground(
                skin, 
                iBackground, 
                NULL, 
                aGc, 
                iRect.iTl, 
                iRect, 
                KAknsDrawParamDefault );
            }
            
        MPX_DEBUG2("CMPXButton::RedrawRect(): iCurrentStateIndex = %d", iCurrentStateIndex);
        ASSERT( iCurrentStateIndex < iButtonArray.Count() );
        TMPXButtonInfo* info( iButtonArray[iCurrentStateIndex] );
        CGulIcon* icon( info->iBitmap );

        if ( icon )
            {
            ASSERT( icon->Bitmap() && icon->Mask() );
            MPX_DEBUG1("CMPXButton::RedrawRect() redrawing icon");
            aGc.BitBltMasked(
                iRect.iTl, // aPoint
                icon->Bitmap(), // aBitmap
                TRect(TPoint(), iRect.Size()), // aSourceRect
                icon->Mask(),
                EFalse); // aInvertMask
            }
        }
    MPX_DEBUG1("CMPXButton::RedrawRect() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXButton::ReloadIconsL
// -----------------------------------------------------------------------------
//
void CMPXButton::ReloadIconsL(
    const TRect& aParentRect )
    {
    MPX_DEBUG1("CMPXButton::ReloadIconsL() entering");
    iParentRect = aParentRect;

    FreeIcons();
    
    iRect = iLayout->ButtonLayout( aParentRect );
    for (TInt i = 0; i < iButtonArray.Count(); i++)
        {
        TMPXButtonInfo* info( iButtonArray[i] );
        info->iBitmap = LoadIconL( info->iState );
        AknIconUtils::SetSize(
            info->iBitmap->Bitmap(),
            iRect.Size() );
        }    
    MPX_DEBUG1("CMPXButton::ReloadIconsL() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXButton::SetBackground
// -----------------------------------------------------------------------------
//
void CMPXButton::SetBackground(
    MAknsControlContext* aBackground)
    {
    ASSERT( aBackground );
    iBackground = aBackground;
    }

// -----------------------------------------------------------------------------
// CMPXButton::NeedRedraw
// -----------------------------------------------------------------------------
//
TBool CMPXButton::NeedRedraw() const
    {
    return iNeedRedraw;
    }
    
// -----------------------------------------------------------------------------
// CMPXButton::ClearRedrawFlag
// -----------------------------------------------------------------------------
//
void CMPXButton::ClearRedrawFlag()
    {
    iNeedRedraw = EFalse;
    }
  
// -----------------------------------------------------------------------------
// CMPXButton::LoadIconL
// -----------------------------------------------------------------------------
//
CGulIcon* CMPXButton::LoadIconL(
    TInt aState )
    {
    MPX_DEBUG1("CMPXButton::LoadIconL() entering");
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    iLayout->GetButtonBitmapsL(
        iButtonId, 
        aState, 
        bitmap, 
        mask);
    CleanupStack::PushL(bitmap);
    CleanupStack::PushL(mask);

    CGulIcon* res = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(2); // bitmap, mask
    
    MPX_DEBUG1("CMPXButton::LoadIconL() exiting");
    return res;
    }

// -----------------------------------------------------------------------------
// CMPXButton::FreeIcons
// -----------------------------------------------------------------------------
//
void CMPXButton::FreeIcons()
    {
    MPX_DEBUG1("CMPXButton::FreeIcons() entering");
    for ( TInt i = 0; i < iButtonArray.Count(); i++ )
        {
        TMPXButtonInfo* info( iButtonArray[i] );
        delete info->iBitmap;
        info->iBitmap = NULL;
        }
    MPX_DEBUG1("CMPXButton::FreeIcons() exiting");
    }


// -----------------------------------------------------------------------------
// CMPXButton::HandleKeyRepeat
// -----------------------------------------------------------------------------
//
void CMPXButton::HandleLongPress()
    {
    MPX_DEBUG1("CMPXButton::HandleLongPress() entering");
    
    iIsLongPress = ETrue;
    
    TMPXButtonInfo* info( iButtonArray[iCurrentStateIndex] );
    if ( info->iLongEnterCommand != KErrNone )
        {
        if( !(info->iLongEnterCommand == EMPXPbvCmdIncreaseVolume || 
              info->iLongEnterCommand == EMPXPbvCmdDecreaseVolume ))
            {
            iTimer->Cancel();    
            }
        
        TRAP_IGNORE(iObserver->HandleButtonCommandL( info->iLongEnterCommand ));
        }
    
    MPX_DEBUG1("CMPXButton::HandleLongPress() exiting");
    }

// -----------------------------------------------------------------------------
// CMPXButton::TimerCallback
// -----------------------------------------------------------------------------
//
TInt CMPXButton::TimerCallback(
    TAny* aPtr )
    {
    MPX_DEBUG1("CMPXButton::TimerCallback() entering");
    static_cast<CMPXButton*>(aPtr)->HandleLongPress();
    MPX_DEBUG1("CMPXButton::TimerCallback() exiting");
    return KErrNone;
    }

//  End of File
