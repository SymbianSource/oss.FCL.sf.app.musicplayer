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
* Description: Music Player Progress Dialogs.
*
*/


#ifndef MPPROGRESSDIALOGHANDLER_H
#define MPPROGRESSDIALOGHANDLER_H

#include "mpmpxcommondefs.h"
#include "mpmpxcollectionviewdefs.h"

class MpMpxCollectionFrameworkWrapper;
class MpMpxHarvesterFrameworkWrapper;
class HbProgressDialog;
class HbPopup;

#include <QObject>

class MpProgressDialogHandler : public QObject
{
    Q_OBJECT

public:

    explicit MpProgressDialogHandler( MpMpxCollectionFrameworkWrapper *cwrapper, MpMpxHarvesterFrameworkWrapper *hwrapper , QObject *parent=0 );
    virtual ~ MpProgressDialogHandler();
    void scan();
    bool isScanning();

    void deleteSongs( QList<int> &selection );
    

signals:

    void deleteStarted();
    void songsDeleted( bool success );
    
    
public slots:

    void cancelRequest();
    void handleDeleteStarted( TCollectionContext context, TInt count );
    void handleDeleteEnded( bool success );
    void handleAddSongs();
    void handleAddSongsFinished( bool success);
    void cancelScan();
    void handleScanStarted();
    void handleScanEnded( int numItemsAdded, int error );
    void handleScanCountChanged(int count);
    void handleDiskEvent( MpxDiskEvents event );
    void handleOutstandingPopupClosing();
    
       
private:
    
    void setOutstandingPopup( HbPopup *popup );
    void clearOutstandingPopup( HbPopup *popup ); 

    MpMpxCollectionFrameworkWrapper      *mMpxCollectionWrapper;       // Not own
    MpMpxHarvesterFrameworkWrapper       *mMpxHarvesterWrapper;         // Not own
    HbPopup                              *mOutStandingProgressNote; // Own
    TBool                                mDeleting;
    TBool                                mScanning;
};

#endif // MPSONGDELETING_H

