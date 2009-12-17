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
* Description:  Listbox array observer
*
*/

 

#ifndef MMPXCOMMONLISTBOXARRAYOBSERVER_H
#define MMPXCOMMONLISTBOXARRAYOBSERVER_H

/**
 * Interface for getting event callbacks from listbox array 
 *
 * @lib mpxcommoncontainer.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( MMPXCommonListBoxArrayObserver )
    {
public:

    enum TMPXCommonListBoxArrayEvents
        {
        EMPXCommonListBoxArrayEventIconArrayChange,
        EMPXCommonListBoxArrayEventMediaArrayChange
        };

    /**
     * Handle listbox array events
     *
     *  @param aEvent list box event
     */               
    virtual void HandleListBoxArrayEventL( 
        TMPXCommonListBoxArrayEvents aEvent ) = 0;   
    };    
    
#endif // MMPXCOMMONLISTBOXARRAYOBSERVER_H
