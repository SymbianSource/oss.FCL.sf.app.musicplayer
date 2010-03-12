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
* Description:  MPX popuplist
*
*/



#ifndef CMPXPOPUPLIST_H
#define CMPXPOPUPLIST_H


// INCLUDES

#include <aknlists.h>
#include <aknPopup.h>

// CLASS DECLARATION

/**
 *  Wrapper class for avkon popuplist.
 *
 *  @lib mpxcollectionviewhg.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMpxPopupList ) : public CAknPopupList
    {
public:
    
    static CMpxPopupList* NewL( 
            CEikListBox* aListBox, 
            TInt aCbaResource,
            AknPopupLayouts::TAknPopupLayouts aType = AknPopupLayouts::EMenuWindow );            
    
    TRect LayoutRect(); 

    virtual ~CMpxPopupList();
    
protected:
    CMpxPopupList();
        
    };
#endif  // CMPXPOPUPLIST_H

// End of File
