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

#include "mpplaybackdata.h"
#include "mptrace.h"

const int KUndefined = -1;

/*!
    \class MpPlaybackData
    \brief Music Player playback data.

    Playback data provides access to current playing song data, and playback
    properties, it is designed as an interface between the playback wrapper
    and the UI, it also provides album art extraction.
*/

/*!
    \fn void durationChanged()

    This signal is emitted when duration changes.
 */

/*!
    \fn void positionChanged()

    This signal is emitted when position changes.
 */

/*!
    \fn void albumArtReady()

    This signal is emitted when album art is ready.
 */

/*!
    \fn void playbackStateChanged()

    This signal is emitted when there is a *transition* on the playback state,
    this is based on a simple state.
 */

/*!
    \fn void playbackInfoChanged()

    This signal is emitted when playback info changes. This includes:
    title, artist and album name.
 */



/*!
    Constructs a new MpPlaybackData.
 */
MpPlaybackData::MpPlaybackData( QObject *parent )
    : QObject(parent),
      mThumbnailManager(0),
      mReqId(KUndefined),
      mDuration(0),
      mPosition(0),
      mAlbumArt()
{
    TX_ENTRY
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(thumbnailReady(QPixmap, void *, int, int)) );

    mDefaultAlbumArt = QPixmap(":/playbackviewicons/defaultalbumart.png");
    TX_EXIT
}

/*!
 Constructs a new MpPlaybackData.
 */
MpPlaybackData::~MpPlaybackData()
{
    TX_ENTRY
    delete mThumbnailManager;
    TX_EXIT
}

/*!
 Sets the song \a duration.
*/
void MpPlaybackData::setDuration( int duration )
{
    TX_ENTRY_ARGS( "duration = " << duration )
    mDuration = duration;
    emit durationChanged();
    TX_EXIT
}

/*!
 Returns the song duration.
*/
int MpPlaybackData::duration() const
{
    TX_LOG_ARGS( "mDuration = " << mDuration )
    return mDuration;
}

/*!
 Sets the song \a position.
*/
void MpPlaybackData::setPosition( int position )
{
    TX_ENTRY_ARGS( "position = " << position )
    mPosition = position;
    emit positionChanged();
    TX_EXIT
}

/*!
 Returns the song position.
*/
int MpPlaybackData::position() const
{
    TX_ENTRY_ARGS( "mPosition = " << mPosition )
    return mPosition;
}

/*!
 Sets the song \a title, returns true if the value is new.
*/
bool MpPlaybackData::setTitle( const QString& title )
{
    TX_ENTRY_ARGS( "title = " << title )
    bool change = false;
    if ( title != mTitle ) {
        change = true;
        mTitle = title;
    }
    TX_EXIT
    return change;
}

/*!
 Returns the song title.
*/
const QString& MpPlaybackData::title() const
{
    TX_ENTRY_ARGS( "mTitle = " << mTitle )
    return mTitle;
}

/*!
 Sets the song \a artist, returns true if the value is new.
*/
bool MpPlaybackData::setArtist( const QString& artist )
{
    TX_ENTRY_ARGS( "artist = " << artist )
    bool change = false;
    if ( artist != mArtist ) {
        change = true;
        mArtist = artist;
    }
    TX_EXIT
    return change;
}

/*!
 Returns the song artist.
*/
const QString& MpPlaybackData::artist() const
{
    TX_ENTRY_ARGS( "mArtist = " << mArtist )
    return mArtist;
}

/*!
 Sets the song \a album, returns true if the value is new.
*/
bool MpPlaybackData::setAlbum( const QString& album )
{
    TX_ENTRY_ARGS( "album = " << album )
    bool change = false;
    if ( album != mAlbum ) {
        change = true;
        mAlbum = album;
    }
    TX_EXIT
    return change;
}

/*!
 Returns the song album.
*/
const QString& MpPlaybackData::album() const
{
    TX_ENTRY_ARGS( "mAlbum = " << mAlbum )
    return mAlbum;
}

/*!
 Sets the song \a uri, returns true if the value is new.
*/
bool MpPlaybackData::setUri( const QString& uri )
{
    TX_ENTRY_ARGS( "uri = " << uri )
    bool change = false;
    if ( uri != mUri ) {
        change = true;
        mUri = uri;
    }
    TX_EXIT
    return change;
}

/*!
 Returns the song uri.
*/
const QString& MpPlaybackData::uri() const
{
    TX_ENTRY_ARGS( "mUri = " << mUri )
    return mUri;
}

/*!
 Sets the song \a albumArtUri.
*/
void MpPlaybackData::setAlbumArtUri( const QString& albumArtUri )
{
    TX_ENTRY_ARGS( "albumArtUri = " << albumArtUri )
    if ( !albumArtUri.isEmpty() ) {
        bool ok = true;
        if ( mReqId != KUndefined ) {
            // There is already an outstanding request. Cancel it first.
            bool ok = mThumbnailManager->cancelRequest(mReqId);
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
 Returns the song album art on \a pixmap.
*/
void MpPlaybackData::albumArt( QPixmap& pixmap ) const
{
    TX_ENTRY
    if ( mAlbumArt.isNull() ) {
        pixmap = QPixmap();
     }
     else {
        pixmap = mAlbumArt;
     }
    TX_EXIT
}

/*!
 Sets the playback \a state.
*/
void MpPlaybackData::setPlaybackState( const SimplifiedState state )
{
    TX_ENTRY_ARGS( "state = " << state )
    if ( state != mPlaybackState ) {
        mPlaybackState = state;
        emit playbackStateChanged();
    }
    TX_EXIT
}

/*!
 Returns the playback state.
*/
MpPlaybackData::SimplifiedState MpPlaybackData::playbackState() const
{
    TX_LOG_ARGS( "mPlaybackState = " << mPlaybackState )
    return mPlaybackState;
}

/*!

*/
void MpPlaybackData::commitPlaybackInfo()
{
    TX_ENTRY
    emit playbackInfoChanged();
    TX_EXIT
}

/*!
 Slot to handle the album art thumb.
*/
void MpPlaybackData::thumbnailReady(
    const QPixmap& pixmap, void *data, int id, int error )
{
    TX_LOG_ARGS( "error = " << error << ", id = " << id )
    Q_UNUSED(data);
    if ( error == 0 && mReqId == id ) {
        mReqId = KUndefined;
        mAlbumArt = pixmap;
        emit albumArtReady();
    }
    else {
        mReqId = KUndefined;
        mAlbumArt = mDefaultAlbumArt;
        emit albumArtReady();
    }
    TX_EXIT
}

