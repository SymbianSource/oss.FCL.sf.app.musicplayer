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
* Description:  Definition of layout switch observer class.
*
*/



#ifndef M_MMPXLAYOUTSWITCHOBSERVER_H
#define M_MMPXLAYOUTSWITCHOBSERVER_H


/**
 *  Observer class to receive events about layout switch events
 *
 *  @lib mpxplaybackview.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( MMPXLayoutSwitchObserver )
    {
public:

    /**
     * Callback function when layout is changed.
     *
     * @since S60 v3.0
     */
    virtual void HandleLayoutChange() = 0;
    };

#endif  // M_MMPXLAYOUTSWITCHOBSERVER_H
