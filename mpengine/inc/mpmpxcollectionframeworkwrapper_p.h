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
* Description: Wrapper for mpx collection framework utilities - private implementation.
*
*/

#ifndef MPMPXCOLLECTIONFRAMEWORKWRAPPER_P_H
#define MPMPXCOLLECTIONFRAMEWORKWRAPPER_P_H

#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelperobserver.h>

#include "mpmpxisolatedcollectionhelper.h" //MMpMpxIsolatedCollectionHelperObserver
#include "mpmpxcollectionviewdefs.h"


class MMPXCollectionUtility;
class MMPXCollectionUiHelper;
class CMPXCollectionOpenUtility;
class MMPXPlaybackUtility;
class MpMpxCollectionData;
class MpMpxCollectionFrameworkWrapper;
class QStringList;


class MpMpxCollectionFrameworkWrapperPrivate : public MMPXCollectionObserver,
                                     public MMPXCHelperObserver,
                                     public MMpMpxIsolatedCollectionHelperObserver,
                                     public MMPXCollectionFindObserver
{

private:
    //Keep in sync with TMCBrowseType in mpxcollectiondb.hrh
    enum MpBrowseType{
        BrowseAll = 0,
        BrowsePlaylist,
        BrowseArtist,
        BrowseAlbum,
        BrowsePodcasts,
        BrowseGenre,
        BrowseComposer,        
        BrowseAlbumSong, 
        BrowseAlbumMediaWall 
    };
    
public:

    explicit MpMpxCollectionFrameworkWrapperPrivate( MpMpxCollectionFrameworkWrapper *wrapper );
    virtual ~MpMpxCollectionFrameworkWrapperPrivate();

    void init( TUid hostUid );
    void openCollection( TCollectionContext context );
    void openCollectionItem( int index );
    void reopenCollection();
    void back();
    void findPlaylists( QStringList &playlists );
    void createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData *collectionData );
    void saveToPlaylist( int playlistIndex, QList<int> &selection );
    void saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData );    
    void deleteSongs( QList<int> &selection );
    void renamePlaylist( QString &newName, int index );
    void renamePlaylist( QString &newName );
    void setShuffle( bool active );
    void previewItem( int index );
    void openIsolatedCollection( TCollectionContext context );
    void releaseIsolatedCollection();
    void reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal );
    void findAlbumSongs( int index );
    void playAlbumSongs( int albumIndex, int songIndex, MpMpxCollectionData* collectionData );
    void setRepeatFeatureEnabled( bool enable );
    void setShuffleFeatureEnabled( bool enable );
	void cancel();
	
    MpMpxCollectionData *collectionData();

private:


    void HandleOpenL( const CMPXMedia& aEntries,
                     TInt aIndex,TBool aComplete,TInt aError );
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,TInt aError );
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );
    void HandleOperationCompleteL( TCHelperOperation aOperation, TInt aErr, void* aArgument );
    void HandleIsolatedOpenL( const CMPXMedia& aEntries, TInt aError );
    void HandleFindAllL( const CMPXMedia& aResults, TBool aComplete, TInt aError );

    void DoInitL();
    void DoOpenCollectionL( TCollectionContext aContext );
    void DoOpenCollectionItemL( TInt aIndex );
    void DoIncrementalOpenL();
    void DoReopenCollectionL();
    void DoBackL();
    void DoFindPlaylistsL( QStringList &playlists );
    void DoSaveToPlaylistL( TMPXItemId playlistId, QList<int> &selection, MpMpxCollectionData *collectionData );
    void DoCreatePlaylistL( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData );
    void DoSaveToPlaylistL( int playlistIndex, QList<int> &selection );
    
    void DoDeleteSongsL( QList<int> &selection );
    void DoDeleteAlbumSongsL( QList<int> &selection );
    void DoRenamePlaylistL( QString &newName, int index );
    void DoRenamePlaylistL( QString &newname );
    void DoRenamePlaylistL( TMPXItemId id, QString &newName );
    void DoSaveToCurrentPlaylistL( QList<int> &selection, MpMpxCollectionData *collectionData );
    void DoSetShuffleL( bool active );
    void DoPreviewSongL( int index );
    void DoPreviewAlbumSongL( int index );
    void DoOpenIsolatedCollectionL( TCollectionContext context );
    void DoReorderPlaylistL( int playlistId, int songId, int originalOrdinal, int newOrdinal );
    void DoFindAlbumSongsL( int index );
    void DoPlayAlbumSongsL( int albumIndex, int songIndex, MpMpxCollectionData* collectionData );

    void DoHandleCollectionMessageL( const CMPXMessage& aMsg );
    void DoHandleItemChangedMessageL( const CMPXMessage& aMsg );

    void PreparePlaylistMediaL( CMPXMedia& aMedia, QList<int> &selection, MpMpxCollectionData *collectionData );
    void createPlaybackUtilityL();

private:

    MpMpxCollectionFrameworkWrapper   *q_ptr;

    MMPXCollectionUtility             *iCollectionUtility;
    MMPXCollectionUiHelper            *iCollectionUiHelper;
    CMPXCollectionOpenUtility         *iIncrementalOpenUtil;
    CMpMpxIsolatedCollectionHelper    *iIsolatedCollectionHelper;
    MMPXPlaybackUtility               *iPlaybackUtility;
    MpMpxCollectionData               *iCollectionData;         // Owned
    MpMpxCollectionData               *iIsolatedCollectionData; // Owned
    TBool                             iFirstIncrementalOpen;
    CMPXMedia                         *iUserPlaylists;          // Owned
    TInt                              iNumItemsAdded;
    TUid                              mHostUid;
    TBool                             iRepeatFeature;
    TBool                             iShuffleFeature;
    TBool                             iReopen;

};

#endif // MPMPXCOLLECTIONFRAMEWORKWRAPPER_P_H

