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
    TX_ENTRY_ARGS("STUB")

    mDefaultAlbumArt = QPixmap( ":/playbackviewicons/defaultalbumart.png" );
    TX_EXIT_ARGS("STUB")
}

/*!
 Constructs a new MpSongData.
 */
MpSongData::~MpSongData()
{
    TX_ENTRY_ARGS("STUB")

    TX_EXIT_ARGS("STUB")
}

/*!
 Returns the song album art on \a pixmap.
*/
void MpSongData::albumArt( QPixmap& pixmap )
{
    TX_ENTRY_ARGS("STUB")
     if ( !mAlbumArt.isNull() ) {
        pixmap = mAlbumArt;
     }
     else {
         pixmap = QPixmap();
     }
    TX_EXIT_ARGS("STUB")
}


/*!
 Returns the song title.
*/
QString MpSongData::title() const
{
    TX_ENTRY_ARGS("STUB")
    return mTitle;
}

/*!
 Returns the song album.
*/
QString MpSongData::album() const
{
    TX_ENTRY_ARGS("STUB")
    return mAlbum;
}

/*!
 Returns the song artist.
*/
QString MpSongData::artist() const
{
    TX_ENTRY_ARGS("STUB")
    return mArtist;
}

/*!
 Returns comment
*/
QString MpSongData::comment() const
{
    TX_ENTRY_ARGS("STUB")
    return mComment;
}

/*!
 Returns the song composer.
*/
QString MpSongData::composer() const
{
    TX_ENTRY_ARGS("STUB")
    return mComposer;
}


/*!
 Returns the song genre.
*/
QString MpSongData::genre() const
{
    TX_ENTRY_ARGS("STUB")
    return mGenre;
}


/*!
 Returns the album track.
*/
QString MpSongData::albumTrack() const
{
    TX_ENTRY_ARGS("STUB")
    return mAlbumTrack;
}

/*!
 Returns link
*/
QString MpSongData::link() const
{
    TX_ENTRY_ARGS("STUB")
    return mLink;
}

/*!
 Returns the release date.
*/
QString MpSongData::year() const
{
    TX_ENTRY_ARGS("STUB")    
    return mYear;
}


/*!
 Sets the song \a title, returns true if the value is new.
*/
bool MpSongData::setTitle( const QString &title )
{
    TX_ENTRY_ARGS("STUB title =" << title )
    bool change = false;
    if ( title != mTitle ) {
        change = true;
        mTitle = title;
    }
    TX_EXIT_ARGS("STUB")
    return change;
}

/*!
 Sets the song \a album, returns true if the value is new.
*/
bool MpSongData::setAlbum( const QString &album )
{
    TX_ENTRY_ARGS("STUB album =" << album )
    bool change = false;
    if ( album != mAlbum ) {
        change = true;
        mAlbum = album;
    }
    TX_EXIT_ARGS("STUB")
    return change;
}

/*!
 Sets the song \a artist, returns true if the value is new.
*/
bool MpSongData::setArtist( const QString &artist )
{
    TX_ENTRY_ARGS("STUB artist =" << artist )
    bool change = false;
    if ( artist != mArtist ) {
        change = true;
        mArtist = artist;
    }
    TX_EXIT_ARGS("STUB")
    return change;
}

/*!
 Sets the song \a comment, returns true if the value is new.
*/
bool MpSongData::setComment( const QString &comment)
{
    TX_ENTRY_ARGS("STUB comment =" << comment )
    bool change = false;
    if ( comment != mComment ) {
        change = true;
        mComment = comment;
    }
    TX_EXIT_ARGS("STUB")
    return change;
}


/*!
 Sets the song \a composer, returns true if the value is new.
*/
bool MpSongData::setComposer( const QString &composer )
{
    TX_ENTRY_ARGS("STUB composer =" << composer )
    bool change = false;
    if ( composer != mComposer ) {
        change = true;
        mComposer = composer;
    }
    TX_EXIT_ARGS("STUB")
    return change;
}

/*!
 Sets the song \a genre, returns true if the value is new.
*/
bool MpSongData::setGenre( const QString &genre )
{
    TX_ENTRY_ARGS("STUB genre =" << genre )
    bool change = false;
    if ( genre != mGenre ) {
        change = true;
        mGenre = genre;
    }
    TX_EXIT_ARGS("STUB")
    return change;
}


/*!
 Sets the song \a date, returns true if the value is new.
*/
bool MpSongData::setYear( int year )
{
    TX_ENTRY_ARGS("STUB year =" << year )
    bool change = false;
    if ( QString::number(year) != mYear ) {
        change = true;
        if ( year >= 0 && year < 9999 ) {
            mYear = QString::number(year);
        } else {
            mYear = QString();
        }
    }
    TX_EXIT_ARGS("STUB")
    return change;
}

/*!
 Sets the \a album track, returns true if the value is new.
*/
bool MpSongData::setAlbumTrack( const QString &track )
{
    TX_ENTRY_ARGS("STUB track =" << track )
    bool change = false;
    if ( track != mAlbumTrack ) {
        change = true;
        mAlbumTrack = track;
    }
    TX_EXIT_ARGS("STUB")
    return change;
}

/*!
 Sets the \a link
*/
void MpSongData::setLink( const QString &link )
{
    TX_ENTRY_ARGS("STUB Link =" << link )
    mLink = link;
    TX_EXIT_ARGS("STUB")
}

/*!
 Sets the song \a albumArtUri.
*/
void MpSongData::setAlbumArtUri( const QString &albumArtUri)
{
    TX_ENTRY_ARGS("STUB albumArtUri =" << albumArtUri )

    // No album art uri. Set default album art.
    mAlbumArt = mDefaultAlbumArt;
    emit albumArtReady();
    TX_EXIT_ARGS("STUB")
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
    TX_ENTRY_ARGS("STUB")
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

    TX_EXIT_ARGS("STUB")
}

/*!
 Emit signal when playback information changed, such as artist, track name
*/
void MpSongData::commitPlaybackInfo()
{
    TX_ENTRY_ARGS("STUB")
    emit playbackInfoChanged();
    TX_EXIT_ARGS("STUB")
}

/*!
 Emit signal when song detail information changed
*/
void MpSongData::commitSongDetailInfo()
{
    TX_ENTRY_ARGS("STUB")
    emit songDetailInfoChanged();
    TX_EXIT_ARGS("STUB")
}

