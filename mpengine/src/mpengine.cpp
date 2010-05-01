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

#include <QTranslator>
#include <QLocale>
#include <hbmessagebox.h>
#include <hbprogressdialog.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <xqsharablefile.h>

#include "mpengine.h"
#include "mpmpxharvesterframeworkwrapper.h"
#include "mpmpxcollectionframeworkwrapper.h"
#include "mpmpxplaybackframeworkwrapper.h"
#include "mpsongscanner.h"
#include "mpmediakeyhandler.h"
#include "mptrace.h"

/*!
    \class MpEngine
    \brief Engine for musicplayer - mpx framework utilities.

    MP Engine provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
-------------------------------------------------------------------------------
    Harvesting related signals
-------------------------------------------------------------------------------
*/

/*!
    \fn void scanStarted()

    This signal is emitted when scan operation is started.

 */

/*!
    \fn void scanEnded()

    This signal is emitted when scan operation ends.

 */

/*!
    \fn void scanCountChanged( int count )

    This signal is emitted when scan count is updated.

 */

/*!
    \fn void libraryUpdated()

    This signal is emitted when MpSongScannerHelper ends scanning,
    or USB-MTP Synchronization finishes.

 */

/*!
    \fn void formatStarted()

    This signal is emitted when EMcMsgFormatStart is received from MPXCollectionUtility.

 */

/*!
    \fn void formatEnded()

    This signal is emitted when EMcMsgFormatEnd is received from MPXCollectionUtility.

 */

/*!
    \fn void diskRemoved()

    This signal is emitted when EMcMsgDiskRemoved is received from MPXCollectionUtility.

 */

/*!
    \fn void diskInserted()

    This signal is emitted when EMcMsgDiskInserted is received from MPXCollectionUtility.

 */

/*!
    \fn void usbMassStorageStarted()

    This signal is emitted when EMcMsgUSBMassStorageStart is received from MPXCollectionUtility.

 */

/*!
    \fn void usbMassStorageEnded()

    This signal is emitted when EMcMsgUSBMassStorageEnd is received from MPXCollectionUtility.

 */

/*!
    \fn void usbMtpStarted()

    This signal is emitted when EMcMsgUSBMtpStart is received from MPXCollectionUtility.

 */

/*!
    \fn void usbMtpEnded()

    This signal is emitted when EMcMsgUSBMtpEnd is received from MPXCollectionUtility.

 */


/*!
-------------------------------------------------------------------------------
    Collection related signals
-------------------------------------------------------------------------------
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
 Constructs music player engine.
 */
MpEngine::MpEngine()
    : mMpxHarvesterWrapper(0),
      mSongScanner(0),
      mMediaKeyHandler(0),
      mUsbOutstandingNote(0),
      mMpxCollectionWrapper(0),
      mMpxPlaybackWrapper(0),
      mMpTranslator(0),
      mUsbBlockingState(USB_NotConnected),
      mPreviousUsbState(USB_NotConnected),
      mViewMode(MpCommon::DefaultView)
{
    TX_LOG
}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{
    TX_ENTRY
    TX_EXIT
}

/*!
 Returns the singleton instance of music player engine.
 */
MpEngine * MpEngine::instance()
{
    static MpEngine instance;
    return &instance;
}

/*!
 Initialize engine
 */
