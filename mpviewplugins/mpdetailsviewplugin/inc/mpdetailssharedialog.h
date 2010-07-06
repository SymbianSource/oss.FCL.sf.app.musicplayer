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
* Description: Header file for share player.
*
*/

#ifdef SHARE_FUNC_ENABLED
#ifndef MPDETAILSSHAREDIALOG_H
#define MPDETAILSSHAREDIALOG_H

#include <HbDialog>
#include <QList>
#include <QSslError>
#include "mpsharedata.h"

class QGraphicsWebView;
class QNetworkAccessManager;
class QNetworkReply;
class MpShareWebView;
class MpSongData;
class HbProgressDialog;

/*!
    Implements Share Player dialog for music player details view.
    First, construct an instance of the class, then call initialize method.
*/
class MpDetailsShareDialog : public HbDialog
{
    Q_OBJECT

public:
    MpDetailsShareDialog();

    /*!
      Initializes an instance of the class with given MpSongData.
      Translation for "unknown" string is passed so that the share
      dialog can display "unknown" if artist or title are not set.
      MpSongData object ownership is not transferred,
      Music Player Engine retains the ownership.
    */
    void initialize( MpSongData* aSongData, const QString& aUnknownTr );
    virtual ~MpDetailsShareDialog();
    void cachePublishingPlayerFiles();
    void logoutPlayer();
    bool isInitialized() const;

public slots:
    void addContext();
    void updateSharedData();
    void onIndexLoad( bool ok );
    void debugJs( QString s );
    void errorHandler( QString aError, QString aMessage );
    void clearCache();
    void showWindow();
    void showProgressDialog();
    void handleRequestSSLErrors( QNetworkReply* aReply, const QList< QSslError >& aErrors );
    void handleRequestFinished( QNetworkReply* aReply );

signals:
    void closeShareDialog();

private:
    void initShareData( MpSongData* aSongData, const QString& aUnknownTr );
    void initLanguage();
    void initNetworkAccessManager();
    void initWebView();
    void initSignalSlots();
    bool initUser();

private:
    MpShareData            mShareData;
    MpShareWebView*        mShareWebView;   // Owned by HbDialog
    QNetworkAccessManager* mShareNetAccMan; // Owned
    HbProgressDialog*      mProgressbar; // Owned
    bool                   mIsInitialized;
};

#endif // MPDETAILSSHAREDIALOG_H
#endif // SHARE_FUNC_ENABLED
