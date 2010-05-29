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

#include <QHash>

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
    int itemId( int index);
    int albumSongId( int index);
    void removeItem( int index);
    bool testCachedItem( int itemId );
    void insertCachedItem( int index);

    bool setCurrentAlbum( int index );
    int currentAlbumIndex() const;
    int albumSongsCount() const;
    QString albumSongData( int index, MpMpxCollectionData::DataType type ) const;

    void setMpxMedia( const CMPXMedia& entries, bool reopen );
    const CMPXMedia& containerMedia();
    void setContext( TCollectionContext context );
    void setAlbumContent( const CMPXMedia& albumContent );
    int itemIndex( int itemUniqueId );
    int albumSongIndex( int songUniqueId );

private:
    void loadAlbumsLookup();
    void loadAlbumSongsLookup();
    void DoGetDataL( int index, MpMpxCollectionData::DataType type, QString& data ) const;
    bool DoIsAutoPlaylistL();
    bool DoIsAutoPlaylistL( int index );
    int DoGetItemCountL( int index );
    int DoGetContainerIdL();
    int DoGetItemIdL( int index );
    int DoGetAlbumSongIdL( int index );
    void DoRemoveItemL( int index );
    bool DoTestCachedItemL( int itemId );

    bool DoSetCurrentAlbumL( int index );
    void DoGetAlbumSongDataL( int index, MpMpxCollectionData::DataType type, QString& data ) const;

    void SetCollectionContextL();
    void DoSetMpxMediaL( const CMPXMedia& entries );
    void DoSetAlbumContentL( const CMPXMedia& albumContent );

private:

    MpMpxCollectionData     *q_ptr;

    TCollectionContext      iContext;

    CMPXMedia               *iContainerMedia;       // Owned
    CMPXMediaArray          *iMediaArray;           // Not owned
    CMPXMedia               *iCachedRemovedItem;    // Owned

    int                     iCurrentAlbumIndex;
    int                     iAlbumSongCount;
    QHash<int, int>         albumIdIndexMapping;
    QHash<int, int>         albumSongIdIndexMapping;

};

#endif // MPMPXCOLLECTIONDATA_P_H

