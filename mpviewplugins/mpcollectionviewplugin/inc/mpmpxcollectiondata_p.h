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
* Description: Music Player collection data - private implementation.
*
*/

#ifndef MPMPXCOLLECTIONDATA_P_H
#define MPMPXCOLLECTIONDATA_P_H

#include <e32base.h>

#include "mpmpxcollectiondata.h"
#include "mpmpxcollectionviewdefs.h"

class CMPXMedia;
class CMPXMediaArray;


class MpMpxCollectionDataPrivate
{
public:

    explicit MpMpxCollectionDataPrivate( MpMpxCollectionData *wrapper );
    virtual ~MpMpxCollectionDataPrivate();

    TCollectionContext context() const;

    int count() const;
    QString collectionTitle() const;
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;

    bool isAutoPlaylist();
    bool isAutoPlaylist( int index );
    int itemCount( int index );
    int containerId();
    int itemId(int index);
    void removeItem(int index);
    bool testCachedItem( int itemId );
    void insertCachedItem(int index);

    void setMpxMedia( const CMPXMedia& entries );
    const CMPXMedia& containerMedia();

private:

    void DoGetDataL( int index, MpMpxCollectionData::DataType type, QString& data ) const;
    bool DoIsAutoPlaylistL();
    bool DoIsAutoPlaylistL( int index );
    int DoGetItemCountL( int index );
    int DoGetContainerIdL();
    int DoGetItemIdL( int index );
    void DoRemoveItemL( int index );
    bool DoTestCachedItemL( int itemId );
    
    void SetCollectionContextL();
    void DoSetMpxMediaL( const CMPXMedia& entries );

private:

    MpMpxCollectionData     *q_ptr;

    CMPXMedia               *iContainerMedia;       // Owned
    CMPXMediaArray          *iMediaArray;           // Not owned

    TCollectionContext      iContext;
    
    CMPXMedia               *iCachedRemovedItem; //Owned

};

#endif // MPMPXCOLLECTIONDATA_P_H

