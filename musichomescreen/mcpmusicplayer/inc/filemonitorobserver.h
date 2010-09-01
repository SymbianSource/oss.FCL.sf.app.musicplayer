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
* Description:  Observer class to the file monitor
*
*/


#ifndef M_FILEMONITOROBSERVER_H
#define M_FILEMONITOROBSERVER_H


/** 
*   Interface class to monitor to get callbacks to folder changes
*/
class MFileMonitorObserver
    {
public:
    /**
     * Handle a file system change notification
     *
     */
    virtual void HandleFileRemovedL() = 0;
    };

#endif // M_FILEMONITOROBSERVER_H
