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

#include "mpquerymanager.h"
#include <QDebug>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkProxyFactory>
#include <qmobilityglobal.h>
#include <QNetworkSession>
#include <QDomElement>
#include <QList>
#include <QFile>
#include <QUrl>
#include <QSslError>
#include <QDir>
#include <QCoreApplication>
#include <XQSysInfo>

#include <thumbnailmanager_qt.h>
#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include "mptrace.h"

const int KUndefined = -1;
const int KRecommendationCount = 2;

MpQueryManager::MpQueryManager()
    : mManager(0),
      mAlbumArtDownloader(0),
      mThumbnailManager(0),
      mDefaultRecommendationAlbumArt("qtg_large_album_art"),
      mRequestType(NoRequest),
      mRecommendationCount(0)
                               
{
    TX_ENTRY

    QString privatePathQt( QCoreApplication::applicationDirPath() );
    TX_LOG_ARGS( "Private path: " << privatePathQt );
    QDir dir( privatePathQt );
    QString newDir = "detailsview";
    bool res = dir.mkdir( newDir );
    TX_LOG_ARGS( "New dir creation result: " << res);

    // TODO: Instead of writing the album art to a file, 
	// then using Thumbnail Manager to convert it, etc. 
	// have you considered just loading it directly into QPixmap? 
	// QPixmap provides a loadFromData() that can load from QByteArray. 
	// This would not only make the availability of the album art immediate, 
	// but also save a lot of cleanup in file system, thumbnail manager, etc.    
    privatePathQt = privatePathQt + "/detailsview";
    QString albumArt1( privatePathQt + "/albumOne.png" );
    QString albumArt2( privatePathQt + "/albumTwo.png" );
    mRecommendationAlbumArtsName << albumArt1 << albumArt2;
    TX_LOG_ARGS( "recommendation album art names: " << mRecommendationAlbumArtsName );

    mManager = new QNetworkAccessManager( this );      
    mAlbumArtDownloader = new QNetworkAccessManager( this );    // TODO: check if we can only use mManager
    mThumbnailManager = new ThumbnailManager( this );
    mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForQuality );
    mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailSmall );

    TX_EXIT
}

MpQueryManager::~MpQueryManager()
{
    TX_ENTRY
    reset();
    if ( mManager ) {
       mManager->deleteLater();
    }
    if ( mAlbumArtDownloader ) {
       mAlbumArtDownloader->deleteLater();
    }
    if( mThumbnailManager ) {
        mThumbnailManager->deleteLater();
    }
    TX_EXIT
}


void MpQueryManager::clearNetworkReplies()
{
    mRequestType = NoRequest; 
    disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );     
    TX_ENTRY_ARGS( "Reply count = " << mReplys.count() );    
    for ( int i = 0; i < mReplys.count(); ++i ) {
        QNetworkReply *reply = mReplys.at( i );
        disconnect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );        
        if ( reply != NULL ) {
            reply->close();
            reply->deleteLater();
            reply = NULL;
        }
    }
    mReplys.clear();
    TX_EXIT
}


void MpQueryManager::queryLocalMusicStore(QString artist,QString album,QString title)
{
    TX_ENTRY
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    // TODO: country information handling, MCC
    QString queryURI("http://api.music.ovi.com/1.0/ru/?");
    constructRequest( queryURI );
    TX_LOG_ARGS( "queryURI : " << queryURI );
    retrieveInformation( queryURI );
    mRequestType = LocalStoreRequest;    
    TX_EXIT    
}
    
void MpQueryManager::queryInspireMeItems(QString artist,QString album,QString title)
{
    TX_ENTRY    
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    // start querying inspire me items
    QString queryRecommendation("http://api.music.ovi.com/1.0/" + mMusicStore + "/releases/recommend/?");
    constructRequest( queryRecommendation );
	// TODO: Store the token to a cenrep key	
    // TODO: Tokens change per new ovi api release. 
	//       Need to figure out a way to get them updated on the fly
    queryRecommendation.append("&Token=03574704-e3d1-4466-9691-e0b34c7abfff");

    TX_LOG_ARGS( queryRecommendation );
    retrieveInformation( queryRecommendation );
    mRequestType = InspireMeItemsMetadataRequest;
    TX_EXIT
}

