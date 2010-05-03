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
* Description: Music Player collection data.
*
*/

#ifndef MPMPXCOLLECTIONDATA_H
#define MPMPXCOLLECTIONDATA_H

#include <QObject>

#include "mpmpxcollectionviewdefs.h"

class MpMpxCollectionDataPrivate;
class CMPXMedia;

#if defined(BUILD_MPDATA_LIB)
#define MPDATA_EXPORT Q_DECL_EXPORT
#else
#define MPDATA_EXPORT Q_DECL_IMPORT
#endif

class MPDATA_EXPORT MpMpxCollectionData : public QObject
{
    Q_OBJECT

    friend class MpMpxCollectionDataPrivate;

public:

    enum DataType {
        Title,
        Uri,
        Duration,
        Count,
        Artist,
        Album,
        Genre,
        Rating,
        AlbumArtUri
    };

    explicit MpMpxCollectionData( QObject *parent=0 );
    virtual ~MpMpxCollectionData();

    TCollectionContext context() const;

    int count() const;
    QString collectionTitle() const;
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;

    bool isAutoPlaylist();
    bool isAutoPlaylist( int index );
    int itemCount( int index );
    int containerId();
    int itemId(int index);
    void removeItem(int index);
    bool testCachedItem( int itemId );
    void insertCachedItem(int index);

    void setMpxMedia( const CMPXMedia& entries );
    const CMPXMedia& containerMedia();

signals:

    void contextChanged( TCollectionContext context );
private:

    Q_DISABLE_COPY(MpMpxCollectionData)
    MpMpxCollectionDataPrivate *d_ptr;

};

#endif // MPMPXCOLLECTIONDATA_H

