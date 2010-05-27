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

#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#include <QPixmap>
#include "mpviewbase.h"

class MpSongData;
class MpDetailsShareDialog;
class MpEngine;

QTM_USE_NAMESPACE

class MpQueryManager : public QObject
{
    Q_OBJECT

public:
    MpQueryManager( QObject *parent=0 );
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
    int &albumArtsReadyCount();

    void insertMapItem( const QString &uri, const QPixmap &pixmap );

signals:
    void onlineStatusChanged( bool state );
    void setAlbumArt( const QString &albumArtUri, const QString &albumArtName );

private:

    int                     mDownloadedAlbumArts;
    MpDetailsShareDialog*   mSharePopup;

    QString                 mArtist;
    QString                 mAlbum;
    QString                 mTitle;

    QStringList          mRecommendationAlbumArtsName;

    QStringList          mRecommendationSongs;
    QStringList          mRecommendationArtists;
    QStringList          mRecommendationAlbumArtsLink;
    QMap<QString, QPixmap>  mRecommendationAlbumArtsMap;
    int                     mAlbumArtsReadyCount;

};
#endif /* MPQUERYMANAGER_H_ */
