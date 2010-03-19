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
* Description: mpnowplayingbackend stub for testing MpNowPlayingWidget
*
*/

#ifndef MPNOWPLAYINGBACKEND_H
#define MPNOWPLAYINGBACKEND_H

//includes
#include <QObject>
#include <QMetaType>

enum SimplifiedPlayerState {
    NotPlaying, //When there is no source
    Playing,    //When it is playing
    Paused      //When is not in Playing State
};

// Register so type can be used in signal and slot connection
Q_DECLARE_METATYPE(SimplifiedPlayerState)


class MpNowPlayingBackEnd : public QObject
{
    Q_OBJECT

public:

    // Test utility functions
    static int getUpdateCounter();
    static void resetUpdateCounter();
    static int getPlayPauseCounter();
    static void resetPlayPauseCounter();
    void triggerStateUpdate(SimplifiedPlayerState state);
    void triggerTitleChanged(const QString &text);
    void triggerArtistChanged(const QString &text);

    // Stub functions
    explicit MpNowPlayingBackEnd(long int playerId=0);
    virtual ~MpNowPlayingBackEnd();

signals:
    void stateUpdate(SimplifiedPlayerState state);
    void titleChanged(const QString &text);
    void artistChanged(const QString &text);

public slots:
    void update();
    void playPause();

};

#endif  // MPNOWPLAYINGBACKEND_H
