/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors the file system for changes in a file.
*
*/


#include <e32base.h>
#include <bautils.h>
#include <mpxlog.h>

#include "filemonitor.h"
#include "filemonitorobserver.h"



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CFileMonitor::CFileMonitor( MFileMonitorObserver& aObserver)
        : CActive( EPriorityNormal ),
        iObserver( aObserver ),
        iFilePath( NULL )
    {
    }

// ---------------------------------------------------------------------------
// Second Phase Constructor
// ---------------------------------------------------------------------------
//
void CFileMonitor::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iFs.Connect() );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CFileMonitor* CFileMonitor::NewL( MFileMonitorObserver& aObserver)
    {
    CFileMonitor* self = new( ELeave ) CFileMonitor( aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFileMonitor::~CFileMonitor()
    {
    Cancel();
    iFs.Close();
    delete iFilePath;
    }

// ---------------------------------------------------------------------------
// Starts monitoring a particular file
// ---------------------------------------------------------------------------
//
void CFileMonitor::StartL(  const TDesC& aFile)
    {
    MPX_DEBUG1("CFileMonitor::Start <---");

    delete iFilePath;
    iFilePath = NULL;
    iFilePath = aFile.AllocL();
		
    TNotifyType notType = ENotifyAll;
    iFs.NotifyChange( notType, iStatus, *iFilePath );
    SetActive();
    MPX_DEBUG1("CFileMonitor::Start --->");
    }

// ---------------------------------------------------------------------------
// RunL callback
// ---------------------------------------------------------------------------
//
void CFileMonitor::RunL()
    {
    MPX_DEBUG1("CFileMonitor::RunL <---");
    if (!BaflUtils::FileExists(iFs, *iFilePath ))
        {
        iObserver.HandleFileRemovedL();
        MPX_DEBUG1("CFileMonitor::RunL - File was removed");
        }
    else
        {
        // Listen again
        TNotifyType notType(ENotifyAll);
        iFs.NotifyChange(notType, iStatus, *iFilePath);
        SetActive();
        MPX_DEBUG1("CFileMonitor::RunL - File exists");
        }
    MPX_DEBUG1("CFileMonitor::RunL --->");
    }

// ---------------------------------------------------------------------------
// Handle Cancelling
// ---------------------------------------------------------------------------
//
void CFileMonitor::DoCancel()
    {
    MPX_DEBUG1("CFileMonitor::DoCancel <---");

    // Stop monitoring
    iFs.NotifyChangeCancel();
    MPX_DEBUG1("CFileMonitor::DoCancel --->");

    }

// ----------------------------------------------------------------------------
// Handles a leave occurring in the request completion event handler RunL()
// ----------------------------------------------------------------------------
//
TInt CFileMonitor::RunError(TInt aError)
    {
    MPX_DEBUG2("CFileMonitor::RunError(%d)", aError );
   
    // Listen again
    TNotifyType notType(ENotifyAll);
    iFs.NotifyChange( notType, iStatus, *iFilePath );
    SetActive();
    return KErrNone;
    }
