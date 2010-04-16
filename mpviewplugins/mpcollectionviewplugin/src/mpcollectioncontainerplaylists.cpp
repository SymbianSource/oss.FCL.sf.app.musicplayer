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
* Description: Music Player collection container definition - Playlists.
*
*/

#include <QtCore>

#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hbgroupbox.h>
#include <hblabel.h>

#include "mpcollectioncontainerplaylists.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"


/*!
    \class MpCollectionContainerPlaylists
    \brief Music Player collection container definition - Playlists.

    'Playlists' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'Playlists' view.

    This container handles the following contexts:
    \li ECollectionContextPlaylists
    \li ECollectionContextPlaylistSongs

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 This container handles the both the auto playlists and user created playlists.
 */
MpCollectionContainerPlaylists::MpCollectionContainerPlaylists( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mInfoBar(0)
{
    TX_LOG
}

/*!
 Destructs the collection container.
 */
MpCollectionContainerPlaylists::~MpCollectionContainerPlaylists()
{
    TX_ENTRY
    delete mInfoBar;
    delete mList;
    TX_EXIT
}

/*!
 Sets up the container by organizing widgets according to its layout.

 \reimp
 */
void MpCollectionContainerPlaylists::setupContainer()
{
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
    if ( mCollectionData->count() ) {
        bool ok = false;
        QGraphicsWidget *widget;
        if ( mCollectionContext == ECollectionContextPlaylists ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "playlists", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerPlaylists::setupContainer", "invalid xml file");
            }
            if ( !mList ) {
                widget = mDocumentLoader->findWidget(QString("playlistsList"));
                mList = qobject_cast<HbListView*>(widget);
                initializeList();
            }
            if ( mInfoBar ) {
                delete mInfoBar;
                mInfoBar = 0;
            }
        }
        else if ( mCollectionContext == ECollectionContextPlaylistSongs ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "playlistSongs", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerPlaylists::setupContainer", "invalid xml file");
            }

            widget = mDocumentLoader->findWidget(QString("playlistSongsDetail"));
            mInfoBar = qobject_cast<HbGroupBox*>(widget);

            QString details;
            if ( mViewMode == MpCommon::FetchView ) {
                details = "Select a song";
            }
            else {
                details = mCollectionData->collectionTitle();
            }
            mInfoBar->setHeading(details);
        }
        if ( mNoMusic ) {
            delete mNoMusic;
            mNoMusic = 0;
        }
    }
    else {
        if ( mInfoBar ) {
            delete mInfoBar;
            mInfoBar = 0;
        }
        // Call empty list from base class
        setupEmptyListContainer();
    }
    TX_EXIT
}

