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
* Description:  Interface class to monitor publish and subscribe property.
*
*/


#ifndef M_PNSMONITOROBSERVER_H
#define M_PNSMONITOROBSERVER_H

/** 
*   Interface class to monitor publish and subscribe property.
*/
class MPNSMonitorObserver
    {
public:

    /**
     * Called when launching music player on the background
     * (when selecting play from the widget and music player is closed)
     *
     * @since S60 5.0
     */
    virtual void OpeningMusicPlayer() = 0;
    };


#endif // M_PNSMONITOROBSERVER_H
