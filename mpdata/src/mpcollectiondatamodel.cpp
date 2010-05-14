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

#include <QIcon>
#include <QList>
#include <QMimeData>
#include <hbicon.h>
#include <hbnamespace.h>

#include "mpcollectiondatamodel.h"
#include "mpcollectionalbumartmanager.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

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
    \fn void orderChanged( int containerId, int itemId, int itemOrdinal, int newOrdinal )

    This signal is emitted when a reorder operations is propagated to the 
    model, indicates that the item with \a containerId , \a itemId is to 
    be moved from \a itemOrdinal to \a newOrdinal.
 */

/*!
    \fn void dataReloaded()

    This signal is emitted when there has been a changed in the data represented
    by this model. This signal is emitted so that the view owning this model can
    refresh the view. This can happen after operations like delete and several
    playlist related operations (save, rearrange, remove).
 */

/*!
 Constructs the collection data model.
 */
MpCollectionDataModel::MpCollectionDataModel( MpMpxCollectionData *data, QObject *parent )
    : QAbstractListModel(parent),
      mCollectionData(data),
      mRowCount(0),
      mAlbumIndexOffset(0)
{
    TX_ENTRY
    connect( mCollectionData, SIGNAL(contextChanged(TCollectionContext)), this, SLOT(setContext(TCollectionContext)) );
    connect( mCollectionData, SIGNAL(dataChanged()), this, SLOT(reloadData()) );
    mAlbumArtManager = new MpCollectionAlbumArtManager(mCollectionData);
    connect( mAlbumArtManager, SIGNAL(albumArtReady(int)), this, SLOT(updateAlbumArt(int)) );
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
    return mRowCount - mAlbumIndexOffset;
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

    int row = index.row() + mAlbumIndexOffset;
    TX_LOG_ARGS("index=" << row << ", role=" << role);
    TCollectionContext context = mCollectionData->context();
    if ( role == Qt::DisplayRole ) {
        QStringList display;
        // Fetch the primary text, which is the title, if available.
        QString primaryText;
        switch ( context ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtists:
            case ECollectionContextAlbums:
            case ECollectionContextArtistAlbums:
            case ECollectionContextPlaylists:
            case ECollectionContextPlaylistSongs:
            case ECollectionContextAlbumsTBone:
            case ECollectionContextArtistAlbumsTBone:
            case ECollectionContextArtistAllSongs:
                primaryText = mCollectionData->itemData(row, MpMpxCollectionData::Title);
                if ( !primaryText.isEmpty() ) {
                    display << primaryText;
                }
                else {
                    display << hbTrId("txt_mus_other_unknown4");
                }
                break;
            case ECollectionContextAlbumsMediaWall:
                primaryText = mCollectionData->itemData(row, MpMpxCollectionData::Artist);
                if ( !primaryText.isEmpty() ) {
                    display << primaryText;
                }
                else {
                    display << hbTrId("txt_mus_other_unknown4");
                }    
                break;
            default:
                break;
        }
        
        // Fetch the secondary text, which depends on the current context, if available.
        QString secondaryText;
        switch ( context ) {
            case ECollectionContextAllSongs:
            case ECollectionContextAlbums:
            case ECollectionContextPlaylistSongs:
            case ECollectionContextAlbumsTBone:
                secondaryText = mCollectionData->itemData(row, MpMpxCollectionData::Artist);
                if ( !secondaryText.isEmpty() ) {
                    display << secondaryText;
                }
                else {
                    display << hbTrId("txt_mus_other_unknown3");
                }
                break;
            case ECollectionContextArtistAlbumsTBone:
                secondaryText = mCollectionData->collectionTitle();
                if ( !secondaryText.isEmpty() ) {
                    display << secondaryText;
                }
                else {
                    display << hbTrId("txt_mus_other_unknown3");
                }
                break;
            case ECollectionContextArtistAllSongs:
                secondaryText = mCollectionData->itemData(row, MpMpxCollectionData::Album);
                if ( !secondaryText.isEmpty() ) {
                    display << secondaryText;
                }
                else {
                    display << hbTrId("txt_mus_other_unknown4");
                }
                break;
            case ECollectionContextAlbumsMediaWall:
                secondaryText = mCollectionData->itemData(row, MpMpxCollectionData::Title);
                if ( !secondaryText.isEmpty() ) {
                    display << secondaryText;
                }
                else {
                    display << hbTrId("txt_mus_other_unknown3");
                }
                break;
            default:
                break;
        }
        returnValue = display;
    }
    else if ( role == Qt::DecorationRole ) {
        switch ( context ) {
            case ECollectionContextAlbums:
            case ECollectionContextAlbumsMediaWall:
            case ECollectionContextArtistAlbumsTBone:
            case ECollectionContextAlbumsTBone:
                    returnValue = mAlbumArtManager->albumArt( row ) ;
                break;
            case ECollectionContextArtistAlbums:
                if ( row == 0 ) {
                    returnValue = HbIcon( "qtg_small_sound" );
                }
                else {
                    returnValue = mAlbumArtManager->albumArt( row );
                }
                break;
        }
    }
    else if ( role == Hb::IndexFeedbackRole ) {
        QString feedbackIndex;
        switch ( context ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtists:
            case ECollectionContextAlbums:
            case ECollectionContextArtistAlbums:
            case ECollectionContextPlaylists:
            case ECollectionContextPlaylistSongs:
            case ECollectionContextAlbumsTBone:
            case ECollectionContextArtistAlbumsTBone:
            case ECollectionContextArtistAllSongs:
                feedbackIndex = mCollectionData->itemData(row, MpMpxCollectionData::Title);
                break;
            case ECollectionContextAlbumsMediaWall:
                feedbackIndex = mCollectionData->itemData(row, MpMpxCollectionData::Artist);
                break;
            default:
                break;
        }
        returnValue = feedbackIndex;
    }
    TX_EXIT
    return returnValue;
}

