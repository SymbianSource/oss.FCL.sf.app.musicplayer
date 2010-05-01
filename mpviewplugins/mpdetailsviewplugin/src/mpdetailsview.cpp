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
#include <qgraphicswebview>
#include <qwebsettings>
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>
#include <QUrl>
#include <QSslError>
#include <QDomElement>
#include <QWebPage>
#include <QWebFrame>
#include <QList>
#include <QFile>
#include <QTranslator>
#include <QLocale>
#include <QGraphicsLinearLayout>

#include <thumbnailmanager_qt.h>
#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hbpopup.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>

#include "mpdetailsview.h"
#include "mpcommondefs.h"
#include "mpmpxdetailsframeworkwrapper.h"
#include "mpsongdata.h"
#include "mptrace.h"

const int KUndefined = -1;
const int KRecommendationNum = 2;

/*!
 Constructor
 */
MpDetailsView::MpDetailsView()
    : mSongData( 0 ),
      mFrameworkWrapper( 0 ),
      mActivated( false ),
      mWindow( 0 ),
      mNavigationBack( 0 ),
      mSongText( NULL ),
      mAlbumText( NULL ),
      mArtistText( NULL ),
      mAlbumArt( NULL ),
      mDocumentLoader( NULL ),
      mDownloadedAlbumArts( 0 ),
      mAlbumArtsReadyCount( 0 ),
      mMpTranslator( 0 ),
      mCommonTranslator( 0 )
{
    TX_ENTRY
    bool widgetsOk = false;
    HbMainWindow *mainWindow = hbInstance->allMainWindows()[0];
    mDocumentLoader = new HbDocumentLoader();

    if ( mDocumentLoader ) {
            mDocumentLoader->load( QString(":/detailsviewdocml/detailsview.docml"), &widgetsOk );
        }
    if ( widgetsOk ) {
        QGraphicsWidget *widget; 
        
        widget = mDocumentLoader->findWidget( QString("content") );
        mContainer = qobject_cast<HbWidget *>(widget);
        if ( mContainer ) {
            setWidget( mContainer );
        }
        
     widget = mDocumentLoader->findWidget( QString("shareButton") );
     mShareButton = qobject_cast<HbPushButton *>(widget);         
        
     widget = mDocumentLoader->findWidget( QString("songText") );
     mSongText = qobject_cast<HbLabel *>(widget);
    
     widget = mDocumentLoader->findWidget( QString("artistText") );
     mArtistText = qobject_cast<HbLabel *>(widget);
    
     widget = mDocumentLoader->findWidget( QString("albumText") );
     mAlbumText = qobject_cast<HbLabel *>(widget);

     widget = mDocumentLoader->findWidget( QString("albumArt") );
     mAlbumArt = qobject_cast<HbLabel *>(widget);
        
     widget = mDocumentLoader->findWidget( QString("songDetailsGroupBox") );
     mSongDetailsGroupBox = qobject_cast<HbGroupBox *>(widget);
        
     widget = mDocumentLoader->findWidget( QString("inspireMeGroupBox") );
     mInspireMeGroupBox = qobject_cast<HbGroupBox *>(widget);
    }
    else {
        TX_LOG_ARGS( "Error: invalid detailsview.docml" );
    }
    TX_EXIT
}

/*!
 Destructs the details view.
 */
MpDetailsView::~MpDetailsView()
{
    TX_ENTRY    
    if ( mFrameworkWrapper ) {
        delete mFrameworkWrapper;
    }
    if ( mNavigationBack ) {
        delete mNavigationBack;
    }
    if ( mDocumentLoader ) {
        delete mDocumentLoader;
    }
    if ( mManager ) {
        mManager->deleteLater();
    }
    if ( mDownloadManager ) {
        mDownloadManager->deleteLater();
    }    
    if( mThumbnailManager ) {
        delete mThumbnailManager;
    }
    delete mMpTranslator;
    delete mCommonTranslator;
    TX_EXIT
}

/*!
 Initializes the details view. Allocates all resources needed by the view.
 */
