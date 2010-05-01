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

#include "mpcommondefs.h"

// Forward declarations
class MpxViewPlugin;
class MusicServices;

// Class declaration
class MpMainWindow: public MpxViewFramework
{
    Q_OBJECT

public:

    enum ViewType {
        CollectionView = 1,
        PlaybackView,
        SettingsView,
        DetailsView };

    MpMainWindow();
    ~MpMainWindow();

    void initialize();

public slots:
    void handleCommand( int commandCode );
    void handleLibraryUpdated();

    void switchView( Qt::Orientation orientation );
    void initializeServiceView( TUid hostUid );

private:
    void activateView(ViewType);
    void connectView();
    void disconnectView();

    MpxViewPlugin*  loadView( ViewType type, MpCommon::MpViewMode viewMode= MpCommon::DefaultView );

private:

    MpxViewPlugin         *mCollectionViewPlugin; // Own
    MpxViewPlugin         *mPlaybackViewPlugin;   // Own
    MpxViewPlugin         *mSettingsViewPlugin;   // Own
    MpxViewPlugin         *mDetailsViewPlugin;    // Own

    MpxViewPlugin         *mCurrentViewPlugin;    // Own
    ViewType              mVerticalViewType;
    MusicServices         *mMusicServices;        // Own

};

#endif // MPMAINVIEW_H