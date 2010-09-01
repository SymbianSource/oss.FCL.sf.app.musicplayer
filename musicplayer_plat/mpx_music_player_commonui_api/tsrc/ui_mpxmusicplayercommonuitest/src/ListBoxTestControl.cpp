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
* Description: CListBoxTestControl test class for STIF Test Framework TestScripter.
*
*/
#include "ListBoxTestControl.h"
#include <eikenv.h>
#include <aknsutils.h> 
#include <aknsdrawutils.h> 
#include <aknutils.h> 
#include <aknsbasicbackgroundcontrolcontext.h>
#include <eiklbi.h> 

CListBoxTestControl::~CListBoxTestControl()
	{
	delete iListBox;
	iListBox = NULL;
	delete iBgContext;
	iBgContext = NULL;
	}   
 
CListBoxTestControl* CListBoxTestControl::NewL(void)
    {
    CListBoxTestControl* self = new(ELeave)CListBoxTestControl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
void CListBoxTestControl::ConstructL(void)
    {
    CreateWindowL();

    
    iListBox = new(ELeave) CEikTextListBox();
    iListBox->SetContainerWindowL(*this);
    iListBox->SetMopParent(this);
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    
    CDesCArray* items = static_cast<CDesCArray*> (iListBox->Model()->ItemTextArray());
    _LIT(KItem1, "MenuItem_1");
    items->AppendL(KItem1);
    _LIT(KItem2, "MenuItem_2");
    items->AppendL(KItem2);
    _LIT(KItem3, "MenuItem_3");
    items->AppendL(KItem3);
    _LIT(KItem4, "MenuItem_4");
    items->AppendL(KItem4);
    _LIT(KItem5, "MenuItem_5");
    items->AppendL(KItem5);

    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);

    iListBox->HandleItemAdditionL();
    iListBox->SetFocus(ETrue);

    
    iListBox->CreateScrollBarFrameL();
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);	    
    
	// make first with no size at all
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	// Setting rect will cause SizeChanged to be called
	// and iBgContext size & position is updated accordingly.
	SetRect( CEikonEnv::Static()->EikAppUi()->ClientRect() );	
		
	ActivateL();
	DrawNow();
}
 
void CListBoxTestControl::SizeChanged()
	{
	if ( iBgContext )
		{
		iBgContext->SetRect(Rect());
 
		if ( &Window() )
			{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
			}
		}
	if ( iListBox )
		{
		iListBox->SetRect(Rect());	
		}
	}
 
 
void CListBoxTestControl::HandleResourceChange(TInt aType)
	{
	TRect rect;
 
    if ( aType==KEikDynamicLayoutVariantSwitch )
    	{    
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
    	}
 
	CCoeControl::HandleResourceChange(aType);
	}
 
 
TTypeUid::Ptr CListBoxTestControl::MopSupplyObject(TTypeUid aId)
	{	
	if (iBgContext)
		{
			return MAknsControlContext::SupplyMopObject(aId, iBgContext );
		}	
	return CCoeControl::MopSupplyObject(aId);
	}
 
 
void CListBoxTestControl::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
    
    // draw background skin first.
  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsDrawUtils::Background( skin, iBgContext, this, gc, aRect );
	}

TInt CListBoxTestControl::CountComponentControls() const
	{
    return 1;
	}
 
CCoeControl* CListBoxTestControl::ComponentControl(TInt aIndex) const
	{
    switch (aIndex)
    	{
        case 0:
            return iListBox; 
        default:
            return 0;
    	}
	}

void CListBoxTestControl::HandleControlEventL( CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
	{
    // TODO: Add your control event handler code here
	}

TKeyResponse CListBoxTestControl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
	{
	if( iListBox )
		{
		return iListBox->OfferKeyEventL( aKeyEvent, aType );
		}
	else
		{
		return EKeyWasNotConsumed;
		}
	}

