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
* Description: Music Player Progress Dialogs Hanlder.
*
*/

#include <hbprogressdialog.h>
#include <hblabel.h>
#include <hbfontspec.h>
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>

#include "mpprogressdialoghandler.h"
#include "mpmpxcollectionframeworkwrapper.h"
#include "mpmpxharvesterframeworkwrapper.h"
#include "mptrace.h"

/*!
    \class MpProgressDialogHandler
    \brief Music Player Progress Dialogs Handler.

    This class controls the asynchronous operation of all progress 
    dialogs used by Music Player
*/

/*!
 Constructs the dialog handler.
 */
MpProgressDialogHandler::MpProgressDialogHandler( MpMpxCollectionFrameworkWrapper *cwrapper, MpMpxHarvesterFrameworkWrapper *hwrapper,  QObject *parent )
    : QObject( parent ),
      mMpxCollectionWrapper(cwrapper),
      mMpxHarvesterWrapper(hwrapper),
      mOutStandingProgressNote(0),
      mDeleting(false),
      mScanning(false)
{
    TX_ENTRY
    connect( mMpxCollectionWrapper, SIGNAL(deleteStarted( TCollectionContext, TInt )), this, SLOT(handleDeleteStarted( TCollectionContext, TInt )) );
    connect( mMpxCollectionWrapper, SIGNAL(songsDeleted( bool )), this, SLOT(handleDeleteEnded( bool )) );
    connect( mMpxCollectionWrapper, SIGNAL( openAddSongsWaitDialog() ), this, SLOT(handleAddSongs() ));
    connect( mMpxCollectionWrapper, SIGNAL( playlistSaved( bool ) ), this, SLOT(handleAddSongsFinished( bool ) ));
    connect( mMpxHarvesterWrapper, SIGNAL(scanStarted()), this, SLOT(handleScanStarted()) );
    connect( mMpxHarvesterWrapper, SIGNAL(scanEnded( int, int )), this, SLOT(handleScanEnded( int, int )) );
    connect( mMpxHarvesterWrapper, SIGNAL(scanCountChanged(int)), this, SLOT(handleScanCountChanged(int)) );
    TX_EXIT
}

/*!
 Destructs the dialog handler.
 */
MpProgressDialogHandler::~MpProgressDialogHandler()
{
    TX_LOG
}

/*!
 Initiates song scanning.
 */
void MpProgressDialogHandler::scan()
{
    if ( !mScanning ) {
        mScanning = true;
        mMpxHarvesterWrapper->scan();
    }
}

/*!
 Returns true if scanning is ongoing.
 */
bool MpProgressDialogHandler::isScanning()
{
    return mScanning;
}

/*!
 Initiates song deleting.
 */
void MpProgressDialogHandler::deleteSongs( QList<int> &selection )
{
    TX_ENTRY
    if ( !mDeleting ) {
        mDeleting = true;
        mMpxCollectionWrapper->deleteSongs( selection );
    }
    TX_EXIT
}

/*!
 Slot called upon cancels ongoing request, if any.
 */
void MpProgressDialogHandler::cancelRequest()
{
    TX_ENTRY
        mMpxCollectionWrapper->cancelRequest();
    TX_EXIT
}

/*!
 Slot called upon notification from MPX FW Wrapper indicating start of
 deleting process.
 */
