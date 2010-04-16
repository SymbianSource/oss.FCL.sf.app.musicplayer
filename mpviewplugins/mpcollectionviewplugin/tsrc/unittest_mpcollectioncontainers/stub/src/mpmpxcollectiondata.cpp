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
* Description: MpMpxCollectionData stub for testing mpcollectioncontainers
*
*/

#include "mptrace.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include <mpxmedia.h>


/*!
 Stub function.
 */
MpMpxCollectionData::MpMpxCollectionData()
    :mContainerMedia(0),
    mMediaSet(EFalse),
    mContext (ECollectionContextUnknown)
{   
}

/*!
 Stub function.
*/
MpMpxCollectionData::~MpMpxCollectionData()
{   
}

/*!
 Stub function.
*/
TCollectionContext MpMpxCollectionData::context()
{
    return mContext;
}

/*!
 Stub function.
*/
QString MpMpxCollectionData::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    Q_UNUSED(index);
    if ( type == MpMpxCollectionData::Uri ) {
        return QString("Uri");
    }
    else {
        return QString();
    }
}
/*!
 Stub function.
*/
void MpMpxCollectionData::setMpxMedia( const CMPXMedia& entries )
{
    delete mContainerMedia;
    mContainerMedia = 0;
    mContainerMedia = CMPXMedia::NewL(entries);
    mMediaSet = ETrue;
}

/*!
 Stub function.
*/
const CMPXMedia& MpMpxCollectionData::containerMedia()
{
    return *mContainerMedia;
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
QString MpMpxCollectionData::collectionTitle() const
{
    QString title("Title");
    return title;
}
//end of file
