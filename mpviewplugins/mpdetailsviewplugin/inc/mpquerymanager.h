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
#ifndef MPQUERYMANAGER_H_
#define MPQUERYMANAGER_H_

#include <QPixmap>
#include <QPointer>
#include <QNetworkReply>
#include <QDomDocument>
#include <QMap>

#include "mpviewbase.h"

class QNetworkAccessManager;
class ThumbnailManager;

/*!
  MpQueryManager class is responsible for performing music player
  related network queries, including retrieving the URL for the
  music.

  It emits searchUrlRetrieved signal when it has received the
  URL.
*/
class MpQueryManager : public QObject
{
    Q_OBJECT

public:
    MpQueryManager();
    ~MpQueryManager();
    
public:
    void clearNetworkReplies();
    void queryLocalMusicStore( QString mArtist,QString mAlbum,QString mTitle );
    void queryInspireMeItems( QString mArtist,QString mAlbum,QString mTitle );
    void clearRecommendations();
    
    QStringList recommendationSongs();
    QStringList recommendationArtists();
    QStringList recommendationAlbumArtsLink();
    QMap<QString, QPixmap>  recommendationAlbumArtsMap(); 
    void insertMapItem( const QString &uri, const QPixmap &pixmap );    

private slots:
    void retrieveInformationFinished( QNetworkReply* reply );
    void retrieveInformationNetworkError( QNetworkReply::NetworkError error );
    void retrieveInformationSslErrors( const QList<QSslError> &error );
    void DownloadFinished( QNetworkReply* reply );    
    void setAlbumArtUri( const QString &albumArtUri, const QString &albumArtName );
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
                  
private:
    void constructRequest( QString &uri );     
    // retrieve URI from Ovi music server
    void retrieveInformation( const QString &urlEncoded );
    
    void composeAlbumCover( QPixmap albumart );
    QString keyValues( QStringList keys, QStringList values ) const;
    void handleParsedXML();

    bool writeImageToFile( const QByteArray &aImageData, const QString &aImageFileName );
    
signals:
    void networkError();  
    void searchUrlRetrieved( const QString& url );
    void recommendationAlbumArtsReady();

private:    
   
    QNetworkAccessManager   *mManager;
    QNetworkAccessManager   *mDownloadManager;
    
    int                     mDownloadedAlbumArts;
  
    QList<QNetworkReply *>  mReplys;
    
    QDomDocument            mDomDocument;
    ThumbnailManager        *mThumbnailManager; //owned    
    
    QString                 mArtist;
    QString                 mAlbum;
    QString                 mTitle;
    
    QStringList          mRecommendationAlbumArtsName;
    
    QStringList          mRecommendationSongs;
    QStringList          mRecommendationArtists;
    QStringList          mRecommendationAlbumArtsLink;
    QMap<QString, QPixmap>  mRecommendationAlbumArtsMap;
    int                     mAlbumArtsReadyCount;
    QPixmap                 mDefaultRecommendationAlbumArt;
       
 
};
#endif /* MPQUERYMANAGER_H_ */
