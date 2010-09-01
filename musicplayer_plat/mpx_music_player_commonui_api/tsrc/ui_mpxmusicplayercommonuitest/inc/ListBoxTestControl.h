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
#ifndef LISTBOXTESTCONTROL_H_
#define LISTBOXTESTCONTROL_H_

#include <eiktxlbx.h>

class CAknsBasicBackgroundControlContext;

class CListBoxTestControl : public CCoeControl, public MCoeControlObserver
    {
public:
	static CListBoxTestControl* NewL(void);
	virtual ~CListBoxTestControl();
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId); // 
private:
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	virtual TInt CountComponentControls() const;
	virtual CCoeControl* ComponentControl(TInt aIndex) const;
    void ConstructL(void);
	void Draw(const TRect& aRect) const;
	
	void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType );	
private:
	CAknsBasicBackgroundControlContext*	iBgContext; 
	CEikTextListBox* iListBox;
};


#endif /*LISTBOXTESTCONTROL_H_*/