void MpQueryManager::clearRecommendations()
{
    TX_ENTRY
    mRecommendationCount = 0;
    mDownloadedAlbumArts = 0;
    mAlbumArtsReadyCount = 0;
    mRecommendationSongs.clear();
    mRecommendationArtists.clear();
    mRecommendationAlbumArtsLink.clear();
    mRecommendationAlbumArtsMap.clear();
    for ( int i = 0; i < mRecommendationAlbumArtsName.count(); ++i ) {
       QFile file( mRecommendationAlbumArtsName.at( i ) );
       TX_LOG_ARGS( "File " << file.fileName() << " exists: " << file.exists() );
       if ( file.exists() && file.remove() ) {
            TX_LOG_ARGS( "File " << file.fileName() << " removed");
       }
    }
    TX_EXIT    
}



/*!
 Returns the Local Music Store(if available) to be used while querying "Inspire Me" Items
 */
void MpQueryManager::queryLocalMusicStore()
{
    TX_ENTRY
    QString imsi,mcc;

    XQSysInfo sysInfo( this );
    imsi = sysInfo.imsi();
    mcc= imsi.left(3);
    TX_LOG_ARGS( "mcc : " << mcc );
    
    QString queryLocalMusicStore("http://api.music.cq1.brislabs.com/1.0/?mcc=" + mcc + "&token=06543e34-0261-40a4-a03a-9e09fe110c1f");
    TX_LOG_ARGS( "queryLocalMusicStore : " << queryLocalMusicStore );
    retrieveInformation( queryLocalMusicStore );
    mRequestType = LocalStoreRequest;
    TX_EXIT
}


int MpQueryManager::recommendationsCount() const
{
    TX_LOG_ARGS ("count: " << mRecommendationSongs.count());
    return mRecommendationSongs.count();
}

QString MpQueryManager::recommendedSong(int index) const
{
    QString result;
    if( (0 <= index) && (index < mRecommendationSongs.count())) {
        result = mRecommendationSongs.at(index);
    }
    TX_LOG_ARGS ("recommendedSong: " << result);    
    return result; 
}

/*!
 Return recommendation artists
 */
QString MpQueryManager::recommendedArtist(int index) const
{
    QString result;
    if( (0 <= index) && (index < mRecommendationArtists.count())) {
        result = mRecommendationArtists.at(index);
    }
    TX_LOG_ARGS ("recommendedArtist: " << result);    
    return result;
}

/*!
 Slot to call when getting response
 */
void MpQueryManager::retrieveInformationFinished( QNetworkReply* reply )
{
    TX_ENTRY
    QString errorStr;
    int errorLine;
    int errorColumn;
    bool parsingSuccess;
        
    if ( reply->error() != QNetworkReply::NoError ) {
		TX_LOG_ARGS("reply->error(): " << reply->error());
        signalError();
        return;
    }
        
    parsingSuccess = mDomDocument.setContent( reply, true, &errorStr, &errorLine, &errorColumn );
    if ( !parsingSuccess ) {
		TX_LOG_ARGS("Parsing Received Content Failed");
        signalError();
        return;
    }

    handleParsedXML();  //CodeScanner throws a warning mis-interpreting the trailing 'L' to be a leaving function.
    
    mReplys.removeAll(reply); // remove it so that we wont process it again
    reply->deleteLater(); // make sure reply is deleted, as we longer care about it
    
    TX_EXIT
}

void MpQueryManager::signalError()
{
    TX_ENTRY; 
    switch(mRequestType) {
        case InspireMeItemsMetadataRequest:
            TX_LOG_ARGS("emit inspireMeItemsRetrievalError");
            emit inspireMeItemsRetrievalError();
            break;
        case InspireMeItemsAlbumArtRequest:
            TX_LOG_ARGS("Warning: InspireMeItemsAlbumArtRequestError, will keep using the default AA icon");            
            break;    
        case LocalStoreRequest:
            TX_LOG_ARGS("emit localMusicStoreRetrievalError");            
            emit localMusicStoreRetrievalError();
            break;
        case NoRequest:   
        default:
            TX_LOG_ARGS("Warning!! Possible uninitialized mRequestType");            
            break;                                 
    }
    TX_EXIT    
}

/*!
 Slot to call when there is network error
 */
void MpQueryManager::retrieveInformationNetworkError( QNetworkReply::NetworkError error )
{
    TX_ENTRY_ARGS( "Network error for retrieving Information" << error);    
    // TODO: agree on error handling

	Q_UNUSED(error)

    disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );     
    signalError();
    TX_EXIT
}

/*!
 Slot to call when there is ssl error
 */
