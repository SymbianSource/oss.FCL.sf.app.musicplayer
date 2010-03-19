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
* Description: Qt Music Player starting function.
*
*/

#include <QtGui>
#include <hbapplication.h>

#include "mpmainwindow.h"
#include "mptrace.h"

/*!
    \brief The Music Player main function.

    This function is where Music Player starts execution.
*/

int main(int argc, char *argv[])
{
    TX_STATIC_ENTRY

    // Initialization
    HbApplication app(argc, argv);

    // Main window widget.
    // Includes decorators such as signal strength and battery life indicator.
    MpMainWindow mainWindow;
    mainWindow.initialize();
    mainWindow.show();

    // Enter event loop
    TX_EXIT
	return app.exec();
    
}
