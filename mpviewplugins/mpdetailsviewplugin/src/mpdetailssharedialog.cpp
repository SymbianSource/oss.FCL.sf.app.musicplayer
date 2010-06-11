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
* Description: Implementation for share player.
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
#include <QNetworkReply>
#include <QDesktopServices>
#include <QNetworkProxyFactory>
#include <QTimer>
#include <hbmessagebox.h>
#include <QFile>
#include <QTextStream>
#include <qsysteminfo.h>
#include <hbmainwindow.h>
#include <hbstyleloader.h>
#include <hbprogressdialog.h>

QTM_USE_NAMESPACE

// SHARE_INDEX_FILE defines where the index.html file is loaded from.
#define SHARE_INDEX_URL "qrc:///shareview/index.html"

#ifdef Q_OS_SYMBIAN
// Symbian target.
#ifdef SHARE_PLAYER_RND
// For R&D testing, index.html may be loaded from E: drive.
// User must manually place index.html in the correct location.
// If the RND file does not exist, then SHARE_INDEX_URL will be used.
#define RND_SHARE_INDEX_URL "file:///f:/index.html"
#define RND_SHARE_INDEX_FILE "f:\\index.html"
#define RND_OVI_LOGIN_FILE "f:\\ovicredentials.txt"
#endif
#else
// Assume Windows target.
#define RND_SHARE_INDEX_URL "file:///c:/temp/index.html"
#define RND_SHARE_INDEX_FILE "c:\\temp\\index.html"
#define RND_OVI_LOGIN_FILE "c:\\temp\\ovicredentials.txt"

#endif

// Default language in case QSystemInfo does not work.
#define DEFAULT_LANGUAGE "en-US"

// Default error message.
#define ERROR_MESSAGE "An error occured. Sharing is not currently available"


/*!
 MpNetworkAccessManager allows local caching of publishing player files
 in order to minimize network traffic.
 The files will be cached to the private directory of the application,
 i.e. in the music player's case, this is C:\Private\10207C62\Cache.
 */
class MpNetworkAccessManager : public QNetworkAccessManager
{
public:
    MpNetworkAccessManager( QObject* parent = 0 )
        : QNetworkAccessManager( parent )
    {
        proxyFactory()->setUseSystemConfiguration( true );
        QNetworkDiskCache* diskCache = new QNetworkDiskCache( this );
        QString location = QDesktopServices::storageLocation( QDesktopServices::CacheLocation );
        diskCache->setCacheDirectory( location );
        setCache( diskCache );
    }

private:
    QNetworkReply* createRequest( Operation op,
                                  const QNetworkRequest &request,
                                  QIODevice* outgoingData = 0 )
    {
        TX_ENTRY
        TX_LOG_ARGS( "share: createRequest URL=" << request.url().toString() )

        QVariant val = request.attribute( QNetworkRequest::CacheLoadControlAttribute );

         // Change the cache load control attrbute!
        QNetworkRequest req = request;
        req.setAttribute( QNetworkRequest::CacheLoadControlAttribute,
                          QVariant( QNetworkRequest::PreferCache ) );
        QNetworkReply* result = QNetworkAccessManager::createRequest( op, req, outgoingData );
        TX_EXIT
        return result;
    }
};

/*!
 MpShareWebView derives from QGraphicsWebView in order to override it's
 contextMenuEvent method to prevent the background context menu from
 being displayed when user makes long click in the web view.
 */
class MpShareWebView : public QGraphicsWebView
{
public:
    MpShareWebView( QGraphicsItem * parent = 0 )
        : QGraphicsWebView( parent )
    {
        settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
        settings()->setAttribute( QWebSettings::LocalStorageDatabaseEnabled, true );
        settings()->enablePersistentStorage();
    }

protected:
    void contextMenuEvent( QGraphicsSceneContextMenuEvent* /*ev*/ ) // Override QGraphicsWebView::contextMenuEvent
    {
        // Fix to prevent "Stop" and "Reload" buttons in page background.
        // Do not respond to the contextMenuEvent.
    }
};


/*!
 Constructor.
 */
MpDetailsShareDialog::MpDetailsShareDialog()
    : mShareWebView( 0 ),
      mShareNetAccMan( 0 ),
      mProgressbar ( 0 ),
      mIsInitialized( false )
{
    // DeleteOnClose attribute prevents crash when user presses Cancel
    // before publishing player is fully loaded.
    setAttribute( Qt::WA_DeleteOnClose, true );
}

/*!
 Initialize the share dialog.
 When fully initialized we set our mIsInitialized flag to true.
 Our isInitialized() method can be called to determine whether
 initialization was successful.
 */