void MpDetailsView::initializeView()
{
    TX_ENTRY    
    
    //Load musicplayer and common translators
        QString lang = QLocale::system().name();
        QString path = QString( "z:/resource/qt/translations/" );
        bool translatorLoaded = false;

        mMpTranslator = new QTranslator( this );
        translatorLoaded = mMpTranslator->load( path + "musicplayer_" + lang );
        TX_LOG_ARGS( "Loading translator ok=" << translatorLoaded );
        if ( translatorLoaded ) {
            qApp->installTranslator( mMpTranslator );
        }

        mCommonTranslator = new QTranslator( this );
        translatorLoaded = mCommonTranslator->load( path + "common_" + lang );
        TX_LOG_ARGS( "Loading common translator ok=" << translatorLoaded );
        if ( translatorLoaded ) {
            qApp->installTranslator( mCommonTranslator );
        }
        
    mWindow = mainWindow();
    mNavigationBack = new HbAction( Hb::BackNaviAction, this );
    mFrameworkWrapper = new MpMpxDetailsFrameworkWrapper( );
    mSongData = mFrameworkWrapper->songData();
    
    mCompositePixmap = QPixmap( 150, 150 );
    
    mDetailList = new HbListWidget( mSongDetailsGroupBox );
    mButton = new HbPushButton( tr( "More recommendations" ) );
    mSongDetailsGroupBox->setCollapsable( true );
    mSongDetailsGroupBox->setHeading( tr("Song details") );
    mSongDetailsGroupBox->setContentWidget( mDetailList );
    mSongDetailsGroupBox->setCollapsed( true );

    mLayout = new QGraphicsLinearLayout( Qt::Vertical );
    mInspireMe = new HbWidget( mInspireMeGroupBox );
    mInspireList = new HbListWidget();
    mInspireMeGroupBox->setCollapsable( true );   
    mInspireMeGroupBox->setHeading( tr("Inspire me") );
    mInspireMeGroupBox->setContentWidget( mInspireMe ); 
    

    mManager = new QNetworkAccessManager( this );
    connect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );
    
    mDownloadManager = new QNetworkAccessManager( this );
    connect( mDownloadManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( DownloadFinished( QNetworkReply * ) ) );
    
    mRecommendationAlbumArtsName << "C:\\Data\\albumOne.png" << "C:\\Data\\albumTwo.png"; 
    
    mThumbnailManager = new ThumbnailManager( this );
    mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForQuality );
    mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailSmall );
    QObject::connect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ),
            this, SLOT( thumbnailReady( QPixmap , void * , int , int  ) ) );
    mDefaultRecommendationAlbumArt = QPixmap( ":/detailsviewicons/defaultalbumart.png" );
    
    // TODO: might need later
    setupMenu();
      
    connect( mNavigationBack, SIGNAL( triggered() ), this, SLOT( back() ) );
    connect( mShareButton, SIGNAL( clicked() ), this, SLOT( share() ) );    
    connect( mSongData, SIGNAL( albumArtReady() ), this, SLOT( albumArtChanged() ) ); 
    connect( mSongData, SIGNAL( playbackInfoChanged() ), this, SLOT( playbackInfoChanged() ) );
    connect( mSongData, SIGNAL( songDetailInfoChanged() ), this, SLOT( songDetailInfoChanged() ) ); 
    connect( mSongDetailsGroupBox, SIGNAL( toggled( bool ) ), this, SLOT( toggleInspireMeGroupBox( bool ) ) );
    connect( mInspireMeGroupBox, SIGNAL( toggled( bool ) ), this, SLOT( toggleDetailsGroupBox( bool ) ) );
    TX_EXIT
}

/*!
 Activates the details view.
 */
void MpDetailsView::activateView()
{
    TX_ENTRY    
    setNavigationAction( mNavigationBack );
    mFrameworkWrapper->retrieveSong();
        
    mActivated = true;
    TX_EXIT
}

/*!
 Deactivates the details view.
 */
void MpDetailsView::deactivateView()
{
    TX_ENTRY    
    setNavigationAction( 0 );
    mActivated = false;    
    TX_EXIT
}

/*!
 Setup the menu.
 */
void MpDetailsView::setupMenu()
{
    TX_ENTRY

    TX_EXIT
}

