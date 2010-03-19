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
* Description: Music Fetcher interface.
*
*/

#ifndef MUSICFETCHER_H
#define MUSICFETCHER_H

#include <xqserviceprovider.h>
#include <QStringList>

// Forward declarations
class GetMusicService;

#if defined(BUILD_MUSICFETCHER_LIB)
#define MUSICFETCHER_EXPORT Q_DECL_EXPORT
#else
#define MUSICFETCHER_EXPORT Q_DECL_IMPORT
#endif

/**
 *  MusicFetcher
 *
 */
class MUSICFETCHER_EXPORT MusicFetcher : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MusicFetcher();

    /**
     * Destructor.
     */
    ~MusicFetcher();

    QString contextTitle() const;

public slots:
    void itemSelected(QString songName);

signals:
    void titleReady(const QString& title);

private:

    GetMusicService *mService;
    friend class GetMusicService;
};

/**
 *  GetMusicService
 *
 */
class GetMusicService : public XQServiceProvider
{
    Q_OBJECT
public:
    GetMusicService( MusicFetcher *parent = 0 );
    ~GetMusicService();
    bool isActive();
    void complete( QStringList filesList);
    QString contextTitle() const;

public slots:
    void fetch(QString title);

private:
    void doComplete( QStringList filesList);

private:
    int mRequestIndex;
    MusicFetcher* mServiceApp;
    QString mTitle;
};

#endif // MUSICFETCHER_H