void MpQueryManager::retrieveInformationSslErrors( const QList<QSslError> &/*error*/ )
{
    // TODO: agree on error handling
    TX_ENTRY_ARGS( "SSL error for retrieving Information" );
    disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );     
    signalError();    
    TX_EXIT
}

/*!
 Slot to call when downloading finished
 */
void MpQueryManager::albumArtDownloaded( QNetworkReply* reply )
{
    TX_ENTRY_ARGS( "mDownloadedAlbumArts = " << mDownloadedAlbumArts );
    TX_ENTRY_ARGS( "QNetworkReply obj " << reply);
    
    // It seems we get several finished signals for the same reply obj
    // do nothing if we get a second signal
    if( mReplys.indexOf(reply) == -1 ) {
        TX_LOG_ARGS("Warning: QNetworkReply AA request may have been processed in previous call: " << reply );
        return;
    }
        
    if ( reply->error() == QNetworkReply::NoError ) {
        
        QString fileName = mRecommendationAlbumArtsName.at( mDownloadedAlbumArts );
        QByteArray imageData = reply->readAll();
        bool ret = writeImageToFile( imageData, fileName );

        // If file writing went OK, emit a signal with the real filename
        // If it failed, use empty filename (since file was removed in any case)
        if ( ret ) {
            // TODO: If album album arts come in different order than they have been asked,
            // then inspire me items will use swapped album arts. Correct
            setAlbumArtUri( mRecommendationAlbumArtsLink.at( mDownloadedAlbumArts), 
                            mRecommendationAlbumArtsName.at( mDownloadedAlbumArts ) );
        }
        else {
            setAlbumArtUri(mRecommendationAlbumArtsLink.at( mDownloadedAlbumArts), "");
        }
        ++mDownloadedAlbumArts;
        mReplys.removeAll(reply); // remove it so that we wont process it again
        reply->deleteLater(); // make sure reply is deleted, as we longer care about it
    }
    else {
        TX_LOG_ARGS( "Error: Downloading album art failed! Will keep using the default AA" );
    }
    
    if( mDownloadedAlbumArts == mRecommendationCount) {
        // no need to be informed anymore
        mAlbumArtDownloader->disconnect(this);
    }
    
    TX_EXIT
}

/*!
 Write the image data to a file with the given filename.
 If writing operation fails for any reason (e.g. OOD),
 returns false, otherwise true.
 */
bool MpQueryManager::writeImageToFile(const QByteArray &aImageData, const QString &aImageFileName )
{
    bool ret( false );
    TX_ENTRY_ARGS( "imagefile: " << aImageFileName );
    if ( aImageFileName.isEmpty() ) {
        TX_LOG_ARGS( "Only store two album arts" );
    }
    else {
        QFile file( aImageFileName );
        if ( !file.open( QIODevice::ReadWrite ) ) {
            TX_LOG_ARGS( "Unable to open file" );
        }
        else {
            qint64 writtenBytes = file.write( aImageData );
            // Verify file write status
            if ( writtenBytes < aImageData.size() ) {
                // If write succeeded only partially, or completely failed,
                // remove the file from filesystem to remove risk of corruption
                TX_LOG_ARGS( "Wrote only " << writtenBytes << " bytes, aborting operation!" );
                file.close();
                QFile::remove( mRecommendationAlbumArtsName.at( mDownloadedAlbumArts ) );
            }
            else {
                // If write fully succeeded, flush contents
                TX_LOG_ARGS( "Wrote all the bytes (" << writtenBytes << "), flushing and closing!");
                file.flush();
                file.close();
                ret = true;
            }
        }
    }
    TX_LOG_ARGS( "Returning with value: " << ret );
    TX_EXIT
    return ret;
}

/*!
 Get Atom response from Ovi server based on query
 */
void MpQueryManager::retrieveInformation( const QString &urlEncoded )
{
    TX_ENTRY_ARGS( "urlEconded = " << urlEncoded)
    connect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ), Qt::UniqueConnection );
    QNetworkReply *reply = mManager->get( QNetworkRequest( QUrl( urlEncoded ) ) );
    mReplys.append( reply );
    
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );
    connect( reply, SIGNAL( sslErrors( QList<QSslError> ) ), this, SLOT( retrieveInformationSslErrors( QList<QSslError> ) ) );
    TX_EXIT
}


/*!
 Find the most suitable link based on Atom response from Ovi music server
 */
