/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Updates CPS storage
*
*/


// INCLUDE FILES

#include "mcpharvesterpublisherobserver.h"
#include "musiccontentpublisher.h"
#include "sapidatapluginconst.h"

#include <LiwServiceHandler.h>
#include <LiwVariant.h>
#include <LiwGenericParam.h>

//#include <cpglobals.h> // CPS string definitions.

// TODO Take these from some common header file!
// CONTANTS
/*
_LIT( KPubData, "publisher" );
_LIT8( KChangeInfo, "change_info" );
_LIT8( KType, "type");
_LIT8( KFilter, "filter" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KContentId,   "content_id" );
_LIT8( KOperation, "operation" );
_LIT8( KActionTrigger, "action_trigger" );
*/
/**
 * Maximum length for the content name
 */
 /*
const TInt KSAPIContentNameMaxLength = 255;

*/
//_LIT8( KCPInterface, "IDataSource" );
_LIT8( KCPContentInterface, "IContentPublishing" );
_LIT8( KCPService, "Service.ContentPublishing" );
_LIT( KMyOperationExecute, "execute" );

// =============================================================================
// --------------------------------------------------------------------------
// Cleanup utility for RPointerArray
// --------------------------------------------------------------------------
//
static void DoCleanup( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );
    static_cast< RCriteriaArray* >( aPtr )->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMCPHarvesterPublisherObserver::CMCPHarvesterPublisherObserver( CMusicContentPublisher* aHarvester )
    : iHarvester( aHarvester )
	{
	}

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMCPHarvesterPublisherObserver* CMCPHarvesterPublisherObserver::NewL(
        CMusicContentPublisher* aHarvester )
    {
    CMCPHarvesterPublisherObserver* self = CMCPHarvesterPublisherObserver::NewLC( aHarvester ) ;
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
CMCPHarvesterPublisherObserver* CMCPHarvesterPublisherObserver::NewLC( 
        CMusicContentPublisher* aHarvester )
    {
    CMCPHarvesterPublisherObserver* self( 
        new( ELeave ) CMCPHarvesterPublisherObserver( aHarvester ) );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCPHarvesterPublisherObserver::~CMCPHarvesterPublisherObserver()
    {
    // TODO automatic releasing.
    ReleaseLiw();
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMCPHarvesterPublisherObserver::ConstructL()
    {
    InitLiwL();
    }

// ---------------------------------------------------------------------------
// Handles Published content notification
// ---------------------------------------------------------------------------
//
TInt CMCPHarvesterPublisherObserver::HandleNotifyL( 
    TInt /*aErrorCode */, 
    TInt /*aEventId*/,
    CLiwGenericParamList& aEventParamList, 
    const CLiwGenericParamList& /*aInParamList*/ )
    {
    //__UHEAP_MARK;
    TInt pos(0);

    aEventParamList.FindFirst(pos, KChangeInfo);
    if (pos != KErrNotFound)
        {
        // Get list of maps
        TLiwVariant variant = (aEventParamList)[pos].Value();
        variant.PushL();
        const CLiwList* changeMapsList = variant.AsList();
        
        TPtrC publisher;
        TPtrC operation;
        TPtrC8 trigger;

        // Iter through list content
        for (TInt i = 0; i < changeMapsList->Count(); ++i)
            {
             if ( changeMapsList->AtL( i, variant )) 
                 {
                 const CLiwMap* map  = variant.AsMap();
                 
                 // Check what triggered a notification
                 variant.Reset();
                 if ( map->FindL( KOperation, variant ))
                     {
                     operation.Set( variant.AsDes()); 
                     }
                 if ( operation == KMyOperationExecute )
                     {
                     variant.Reset();
                     if( map->FindL( KActionTrigger, variant ))
                         {
                         trigger.Set( variant.AsData()); 
                         }
                     TLiwVariant pubVariant;
                     if( map->FindL( KContentId, pubVariant) )
                         {
                         publisher.Set( pubVariant.AsDes());
                         }
                     pubVariant.PushL();
                     if( publisher.Length() && trigger.Length())
                         {
                         iHarvester->HandlePublisherNotificationL( publisher, trigger );
                         }
                     CleanupStack::PopAndDestroy( &pubVariant );
                     }
                 }
            }
        CleanupStack::PopAndDestroy( &variant );
        }
    //__UHEAP_MARKEND;
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// Register for notifications
// ---------------------------------------------------------------------------
//
void CMCPHarvesterPublisherObserver::RegisterL( CLiwDefaultMap* aFilter )
    {
    CLiwGenericParamList* inParamList( CLiwGenericParamList::NewLC() );
    CLiwGenericParamList* outParamList( CLiwGenericParamList::NewLC() );
    
    // Fill in input list for RequestNotification command
    inParamList->AppendL(TLiwGenericParam(KType,TLiwVariant( KPubData )));
    inParamList->AppendL(TLiwGenericParam(KFilter ,TLiwVariant( aFilter )));
        
    iError = KErrNone;
    TRAP( iError, iInterface->ExecuteCmdL( 
                KRequestNotification,
                *inParamList,
                *outParamList,
                0,
                this ) );
    
    CleanupStack::PopAndDestroy( outParamList );
    CleanupStack::PopAndDestroy( inParamList ); 
    }

// ---------------------------------------------------------------------------
// Sing off to notification
// ---------------------------------------------------------------------------
//
void CMCPHarvesterPublisherObserver::ReleaseL()
    {
    if( iInterface )
        {
        CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL();
        CleanupStack::PushL( inParamList );
        CLiwGenericParamList* outParamList = CLiwGenericParamList::NewL();
        CleanupStack::PushL( outParamList );
        
        TInt err(KErrNone);
        TRAP(err, iInterface->ExecuteCmdL( 
                KRequestNotification,
                *inParamList,
                *outParamList,
                KLiwOptCancel,
                this ));
        
        CleanupStack::PopAndDestroy( outParamList );
        CleanupStack::PopAndDestroy( inParamList );
        }
    }


// ------------------------------------------------------------------------
// CWidgetUiWindowManager::FindObserver
//
// Init LIW interface.
// ------------------------------------------------------------------------
void CMCPHarvesterPublisherObserver::InitLiwL()
    {
    if ( !iLiwHandler )
        {
        TInt error( KErrNone );
        CLiwGenericParamList* in( NULL );
        CLiwGenericParamList* out( NULL );
        CLiwCriteriaItem* criterion( NULL );
        MLiwInterface* liwIface( NULL );

        RCriteriaArray array( 1 );
        TCleanupItem cleanup( DoCleanup, &array );
        CleanupStack::PushL( cleanup );

        // Do the basic initialization.
        iLiwHandler = CLiwServiceHandler::NewL();

        // Get iCPSIface.
        in = CLiwGenericParamList::NewLC();
        out = CLiwGenericParamList::NewLC();

        criterion = CLiwCriteriaItem::NewL( 
            KLiwCmdAsStr,
            KCPContentInterface, 
            KCPService );
            
        error = array.Append( criterion );
        if ( error )
            {
            delete criterion; criterion = NULL;
            User::Leave( error );
            }

        criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );
            
        iLiwHandler->AttachL( array );
        iLiwHandler->ExecuteServiceCmdL( *criterion, *in, *out ); 

        out->FindFirst( error, KCPContentInterface );
        User::LeaveIfError( error );

        iInterface = (*out)[ error ].Value().AsInterface();

        CleanupStack::PopAndDestroy( 3 );
        }
    }

void CMCPHarvesterPublisherObserver::ReleaseLiw()
    {    
    if ( iInterface )
        {
        iInterface->Close();
        iInterface = NULL;
        }
        
    delete iLiwHandler;
    iLiwHandler = NULL;
    }
 //  End of File

