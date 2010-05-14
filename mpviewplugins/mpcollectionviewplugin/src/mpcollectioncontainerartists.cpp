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

#include <hgmediawall.h>

#include "mpcollectioncontainerartists.h"
#include "mpmpxcollectiondata.h"
#include "mpcollectiondatamodel.h"
#include "mpcollectiontbonelistdatamodel.h"
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
    \li ECollectionContextArtistAlbumsTBone
    \li ECollectionContextArtistAllSongs

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container.
 */
MpCollectionContainerArtists::MpCollectionContainerArtists( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionListContainer(loader, parent),
      mInfoBar(0),
      mTBone(0),
      mTBoneListModel(0),
      mCurrentArtistIndex(0),
      mCurrentAlbumIndex(0)
{
    TX_LOG
    mCollectionContext = ECollectionContextArtists;
}

/*!
 Destructs the collection container.
 */
MpCollectionContainerArtists::~MpCollectionContainerArtists()
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
void MpCollectionContainerArtists::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
    MpCollectionContainer::setDataModel(dataModel);
    int count = mCollectionData->count();
    switch ( mCollectionContext ) {
        case ECollectionContextArtists:
            if ( mList ) {
                mList->setModel(dataModel);
                if ( count ) {
                    if ( mCurrentArtistIndex >= count ) {
						// This can happen if the last item is deleted
                        mCurrentArtistIndex = count - 1;
                    }
                    mList->scrollTo( dataModel->index(mCurrentArtistIndex, 0) );
                }
                // Reset the current album index for navigation:
                // ECollectionContextArtistAlbums -> ECollectionContextArtists
                mCurrentAlbumIndex = 0;
            }
            break;
        case ECollectionContextArtistAlbums:
            if ( mList ) {
                mList->setModel(dataModel);
                if ( count ) {
                    if ( mCurrentAlbumIndex >= count ) {
                        // This can happen if the last item is deleted
                        mCurrentAlbumIndex = count - 1;
                    }
                    mList->scrollTo( dataModel->index(mCurrentAlbumIndex, 0) );
                }
                // Reset the album index offset for navigation:
                // ECollectionContextArtistAlbumsTBone -> ECollectionContextArtistAlbums
                mAlbumIndexOffset = 0;
            }
            break;
        case ECollectionContextArtistAlbumsTBone:
            if ( mTBone ) {
                if ( count > 1 ) {
                    // Selected artist has more than 1 album and therefore the
                    // artist's "All songs" exist. Since we don't show artist's
                    // "All songs" in TBone, we need to set an offset.
                    mAlbumIndexOffset = 1;
                }
                else {
                    // Selected artist has exactly 1 album and therefore the
                    // artist's "All songs" doesn't exist.
                    mAlbumIndexOffset = 0;
                }
                if ( mCurrentAlbumIndex >= count ) {
				    // This can happen if the last item is deleted
                    mCurrentAlbumIndex = count - 1;
                }
                mTBone->setModel(dataModel);
                mTBone->scrollTo( dataModel->index(mCurrentAlbumIndex - mAlbumIndexOffset, 0) );
                if ( mTBoneListModel == 0 ) {
                    mTBoneListModel = new MpCollectionTBoneListDataModel(mCollectionData);
                    connect( mTBoneListModel, SIGNAL(albumDataChanged()), this, SLOT(albumDataChanged()) );
                }
                mList->setModel(mTBoneListModel);

                if ( !mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
                    emit findAlbumSongs(mCurrentAlbumIndex);
                }
            }
            break;
        case ECollectionContextArtistAllSongs:
            if ( mList ) {
                mList->setModel(dataModel);
            }
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when an item is selected by the user.
 */
void MpCollectionContainerArtists::itemActivated( const QModelIndex &index )
{
    int row = index.row();
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext << "index=" << row);
    switch ( mCollectionContext ) {
        case ECollectionContextArtists:
            mCurrentArtistIndex = row;
            TX_LOG_ARGS("mCurrentArtistIndex=" << mCurrentArtistIndex);
            MpCollectionListContainer::itemActivated(index);
            break;
        case ECollectionContextArtistAlbums:
            mCurrentAlbumIndex = row;
            TX_LOG_ARGS("mCurrentAlbumIndex=" << mCurrentAlbumIndex);
            MpCollectionListContainer::itemActivated(index);
            break;
        case ECollectionContextArtistAlbumsTBone:
            TX_LOG_ARGS("mCurrentAlbumIndex=" << mCurrentAlbumIndex);
            if ( mViewMode == MpCommon::FetchView ) {
                MpCollectionListContainer::itemActivated(index);
            }
            else {
                emit playAlbumSongs(mCurrentAlbumIndex, row);
            }
            break;
        case ECollectionContextArtistAllSongs:
            MpCollectionListContainer::itemActivated(index);
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when scrolling ends in media wall and an album is centered.
 */
void MpCollectionContainerArtists::albumCentered()
{
    TX_ENTRY
    int index = mTBone->currentIndex().row();
    TX_LOG_ARGS("index=" << index);
    index += mAlbumIndexOffset;
    if ( mCurrentAlbumIndex != index ) {
        // Prevent reloading if user just moves the center album a little
        // and the same album re-centers.
        mCurrentAlbumIndex = index;
        TX_LOG_ARGS("mCurrentAlbumIndex=" << mCurrentAlbumIndex);
        if ( !mCollectionData->setCurrentAlbum(mCurrentAlbumIndex) ) {
            emit findAlbumSongs(mCurrentAlbumIndex);
        }
    }
    TX_EXIT
}

/*!
 Slot to be called data model has new data. This occurs after a delete operation is complete.
 Two cases:
     1) User deleted an artist.
	 2) User deleted last song in an album.
 */
void MpCollectionContainerArtists::dataReloaded()
{
    TX_ENTRY
    if ( mCollectionContext == ECollectionContextArtistAlbumsTBone ) {
        if ( mCurrentAlbumIndex > 0 ) {
            --mCurrentAlbumIndex;
        }
        mTBone->scrollTo( mDataModel->index(mCurrentAlbumIndex - mAlbumIndexOffset, 0) );
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
void MpCollectionContainerArtists::albumDataChanged()
{
    TX_ENTRY
    emit findAlbumSongs(mCurrentAlbumIndex);
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
        QString artist;
        switch ( mCollectionContext ) {
            case ECollectionContextArtists:
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
                if ( mTBone ) {
                    delete mTBone;
                    mTBone = 0;
                }
                break;
            case ECollectionContextArtistAlbums:
                mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artistAlbums", &ok);
                if ( !ok ) {
                    TX_LOG_ARGS("Error: invalid xml file.");
                    Q_ASSERT_X(ok, "MpCollectionContainerArtists::setupContainer", "invalid xml file");
                }
                widget = mDocumentLoader->findWidget(QString("artistDetail"));
                mInfoBar = qobject_cast<HbGroupBox*>(widget);

                artist = mCollectionData->collectionTitle();
                if ( artist.isEmpty() ) {
                    artist = hbTrId("txt_mus_subtitle_unknown");
                }
                mInfoBar->setHeading(artist);
                if ( mTBone ) {
                    delete mTBone;
                    mTBone = 0;
                }
                break;
            case ECollectionContextArtistAlbumsTBone:
                {
                if ( mViewMode == MpCommon::FetchView ) {
                    mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artistAlbumTBoneFetcher", &ok);
                    if ( !ok ) {
                        TX_LOG_ARGS("Error: invalid xml file.");
                        Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
                    }
                    widget = mDocumentLoader->findWidget(QString("artistDetail"));
                    mInfoBar = qobject_cast<HbGroupBox*>(widget);
                    mInfoBar->setHeading( hbTrId("txt_mus_subtitle_select_a_song") );
                }
                else {
                    mDocumentLoader->load(QString(":/docml/musiccollection.docml"), "artistAlbumTBone", &ok);
                    if ( !ok ) {
                        TX_LOG_ARGS("Error: invalid xml file.");
                        Q_ASSERT_X(ok, "MpCollectionContainerAlbums::setupContainer", "invalid xml file");
                    }
                    if ( mInfoBar ) {
                        delete mInfoBar;
                        mInfoBar = 0;
                    }
                }
                widget = mDocumentLoader->findWidget(QString("artistAlbumWall"));
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
                break;
            case ECollectionContextArtistAllSongs:
                // No need to load anything. Just reuse the section "artistAlbums" loaded
                // in ECollectionContextArtistAlbums context.
                if ( mViewMode == MpCommon::FetchView ) {
                    mInfoBar->setHeading( hbTrId("txt_mus_subtitle_select_a_song") );
                }
                else {
                    artist = mCollectionData->collectionTitle();
                    if ( artist.isEmpty() ) {
                        artist = hbTrId("txt_mus_subtitle_unknown_all");
                    }
                    else {
                        artist = hbTrId("txt_mus_subtitle_1_all").arg(artist);
                    }
                    mInfoBar->setHeading(artist);
                }
                break;
            default:
                break;
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

