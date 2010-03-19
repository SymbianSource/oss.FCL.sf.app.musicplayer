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
* Description: Music Player collection view base list container definition.
*
*/

#ifndef MPCOLLECTIONLISTCONTAINER_H
#define MPCOLLECTIONLISTCONTAINER_H


#include <QObject>

#include <hbwidget.h>
#include <hbeffect.h>

#include "mpcollectioncontainer.h"
#include "mpmpxcollectionviewdefs.h"

class HbListView;
class HbAbstractViewItem;

class MpCollectionListContainer : public MpCollectionContainer
{
    Q_OBJECT

public:

    virtual ~MpCollectionListContainer();

    void initialize();
    void setDataModel( MpCollectionDataModel *dataModel );


public slots:

    virtual void itemActivated( const QModelIndex &index );
    virtual void onLongPressed(HbAbstractViewItem *listViewItem, const QPointF &coords);
    void itemChosenFxComplete1( const HbEffect::EffectStatus &status );
    void itemChosenFxComplete2( const HbEffect::EffectStatus &status );


protected:

    explicit MpCollectionListContainer( HbDocumentLoader *loader, QGraphicsItem *parent=0 );
    virtual void initializeList();

protected:

    HbListView              *mList;
    QModelIndex             mChosenIndex;
    bool                    mEffectOnGoing;

};

#endif  // MPCOLLECTIONLISTCONTAINER_H

