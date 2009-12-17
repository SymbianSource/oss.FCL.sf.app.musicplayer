/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MPX asynchronour save helper API 
*
*/



// INCLUDE FILES
#include "mpxsavehelper.h"

#include <eikenv.h>
#include <pathinfo.h>
#include <sysutil.h>
#include <mpxlog.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXSaveHelper::CMPXSaveHelper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXSaveHelper::CMPXSaveHelper( MMPXSaveHelperObserver* aObserver ) 
    : CActive( EPriorityStandard )
    {
    iObserver = aObserver;
    }

    
// -----------------------------------------------------------------------------
// CMPXSaveHelper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPXSaveHelper::ConstructL(MFileManObserver* aFMObserver )
    {
    iFileMan = CFileMan::NewL( CEikonEnv::Static()->FsSession(), aFMObserver );

    // Add this active object to the scheduler.
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMPXSaveHelper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXSaveHelper* CMPXSaveHelper::NewL( MMPXSaveHelperObserver* aObserver, MFileManObserver* aFMObserver )
    {
    CMPXSaveHelper* self = new( ELeave ) CMPXSaveHelper( aObserver );   
    CleanupStack::PushL( self );
    self->ConstructL( aFMObserver );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXSaveHelper::~CMPXSaveHelper   
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXSaveHelper::~CMPXSaveHelper()
    {
    delete iFileMan;
    }

// -----------------------------------------------------------------------------
// CMPXSaveHelper::StartCopyOperationL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXSaveHelper::StartCopyOperationL( 
    const TDesC& anOld, 
    const TDesC& aNew,
    TBool aMove /*=EFalse*/ )
    {
    if (!IsActive())
        {
        if ( aMove )
            {
            iFileMan->Move( anOld, aNew, CFileMan::EOverWrite, iStatus );
            }
        else
            {
            iFileMan->Copy( anOld, aNew, CFileMan::EOverWrite, iStatus );
            }
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CMPXSaveHelper::StartCopyOperationL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPXSaveHelper::StartCopyOperationL( 
    RFile& anOldFile, 
    const TDesC& aNew,
    TBool aMove /*=EFalse*/,
    TBool aSync )
    {
    if ( !aSync )
        {
        if ( !IsActive())
            {
            if ( aMove )
                {
                TFileName filename;
                anOldFile.FullName( filename );
                User::LeaveIfError( iFileMan->Move( filename, aNew, CFileMan::EOverWrite, iStatus ));
                }
            else
                {
                User::LeaveIfError( iFileMan->Copy( anOldFile, aNew, CFileMan::EOverWrite, iStatus ));
                }
                
            SetActive();
            }
        }
    else
        {
        TInt err = iFileMan->Copy( anOldFile, aNew, CFileMan::EOverWrite);
        if ( !err && iObserver )
            {
            iObserver->HandleSaveComplete( iStatus.Int() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXSaveHelper::RunL
// -----------------------------------------------------------------------------
//
void CMPXSaveHelper::RunL() 
    {   
    MPX_DEBUG1("CMPXSaveHelper::RunL(): entering");
    if ( iObserver )
        {
        iObserver->HandleSaveComplete( iStatus.Int() );
        }
    MPX_DEBUG1("CMPXSaveHelper::RunL(): exiting");
    }

// -----------------------------------------------------------------------------
// CMPXSaveHelper::DoCancel
// -----------------------------------------------------------------------------
//
void CMPXSaveHelper::DoCancel()
    {
    MPX_DEBUG1("CMPXSaveHelper::DoCancel(): entering");
    
    if ( iObserver )
        {
        iObserver->HandleSaveComplete( KErrCancel );
        }

    MPX_DEBUG1("CMPXSaveHelper::DoCancel(): exiting");
    }

//  End of File  
