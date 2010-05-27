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
#ifdef SHARE_FUNC_ENABLED

#include "mpdetailssharedialog.h"
#include "mpsharedata.h"
#include "mpsongdata.h"
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
#include <hbmessagebox.h>
#include <QFile>
#include <QTextStream>


// SHARE_INDEX_FILE defines where the index.html file is loaded from.
#define SHARE_INDEX_URL "qrc://shareview/index.html"

#ifdef Q_OS_SYMBIAN
// Symbian target.
#ifdef SHARE_PLAYER_RND
// For R&D testing, index.html may be loaded from E: drive.
// User must manually place index.html in the correct location.
// If the RND file does not exist, then SHARE_INDEX_URL will be used.
#define RND_SHARE_INDEX_URL "file://e:/index.html"
#define RND_SHARE_INDEX_FILE "e:\\index.html"
#define RND_OVI_LOGIN_FILE "e:\\ovicredentials.txt"
#define RND_DEFAULT_USER "hipsu"
#define RND_DEFAULT_PASS "hipsupass"
#endif
#else
// Assume Windows target.
#define RND_SHARE_INDEX_URL "file://c:/temp/index.html"
#define RND_SHARE_INDEX_FILE "c:\\temp\\index.html"
#define RND_OVI_LOGIN_FILE "c:\\temp\\ovicredentials.txt"

#endif

// For allowing local caching of javascript files loaded by popup
class MpNetworkAccessManager : public QNetworkAccessManager
{
public:
    MpNetworkAccessManager() { };

private:
    QNetworkReply *createRequest( Operation op,
                                 const QNetworkRequest &request,
                                 QIODevice *outgoingData = 0 )
    {
        TX_ENTRY
        QVariant val = request.attribute( QNetworkRequest::CacheLoadControlAttribute );
        qDebug() << "request cachecontrol - " << val.typeName() << val.toInt();

         // Change the cache load control attrbute!
        QNetworkRequest req = request;
        req.setAttribute( QNetworkRequest::CacheLoadControlAttribute,
                           QVariant( QNetworkRequest::PreferCache ) ); // was PreferNetwork
        TX_EXIT
        return QNetworkAccessManager::createRequest( op, req, outgoingData );
    }
};


/*!
 Constructor.
 */
MpDetailsShareDialog::MpDetailsShareDialog()
    : mShareWebView( 0 ),
      mShareNetAccMan( 0 )
{
}

void MpDetailsShareDialog::initialize( MpSongData* aSongData )
{
    TX_ENTRY
    if ( !initUser() )
    {
        emit closeShareDialog();
        return;
    }
    initShareData( aSongData );
    initNetworkAccessManager();
    initWebView();
    initSignalSlots();

    setDismissPolicy( HbDialog::NoDismiss );

    // No timeout needed for the dialog.
    setTimeout( HbPopup::NoTimeout );

#ifdef SHARE_PLAYER_RND
    // Test whether the RND file exists.
    QFile file( RND_SHARE_INDEX_FILE );
    if ( file.exists() )
    {
        // Load the RND URL from the specified location to the web view.
        TX_LOG_ARGS( "share: Use RND index.html file " << RND_SHARE_INDEX_FILE )
        mShareWebView->load( QUrl( RND_SHARE_INDEX_URL ) );
    }
    else
#endif
    {
        // Load the production URL from the application resources to the web view.
        TX_LOG_ARGS( "share: Use QRC index.html file " << SHARE_INDEX_URL )
        mShareWebView->load( QUrl( SHARE_INDEX_URL ) );
    }
    TX_EXIT
}


/*!
 Destructor.
 */
MpDetailsShareDialog::~MpDetailsShareDialog()
{
    TX_ENTRY
    logoutPlayer();
    delete mShareNetAccMan;
    TX_EXIT
}

void MpDetailsShareDialog::initShareData( MpSongData* aSongData )
{
    TX_ENTRY
    // Set information for the share data.
    mShareData.setOwner( this );
    mShareData.setSongData( aSongData );
    // TODO need localized error message.
    mShareData.setErrorMessage( "An error occured. Sharing is not currently available" );
    // TODO need language setting.
    mShareData.setLanguage( "en" );
    TX_EXIT
}

void MpDetailsShareDialog::initNetworkAccessManager()
{
    TX_ENTRY
    // Make our own network access manager to allow JS retrieval from local cache,
    // since configuration for the default network access manager seems to be
    // to always redownload from network.
    mShareNetAccMan = new MpNetworkAccessManager();
    mShareNetAccMan->proxyFactory()->setUseSystemConfiguration( true );
    QNetworkDiskCache* diskCache = new QNetworkDiskCache( this );
    QString location = QDesktopServices::storageLocation( QDesktopServices::CacheLocation );
    diskCache->setCacheDirectory( location );
    mShareNetAccMan->setCache( diskCache );
    TX_EXIT
}

void MpDetailsShareDialog::initWebView()
{
    TX_ENTRY
    mShareWebView = new QGraphicsWebView();
    mShareWebView->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
    mShareWebView->settings()->setAttribute( QWebSettings::LocalStorageDatabaseEnabled, true );
    mShareWebView->settings()->enablePersistentStorage();
    mShareWebView->page()->setNetworkAccessManager( mShareNetAccMan );
    setContentWidget( mShareWebView );
    TX_EXIT
}