void MpDetailsShareDialog::initialize( MpSongData* aSongData, const QString& aUnknownTr )
{
    TX_ENTRY
    if ( !initUser() )
    {
        emit closeShareDialog();
        return;
    }
    initShareData( aSongData, aUnknownTr );
    initLanguage();
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

    // Flag that the dialog is now fully initialized.
    mIsInitialized = true;

    // Show progress dialog in .3 second if loading not finished.
    QTimer::singleShot(300, this, SLOT(showProgressDialog()));

    TX_EXIT
}

/*!
 Destructor.
 */
MpDetailsShareDialog::~MpDetailsShareDialog()
{
    TX_ENTRY
    if ( mShareData.songData() )
    {
        // Ensure that we remove the temporary album art file when we close the dialog.
        // TODO this should be removed when base64 issue is solved.
        TX_LOG_ARGS( "share: remove album art file" )
        mShareData.songData()->removeAlbumArtFile();
    }
    logoutPlayer();
    // Probably mShareNetAccMan should not be deleted but qt documentation
    // does not indicate whether QWebPage takes ownership of the object or not.
    // See http://doc.qt.nokia.com/4.6/qwebpage.html
    //delete mShareNetAccMan;
    TX_EXIT
}

/*!
 Initialize share data.
 */
void MpDetailsShareDialog::initShareData( MpSongData* aSongData, const QString& aUnknownTr )
{
    TX_ENTRY
    // Set information for the share data.
    mShareData.setOwner( this );
    mShareData.setSongData( aSongData );
    mShareData.setErrorMessage( tr( ERROR_MESSAGE ) );
    mShareData.setUnknownTr( aUnknownTr );
    TX_EXIT
}

/*!
 Initialize language.
 Language string is formatted like "en-US", where "en" is the ISO-639-1 language code,
 and "US" is the ISO-3166-1 country code.
 We use the QT Mobility API (systeminfo) to obtain the settings from the device.
 In the event that we cannot construct the QSystemInfo variable then we will fallback
 to some DEFAULT_LANGUAGE setting.
 */
void MpDetailsShareDialog::initLanguage()
{
    TX_ENTRY
    // Set language string, example "en-US".
    QString language;
    QSystemInfo* sysInfo = new QSystemInfo( this );
    if ( sysInfo )
    {
        language += sysInfo->currentLanguage(); // ISO-639-1 language code.
        language += "-";
        language += sysInfo->currentCountryCode(); // ISO-3166-1 country code.
        delete sysInfo;
    }
    else
    {
        // Fallback to the default language.
        language = DEFAULT_LANGUAGE;
    }
    TX_LOG_ARGS( "share: language '" << language << "'" )
    mShareData.setLanguage( language );
    TX_EXIT
}

/*!
 Initialize network access manager.
 */
void MpDetailsShareDialog::initNetworkAccessManager()
{
    TX_ENTRY
    // Make our own network access manager to allow file retrieval from local cache,
    // since configuration for the default network access manager seems to be
    // to always redownload from network.
    if ( !mShareNetAccMan )
    {
        TX_LOG_ARGS( "share: construct network access manager" )
        mShareNetAccMan = new MpNetworkAccessManager( this );
    }
    TX_EXIT
}

/*!
 Initialize the web view.
 */
void MpDetailsShareDialog::initWebView()
{
    TX_ENTRY
    if ( !mShareWebView )
    {
        TX_LOG_ARGS( "share: construct share web view" )
        mShareWebView = new MpShareWebView( this );
        mShareWebView->page()->setNetworkAccessManager( mShareNetAccMan );
        mShareWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
        mShareWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
        setContentWidget( mShareWebView );
        addContext();
        mProgressbar = new HbProgressDialog(HbProgressDialog::WaitDialog);
        mProgressbar->setText(tr("Loading"));
    }
    TX_EXIT
}

/*!
 Initialize signals and slots.
 */
void MpDetailsShareDialog::initSignalSlots()
{
    TX_ENTRY
    // Connect various signals to slots for networking.
    connect( mShareNetAccMan, SIGNAL( sslErrors( QNetworkReply*, const QList< QSslError >& ) ),
             this, SLOT( handleRequestSSLErrors( QNetworkReply*, const QList< QSslError >& ) ) );
    connect( mShareNetAccMan, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( handleRequestFinished( QNetworkReply* ) ) );

    // Connect various signals to slots for interface to webview.
    connect( mShareWebView, SIGNAL( loadFinished( bool ) ), SLOT( onIndexLoad( bool ) ) );
    connect( mShareWebView->page()->mainFrame(), SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addContext() ) );
    connect( mShareWebView->page(), SIGNAL( windowCloseRequested() ), this, SIGNAL( closeShareDialog() ) );
    connect( mProgressbar, SIGNAL(cancelled()), this, SIGNAL( closeShareDialog() ) );
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
        // There must be e:ovicredentials.txt in the filesystem but don't show error dialog
        // otherwise it will appear as soon as detailsview is created.
        result = false;
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
 Returns true if the dialog has been fully initialized.
  */
