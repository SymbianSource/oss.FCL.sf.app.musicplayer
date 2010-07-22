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

#include "mpengine.h"

#include "mptrace.h"


MpEngine::MpEngine()
{
    TX_LOG_ARGS("Stub")
}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::refreshLibrary()
{
    TX_ENTRY_ARGS("Stub")
}

bool MpEngine::verifyUsbBlocking( bool showMessage )
{
    TX_ENTRY_ARGS("Stub")
    return false;
}

void MpEngine::checkForSystemEvents()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleScanStarted() 
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleScanEnded( int count, int error ) 
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleDiskEvent( MpxDiskEvents event )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleUsbEvent( MpxUsbEvents event )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::handleBlockingNoteClosing()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::openCollection( TCollectionContext context )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::openCollectionItem( int index )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::back()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::findPlaylists( QStringList &playlists )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::renamePlaylist( QString &newName, int index )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::renamePlaylist( QString &newName )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::deleteSongs( QList<int> &selection )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::previewItem( int index )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::openIsolatedCollection( TCollectionContext context )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::releaseIsolatedCollection()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::findAlbumSongs( int index )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::playAlbumSongs( int albumIndex, int songIndex )
{
    TX_ENTRY_ARGS("Stub")
}

MpMpxCollectionData *MpEngine::collectionData()
{
    TX_ENTRY_ARGS("Stub")
    return false;
}

void MpEngine::reopenCollection()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    TX_ENTRY_ARGS("Stub")
}

MpPlaybackData *MpEngine::playbackData()
{
    TX_ENTRY_ARGS("Stub")
    return false;
}


void MpEngine::playEmbedded( QString aFilename )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::playEmbedded(const XQSharableFile& file )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::playPause()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::stop()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::skipForward()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::skipBackward()
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::setPosition( int position )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::setShuffle( bool mode )
{
    TX_ENTRY_ARGS("Stub")
}

void MpEngine::setRepeat( bool mode )
{
    TX_ENTRY_ARGS("Stub")
}

MpSongData *MpEngine::songData()
{
    TX_ENTRY_ARGS("Stub")
    return 0;
}

void MpEngine::retrieveSong()
{
    TX_ENTRY_ARGS("Stub")
}