/*!
 Slot to handle back command from softkey.

 \reimp
 */
void MpDetailsView::back()
{
    TX_ENTRY  
    emit command( MpCommon::ActivatePlaybackView );
    TX_EXIT
}

/*!
 Slot to be called when share button is clicked
 */
void MpDetailsView::share()
{
    TX_ENTRY  
    TX_EXIT    
}

/*!
 Slot to handle Album art changed.
 */
void MpDetailsView::albumArtChanged( )
{    
    TX_ENTRY
    QPixmap pixmap;
    QIcon qicon;
    mSongData->albumArt( pixmap );
    composeAlbumCover( pixmap );
    if ( !mCompositePixmap.isNull() )
        qicon = QIcon( mCompositePixmap );
    else
        qicon = QIcon( pixmap );
    HbIcon icon( qicon );  
    mAlbumArt->setIcon( icon );
    TX_EXIT
}

void MpDetailsView::loadSharePlayer()
{
    TX_ENTRY       
    TX_EXIT    
}


/*!
 Slot to call when widget is loaded
 */
void MpDetailsView::webViewLoaded( bool ok )
{
    TX_ENTRY    
    if ( ok ) {
        TX_LOG_ARGS( "Loading web page successfully." );

    } else {
        TX_LOG_ARGS( "Loading web page failed!" );
    }
    TX_EXIT    
}


/*!
 Compose the album art.
 */
void MpDetailsView::composeAlbumCover( QPixmap albumart )
{
    TX_ENTRY
    mCompositePixmap.fill( Qt::transparent );
    QPainter painter( &mCompositePixmap );
    painter.setCompositionMode( QPainter::CompositionMode_Clear );
    painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
    painter.fillRect( mCompositePixmap.rect(), Qt::transparent );
    painter.drawPixmap( QRect( 0, 0, 150, 150 ), albumart );
    TX_EXIT
}

/*!
 Make a key & value pair string for querying
 */
QString MpDetailsView::keyValues( QStringList keys, QStringList values ) const
{
    TX_ENTRY
    QString str;
    if ( keys.length() != values.length() ) {
        TX_LOG_ARGS( "Error: keys length is not equal to values length" ); 
    } else {
        for ( int i = 0; i < keys.length(); i++ ) {
            QString tValue = values.at( i );
            if ( 0 != tValue.length() )
            {
                str += keys.at( i ) + "=" + values.at( i ) + "&";
            }
        }
    }
    TX_EXIT
    return str.left( str.length() - 1 );
}

/*!
 Find the most suitable link based on Atom response from Ovi music server
 */
void MpDetailsView::handleParsedXML()
{
    TX_ENTRY
    QDomElement rootElement = mDomDocument.documentElement();
    
    if ( rootElement.attribute( "type" ) == tr( "search" ) ) {
        TX_LOG_ARGS( "URI response" )
        QString result;
        QDomElement entry = rootElement.firstChildElement( "entry" );
        while ( !entry.isNull() )
        {
            if ( entry.attribute( "type" ) == tr( "musictrack" ) ) {
                QDomElement link = entry.firstChildElement( "link" );
                while ( !link.isNull() )
                {
                    if ( link.attribute( "rel" ) == tr( "alternate" )
                        && link.attribute( "type" ) == tr( "text/html" ) ) {
                        result = link.attribute( "href" );
                    }
                    link = link.nextSiblingElement( "link" );
                }
            }
            entry = entry.nextSiblingElement( "entry" );
        }  
        
        mSongData->setLink( result );
    } else if ( rootElement.attribute( "type" ) == tr( "recommendedTracks" ) ) {
        TX_LOG_ARGS( "Recommendation response" )
        QDomElement entry = rootElement.firstChildElement( "entry" );
        QNetworkReply *reply;
        int count = 0;
        while ( !entry.isNull() && count < KRecommendationNum )
        {
            if ( entry.attribute( "type" ) == tr( "musictrack" ) ) {
                QDomElement link = entry.firstChildElement( "link" );
                while ( !link.isNull() )
                {
                    if ( link.attribute( "title" ) == tr( "albumart100" ) ) {
                        mRecommendationAlbumArtsLink.append( link.attribute( "href" ) );
                        break;
                    } else {
                        link = link.nextSiblingElement( "link" );
                    }                    
                }
                QDomElement metadata = entry.firstChildElement( "metadata" );
                mRecommendationSongs.append( metadata.firstChildElement( "name" ).text() );
                mRecommendationArtists.append( metadata.firstChildElement( "primaryartist" ).text() );
                count++;
            }
            entry = entry.nextSiblingElement( "entry" );
        }          
        
        for (int i = 0; i < KRecommendationNum; i++ ) {
            TX_LOG_ARGS( "song name: " << mRecommendationSongs.at(i) );
            TX_LOG_ARGS( "Artist name: " << mRecommendationArtists.at(i) );
            TX_LOG_ARGS( "Album art link: " << mRecommendationAlbumArtsLink.at(i) );
            
            if ( mRecommendationAlbumArtsLink.at( i ).contains( "http", Qt::CaseInsensitive ) ) {
                reply = mDownloadManager->get( QNetworkRequest( QUrl( mRecommendationAlbumArtsLink.at(i) ) ) );
                mReplys.append( reply );
                connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );
                connect( reply, SIGNAL( sslErrors( QList<QSslError> ) ), this, SLOT( retrieveInformationSslErrors( QList<QSslError> ) ) );
            }             
        }
    } else {
        TX_LOG_ARGS( "Not supported response" )
    }
    TX_EXIT
}

