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
      mTitle( NULL ),
      mAlbum( NULL ),
      mArtist( NULL ),
      mComposer( NULL),
      mGenre( NULL ),
      mYear( NULL ),
      mAlbumTrack( NULL ),
      mAlbumArt(),
      mReqId( KUndefined )
{
    TX_ENTRY_ARGS("Stub")
}

/*!
 Constructs a new MpSongData.
 */
MpSongData::~MpSongData()
{
    TX_ENTRY_ARGS("Stub")
}

/*!
 Returns the song album art on \a pixmap.
*/
void MpSongData::albumArt( QPixmap& pixmap )
{
     if ( !mAlbumArt.isNull() ) {
        pixmap = mAlbumArt;
     }
     else {
         pixmap = QPixmap();
     }
}


/*!
 Returns the song title.
*/
QString MpSongData::title() const
{
    return mTitle;
}

/*!
 Returns the song album.
*/
QString MpSongData::album() const
{
    return mAlbum;
}

/*!
 Returns the song artist.
*/
QString MpSongData::artist() const
{
    return mArtist;
}

/*!
 Returns comment
*/
QString MpSongData::comment() const
{
    return mComment;
}

/*!
 Returns the song composer.
*/
QString MpSongData::composer() const
{
    return mComposer;
}


/*!
 Returns the song genre.
*/
QString MpSongData::genre() const
{
    return mGenre;
}


/*!
 Returns the album track.
*/
QString MpSongData::albumTrack() const
{
    return mAlbumTrack;
}

/*!
 Returns link
*/
QString MpSongData::link() const
{
    return mLink;
}

/*!
 Returns the release date.
*/
QString MpSongData::year() const
{
    return mYear;
}


/*!
 Sets the song \a title, returns true if the value is new.
*/
bool MpSongData::setTitle( const QString &title )
{
    bool change = false;
    if ( title != mTitle ) {
        change = true;
        mTitle = title;
    }
    return change;
}

/*!
 Sets the song \a album, returns true if the value is new.
*/
bool MpSongData::setAlbum( const QString &album )
{
    bool change = false;
    if ( album != mAlbum ) {
        change = true;
        mAlbum = album;
    }
    return change;
}

/*!
 Sets the song \a artist, returns true if the value is new.
*/
bool MpSongData::setArtist( const QString &artist )
{
    bool change = false;
    if ( artist != mArtist ) {
        change = true;
        mArtist = artist;
    }
    return change;
}

/*!
 Sets the song \a comment, returns true if the value is new.
*/
bool MpSongData::setComment( const QString &comment)
{
    bool change = false;
    if ( comment != mComment ) {
        change = true;
        mComment = comment;
    }
    return change;
}


/*!
 Sets the song \a composer, returns true if the value is new.
*/
bool MpSongData::setComposer( const QString &composer )
{
    bool change = false;
    if ( composer != mComposer ) {
        change = true;
        mComposer = composer;
    }
    return change;
}

/*!
 Sets the song \a genre, returns true if the value is new.
*/
bool MpSongData::setGenre( const QString &genre )
{
    bool change = false;
    if ( genre != mGenre ) {
        change = true;
        mGenre = genre;
    }
    return change;
}


/*!
 Sets the song \a date, returns true if the value is new.
*/
bool MpSongData::setYear( int year )
{
    bool change = false;
    if ( QString::number(year) != mYear ) {
        change = true;
        if ( year >= 0 && year < 9999 ) {
            mYear = QString::number(year);
        }
    }
    return change;
}

/*!
 Sets the \a album track, returns true if the value is new.
*/
bool MpSongData::setAlbumTrack( const QString &track )
{
    bool change = false;
    if ( track != mAlbumTrack ) {
        change = true;
        mAlbumTrack = track;
    }
    return change;
}

/*!
 Sets the \a link
*/
void MpSongData::setLink( const QString &link )
{
    mLink = link;
}

/*!
 Sets the song \a albumArtUri.
*/
void MpSongData::setAlbumArtUri( const QString &albumArtUri)
{
    Q_UNUSED(albumArtUri);
    mAlbumArt = mDefaultAlbumArt;
    emit albumArtReady();
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
    Q_UNUSED( pixmap );
    Q_UNUSED( data );
    Q_UNUSED( id );
    Q_UNUSED( error );
}

/*!
 Emit signal when playback information changed, such as artist, track name
*/
void MpSongData::commitPlaybackInfo()
{
    emit playbackInfoChanged();
}

/*!
 Emit signal when song detail information changed
*/
void MpSongData::commitSongDetailInfo()
{
    emit songDetailInfoChanged();
}

QString MpSongData::albumArtBase64() const
{

    return QString("");
}

void MpSongData::removeAlbumArtFile() const
{

}
