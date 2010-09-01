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
* Description:  Plugin manager, responsible to load the plugins.
*
*/

#include <mpxlog.h>                     // MPX_DEBUG
#include <ecom/ecom.h>
// for CeanupResetAndDestroyPushL
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "musiccontentpublisher.h"
#include "pluginmanager.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPluginManager::CPluginManager( TUid aUid, 
        TAny* aParameter, 
        CMusicContentPublisher* aContentPublisher) :
                CActive( EPriorityStandard ), 
                iUid( aUid ), 
                iParameter( aParameter ), 
                iContentPublisher(aContentPublisher)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPluginManager::ConstructL()
    {
    MPX_DEBUG1("CPluginManager::ConstructL <---");
    iSession = &REComSession::OpenL();
    LoadPluginsL();
    iSession->NotifyOnChange( iStatus );
    CActiveScheduler::Add( this );
    SetActive();
    MPX_DEBUG1("CPluginManager::ConstructL --->");
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPluginManager* CPluginManager::NewL( TUid aUid, 
        TAny *aParameter, 
        CMusicContentPublisher* aContentPublisher )
    {
    CPluginManager* self = CPluginManager::NewLC( aUid, 
            aParameter, 
            aContentPublisher );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPluginManager* CPluginManager::NewLC( TUid aUid, 
        TAny *aParameter,
        CMusicContentPublisher* aContentPublisher )
    {
    CPluginManager* self = new( ELeave ) CPluginManager( aUid, 
            aParameter, 
            aContentPublisher );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CPluginManager::~CPluginManager()
    {
    MPX_DEBUG1("CPluginManager::~CPluginManager <---");
    Cancel();
    CleanPluginsTable();
    iPluginMap.Close();
    if ( iSession )
        {
        iSession->Close();
        }
    REComSession::FinalClose();
    MPX_DEBUG1("CPluginManager::~CPluginManager --->");
    }

// ---------------------------------------------------------------------------
// Retuns a plugin by Implementation UID
// ---------------------------------------------------------------------------
//
TAny* CPluginManager::GetImplementation( TUid aUid )
    {
    TAny* ret= NULL;
    THashMapIter<TInt32, TAny*> iter( iPluginMap );
    const TInt32* pluginInstanceKey( NULL );
    TAny*const* ptr = iter.NextValue();
    while ( ptr )
        {
        pluginInstanceKey = iter.CurrentKey();
        if (REComSession::GetImplementationUidL( TUid::Uid( *pluginInstanceKey ) ) == aUid) 
            {
            ret = *ptr;
            break;
            }
        ptr = iter.NextValue();
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Retuns a plugin by index
// ---------------------------------------------------------------------------
//
TAny* CPluginManager::GetImplementation( TInt aIndex )
    {
    TAny* ret= NULL;
    THashMapIter<TInt32, TAny*> iter( iPluginMap );
    TAny*const* ptr = iter.NextValue();
    for ( TInt i( 0 ); ptr; i++ )
        {
        if ( i == aIndex )
            {
            ret = *ptr;
            break;
            }
        ptr = iter.NextValue();
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Returns the count of the plug-ins.
// ---------------------------------------------------------------------------
//
TInt CPluginManager::GetCount()
    {
    return iPluginMap.Count();
    }

// ---------------------------------------------------------------------------
// Loads the plugins.
// ---------------------------------------------------------------------------
//
void CPluginManager::LoadPluginsL()
    {
    MPX_DEBUG1("CPluginManager::LoadPluginsL <---");
    // Read info about all implementations into infoArray
    RImplInfoPtrArray infoArray;
    CleanupResetAndDestroyPushL( infoArray );

    REComSession::ListImplementationsL( iUid, infoArray );
    for ( TInt i = 0; i < infoArray.Count(); i++ )
        {
        TUid current_plugin = infoArray[i]->ImplementationUid();
        TAny* plug( NULL );
        TUid instanceKey;
        TInt err( KErrNone );
        TRAP( err, plug = REComSession::CreateImplementationL(
                        current_plugin,
                        instanceKey,
                        iParameter ) );
        if ( err == KErrNone && plug )
            {
            CleanupStack::PushL( plug );
            TInt32 key = instanceKey.iUid;
            iPluginMap.InsertL( key, plug );
            CleanupStack::Pop( plug );
            }
        }

    CleanupStack::PopAndDestroy( &infoArray );
    MPX_DEBUG1("CPluginManager::LoadPluginsL --->");
    }

// ---------------------------------------------------------------------------
// Unloads the plgugins.
// ---------------------------------------------------------------------------
//        
void CPluginManager::CleanPluginsTable()
    {
    MPX_DEBUG1("CPluginManager::CleanPluginsTable <---");
    THashMapIter<TInt32, TAny*> iter( iPluginMap );
    TAny*const* ptr = iter.NextValue();
    CBase* plug( NULL );
    const TInt32* pluginInstanceKey( NULL );
    while ( ptr )
        {
        plug = static_cast<CBase*>( *ptr );
        delete plug;
        plug = NULL;
        pluginInstanceKey = iter.CurrentKey();
        REComSession::DestroyedImplementation( TUid::Uid( *pluginInstanceKey ) );
        ptr = iter.NextValue();
        pluginInstanceKey = NULL;
        }
    MPX_DEBUG1("CPluginManager::CleanPluginsTable --->");
    }

// ---------------------------------------------------------------------------
//From CActive
// ---------------------------------------------------------------------------
//
void CPluginManager::RunL()
    {
    MPX_DEBUG1("CPluginManager::RunL <---");
    CleanPluginsTable();
    iPluginMap.Close();
    iContentPublisher->ResetL();
    LoadPluginsL();
    iSession->NotifyOnChange( iStatus );
    SetActive();
    MPX_DEBUG1("CPluginManager::RunL --->");
    }

// ---------------------------------------------------------------------------
//From CActive
// ---------------------------------------------------------------------------
//
void CPluginManager::DoCancel()
    {
    MPX_DEBUG1("CPluginManager::DoCancel <---");
    iSession->CancelNotifyOnChange( iStatus );
    MPX_DEBUG1("CPluginManager::DoCancel --->");
    }

// ---------------------------------------------------------------------------
//From CActive
// ---------------------------------------------------------------------------
//
TInt CPluginManager::RunError( TInt /*aError*/ )
    {
    MPX_DEBUG1("CPluginManager::RunError <---");
    MPX_DEBUG1("CPluginManager::RunError --->");
    return KErrNone;
    }

//  End of File