/*!
 \reimp
 */
Qt::DropActions MpCollectionDataModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

/*! 
 \reimp
*/
bool MpCollectionDataModel::removeRows(int row, int count, const QModelIndex &parent )
{
    if ( count != 1 || row >= mRowCount ) {
        return false;
    }
    beginRemoveRows ( parent, row, row);
    //This call internally caches the item, to be inserted if it it drag and drop.
    mCollectionData->removeItem(row);
    mRowCount--;
    endRemoveRows();
    return true;
}

/*! 
 \reimp
*/
QStringList MpCollectionDataModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/x-mpcollectiondatamodelrowandids");
    return types;
}

/*! 
 \reimp
*/
QMimeData *MpCollectionDataModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.count() != 1)
        return 0;
    QStringList types = mimeTypes();
    if (types.isEmpty())
        return 0;
    QMimeData *data = new QMimeData();
    QString format = types.at(0);
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexes.at(0).row();
    stream << mCollectionData->containerId();
    stream << mCollectionData->itemId( indexes.at(0).row() );
    
    data->setData(format, encoded);
    return data;
}

/*! 
 \reimp
*/
bool MpCollectionDataModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                          int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column);
    // check if the action is supported
    if (!data || action != Qt::MoveAction | row > mRowCount ) {
        return false;
    }
    // check if the format is supported
    QStringList types = mimeTypes();
    if (types.isEmpty()) {
        return false;
    }
    QString format = types.at(0);
    if (!data->hasFormat(format)) {
        return false;
    }
    // decode and insert
    QByteArray encoded = data->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    int rowFrom = -1;
    int mpxContainerId = -1;
    int mpxItemId = -1;
    if (!stream.atEnd()) {
        stream >> rowFrom;
        stream >> mpxContainerId;
        stream >> mpxItemId;
    } 
    if ( rowFrom == -1 || mpxContainerId == -1 || mpxItemId == -1  || 
            !mCollectionData->testCachedItem( mpxItemId ) || rowFrom > mRowCount) {
        return false;
    }    
        
    beginInsertRows( parent, row, row );
    emit orderChanged( mpxContainerId, mpxItemId, rowFrom, row );
    mCollectionData->insertCachedItem( row );
    mRowCount++;
    endInsertRows();
    return true;
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpCollectionDataModel::collectionData()
{
    return mCollectionData;
}

/*!
 Slot to be called when collection context is changed.
 */
void MpCollectionDataModel::setContext( TCollectionContext context )
{
    TX_ENTRY_ARGS( "context=" << context );
    // Reset the album index offset for navigation
    mAlbumIndexOffset = 0;
    switch ( context ) {
        case ECollectionContextArtistAlbums:
        case ECollectionContextAlbums:
            mAlbumArtManager->setThumbnailSize( MpCommon::ListThumb );
            mAlbumArtManager->enableDefaultArt( true );
            break;
        case ECollectionContextArtistAlbumsTBone:
            if ( mCollectionData->count() > 1 ) {
                // Selected artist has more than 1 album and therefore the
                // artist's "All songs" exist. Since we don't show artist's
                // "All songs" in TBone, we need to set an offset.
                mAlbumIndexOffset = 1;
            }
            //intentional fallthrough
       case ECollectionContextAlbumsTBone:
            mAlbumArtManager->setThumbnailSize( MpCommon::TBoneThumb );
            mAlbumArtManager->enableDefaultArt( false );
            break;
        case ECollectionContextAlbumsMediaWall:
            mAlbumArtManager->setThumbnailSize( MpCommon::MediaWallThumb );
            mAlbumArtManager->enableDefaultArt( false );
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when album art for the \a index needs to be updated.
 */
void MpCollectionDataModel::updateAlbumArt( int index )
{
    TX_ENTRY_ARGS("index=" << index);

    index -= mAlbumIndexOffset;
    if ( index >= 0 && index < mRowCount ) {
        QModelIndex modelIndex = QAbstractItemModel::createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    TX_EXIT
}

/*!
 Slot to be called when data has changed and model needs to be refreshed
 to reflect the new data.
 */
void MpCollectionDataModel::refreshModel()
{
    TX_ENTRY
    // Cancel all outstanding album art request first, then reset the model.
    mAlbumArtManager->cancel();
    mRowCount = mCollectionData->count();

    TCollectionContext context = mCollectionData->context();
    if ( context == ECollectionContextAlbums || 
         context == ECollectionContextArtistAlbums ||
         context == ECollectionContextAlbumsMediaWall ) {
        // Before providing the new data to the view (list, grid, etc.), we want
        // to make sure that we have enough album arts for the first screen.
        mAlbumArtManager->cacheFirstScreen();
    }
    reset();
    TX_EXIT
}

/*!
 Slot to be called when data has changed (same context) and model needs to reload
 the data.
 */
void MpCollectionDataModel::reloadData()
{
    TX_ENTRY
    mAlbumArtManager->cancel();
    mRowCount = mCollectionData->count();
    reset();
    emit dataReloaded();
    TX_EXIT
}

