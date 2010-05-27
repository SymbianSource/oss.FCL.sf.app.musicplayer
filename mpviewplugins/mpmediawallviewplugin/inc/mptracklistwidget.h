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
* Description:  Track List Widget for Music Player Media Wall.
*
*/
#ifndef MPTRACKLISTWIDGET_H
#define MPTRACKLISTWIDGET_H

#include <QGesture>

#include <hbwidget.h>
#include <hblistview.h>

class HbFrameItem;

class MpTrackListWidget : public HbWidget
{
    Q_OBJECT
  
public:
    
    MpTrackListWidget( QGraphicsItem *parent = 0 );
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, 
                QWidget *widget = 0 );
    HbListView *list(); 


signals:

    void close();

private:
    void gestureEvent(QGestureEvent *event);
    QSwipeGesture::SwipeDirection swipeAngleToDirection( int angle );
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
private:
    HbListView      *mList;
    HbFrameItem     *mFrameItem;
};

#endif // MPTRACKLISTWIDGET_H

// EOF
