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

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <hbicon.h>

#include "mpnowplayingwidget.h"
#include "mpnowplayingwidget_p.h"
#include "mptrace.h"

/*!
    \class MpNowPlayingWidget
    \brief Custom HB widget that displays currently playing song information.

    This is a composite widget made out of HB widgets, it displays information
    for the current playing song on the playback engine indicated at 
    construction time.
*/

/*!
    \fn MpNowPlayingWidget::clicked()

    MpNowPlayingWidget emits this signal when there is a mouseReleaseEvent on
    top of the widget.
*/

/*!
    \fn MpNowPlayingWidget::playbackAttachmentChanged(bool attached)

    MpNowPlayingWidget emits this signal with a true value if the widget is 
    ready to display relevant data for the current playback, it is emmited 
    with false value if there is not playback instance to bind with. It is 
    recommended to use this signal to place and remove the widget from the UI.
*/


/*!
 Constructs the now playing widget.
 */
MpNowPlayingWidget::MpNowPlayingWidget(long int playerId, QGraphicsItem *parent )
    : HbWidget(parent),
    d_ptr ( new MpNowPlayingWidgetPrivate( playerId, this ) )
{
    TX_ENTRY_ARGS( "Player ID =" << playerId << " Parent=" << (void *)parent )
    TX_EXIT
}

/*!
 Destructs the now playing widget.
 */
MpNowPlayingWidget::~MpNowPlayingWidget()
{
    TX_ENTRY
    delete d_ptr;
    TX_EXIT
}

/*!
 If \a enabled is true, the item is enabled; otherwise, it is disabled.
 Item is enabled by default.
 */
void MpNowPlayingWidget::setEnabled( bool enabled )
{
    d_ptr->setEnabled(enabled);
}

/*!
    \reimp
 */
void MpNowPlayingWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    TX_ENTRY
    HbWidget::paint(painter, option, widget);
    QColor color(105,105,105,255);
    painter->fillRect(rect(), color);
    TX_EXIT
}

/*!
    \reimp
 */
void MpNowPlayingWidget::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    TX_ENTRY
    if ( event->button() == Qt::LeftButton ) {
        event->accept();
    }
    else {
        event->ignore();
    }
    TX_EXIT
}

/*!
    \reimp
 */
void MpNowPlayingWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    TX_ENTRY
    if ( event->button() == Qt::LeftButton ) {
        if ( !d_ptr->handleClickEvent( event ) && rect().contains( event->pos() ) ) {
            emit clicked();
        }
        event->accept();
    }
    else {
        event->ignore();
    }
    TX_EXIT
}

