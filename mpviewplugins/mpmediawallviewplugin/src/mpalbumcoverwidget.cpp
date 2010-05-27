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
* Description:  Album Cover Widget for Music Player Media Wall.
*
*/
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <hbinstantfeedback.h>

#include "mpalbumcoverwidget.h"


/*!
    Constructs the album cover widget
 */
MpAlbumCoverWidget::MpAlbumCoverWidget( QGraphicsItem *parent ) : 
    QGraphicsWidget( parent )
{
    grabGesture(Qt::TapGesture);
    grabGesture(Qt::SwipeGesture);
}

/*!
    Sets the \a icon as current album cover.
 */
void MpAlbumCoverWidget::setIcon( const HbIcon &icon )
{
    if (icon != mIcon) {
        mIcon = icon;
        mPixmap = QPixmap();
    }
}

/*!
    Sets the \a icon as default cover, to be used in case the album cover is null icon.
 */
void MpAlbumCoverWidget::setDefaultIcon( const HbIcon &icon )
{
    mDefaultIcon = icon;
}

/*!
    \reimp
 */
void MpAlbumCoverWidget::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( widget )
    Q_UNUSED( option )
    //TODO: add a function to set default album art ,and use default when qicon is NULL,
    if ( !mIcon.isNull() ) {
        if ( mPixmap.isNull() ) {
            mPixmap = mIcon.qicon().pixmap( size().toSize() );
        }
        //We paint directly to stretch up/down if necesary.
        painter->drawPixmap( rect(), mPixmap, QRectF() );
    }
    else {
        //We use HbIcon paint to render vector graphics.
        mDefaultIcon.setSize(size());
        mDefaultIcon.paint(painter,rect());
    }
}

/*!
    \reimp
 */
void MpAlbumCoverWidget::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton ) {
        HbInstantFeedback::play( HbFeedback::Basic );
        event->accept();
    }
    else {
        event->ignore();
    }
}

/*!
    \reimp
 */
void MpAlbumCoverWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton ) {
        emit clicked();
        event->accept();
    }
    else {
        event->ignore();
    }
}

//EOF
