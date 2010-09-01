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
* Description:  Implementation of TLS Helper
*
*/



// INCLUDE FILES
#include "mpxtlshelper.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Initialize TLS for storing application information.
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXTlsHelper::InitializeL()
    {
    TMPXTlsStruct* tls = reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( !tls )
        {
        TMPXTlsStruct* tls = new ( ELeave ) TMPXTlsStruct;
        tls->iHostUid = KNullUid;
        tls->iNeedSave = EFalse;
        tls->iAllowMove = EFalse;
        tls->iLaunchMode = EMPXLaunchModeStopped;
        tls->useCount = 1;
        Dll::SetTls( reinterpret_cast<TAny*>( tls ) );
        }
    else
        {
        tls->useCount++;
        }
    }

// ---------------------------------------------------------------------------
// Uninitialize TLS data storage. Must be called
// before exiting application to unload resources.
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXTlsHelper::Uninitialize()
    {
    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls && !( --tls->useCount ) )
        {
        delete tls;
        tls = NULL;
        Dll::SetTls( NULL );
        }
    }

// ---------------------------------------------------------------------------
// Store host application UID to TLS.
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXTlsHelper::SetHostUidL(
    const TUid& aUid )
    {
    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        tls->iHostUid = aUid;
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// Store file path of saved clip.
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXTlsHelper::SetFilePath(
    const TDesC& aFilePath )
    {
    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        tls->iFilePath = (TFileName) aFilePath;
        }
    }


// ---------------------------------------------------------------------------
// Store file path of saved clip.
// ---------------------------------------------------------------------------
//
EXPORT_C TFileName MPXTlsHelper::FilePath()
    {
    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        return (TFileName)tls->iFilePath;
        }

    return TFileName(NULL);
    }

// ---------------------------------------------------------------------------
// Fetch host application UID from TLS.
// ---------------------------------------------------------------------------
//
EXPORT_C TUid MPXTlsHelper::HostUid()
    {
    TUid hostUid( KNullUid );

    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        hostUid = tls->iHostUid;
        }

    return hostUid;
    }

// ---------------------------------------------------------------------------
// Set 'need save' flag.
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXTlsHelper::SetNeedSave( TBool aNeedSave )
    {
    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        tls->iNeedSave = aNeedSave;
        }
    }

// ---------------------------------------------------------------------------
// Get 'need save' flag.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool MPXTlsHelper::NeedSave()
    {
    TBool needSave( EFalse );

    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        needSave = tls->iNeedSave;
        }

    return needSave;
    }

// ---------------------------------------------------------------------------
// Sets Allow Move flag.
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXTlsHelper::SetAllowMove( TBool aAllowMove )
    {
    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        tls->iAllowMove = aAllowMove;
        }
    }

// ---------------------------------------------------------------------------
// Gets Allow Move flag.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool MPXTlsHelper::AllowMove()
    {
    TBool allowMove( EFalse );

    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        allowMove = tls->iAllowMove;
        }

    return allowMove;
    }

// ---------------------------------------------------------------------------
// Set launch mode.
// ---------------------------------------------------------------------------
//
EXPORT_C void MPXTlsHelper::SetLaunchModeL( TMPXLaunchMode aMode )
    {
    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        tls->iLaunchMode = aMode;
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// Get launch mode.
// ---------------------------------------------------------------------------
//
EXPORT_C TMPXLaunchMode MPXTlsHelper::LaunchMode()
    {
    TMPXLaunchMode mode( EMPXLaunchModeUnknown );

    TMPXTlsStruct* tls =  reinterpret_cast<TMPXTlsStruct*>( Dll::Tls() );
    if ( tls )
        {
        mode = tls->iLaunchMode;
        }

    return mode;
    }

//  End of File
