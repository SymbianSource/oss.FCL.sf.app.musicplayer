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
* Description: Music Player collection container definition - Genres.
*
*/

// INCLUDE FILES
#include <QtCore>

#include <hbdocumentloader.h>
#include <hblistview.h>

#include "mpcollectioncontainergenres.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainerGenres
    \brief Music Player collection container definition - Genres.

    'Genres' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'Genres' view.

    This container handles the following contexts:
    \li ECollectionContextGenres
    \li ECollectionContextGenreSongs

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 */
MpCollectionContainerGenres::MpCollectionContainerGenres( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent)
{
    TX_LOG
}

/*!
 Destructs the collection container.
 */
MpCollectionContainerGenres::~MpCollectionContainerGenres()
{
    TX_ENTRY
    delete mList;
    TX_EXIT
}

/*!
 Sets up the container by organizing widgets according to its layout.

 \reimp
 */
void MpCollectionContainerGenres::setupContainer()
{
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
    if ( !mList ) {
        bool ok = false;
        mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "genre", &ok);

        if ( !ok ) {
            TX_LOG_ARGS("Error: invalid xml file.");
            Q_ASSERT_X(ok, "MpCollectionContainerGenres::setupContainer", "invalid xml file");
        }

        QGraphicsWidget *widget;
        widget = mDocumentLoader->findWidget(QString("genreList"));
        mList = qobject_cast<HbListView*>(widget);
        initializeList();
    }
    TX_EXIT
}

