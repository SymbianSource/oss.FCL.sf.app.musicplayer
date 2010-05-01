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
#include <hbgroupbox.h>
#include <hbindexfeedback.h>

#include "mpcollectioncontainerartists.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainerArtists
    \brief Music Player collection container definition - Artists.

    'Artists' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'Artists' view.

    This container handles the following contexts:
    \li ECollectionContextArtists
    \li ECollectionContextArtistAlbums
    \li ECollectionContextArtistSongs
    
    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 */
MpCollectionContainerArtists::MpCollectionContainerArtists( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mInfoBar(0)
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
    
    if ( mCollectionContext == ECollectionContextArtists ) {
        mArtist = mCollectionData->itemData(row, MpMpxCollectionData::Title);
        if ( mArtist.isEmpty() ) {
        mArtist = hbTrId("txt_mus_subtitle_unknown");
        }
    }
    else if ( mCollectionContext == ECollectionContextArtistAlbums ) {
        mAlbum = mCollectionData->itemData(row, MpMpxCollectionData::Title);
        if ( mAlbum.isEmpty() ) {
            mAlbum = hbTrId("txt_mus_dblist_val_unknown3");
        }
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
    if ( mCollectionData->count() ) {
        bool ok = false;
        QGraphicsWidget *widget;
        if ( mCollectionContext == ECollectionContextArtists ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artists", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerArtists::setupContainer", "invalid xml file");
            }
            if ( !mList ) {
                widget = mDocumentLoader->findWidget(QString("artistsList"));
                mList = qobject_cast<HbListView*>(widget);
                mIndexFeedback->setItemView(mList);
                initializeList();
            }
            if ( mInfoBar ) {
                delete mInfoBar;
                mInfoBar = 0;
            }
        }
        else if ( mCollectionContext == ECollectionContextArtistAlbums ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artistSongs", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerArtists::setupContainer", "invalid xml file");
            }
           
            widget = mDocumentLoader->findWidget(QString("artistSongsDetail"));
            mInfoBar = qobject_cast<HbGroupBox*>(widget);
            
            QString details;
            if ( mViewMode == MpCommon::FetchView ) { 
                details = "Select a song";
            }
            else {
                details = mArtist;
            }
            mInfoBar->setHeading(details);
        }
        else if ( mCollectionContext == ECollectionContextAlbumSongs ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artistSongs", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerArtists::setupContainer", "invalid xml file");
            }
                   
            widget = mDocumentLoader->findWidget(QString("artistSongsDetail"));
            mInfoBar = qobject_cast<HbGroupBox*>(widget);
                    
            QString details;
            if ( mViewMode == MpCommon::FetchView ) {
                details = "Select a song";
                }
            else {
                details = mArtist;
                details.append(" - ");
                details.append(mAlbum);
            }
            mInfoBar->setHeading(details);
        }
        else if ( mCollectionContext == ECollectionContextArtistSongs ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artistSongs", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerArtists::setupContainer", "invalid xml file");
            }
                   
            widget = mDocumentLoader->findWidget(QString("artistSongsDetail"));
            mInfoBar = qobject_cast<HbGroupBox*>(widget);
                    
            QString details;
            if ( mViewMode == MpCommon::FetchView ) {
                details = "Select a song";
                }
            else if ( mArtist.isEmpty() ) {
                details = hbTrId("txt_mus_subtitle_unknown_all");
            }
            else{
                details = hbTrId( "txt_mus_subtitle_1_all" ).arg(mArtist);
    
            }
            mInfoBar->setHeading(details);
        }
    } 
    else {
        if ( mInfoBar ) {
            // When last song in an album is deleted and album list is reloaded
            delete mInfoBar;
            mInfoBar = 0;
        }
        // Call empty list from base class
        setupEmptyListContainer();
    }
    TX_EXIT
}

