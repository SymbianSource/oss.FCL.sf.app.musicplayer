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
    \fn void playlistOpened()

    This signal is emitted when

    \sa openCollectionItem()
 */

/*!
 Constructs the utility wrapper.
 */
MpMpxFrameworkWrapper::MpMpxFrameworkWrapper( MpCommon::MpViewMode viewMode, QObject *parent )
    : QObject(parent)
{
    d_ptr = new MpMpxFrameworkWrapperPrivate(this);
    d_ptr->init(viewMode);
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
    d_ptr->openCollection(context);
}

/*!
 Opens the collection at a specific \a index.

 \sa collectionOpened(), playlistOpened()
 */
void MpMpxFrameworkWrapper::openCollectionItem( int index )
{
    d_ptr->openCollectionItem(index);
}

/*!
 Opens the collection in its current state.

 \sa collectionOpened()
 */
void MpMpxFrameworkWrapper::reopenCollection()
{
    d_ptr->reopenCollection();
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
    d_ptr->findPlaylists(playlists);
}

/*!
 Creates a new playlist with name \a playlistName and adds \a selection.
 */
void MpMpxFrameworkWrapper::createPlaylist( QString playlistName, QList<int> selection )
{
    d_ptr->createPlaylist(playlistName, selection);
}

/*!
 Adds \a selection to the playlist specified in \a playlistIndex.
 */
void MpMpxFrameworkWrapper::saveToPlaylist( int playlistIndex, QList<int> selection )
{
    d_ptr->saveToPlaylist(playlistIndex, selection);
}

/*!
 Rename a playlist by \a index with \a name.
 */
void MpMpxFrameworkWrapper::renamePlaylist( QString newName, int index )
{
    d_ptr->renamePlaylist(newName, index);
}

/*!
 Rename a playlist with \a name.
 */
void MpMpxFrameworkWrapper::renamePlaylist( QString newName )
{
    d_ptr->renamePlaylist(newName);
}

/*!
 Deletes \a selection from the collection.
 */
void MpMpxFrameworkWrapper::deleteSongs( QList<int> selection )
{
    d_ptr->deleteSongs(selection);
}

/*!
 Sets the suffle according to \a active.
 */
void MpMpxFrameworkWrapper::setShuffle( bool active )
{
    d_ptr->setShuffle(active);
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpMpxFrameworkWrapper::collectionData()
{
    return d_ptr->collectionData();
}

/*!
 Initiates collection scanning.
 */
void MpMpxFrameworkWrapper::scan()
{
    d_ptr->scan();
}

/*!
 Cancels ongoing collection scanning.
 */
void MpMpxFrameworkWrapper::cancelScan()
{
    d_ptr->cancelScan();
}

/*!
 Initiate a playback preview for the selected item.
 */
void MpMpxFrameworkWrapper::previewItem( int index )
{
    d_ptr->previewItem(index);
}
