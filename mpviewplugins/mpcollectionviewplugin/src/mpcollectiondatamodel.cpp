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
* Description: Music Player collection data model.
*
*/

#include <QBrush>
#include <QColor>

#include <hbicon.h>

#include "mpcollectiondatamodel.h"
#include "mpcollectionalbumartmanager.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

const int KInitCacheSize = 11;

/*!
    \class MpCollectionDataModel
    \brief Music Player collection data model.

    Collection data model implements the interface specified by HbAbstractDataModel,
    which defines the standard interface that item models must use to be able to
    interoperate with other components in the model/view architecture.

    Every item of data that can be accessed via a model has an associated model
    index.

    Each item has a number of data elements associated with it and they can be
    retrieved by specifying a role (see Qt::ItemDataRole) to the model's data
    returned by itemData() function.

    \sa HbAbstractDataModel
*/

/*!
 Constructs the collection data model.
 */
MpCollectionDataModel::MpCollectionDataModel( MpMpxCollectionData *data, QObject *parent )
    : QAbstractListModel(parent),
      mCollectionData(data),
      mRowCount(0),
      mCachingInProgress(false)
{
    TX_ENTRY
    mAlbumArtManager = new MpCollectionAlbumArtManager();
    connect( mAlbumArtManager, SIGNAL(albumArtReady(int)), this, SLOT(updateAlbumArt(int)) );
    connect( mAlbumArtManager, SIGNAL(albumCacheReady()), this, SLOT(albumCacheReady()) );
    TX_EXIT
}

/*!
 Destructs the collection data model.
 */
MpCollectionDataModel::~MpCollectionDataModel()
{
    TX_ENTRY
    delete mAlbumArtManager;
    TX_EXIT
}

/*!
 Returns the number of rows under the given \a parent.

 View will request for the row count immediately after a model is set.
 To prevent it from reading data while caching the album art for the first
 screen, return row count as zero.

 \reimp
 */
int MpCollectionDataModel::rowCount( const QModelIndex &parent ) const
{
    TX_LOG
    Q_UNUSED(parent);
    if ( mCachingInProgress ) {
        return 0;
    }
    else {
        return mRowCount;
    }
}

/*!
 Returns the data stored for the item referred to by the \a index.

 \reimp
 */
QVariant MpCollectionDataModel::data(const QModelIndex &index, int role) const
{
    TX_ENTRY
    QVariant returnValue = QVariant();
    if ( !index.isValid() ) {
        return returnValue;
    }

    int row = index.row();
    TX_LOG_ARGS("index=" << row << ", role=" << role);
    TCollectionContext context = mCollectionData->context();
    if ( role == Qt::DisplayRole ) {
        QStringList display;
        // Fetch the primary text, which is the title, if available.
        QString primaryText;
        primaryText = mCollectionData->itemData(row, MpMpxCollectionData::Title);
        if ( !primaryText.isEmpty() ) {
            display << primaryText;
        }
        else {
            display << QString( tr("Unknown") );
        }

        // Fetch the secondary text, which depends on the current context, if available.
        QString secondaryText;
        switch ( context ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtistAlbums:
            case ECollectionContextPlaylistSongs:
                secondaryText = mCollectionData->itemData(row, MpMpxCollectionData::Artist);
                if ( !secondaryText.isEmpty() ) {
                    display << secondaryText;
                }
                else {
                    display << QString( tr("Unknown") );
                }
                break;
            case ECollectionContextPlaylists:
                secondaryText = mCollectionData->itemData(row, MpMpxCollectionData::Count);
                if ( !secondaryText.isEmpty() ) {
                    display << secondaryText;
                }
                break;
        }
        returnValue = display;
    }
    else if ( role == Qt::DecorationRole ) {
        switch ( context ) {
            case ECollectionContextArtistAlbums:
                bool defaultArt = true;
                QString albumArtUri = mCollectionData->itemData(row, MpMpxCollectionData::AlbumArtUri);
                if ( !albumArtUri.isEmpty() ) {
                     HbIcon icon = mAlbumArtManager->albumArt(albumArtUri, row);
                     if ( !icon.isNull() ) {
                         returnValue = icon;
                         defaultArt = false;
                     }
                }
                if ( defaultArt ) {
                    // No album art, use default album art
                    HbIcon icon(QString(":/icons/default_album.png"));
                    returnValue = icon;
                }
                break;
        }
    }
    TX_EXIT
    return returnValue;
}

/*!
 Must be called when data has changed and model needs to be refreshed
 to reflect the new data.
 */
void MpCollectionDataModel::refreshModel()
{
    TX_ENTRY
    // Cancel all outstanding album art request first, then reset the model.
    mAlbumArtManager->cancel();
    mRowCount = mCollectionData->count();

    TCollectionContext context = mCollectionData->context();
    if ( context == ECollectionContextArtistAlbums ) {
        // Before providing the new data to the view (list, grid, etc.), we want
        // to make sure that we have enough album arts for the first screen.
        if ( mRowCount > 0 ) {
            int initCount = ( mRowCount > KInitCacheSize ) ? KInitCacheSize : mRowCount;
            QStringList albumArtList;
            QString albumArtUri;
            for ( int i = 0; i < initCount; i++ ) {
                albumArtUri = mCollectionData->itemData(i, MpMpxCollectionData::AlbumArtUri);
                if ( !albumArtUri.isEmpty() ) {
                     albumArtList << albumArtUri;
                }
            }
            mCachingInProgress = mAlbumArtManager->cacheAlbumArt(albumArtList);
            if ( !mCachingInProgress ) {
                reset();
            }
        }
        else {
            reset();
        }
    }
    else {
        reset();
    }
    TX_EXIT
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpCollectionDataModel::collectionData()
{
    return mCollectionData;
}

/*!
 Slot to be called when album art for the \a index needs to be updated.
 */
void MpCollectionDataModel::updateAlbumArt( int index )
{
    TX_ENTRY_ARGS("index=" << index);
    if ( index > 0 && index < mRowCount ) {
        QModelIndex modelIndex = QAbstractItemModel::createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    TX_EXIT
}

/*!
 Slot to be called when album art cache is ready.
 */
void MpCollectionDataModel::albumCacheReady()
{
    TX_ENTRY
    if ( mCachingInProgress ) {
        mCachingInProgress = false;
        reset();
    }
    TX_EXIT
}
