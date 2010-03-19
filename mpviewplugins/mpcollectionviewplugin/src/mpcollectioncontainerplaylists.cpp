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

#include "mpcollectioncontainerplaylists.h"
#include "mpmpxcollectiondata.h"
#include "mpcollectioninfobar.h"
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
      mInfoBar(0),
      mListInitialized(false)
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

    if ( !mList ) {
        bool ok = false;
        mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "playlist", &ok);

        if ( !ok ) {
            TX_LOG_ARGS("Error: invalid xml file.");
            Q_ASSERT_X(ok, "MpCollectionContainerPlaylists::setupContainer", "invalid xml file");
        }

        QGraphicsWidget *widget;
        widget = mDocumentLoader->findWidget(QString("playlistDetail"));
        mInfoBar = qobject_cast<MpCollectionInfoBar*>(widget);

        widget = mDocumentLoader->findWidget(QString("playlistList"));
        mList = qobject_cast<HbListView*>(widget);
        initializeList();
    }

    int count = mCollectionData->count();
    QString details;
    QString detailsCount;
    QString playlist;
    switch ( mCollectionContext ) {
    case ECollectionContextPlaylists:
        details.setNum(count);
        details.append( tr(" playlist(s)") );
        mInfoBar->setText(details);
        break;
    case ECollectionContextPlaylistSongs:
        if ( mViewMode == MpCommon::FetchView ) {
            details = "Select a song";
        }
        else {
            details = QString( tr("Playlist: ") );
            playlist = mCollectionData->collectionTitle();
            if ( playlist.isEmpty() ) {
                playlist = QString( tr("Unknown") );
            }
            details.append(playlist);
            detailsCount.setNum(count);
        }
        mInfoBar->setText(details, detailsCount);
        break;
    default:
        TX_LOG_ARGS("Error: Wrong context; should never get here.");
        break;
    }
    TX_EXIT
}

