


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

#include <hbicon.h>
#include "stub/inc/mpcollectionalbumartmanager.h"

int gAlbumInitCounter = 0;

/*!
 Returns gAlbumInitCounter.
 gAlbumInitCounter counts the number of constructor/destructor calls.
 */
int MpCollectionAlbumArtManager::getInitCounter()
{
    return gAlbumInitCounter;
}

/*!
 Resets gAlbumInitCounter to zero.
 */
void MpCollectionAlbumArtManager::resetInitCounter()
{
    gAlbumInitCounter = 0;
}

/*!
 Stub function.
 */
MpCollectionAlbumArtManager::MpCollectionAlbumArtManager( QObject *parent )
    : QObject(parent),
      mIcon(0),
      mAlbumArtReturn(true),
      mCacheAlbumArtReturn(true),
      mAlbumArtCount(0),
      mCacheAlbumArtCount(0)
{
    gAlbumInitCounter++;
    mIcon = new HbIcon(QString(":/icons/artists"));
}

/*!
 Stub function.
 */
MpCollectionAlbumArtManager::~MpCollectionAlbumArtManager()
{
    gAlbumInitCounter--;
    delete mIcon;
}

/*!
 Stub function.
 */
HbIcon MpCollectionAlbumArtManager::albumArt( const QString& albumArtUri, int index )
{
    Q_UNUSED(albumArtUri);
    Q_UNUSED(index);
    mAlbumArtCount++;
    HbIcon icon;
    if ( mAlbumArtReturn ) {
        return *mIcon;
    }
    return icon;
}

/*!
 Stub function.
 */
bool MpCollectionAlbumArtManager::cacheAlbumArt( const QStringList albumArtList )
{
    mCacheAlbumArtCount++;
    if ( albumArtList.empty() ) {
        return false;
    }
    return mCacheAlbumArtReturn;
}

/*!
 Stub function.
 */
void MpCollectionAlbumArtManager::cancel()
{
}
