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
#include <qnetworksession.h>
#include <QDomElement>
#include <QList>
#include <QFile>
#include <QUrl>
#include <QSslError>
#include <QDir>
#include <QCoreApplication>

#include "mpdetailssharedialog.h"
#include "mptrace.h"

const int KRecommendationCount = 2;

MpQueryManager::MpQueryManager()
                               
{
    TX_ENTRY

    QString privatePathQt( QCoreApplication::applicationDirPath() );
    TX_LOG_ARGS( "Private path: " << privatePathQt );
    QDir dir( privatePathQt );
    QString newDir = "detailsview";
    bool res = dir.mkdir( newDir );
    TX_LOG_ARGS( "New dir creation result: " << res);

    // We're not handling a negative result for directory creation here,
    // instead it will be escalated in DownloadFinished() method.
    privatePathQt = privatePathQt + "/detailsview";
    QString albumArt1( privatePathQt + "/albumOne.png" );
    QString albumArt2( privatePathQt + "/albumTwo.png" );
    mRecommendationAlbumArtsName << albumArt1 << albumArt2;
    TX_LOG_ARGS( "recommendation album art names: " << mRecommendationAlbumArtsName );

    mManager = new QNetworkAccessManager( this );
      
    mDownloadManager = new QNetworkAccessManager( this );
    connect( mDownloadManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( DownloadFinished( QNetworkReply * ) ) );
     
    TX_EXIT
}

MpQueryManager::~MpQueryManager()
{
    TX_ENTRY
    if ( mManager ) {
       mManager->deleteLater();
     }
    if ( mDownloadManager ) {
       mDownloadManager->deleteLater();
    }
    TX_EXIT
}


void MpQueryManager::clearNetworkReplies()
{
    disconnect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );     
    TX_ENTRY_ARGS( "Reply count = " << mReplys.count() );    
    for ( int i = 0; i < mReplys.count(); i++ ) {
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
    TX_EXIT    
}
    
void MpQueryManager::queryInspireMeItems(QString artist,QString album,QString title)
{
    TX_ENTRY    
    mArtist=artist;
    mAlbum=album;
    mTitle=title;
    // start querying inspire me items
    QString queryRecommendation("http://api.music.ovi.com/1.0/gb/releases/recommend/?");
    constructRequest( queryRecommendation );
    TX_LOG_ARGS( "queryRecommendation : " << queryRecommendation );
    retrieveInformation( queryRecommendation );
    TX_EXIT
}

void MpQueryManager::clearRecommendations()
{
    TX_ENTRY    
    mDownloadedAlbumArts = 0;
    mAlbumArtsReadyCount = 0;
    for ( int i = 0; i < KRecommendationCount; i++ ) {
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
    TX_EXIT    
}

/*!
 Return recommendation songs
 */
QStringList MpQueryManager::recommendationSongs()
{
    TX_LOG  
    return mRecommendationSongs;
}

/*!
 Return recommendation artists
 */
QStringList MpQueryManager::recommendationArtists()
{
    TX_LOG  
    return mRecommendationArtists;
}

/*!
 Return recommendation album arts links
 */
QStringList MpQueryManager::recommendationAlbumArtsLink()
{
    TX_LOG  
    return mRecommendationAlbumArtsLink;
}

/*!
 Return map of name and pixmap
 */
QMap<QString, QPixmap>  MpQueryManager::recommendationAlbumArtsMap()
{
    TX_LOG  
    return mRecommendationAlbumArtsMap;
}

/*!
 Return the number of ready album arts
 */
int &MpQueryManager::albumArtsReadyCount()
{
    TX_LOG  
    return mAlbumArtsReadyCount;
}

/*!
 Insert one uri & pixmap item into map
 */
void MpQueryManager::insertMapItem( const QString &uri, const QPixmap &pixmap )
{
    TX_ENTRY_ARGS( "Map Item URI: " << uri );
    mRecommendationAlbumArtsMap.insert( uri, pixmap );
    TX_EXIT
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
        emit networkError();
    }
    TX_EXIT
}

/*!
 Slot to call when there is network error
 */
