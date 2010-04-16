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
* Description: Wrapper for mpx framework utilities.
*
*/

#ifndef MPMPXFRAMEWORKWRAPPER_H
#define MPMPXFRAMEWORKWRAPPER_H

#include <QObject>

#include "mpmpxcollectionviewdefs.h"
#include "mpcommondefs.h"

class MpMpxFrameworkWrapperPrivate;
class MpMpxCollectionData;
class QStringList;


class MpMpxFrameworkWrapper : public QObject
{
    Q_OBJECT

    friend class MpMpxFrameworkWrapperPrivate;

public:

    explicit MpMpxFrameworkWrapper( 
        MpCommon::MpViewMode viewMode=MpCommon::DefaultView, QObject *parent=0 );
    virtual ~MpMpxFrameworkWrapper();

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
    
signals:

    void collectionPlaylistOpened();
    void playlistSaved( bool success );
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );
    
    void isolatedCollectionOpened( MpMpxCollectionData* collectionData );

public slots:

    void reopenCollection();
    void reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal );

private:

    Q_DISABLE_COPY( MpMpxFrameworkWrapper )
    MpMpxFrameworkWrapperPrivate *d_ptr;

};

#endif // MPMPXFRAMEWORKWRAPPER_H
