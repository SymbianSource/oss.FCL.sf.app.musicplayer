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
* Description:  Monitors an application exit event.
*
*/

#include <mpxlog.h>                     // MPX_DEBUG
#include <e32base.h>
#include <e32std.h>
#include <apacmdln.h>
#include <apgtask.h>
#include <AknTaskList.h>


#include "applicationmonitor.h"
#include "applicationmonitorobserver.h"



// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CApplicationMonitor::CApplicationMonitor( MApplicationMonitorObserver& aObserver)
        : CActive( EPriorityNormal ),
        iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// Second Phase Constructor
// ---------------------------------------------------------------------------
//
void CApplicationMonitor::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CApplicationMonitor* CApplicationMonitor::NewL( MApplicationMonitorObserver& aObserver)
    {
    MPX_DEBUG1("CApplicationMonitor::NewL <---");
    CApplicationMonitor* self = new( ELeave ) CApplicationMonitor( aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    MPX_DEBUG1("CApplicationMonitor::NewL --->");
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CApplicationMonitor::~CApplicationMonitor()
    {
    MPX_DEBUG1("CApplicationMonitor::~CApplicationMonitor <---");
    Cancel();
    iThread.Close();
    MPX_DEBUG1("CApplicationMonitor::~CApplicationMonitor --->");
    }

// ---------------------------------------------------------------------------
// Starts monitoring a particular Application
// ---------------------------------------------------------------------------
//
void CApplicationMonitor::StartL( TUid aAppUid , TBool aRootAppIndication )
    {
    MPX_DEBUG1("CApplicationMonitor::StartL <---");
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    TUint64 threadId(0);
    TBool taskExists(EFalse);
    TInt status = KErrNone;
    if(aRootAppIndication)
        {
        CAknTaskList* taskList = CAknTaskList::NewL( wsSession );
        TApaTask task = taskList->FindRootApp( aAppUid );
        delete taskList;
        taskExists = task.Exists();
        threadId = task.ThreadId().Id();
        }
    else
        {
        TApaTaskList taskList( wsSession );
        TApaTask task = taskList.FindApp( aAppUid );
        taskExists = task.Exists();
        threadId = task.ThreadId().Id();
        }
    wsSession.Close();
    
    if (taskExists)
        {
        MPX_DEBUG1("CApplicationMonitor::StartL app found");
        status = iThread.Open(TThreadId(threadId));
        if ( status == KErrNone )
        	{
        	iThread.Logon(iStatus);
        	SetActive();
			}
        }
    else
        {
        MPX_DEBUG1("CApplicationMonitor::StartL not app found");
        User::Leave(KErrNotFound);
        }
    MPX_DEBUG1("CApplicationMonitor::StartL --->");
    }

// ---------------------------------------------------------------------------
// RunL callback
// ---------------------------------------------------------------------------
//
void CApplicationMonitor::RunL()
    {
    MPX_DEBUG1("CApplicationMonitor::RunL <---");
    switch (iStatus.Int())
        {
        case EExitKill: 
        case EExitTerminate:
        case EExitPanic:
            MPX_DEBUG1("CApplicationMonitor::RunL kill\terminate\panic");
            iObserver.HandleApplicationClosedL((TExitType) iStatus.Int());
            break;
        case KErrCancel:
        case KErrNoMemory:
            MPX_DEBUG1("CApplicationMonitor::RunL cancel\memory");
            break;
        case EExitPending: 
        default:
            MPX_DEBUG1("CApplicationMonitor::RunL exitpendin\default");
            // Listen again
            iThread.Logon(iStatus);
            SetActive();
            break;
        }
    MPX_DEBUG1("CApplicationMonitor::RunL --->");
    }

// ---------------------------------------------------------------------------
// Handle Cancelling
// ---------------------------------------------------------------------------
//
void CApplicationMonitor::DoCancel()
    {
    MPX_DEBUG1("CApplicationMonitor::DoCancel <---");
    // Stop monitoring
    iThread.LogonCancel(iStatus);
    MPX_DEBUG1("CApplicationMonitor::DoCancel --->");
    }

// ----------------------------------------------------------------------------
// Handles a leave occurring in the request completion event handler RunL()
// ----------------------------------------------------------------------------
//
TInt CApplicationMonitor::RunError(TInt /*aError*/)
    {
    MPX_DEBUG1("CApplicationMonitor::RunError <---");
    // Listen again
    iThread.Logon(iStatus);
    SetActive();
    MPX_DEBUG1("CApplicationMonitor::RunError --->");
    return KErrNone;
    }
