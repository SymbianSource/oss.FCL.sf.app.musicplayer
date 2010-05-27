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
* Description: MpMpxCollectionData stub for testing MpCollectionDataModel
*
*/

#ifndef MPMPXCOLLECTIONDATA_H
#define MPMPXCOLLECTIONDATA_H

#include <QObject>

#include "mpmpxcollectionviewdefs.h"


class MpMpxCollectionData : public QObject
{
    Q_OBJECT

public:

    enum DataType {
        Title,
        Artist,
        Album,
        Count,
        AlbumArtUri
    };

    // Test utility functions
    static int getInitCounter();
    static void resetInitCounter();
    static int getItemDataCounter();
    static void resetItemDataCounter();
    static int getremoveItemCounter();
    static void resetremoveItemCounter();
    static void setCachedItemId (int id);
    static int getInsertCachedItemCounter();
    static void resetInsertCachedItemCounter();

    // Stub functions
    explicit MpMpxCollectionData( QObject *parent=0 );
    virtual ~MpMpxCollectionData();

    TCollectionContext context() const;

    int count() const;
    QString collectionTitle() const;
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;

    int containerId();
    int itemId(int index);
    void removeItem(int index);
    bool testCachedItem( int itemId );
    void insertCachedItem(int index);

    void setContext( TCollectionContext context );

signals:

    void contextChanged( TCollectionContext context );

public:

    TCollectionContext      mContext;
    int                     mCount;
    bool                    mItemDataReturn;
    bool                    mCollectionTitleReturn;

};

#endif // MPMPXCOLLECTIONDATA_H

