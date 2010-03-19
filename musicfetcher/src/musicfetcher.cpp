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
* Description:
*
*/

#include <hbapplication.h>
#include <qstringlist.h>

#include "musicfetcher.h"
#include "mptrace.h"

/*!
    \class MusicFetcher
    \brief Music fecther service.

    Music fetcher implements fetching of music uris via service provider API.
*/

// ----------------------------------------------------------------------------
// Constructs the music fetcher.
// ----------------------------------------------------------------------------
//
MusicFetcher::MusicFetcher()
{
    TX_ENTRY
    mService = new GetMusicService(this);
    TX_EXIT
}

// ----------------------------------------------------------------------------
// Destructs the music fetcher.
// ----------------------------------------------------------------------------
//
MusicFetcher::~MusicFetcher()
{
    TX_ENTRY
    delete mService;
    TX_EXIT
}

// ----------------------------------------------------------------------------
// Returns a string to show in music fetcher application title.
// ----------------------------------------------------------------------------
//
QString MusicFetcher::contextTitle() const
{
    return mService->contextTitle();
}

// ----------------------------------------------------------------------------
// Slot to be called when music item has been selected.
// ----------------------------------------------------------------------------
//
void MusicFetcher::itemSelected(QString songName)
{
    TX_ENTRY_ARGS("songName: " << songName);
    QStringList list;
    list.append(songName);
    mService->complete( list );
}

// ----------------------------------------------------------------------------
// Constructs the music service.
// ----------------------------------------------------------------------------
//
GetMusicService::GetMusicService(MusicFetcher* parent)
    : XQServiceProvider(QLatin1String("Music Fetcher.com.nokia.services.media.Music"), parent),
    mRequestIndex(0),
    mServiceApp(parent)
{
    TX_ENTRY
    publishAll();
    TX_EXIT
}

// ----------------------------------------------------------------------------
// Destructs the music service.
// ----------------------------------------------------------------------------
//
GetMusicService::~GetMusicService()
{
    TX_LOG
}

// ----------------------------------------------------------------------------
// Completes the service request.
// ----------------------------------------------------------------------------
//
void GetMusicService::complete(QStringList filesList)
{
    doComplete(filesList);
}

// ----------------------------------------------------------------------------
// Completes the service request.
// ----------------------------------------------------------------------------
//
void GetMusicService::doComplete(QStringList filesList)
{
    if ( isActive() ) {
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        completeRequest(mRequestIndex, filesList);
        mRequestIndex=0;
    }
}

// ----------------------------------------------------------------------------
// Returns title set by service client.
// ----------------------------------------------------------------------------
//
QString GetMusicService::contextTitle() const
{
    return mTitle;
}

// ----------------------------------------------------------------------------
// Returns indication whether service request still active.
// ----------------------------------------------------------------------------
//
bool GetMusicService::isActive()
{
    return mRequestIndex > 0;
}

/*!
    Music Fetcher service interface defined in service registration xml.
    Service client needs to create a request using interface name
    "com.nokia.services.media.Music" and set string to show in
    opened views title as a request argument.

    Example usage:
    \code
    XQApplicationManager appMgr;
    XQAiwRequest* req = appMgr.create("com.nokia.services.media.Music",
        "fetch(QString)", true);

    if (req)
    {
        connect(req, SIGNAL(requestOk(const QVariant&)),
            SLOT(handleRequestOk(const QVariant&)));
        connect(req, SIGNAL(requestError(int,const QString&)),
            SLOT(handleRequestError(int,const QString&)));

        // Set argument for request (title for opened views)
        QList<QVariant> args;
        args << QVariant(QString("<title to show>"));
        req->setArguments(args);

        // Make the request
        if (!req->send())
        {
            qDebug() <<  "Failed to send REQ";
        }
        delete req;
    }
    \endcode
 */
void GetMusicService::fetch(QString title)
{
    TX_ENTRY_ARGS("title: " << title);
    mTitle = title;
    emit mServiceApp->titleReady(title);
    mRequestIndex = setCurrentRequestAsync();
}