void MpEngine::initialize( MpCommon::MpViewMode viewMode, TUid hostUid )
{
    TX_ENTRY
    mViewMode = viewMode;
    mHostUid = hostUid;
    
    //Load musicplayer translator
    QString lang = QLocale::system().name();
    QString path = QString( "z:/resource/qt/translations/" );
    bool translatorLoaded = false;

    mMpTranslator = new QTranslator( this );
    translatorLoaded = mMpTranslator->load( path + "musicplayer_" + lang );
    TX_LOG_ARGS( "Loading translator ok=" << translatorLoaded );
    if ( translatorLoaded ) {
        qApp->installTranslator( mMpTranslator );
    }
    
    if( mViewMode == MpCommon::DefaultView || mViewMode == MpCommon::FetchView ){
        // Harvesting Wrapper
        mMpxHarvesterWrapper = new MpMpxHarvesterFrameworkWrapper( mViewMode, mHostUid );
        connect( mMpxHarvesterWrapper, SIGNAL( scanStarted() ), 
                 this, SLOT( handleScanStarted() ), Qt::QueuedConnection );
        connect( mMpxHarvesterWrapper, SIGNAL( scanEnded(int, int) ), 
                 this, SLOT( handleScanEnded(int, int) ), Qt::QueuedConnection );
        qRegisterMetaType<MpxDiskEvents>("MpxDiskEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( diskEvent(MpxDiskEvents) ), 
                 this, SLOT( handleDiskEvent(MpxDiskEvents) ), Qt::QueuedConnection );
        qRegisterMetaType<MpxUsbEvents>("MpxUsbEvents");
        connect( mMpxHarvesterWrapper, SIGNAL( usbEvent(MpxUsbEvents) ), 
                 this, SLOT( handleUsbEvent(MpxUsbEvents) ), Qt::QueuedConnection );
        mSongScanner = new MpSongScanner( mMpxHarvesterWrapper );
        mMediaKeyHandler = new MpMediaKeyHandler();
        
        // Collection Wrapper
        mMpxCollectionWrapper = new MpMpxCollectionFrameworkWrapper( mViewMode, mHostUid );
        connect( mMpxCollectionWrapper, SIGNAL( collectionPlaylistOpened() ),
                this, SIGNAL( collectionPlaylistOpened() ),
                Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( playlistSaved( bool ) ),
                this, SIGNAL( playlistSaved( bool ) ),
                Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( songsDeleted( bool ) ),
                this, SIGNAL( songsDeleted( bool ) ),
                Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( playlistsRenamed( bool ) ),
                this, SIGNAL( playlistsRenamed( bool ) ),
                Qt::QueuedConnection );
        connect( mMpxCollectionWrapper, SIGNAL( isolatedCollectionOpened( MpMpxCollectionData* ) ),
                this, SIGNAL( isolatedCollectionOpened( MpMpxCollectionData* ) ),
                Qt::QueuedConnection );
    }

    // Playback Wrapper 
    mMpxPlaybackWrapper = new MpMpxPlaybackFrameworkWrapper( mViewMode, mHostUid );

    TX_EXIT
}

/*!
  Deinitialize wrappers
 */
void MpEngine::close( )
{
    delete mMpTranslator;
    mMpTranslator = 0;
    delete mSongScanner;
    mSongScanner = 0;
    delete mMediaKeyHandler;
    mMediaKeyHandler = 0;   
    delete mUsbOutstandingNote;
    mUsbOutstandingNote = 0;
    delete mMpxPlaybackWrapper;
    mMpxPlaybackWrapper = 0;
    delete mMpxHarvesterWrapper;
    mMpxHarvesterWrapper = 0;
    delete mMpxCollectionWrapper;
    mMpxCollectionWrapper = 0;
}

/*!
-------------------------------------------------------------------------------
    Harvesting related
-------------------------------------------------------------------------------
*/

/*!
 \
 Used to verify if an action can be executed depending on USB blocking state.
 If not, a notification note might be displayed.
 */
bool MpEngine::verifyUsbBlocking( bool showMessage )
{
    TX_ENTRY
    bool result( false );
    if ( mUsbBlockingState == USB_Connected ) {
        result = true;
        if ( showMessage ) {
            HbMessageBox *dialog = new HbMessageBox( HbMessageBox::MessageTypeInformation );
            dialog->setText( hbTrId( "txt_mus_info_usb_conn_in_progress" ) );
            dialog->setModal( true );
            setOutstandingPopup( dialog );
            mUsbOutstandingNote->show();;
        }
    }
    TX_EXIT
    return result;
}

/*!
 \
 Request Harvester to check if there are any system events active.
 */
