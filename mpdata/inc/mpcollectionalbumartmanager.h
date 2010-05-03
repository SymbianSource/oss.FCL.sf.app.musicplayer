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
#include <QCache>
#include <QIcon>
#include <QList>

class MpMpxCollectionData;
class ThumbnailManager;

class MpCollectionAlbumArtManager : public QObject
{
    Q_OBJECT

public:

    explicit MpCollectionAlbumArtManager( MpMpxCollectionData *data, QObject *parent=0 );
    virtual ~MpCollectionAlbumArtManager();

    const QIcon* albumArt( int index );
    void cacheFirstScreen();
    void cancel();

signals:

    void albumArtReady( int index );

public slots:

    void thumbnailReady( QPixmap pixmap, void *data, int id, int error );

private:

    MpMpxCollectionData             *mCollectionData;
    ThumbnailManager                *mThumbnailManager;
    QCache<int, QIcon>              mImageCache;
    bool                            mCachingInProgress;

    QIcon                           *mDefaultIcon;

    QList<int>                      mRequestQueue;
    bool                            mPendingRequest;
    int                             mRequestId;

};

#endif // MPCOLLECTIONALBUMARTMANAGER_H

