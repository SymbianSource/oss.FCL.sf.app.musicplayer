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
* Description:  reflection effect for Music Player Media Wall.
*
*/
#ifndef MPREFLECTIONEFFECT_H
#define MPREFLECTIONEFFECT_H

#include <QGraphicsEffect>

class MpReflectionEffect: public QGraphicsEffect
{
    Q_OBJECT

public:
    MpReflectionEffect(QObject *parent = 0);
    ~MpReflectionEffect();

    QRectF boundingRectFor(const QRectF &rect) const;

protected:
    void draw(QPainter *painter);

private:
    Q_DISABLE_COPY(MpReflectionEffect)
};

#endif // MPREFLECTIONEFFECT_H

// EOF