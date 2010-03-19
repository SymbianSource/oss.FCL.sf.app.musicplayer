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
* Description: Music Player now playing widget.
*
*/

#ifndef MPNOWPLAYINGWIDGET_H
#define MPNOWPLAYINGWIDGET_H

//includes
#include <QtGlobal>
#include <hbwidget.h>

#ifdef BUILD_MPNOWPLAYINGBANNER
#define MPNOWPLAYINGBANNER_EXPORT Q_DECL_EXPORT
#else
#define MPNOWPLAYINGBANNER_EXPORT Q_DECL_IMPORT
#endif

//forward declartions
class QGraphicsSceneMouseEvent;
class MpNowPlayingWidgetPrivate;

class MPNOWPLAYINGBANNER_EXPORT MpNowPlayingWidget : public HbWidget
{
    Q_OBJECT
    friend class MpNowPlayingWidgetPrivate;

public:
    explicit MpNowPlayingWidget(long int playerId, QGraphicsItem *parent=0 );
    virtual ~MpNowPlayingWidget();
    void setEnabled( bool enabled );
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0 );

signals:
    void clicked();
    void playbackAttachmentChanged(bool attached);

private:
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

private:
    Q_DISABLE_COPY(MpNowPlayingWidget)
    MpNowPlayingWidgetPrivate * const d_ptr; //owned

};

#endif    // MPNOWPLAYINGWIDGET_H
