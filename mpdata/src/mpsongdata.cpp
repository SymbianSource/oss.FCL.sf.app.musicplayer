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
* Description: Playback Data provider for playback view.
*
*/


#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QBuffer>
#include <QPixmap>
#include <QTime>
#include <hbicon.h>
#include <QIcon>
#include <QFile>

#include <thumbnailmanager_qt.h>
#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include "mpsongdata.h"
#include "mptrace.h"

const int KUndefined = -1;

/*!
    \class MpSongData
    \brief Music Player song metadata.

    Song data provide acces to current playing song metadata
*/
    
/*!
    \fn void albumArtReady()

    This signal is albuma alrt is ready.
 */

/*!
    \fn void playbackInfoChanged()

    This signal is emitted when basic information is available
 */
    
/*!
    \fn void songDetailInfoChanged()

    This signal is emitted when detail information is available
 */
    
       

/*!
    Constructs a new MpSongData.
 */
MpSongData::MpSongData( QObject *parent )
    : QObject( parent ),
      mAlbumArt(),
      mReqId( KUndefined )
{
    TX_ENTRY
    mThumbnailManager = new ThumbnailManager( this );
    mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForQuality );
    mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailMedium );
    QObject::connect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ),
            this, SLOT( thumbnailReady( QPixmap , void * , int , int  ) ) );

    mDefaultAlbumArt = new HbIcon( "qtg_large_music_album" );
    TX_EXIT
}

/*!
 Constructs a new MpSongData.
 */
MpSongData::~MpSongData()
{
    TX_ENTRY
    if( mThumbnailManager ) {
        delete mThumbnailManager;
    }
    removeAlbumArtFile(); // TODO Remove when base64 is working
    TX_EXIT
}

/*!
 Returns the song album art on \a pixmap.
*/
void MpSongData::albumArt( HbIcon& icon ) const
{
    TX_ENTRY
     if (  !mAlbumArt || mAlbumArt->isNull() ) {
         TX_LOG_ARGS( "Album art is NULL." );
         icon = HbIcon();
     } else {
         TX_LOG_ARGS( "Album art is not NULL." );
         icon = *mAlbumArt ;
     }
    TX_EXIT
}


/*!
 Returns the song title.
*/
QString MpSongData::title() const
{
    TX_LOG
    return mTitle;
}

/*!
 Returns the song album.
*/
QString MpSongData::album() const
{
    TX_LOG
    return mAlbum;
}

/*!
 Returns the song artist.
*/
QString MpSongData::artist() const
{
    TX_LOG
    return mArtist;
}

/*!
 Returns comment
*/
QString MpSongData::comment() const
{
    TX_LOG
    return mComment;
}

/*!
 Returns the song composer.
*/
QString MpSongData::composer() const
{
    TX_LOG
    return mComposer;
}


/*!
 Returns the song genre.
*/
QString MpSongData::genre() const
{
    TX_LOG
    return mGenre;
}


/*!
 Returns the album track.
*/
QString MpSongData::albumTrack() const
{
    TX_LOG
    return mAlbumTrack;
}

/*!
 Returns link
*/
QString MpSongData::link() const
{
    TX_LOG
    return mLink;
}

/*!
 Returns the release date.
*/
QString MpSongData::year() const
{
    TX_LOG    
    return mYear;
}

/*!
 Returns the file name
*/
QString MpSongData::fileName() const
{
    TX_LOG    
    return mFileName;
}

/*!
 Returns the MIME type
*/
QString MpSongData::mimeType() const
{
    TX_LOG    
    return mMimeType;   
}

/*!
 Returns the duration
*/
QString MpSongData::duration() const
{
    TX_LOG    
    return mDuration;  
}

/*!
 Returns the bit rate
*/
QString MpSongData::bitRate() const
{
    TX_LOG    
    return mBitRate;  
}

/*!
 Returns the sampling rate
*/
QString MpSongData::sampleRate() const
{
    TX_LOG    
    return mSampleRate;  
}

/*!
 Returns the size
*/
QString MpSongData::size() const
{
    TX_LOG    
    return mSize;  
}

/*!
 Returns the size
*/
QString MpSongData::modified() const
{
    TX_LOG    
    return mModified;  
}

/*!
 Returns the copy right
*/
QString MpSongData::copyright() const
{
    TX_LOG    
    return mCopyright;  
}

