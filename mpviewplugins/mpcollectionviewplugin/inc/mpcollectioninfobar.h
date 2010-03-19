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
* Description: Music Player collection container information bar.
*
*/

#ifndef MPCOLLECTIONINFOBAR_H
#define MPCOLLECTIONINFOBAR_H


#include <hbwidget.h>

class HbLabel;
class HbDocumentLoader;

class MpCollectionInfoBar : public HbWidget
{
    Q_OBJECT

public:

    explicit MpCollectionInfoBar( QGraphicsItem *parent=0 );
    virtual ~MpCollectionInfoBar();

    void setText( const QString &textLeft );
    void setText( const QString &textLeft, const QString &textRight );
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
        QWidget *widget=0 );

public slots:

    void orientationChange( Qt::Orientation orientation );

private:

    HbDocumentLoader        *mDocumentLoader;   // Own
    HbWidget                *mContainer;        // Own
    HbLabel                 *mTextLeft;
    HbLabel                 *mTextRight;
    bool                    mSingleText;
    Qt::Orientations        mOrientation;

};

#endif  // MPCOLLECTIONINFOBAR_H
