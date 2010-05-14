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
* Description: Music Player engine.
*
*/

#ifndef MPENGINE_H
#define MPENGINE_H

#include <QObject>
#include "mpmpxcollectionviewdefs.h"
#include "mpmpxcommondefs.h"
#include "mpcommondefs.h"


class QStringList;
class QTranslator;
class HbPopup;
class MpMpxHarvesterFrameworkWrapper;
class MpSongScanner;
class MpMediaKeyHandler;
class MpMpxCollectionFrameworkWrapper;
class MpMpxPlaybackFrameworkWrapper;
class MpMpxCollectionData;
class MpPlaybackData;
class XQSharableFile;

#if defined(BUILD_MPENGINE_LIB)
#define MPENGINE_EXPORT Q_DECL_EXPORT
#else
#define MPENGINE_EXPORT Q_DECL_IMPORT
#endif


class MPENGINE_EXPORT MpEngine : public QObject
{
    Q_OBJECT

private:

    enum UsbBlockingState {
        USB_NotConnected,          // Not connected
        USB_Connected,             // Connected in MTP mode but not synchronizing
        USB_Synchronizing          // Connected in MassStorage mode or MTP mode and synchronizing
    };

private:
    explicit MpEngine();

public:

    virtual ~MpEngine();
    static MpEngine *instance();

    void initialize(MpCommon::MpViewMode viewMode, TUid uid = TUid::Uid( MpCommon::KMusicPlayerUid ) );
    void close();
	
    // Harvester related
    bool verifyUsbBlocking( bool showMessage = false );
    void checkForSystemEvents();
    
    // Collection related
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

    
    void previewItem( int index );
    void openIsolatedCollection( TCollectionContext context );
    void releaseIsolatedCollection();
    
    MpMpxCollectionData *collectionData();
    
    // Playback related
    
    MpPlaybackData *playbackData();

signals:
    
    // Harvester related
    void libraryAboutToUpdate();
    void libraryUpdated();
    void usbBlocked( bool blocked );
    
    // Collection related
    void collectionPlaylistOpened();
    void playlistSaved( bool success );
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );
    
    void isolatedCollectionOpened( MpMpxCollectionData* collectionData );

public slots:

    // Harvester related
    void refreshLibrary();
    void handleScanStarted();
    void handleScanEnded( int count, int error );
    void handleDiskEvent( MpxDiskEvents event );
    void handleUsbEvent( MpxUsbEvents event );
    void handleOutstandingNoteClosing();
    
    // Collection related
    void reopenCollection();
    void reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal );

	// Playback related
	void playEmbedded( QString aFilename );
	void playEmbedded( const XQSharableFile&  file );
    void playPause();
    void stop();
    void skipForward();
    void skipBackward();
    void setPosition( int position );
    void setShuffle( bool mode );
    void setRepeat( bool mode );
	
private:

    // Harvester related
    void handleUsbMassStorageStartEvent();
    void handleUsbMassStorageEndEvent();
    void handleUsbMtpStartEvent();
    void handleUsbMtpEndEvent();
    void handleUsbMtpNotActive();

    void changeUsbBlockingState( UsbBlockingState state );
    void launchBlockingNote();
    void setOutstandingPopup( HbPopup *popup );

private:

    Q_DISABLE_COPY( MpEngine )

    // Harvesting related
    MpMpxHarvesterFrameworkWrapper       *mMpxHarvesterWrapper;  // Own
    MpSongScanner                        *mSongScanner;          // Own
    MpMediaKeyHandler                    *mMediaKeyHandler;      // Own
    HbPopup                              *mUsbOutstandingNote;   // Own
    
    // Collection related
    MpMpxCollectionFrameworkWrapper      *mMpxCollectionWrapper; //Own

    // Playback related
    MpMpxPlaybackFrameworkWrapper        *mMpxPlaybackWrapper; //Own
        
    // General
    QTranslator                          *mMpTranslator;         // Own
    UsbBlockingState                     mUsbBlockingState;
    UsbBlockingState                     mPreviousUsbState;    
    MpCommon::MpViewMode                 mViewMode;
    TUid                                 mHostUid;

};

#endif // MPENGINE_H
