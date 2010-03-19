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
* Description: Music Player collection view base container definition.
*
*/

#ifndef MPCOLLECTIONCONTAINER_H
#define MPCOLLECTIONCONTAINER_H


#include <QObject>

#include <hbwidget.h>
#include <hbeffect.h>

#include "mpmpxcollectionviewdefs.h"
#include "mpcommondefs.h"

class HbDocumentLoader;
class MpCollectionDataModel;
class MpMpxCollectionData;

class MpCollectionContainer : public HbWidget
{
    Q_OBJECT

public:

    virtual ~MpCollectionContainer();

    virtual void initialize() = 0;
    virtual void setDataModel( MpCollectionDataModel *dataModel );
    virtual void orientationChange( Qt::Orientation orientation );
    void setViewMode( MpCommon::MpViewMode viewMode );

signals:

    void itemActivated( int index );
    void itemLongPressed( int index, const QPointF &coords );

protected:

    explicit MpCollectionContainer( HbDocumentLoader *loader, QGraphicsItem *parent=0 );
    virtual void setupContainer() = 0;

protected:

    HbDocumentLoader        *mDocumentLoader;   // Not own
    MpCollectionDataModel   *mDataModel;        // Not own
    MpMpxCollectionData     *mCollectionData;   // Not own
    TCollectionContext      mCollectionContext;
    MpCommon::MpViewMode    mViewMode;

};

#endif  // MPCOLLECTIONCONTAINER_H

