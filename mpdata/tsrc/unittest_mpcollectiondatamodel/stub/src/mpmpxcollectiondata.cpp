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
* Description: MpMpxCollectionData stub for testing MpCollectionDataModel
*
*/

#include "mptrace.h"
#include "stub/inc/mpmpxcollectiondata.h"


int gInitCounter = 0;
int gItemDataCount = 0;
int gRemoveItemCount = 0;
int gCachedItemId = 0;
int gInsertCachedItemCount = 0;

/*!
 Returns gInitCounter.
 gInitCounter counts the number of constructor/destructor calls.
 */
int MpMpxCollectionData::getInitCounter()
{
    return gInitCounter;
}

/*!
 Resets gInitCounter to zero.
 */
void MpMpxCollectionData::resetInitCounter()
{
    gInitCounter = 0;
}

/*!
 Returns gItemDataCount.
 gItemDataCount counts the number of itemData() calls.
 */
int MpMpxCollectionData::getItemDataCounter()
{
    return gItemDataCount;
}

/*!
 Resets gItemDataCount to zero.
 */
void MpMpxCollectionData::resetItemDataCounter()
{
    gItemDataCount = 0;
}

/*!
 Returns gItemDataCount.
 gRemoveItemCount counts the number of itemData() calls.
 */
int MpMpxCollectionData::getremoveItemCounter()
{
    return gRemoveItemCount;
}

/*!
Resets gRemoveItemCount to zero.
*/
void MpMpxCollectionData::resetremoveItemCounter()
{
    gRemoveItemCount = 0;
}

/*!
Sets gCachedItemId.
*/
void MpMpxCollectionData::setCachedItemId(int id)
{
    gCachedItemId = id;
}

/*!
 Returns gInsertCachedItemCount.
 */
int MpMpxCollectionData::getInsertCachedItemCounter()
{
    return gInsertCachedItemCount;
}

/*!
 Resets gInsertCachedItemCount to zero.
 */
void MpMpxCollectionData::resetInsertCachedItemCounter()
{
    gInsertCachedItemCount = 0;
}

/*!
 Stub function.
 */
MpMpxCollectionData::MpMpxCollectionData( QObject *parent )
    : QObject(parent),
      mContext(ECollectionContextUnknown),
      mCount(0),
      mItemDataReturn(true)
{
    gInitCounter++;
}

/*!
 Stub function.
 */
MpMpxCollectionData::~MpMpxCollectionData()
{
    gInitCounter--;
}

/*!
 Stub function.
 */
TCollectionContext MpMpxCollectionData::context() const
{
    return mContext;
}

/*!
 Stub function.
 */
int MpMpxCollectionData::count() const
{
    return mCount;
}

/*!
 Stub function.
 */
QString MpMpxCollectionData::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    if ( index < 0 || index > mCount ) {
        TX_LOG_ARGS("index=" << index);
        qFatal("MpMpxCollectionData::itemData called with index out of range");
    }
    gItemDataCount++;
    QString data;
    if ( !mItemDataReturn && (type != MpMpxCollectionData::Count) ) {
        return data;
    }
    switch ( type ) {
        case MpMpxCollectionData::Title:
            data = QString("Title");
            break;
        case MpMpxCollectionData::Artist:
            data = QString("Artist");
            break;
        case MpMpxCollectionData::Album:
            data = QString("Album");
            break;
        case MpMpxCollectionData::Count:
            data = QString("Count");
            break;
        case MpMpxCollectionData::AlbumArtUri:
            data = QString("AlbumArtUri");
            break;
        default:
            qFatal("MpMpxCollectionData::itemData called with unknown type");
            break;
    }
    return data;
}

/*!
 Stub function.
 */
int MpMpxCollectionData::containerId()
{
    return 65535;
}

/*!
 Stub function.
 */
int MpMpxCollectionData::itemId(int index)
{
    return 2000+index;
}

/*!
 Stub function.
 */
void MpMpxCollectionData::removeItem(int index)
{
    Q_UNUSED(index);
    gRemoveItemCount++;
    return ;
}

/*!
 Stub function.
 */
bool MpMpxCollectionData::testCachedItem( int itemId )
{
    return gCachedItemId == itemId;
}

/*!
 Stub function.
 */
void MpMpxCollectionData::insertCachedItem(int index)
{
    Q_UNUSED(index);
    gInsertCachedItemCount++;
    return ;
}
