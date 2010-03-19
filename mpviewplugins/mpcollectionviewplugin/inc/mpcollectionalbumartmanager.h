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
* Description: Music Player collection album art manager.
*
*/


#ifndef MPCOLLECTIONALBUMARTMANAGER_H
#define MPCOLLECTIONALBUMARTMANAGER_H

#include <QObject>
#include <QHash>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QBitmap>
#include <hbicon.h>

class ThumbnailManager;

class MpCollectionAlbumArtManager : public QObject
{
    Q_OBJECT

public:

    explicit MpCollectionAlbumArtManager( QObject *parent=0 );
    virtual ~MpCollectionAlbumArtManager();

    HbIcon albumArt( const QString &albumArtUri, int index );
    bool cacheAlbumArt( const QStringList albumArtList );
    void cancel();

signals:

    void albumArtReady( int index );
    void albumCacheReady();

public slots:

    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

private:

    ThumbnailManager                *mThumbnailManager;
    QHash<QString, HbIcon>          mImageCache;
    QMap<int, QString>              mTnmReqMap;
    bool                            mCachingInProgress;

    QQueue< QPair<QString, int> >   mRequestQueue;
    int                             mRequestCount;
};

#endif // MPCOLLECTIONALBUMARTMANAGER_H

