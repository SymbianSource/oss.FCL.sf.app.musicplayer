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
* Description: MpCollectionAlbumArtManager stub for testing MpCollectionDataModel
*
*/


#ifndef MPCOLLECTIONALBUMARTMANAGER_H
#define MPCOLLECTIONALBUMARTMANAGER_H

#include <QObject>

class HbIcon;

class MpCollectionAlbumArtManager : public QObject
{
    Q_OBJECT

public:

    // Test utility functions
    static int getInitCounter();
    static void resetInitCounter();

    // Stub functions
    explicit MpCollectionAlbumArtManager( QObject *parent=0 );
    virtual ~MpCollectionAlbumArtManager();
    HbIcon albumArt( const QString &albumArtUri, int index );
    bool cacheAlbumArt( const QStringList albumArtList );
    void cancel();

signals:

    void albumArtReady( int index );
    void albumCacheReady();

public:

    HbIcon      *mIcon;
    bool        mAlbumArtReturn;
    bool        mCacheAlbumArtReturn;
    int         mAlbumArtCount;
    int         mCacheAlbumArtCount;

};

#endif // MPCOLLECTIONALBUMARTMANAGER_H