bool MpDetailsShareDialog::isInitialized() const
{
    return mIsInitialized;
}

/*!
 Attempt to cache the publishing player files from internet
 to improve user experience for first-time use.
 If the files are already in the cache and have not expired,
 then this should not do anything.
 */
void MpDetailsShareDialog::cachePublishingPlayerFiles()
{
    TX_ENTRY
    // We need the network access manager, so make sure it is initialized.
    if ( !mShareNetAccMan )
    {
        initNetworkAccessManager();
    }
    // Attempt to get the required publishing player files from the net in advance.
    // We don't listen to any signal that the download succeeded or failed
    // since we will let it silently fail at this stage.
    // These URLs are also included in index.html resource, and the two must
    // be kept the same.
    mShareNetAccMan->get( QNetworkRequest( QUrl(
            "http://hf.ci.wipsl.com/PleiXXPTsup/noheva-be/css/ovi.player.share.ui.css" ) ) );
    mShareNetAccMan->get( QNetworkRequest( QUrl(
            "http://hf.ci.wipsl.com/PleiXXPTsup/noheva-be/js/publishplayer.js" ) ) );
    TX_EXIT
}

/*!
 Release resources from share player.
 */
void MpDetailsShareDialog::logoutPlayer()
{
    TX_ENTRY
    if ( mShareWebView )
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
    if ( mShareWebView )
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
    if ( mShareWebView )
    {
        // We don't need to call updateContextArea when the track URL has been updated.
        //mShareWebView->page()->mainFrame()->evaluateJavaScript( "music.updateContextArea();" );
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
void MpDetailsShareDialog::errorHandler( QString aError, QString aMessage )
{
    TX_ENTRY
    // If error argument ends with "_SUCCESS", then this should be an info message.
    // If error argument ends with "_FAILED" or something else, then this should be a warning message.
    TX_LOG_ARGS( "share: errorHandler: " << aError << ": " << aMessage )
    if ( aError.endsWith( "_SUCCESS" ) )
    {
        // TODO this method seems to be deprecated?
        HbMessageBox::information( tr( "%1" ).arg( aError ) + ": " + tr( "%1" ).arg( aMessage ) ); // For week16 hbwidgets
    }
    else
    {
        // TODO this method seems to be deprecated?
        HbMessageBox::warning( tr( "%1" ).arg( aError ) + ": " + tr( "%1" ).arg( aMessage ) ); // For week16 hbwidgets
        // HbMessageBox::launchWarningMessageBox( tr( "%1" ).arg( aError ) + ": " + tr( "%1" ).arg( message ) ); // For week12 hbwidgets
    }
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

/*!
 Slot to show the publishing window after html elements are created in javascript.
 */
void MpDetailsShareDialog::showWindow()
{
    TX_ENTRY
    if (mProgressbar)
        mProgressbar->close();
    show();
    TX_EXIT
}

/*!
 Slot to show progress dialog if the publishing window is not loaded in .3 sec.
 */
void MpDetailsShareDialog::showProgressDialog()
{
    TX_ENTRY
    if (!isVisible())
        mProgressbar->show();
    TX_EXIT
}

/*!
 Slot to SSL errors in network request. We will ignore any errors.
 */
void MpDetailsShareDialog::handleRequestSSLErrors( QNetworkReply* aReply, const QList< QSslError >& aErrors )
{
    TX_ENTRY
    aReply->ignoreSslErrors();
    for( int i = 0; i < aErrors.count(); i++ )
    {
        TX_LOG_ARGS( "SSL error " << aErrors.at( i ).errorString() );
    }
    TX_EXIT
}

/*!
 Slot to handle network request completion.
 */
void MpDetailsShareDialog::handleRequestFinished( QNetworkReply* aReply )
{
    TX_ENTRY
    if ( aReply->error() != QNetworkReply::NoError )
    {
        TX_LOG_ARGS( "Network request error " << aReply->error() << aReply->errorString() );
        // TODO what to do now?
    }
    TX_EXIT
}

#endif // SHARE_FUNC_ENABLED