void MpEngine::checkForSystemEvents()
{
    TX_ENTRY
    mMpxHarvesterWrapper->checkForSystemEvents();
    TX_EXIT
}

/*!
 Slot to be called to start Refresh library process.
 If scanning is already ongoing, this request is ignored.
 */
void MpEngine::refreshLibrary()
{
    TX_ENTRY
    if ( !verifyUsbBlocking( true ) ) {
        emit libraryAboutToUpdate();
        mSongScanner->scan();
    }
    TX_EXIT
}

/*!
 Slot to be called when song scanning starts.
 */
void MpEngine::handleScanStarted() {
    TX_ENTRY
    mMediaKeyHandler->setEnabled(false);
    TX_EXIT
}

/*!
 Slot to be called when song scanning ends.
 */
void MpEngine::handleScanEnded( int count, int error ) {
    TX_ENTRY
    Q_UNUSED( count );
    Q_UNUSED( error );
    mMediaKeyHandler->setEnabled(true);
    emit libraryUpdated();
    TX_EXIT
}

/*!
 Slot to be called when disk event is received from MPX framework.
 */
void MpEngine::handleDiskEvent( MpxDiskEvents event )
{
    TX_ENTRY_ARGS("event=" << event);
    switch ( event ) {
        case DiskFormatStarted:
            mMediaKeyHandler->setEnabled(false);
            break;
        case DiskFormatEnded:
            mMediaKeyHandler->setEnabled(true);
            break;
        case DiskRemoved:
            if ( mUsbBlockingState != USB_Synchronizing ) {
                emit libraryUpdated();
            }
            break;
        case DiskInserted:
            if ( mUsbBlockingState == USB_NotConnected ) {
                refreshLibrary();
            }
            else if ( mUsbBlockingState == USB_Connected ) {
                emit libraryUpdated();
            }
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when USB event is received from MPX framework.
 */
void MpEngine::handleUsbEvent( MpxUsbEvents event )
{
    TX_ENTRY_ARGS("event=" << event);
    switch ( event ) {
        case UsbMassStorageStarted:
            handleUsbMassStorageStartEvent();
            break;
        case UsbMassStorageEnded:
            handleUsbMassStorageEndEvent();
            break;
        case UsbMtpStarted:
            handleUsbMtpStartEvent();
            break;
        case UsbMtpEnded:
            handleUsbMtpEndEvent();
            break;
        case UsbMtpNotActive:
            handleUsbMtpNotActive();
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when mUsbOutstandingNote is about to close.
 */
void MpEngine::handleOutstandingNoteClosing()
{
    TX_ENTRY
    mUsbOutstandingNote = 0;
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMassStorageStart event is received.
 */
void MpEngine::handleUsbMassStorageStartEvent()
{
    TX_ENTRY
    mMediaKeyHandler->setEnabled(false);

    changeUsbBlockingState( USB_Synchronizing );
    emit usbBlocked(true);
    
    launchBlockingNote();
    
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMassStorageEnd event is received.
 */
void MpEngine::handleUsbMassStorageEndEvent()
{
    TX_ENTRY
    mMediaKeyHandler->setEnabled(true);    

    changeUsbBlockingState( USB_NotConnected );
    emit usbBlocked(false);

    HbAction *action;
    HbMessageBox *promptRefresh = new HbMessageBox( HbMessageBox::MessageTypeQuestion );
    promptRefresh->setText( hbTrId( "txt_mus_info_music_may_need_to_be_refreshed" ) );
    promptRefresh->setTimeout( HbPopup::NoTimeout );
    promptRefresh->setModal( true );
    promptRefresh->clearActions();
    action = new HbAction( hbTrId( "txt_common_button_yes" ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( refreshLibrary() ) );
    promptRefresh->addAction( action );
    action = new HbAction( hbTrId( "txt_common_button_no" ) );
    promptRefresh->addAction( action );
    setOutstandingPopup( promptRefresh );
    mUsbOutstandingNote->show();
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMTPStart event is received.
 */
void MpEngine::handleUsbMtpStartEvent()
{
    TX_ENTRY
    mMediaKeyHandler->setEnabled(false);
    
    changeUsbBlockingState( USB_Synchronizing );
    emit usbBlocked(true);
    
    //Cancel any ongoing operation.
    emit libraryAboutToUpdate();
    
    launchBlockingNote();
    
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMTPEnd event is received.
 */
void MpEngine::handleUsbMtpEndEvent()
{
    TX_ENTRY
    mMediaKeyHandler->setEnabled(true);    

    changeUsbBlockingState( USB_NotConnected );
    emit usbBlocked(false);
    
    if ( mUsbOutstandingNote ) {
        mUsbOutstandingNote->close();
    }
    if ( mPreviousUsbState == USB_Synchronizing ) {
        emit libraryUpdated();
    }
    TX_EXIT
}

/*!
 To be called when EMcMsgUSBMTPNotActive event is received.
 */
void MpEngine::handleUsbMtpNotActive()
{
    TX_ENTRY
    changeUsbBlockingState( USB_Connected );
    emit usbBlocked(true);
    TX_EXIT
}

/*!
 Internal
 Update the new and previous usb blocking state
 */
void MpEngine::changeUsbBlockingState( UsbBlockingState state )
{
    TX_ENTRY
    mPreviousUsbState = mUsbBlockingState;
    mUsbBlockingState = state;
    TX_EXIT
}

/*!
 Internal
 Used to launch the usb blocking note
 */
void MpEngine::launchBlockingNote()
{
    TX_ENTRY

    HbProgressDialog *usbBlockingNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
    usbBlockingNote->setModal( true );
    if ( usbBlockingNote->actions().count() ) {
        //Hide cancel action.
        usbBlockingNote->actions().at( 0 )->setVisible( false );
    }
    usbBlockingNote->setDismissPolicy( HbPopup::NoDismiss );
    usbBlockingNote->setText( hbTrId( "txt_mus_info_usb_conn_in_progress" ) );
    setOutstandingPopup( usbBlockingNote );
    mUsbOutstandingNote->show();

    TX_EXIT
}

/*!
 \internal
 sets \a popup as the current outstanding popup and cancels any other active popup.
 */
void MpEngine::setOutstandingPopup( HbPopup *popup )
{
    TX_ENTRY
    //Close previous popup (Normally blocking usb note)
    if ( mUsbOutstandingNote ) {
        disconnect( mUsbOutstandingNote, SIGNAL( aboutToClose() ), this, SLOT( handleOutstandingNoteClosing() ) );
        mUsbOutstandingNote->close();
    }

    //Set new outstanding popup
    popup->setAttribute( Qt::WA_DeleteOnClose );
    connect( popup, SIGNAL( aboutToClose() ), this, SLOT( handleOutstandingNoteClosing() ) );
    mUsbOutstandingNote = popup;
    TX_EXIT
}

/*!
-------------------------------------------------------------------------------
    Collection related
-------------------------------------------------------------------------------
*/

/*!
 Opens the collection for the given \a context.

 \sa collectionOpened()
 */
void MpEngine::openCollection( TCollectionContext context )
{
    mMpxCollectionWrapper->openCollection( context );
}

/*!
 Opens the collection at a specific \a index.

 \sa collectionOpened(), playlistOpened()
 */
void MpEngine::openCollectionItem( int index )
{
    mMpxCollectionWrapper->openCollectionItem( index );
}

/*!
 Navigates back to the container of the current items.

 \sa collectionOpened()
 */
void MpEngine::back()
{
    mMpxCollectionWrapper->back();
}

/*!
 Loads the user created \a playLists.

 */
void MpEngine::findPlaylists( QStringList &playlists )
{
    mMpxCollectionWrapper->findPlaylists( playlists );
}

/*!
 Creates a new playlist with name \a playlistName and adds \a selection and optionally uses \a collectionData.
 */
void MpEngine::createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->createPlaylist( playlistName, selection, collectionData );
    }
}

/*!
 Adds \a selection to the playlist specified in \a playlistIndex.
 */
void MpEngine::saveToPlaylist( int playlistIndex, QList<int> &selection )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->saveToPlaylist( playlistIndex, selection );
    }
}

/*!
 Rename a playlist by \a index with \a name.
 */
void MpEngine::renamePlaylist( QString &newName, int index )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->renamePlaylist( newName, index );
    }
}

/*!
 Adds \a selection to the current playlist from the specified \a collectionData.
 */
void MpEngine::saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->saveToCurrentPlaylist( selection, collectionData );
    }
}

/*!
 Rename a playlist with \a name.
 */
void MpEngine::renamePlaylist( QString &newName )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->renamePlaylist( newName );
    }
}

