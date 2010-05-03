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
* Description: Music Player song scanner.
*
*/

#include <hbprogressdialog.h>
#include <hbnotificationdialog.h>
#include <hblabel.h>
#include <hbfontspec.h>
#include <hbmessagebox.h>

#include "mpsongscanner.h"
#include "mpmpxharvesterframeworkwrapper.h"
#include "mptrace.h"

/*!
    \class MpSongScanner
    \brief Music Player song scanner.

    Song scanner interfaces with MPX Harvesting Framework to harvest
    music files in the device.
*/

/*!
 \fn void scanEnd()

 This signal is emitted when scanning is ended.

 \sa scan()
*/

/*!
 Constructs the song scanner.
 */
MpSongScanner::MpSongScanner( MpMpxHarvesterFrameworkWrapper *wrapper, QObject *parent )
    : QObject( parent ),
      mMpxWrapper(wrapper),
      mScanProgressNote(0),
      mScanning(false)
{
    TX_ENTRY
    connect( mMpxWrapper, SIGNAL(scanStarted()), this, SLOT(handleScanStarted()) );
    connect( mMpxWrapper, SIGNAL(scanEnded( int, int )), this, SLOT(handleScanEnded( int, int )) );
    connect( mMpxWrapper, SIGNAL(scanCountChanged(int)), this, SLOT(handleScanCountChanged(int)) );
    TX_EXIT
}

/*!
 Destructs the song scanner.
 */
MpSongScanner::~MpSongScanner()
{
    TX_LOG
}

/*!
 Initiates song scanning.
 */
void MpSongScanner::scan()
{
    if ( !mScanning ) {
        mScanning = true;
        mMpxWrapper->scan();
    }
}

/*!
 Returns true if scanning is ongoing.
 */
bool MpSongScanner::isScanning()
{
    return mScanning;
}

/*!
 Cancels ongoing song scanning, if any.

 \sa scan()
 */
void MpSongScanner::cancelScan()
{
    if ( mScanning ) {
        mScanning = false;
        mMpxWrapper->cancelScan();
    }
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating start of
 scanning process.
 */
void MpSongScanner::handleScanStarted()
{
    if ( !mScanProgressNote ) {
        mScanProgressNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
        connect( mScanProgressNote, SIGNAL( cancelled() ), this, SLOT( cancelScan() ) );
        connect( mScanProgressNote, SIGNAL( aboutToClose() ), this, SLOT( handleProgressNoteClosing() ) );
    }
    mScanProgressNote->setModal( true );
    HbLabel *title = new HbLabel( hbTrId( "txt_mus_title_refreshing" ) );
    title->setFontSpec(HbFontSpec(HbFontSpec::Primary));

    mScanProgressNote->setHeadingWidget( title );
    mScanProgressNote->setTextAlignment( Qt::AlignCenter );
    mScanProgressNote->setText( QString("") );
    mScanProgressNote->setAttribute( Qt::WA_DeleteOnClose );
    mScanProgressNote->show();
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating end of
 scanning process.
 */
void MpSongScanner::handleScanEnded( int numItemsAdded, int error )
{
    if (error == KErrDiskFull) {
		if ( mScanProgressNote ) {     
                mScanProgressNote->cancel();
        }
        HbMessageBox *diskFullDialog = new HbMessageBox();
        diskFullDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
        diskFullDialog->setText( hbTrId( "txt_mus_title_refresh_cancelled" ) );
        diskFullDialog->setTimeout( HbPopup::NoTimeout);
        diskFullDialog->exec();
        mScanning = false;
               
    }
    else{
        QString added;
        HbNotificationDialog *finishedDialog = new HbNotificationDialog();
        finishedDialog->setModal(true);
        finishedDialog->setAttribute( Qt::WA_DeleteOnClose );
    
        added = hbTrId( "txt_mus_dpopinfo_ln_songs_added", numItemsAdded );
        finishedDialog->setText( added );
              
        if( error < 0) {
            if ( mScanProgressNote ) {     
                mScanProgressNote->cancel();
            }
            finishedDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
            finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_cancelled" ) );
        }
        else if ( mScanning ) {
            if ( mScanProgressNote ) {     
                mScanProgressNote->cancel();
            }
            finishedDialog->setIcon( HbIcon( QString("qtg_large_ok") ) );
            finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_complete" ) );
        }
        else {
            finishedDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
            finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_cancelled" ) );
        }
        mScanning = false;
        finishedDialog->show();
    }
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating the number of
 songs scanned so far.
 */
void MpSongScanner::handleScanCountChanged(int count)
{
    QString added;

    added = hbTrId( "txt_mus_info_ln_songs_added" , count );
    if ( mScanProgressNote ) {
        mScanProgressNote->setText( added );
    }
}

/*!
 Slot to be called when disk event is received from MPX framework.
 */
void MpSongScanner::handleDiskEvent( MpxDiskEvents event )
{
    Q_UNUSED( event );
    if ( mScanning ) {
        if ( mScanProgressNote ) {
            mScanProgressNote->cancel();
        }
        mScanning = false;
        // AK - Should we show a dialog?
    }
    TX_EXIT
}

/*!
 Slot used to clear mScanProgressNote when dialog is closing.
 */
void MpSongScanner::handleProgressNoteClosing()
{
    mScanProgressNote = 0;
}