void MpQueryManager::retrieveInformationNetworkError( QNetworkReply::NetworkError error )
{
    Q_UNUSED(error);
    TX_ENTRY_ARGS( "Network error for retrieving Information" << error);
    TX_EXIT
}

/*!
 Slot to call when there is ssl error
 */
void MpQueryManager::retrieveInformationSslErrors( const QList<QSslError> &/*error*/ )
{   
    // TODO: agree on error handling
    TX_ENTRY_ARGS( "SSL error for retrieving Information" );
    TX_EXIT
}

/*!
 Slot to call when downloading finished
 */
void MpQueryManager::DownloadFinished( QNetworkReply* reply )
{
    TX_ENTRY_ARGS( "mDownloadedAlbumArts = " << mDownloadedAlbumArts );
    if ( reply->error() == QNetworkReply::NoError )
        {
        QString fileName = mRecommendationAlbumArtsName.at( mDownloadedAlbumArts );
        QByteArray imageData = reply->readAll();
        bool ret = writeImageToFile( imageData, fileName );

        // If file writing went OK, emit a signal with the real filename
        // If it failed, use empty filename (since file was removed in any case)
        if ( ret )
            {
            emit setAlbumArt( mRecommendationAlbumArtsLink.at( mDownloadedAlbumArts),
                                 mRecommendationAlbumArtsName.at( mDownloadedAlbumArts ) );
            }
        else
            {
            emit setAlbumArt( mRecommendationAlbumArtsLink.at( mDownloadedAlbumArts), "" );
            }
        }
    else
    {
        TX_LOG_ARGS( "Downloading album art failed!" );
        emit networkError();
    }
    
    mDownloadedAlbumArts++;
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
    if ( aImageFileName.isEmpty() )
        {
        TX_LOG_ARGS( "Only store two album arts" );
        }
    else
        {
        QFile file( aImageFileName );

        if ( !file.open( QIODevice::ReadWrite ) )
            {
            TX_LOG_ARGS( "Unable to open file" );
            }
        else
            {
            qint64 writtenBytes = file.write( aImageData );

            // Verify file write status
            if ( writtenBytes < aImageData.size() )
                {
                // If write succeeded only partially, or completely failed,
                // remove the file from filesystem to remove risk of corruption
                TX_LOG_ARGS( "Wrote only " << writtenBytes << " bytes, aborting operation!" );
                file.close();
                QFile::remove( mRecommendationAlbumArtsName.at( mDownloadedAlbumArts ) );
                }
            else
                {
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
    connect( mManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( retrieveInformationFinished( QNetworkReply * ) ) );    
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
    
    if ( rootElement.attribute( "type" ) == "search" ) {
        TX_LOG_ARGS( "URI response" )
        QString result;
        QDomElement entry = rootElement.firstChildElement( "entry" );
        while ( !entry.isNull() )
        {
            if ( entry.attribute( "type" ) == "musictrack" ) {
                QDomElement link = entry.firstChildElement( "link" );
                while ( !link.isNull() )
                {
                    if ( link.attribute( "rel" ) == "alternate"
                        && link.attribute( "type" ) == "text/html" ) {
                        result = link.attribute( "href" );
                    }
                    link = link.nextSiblingElement( "link" );
                }
            }
            entry = entry.nextSiblingElement( "entry" );
        }
		// Signal that the url was received. Might be empty string.
		emit searchUrlRetrieved( result );
    } else if ( rootElement.attribute( "type" ) == "recommendedTracks" ) {
        TX_LOG_ARGS( "Recommendation response" )
        QDomElement entry = rootElement.firstChildElement( "entry" );
        QNetworkReply *reply;
        int count = 0;
        while ( !entry.isNull() && count < KRecommendationCount )
        {
            if ( entry.attribute( "type" ) == "musictrack" ) {
                QDomElement link = entry.firstChildElement( "link" );
                while ( !link.isNull() )
                {
                    if ( link.attribute( "title" ) == "albumart100" ) {
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
        
        for (int i = 0; i < KRecommendationCount; i++ ) {
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

