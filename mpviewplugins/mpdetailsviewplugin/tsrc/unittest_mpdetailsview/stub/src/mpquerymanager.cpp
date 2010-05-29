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
* Description: Music Player Query Manager.
*
*/

#include "mpquerymanager.h"
#include <QObject>
#include <QList>
#include <QFile>
#include <QUrl>

#include "mpdetailssharedialog.h"
#include "mptrace.h"

const int KRecommendationNum = 2;

MpQueryManager::MpQueryManager(QObject *parent)
                                :QObject(parent)
{
    TX_ENTRY_ARGS("STUB")
    mRecommendationAlbumArtsName << "C:\\Data\\albumOne.png" << "C:\\Data\\albumTwo.png";
    TX_EXIT_ARGS("STUB")
}

MpQueryManager::~MpQueryManager()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}


void MpQueryManager::clearNetworkReplies()
{
    TX_ENTRY_ARGS("STUB Reply count = ");
    TX_EXIT_ARGS("STUB")
}


void MpQueryManager::queryLocalMusicStore(QString artist,QString album,QString title)
{
    TX_ENTRY_ARGS("STUB")
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    TX_EXIT_ARGS("STUB")    
}
    
void MpQueryManager::queryInspireMeItems(QString artist,QString album,QString title)
{
    TX_ENTRY_ARGS("STUB")    
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    TX_EXIT_ARGS("STUB")
}

void MpQueryManager::clearRecommendations()
{
    TX_ENTRY_ARGS("STUB")    
    mDownloadedAlbumArts = 0;
    mAlbumArtsReadyCount = 0;
    for ( int i = 0; i < KRecommendationNum; i++ ) {
       mRecommendationSongs.clear();
       mRecommendationArtists.clear();
       mRecommendationAlbumArtsLink.clear();
       mRecommendationAlbumArtsMap.clear();      
       QFile file( mRecommendationAlbumArtsName.at( i ) );        
       if ( file.exists() ) {
           if ( file.remove() ) {
               TX_LOG_ARGS( "File removed - " << file.fileName() );
           }
           else {
               TX_LOG_ARGS( "Cannot remove file - " << file.fileName() );
           }
       } else {
           TX_LOG_ARGS( "File doesn't exist - " << file.fileName() );
       }
    }
    TX_EXIT_ARGS("STUB")    
}

/*!
 Return recommendation songs
 */
QStringList MpQueryManager::recommendationSongs()
{
    TX_LOG  
    return mRecommendationSongs;
}

/*!
 Return recommendation artists
 */
QStringList MpQueryManager::recommendationArtists()
{
    TX_LOG  
    return mRecommendationArtists;
}

/*!
 Return recommendation album arts links
 */
QStringList MpQueryManager::recommendationAlbumArtsLink()
{
    TX_LOG  
    return mRecommendationAlbumArtsLink;
}

/*!
 Return map of name and pixmap
 */
QMap<QString, QPixmap>  MpQueryManager::recommendationAlbumArtsMap()
{
    TX_LOG  
    return mRecommendationAlbumArtsMap;
}

/*!
 Return the number of ready album arts
 */
int &MpQueryManager::albumArtsReadyCount()
{
    TX_LOG  
    return mAlbumArtsReadyCount;
}

/*!
 Insert one uri & pixmap item into map
 */
void MpQueryManager::insertMapItem( const QString &uri, const QPixmap &pixmap )
{
    TX_ENTRY_ARGS("STUB Map Item URI: " << uri );
    mRecommendationAlbumArtsMap.insert( uri, pixmap );
    TX_EXIT_ARGS("STUB")
}

