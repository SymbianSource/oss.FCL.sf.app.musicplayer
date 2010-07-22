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
* Description: Reflection effect for Music Player Media Wall.
*
*/

#include <QPainter>
#include <QGraphicsItem>
#include <hbinstance.h>

#include "mpreflectioneffect.h"


/*!
    \class MpReflectionEffect
    \brief The MpReflectionEffect class provides a reflection effect.

    Renders the source with a reflection.
*/

/*!
    Constructs a new MpReflectionEffect instance.
    The \a parent parameter is passed to QGraphicsEffect's constructor.
*/
MpReflectionEffect::MpReflectionEffect(QObject *parent)
    : QGraphicsEffect( parent )
{
}

/*!
    Destroys the effect.
*/
MpReflectionEffect::~MpReflectionEffect()
{
}

/*!
    \reimp
*/
QRectF MpReflectionEffect::boundingRectFor(const QRectF &rect) const
{
    QRectF retRect = rect;
    //this bounding rect is in device cordinates, correcting based on current transform.
    if ( hbInstance->allMainWindows()[0]->orientation() == Qt::Horizontal ) {
#ifdef __WINS__        
        retRect.adjust(0,0,retRect.width(),0); //wincw with forced rotation is to the right.
#else
        retRect.adjust(-retRect.width(),0,0,0);//currently hardware rotations is to the left.
#endif
    } else {
        retRect.adjust(0,0,0,retRect.height());
    }
    
    return retRect;
}

/*!
    \reimp
*/
void MpReflectionEffect::draw(QPainter *painter)
{
    //currently sourceBoundingRect returns somehing like boundingRectFor and 
    //wrong rect for widgets with scroll area. As a workaround we can get the
    //original bounding rect assuming that the widget using the effect is the 
    //effect parent.
    QGraphicsItem * sourceItem;
    sourceItem = qobject_cast<QGraphicsItem *>( parent() );
    QPixmap pixmap;
    QPoint offset;
    pixmap = sourcePixmap( Qt::LogicalCoordinates,&offset, QGraphicsEffect::NoPad );
    painter->drawPixmap( offset, pixmap );
    if ( sourceItem ) {
        painter->save();
        painter->scale( 1, -1 );
        //for some reson tranlation does not work, translating cordinates by hand.
        QPointF reflectionOffset( 0.0, - 2.0 * sourceItem->boundingRect().height() );
        painter->drawPixmap( offset + reflectionOffset, pixmap );
        QRectF reflectionRect;
        reflectionRect.setTopLeft( reflectionOffset );
        reflectionRect.setHeight ( sourceItem->boundingRect().height() );
        reflectionRect.setWidth( pixmap.width() );
        painter->fillRect( reflectionRect, QColor::fromRgbF( 0, 0, 0, 0.5f ) );
        painter->restore();
    }
}

// EOF
