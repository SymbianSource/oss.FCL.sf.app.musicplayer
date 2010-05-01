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
* Description: Wrapper for mpx collection framework utilities.
*
*/

#ifndef MPMPXCOLLECTIONFRAMEWORKWRAPPER_H
#define MPMPXCOLLECTIONFRAMEWORKWRAPPER_H

#include <QObject>

#include "mpmpxcollectionviewdefs.h"
#include "mpcommondefs.h"

class MpMpxCollectionFrameworkWrapperPrivate;
class MpMpxCollectionData;
class QStringList;


class MpMpxCollectionFrameworkWrapper : public QObject
{
    Q_OBJECT

    friend class MpMpxCollectionFrameworkWrapperPrivate;

public:

    explicit MpMpxCollectionFrameworkWrapper( 
                MpCommon::MpViewMode viewMode=MpCommon::DefaultView, TUid hostUid = TUid::Uid( MpCommon::KMusicPlayerUid ), QObject *parent=0 );
    virtual ~MpMpxCollectionFrameworkWrapper();

    void openCollection( TCollectionContext context );
    void openCollectionItem( int index );
    void back();
    void findPlaylists( QStringList &playlists );
    void createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData = 0  );
    void saveToPlaylist( int playlistIndex, QList<int> &selection );
    void saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData );    
    void renamePlaylist( QString &newName, int index );
    void renamePlaylist( QString &newName );
    void deleteSongs( QList<int> &selection );
    void setShuffle( bool active );
    
    void previewItem( int index );
    void openIsolatedCollection( TCollectionContext context );
    void releaseIsolatedCollection();
    
    MpMpxCollectionData *collectionData();
    
    void reopenCollection();
    void reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal );
    
signals:

    void collectionPlaylistOpened();
    void playlistSaved( bool success );
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );
    
    void isolatedCollectionOpened( MpMpxCollectionData* collectionData );

private:

    Q_DISABLE_COPY( MpMpxCollectionFrameworkWrapper )
    MpMpxCollectionFrameworkWrapperPrivate *d_ptr;

};

#endif // MPMPXCOLLECTIONFRAMEWORKWRAPPER_H