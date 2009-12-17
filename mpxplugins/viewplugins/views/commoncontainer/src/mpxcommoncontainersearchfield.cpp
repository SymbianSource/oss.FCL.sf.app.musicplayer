/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that implements Search Field feature.
*
*/


// INCLUDE FILES
#include <eikmenub.h>
#include <eikdef.h>
#include <akninfrm.h>
#include <eikon.rsg>
#include <eikenv.h>
#include <txtglobl.h>
#include <gulfont.h>
#include <txtfrmat.h>
#include <eikgted.h>
#include <aknconsts.h>
#include <akneditstateindicator.h>
#include <aknslistboxbackgroundcontrolcontext.h>
#include <aknlayout2def.h>
#include <aknlayout2scalabledef.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <featmgr.h>
#include <fepbase.h>
#include <aknutils.h>
#include "mpxcommoncontainersearchfieldwrapper.h"
#include "mpxcommoncontainersearchfield.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMPXCommonContainerSearchField* CMPXCommonContainerSearchField::NewL(
    CMPXCommonContainerSearchFieldWrapper* aParent,
    TBool aFindKeywordMode )
    {
    CMPXCommonContainerSearchField* self = new (ELeave) CMPXCommonContainerSearchField( aFindKeywordMode );
    CleanupStack::PushL( self );

    self->ConstructL( aParent );
    self->MakeVisibleL( EFalse );
    self->SetFocus( EFalse );

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::MakeVisibleL( TBool aVisible )
    {
    if ( aVisible )
        {
        HandleFindSizeChanged();
        SetTextL( KNullDesC );
        }

    // if ( AknLayoutUtils::PenEnabled() )
        // {
        // Note, commented out, caused LSK and RSK problems.
        // SetPointerCapture( aVisible );
        // Note, commented out, activated Virtual KeyBoard right away.
        // if ( aVisible )
            // {
            // ActivateVkbL();
            // }
        // }

    CCoeControl::MakeVisible( aVisible );

    iSearchFieldKeyHandler->Reset();
    iSearchFieldKeyHandler->EnableL( aVisible );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::MakeVisible( TBool aVisible )
    {
    TRAP_IGNORE( MakeVisibleL( aVisible ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMPXCommonContainerSearchField::~CMPXCommonContainerSearchField()
    {
    delete iSearchFieldKeyHandler;
    delete iEditor;
    delete iInputFrame;
	delete iPrevKeyword;
	CCoeEnv::Static()->RemoveFepObserver(
	    static_cast<MCoeFepObserver &>( *this ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMPXCommonContainerSearchField::CMPXCommonContainerSearchField( TBool aFindKeywordMode )
 :  iFindKeywordMode( aFindKeywordMode ),
    iHandleFEPFind( ETrue )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::ConstructL( CMPXCommonContainerSearchFieldWrapper* aParent )
    {
    CreateWindowL();

    SetMopParent( aParent );
    iParent = aParent;

    iEditor = new (ELeave) CEikGlobalTextEditor;

    iInputFrame = CAknInputFrame::NewL( iEditor,
                                        EFalse,
                                        KAvkonBitmapFile,
                                        EMbmAvkonQgn_indi_find_glass,
                                        EMbmAvkonQgn_indi_find_glass_mask,
                                        CAknInputFrame::EPopupLayout );
    iInputFrame->SetContainerWindowL( *this );

    AknEditUtils::ConstructEditingL ( iEditor,
                                      40, // TODO
                                      1,
                                      EAknEditorCharactersLowerCase,
                                      EAknEditorAlignRight,
                                      EFalse,
                                      ETrue,
                                      EFalse );
    iEditor->SetContainerWindowL( *this );
    iEditor->SetObserver( this );
    iEditor->SetBorder( TGulBorder::ENone );
    iEditor->SetAknEditorCase( EAknEditorLowerCase );
    iEditor->SetAknEditorInputMode( EAknEditorTextInputMode );

    // Disallow chinese input.
    if ( FeatureManager::FeatureSupported( KFeatureIdAvkonApac ) )
        {
        iEditor->SetAknEditorAllowedInputModes( EAknEditorTextInputMode |
                                                EAknEditorNumericInputMode );
        }

    // Find keyword should use EAknEditorFlagDefault since in find
    // input we allow all types of input.
   	iEditor->SetAknEditorFlags( ( ( iFindKeywordMode ) ?
   	                                    EAknEditorFlagDefault :
   	                                    EAknEditorFlagLatinInputModesOnly ) |
   	                            EAknEditorFlagUseSCTNumericCharmap );
    iEditor->SetAknEditorPermittedCaseModes( EAknEditorUpperCase |
                                             EAknEditorLowerCase );

    iSearchFieldKeyHandler = new (ELeave) CMPXCommonContainerSearchFieldKeyHandler( *this );

    /* TODO?
    iAvkonAppUi->AddToStackL( iSearchFieldKeyHandler,
                              ECoeStackPriorityFep + 1,
                              ECoeStackFlagRefusesFocus );
    */

    iEditor->SetEdwinObserver( iSearchFieldKeyHandler );
	iEditor->SetSkinBackgroundControlContextL( NULL );

	CCoeEnv::Static()->AddFepObserverL(
	    static_cast<MCoeFepObserver &>( *this ) );

	iPrevKeyword = KNullDesC().AllocL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::SetOrdinalPosition( TInt aPos )
    {
    Window().SetOrdinalPosition( aPos );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::HandleFindSizeChanged()
    {
    if ( IsVisible() )
        {
        TRect searchRect = iParent->GetSearchFieldRect( ETrue,
                                                        iParent->ClientRect() );
        AknLayoutUtils::LayoutControl( this,
                                       searchRect,
                                       AknLayout::popup_find_window() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::EnableKeyEventHandlerL( TBool aEnable )
    {
    iSearchFieldKeyHandler->EnableL( aEnable );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::HandleControlEventL( CCoeControl* /*aControl*/,
                                             TCoeEvent aEventType )
    {
	if ( iFindKeywordMode )
		{
	    if ( iEditor->AknEditorCurrentInputMode() == EAknEditorCalculatorNumberModeKeymap
            // When the user copies and paste's content to the find dialog, the editor
            // is in Text Input mode, The engine should still get the find keyword request
            || iEditor->AknEditorCurrentInputMode() == EAknEditorTextInputMode )
	    	{
		    if ( aEventType == MCoeControlObserver::EEventStateChanged )
		        {
		        HBufC* newText = GetTextLC();
		        if ( ! newText )
		            {
		            newText = KNullDesC().AllocLC();
		            }

		        if ( iPrevKeyword->Compare( *newText ) != 0 )
		        	{
		            delete iPrevKeyword;
		            iPrevKeyword = newText;
		            CleanupStack::Pop( newText );

					iParent->FindKeywordL( *newText );

					iHandleFEPFind = EFalse;
		        	}
				else
					{
					CleanupStack::PopAndDestroy( newText );
					}
		        }
	    	}
		}

    if ( aEventType == EEventRequestFocus )
        {
        FocusChanged( EDrawNow );
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerSearchField::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    // Escape key event handling.
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
        {
        // TODO: Check that closing Search Field goes ok with this case.

        CCoeEnv::Static()->AppUi()->RemoveFromStack( this );
        MakeVisible( EFalse );
        SetFocus( EFalse );
        return EKeyWasConsumed;
        }

	if ( aType == EEventKey )
		{
		/* TODO: This feature not needed?
		// Handle up and down arow keys to search for next and previous keywords.
		if ( aKeyEvent.iCode == EKeyUpArrow )
			{
			// Find previous keyword.
			iParent->FindKeywordL( NULL, EFalse );
			iHandleFEPFind = EFalse;
			return EKeyWasConsumed;
			}
		if ( aKeyEvent.iCode == EKeyDownArrow )
			{
			// Find next keyword.
			iParent->FindKeywordL( NULL, ETrue );
			iHandleFEPFind = EFalse;
			return EKeyWasConsumed;
			}
		*/
		}

	return iEditor->OfferKeyEventL( aKeyEvent, aType );
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( ! AknLayoutUtils::PenEnabled() )
        {
        return;
        }

    // Pointer is inside the Search Field.
    if ( Rect().Contains( aPointerEvent.iPosition ) )
        {
        iSearchFieldKeyHandler->EnableL( ETrue );
        iEditor->HandlePointerEventL( aPointerEvent );
        iSearchFieldKeyHandler->SetFirstKeyEvent( EFalse );
        }
    else
        {
        // Pointer outside of control, close control?
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TInt CMPXCommonContainerSearchField::CountComponentControls() const
    {
    return 2; // iEditor and iInputFrame.
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CCoeControl* CMPXCommonContainerSearchField::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
			return iInputFrame;
            }
        case 1:
            {
            return iEditor;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::SizeChanged()
    {
    iInputFrame->SetRect( Rect() );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::FocusChanged( TDrawNow aDrawNow )
    {
    // this is a workaround
    TRAP_IGNORE( iSearchFieldKeyHandler->EnableL( IsFocused() ) );
    CCoeControl::FocusChanged( aDrawNow );
    iEditor->SetFocus( IsFocused() );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TInt CMPXCommonContainerSearchField::TextLength() const
    {
    return iEditor->TextLength();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
HBufC* CMPXCommonContainerSearchField::GetTextLC() const
    {
    HBufC *retVal = NULL;

    if ( TextLength() || iFindKeywordMode )
        {
        retVal = HBufC::NewLC( TextLength() + 1 );
        TPtr ptr = retVal->Des();
        iEditor->GetText( ptr );
        ptr.ZeroTerminate();
        }

    return retVal;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::SetTextL( const TDesC& aTxt )
    {
    iEditor->SetTextL( &aTxt );

    TInt curPos = TextLength();
    iEditor->SetSelectionL( curPos, curPos );
	iEditor->DrawNow();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::SelectAllL()
    {
    iEditor->SelectAllL();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::SetInfoTextL( const TDesC& aText )
    {
    iInputFrame->SetInfoTextL( aText );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::ClipboardL( CEikEdwin::TClipboardFunc aClipboardFunc )
    {
    iEditor->ClipboardL( aClipboardFunc );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
CEikEdwin* CMPXCommonContainerSearchField::Editor() const
    {
    return iEditor;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CMPXCommonContainerSearchFieldKeyHandler::CMPXCommonContainerSearchFieldKeyHandler( CMPXCommonContainerSearchField& aSearchField )
 :  iEnabled( EFalse ),
    iFirstKeyEvent( ETrue ),
    iLeftRightEvent ( EFalse ),
    iSearchField( aSearchField )
    {
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerSearchFieldKeyHandler::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                          TEventCode aType )
    {
    TKeyResponse response( EKeyWasNotConsumed );
    iLeftRightEvent = EFalse;

    if ( iEnabled && aType == EEventKey )
        {
        switch( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                iFirstKeyEvent = EFalse;
                break;
                }

            case EKeyLeftArrow:
            case EKeyRightArrow:
                {
                iFirstKeyEvent = EFalse;
                iLeftRightEvent = ETrue;

				// Below added to compensate for a weird key event situation:
				//   For some reason the CMPXCommonContainerSearchField::OfferKeyEventL function
				//   is not being called for an EEventKey when you press the left/right
				//   arrows in the menupane produced to paste previously used special
				//   chars (this is brought up by pressing the pen button on the phone).
				//   This only happens when the cursor is in the first or last position in
				//   the Search Field with text in it.
				//   Therefore, in this particular scenario, we need to process the EEventKey
				//   here since we will not be able to in CMPXCommonContainerSearchField::OfferKeyEventL
                // TODO if ( CBrowserAppUi::Static()->ContentView()->MenuBar()->MenuPane()->IsFocused())
		        // TODO    {
		        // TODO    response = CBrowserAppUi::Static()->ContentView()->MenuBar()->MenuPane()->OfferKeyEventL( aKeyEvent, aType );
		        // TODO    }
                break;
                }

            case EKeyBackspace:
            case EKeyDelete:
                {
                if ( iFirstKeyEvent )
                    {
                    iFirstKeyEvent = EFalse;
                    iCoeEnv->SimulateKeyEventL( aKeyEvent, aType );
                    response = EKeyWasConsumed;
                    }
                break;
                }

            default:
                {
                if ( iFirstKeyEvent )
                    {
                    if ( TChar( aKeyEvent.iCode ).IsPrint() )
                        {
                        iFirstKeyEvent = EFalse;
                        response = EKeyWasConsumed;
                        iCoeEnv->SimulateKeyEventL( aKeyEvent, aType );
                        }
                    }
                break;
                }
            }
        }

    return response;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldKeyHandler::HandleEdwinEventL( CEikEdwin* /*aEdwin*/,
                                                     TEdwinEvent aEventType )
    {
    if ( aEventType == EEventTextUpdate )
        {
        // if text changed in the editor we don't have to handle clear key anymore.
        iFirstKeyEvent = EFalse;

        // For some reason typing letters on the qwerty keyboard of one product
        // does not trigger the HandleCompletionOfTransactionL() or the
        // HandleControlEventL(). Therefore we must check the text here.
        iSearchField.UpdateFindKeywordL();
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldKeyHandler::EnableL( TBool aEnable )
    {
    iEnabled = aEnable;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldKeyHandler::Reset()
    {
    iFirstKeyEvent = ETrue;
    }
	
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CMPXCommonContainerSearchFieldKeyHandler::IsLeftRightEvent()
    {
    return iLeftRightEvent;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldKeyHandler::SetFirstKeyEvent( TBool aFirstKeyEvent )
    {
    iFirstKeyEvent = aFirstKeyEvent;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::HandleResourceChange( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::HandleStartOfTransactionL()
	{
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::HandleCompletionOfTransactionL()
	{
	if ( iParent && iFindKeywordMode && this->IsVisible() && iHandleFEPFind )
		{
		UpdateFindKeywordL();
		}

    iHandleFEPFind = ETrue;
	}

// ----------------------------------------------------------------------------
// Simulates pointer event to force VKB.
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::ActivateVkbL()
    {
    if ( iEditor && iEditor->TextView() )
        {
        // Make sure observer is set.
        iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();

       	// First get point at cursor location.
        TInt pos = iEditor->CursorPos();
       	CTextView* textView = iEditor->TextView();
       	TPoint curPos;
       	textView->DocPosToXyPosL( pos, curPos );

       	TPointerEvent pe;
       	pe.iPosition = curPos;

       	pe.iType = TPointerEvent::EButton1Down;
       	iEditor->HandlePointerEventL( pe );

        // VKB will only activate is nothing selected
        iEditor->SetSelectionL( pos, pos );

       	pe.iType = TPointerEvent::EButton1Up;
       	iEditor->HandlePointerEventL( pe );
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerSearchField::UpdateFindKeywordL()
    {
    HBufC* newKeyword = GetTextLC();

    if ( newKeyword->Compare( *iPrevKeyword ) != 0 )
        {
        delete iPrevKeyword;
        iPrevKeyword = newKeyword;
        CleanupStack::Pop( newKeyword );

        iParent->FindKeywordL( *newKeyword );
        }
    else
        {
        CleanupStack::PopAndDestroy( newKeyword );
        }
    }