void MpProgressDialogHandler::handleDeleteStarted(TCollectionContext context, TInt count)
{
    TX_ENTRY
    emit deleteStarted();
    HbProgressDialog *deleteProgressNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
    connect( deleteProgressNote, SIGNAL( cancelled() ), this, SLOT( cancelRequest() ) );
    connect( deleteProgressNote, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
    deleteProgressNote->setModal( true );
    deleteProgressNote->setDismissPolicy( HbPopup::NoDismiss );
    if ( context == ECollectionContextPlaylistSongs ){
        if (count >= 100) {  //show progress dialog if removing more than 100 songs 
            deleteProgressNote->setText( hbTrId( "txt_mus_info_removing_songs" ) );
            setOutstandingPopup(deleteProgressNote);
            deleteProgressNote->show();
        }
    }
    else if (context != ECollectionContextPlaylists ){  //no progress dialog for delete playlist
        deleteProgressNote->setText( hbTrId( "txt_mus_info_deleting" ) );
        setOutstandingPopup(deleteProgressNote);
        deleteProgressNote->show();
    }
    TX_EXIT
}

/*!
 Slot called upon notification from MPX FW CollectionHelper indicating end of
 deleting process.
 */
void MpProgressDialogHandler::handleDeleteEnded( bool success )
{
    TX_ENTRY
    Q_UNUSED( success );
    HbProgressDialog *deleteProgressNote = qobject_cast<HbProgressDialog *>( mOutStandingProgressNote );
    emit songsDeleted( success );
    if ( deleteProgressNote ) {     
        deleteProgressNote->cancel();
    }
    mDeleting = false;
    TX_EXIT
}

/*!
 Slot called upon notification from MPX FW Wrapper indicating start of
 add songs process.
 */
void MpProgressDialogHandler::handleAddSongs()
{
    HbProgressDialog *addSongsWaitNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
    connect( addSongsWaitNote, SIGNAL( cancelled() ), this, SLOT( cancelRequest() ) );
    connect( addSongsWaitNote, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
    addSongsWaitNote->setModal( true );
    addSongsWaitNote->setText( hbTrId( "txt_mus_info_adding_songs" ) );
    setOutstandingPopup( addSongsWaitNote );
    addSongsWaitNote->show();
    
    
}

/*!
 Slot called upon notification from MPX FW CollectionHelper indicating end of
 add songs process.
 */
void MpProgressDialogHandler::handleAddSongsFinished( bool success )
{
    Q_UNUSED(success);
    HbProgressDialog *addSongsWaitNote = qobject_cast<HbProgressDialog *>( mOutStandingProgressNote );
    if (addSongsWaitNote){
        addSongsWaitNote->cancel();
    }
}

/*!
 Cancels ongoing song scanning, if any.

 \sa scan()
 */
void MpProgressDialogHandler::cancelScan()
{
    if ( mScanning ) {
        mScanning = false;
        mMpxHarvesterWrapper->cancelScan();
    }
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating start of
 scanning process.
 */
void MpProgressDialogHandler::handleScanStarted()
{
    HbProgressDialog *scanProgressNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
    connect( scanProgressNote, SIGNAL( cancelled() ), this, SLOT( cancelScan() ) );
    connect( scanProgressNote, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
    
    scanProgressNote->setModal( true );
    HbLabel *title = new HbLabel( hbTrId( "txt_mus_title_refreshing" ) );
    title->setFontSpec(HbFontSpec(HbFontSpec::Primary));

    scanProgressNote->setHeadingWidget( title );
    scanProgressNote->setText( QString("") );
    setOutstandingPopup( scanProgressNote );
    scanProgressNote->show();
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating end of
 scanning process.
 */
void MpProgressDialogHandler::handleScanEnded( int numItemsAdded, int error )
{
    HbProgressDialog *scanProgressNote = qobject_cast<HbProgressDialog *>( mOutStandingProgressNote );
    if (error == KErrDiskFull) {
        if ( scanProgressNote ) {     
            scanProgressNote->cancel();
        }
        QString diskfull;
        diskfull = hbTrId( "txt_mus_title_refresh_cancelled" );
        diskfull.append(" ");
        diskfull.append( hbTrId( "txt_mus_info_out_of_disk_space" ) );
        HbMessageBox *diskFullDialog = new HbMessageBox();
        setOutstandingPopup( diskFullDialog );
        diskFullDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
        diskFullDialog->setText( diskfull );
        diskFullDialog->setTimeout( HbPopup::NoTimeout);
        diskFullDialog->show();
        mScanning = false;
               
    }
    else{
        QString added;
        HbNotificationDialog *finishedDialog = new HbNotificationDialog();
        finishedDialog->setModal(true);
            
        added = hbTrId( "txt_mus_dpopinfo_ln_songs_added", numItemsAdded );
        finishedDialog->setText( added );
              
        if( error < 0) {
            if ( scanProgressNote ) {     
                scanProgressNote->cancel();
            }
            finishedDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
            finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_cancelled" ) );
        }
        else if ( mScanning ) {
            if ( scanProgressNote ) {     
                scanProgressNote->cancel();
            }
            finishedDialog->setIcon( HbIcon( QString("qtg_large_ok") ) );
            finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_complete" ) );
        }
        else {
            finishedDialog->setIcon( HbIcon( QString("qtg_small_fail") ) );
            finishedDialog->setTitle( hbTrId( "txt_mus_dpophead_refresh_cancelled" ) );
        }
        mScanning = false;
        setOutstandingPopup( finishedDialog );
        finishedDialog->show();
    }
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating the number of
 songs scanned so far.
 */
void MpProgressDialogHandler::handleScanCountChanged(int count)
{
    HbProgressDialog *scanProgressNote = qobject_cast<HbProgressDialog *>( mOutStandingProgressNote );
    QString added;
    added = hbTrId( "txt_mus_info_ln_songs_added" , count );
    if ( scanProgressNote ) {
        scanProgressNote->setText( added );
    }
}

/*!
 Slot to be called when disk event is received from MPX framework.
 */
void MpProgressDialogHandler::handleDiskEvent( MpxDiskEvents event )
{
    Q_UNUSED( event );
    HbProgressDialog *progressNote = qobject_cast<HbProgressDialog *>( mOutStandingProgressNote );
    if ( progressNote ) {
        progressNote->cancel();
    }
    mScanning = false;
    mDeleting = false;
    TX_EXIT
}

/*!
 Slot to be called when mOutStandingProgressNote is about to close.
 */
void MpProgressDialogHandler::handleOutstandingPopupClosing()
{
    TX_ENTRY
    mOutStandingProgressNote = 0;
    TX_EXIT
}

/*!
 \internal
 sets \a popup as the current outstanding popup and cancels any other active popup.
 */
void MpProgressDialogHandler::setOutstandingPopup( HbPopup *popup )
{
    TX_ENTRY
    //Close previous popup
    if ( mOutStandingProgressNote ) {
        disconnect( mOutStandingProgressNote, SIGNAL( aboutToClose() ), this, SLOT( handleOutstandingPopupClosing() ) );
        mOutStandingProgressNote->close();
    }

    //Set new outstanding popup
    popup->setAttribute( Qt::WA_DeleteOnClose );
    connect( popup, SIGNAL( aboutToClose() ), this, SLOT( handleOutstandingPopupClosing() ) );
    mOutStandingProgressNote = popup;
    TX_EXIT
}

