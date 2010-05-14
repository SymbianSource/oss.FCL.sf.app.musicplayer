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
* Description: Music Player secondary collection abstract data model. This is
*              primarily used to support Mediawall in Collection View. This
*              model represents the list containing album songs.
*
*/

#include <hbnamespace.h>

#include "mpcollectiontbonelistdatamodel.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
    \class MpCollectionTBoneListDataModel
    \brief Music Player collection data model.

    Secondary collection data model implements the interface specified by
    QAbstractListModel, which defines the standard interface that item models
    must use to be able to interoperate with other components in the model/view
    architecture.

    Every item of data that can be accessed via a model has an associated model
    index.

    Each item has a number of data elements associated with it and they can be
    retrieved by specifying a role (see Qt::ItemDataRole) to the model's data
    returned by itemData() function.

    MpCollectionTBoneListDataModel is primarily used to support Mediawall in
    Collection View. This model represents the list containing album songs.

    \sa QAbstractListModel
*/

/*!
    \fn void albumDataChanged()

    This signal is specific to views with TBone. This signal is emitted when
    a new data set is available for the list section of the TBone. Currently,
    the only operation that can trigger this is the delete operation.
 */

/*!
 Constructs the collection data model.
 */
MpCollectionTBoneListDataModel::MpCollectionTBoneListDataModel( MpMpxCollectionData *data, QObject *parent )
    : QAbstractListModel(parent),
      mCollectionData(data),
      mRowCount(0)
{
    TX_ENTRY
    connect( mCollectionData, SIGNAL(refreshAlbumSongs()),
             this, SLOT(refreshModel()) );
    connect( mCollectionData, SIGNAL(albumDataChanged()),
             this, SIGNAL(albumDataChanged()) );
    TX_EXIT
}

/*!
 Destructs the collection data model.
 */
MpCollectionTBoneListDataModel::~MpCollectionTBoneListDataModel()
{
    TX_LOG
}

/*!
 Returns the number of rows under the given \a parent.

 \reimp
 */
int MpCollectionTBoneListDataModel::rowCount( const QModelIndex &parent ) const
{
    TX_LOG
    Q_UNUSED(parent);
    return mRowCount;
}

/*!
 Returns the data stored for the item referred to by the \a index.

 \reimp
 */
QVariant MpCollectionTBoneListDataModel::data(const QModelIndex &index, int role) const
{
    TX_ENTRY
    QVariant returnValue = QVariant();
    if ( !index.isValid() ) {
        return returnValue;
    }

    int row = index.row();
    TX_LOG_ARGS("index=" << row << ", role=" << role);
    if ( role == Qt::DisplayRole ) {
        // Fetch the primary text, which is the title, if available.
        QString songTitle = mCollectionData->albumSongData(row, MpMpxCollectionData::Title);
        if ( !songTitle.isEmpty() ) {
            returnValue = songTitle;
        }
        else {
            returnValue = hbTrId("txt_mus_other_unknown4");
        }
    }
    TX_EXIT
    return returnValue;
}

/*!
 Slot to be called when data has changed and model needs to be refreshed
 to reflect the new data.
 */
void MpCollectionTBoneListDataModel::refreshModel()
{
    TX_ENTRY
    mRowCount = mCollectionData->albumSongsCount();
    reset();
    TX_EXIT
}

