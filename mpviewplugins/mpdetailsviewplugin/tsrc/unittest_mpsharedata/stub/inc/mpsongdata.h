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
* Description: Stub of Metadata of song
*
*/

#ifndef MPSONGDATA_H
#define MPSONGDATA_H

#include <QObject>
#include <QString>
#include <QPixmap>


//forward declartions
class QIcon;

//class declaration
class MpSongData : public QObject
{
    Q_OBJECT

public:
    explicit MpSongData( QObject *parent=0 );
    virtual ~MpSongData();
    
    bool setTitle( const QString &title );
    bool setAlbum( const QString &album );
    bool setArtist( const QString &artist );
    bool setComment( const QString &comment );
    void setAlbumArtUri( const QString &albumArtUri );
    bool setYear( int date );
    bool setGenre( const QString &genre );
    bool setComposer( const QString &compoer );
    bool setAlbumTrack( const QString &albumtrack );
    void setLink( const QString &link );
    bool setMusicURL( const QString &musicURL );

    // inform details view when basic information is ready to accelerate UI update
    void commitPlaybackInfo();
    // inform details view when details information is ready
    void commitSongDetailInfo();

public slots:
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    
    int reservedLength() const { return mLink.length() + mTitle.length() + mArtist.length(); }

    QString title() const;
    QString album() const;
    QString artist() const;
    QString comment() const;
    void albumArt( QPixmap& pixmap );
    QString year() const;
    QString genre() const;
    QString composer() const;
    QString albumTrack() const;
    QString musicURL() const;
    QString link() const;
    QString albumArtBase64() const;

signals:
    void albumArtReady();
    void playbackInfoChanged();
	void songDetailInfoChanged();	

private:
    QString                  mTitle;
    QString                  mAlbum;
    QString                  mArtist;
    QString                  mComment;
    QString                  mComposer;
    QString                  mGenre;
    QString                  mYear;
    QString                  mAlbumTrack;
    QPixmap                  mAlbumArt;
    int                      mReqId;
    QString                  currentAAUri; 
    QPixmap                  mDefaultAlbumArt;
    QString                  mLink;
    QString                  mMusicURL;
    
    Q_DISABLE_COPY(MpSongData)
};

#endif // MPSONGDATA_H
