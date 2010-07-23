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

#include "mpdetailssharedialog.h"
#include "mptrace.h"
#include <QObject>
#include <QGraphicsWebView>
#include <QGraphicsScene>
#include <QWebPage>
#include <QWebFrame>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QNetworkProxyFactory>


// For allowing local caching of javascript files loaded by popup
class MpNetworkAccessManager : public QNetworkAccessManager
{
public:
    MpNetworkAccessManager() { };

private:
    QNetworkReply *createRequest( Operation /*op*/,
                                 const QNetworkRequest &/*request*/,
                                 QIODevice */*outgoingData = 0*/ ) { return 0; }
};


MpDetailsShareDialog::MpDetailsShareDialog(  )
    : mSharedData(  ),
      mShareWebView( 0 ),
      mShareNetAccMan( 0 )
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

MpDetailsShareDialog::~MpDetailsShareDialog()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::initialize( MpSongData* aSongData )
{
    Q_UNUSED(aSongData)
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::updateSharedData()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::reload()
{
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}

void MpDetailsShareDialog::onIndexLoad( bool aOk )
{
    Q_UNUSED(aOk)
    TX_ENTRY_ARGS("STUB")
    TX_EXIT_ARGS("STUB")
}
