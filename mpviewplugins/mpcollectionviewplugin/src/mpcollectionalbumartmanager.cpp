


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

#include <thumbnailmanager_qt.h>

#include "mpcollectionalbumartmanager.h"
#include "mptrace.h"

const int KMaxThumbnailReq = 5;

/*!
    \class MpCollectionAlbumArtManager
    \brief Music Player collection album art manager.

    Collection album art manager provides access to album art needed for
    display in certain collection views. It hides interface to the thumbnail
    manager and also implements a caching mechanism for performance reasons.
*/

/*!
    \fn void albumArtReady( int index )

    This signal is emitted when album art for \a index is ready.

    \sa getAlbumArt()
 */

/*!
    \fn void albumCacheReady()

    This signal is emitted when album art cache is ready.

    \sa cacheAlbumArt()
 */

/*!
 Constructs the album art manager.
 */
MpCollectionAlbumArtManager::MpCollectionAlbumArtManager( QObject *parent )
    : QObject(parent),
      mCachingInProgress(false),
      mRequestCount(0)
{
    TX_ENTRY
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);
    
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(thumbnailReady(QPixmap, void *, int, int)) );
    TX_EXIT
}

/*!
 Destructs the album art manager.
 */
MpCollectionAlbumArtManager::~MpCollectionAlbumArtManager()
{
    TX_ENTRY
    cancel();
    mImageCache.clear();
    TX_EXIT
}

/*!
 Returns the album art for the given \a albumArtUri. If the album art is not
 available in its cache, an asynchronous request is made to the thumbnail manager
 and a null icon is returned.

 \sa signal albumArtReady
 */
HbIcon MpCollectionAlbumArtManager::albumArt( const QString& albumArtUri, int index )
{
    TX_ENTRY_ARGS("albumArtUri=" << albumArtUri << ", index=" << index);
    HbIcon icon;
    if ( mImageCache.contains(albumArtUri) ) {
        icon = mImageCache.value(albumArtUri);
        TX_EXIT_ARGS("true - album art returned");
    }
    else {
        if ( mRequestCount < KMaxThumbnailReq ) {
            // Using negative index as priority will ensure that thumbnail requests
            // are processed in the order they were requested.
            int *clientData = new int(index);
            int reqId = mThumbnailManager->getThumbnail( albumArtUri, clientData, -index );
            if ( reqId != -1 ) {
                mTnmReqMap.insert( reqId, albumArtUri );
                mRequestCount++;
                TX_EXIT_ARGS("false - album art requested");
            }
            else {
                TX_EXIT_ARGS("Err: thumbnail manager returned (-1) for getThumbnail request!");
            }
        }
        else {
            mRequestQueue.enqueue( qMakePair(albumArtUri, index) );
            TX_EXIT_ARGS("false - request queued");
        }
    }
    return icon;
}

/*!
 Request to cache the album art for the items specified in \a albumArtList.
 Returns 'true' if caching is started. If all items already exist in cache,
 'false' is returned.

 \sa signal albumCacheReady
 */
bool MpCollectionAlbumArtManager::cacheAlbumArt( const QStringList albumArtList )
{
    TX_ENTRY
    int allAvailable = true;
    if ( !albumArtList.empty() ) {
        QString albumArtUri;
        int reqId;
        QStringListIterator iter(albumArtList);
        while ( iter.hasNext() ) {
            albumArtUri = iter.next();
            if ( !mImageCache.contains(albumArtUri) ) {
                reqId = mThumbnailManager->getThumbnail( albumArtUri );
                if ( reqId != -1 ) {
                    mTnmReqMap.insert( reqId, albumArtUri );
                    mRequestCount++;
                    allAvailable = false;
                }
                else {
                    TX_EXIT_ARGS("Err: thumbnail manager returned (-1) for getThumbnail request!");
                }
                TX_LOG_ARGS(albumArtUri);
            }
        }
    }

    if ( allAvailable ) {
        TX_EXIT_ARGS("Caching is done!");
        return false;
    }
    else {
        TX_EXIT_ARGS("Caching is in progress!");
        mCachingInProgress = true;
        return true;
    }
}

/*!
 Cancels all outstanding album art requests.

 \sa getAlbumArt, cacheAlbumArt
 */
void MpCollectionAlbumArtManager::cancel()
{
    TX_ENTRY
    if ( !mTnmReqMap.empty() ) {
        QMapIterator<int, QString> iter(mTnmReqMap);
        while ( iter.hasNext() ) {
            iter.next();
            bool result = mThumbnailManager->cancelRequest(iter.key());
        }
    }
    mTnmReqMap.clear();
    mRequestQueue.clear();
    mRequestCount = 0;
    mCachingInProgress = false;
    TX_EXIT
}


/*!
 Slot to be called when thumbnail bitmap generation or loading is complete.
 */
void MpCollectionAlbumArtManager::thumbnailReady( const QPixmap& pixmap, void *data, int id, int error )
{
    TX_ENTRY_ARGS("id=" << id << ", error=" << error);

    // Find the index
    if ( mTnmReqMap.contains(id) ) {
        // Remove the request whether it completed successfully or with error.
        QString albumArtUri = mTnmReqMap[id];
        mTnmReqMap.remove( id );
        mRequestCount--;

        if ( mCachingInProgress ) {
            if ( error == 0 ) {
                QIcon qicon(pixmap);
                HbIcon icon(qicon);
                mImageCache.insert(albumArtUri, icon);
            }
            else {
                TX_EXIT_ARGS("Err: thumbnail manager returned (-1) for getThumbnail request!");
            }
            if ( mTnmReqMap.empty() ) {
                TX_LOG_ARGS("Album art cache ready!");
                mCachingInProgress = false;
                emit albumCacheReady();
                return;
            }
        }
        else {
            if ( error == 0 ) {
                int *clientData = (int *)data;
                int index = *clientData;
                delete clientData;

                QIcon qicon(pixmap);
                HbIcon icon(qicon);
                mImageCache.insert(albumArtUri, icon);
                TX_LOG_ARGS("Album art ready for index=" << index);
                emit albumArtReady(index);
            }
            else {
                TX_EXIT_ARGS("Err: thumbnail manager returned (-1) for getThumbnail request!");
            }
        }
    }

    // Check to see if any request is pending in the queue
    while ( !mRequestQueue.isEmpty()
            && (mRequestCount < KMaxThumbnailReq) ) {
        QPair<QString, int> req = mRequestQueue.dequeue();
        QString albumArtUri = req.first;
        int index = req.second;

        // Using negative index as priority will ensure that thumbnail requests
        // are processed in the order they were requested.
        int *clientData = new int(index);
        int reqId = mThumbnailManager->getThumbnail( albumArtUri, clientData, -index );
        if ( reqId != -1 ) {
            mTnmReqMap.insert( reqId, albumArtUri );
            mRequestCount++;
        }
        else {
            TX_EXIT_ARGS("Err: thumbnail manager returned (-1) for getThumbnail request!");
        }
    }
    TX_EXIT
}