/*!
 Sets recommendation album art
*/
void MpDetailsView::setAlbumArtUri( const QString &albumArtUri, const QString &albumArtName )
{
    TX_ENTRY_ARGS( "albumArtUri = " << albumArtUri )
    TX_LOG_ARGS( "albumArtName = " << albumArtName )
    if ( !albumArtUri.isEmpty() ) {
        int id = mThumbnailManager->getThumbnail( albumArtName, reinterpret_cast<void *>( const_cast<QString *>( &albumArtUri ) ) );
        if ( id == KUndefined ) {
            // Request failed. Set default album art.
            mRecommendationAlbumArtsMap.insert( albumArtUri, mDefaultRecommendationAlbumArt );
            recommendationAlbumArtReady();
        }
    }
    else {
        // No album art uri. Set default album art.
        mRecommendationAlbumArtsMap.insert( albumArtUri, mDefaultRecommendationAlbumArt );
        recommendationAlbumArtReady();
    }
    TX_EXIT
}

/*!
 Render inspireme groupbox after album arts downloaded
 */
void MpDetailsView::RenderInspireMeGroupBox() 
{
    TX_ENTRY
    for ( int i = 0; i < KRecommendationNum; i++ ) {
       HbListWidgetItem  *item = new HbListWidgetItem();       
       HbIcon icon( QIcon( mRecommendationAlbumArtsMap.value( mRecommendationAlbumArtsLink.at( i ) ) ) );
       item->setIcon( icon );
       item->setText( mRecommendationSongs.at( i ) );
       item->setSecondaryText( mRecommendationArtists.at( i ) );

       mInspireList->addItem( item );       
    }
    
    // TODO: HbListWidget has some problem to return the correct height, hard code to 180 for now.
    TX_LOG_ARGS( "height = " << mInspireList->geometry().height() );
    TX_LOG_ARGS( "height2 = " << mInspireList->size().height() );
    mInspireList->setMinimumHeight( 180 );
    mInspireList->setMaximumHeight( 180 );    
    
    mLayout->addItem( mInspireList );
    mButton->show();
    mLayout->addItem( mButton );
    mInspireMe->setLayout( mLayout );
    
    TX_EXIT
}

void MpDetailsView::recommendationAlbumArtReady()
{   
    TX_ENTRY_ARGS( "mAlbumArtsReadyCount = " << mAlbumArtsReadyCount )
    mAlbumArtsReadyCount++;
    if ( mAlbumArtsReadyCount == KRecommendationNum ) {
        RenderInspireMeGroupBox();
    }
    TX_EXIT
}


/*!
 Slot to handle basic song information
 */
