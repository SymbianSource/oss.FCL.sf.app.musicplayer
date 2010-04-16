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
#include <hbnotificationdialog.h>
#include <hbinstance.h>

#include "mpengine.h"
#include "mpmpxframeworkwrapper.h"
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
    \fn void libraryRefreshed()

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
 Constructs music player engine.
 */
MpEngine::MpEngine()
    : mMpxWrapper(0),
      mSongScanner(0),
      mMediaKeyHandler(0),
      mUsbBlockingNote(0),
      mMpTranslator(0),
      mUsbBlockingState(USB_NotConnected),
      mPreviousUsbState(USB_NotConnected)
{
    TX_ENTRY

    mMpxWrapper = new MpMpxFrameworkWrapper();
    connect( mMpxWrapper, SIGNAL( scanStarted() ), this, SLOT( handleScanStarted() ) );
    connect( mMpxWrapper, SIGNAL( scanEnded(int, int) ), this, SLOT( handleScanEnded(int, int) ) );
    connect( mMpxWrapper, SIGNAL( diskEvent(MpxDiskEvents) ), this, SLOT( handleDiskEvent(MpxDiskEvents) ) );
    connect( mMpxWrapper, SIGNAL( usbEvent(MpxUsbEvents) ), this, SLOT( handleUsbEvent(MpxUsbEvents) ) );

    mSongScanner = new MpSongScanner( mMpxWrapper );
    mMediaKeyHandler = new MpMediaKeyHandler();

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

    TX_EXIT
}

/*!
 Destructs music player engine.
 */
MpEngine::~MpEngine()
{
    TX_ENTRY
    delete mMpTranslator;
    delete mSongScanner;
    delete mMediaKeyHandler;
    delete mUsbBlockingNote;
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
 Refresh library by starting the scan.
 If scanning is already ongoing, this request is ignored.
 */
void MpEngine::refreshLibrary()
{
    TX_ENTRY
    emit libraryAboutToRefresh();
    mSongScanner->scan();
    TX_EXIT
}

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
            HbMessageBox dialog ( HbMessageBox::MessageTypeInformation );
            dialog.setText( QString( tr( "USB connection in progress. Cannot proceed with operation" ) ) );
            dialog.setModal( true );
            dialog.exec();
        }
    }
    TX_EXIT
    return result;
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
    emit libraryRefreshed();
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
            if ( mUsbBlockingState == USB_NotConnected ) {
                emit exitApplication();
            }
            break;
        case DiskInserted:
            if ( mUsbBlockingState == USB_NotConnected ) {
                refreshLibrary();
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
 To be called when EMcMsgUSBMassStorageStart event is received.
 */
void MpEngine::handleUsbMassStorageStartEvent()
{
    TX_ENTRY
    mMediaKeyHandler->setEnabled(false);

    changeUsbBlockingState( USB_Synchronizing );
    emit usbBlocked(true);
    
    if ( !mUsbBlockingNote ) {
        mUsbBlockingNote = new HbNotificationDialog();
        mUsbBlockingNote->setText( QString( tr( "USB connection in progress" ) ) );
        mUsbBlockingNote->setModal( true );
        mUsbBlockingNote->setTimeout( HbPopup::NoTimeout );
    }
    mUsbBlockingNote->show();
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
    
    if ( mUsbBlockingNote ) {
        delete mUsbBlockingNote;
        mUsbBlockingNote = 0;
    }
    HbMessageBox promptRefresh( HbMessageBox::MessageTypeQuestion );
    promptRefresh.setText( QString( tr( "List may need refreshing due to recent USB synchronisation. Refresh now?" ) ) );
    promptRefresh.setTimeout( HbPopup::NoTimeout );
    promptRefresh.setModal( true );
    HbAction *action = promptRefresh.exec();
    if ( action == promptRefresh.primaryAction() ) {
        refreshLibrary();
    }
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
    
    if ( !mUsbBlockingNote ) {
        mUsbBlockingNote = new HbNotificationDialog();
        mUsbBlockingNote->setText( QString( tr( "USB connection in progress" ) ) );
        mUsbBlockingNote->setModal( true );
        mUsbBlockingNote->setTimeout( HbPopup::NoTimeout );
    }
    mUsbBlockingNote->show();
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
    
    if ( mUsbBlockingNote ) {
        delete mUsbBlockingNote;
        mUsbBlockingNote = 0;
    }
    if ( mPreviousUsbState == USB_Synchronizing ) {
        emit libraryRefreshed();
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
