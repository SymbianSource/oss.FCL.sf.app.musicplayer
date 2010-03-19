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
* Description: Wrapper for mpx framework utilities - private implementation.
*
*/

#ifndef MPMPXFRAMEWORKWRAPPER_P_H
#define MPMPXFRAMEWORKWRAPPER_P_H

#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelperobserver.h>

#include "mpmpxcollectionviewdefs.h"
#include "mpcommondefs.h"

class MMPXCollectionUtility;
class MMPXCollectionUiHelper;
class CMPXCollectionOpenUtility;
class MMPXPlaybackUtility;
class MMPXHarvesterUtility;
class MpMpxCollectionData;
class MpMpxFrameworkWrapper;
class QStringList;


class MpMpxFrameworkWrapperPrivate : public MMPXCollectionObserver,
                                     public MMPXCHelperObserver
{
public:

    explicit MpMpxFrameworkWrapperPrivate( MpMpxFrameworkWrapper *wrapper );
    virtual ~MpMpxFrameworkWrapperPrivate();

    void init( MpCommon::MpViewMode viewMode );
    void openCollection( TCollectionContext context );
    void openCollectionItem( int index );
    void reopenCollection();
    void back();
    void findPlaylists( QStringList &playlists );
    void createPlaylist( QString playlistName, QList<int> selection );
    void saveToPlaylist( int playlistIndex, QList<int> selection );
    void deleteSongs( QList<int> selection );
    void renamePlaylist( QString newName, int index);
    void renamePlaylist( QString newName);
    void setShuffle( bool active );
    void scan();
    void cancelScan();
    void previewItem( int index );

    MpMpxCollectionData *collectionData();

    void HandleOpenL( const CMPXMedia& aEntries,
                     TInt aIndex,TBool aComplete,TInt aError );
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist,TInt aError );
    void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
    void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

    void HandleOperationCompleteL( TCHelperOperation aOperation, TInt aErr, void* aArgument );

private:

    void DoInitL();
    void DoOpenCollectionL( TCollectionContext aContext );
    void DoOpenCollectionItemL( TInt aIndex );
    void DoIncrementalOpenL();
    void DoReopenCollectionL();
    void DoBackL();
    void DoFindPlaylistsL( QStringList &playlists );
    void DoCreatePlaylistL( QString playlistName, QList<int> selection );
    void DoSaveToPlaylistL( int playlistIndex, QList<int> selection );
    void DoDeleteSongsL( QList<int> selection );
    void DoRenamePlaylistL( QString newName, int index );
    void DoRenamePlaylistL( QString newname );
    void DoRenamePlaylistL( TMPXItemId id, QString newName );
    void DoSetShuffleL( bool active );
    void DoScanL();
    void DoCancelScanL();
    void DoPreviewItemL( int index );
    void DoHandleCollectionMessageL( const CMPXMessage& aMsg );

    void PreparePlaylistMediaL( CMPXMedia& aMedia, QList<int> selection );

private:

    MpMpxFrameworkWrapper       *q_ptr;

    MMPXCollectionUtility       *iCollectionUtility;
    MMPXCollectionUiHelper      *iCollectionUiHelper;
    CMPXCollectionOpenUtility   *iIncrementalOpenUtil;
    MMPXPlaybackUtility         *iPlaybackUtility;
    MMPXHarvesterUtility        *iHarvesterUtility;

    MpMpxCollectionData         *iCollectionData;       // Owned
    TBool                       iFirstIncrementalOpen;

    CMPXMedia                   *iUserPlaylists;        // Owned

    TInt                        iNumItemsAdded;

    MpCommon::MpViewMode        iViewMode;

};

#endif // MPMPXFRAMEWORKWRAPPER_P_H

