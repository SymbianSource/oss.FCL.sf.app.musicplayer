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
* Description: Music Player collection song scanner.
*
*/

#include <hbprogressnote.h>

#include "mpcollectionsongscanner.h"
#include "mpmpxframeworkwrapper.h"
#include "mptrace.h"

/*!
    \class MpCollectionSongScanner
    \brief Music Player collection song scanner.

    Song scanner interfaces with MPX Harvesting Framework to harvest
    music files in the device.
*/

/*!
 \fn void scanEnd()

 This signal is emitted when scanning is ended.

 \sa scan()
*/

/*!
 Constructs the collection song scanner.
 */
MpCollectionSongScanner::MpCollectionSongScanner( MpMpxFrameworkWrapper *wrapper, QObject *parent )
    : QObject(parent),
      mMpxWrapper(wrapper),
      mScanning(false),
      mScanProgressNote(0)
{
    TX_ENTRY
    connect( mMpxWrapper, SIGNAL(scanStarted()), this, SLOT(handleScanStarted()) );
    connect( mMpxWrapper, SIGNAL(scanEnded()), this, SLOT(handleScanEnded()) );
    connect( mMpxWrapper, SIGNAL(scanCountChanged(int)), this, SLOT(handleScanCountChanged(int)) );
    TX_EXIT
}

/*!
 Destructs the collection song scanner.
 */
MpCollectionSongScanner::~MpCollectionSongScanner()
{
    TX_ENTRY
    if ( mScanProgressNote ) {
        delete mScanProgressNote;
    }
    TX_EXIT
}

/*!
 Initiates song scanning.
 */
void MpCollectionSongScanner::scan()
{
    mScanning = true;
    mMpxWrapper->scan();
}

/*!
 Returns true if scanning is ongoing.
 */
bool MpCollectionSongScanner::isScanning()
{
    return mScanning;
}

/*!
 Cancels ongoing song scanning, if any.

 \sa scan()
 */
void MpCollectionSongScanner::cancelScan()
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
void MpCollectionSongScanner::handleScanStarted()
{
    if ( !mScanProgressNote ) {
        mScanProgressNote = new HbProgressNote(HbProgressNote::WaitNote);
        connect(mScanProgressNote, SIGNAL(cancelled()), this, SLOT(cancelScan()));
    }
    mScanProgressNote->setText( QString("Scanning...") );
    mScanProgressNote->show();
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating end of
 scanning process.
 */
void MpCollectionSongScanner::handleScanEnded()
{
    if ( mScanning ) {
        mScanning = false;
        mScanProgressNote->cancel();
    }
    emit scanEnded();
}

/*!
 Slot called upon notification from MPX Harvesting FW indicating the number of
 songs scanned so far.
 */
void MpCollectionSongScanner::handleScanCountChanged(int count)
{
    QString added;
    added.setNum(count);
    added.append(" file(s) added.");
    mScanProgressNote->setText(added);
}

