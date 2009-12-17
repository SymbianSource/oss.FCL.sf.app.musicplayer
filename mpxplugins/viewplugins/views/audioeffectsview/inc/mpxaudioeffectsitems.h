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
* Description:  Setting List.
*
*/


#ifndef MPXAUDIOEFFECTSITEMS_H
#define MPXAUDIOEFFECTSITEMS_H


// INCLUDES

// SYSTEM INCLUDES
 
#include <aknsettingpage.h> 
#include <akntextsettingpage.h> 
#include <aknsettingitemlist.h> // CAknSettingItemList
#include <AknQueryDialog.h> 

// USER INCLUDES
#include "mpxaudioeffectsmodel.h"
#include "mpxaudioeffects.hrh"

// FORWARD DECLARATIONS
class CAknSettingItem;
class CAknSliderSettingPage;
class CMPXAudioEffectsModel;

// CLASS DECLARATION

/**
*
* @class	CMPXAudioEffectsItems 
* @brief	This is the SettingItemList class for a setting list based on the
* standard Symbian OS architecture.    
*/
NONSHARABLE_CLASS(CMPXAudioEffectsItems) : public CAknSettingItemList, 
                                           public MAknSettingPageObserver
	{
public: // Constructors and destructor

    /**
    *  Constructor
    */
	CMPXAudioEffectsItems(CMPXAudioEffectsModel* aModel);
	
	/**
	* Virtual destructor
	*/
	virtual ~CMPXAudioEffectsItems();
	
private: 
    /*
    * From MAknSettingPageObserver
    */
	void HandleSettingPageEventL(CAknSettingPage* aSettingPage,
	                             TAknSettingPageEvent aEventType);
	
	/*
    * From CAknSettingItemList
    */
	CAknSettingItem* CreateSettingItemL(TInt identifier);
	
	/*
    * From CAknSettingItemList
    */
	void EditItemL (TInt aIndex, TBool aCalledFromMenu);
	
public:// members

    /*
    * ChangeSelectedItem()
    * Modifys the current list box selection
    */
	void ChangeSelectedItemL();
	
private: 
    /**
    * From CAknSettingItemList
    */
	void SizeChanged();

private: // New functions

    /*
    *  Sets the text for the balance slider item
    */
    void SetBalanceTextL( CAknSliderSettingPage* slider );
    
private: // member data
	CMPXAudioEffectsModel* iModel;
	CArrayPtr<CGulIcon>* iIconArray;
	};

#endif // MPXAUDIOEFFECTSITEMS_H

// End of File
