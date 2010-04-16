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

#ifndef MPMPXCOLLECTIONDATA_H
#define MPMPXCOLLECTIONDATA_H

#include "mpmpxcollectionviewdefs.h"

class CMPXMedia;
 

class MpMpxCollectionData 
{

public:

    enum DataType {
        Title,
        Uri,
        Duration,
        Count,
        Artist,
        Album,
        Genre,
        Rating,
        AlbumArtUri
    };

    // Stub functions
     MpMpxCollectionData();
    ~MpMpxCollectionData();

    TCollectionContext context();
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;

    void setMpxMedia( const CMPXMedia& entries );
    const CMPXMedia& containerMedia();
    int count() const;
    QString collectionTitle() const;

public:
    CMPXMedia               *mContainerMedia;
    TBool                   mMediaSet;
    TCollectionContext      mContext;
    int                     mCount;
};

#endif // MPMPXCOLLECTIONDATA_H

