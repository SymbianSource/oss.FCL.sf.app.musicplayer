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
* Description: Music Player collection data.
*
*/

#include "mpmpxcollectiondata.h"
#include "mpmpxcollectiondata_p.h"

/*!
    \class MpMpxCollectionData
    \brief Music Player collection data.

    Collection data provides access to collection data needed by the view.
    Every item of data can be accessed via an index and its details by specifying
    the data type of the selected data item.
*/

/*!
    \enum MpMpxCollectionData::DataType

    This enum defines available data types.
 */

/*!
    \fn void contextChanged( TCollectionContext context )

    This signal is emitted when a new data set is available and the
    collection context is changed.

    \sa setMpxMedia()
 */

/*!
 Constructs the collection data.
 */
MpMpxCollectionData::MpMpxCollectionData( QObject *parent )
    : QObject(parent)
{
    d_ptr = new MpMpxCollectionDataPrivate(this);
}

/*!
 Destructs the collection data.
 */
MpMpxCollectionData::~MpMpxCollectionData()
{
    delete d_ptr;
}

/*!
 Returns the current collection context.
 */
TCollectionContext MpMpxCollectionData::context() const
{
    return d_ptr->context();
}

/*!
 Returns the number of data entries.
 */
int MpMpxCollectionData::count() const
{
    return d_ptr->count();
}

/*!
 Returns the \a title of the collection data based on the current context.
 */
QString MpMpxCollectionData::collectionTitle() const
{
    return d_ptr->collectionTitle();
}

/*!
 Returns the data for the specified \a index and \a type.
 */
QString MpMpxCollectionData::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    return d_ptr->itemData(index, type);
}

/*!
 Returns if the current container is of type 'Auto playlist'.
 */
bool MpMpxCollectionData::isAutoPlaylist()
{
    return d_ptr->isAutoPlaylist();
}

/*!
 Returns if the item specified by \a index is of type 'Auto playlist'.
 */
bool MpMpxCollectionData::isAutoPlaylist( int index )
{
    return d_ptr->isAutoPlaylist(index);
}

/*!
 Returns the count of the items contained within item specified by \a index.
 */
int MpMpxCollectionData::itemCount( int index )
{
    return d_ptr->itemCount(index);
}

/*!
 Returns the current container id.
 */
int MpMpxCollectionData::containerId()
{
    return d_ptr->containerId();
}

/*!
 Returns id of the item specified by \a index
 */
int MpMpxCollectionData::itemId(int index)
{
    return d_ptr->itemId(index);
}

/*!
 Removes item specified by \a index, also internally caches the item for drag
 and drop operations.
 
 \sa testCachedItem() insertCachedItem()
 */
void MpMpxCollectionData::removeItem(int index)
{
    d_ptr->removeItem(index);
}

/*!
 Test wether the cached item matches \a itemId.
 \sa removeItem() insertCachedItem()
 */
bool MpMpxCollectionData::testCachedItem( int itemId )
{
    return d_ptr->testCachedItem( itemId );
}

/*!
 Inserts the cached item in the location specified by \a index
 \sa removeItem() testCachedItem()
 */
void MpMpxCollectionData::insertCachedItem(int index)
{
    return d_ptr->insertCachedItem( index );
}

/*!
 Sets the media \a entries from the MPX framework.
 Internal usage only from MpMpxFrameworkWrapper.
 */
void MpMpxCollectionData::setMpxMedia( const CMPXMedia& entries )
{
    d_ptr->setMpxMedia(entries);
}

/*!
 Returns the container media from the MPX framework.
 Internal usage only from MpMpxFrameworkWrapper.


 \sa setMpxMedia()
 */
const CMPXMedia& MpMpxCollectionData::containerMedia()
{
    return d_ptr->containerMedia();
}

