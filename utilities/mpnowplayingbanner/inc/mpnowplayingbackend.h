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
* Description: Music Player now playing widget backend.
*
*/

#ifndef MPNOWPLAYINGBACKEND_H
#define MPNOWPLAYINGBACKEND_H

//includes
#include <QObject>
#include <QMetaType>

//forward declartions
class MpNowPlayingBackEndPrivate;

enum SimplifiedPlayerState {
    NotPlaying, //When there is no source
    Playing,    //When it is playing
    Paused      //When is not in Playing State
};
// Register so type can be used in signal and slot connection
Q_DECLARE_METATYPE(SimplifiedPlayerState)

//class declaration
class MpNowPlayingBackEnd : public QObject
{
    Q_OBJECT
    friend class MpNowPlayingBackEndPrivate;

public:
    explicit MpNowPlayingBackEnd( long int playerId = 0 );
    virtual ~MpNowPlayingBackEnd();

signals:
    void stateUpdate( SimplifiedPlayerState state );
    void titleChanged( const QString &text );
    void artistChanged( const QString &text );

public slots:
    void update();
    void playPause();

private:
    Q_DISABLE_COPY(MpNowPlayingBackEnd)
    MpNowPlayingBackEndPrivate * const d_ptr;

};

#endif  // MPNOWPLAYINGBACKEND_H
