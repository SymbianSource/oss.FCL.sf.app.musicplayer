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
* Description: Music Player collection abstract data model.
*
*/

#ifndef MPCOLLECTIONDATAMODEL_H
#define MPCOLLECTIONDATAMODEL_H

#include <QAbstractListModel>
#include "mpmpxcollectionviewdefs.h"

class MpMpxCollectionData;
class MpCollectionAlbumArtManager;

#if defined(BUILD_MPDATA_LIB)
#define MPDATA_EXPORT Q_DECL_EXPORT
#else
#define MPDATA_EXPORT Q_DECL_IMPORT
#endif

class MPDATA_EXPORT MpCollectionDataModel : public QAbstractListModel
{
    Q_OBJECT
   
public:

    explicit MpCollectionDataModel( MpMpxCollectionData *data, QObject *parent=0 );
    virtual ~MpCollectionDataModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);
    
    MpMpxCollectionData *collectionData();
    
signals:

    void orderChanged( int containerId, int itemId, int itemOrdinal, int newOrdinal );
    void dataReloaded();

public slots:

    void setContext( TCollectionContext context );
    void updateAlbumArt( int index );
    void refreshModel();
    void reloadData();

private:

    MpMpxCollectionData             *mCollectionData;       // Not own
    MpCollectionAlbumArtManager     *mAlbumArtManager;      // Own
    int                             mRowCount;
    int                             mAlbumIndexOffset;

};

#endif // MPCOLLECTIONDATAMODEL_H