void MpQueryManager::handleParsedXML()
{
    TX_ENTRY
    QDomElement rootElement = mDomDocument.documentElement();
    
    if ( rootElement.attribute( "type" ) == "recommendedTracks" ) {
        TX_LOG_ARGS( "Recommendation response" )
        QDomElement entry = rootElement.firstChildElement( "entry" );
        mRecommendationCount = 0;
        while ( !entry.isNull() && mRecommendationCount < KRecommendationCount ) {
            if ( entry.attribute( "type" ) == "musictrack" ) {
                QDomElement link = entry.firstChildElement( "link" );
                while ( !link.isNull() ) {
                    if ( link.attribute( "title" ) == "albumart100" ) {
                        mRecommendationAlbumArtsLink.append( link.attribute( "href" ) );
                        // TODO: This may get called twice for one inspire me item. Investigate why
                        mRecommendationAlbumArtsMap.insert( link.attribute( "href" ), mDefaultRecommendationAlbumArt );                      
                        break;
                    }
                    else {
                        link = link.nextSiblingElement( "link" );
                    }
                }
                QDomElement metadata = entry.firstChildElement( "metadata" );
                mRecommendationSongs.append( metadata.firstChildElement( "name" ).text() );
                mRecommendationArtists.append( metadata.firstChildElement( "primaryartist" ).text() );
                ++mRecommendationCount;
            }
            entry = entry.nextSiblingElement( "entry" );
        }
 
        emit inspireMeItemsMetadataRetrieved();

        QNetworkReply *reply = 0;
        // we need to channel the retrieved album arts to albumArtDownloaded slot only
        disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );        
        for (int i = 0; i < mRecommendationCount; i++ ) {
            TX_LOG_ARGS( "song name: " << mRecommendationSongs.at(i) );
            TX_LOG_ARGS( "Artist name: " << mRecommendationArtists.at(i) );
            TX_LOG_ARGS( "Album art link: " << mRecommendationAlbumArtsLink.at(i) );
            mRequestType = InspireMeItemsAlbumArtRequest;
            if ( mRecommendationAlbumArtsLink.at( i ).contains( "http", Qt::CaseInsensitive ) ) {
                reply = mAlbumArtDownloader->get( QNetworkRequest( QUrl( mRecommendationAlbumArtsLink.at(i) ) ) );
                mReplys.append( reply );
                connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( retrieveInformationNetworkError( QNetworkReply::NetworkError ) ) );
                connect( reply, SIGNAL( sslErrors( QList<QSslError> ) ), this, SLOT( retrieveInformationSslErrors( QList<QSslError> ) ) );
            }
        }
       // we have queried for album arts for inspire me items. Now, time to wait for a response        
       connect( mAlbumArtDownloader, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( albumArtDownloaded( QNetworkReply * ) ) );

    }
    else if ( rootElement.attribute( "type" ) == "storeList" ) {
		TX_LOG_ARGS( "Music Store List" )
        QDomElement entry = rootElement.firstChildElement( "workspace" );
        QString previousMusicStore = mMusicStore;
        mMusicStore = entry.attribute( "countryCode" );
        if(!mMusicStore.isEmpty()) {
            bool musicStoreUpdated = (previousMusicStore != mMusicStore);
        	TX_LOG_ARGS("Music Store" << mMusicStore );
            emit localMusicStoreRetrieved(musicStoreUpdated);
        }
        else {
            emit localMusicStoreRetrievalError();   
        }		
    }
    else {
        TX_LOG_ARGS( "Not supported response" )
    }
    TX_EXIT
}

void MpQueryManager::clearThumbnails()
{
    TX_ENTRY
    for(int i = 0; i < mThumbnailRequests.count(); ++i ) {
        mThumbnailManager->cancelRequest(mThumbnailRequests.at(i));
    }
    mThumbnailRequests.clear();    
    for(int i = 0; i < mRecommendationAlbumArtsName.count(); ++i) {
        mThumbnailManager->deleteThumbnails(mRecommendationAlbumArtsName.at(i)); // async, returns immidiately
    }
    TX_EXIT        
}

void MpQueryManager::reset()
{
    TX_ENTRY
    mManager->disconnect(this);
    mAlbumArtDownloader->disconnect(this);
    mThumbnailManager->disconnect(this);
    clearNetworkReplies();
    clearRecommendations();
    clearThumbnails();
    mRecommendationAlbumArtsMap.clear();
    TX_EXIT
}
    