void MpDetailsView::playbackInfoChanged()
{
    TX_ENTRY
    mSongText->setPlainText( mSongData->title() );
    mAlbumText->setPlainText( mSongData->album() );
    mArtistText->setPlainText( mSongData->artist() );
    
    // Clear information & Remove album arts downloaded previously when song changes
    TX_LOG_ARGS( "Reply count = " << mReplys.count() );
    for ( int i = 0; i < mReplys.count(); i++ ) {
        QNetworkReply *reply = mReplys.at( i );
        if ( reply != NULL ) {
            TX_LOG_ARGS( "Reply index : " << i );
            reply->close();
            delete reply;
            reply = NULL;
        }   
    }
    mReplys.clear();
    
    mDownloadedAlbumArts = 0;
    mAlbumArtsReadyCount = 0;
    mInspireList->clear();
    for ( int i = 0; i < KRecommendationNum; i++) {
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
    mButton->hide();
        
    // TODO: country information handling, MCC
    QString queryURI("http://api.music.ovi.com/1.0/ru/?");
    constructRequest( queryURI );
    TX_LOG_ARGS( "queryURI : " << queryURI );
    retrieveInformation( queryURI );
    
    QString queryRecommendation("http://api.music.ovi.com/1.0/gb/releases/recommend/?");
    constructRequest( queryRecommendation );
    TX_LOG_ARGS( "queryRecommendation : " << queryRecommendation );
    retrieveInformation( queryRecommendation );
    
    loadSharePlayer();
    TX_EXIT
}

/*!
 Slot to handle detail song information
 */
void MpDetailsView::songDetailInfoChanged()
{
    TX_ENTRY
    mDetailList->clear();
    
    HbListWidgetItem *item = new HbListWidgetItem();  
    item->setText( tr( "Track" ) );
    item->setSecondaryText( mSongData->albumTrack() );
    mDetailList->addItem( item );
    
    item = new HbListWidgetItem();
    item->setText( tr( "Composer" ) );
    item->setSecondaryText( mSongData->composer() );
    mDetailList->addItem( item );

    item = new HbListWidgetItem();
    item->setText( tr( "Year" ) );
    item->setSecondaryText( mSongData->year() );
    mDetailList->addItem( item );
    
    item = new HbListWidgetItem();
    item->setText( tr( "Genre" ) );
    item->setSecondaryText( mSongData->genre() );
    mDetailList->addItem( item );

    item = new HbListWidgetItem();
    item->setText( tr( "Comment" ) );
    item->setSecondaryText( mSongData->comment() );
    mDetailList->addItem( item );    
    TX_EXIT
}

/*!
 Slot to handle details groupbox toggling
 */
void MpDetailsView::toggleDetailsGroupBox(bool /*state*/)
{
    TX_ENTRY
    if ( !mInspireMeGroupBox->isCollapsed() ) {
        mSongDetailsGroupBox->setCollapsed( true );
    }
    TX_EXIT
}

/*!
 Slot to handle inspire me groupbox toggling
 */
void MpDetailsView::toggleInspireMeGroupBox(bool /*state*/)
{
    TX_ENTRY
    if ( !mSongDetailsGroupBox->isCollapsed() ) {
        mInspireMeGroupBox->setCollapsed( true );
    }
    TX_EXIT
}

/*!
 Construct the query for fetching URI & recommendations
 */
void MpDetailsView::constructRequest( QString &uri )
{
    TX_ENTRY_ARGS( "uri =" << uri)
    
    QStringList keys;
    keys << tr("artist") << tr("albumtitle") << tr("tracktitle") << tr("orderby");
    
    // TODO: need to clarify which crition to use for sort, currently hard code to "relevancy"
    // order can be relevancy, alltimedownloads, streetreleasedate, sortname, recentdownloads
    QStringList values;
    values << mSongData->artist() << mSongData->album()
            << mSongData->title() << QString(tr("relevancy"));
    TX_LOG_ARGS( "Artist: " << mSongData->artist() ); 
    TX_LOG_ARGS( "Album: " << mSongData->album() );
    TX_LOG_ARGS( "Title: " << mSongData->title() );
    
    uri += keyValues( keys, values );

    QUrl url(uri);
    uri = url.toEncoded();
    TX_EXIT
}

/*!
 Get Atom response from Ovi server based on query
 */
void MpDetailsView::retrieveInformation( const QString &urlEncoded )
{
    TX_ENTRY_ARGS( "urlEconded = " << urlEncoded)
    QNetworkReply *reply = mManager->get( QNetworkRequest( QUrl( urlEncoded ) ) );
    mReplys.append( reply );
    
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( sslErrors( QList<QSslError> ) ), this, SLOT( retrieveInformationSslErrors( QList<QSslError> ) ) );
    TX_EXIT
}


