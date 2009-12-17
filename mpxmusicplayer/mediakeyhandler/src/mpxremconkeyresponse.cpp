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
* Description:  Helper class for sending response back to RemCon Framework.
*
*/



// INCLUDE FILES
#include "mpxremconkeyresponse.h"
#include "mpxlog.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXRemConKeyResponse::CMPXRemConKeyResponse( 
    CRemConCoreApiTarget& aRemConCoreApiTarget ) : 
    CActive( CActive::EPriorityStandard ),
    iRemConCoreApiTarget( aRemConCoreApiTarget )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXRemConKeyResponse* CMPXRemConKeyResponse::NewL(
    CRemConCoreApiTarget& aRemConCoreApiTarget )
    {
    CMPXRemConKeyResponse* self = 
        new (ELeave) CMPXRemConKeyResponse( aRemConCoreApiTarget );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXRemConKeyResponse::~CMPXRemConKeyResponse()
    {
    Cancel();
    iResponseArray.Close();
    }

// ---------------------------------------------------------------------------
// Send the any key response back to Remcon server
// ---------------------------------------------------------------------------
//
void CMPXRemConKeyResponse::CompleteAnyKey( 
    TRemConCoreApiOperationId aOperationId )
    {
    if ( !IsActive() )
        {
        switch ( aOperationId )
            {
            case ERemConCoreApiVolumeUp:
                {
                iRemConCoreApiTarget.VolumeUpResponse( iStatus, KErrNone );
                SetActive();
                break;
                }
            case ERemConCoreApiVolumeDown:
                {
                iRemConCoreApiTarget.VolumeDownResponse( iStatus, KErrNone );
                SetActive();
                break;
                }
            default:
                {
                TInt error = KErrNone;
                iRemConCoreApiTarget.SendResponse(
                    iStatus, aOperationId, error );
                SetActive();
                break;
                }
            }
        }
    // already active. Append to array and complete later.
    else
        {
        iResponseArray.Append( aOperationId );
        }
    }
    
// ---------------------------------------------------------------------------
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------
//
void CMPXRemConKeyResponse::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// Handles an active object's request completion event.
// ---------------------------------------------------------------------------
//
void CMPXRemConKeyResponse::RunL()
    {
    MPX_DEBUG2( "CMPXRemConKeyResponse.RunL() -- iStatus=%d", iStatus.Int() );

    // if any existing -> Send response
    if ( iResponseArray.Count() )
        {
        CompleteAnyKey( iResponseArray[0] );
        // Remove already completed key
        iResponseArray.Remove( 0 );
        iResponseArray.Compress();
        }
    }

// End of File
