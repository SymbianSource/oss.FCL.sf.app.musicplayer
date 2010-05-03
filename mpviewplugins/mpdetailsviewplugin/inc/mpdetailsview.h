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
* Description: Music Player details view.
*
*/

#ifndef MPDETAILSVIEW_H
#define MPDETAILSVIEW_H

#include <qpixmap>
#include <qpointer.h>
#include <QNetworkReply>
#include <QDomDocument>
#include <QMap>

#include "mpviewbase.h"

class QGraphicsWebView;
class QNetworkAccessManager;

class HbMainWindow;
class HbAction;
class HbLabel;
class HbWidget;
class HbGroupBox;
class HbPushButton;
class HbDocumentLoader;
class HbListWidget;
class HbDialog;
class ThumbnailManager;
class QTranslator;
class QGraphicsLinearLayout;

class MpSongData;
class MpMpxDetailsFrameworkWrapper;

//class declaration
class MpDetailsView : public MpViewBase
{
    Q_OBJECT
    
public:

    MpDetailsView();
    virtual ~MpDetailsView();

    void initializeView();
    void activateView();
    void deactivateView();

signals:
    void command( int aCommand );
    
public slots:
    void back();    
    void albumArtChanged();
    
private slots:
    void share();
    void webViewLoaded( bool ok );
    void playbackInfoChanged();
    void songDetailInfoChanged();
    
    // groupbox handling, one or zero groupbox is expanded
    void toggleDetailsGroupBox( bool state );
    void toggleInspireMeGroupBox( bool state );
      
    void retrieveInformationFinished( QNetworkReply* reply );
    void retrieveInformationNetworkError( QNetworkReply::NetworkError error );
    void retrieveInformationSslErrors( const QList<QSslError> &error );
    
    void DownloadFinished( QNetworkReply* reply );
    
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    
    void addContext();
    void close();
    
private:
    void setupMenu();
    void loadSharePlayer();
    
    void constructRequest( QString &uri );     
    // retrieve URI from Ovi music server
    void retrieveInformation( const QString &urlEncoded );
    
    void composeAlbumCover( QPixmap albumart );
    QString keyValues( QStringList keys, QStringList values ) const;
    void handleParsedXML();
    
    void setAlbumArtUri( const QString &albumArtUri, const QString &albumArtName );
    void RenderInspireMeGroupBox();
    void recommendationAlbumArtReady();
    
private:
    MpSongData              *mSongData;
    MpMpxDetailsFrameworkWrapper   *mFrameworkWrapper;
    
    bool                    mActivated;
    HbMainWindow            *mWindow;
    HbAction                *mSoftKeyBack;
    
    HbWidget                *mContainer;
    HbLabel                 *mSongText; //owned
    HbLabel                 *mAlbumText; //owned
    HbLabel                 *mArtistText; //owned
    HbLabel                 *mAlbumArt; //owned
    HbGroupBox              *mSongDetailsGroupBox; //owned
    HbGroupBox              *mInspireMeGroupBox; //owned
    HbPushButton            *mShareButton; //owned
    HbDocumentLoader        *mDocumentLoader;//owned
    QPixmap                  mCompositePixmap;
    HbListWidget            *mDetailList; // owned by mSongDetailsGroupBox
    HbListWidget            *mInspireList; // owned by mInspireMeGroupBox
    HbWidget                *mInspireMe;
    QGraphicsLinearLayout   *mLayout;
    HbPushButton            *mButton;
    
    QList<QString>          mRecommendationSongs;
    QList<QString>          mRecommendationArtists;
    QList<QString>          mRecommendationAlbumArtsLink;
    QList<QString>          mRecommendationAlbumArtsName;
    
    QNetworkAccessManager   *mManager;
    QNetworkAccessManager   *mDownloadManager;
    
    int                     mDownloadedAlbumArts;
    
    ThumbnailManager        *mThumbnailManager; //owned
    QPixmap                 mDefaultRecommendationAlbumArt;
    QMap<QString, QPixmap>    mRecommendationAlbumArtsMap;
    int                     mAlbumArtsReadyCount;
    QList<QNetworkReply *>  mReplys;
    
    QDomDocument            mDomDocument;
    
    QTranslator             *mMpTranslator;         // Own
    QTranslator             *mCommonTranslator;     // Own
    
    HbDialog                *mPopup;
    QGraphicsWebView        *mWebView; // owned by popup dialog
    
    Q_DISABLE_COPY(MpDetailsView)
};


#endif //MPDETAILSVIEW_H