void MpDetailsShareDialog::initSignalSlots()
{
    TX_ENTRY
    // Connect various signals to slots for interface to webview.
    connect( mShareWebView, SIGNAL( loadFinished( bool ) ), SLOT( onIndexLoad( bool ) ) );
    connect( mShareWebView->page()->mainFrame(), SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addContext() ) );
    connect( mShareWebView->page(), SIGNAL( windowCloseRequested() ), this, SIGNAL( closeShareDialog() ) );
    TX_EXIT
}

/*!
 initUser is used as temporary solution until Single Sign On is implemented in platform.
 */
bool MpDetailsShareDialog::initUser()
{
    TX_ENTRY
    bool result = false;
#ifdef SHARE_PLAYER_RND
    // ovicredentials.txt is used as temporary solution until Single Sign On is implemented in platform.
    QFile file( RND_OVI_LOGIN_FILE );
    if ( !file.open( QFile::ReadOnly ) )
    {
//        errorHandler( "share", QString( RND_OVI_LOGIN_FILE ) + " missing" );
        mShareData.setUsername( RND_DEFAULT_USER );
        mShareData.setPassword( RND_DEFAULT_PASS );
        result = true;
    }
    else
    {
        QTextStream stream ( &file );
        QString strCred = stream.readLine( 0 );
        file.close();
        QStringList slCred = strCred.split( ":" );
        if ( slCred.length() > 1 )
        {
            mShareData.setUsername( slCred[ 0 ] );
            mShareData.setPassword( slCred[ 1 ] );
            result = true;
        }
        else
        {
            errorHandler( "share", QString( RND_OVI_LOGIN_FILE ) + " username:password expected" );
        }
    }
    TX_LOG_ARGS( "share: credentials " << mShareData.username() << " / " << mShareData.password() )
#else
    // TODO: Single Sign On stuff.
#endif // SHARE_PLAYER_RND
    TX_EXIT
    return result;
}

/*!
 Release resources from share player.
 */
void MpDetailsShareDialog::logoutPlayer()
{
    TX_ENTRY
    if (mShareWebView)
    {
        mShareWebView->page()->mainFrame()->evaluateJavaScript( "music.teardown();" );
    }
    TX_EXIT
}

/*!
 Adds the shared data context to the javascript of the loaded page.
 */
void MpDetailsShareDialog::addContext()
{
    TX_ENTRY
    if (mShareWebView)
    {
        mShareWebView->page()->mainFrame()->addToJavaScriptWindowObject( "context", &mShareData );
    }
    TX_EXIT
}

/*!
 Updates the shared data context in the javascript of the loaded page.
 */
void MpDetailsShareDialog::updateSharedData()
{
    TX_ENTRY
    if (mShareWebView)
    {
        mShareWebView->page()->mainFrame()->evaluateJavaScript( "music.updateContextArea();" );
        mShareWebView->page()->mainFrame()->evaluateJavaScript( "music.updateMetadata();" );
    }
    TX_EXIT
}

/*!
 Slot to call when index.html loading completes.
 */
void MpDetailsShareDialog::onIndexLoad( bool aOk )
{
    TX_ENTRY
    if ( !aOk )
    {
        // Close the popup window, failed to load index.html.
        // This is pretty serious and most likely unrecoverable error.
        // Only thing we can do really is to close the share player
        // dialog - TODO do we need to show any error message to user?
        TX_LOG_ARGS( "share: failed to load index.html" )
        emit closeShareDialog();
    }
    TX_EXIT
}

/*!
 Slot to call for debug output.
 */
void MpDetailsShareDialog::debugJs( QString s )
{
    TX_ENTRY
    TX_LOG_ARGS( "share: debugJs: " << s )
    TX_EXIT
}

/*!
 Slot to call for displaying an error message to the user.
 */
void MpDetailsShareDialog::errorHandler( QString error, QString message )
{
    TX_ENTRY
    TX_LOG_ARGS( "share: errorHandler: " << error << ": " << message )
    HbMessageBox::warning( error + ": " + message ); // For week16 hbwidgets
//    HbMessageBox::launchWarningMessageBox( error + ": " + message ); // For week12 hbwidgets
    TX_EXIT
}

/*!
 Slot to call to clear the web view cache.
 */
void MpDetailsShareDialog::clearCache()
{
    TX_ENTRY
    QAbstractNetworkCache* cache = mShareNetAccMan ? mShareNetAccMan->cache() : NULL;
    if ( cache )
    {
        TX_LOG_ARGS( "share: clearCache: clearing cache" )
        cache->clear();
#ifdef SHARE_PLAYER_RND
        errorHandler( "Cache", "Cleared cache!" );
#endif
    }
    else
    {
        TX_LOG_ARGS( "share: clearCache: unable to clear cache" )
#ifdef SHARE_PLAYER_RND
        errorHandler( "Cache", "Could not clear cache!" );
#endif
    }
    TX_EXIT
}

#endif // SHARE_FUNC_ENABLED
