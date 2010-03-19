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

#include <QtCore>

#include <hbabstractviewitem.h>

#include "mpcollectioncontainer.h"
#include "mpcollectiondatamodel.h"
#include "mpmpxcollectiondata.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainer
    \brief Music Player collection view base container definition.

    Collection container is a base container that provides interface to
    be implemented by the specific collection context containers, e.g.
    All Songs, Artists, etc.
*/

/*!
    \fn void itemActivated( int index );

    This signal is emitted when an item of the container with \a index is activated.
 */

/*!
    \fn void itemLongPressed( int index, const QPointF &coords );

    This signal is emitted when an item of the container with \a index is long pressed.
 */

/*!
 Constructs the collection container.
 */
MpCollectionContainer::MpCollectionContainer( HbDocumentLoader *loader, QGraphicsItem *parent )
    : HbWidget(parent),
      mDocumentLoader(loader),
      mCollectionContext(ECollectionContextUnknown)
{
    TX_LOG
}

/*!
 Destructs the collection container.
 */
MpCollectionContainer::~MpCollectionContainer()
{
    TX_LOG
}

/*!
 Sets the data model for the container.
 */
void MpCollectionContainer::setDataModel( MpCollectionDataModel *dataModel )
{
    TX_ENTRY
    mDataModel = dataModel;
    mCollectionData = mDataModel->collectionData();
    mCollectionContext = mCollectionData->context();

    setupContainer();
    TX_EXIT
}

/*!
 Loads the correct layout based on the \a orientation.
 */
void MpCollectionContainer::orientationChange( Qt::Orientation orientation )
{
    Q_UNUSED(orientation);
}

/*!
 Sets the view mode.
 */
void MpCollectionContainer::setViewMode( MpCommon::MpViewMode viewMode )
{
    mViewMode = viewMode;
}

