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

// INCLUDE FILES
#include <QtCore>
#include <QGraphicsGridLayout>

#include <hblistview.h>
#include <hbabstractviewitem.h>
#include <hblistviewitem.h>
#include <hbscrollbar.h>

#include "mpcollectionlistcontainer.h"
#include "mpcollectiondatamodel.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
    \class MpCollectionListContainer
    \brief Music Player collection view base list container definition.

    Collection container is a base container that provides interface to
    be implemented by the specific collection context containers, e.g.
    All Songs, Artists, etc. Collection container owns UI elements such
    as the layout and the widgets.
*/

/*!
 Constructs the collection container.
 */
MpCollectionListContainer::MpCollectionListContainer( HbDocumentLoader *loader, QGraphicsItem *parent )
    : MpCollectionContainer(loader, parent),
      mList(0),
      mEffectOnGoing(false)
{
    TX_LOG
}

/*!
 Destructs the collection container.
 */
MpCollectionListContainer::~MpCollectionListContainer()
{
    TX_LOG
}

/*!
 Initializes the collection container. Allocates all resources needed by the
 container.
 */
void MpCollectionListContainer::initialize()
{
    TX_ENTRY
    HbEffect::add(QString("viewItem"), QString(":/effects/select.fxml"),
        QString("chosen") );
    HbEffect::add(QString("viewItem"), QString(":/effects/select_end.fxml"),
        QString("chosenEnd") );
    TX_EXIT
}

/*!
 Sets the data model for the container.
 */
void MpCollectionListContainer::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY
    MpCollectionContainer::setDataModel(dataModel);
    mList->setModel(0);
    mList->setModel(dataModel);
    TX_EXIT
}

/*!
 Slot to be called when an item is selected by the user.
 Start animation.
 */
void MpCollectionListContainer::itemActivated( const QModelIndex &index )
{
    TX_ENTRY_ARGS("index=" << index.row());
    if ( !mEffectOnGoing ) {
        HbAbstractViewItem *listViewItem = mList->itemByIndex(index);
        mEffectOnGoing = true;
        mChosenIndex = index;
        HbEffect::start(listViewItem, QString("viewItem"), QString("chosen"),
            this, "itemChosenFxComplete1");
    }
    TX_EXIT
}

/*!
 Slot to be called when an item is long pressed by the user.
 */
void MpCollectionListContainer::onLongPressed( HbAbstractViewItem *listViewItem, const QPointF &coords )
{
    TX_ENTRY
    emit MpCollectionContainer::itemLongPressed(listViewItem->modelIndex().row(), coords);
    TX_EXIT
}

/*!
 Slot for item selected effects part 1.
 */
void MpCollectionListContainer::itemChosenFxComplete1(
    const HbEffect::EffectStatus &status )
{
    Q_UNUSED(status);
    HbAbstractViewItem *listViewItem = mList->itemByIndex(mChosenIndex);
    HbEffect::start(listViewItem, QString("viewItem"), QString("chosenEnd"),
        this, "itemChosenFxComplete2");
}

/*!
 Slot for item selected effects part 2. The end.
 */
void MpCollectionListContainer::itemChosenFxComplete2(
    const HbEffect::EffectStatus &status )
{
    Q_UNUSED(status);
    mEffectOnGoing = false;
    emit MpCollectionContainer::itemActivated( mChosenIndex.row() );
}


/*!
 \internal
 */
 void MpCollectionListContainer::initializeList()
 {
    mList->setItemRecycling(true);
    mList->setScrollingStyle( HbListView::PanOrFlick );
    mList->setClampingStyle( HbListView::BounceBackClamping );
    mList->setLongPressEnabled(true);

    connect(mList, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
    connect(mList, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)), this, SLOT(onLongPressed(HbAbstractViewItem*, QPointF)));

    HbScrollBar *scrollbar = mList->verticalScrollBar();
    scrollbar->show();
    scrollbar->setInteractive(true);
    mList->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);

    mList->listItemPrototype()->setGraphicsSize(HbListViewItem::Thumbnail);
}

