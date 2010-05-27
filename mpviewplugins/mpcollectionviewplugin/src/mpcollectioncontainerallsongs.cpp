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
* Description: Music Player collection container definition - All Songs.
*
*/

#include <QtCore>

#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hbgroupbox.h>
#include <hbindexfeedback.h>

#include "mpcollectioncontainerallsongs.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainerAllSongs
    \brief Music Player collection container definition - All Songs.

    'All Songs' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'All Songs' view.

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 */
MpCollectionContainerAllSongs::MpCollectionContainerAllSongs( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mInfoBar(0)
{
    TX_LOG
    mCollectionContext = ECollectionContextAllSongs;
}

/*!
 Destructs the collection container.
 */
MpCollectionContainerAllSongs::~MpCollectionContainerAllSongs()
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
void MpCollectionContainerAllSongs::setupContainer()
{
    TX_ENTRY
    if ( mCollectionData->count() ) {
        bool ok = false;
        mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "allSongs", &ok);
        if ( !ok ) {
            TX_LOG_ARGS("Error: invalid xml file.");
            Q_ASSERT_X(ok, "MpCollectionContainerAllSongs::setupContainer", "invalid xml file");
        }
        QGraphicsWidget *widget;
        widget = mDocumentLoader->findWidget(QString("allSongsDetail"));
        mInfoBar = qobject_cast<HbGroupBox*>(widget);

        widget = mDocumentLoader->findWidget(QString("allSongsList"));
        mList = qobject_cast<HbListView*>(widget);
        mIndexFeedback->setItemView(mList);
        initializeList();

        QString details;
        if ( mViewMode == MpCommon::FetchView ) {
            details = hbTrId("txt_mus_subtitle_select_a_song");
        }
        else {
            int count = mCollectionData->count();
            details = hbTrId("txt_mus_subhead_ln_songs", count);
        }
        mInfoBar->setHeading(details);
    }
    else {
        // Call empty list from base class
        setupEmptyListContainer();
    }
    TX_EXIT
}

