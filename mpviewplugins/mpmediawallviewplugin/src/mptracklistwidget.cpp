/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Track List Widget for Music Player Media Wall.
*
*/


#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>

#include <hbframeitem.h>
#include <hbinstance.h>
#include <hbmainwindow.h>


#include "mptracklistwidget.h"

const int swipeAngleTolerance = 30; // angle is from 0 to 360


MpTrackListWidget::MpTrackListWidget( QGraphicsItem *parent ) : HbWidget( parent ) 
{
    mList = new HbListView( );
    QGraphicsLinearLayout   *layout = new QGraphicsLinearLayout( );
    layout->addItem( mList );
    setLayout( layout );
    
    grabGesture(Qt::SwipeGesture);
    mFrameItem = new HbFrameItem( this );
    mFrameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
    
    //TODO: REMOVE PAINT and enable frame drawer when graphic is delibered.
    //mFrameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_popup" );  
    mFrameItem->setZValue(-1);
}

/*!
    \reimp
 */
void MpTrackListWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    mFrameItem->setGeometry( rect() );
    HbWidget::resizeEvent( event );
}


/*!
    \reimp
 */
void MpTrackListWidget::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( widget )
    Q_UNUSED( option )
    painter->setPen(Qt::darkGray);
    painter->setBrush(QBrush(Qt::gray ));
    painter->drawRoundedRect( rect(), 4.0 , 4.0 );
}

/*!
    \reimp
 */
void MpTrackListWidget::gestureEvent(QGestureEvent *event)
{
    QGesture* gesture = event->gesture(Qt::SwipeGesture);
    if (gesture) {
        QSwipeGesture* swipe = static_cast<QSwipeGesture *>(gesture);
        if (swipe->state() == Qt::GestureFinished && 
                swipeAngleToDirection (swipe->swipeAngle()) == QSwipeGesture::Left ) {
            //Left gesture is the direction in wich the track list slides to close.
            emit close();
            event->accept(Qt::SwipeGesture);
        }
    }    
}

/*!
    Returns the HbListView instance.
 */
HbListView *MpTrackListWidget::list()
{
    return mList;
} 

/*!
    Maps swipe angle to SwipeDirection based on a tolerance parameter and orientation.
 */
QSwipeGesture::SwipeDirection MpTrackListWidget::swipeAngleToDirection(
        int angle)
{
    int delta = swipeAngleTolerance;
    if ( hbInstance->allMainWindows()[0]->orientation() == Qt::Horizontal ) {
        //correction for transformation on rotation.
        angle += ( angle < 90 ) ? 270 : -90;  
    }
    QSwipeGesture::SwipeDirection direction(QSwipeGesture::NoDirection);
    if ((angle > 90-delta) && (angle < 90+delta)) {
        direction = QSwipeGesture::Up;
    } else if ((angle > 270-delta) && (angle < 270+delta)) {
        direction = QSwipeGesture::Down;
    } else if (((angle >= 0) && (angle < delta)) 
            || ((angle > 360-delta) && (angle <= 360))) {
        direction = QSwipeGesture::Right;
    } else if ((angle > 180-delta) && (angle < 180+delta)) {
        direction = QSwipeGesture::Left;
    }
    return direction;    
}

/*!
    \reimp
 */
void MpTrackListWidget::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton ) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

/*!
    \reimp
 */
void MpTrackListWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        event->accept();
    }
    else {
        event->ignore();
    }
}


