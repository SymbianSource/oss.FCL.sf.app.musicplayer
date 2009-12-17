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


#include <mpxlog.h>
#include <mpxconstants.h>

#include "pnsmonitor.h"
#include "pnsmonitorobserver.h"

// RProperty key to identify the case when Music Player launching
// in the background
const TInt KMPXLaunchingOnBackground( 100 );

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPNSMonitor::CPNSMonitor( MPNSMonitorObserver& aObserver )
        : CActive( CActive::EPriorityLow ),
        iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// Second Phase Constructor
// ---------------------------------------------------------------------------
//
void CPNSMonitor::ConstructL()
    {
    User::LeaveIfError( iPropertyHandler.Attach( KAppUidMusicPlayerX,
            KMPXLaunchingOnBackground ) );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CPNSMonitor* CPNSMonitor::NewL( MPNSMonitorObserver& aObserver )
    {
    CPNSMonitor* self = new( ELeave ) CPNSMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPNSMonitor::~CPNSMonitor()
    {
    Cancel();
    iPropertyHandler.Close();
    }

// ---------------------------------------------------------------------------
// Starts monitoring
// ---------------------------------------------------------------------------
//
void CPNSMonitor::Start()
    {
    MPX_DEBUG1("CPNSMonitor::Start <---");
    iPropertyHandler.Subscribe( iStatus );
    SetActive();
    MPX_DEBUG1("CPNSMonitor::Start --->");
    }

// ---------------------------------------------------------------------------
// RunL callback
// ---------------------------------------------------------------------------
//
void CPNSMonitor::RunL()
    {
    MPX_DEBUG1("CPNSMonitor::RunL <---");
    TInt ret( 0 );
    TInt err = RProperty::Get( KAppUidMusicPlayerX,
                KMPXLaunchingOnBackground,
                ret );

    if ( ret )
        {
        iObserver.OpeningMusicPlayer();
        MPX_DEBUG1("CPNSMonitor::RunL - Opening Music Player on the background");
        }
    
	// Listen again
    Start();
    
    MPX_DEBUG1("CPNSMonitor::RunL --->");
    }

// ---------------------------------------------------------------------------
// Handle Cancelling
// ---------------------------------------------------------------------------
//
void CPNSMonitor::DoCancel()
    {
    MPX_DEBUG1("CPNSMonitor::DoCancel <---");
    // Stop monitoring
    iPropertyHandler.Cancel();
    MPX_DEBUG1("CPNSMonitor::DoCancel --->");
    }

// ----------------------------------------------------------------------------
// Handles a leave occurring in the request completion event handler RunL()
// ----------------------------------------------------------------------------
//
TInt CPNSMonitor::RunError( TInt aError )
    {
    MPX_DEBUG2("CPNSMonitor::RunError(%d)", aError );
   
    // Listen again
    Start();
    return KErrNone;
    }