/*!
 Deletes \a selection from the collection.
 */
void MpEngine::deleteSongs( QList<int> &selection )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->deleteSongs( selection );
    }
}



/*!
 Initiate a playback preview for the selected item.
 */
void MpEngine::previewItem( int index )
{
    mMpxCollectionWrapper->previewItem( index );
}


/*!
 Opens the an isolated collection with \a context.
 \sa isolatedCollectionOpened()
 */
void MpEngine::openIsolatedCollection( TCollectionContext context )
{
    if ( !verifyUsbBlocking( true ) ) {
        mMpxCollectionWrapper->openIsolatedCollection( context );
    }
}

/*!
 Releases the resources used for the isolated collection.
 */
void MpEngine::releaseIsolatedCollection()
{
    mMpxCollectionWrapper->releaseIsolatedCollection();
}

/*!
 Returns pointer to MpMpxCollectionData, which is the collection data.
 */
MpMpxCollectionData *MpEngine::collectionData()
{
    return mMpxCollectionWrapper->collectionData();
}

/*!
 Slot to be called to reopen the collection in its current state.

 \sa collectionOpened()
 */
void MpEngine::reopenCollection()
{
    mMpxCollectionWrapper->reopenCollection();
}

/*!
 Slot to be called to request a reorder operation , indicates that 
 the item with \a playlistId , \a songId is to be moved from 
 \a originalOrdinal to \a newOrdinal.
 */
