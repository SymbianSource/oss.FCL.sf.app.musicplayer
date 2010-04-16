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
* Description: Music Player collection data model stub for testing mpcollectioncontainers.
*
*/


#include "stub/inc/mpcollectiondatamodel.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include "mptrace.h"


/*!
 Stub function
 */
MpCollectionDataModel::MpCollectionDataModel( MpMpxCollectionData *data, QObject *parent )
    : QAbstractListModel(parent),
      mCollectionData(data)
{
    TX_ENTRY
    TX_EXIT
}

/*!
 Stub function
 */
MpCollectionDataModel::~MpCollectionDataModel()
{
    TX_ENTRY
    TX_EXIT
}

/*!
 Stub function
 */
int MpCollectionDataModel::rowCount( const QModelIndex &parent ) const
{
    TX_LOG
    Q_UNUSED(parent);
    if ( mCachingInProgress ) {
        return 0;
    }
    else {
        return mRowCount;
    }
}

/*!
 Stub function
 */
QVariant MpCollectionDataModel::data(const QModelIndex &index, int role) const
{
    TX_ENTRY
    Q_UNUSED(index);
    Q_UNUSED(role);
    QVariant returnValue = QVariant();
    return returnValue;
    TX_EXIT
}

/*!
 Stub function
 */
void MpCollectionDataModel::refreshModel()
{
    TX_ENTRY
    TX_EXIT
}

/*!
 Stub function
 */
MpMpxCollectionData *MpCollectionDataModel::collectionData()
{
    return mCollectionData;
}

