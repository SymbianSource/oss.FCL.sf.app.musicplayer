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

#include "mpmpxframeworkwrapper.h"
#include "mpmpxframeworkwrapper_p.h"

/*!
    \class MpMpxFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
    \fn void collectionPlaylistOpened()

    This signal is emitted when A new collection playlist is opened and 
    playback is initialized.

 */

/*!
    \fn void playlistSaved( bool success )

    This signal is emitted when playlist save operation is completed, it 
    indicates the operation \a success .

 */

/*!
    \fn void songsDeleted( bool success )

    This signal is emitted when song delete operation is completed, it 
    indicates the operation \a success .

 */

/*!
    \fn void playlistsRenamed( bool success )

    This signal is emitted when playlist rename operation is completed, it
    indicates the operation  \a success .

 */

/*!
    \fn void isolatedCollectionOpened( MpMpxCollectionData* collectionData )

    This signal is emitted when an isolated collection is opened on \a context

 */
 
 
 
/*!
 Constructs the utility wrapper.
 */
MpMpxFrameworkWrapper::MpMpxFrameworkWrapper( MpCommon::MpViewMode viewMode, QObject *parent )
    : QObject( parent )
{
    d_ptr = new MpMpxFrameworkWrapperPrivate( this );
    d_ptr->init( viewMode );
}

/*!
 Destructs the utility wrapper.
 */
MpMpxFrameworkWrapper::~MpMpxFrameworkWrapper()
{
    delete d_ptr;
}

/*!
 Opens the collection for the given \a context.

 \sa collectionOpened()
 */
void MpMpxFrameworkWrapper::openCollection( TCollectionContext context )
{
    d_ptr->openCollection( context );
}

/*!
 Opens the collection at a specific \a index.

 \sa collectionOpened(), playlistOpened()
 */
void MpMpxFrameworkWrapper::openCollectionItem( int index )
{
    d_ptr->openCollectionItem( index );
}

/*!
 Navigates back to the container of the current items.

 \sa collectionOpened()
 */
void MpMpxFrameworkWrapper::back()
{
    d_ptr->back();
}

/*!
 Loads the user created \a playLists.

 */
void MpMpxFrameworkWrapper::findPlaylists( QStringList &playlists )
{
    d_ptr->findPlaylists( playlists );
}

/*!
 Creates a new playlist with name \a playlistName and adds \a selection and optionally uses \a collectionData.
 */
void MpMpxFrameworkWrapper::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    d_ptr->createPlaylist( playlistName, selection, collectionData );
}

/*!
 Adds \a selection to the playlist specified in \a playlistIndex.
 */
void MpMpxFrameworkWrapper::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    d_ptr->saveToPlaylist( playlistIndex, selection );
}

/*!
 Rename a playlist by \a index with \a name.
 */
void MpMpxFrameworkWrapper::renamePlaylist( QString &newName, int index )
{
    d_ptr->renamePlaylist( newName, index );
}

/*!
 Adds \a selection to the current playlist from the specified \a collectionData.
 */
void MpMpxFrameworkWrapper::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    d_ptr->saveToCurrentPlaylist( selection, collectionData );
}

/*!
 Rename a playlist with \a name.
 */
void MpMpxFrameworkWrapper::renamePlaylist( QString &newName )
{
    d_ptr->renamePlaylist( newName );
}

/*!
 Deletes \a selection from the collection.
 */
void MpMpxFrameworkWrapper::deleteSongs( QList<int> &selection )
{
    d_ptr->deleteSongs( selection );
}

/*!
 Sets the suffle according to \a active.
 */
void MpMpxFrameworkWrapper::setShuffle( bool active )
{
    d_ptr->setShuffle( active );
}

/*!
 Initiate a playback preview for the selected item.
 */
void MpMpxFrameworkWrapper::previewItem( int index )
{
    d_ptr->previewItem( index );
}


/*!
 Opens the an isolated collection with \a context.
 \sa isolatedCollectionOpened()
 */
void MpMpxFrameworkWrapper::openIsolatedCollection( TCollectionContext context )
{
       d_ptr->openIsolatedCollection( context );     
}

/*!
 Releases the resources used for the isolated collection.
 */
void MpMpxFrameworkWrapper::releaseIsolatedCollection()
{
    d_ptr->releaseIsolatedCollection();
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpMpxFrameworkWrapper::collectionData()
{
    return d_ptr->collectionData();
}

/*!
 Slot to be called to reopen the collection in its current state.

 \sa collectionOpened()
 */
void MpMpxFrameworkWrapper::reopenCollection()
{
    d_ptr->reopenCollection();
}

/*!
 Slot to be called to request a reorder operation , indicates that 
 the item with \a playlistId , \a songId is to be moved from 
 \a originalOrdinal to \a newOrdinal.
 */
void MpMpxFrameworkWrapper::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    d_ptr->reorderPlaylist( playlistId, songId, originalOrdinal, newOrdinal );
}


