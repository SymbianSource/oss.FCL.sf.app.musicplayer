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
* Description: Framework wraper for Playback.
*
*/

#ifndef MPMPXPBFRAMEWORKWRAPPER_H
#define MPMPXPBFRAMEWORKWRAPPER_H

#include <QObject>

class MpMpxPbFrameworkWrapperPrivate;
class MpPlaybackData;

class MpMpxPbFrameworkWrapper : public QObject
{
    Q_OBJECT

    friend class MpMpxPbFrameworkWrapperPrivate;

public:

    explicit MpMpxPbFrameworkWrapper( QObject *parent=0 );
    virtual ~MpMpxPbFrameworkWrapper();

    MpPlaybackData *playbackData();

public slots:

    void playPause();
    void stop();
    void skipForward();
    void skipBackward();
    void setPosition( int position );
    void setShuffle( bool mode );
    void setRepeat( bool mode );

private:

    Q_DISABLE_COPY(MpMpxPbFrameworkWrapper)
    MpMpxPbFrameworkWrapperPrivate *d_ptr;

};

#endif  // MPMPXPBFRAMEWORKWRAPPER_H