/*!
 Returns the music URL
*/
QString MpSongData::musicURL() const
{
    TX_LOG    
    return mMusicURL;  
}

/*!
 Returns whether the song is protected
*/
bool MpSongData::isDrmProtected() const
{
    TX_LOG    
    return mDrmProtected;
}

/*!
 Sets the song \a title, returns true if the value is new.
*/
bool MpSongData::setTitle( const QString &title )
{
    TX_ENTRY_ARGS( "title =" << title )
    bool change = false;
    if ( title != mTitle ) {
        change = true;
        mTitle = title;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a album, returns true if the value is new.
*/
bool MpSongData::setAlbum( const QString &album )
{
    TX_ENTRY_ARGS( "album =" << album )
    bool change = false;
    if ( album != mAlbum ) {
        change = true;
        mAlbum = album;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a artist, returns true if the value is new.
*/
bool MpSongData::setArtist( const QString &artist )
{
    TX_ENTRY_ARGS( "artist =" << artist )
    bool change = false;
    if ( artist != mArtist ) {
        change = true;
        mArtist = artist;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a comment, returns true if the value is new.
*/
bool MpSongData::setComment( const QString &comment)
{
    TX_ENTRY_ARGS( "comment =" << comment )
    bool change = false;
    if ( comment != mComment ) {
        change = true;
        mComment = comment;
    }
    TX_EXIT
    return change;
}


/*!
 Sets the song \a composer, returns true if the value is new.
*/
bool MpSongData::setComposer( const QString &composer )
{
    TX_ENTRY_ARGS( "composer =" << composer )
    bool change = false;
    if ( composer != mComposer ) {
        change = true;
        mComposer = composer;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a genre, returns true if the value is new.
*/
bool MpSongData::setGenre( const QString &genre )
{
    TX_ENTRY_ARGS( "genre =" << genre )
    bool change = false;
    if ( genre != mGenre ) {
        change = true;
        mGenre = genre;
    }
    TX_EXIT
    return change;
}


/*!
 Sets the song \a date, returns true if the value is new.
*/
bool MpSongData::setYear( int year )
{
    TX_ENTRY_ARGS( "year =" << year )
    bool change = false;
    if ( QString::number(year) != mYear ) {
        change = true;
        if ( year >= 0 && year < 9999 ) {
            mYear = QString::number(year);
        } else {
            mYear = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a album track, returns true if the value is new.
*/
bool MpSongData::setAlbumTrack( const QString &track )
{
    TX_ENTRY_ARGS( "track =" << track )
    bool change = false;
    if ( track != mAlbumTrack ) {
        change = true;
        mAlbumTrack = track;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a link
*/
void MpSongData::setLink( const QString &link )
{
    TX_ENTRY_ARGS( "Link =" << link )
    mLink = link;
    TX_EXIT
}

/*!
 Sets the song \a albumArtUri.
*/
void MpSongData::setAlbumArtUri( const QString &albumArtUri)
{
    TX_ENTRY_ARGS( "albumArtUri = " << albumArtUri )
    if ( !albumArtUri.isEmpty() ) {
        TX_LOG_ARGS( "There is album art" );
        bool ok = true;
        if ( mReqId != KUndefined ) {
            // There is already an outstanding request. Cancel it first.
            bool ok = mThumbnailManager->cancelRequest( mReqId );
        }
        if ( ok ) {
            mReqId = mThumbnailManager->getThumbnail( albumArtUri );
            if ( mReqId == KUndefined ) {
                // Request failed. Set default album art.
                mAlbumArt = mDefaultAlbumArt;
                emit albumArtReady();
            }
        }
    }
    else {
        // No album art uri. Set default album art.
        TX_LOG_ARGS( "There is No album art" );
        mAlbumArt = mDefaultAlbumArt;
        emit albumArtReady();
    }
    TX_EXIT
}

/*!
 Sets the \a file name
*/
bool MpSongData::setFileName( const QString &fileName )
{
    TX_ENTRY_ARGS( "File name =" << fileName )
    bool change = false;
    if ( fileName != mFileName ) {
        change = true;
        mFileName = fileName;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a MIME type
*/
bool MpSongData::setMimeType( const QString &mimeType )
{    
    TX_ENTRY_ARGS( "Mime =" << mimeType )
    bool change = false;
    if ( mimeType != mMimeType ) {
        change = true;
        mMimeType = mimeType;
    }
    TX_EXIT
    return change;    
}

/*!
 Sets the \a duration
*/
bool MpSongData::setDuration( int duration )
{   
    TX_ENTRY_ARGS( "Duration =" << duration )
    bool change = false;
    QString timeFormatOne("%1:%2:%3");
    QString timeFormatTwo("%1:%2");
    if ( QString::number( duration ) != mDuration ) {
        change = true;
        if ( duration >= 3600 ) {
            // more than one hours
            QString hourStr, minStr, secStr;
            int hour = duration / 3600;
            int min = duration % 3600 / 60;
            int sec = duration % 3600 % 60;
            
            hourStr = hour >= 10 ? QString::number( hour ) : QString::number( hour ).prepend( "0" );
            minStr = min >= 10 ? QString::number( min ) : QString::number( min ).prepend( "0" );
            secStr = sec >= 10 ? QString::number( sec ) : QString::number( sec ).prepend( "0" );            
            mDuration = timeFormatOne.arg( hourStr ).arg( minStr ).arg( secStr );
        } else if ( duration >= 60 && duration < 3600 ) {
            // more than one min && less than one hour
            QString minStr, secStr;
            int min = duration / 60;
            int sec = duration % 60;
            
            minStr = min >= 10 ? QString::number( min ) : QString::number( min ).prepend( "0" );
            secStr = sec >= 10 ? QString::number( sec ) : QString::number( sec ).prepend( "0" );     
            mDuration = timeFormatTwo.arg( minStr ).arg( secStr );
        } else if ( duration > 0 && duration < 60 ) {
            QString secStr;
            secStr = duration >= 10 ? QString::number( duration ) : QString::number( duration ).prepend( "0" ); 
            mDuration = secStr;
        } else {
            mDuration = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets bit rate
*/
bool MpSongData::setBitRate( int bitRate)
{
    TX_ENTRY_ARGS( "Bit rate =" << bitRate )
    bool change = false;
    if ( QString::number( bitRate ) != mBitRate ) {
        change = true;
        if ( bitRate > 0 ) {
            mBitRate = QString::number( bitRate / 1000 );
        } else {
            mBitRate = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets sample rate
*/
bool MpSongData::setSampleRate( int sampleRate )
{
    TX_ENTRY_ARGS( "Sample rate =" << sampleRate )
    bool change = false;
    if ( QString::number( sampleRate ) != mSampleRate ) {
        change = true;
        if ( sampleRate > 0 ) {
            mSampleRate = QString::number( sampleRate );
        } else {
            mSampleRate = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a size
*/
bool MpSongData::setSize( int size )
{   
    TX_ENTRY_ARGS( "Size =" << size )
    bool change = false;
    if ( QString::number( size ) != mSize ) {
        change = true;
        mSize = QString::number( size / 1000 );
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a modification information
*/
bool MpSongData::setModified( const QString &modified )
{
    TX_ENTRY_ARGS( "Modified =" << modified )
    bool change = false;
    if ( modified != mModified ) {
        change = true;
        mModified = modified;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a copyright information
*/
bool MpSongData::setCopyright( const QString &copyright )
{
    TX_ENTRY_ARGS( "Copyright =" << copyright )
    bool change = false;
    if ( copyright != mCopyright ) {
        change = true;
        mCopyright = copyright;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a music URL
*/
bool MpSongData::setMusicURL( const QString &musicURL )
{
    TX_ENTRY_ARGS( "Music URL =" << musicURL )
    bool change = false;
    if ( musicURL != mMusicURL ) {
        change = true;
        mMusicURL = musicURL;
    }
    TX_EXIT
    return change;
}

/*!
 Set whether the song is DRM protected
*/
bool MpSongData::setDrmProtected( bool drmProtected )
{
    TX_ENTRY_ARGS( "DRM protected =" << drmProtected )
    bool change = false;
    if ( drmProtected != mDrmProtected ) {
        change = true;
        mDrmProtected = drmProtected;
    }
    TX_EXIT
    return change;
}

/*!
 Slot to handle the album art thumb.
*/
void MpSongData::thumbnailReady(
        const QPixmap& pixmap,
        void *data,
        int id,
        int error  )
{
    TX_ENTRY
    Q_UNUSED( data );
    if ( error == 0 && mReqId == id ) {
        QIcon qicon;
        QPixmap mCompositePixmap;
        mReqId = KUndefined;
        
        mCompositePixmap = QPixmap( 360, 360 );
        mCompositePixmap.fill( Qt::transparent );
        QPainter painter(&mCompositePixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.fillRect(mCompositePixmap.rect(), Qt::transparent);
        painter.drawPixmap(QRect(0, 0,360,360), pixmap);
        
        if ( !mCompositePixmap.isNull() ) {
            qicon = QIcon( mCompositePixmap );
        }
        else {
            qicon = QIcon( pixmap );
        }
        
        if ( mAlbumArt == mDefaultAlbumArt ) {        
            TX_LOG_ARGS( "Album art is default album art." )
            delete mAlbumArt; 
            mAlbumArt = new HbIcon(qicon);
            mDefaultAlbumArt = new HbIcon( "qtg_large_music_album" );
        } else {
            TX_LOG_ARGS( "Album art is NOT default album art." )
            delete mAlbumArt;
            mAlbumArt = new HbIcon(qicon);
        }       
                
        emit albumArtReady();
    }
    else {
        mReqId = KUndefined;
        mAlbumArt = mDefaultAlbumArt;
        emit albumArtReady();
    }

    TX_EXIT
}

/*!
 Emit signal when playback information changed, such as artist, track name
*/
void MpSongData::commitPlaybackInfo()
{
    TX_ENTRY
    emit playbackInfoChanged();
    TX_EXIT
}

/*!
 Emit signal when song detail information changed
*/
void MpSongData::commitSongDetailInfo()
{
    TX_ENTRY
    emit songDetailInfoChanged();
    TX_EXIT
}

/*!
 Retrieve the album art in base64 encoding suitable for inline HTML display for sharing player.
 */
QString MpSongData::albumArtBase64() const
{
    /*
    // Converts the current album art icon to a base64 string, and return the string.
    TX_LOG
    if ( mAlbumArt->isNull() ) {
        TX_ENTRY_ARGS( "MpSongData: album art isNull" )
        return "nullimgcraptoberemoved";
    }
    TX_ENTRY_ARGS("MpSongData: album art exists");
    QByteArray array;
    QBuffer buffer( &array );
    buffer.open( QIODevice::WriteOnly );
    mAlbumArt->pixmap().save( &buffer, "PNG" ); // writes pixmap into bytes in PNG format
    buffer.close();
    QString result = array.toBase64().constData();
    TX_ENTRY_ARGS("MpSongData: album art base64 length: " << result.length());
    return result;
    */
    // TODO: this is temporary solution until base64 defect in QT is fixed.
    TX_LOG
    QByteArray array;

    // Remove old album art in case new one cannot be written.
    removeAlbumArtFile();

    QString sTimeStamp = QTime::currentTime().toString( "hhmmsszzz" );
    QString sTempFileLocation = QString( "e:\\album_art_%1.png" ).arg( sTimeStamp );

    ( ( MpSongData* ) this )->mTempAlbumArt = sTempFileLocation;
    TX_LOG_ARGS( "Create album art file " << mTempAlbumArt );

    QFile file( mTempAlbumArt );
    file.open( QIODevice::WriteOnly );
    if ( mAlbumArt && !mAlbumArt->isNull() && !mAlbumArt->qicon().isNull() )
    {
        QPixmap p = mAlbumArt->qicon().pixmap( QSize( 74, 74 ), QIcon::Normal, QIcon::Off );
        p.save( &file, "PNG" );
        //mAlbumArt->pixmap().save( &file, "PNG" ); // writes pixmap into bytes in PNG format
    }
    file.close();
    return mTempAlbumArt;
}

/*!
 Delete temporary album art file.
 */
void MpSongData::removeAlbumArtFile() const
{
    TX_ENTRY
    if ( !mTempAlbumArt.isEmpty() )
    {
        TX_LOG_ARGS( "Remove album art file " << mTempAlbumArt );
        QFile::remove( mTempAlbumArt );
        ( ( MpSongData* ) this )->mTempAlbumArt = "";
    }
    else
    {
        TX_LOG_ARGS( "Album art filename is empty" );
    }
    TX_EXIT
}
