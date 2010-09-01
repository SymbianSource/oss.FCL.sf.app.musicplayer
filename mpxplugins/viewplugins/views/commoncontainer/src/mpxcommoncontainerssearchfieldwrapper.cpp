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
* Description:  Wrapper class for Search Field. Handles search field
*                activation, deactivation and size. Wrapper is a window owning
*                control.
*
*/


#include <aknutils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include "mpxcommoncontainersearchfieldwrapper.h"
#include "mpxcommoncontainersearchfield.h"

// CONSTANTS
_LIT( KMPXCommonContainerPanic, "MPX Common Container" );

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CMPXCommonContainerSearchFieldWrapper::CMPXCommonContainerSearchFieldWrapper(
    MMPXCommonContainerSearchFieldObserver* aObserver )
    : iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CMPXCommonContainerSearchFieldWrapper* CMPXCommonContainerSearchFieldWrapper::NewL(
    MMPXCommonContainerSearchFieldObserver* aObserver )
    {
    CMPXCommonContainerSearchFieldWrapper* self
        = new (ELeave) CMPXCommonContainerSearchFieldWrapper( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldWrapper::ConstructL()
    {
    CreateWindowL();
    iSearchField = CMPXCommonContainerSearchField::NewL( this );
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CMPXCommonContainerSearchFieldWrapper
        ::~CMPXCommonContainerSearchFieldWrapper()
    {
    if ( iSearchField )
        {
        if ( IsSearchFieldActive() )
            {
            CCoeEnv::Static()->AppUi()->RemoveFromStack( iSearchField );
            }
        }
    delete iSearchField;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
TBool CMPXCommonContainerSearchFieldWrapper::IsSearchFieldActive()
    {
    return iSearchField->IsVisible();
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldWrapper::ActivateSearchFieldL()
    {
    MakeVisible( ETrue );
    CCoeEnv::Static()->AppUi()->RemoveFromStack( iSearchField );
    CCoeEnv::Static()->AppUi()->AddToStackL( iSearchField,
            ECoeStackPriorityMenu );
    iSearchField->ActivateL();
    iSearchField->MakeVisible( ETrue );
    iSearchField->SetFocus( ETrue );
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldWrapper::DeactivateSearchField()
    {
    iSearchField->MakeVisible( EFalse );
    iSearchField->SetFocus( EFalse );
    MakeVisible( EFalse );
    CCoeEnv::Static()->AppUi()->RemoveFromStack( iSearchField );
    TRAP_IGNORE( FindKeywordL( KNullDesC ) );
    }

// --------------------------------------------------------------------------
// Returns rectangle for list, based on view's client rectangle. Handles both
// the case when Search Field is visible and the case when it's invisible.
// --------------------------------------------------------------------------
//
TRect CMPXCommonContainerSearchFieldWrapper::GetListRect(
        TBool aSearchVisible, TRect aClientRect )
    {
    TRect listRect = aClientRect;

    SetClientRect( aClientRect ); // Update stored client rect.

    if ( aSearchVisible )
        {
        TAknWindowLineLayout layout =
            AknLayoutScalable_Avkon::popup_find_window().LayoutLine();

        // Height of list is reduced from bottom with size of Search Field.
        listRect.iBr.iY -= layout.iH;

        __ASSERT_DEBUG( listRect.iBr.iY > 0 && layout.iH > 0,
                        User::Panic( KMPXCommonContainerPanic, 1 ) );
        }

    return listRect;
    }

// --------------------------------------------------------------------------
// Returns rectangle for Search Field, based on view's client rectangle.
// Handles both the case when Search Field is visible and the case when it's
// invisible.
// --------------------------------------------------------------------------
//
TRect CMPXCommonContainerSearchFieldWrapper::GetSearchFieldRect(
        TBool aSearchVisible, TRect aClientRect )
    {
    TRect searchRect = TRect( 0, 0, 0, 0 );

    SetClientRect( aClientRect ); // Update stored client rect.

    if ( aSearchVisible )
        {
        TAknWindowLineLayout layout =
            AknLayoutScalable_Avkon::popup_find_window().LayoutLine();

        // TopLeft of Search Field is based on list's bottom, see GetListRect
        searchRect.iTl.iX = aClientRect.iTl.iX;
        searchRect.iTl.iY = aClientRect.iBr.iY - layout.iH;

        // BottomRight of Search Field is at the end of client rectangle.
        searchRect.iBr.iX = aClientRect.iBr.iX;
        searchRect.iBr.iY = aClientRect.iBr.iY;

        __ASSERT_DEBUG( searchRect.iTl.iX < searchRect.iBr.iX &&
                        searchRect.iTl.iY < searchRect.iBr.iY &&
                        searchRect.iBr.iX > 0 &&
                        searchRect.iBr.iY > 0 &&
                        layout.iH > 0,
                        User::Panic( KMPXCommonContainerPanic, 2 ) );
        }

    return searchRect;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldWrapper::FindKeywordL(
        const TDesC& aFindWord )
    {
    if ( iObserver )
        {
        iObserver->NewFilteringWordL( aFindWord );
        }
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
CCoeControl *CMPXCommonContainerSearchFieldWrapper::ComponentControl(
        TInt /*aIndex*/ ) const
    {
    return iSearchField;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
TInt CMPXCommonContainerSearchFieldWrapper::CountComponentControls() const
    {
    return 1;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
TRect& CMPXCommonContainerSearchFieldWrapper::ClientRect()
    {
    return iClientRect;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldWrapper::SetClientRect(
        const TRect& aClientRect )
    {
    iClientRect = aClientRect;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CMPXCommonContainerSearchFieldWrapper::SizeChanged()
    {
    iSearchField->HandleFindSizeChanged();
    }