/*!
 Slot to call when getting response
 */
void MpDetailsView::retrieveInformationFinished( QNetworkReply* reply )
{
    TX_ENTRY
    QString errorStr;
    int errorLine;
    int errorColumn;
    bool parsingSuccess;
    
    if ( reply->error() == QNetworkReply::NoError )
    {
        parsingSuccess = mDomDocument.setContent( reply, true, &errorStr, &errorLine, &errorColumn );
        if ( parsingSuccess ) {
            handleParsedXML();  //CodeScanner throws a warning mis-interpreting the trailing 'L' to be a leaving function.
        } else {
            // TODO: agree on error handling            
            TX_LOG_ARGS( "XML parsing error" );
        }
    }
    else
    {
    // TODO: agree on error handling
        TX_LOG_ARGS( "Network error in retrieving Information" );
    }
    TX_EXIT
}

/*!
 Slot to call when there is network error
 */
void MpDetailsView::retrieveInformationNetworkError( QNetworkReply::NetworkError /*error*/ )
{
    // TODO: agree on error handling
    TX_ENTRY_ARGS( "Network error for retrieving Information" );
    TX_EXIT
}

/*!
 Slot to call when there is ssl error
 */
void MpDetailsView::retrieveInformationSslErrors( const QList<QSslError> &/*error*/ )
{   
    // TODO: agree on error handling
    TX_ENTRY_ARGS( "SSL error for retrieving Information" );
    TX_EXIT
}

/*!
 Slot to call when downloading finished
 */
void MpDetailsView::DownloadFinished( QNetworkReply* reply )
{
    TX_ENTRY_ARGS( "mDownloadedAlbumArts = " << mDownloadedAlbumArts );
    if ( reply->error() == QNetworkReply::NoError )
    {
        QString fileName = mRecommendationAlbumArtsName.at( mDownloadedAlbumArts );
        TX_LOG_ARGS( "File name: " << fileName );
        QFile file(fileName);

        if ( fileName.isEmpty() ) {
            TX_LOG_ARGS( "Only store two album arts" );
        } else {
            if ( !file.open( QIODevice::ReadWrite ) ) {
                TX_LOG_ARGS( "Unable to open file" );
            } else {
                file.write( reply->readAll() );
                file.flush();
                file.close();
                setAlbumArtUri( mRecommendationAlbumArtsLink.at( mDownloadedAlbumArts ), 
                                mRecommendationAlbumArtsName.at( mDownloadedAlbumArts ) );
            }
        }
    }
    else
    {
        TX_LOG_ARGS( "Downloading album art failed!" );
    }
    
    mDownloadedAlbumArts++;
    TX_EXIT
}

/*!
 Slot to handle the recommendation album art 
*/
void MpDetailsView::thumbnailReady(
        const QPixmap& pixmap,
        void *data,
        int /*id*/,
        int error  )
{
    TX_ENTRY
    QString uri = *( reinterpret_cast<QString *>( data ) );
    TX_LOG_ARGS( "Uri: " << uri );
    
    if ( error == 0 ) {
        TX_LOG_ARGS( "album art link: " << uri );
        mRecommendationAlbumArtsMap.insert( uri, pixmap );
        recommendationAlbumArtReady();
    }
    else {
        mRecommendationAlbumArtsMap.insert( uri, mDefaultRecommendationAlbumArt );
        recommendationAlbumArtReady();
    }

    TX_EXIT
}


/*!
 Slot to add context to javascript
 */
void MpDetailsView::addContext()
{
    TX_ENTRY
    TX_EXIT
}

/*!
 Slot to close widget
 */
void MpDetailsView::close()
{
    TX_ENTRY
    TX_EXIT
}

