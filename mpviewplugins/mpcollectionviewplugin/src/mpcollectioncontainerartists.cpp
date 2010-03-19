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
* Description: Music Player collection container definition - Artists.
*
*/

#include <QtCore>

#include <hbdocumentloader.h>
#include <hblistview.h>

#include "mpcollectioncontainerartists.h"
#include "mpmpxcollectiondata.h"
#include "mpcollectioninfobar.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainerArtists
    \brief Music Player collection container definition - Artists.

    'Artists' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'Artists' view.

    This container handles the following contexts:
    \li ECollectionContextArtistAlbums
    \li ECollectionContextAlbumSongs

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 */
MpCollectionContainerArtists::MpCollectionContainerArtists( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mInfoBar(0),
      mListInitialized(false)
{
    TX_LOG
}

/*!
 Destructs the collection container.
 */
MpCollectionContainerArtists::~MpCollectionContainerArtists()
{
    TX_ENTRY
    delete mInfoBar;
    delete mList;
    TX_EXIT
}

/*!
 Slot to be called when an item is selected by the user.
 */
void MpCollectionContainerArtists::itemActivated( const QModelIndex &index )
{
    int row = index.row();
    TX_ENTRY_ARGS("index=" << row);

    mAlbum = mCollectionData->itemData(row, MpMpxCollectionData::Title);
    if ( mAlbum.isEmpty() ) {
        mAlbum = QString( tr("Unknown") );
    }

    mArtist = mCollectionData->itemData(row, MpMpxCollectionData::Artist);
    if ( mArtist.isEmpty() ) {
        mArtist = QString( tr("Unknown") );
    }

    MpCollectionListContainer::itemActivated(index);
    TX_EXIT
}

/*!
 Sets up the container by organizing widgets according to its layout.

 \reimp
 */
void MpCollectionContainerArtists::setupContainer()
{
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);

    if ( !mList ) {
        bool ok = false;
        mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artist", &ok);

        if ( !ok ) {
            TX_LOG_ARGS("Error: invalid xml file.");
            Q_ASSERT_X(ok, "MpCollectionContainerArtists::setupContainer", "invalid xml file");
        }

        QGraphicsWidget *widget;
        widget = mDocumentLoader->findWidget(QString("artistDetail"));
        mInfoBar = qobject_cast<MpCollectionInfoBar*>(widget);

        widget = mDocumentLoader->findWidget(QString("artistList"));
        mList = qobject_cast<HbListView*>(widget);
        initializeList();
    }

    QString details;
    int count = 0;
    switch ( mCollectionContext ) {
        case ECollectionContextArtistAlbums:
            count = mCollectionData->count();
            details.setNum(count);
            details.append( tr(" album(s)") );
            mInfoBar->setText(details);
            break;
        case ECollectionContextAlbumSongs:
            if ( mViewMode == MpCommon::FetchView ) {
                details = "Select a song";
            }
            else {
                details = mArtist;
                details.append(" : ");
                details.append(mAlbum);
            }
            mInfoBar->setText(details);
            break;
        default:
            TX_LOG_ARGS("Error: Wrong context; should never get here.");
            break;
    }
    TX_EXIT
}

