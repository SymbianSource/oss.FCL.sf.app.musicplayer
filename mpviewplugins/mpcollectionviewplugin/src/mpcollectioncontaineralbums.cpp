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
* Description: Music Player collection container definition - Albums.
*
*/

#include <QtCore>

#include <hbdocumentloader.h>
#include <hblistview.h>
#include <hbgroupbox.h>
#include <hbindexfeedback.h>

#include <hgmediawall.h>

#include "mpcollectioncontaineralbums.h"
#include "mpmpxcollectiondata.h"
#include "mpcollectiondatamodel.h"
#include "mpcollectiontbonelistdatamodel.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainerAlbums
    \brief Music Player collection container definition - Albums.

    'Albums' collection container implements the interface specified
    by MpCollectionContainer. It provides a layout and widgets for the
    'Albums' view.

    This container handles the following contexts:
    \li ECollectionContextAlbums
    \li ECollectionContextAlbumsTBone

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 */
MpCollectionContainerAlbums::MpCollectionContainerAlbums( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mInfoBar(0),
      mTBone(0),
      mTBoneListModel(0),
      mCurrentAlbumIndex(0)
{
    TX_LOG
    mCollectionContext = ECollectionContextAlbums;
}

/*!
 Destructs the collection container.
 */
MpCollectionContainerAlbums::~MpCollectionContainerAlbums()
{
    TX_ENTRY
	delete mInfoBar;
    delete mTBone;
    delete mList;
    delete mTBoneListModel;
    TX_EXIT
}

/*!
 Sets the data model for the container.
 */
void MpCollectionContainerAlbums::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY
    MpCollectionContainer::setDataModel(dataModel);
    if ( mCollectionContext == ECollectionContextAlbums ) {
        if ( mList ) {
            mList->setModel(dataModel);
            if ( mCollectionData->count() ) {
                mList->scrollTo( dataModel->index(mCurrentAlbumIndex, 0) );
            }
        }
    }
    else if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
        if ( mTBone ) {
            mTBone->setModel(dataModel);
            mTBone->scrollTo( dataModel->index(mCurrentAlbumIndex, 0) );
            if ( mTBoneListModel == 0 ) {
                mTBoneListModel = new MpCollectionTBoneListDataModel(mCollectionData);
                connect( mTBoneListModel, SIGNAL(albumDataChanged()), this, SLOT(albumDataChanged()) );
            }
            mList->setModel(mTBoneListModel);
            if ( !mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
                emit findAlbumSongs(mCurrentAlbumIndex);
            }
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when an item is selected by the user.
*/
void MpCollectionContainerAlbums::itemActivated( const QModelIndex &index )
{
    if ( mCollectionContext == ECollectionContextAlbums ) {
        mCurrentAlbumIndex = index.row();
        TX_ENTRY_ARGS("mCurrentAlbumIndex=" << mCurrentAlbumIndex);
        MpCollectionListContainer::itemActivated(index);
    }
    else if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
        int row = index.row();
        TX_ENTRY_ARGS("index=" << row);
        if ( mViewMode == MpCommon::FetchView ) {
            MpCollectionListContainer::itemActivated(index);
        }
        else {
            emit playAlbumSongs(mCurrentAlbumIndex, row);
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when scrolling ends in media wall and an album is centered.
 */
void MpCollectionContainerAlbums::albumCentered()
{
    TX_ENTRY
    QModelIndex index = mTBone->currentIndex();
    if ( mCurrentAlbumIndex != index.row() ) {
        // Prevent reloading if user just moves the center album a little
        // and the same album re-centers.
        mCurrentAlbumIndex = index.row();
        TX_LOG_ARGS("mCurrentAlbumIndex=" << mCurrentAlbumIndex);
        if ( !mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
            emit findAlbumSongs(mCurrentAlbumIndex);
        }
    }
    TX_EXIT
}

/*!
 Slot to be called data model has new data.
 Two cases:
     1) User deleted an album.
	 2) User deleted last song in an album.
 */
void MpCollectionContainerAlbums::dataReloaded()
{
    TX_ENTRY
    if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
        if ( mCurrentAlbumIndex > 0 ) {
            --mCurrentAlbumIndex;
        }
        mTBone->scrollTo( mDataModel->index(mCurrentAlbumIndex, 0) );
        if ( !mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
            emit findAlbumSongs(mCurrentAlbumIndex);
        }
    }
    else {
        MpCollectionListContainer::dataReloaded();
    }
    TX_EXIT
}

/*!
 Slot to be called data model has new data.
 User has deleted one of the songs from TBone list.
 */
void MpCollectionContainerAlbums::albumDataChanged()
{
    TX_ENTRY
    emit findAlbumSongs(mCurrentAlbumIndex);
    TX_EXIT
}

/*!
 Sets up the container by organizing widgets according to its layout.

 \reimp
 */
void MpCollectionContainerAlbums::setupContainer()
{
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
    if ( mCollectionData->count() ) {
        bool ok = false;
        QGraphicsWidget *widget;
        if ( mCollectionContext == ECollectionContextAlbums ) {
            mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "albums", &ok);
            if ( !ok ) {
                TX_LOG_ARGS("Error: invalid xml file.");
                Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
            }
            if ( !mList ) {
                widget = mDocumentLoader->findWidget(QString("albumsList"));
                mList = qobject_cast<HbListView*>(widget);
                mIndexFeedback->setItemView(mList);
                initializeList();
            }
            if ( mInfoBar ) {
                delete mInfoBar;
                mInfoBar = 0;
            }
            if ( mTBone ) {
                delete mTBone;
                mTBone = 0;
            }
        }
        else if ( mCollectionContext == ECollectionContextAlbumsTBone ) {
            if ( mViewMode == MpCommon::FetchView ) {
                mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "albumTBoneFetcher", &ok);
                if ( !ok ) {
                    TX_LOG_ARGS("Error: invalid xml file.");
                    Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
                }
                widget = mDocumentLoader->findWidget(QString("albumTBoneDetail"));
                mInfoBar = qobject_cast<HbGroupBox*>(widget);
                mInfoBar->setHeading( hbTrId("txt_mus_subtitle_select_a_song") );
            }
            else {
                mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "albumTBone", &ok);
                if ( !ok ) {
                    TX_LOG_ARGS("Error: invalid xml file.");
                    Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
                }
            }
            widget = mDocumentLoader->findWidget(QString("albumWall"));
            mTBone = qobject_cast<HgMediawall*>(widget);
            HbIcon defaultIcon( "qtg_large_music_album" );
            defaultIcon.setSize(mTBone->itemSize());
            mTBone->setDefaultImage( defaultIcon.pixmap().toImage() );
            mTBone->setTitleFontSpec( HbFontSpec(HbFontSpec::Primary) );
            mTBone->setDescriptionFontSpec( HbFontSpec(HbFontSpec::Secondary) );
            mTBone->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOff );
            mTBone->enableReflections(true);
            connect( mTBone, SIGNAL(scrollingEnded()), this, SLOT(albumCentered()) );
        }
    }
    else {
        // Must delete widgets when last song is deleted and view is reloaded.
        if ( mInfoBar ) {
            delete mInfoBar;
            mInfoBar = 0;
        }
        if ( mTBone ) {
            delete mTBone;
            mTBone = 0;
        }
        // Call empty list from base class
        setupEmptyListContainer();
    }
    TX_EXIT
}

