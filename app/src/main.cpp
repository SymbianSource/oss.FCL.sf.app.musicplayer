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
#include <hbtranslator.h>

#include "mpmainwindow.h"
#include "mptrace.h"

/*!
    \brief The Music Player main function.

    This function is where Music Player starts execution.
    
    This function can be called by starting an activity. Currently supported:
    -MusicMainView
    -MusicNowPlayingView
    
    A way to start start an activity is by using the XQApplicationManager:
    
    QUrl url;
    url.setUrl("appto://10207C62?activityname=MusicMainView&launchtype=standalone");
    mReq = mAppMgr.create(url);    
    if (mReq == NULL) {
        // No handlers for the URI
        return;
    }
    bool res = mReq->send(); //Fire and Forget
   if (!res) {
       // Request failed. Handle error 
      int error = mReq->lastError();
   }
*/

int main(int argc, char *argv[])
{
    TX_STATIC_ENTRY

    // Initialization
    HbApplication app(argc, argv);
    QVariantHash params = app.activateParams();
    HbTranslator translator;
    translator.loadCommon();
    MpMainWindow::ActivityMode mode;
    
    if ( !params.value( "activityname" ).toString().compare( "MusicNowPlayingView" ) ) { 
        if( params.contains( "shuffle" ) ) {
            mode = !params.value( "shuffle" ).toString().compare( "yes" ) ? MpMainWindow::MusicNowPlayingViewShuffleAll : MpMainWindow::MusicNowPlayingView;
        }
        else {
            mode = MpMainWindow::MusicNowPlayingView;
        }
    }
    else {
        mode = MpMainWindow::MusicMainView;
    }

    // Main window widget.
    // Includes decorators such as signal strength and battery life indicator.
    MpMainWindow mainWindow;
    mainWindow.viewport()->grabGesture(Qt::SwipeGesture);
    mainWindow.viewport()->grabGesture(Qt::PanGesture);
    mainWindow.viewport()->grabGesture(Qt::TapGesture);
    mainWindow.viewport()->grabGesture(Qt::TapAndHoldGesture);
    mainWindow.initialize( mode );
    mainWindow.show();

    // Enter event loop
    TX_EXIT
    return app.exec();

}

