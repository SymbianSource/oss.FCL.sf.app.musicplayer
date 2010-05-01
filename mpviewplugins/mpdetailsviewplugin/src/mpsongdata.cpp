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


#include <qstring>
#include <qpixmap>

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
    
    mDefaultAlbumArt = QPixmap( ":/detailsviewicons/defaultalbumart.png" );
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
    TX_EXIT
}

/*!
 Returns the song album art on \a pixmap.
*/
void MpSongData::albumArt( QPixmap& pixmap )
{
    TX_ENTRY
     if ( !mAlbumArt.isNull() ) {
        pixmap = mAlbumArt;
     }
     else {
         pixmap = QPixmap();
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
        mAlbumArt = mDefaultAlbumArt;
        emit albumArtReady();
    }
    TX_EXIT
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
        mReqId = 0;
        mAlbumArt = pixmap;
        emit albumArtReady();
    }
    else {
        mReqId = 0;
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

