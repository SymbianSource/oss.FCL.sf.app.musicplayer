/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer class to the Application monitor
*
*/


#ifndef M_APPLICATIONMONITOROBSERVER_H
#define M_APPLICATIONMONITOROBSERVER_H

#include <e32std.h>

/** 
*   Interface class to monitor an application exit.
*/
class MApplicationMonitorObserver
    {
public:
    /**
     * Handle an application close event.
     */
    virtual void HandleApplicationClosedL(TExitType aReason) = 0;
    };

#endif // M_APPLICATIONMONITOROBSERVER_H
