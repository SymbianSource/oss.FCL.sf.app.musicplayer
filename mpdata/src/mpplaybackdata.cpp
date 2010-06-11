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

#include <QIcon>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <hbicon.h>
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
    : QObject( parent ),
      mThumbnailManager( new ThumbnailManager( this ) ),
      mReqId( KUndefined ),
      mDuration(0),
      mPosition(0),
      mAlbumId(0),
      mId(0),
      mAlbumArt( "qtg_large_music_album" ),
      mDefaultAlbumArt( "qtg_large_music_album" ),
      mPlaybackState( NotPlaying ),
      mRealAudio( false )
{
    TX_ENTRY
    mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForQuality );
    mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailLarge );
    connect( mThumbnailManager, SIGNAL( thumbnailReady(QPixmap, void *, int, int ) ),
             this, SLOT(thumbnailReady( QPixmap, void *, int, int ) ) );
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
    // data is different or mArtist was not set befor or it was reset.
    // When artist is empty and mArtist is empty mArtist should be unknown text.
    if ( artist != mArtist || mArtist.isNull()) {
        change = true;
        if ( artist.isEmpty() ){
            mArtist = hbTrId( "txt_mus_other_unknown3" );      
        }
        else {
            mArtist = artist;
        }
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
    // data is different or mAlbum was not set befor or it was reset.
    // when album is empty and mAlbum is empty mAlbum should be unknown text.
    if ( album != mAlbum || mAlbum.isNull() ) {
        change = true;
        if ( album.isEmpty() ){
            mAlbum = hbTrId( "txt_mus_other_unknown4" );
        }
        else{
            mAlbum = album;            
        }
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
 Returns the song album art on \a icon.
*/
void MpPlaybackData::albumArt( HbIcon& icon ) const
{
    TX_ENTRY
    icon = mAlbumArt ;
    TX_EXIT
}

/*!
 Sets the song's album \a id, returns true if the value is new.
*/
bool MpPlaybackData::setAlbumId( int id )
{
    bool change = false;
    if (mAlbumId != id) {
        mAlbumId = id;
        change = true;
    }
    return change;
}

/*!
 Returns the id of the album to which the song belongs.
*/
int MpPlaybackData::albumId()
{
    return mAlbumId;
}

/*!
 Sets the song's \a id, returns true if the value is new.
*/
bool MpPlaybackData::setId( int id )
{
    bool change = false;
    if (mId != id) {
        mId = id;
        change = true;
    }
    return change;
}

/*!
 Returns the id the song.
*/
int MpPlaybackData::id()
{
    return mId;
}
/*!
Set Real Audio \a mode.
*/
bool MpPlaybackData::setRealAudio( bool mode )
{
    TX_ENTRY
    bool change = false;
    if (mRealAudio != mode) {
        mRealAudio = mode;
        change = true;
    }
    TX_EXIT
    return change;
}

/*!
 Returns the mode of Real Audio.
*/
bool MpPlaybackData::realAudio()
{
    return mRealAudio;
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
 Resets the data, currently called when source is removed.
*/
void MpPlaybackData::resetData()
{
    mDuration = 0;
    mPosition = 0;
    mTitle = QString();
    mAlbum = QString();
    mArtist = QString();
    mUri = QString();
    mAlbumId = 0;
    mId = 0;
    mAlbumArt = HbIcon();
    mRealAudio = false;
    
    emit durationChanged();
    emit positionChanged();
    emit albumArtReady();
    emit playbackInfoChanged();
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
        
        
        mAlbumArt = HbIcon(qicon);
        
        emit albumArtReady();
    }
    else {
        mReqId = KUndefined;
        mAlbumArt = mDefaultAlbumArt;
        emit albumArtReady();
    }
    TX_EXIT
}