void MpEngine::reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal )
{
    mMpxCollectionWrapper->reorderPlaylist( playlistId, songId, originalOrdinal, newOrdinal );
}

/*!
 Returns pointer to MpPlaybackData, which is the playback data.
 */
MpPlaybackData *MpEngine::playbackData()
{
    return mMpxPlaybackWrapper->playbackData();
}
/*!
 Slot to be called to request embedded playback of item with Uri aFilename
 */

void MpEngine::playEmbedded( QString aFilename )
{
    mMpxPlaybackWrapper->play( aFilename );
}

/*!
 Slot to be called to request embedded playback of item with file handle
 */

void MpEngine::playEmbedded(const XQSharableFile& file )
{
    mMpxPlaybackWrapper->play( file );
}
/*!
 Slot to handle a play pause.
 */
void MpEngine::playPause()
{
    mMpxPlaybackWrapper->playPause();
}

/*!
 Slot to handle a stop.
 */
void MpEngine::stop()
{
    mMpxPlaybackWrapper->stop();
}

/*!
 Slot to handle a skeep forward.
 */
void MpEngine::skipForward()
{
    mMpxPlaybackWrapper->skipForward();
}

/*!
 Slot to handle a skeep backwards.
 */
void MpEngine::skipBackward()
{
    mMpxPlaybackWrapper->skipBackward();
}

/*!
 Slot to handle a request to change \a position.
 */
void MpEngine::setPosition( int position )
{
    mMpxPlaybackWrapper->setPosition( position );
}

/*!
 Slot to handle a request to change shuffle \a mode.
 */
void MpEngine::setShuffle( bool mode )
{
    mMpxPlaybackWrapper->setShuffle( mode );
}

/*!
 Slot to handle a request to change repeat \a mode.
 */
void MpEngine::setRepeat( bool mode )
{
    mMpxPlaybackWrapper->setRepeat( mode );
}

