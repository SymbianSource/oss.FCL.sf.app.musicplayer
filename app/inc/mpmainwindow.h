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
* Description: Qt Music Player main window definition.
*
*/


#ifndef MPMAINVIEW_H
#define MPMAINVIEW_H

// System includes
#include <hbmainwindow.h>
#include <mpxviewframeworkqt.h>

// Forward declarations
class MpxViewPlugin;
class MusicFetcher;

// Class declaration
class MpMainWindow: public MpxViewFramework
{
    Q_OBJECT
    Q_PROPERTY(MpxViewPlugin* mCurrentViewPlugin READ currentViewPlugin)

public:

    enum ViewType {
        CollectionView = 1,
        PlaybackView,
        SettingsView,
        DetailsView };

    MpMainWindow();
    ~MpMainWindow();

    void initialize();
    MpxViewPlugin* currentViewPlugin();

public slots:
    void handleCommand( int commandCode );
    void handleLibraryUpdated();
    void handleExitApplication();

private:
    void activateView(ViewType);
    void connectView();
    void disconnectView();

private:

    MpxViewPlugin         *mCollectionViewPlugin; // Not own
    MpxViewPlugin         *mPlaybackViewPlugin;   // Not own
    MpxViewPlugin         *mSettingsViewPlugin;   // Not own
    MpxViewPlugin         *mDetailsViewPlugin;    // Not own
    MpxViewPlugin         *mCurrentViewPlugin;    // Not own
    MpxViewPlugin         *mPreviousViewPlugin;   // Not own
    MusicFetcher		  *mMusicFetcher; 		  // Own

};

#endif // MPMAINVIEW_H