/*!
 Construct the query for fetching URI & recommendations
 */
void MpQueryManager::constructRequest( QString &uri )
{
    TX_ENTRY_ARGS( "uri =" << uri)
    
    QStringList keys;
    keys << "artist" << "albumtitle" << "tracktitle" << "orderby";
    
    // TODO: need to clarify which crition to use for sort, currently hard code to "relevancy"
    // order can be relevancy, alltimedownloads, streetreleasedate, sortname, recentdownloads
    QStringList values;
    values << mArtist << mAlbum << mTitle << QString("relevancy");
    TX_LOG_ARGS( "Artist: " << mArtist ); 
    TX_LOG_ARGS( "Album: " << mAlbum );
    TX_LOG_ARGS( "Title: " << mTitle );
    
    uri += keyValues( keys, values );

    QUrl url(uri);
    uri = url.toEncoded();
    TX_EXIT
}

/*!
 Make a key & value pair string for querying
 */
QString MpQueryManager::keyValues( QStringList keys, QStringList values ) const
{
    TX_ENTRY
    QString str;
    if ( keys.length() != values.length() ) {
        TX_LOG_ARGS( "Error: keys length is not equal to values length" ); 
    }
    else {
        for ( int i = 0; i < keys.length(); i++ ) {
            QString tValue = values.at( i );
            if ( 0 != tValue.length() ) {
                str += keys.at( i ) + "=" + values.at( i ) + "&";
            }
        }
    }
    TX_EXIT
    return str.left( str.length() - 1 );
}

/*!
 Sets recommendation album art
*/
void MpQueryManager::setAlbumArtUri( const QString &albumArtUri, const QString &albumArtName )
{
    // TODO: rename this function. Doing too many things
    TX_ENTRY_ARGS( "albumArtUri = " << albumArtUri )
    TX_LOG_ARGS( "albumArtName = " << albumArtName )
    if ( !albumArtUri.isEmpty() && !albumArtName.isEmpty() ) {
        // TODO: this is no good to pass the address of the albumArtUri. It is an item in the list, which can cleaned
        int id = mThumbnailManager->getThumbnail( albumArtName, reinterpret_cast<void *>( const_cast<QString *>( &albumArtUri ) ) );
        if ( id == KUndefined ) {
            // Request failed. Set default album art
            mRecommendationAlbumArtsMap.insert( albumArtUri, mDefaultRecommendationAlbumArt );
        }
        else {
            // Async request went throu
            TX_LOG_ARGS("Request to thumbnail manager made. Id: " << id)
            mThumbnailRequests.append(id);
            mThumbnailManager->disconnect( this ); // to prevent multiple same connections with thumbnailmanager
            QObject::connect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ),
            this, SLOT( thumbnailReady( QPixmap , void * , int , int  ) ) );
        }
    }
    else {
        // No album art uri. Set default album art.
        mRecommendationAlbumArtsMap.insert( albumArtUri, mDefaultRecommendationAlbumArt );
    }
    TX_EXIT
}

/*!
 Slot to handle the recommendation album art 
*/
void MpQueryManager::thumbnailReady(
        const QPixmap pixmap,
        void *data,
        int id,
        int error  )
{
    TX_ENTRY

    if( mThumbnailRequests.indexOf(id) == -1 ) {
        TX_LOG_ARGS("Warning: some old thumbnail request from previous song. Ignoring")
        return;
    }
    
    // TODO: Using data from array, which can be reset. There must be a safer way    
    QString uri = *( reinterpret_cast<QString *>( data ) );
    TX_LOG_ARGS( "Uri: " << uri );
    
    if ( error == 0 ) {
        TX_LOG_ARGS( "album art link: " << uri );
        mRecommendationAlbumArtsMap.insert( uri, HbIcon(QIcon(pixmap)) );
    }
    else {
        mRecommendationAlbumArtsMap.insert( uri, mDefaultRecommendationAlbumArt );        
    }

    emit inspireMeItemAlbumArtReady();    
    TX_EXIT
}

bool MpQueryManager::isLocalMusicStore() const
{
    TX_LOG_ARGS( "isLocalMusicStore = " << !mMusicStore.isEmpty() )
    return !mMusicStore.isEmpty();
}

HbIcon MpQueryManager::recommendedAlbumArt(int index) const
{
    TX_LOG_ARGS( "index = " << index )
    return mRecommendationAlbumArtsMap.value( mRecommendationAlbumArtsLink.at( index ) );
}
