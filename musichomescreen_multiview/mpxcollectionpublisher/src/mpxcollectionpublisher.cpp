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
* Description:  Manages MCP plugins, and content publishing.
*
*/


#include <liwservicehandler.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionmessage.h>

#include "cpglobals.h" //This will be moved to domain API (HSFW)
#include "mpxcollectionpublisher.h"



_LIT8( KMCPDataKey,"musicmenu_collectioninfo_numb1data" );
_LIT( KMCPublisher, "MpxCollectionPublisher" );
_LIT( KMCPType,"integer" );
_LIT( KMCPContent,"musicmenu_collectioninfo_numb1" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMpxCollectionPublisher::CMpxCollectionPublisher(
        MLiwInterface* aCPSInterface ) : iRefreshOperation( EFalse )
	{
	iCPSInterface = aCPSInterface;

	}

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMpxCollectionPublisher::ConstructL()
    {
    iDefaultAttrs.Append( KMPXMediaColDetailNumberOfItems );
    iCollectionUiHelper = 
            CMPXCollectionHelperFactory:: NewCollectionUiHelperL();      
	iCollectionUtility = 
	        MMPXCollectionUtility::NewL( this, KMcModeIsolated );
    
	TUid collection = KLocalCollection;
	collection = iCollectionUtility->
                    Collection().CollectionIDL( collection );
    iCollectionUtility->Collection().CommandL( EMcCmdCollectionInit, 
                                               collection.iUid );
	//music
    iAllSongsPath = iCollectionUiHelper->MusicAllSongsPathL();
    iCollectionUtility->Collection().CancelRequest();
    iCollectionUtility->Collection().MediaL( *iAllSongsPath , 
    	        iDefaultAttrs.Array());
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMpxCollectionPublisher* CMpxCollectionPublisher::NewL(  
        MLiwInterface* aCPSInterface )
    {
    CMpxCollectionPublisher* self = 
            new ( ELeave ) CMpxCollectionPublisher( aCPSInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMpxCollectionPublisher::~CMpxCollectionPublisher()
    {
    if ( iCollectionUiHelper )
		{
		iCollectionUiHelper->Close();
		}

	if ( iCollectionUtility )
		{
		iCollectionUtility->Close();
		}
	iDefaultAttrs.Close();
	delete iAllSongsPath;
    }

  
void CMpxCollectionPublisher::UpdateL() 
    {

    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// empty
// ---------------------------------------------------------------------------
// 	    
void CMpxCollectionPublisher::HandleOpenL( const CMPXMedia& /* aMedia */,
            TInt /* aIndex */,
    		TBool /* aComplete */,TInt /* aError */  )
    {
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// empty
// ---------------------------------------------------------------------------
//     					
void CMpxCollectionPublisher::HandleOpenL( 
            const CMPXCollectionPlaylist& /* aPlaylist */, 
            TInt /* aError */)
    {
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles MediaL
// ---------------------------------------------------------------------------
// 
void CMpxCollectionPublisher::HandleCollectionMediaL( const CMPXMedia& aMedia,
		TInt aError )
	{
	if (KErrNone == aError && 
	        aMedia.IsSupported( KMPXMediaColDetailNumberOfItems ) )
		{
		TInt numberOfSongs = 
		       aMedia.ValueTObjectL<TBool>( KMPXMediaColDetailNumberOfItems );
        PublishNumberL( numberOfSongs );
		}
	}

// ---------------------------------------------------------------------------
// Handle collection messages.
// ---------------------------------------------------------------------------
//
void CMpxCollectionPublisher::HandleCollectionMessageL( CMPXMessage* aMsg )
    {       
    TMPXMessageId messageId = 
            aMsg->ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId );
    if ( messageId == KMPXMessageGeneral )        
        {          
        DoHandleGeneralMessageL( *aMsg );   
        } 
    else if( messageId == KMPXMessageIdItemChanged )
        {
        // Loop through messages for arrays.
        //
        if ( aMsg->IsSupported( KMPXMessageArrayContents ) )
            {
			CMPXMessageArray* messageArray =
                    aMsg->Value<CMPXMessageArray>( KMPXMessageArrayContents );
							
            User::LeaveIfNull( messageArray ); 
            for( TInt i=0; i<messageArray->Count(); ++i )
                {
                HandleCollectionMessage( (*messageArray)[i], KErrNone);  
                } 
            }            
        else // Single item
            {                
            DoHandleItemChangedMessageL( *aMsg ) ;                
            }                
        }                     
    }     

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection messages.
// ---------------------------------------------------------------------------
//
void CMpxCollectionPublisher::HandleCollectionMessage( CMPXMessage* aMsg, 
                                                  TInt aError )
    {
	if ( aError == KErrNone && aMsg )
		{
        TRAP_IGNORE( HandleCollectionMessageL( aMsg ) );
        }
    }  

// ---------------------------------------------------------------------------
// Handle item change message.
// ---------------------------------------------------------------------------
//
void CMpxCollectionPublisher::DoHandleItemChangedMessageL( 
                                                    const  CMPXMessage& aMsg )
    {
    TMPXChangeEventType eventType( 
            aMsg.ValueTObjectL<TMPXChangeEventType>( 
                        KMPXMessageChangeEventType ) );
    TMPXItemId itemId( 
            aMsg.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId ) );
  
    if ( !iRefreshOperation && 
          ( eventType == EMPXItemDeleted || eventType == EMPXItemInserted ) ) 
        { 
        //everytime an item is changed we need to refresh data
        iCollectionUtility->Collection().CancelRequest();
        iCollectionUtility->Collection().MediaL( *iAllSongsPath , 
        	        iDefaultAttrs.Array() );
        }                           
        
    } 

// ---------------------------------------------------------------------------
// Handle collection general message.
// ---------------------------------------------------------------------------
//
void CMpxCollectionPublisher::DoHandleGeneralMessageL(  
        const CMPXMessage& aMsg )
    {
    TInt event = aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralEvent );
    TInt type = aMsg.ValueTObjectL<TInt>( KMPXMessageGeneralType ); 
    
      
    if ( event == TMPXCollectionMessage::EBroadcastEvent && 
         ( type == EMcMsgRefreshStart || type == EMcMsgFormatStart ||             
          type == EMcMsgUSBMassStorageStart || type == EMcMsgUSBMTPStart )         
         )
        {
        //if refresh operation starts we block until refresh ends
        iRefreshOperation = ETrue;
        iCollectionUtility->Collection().CancelRequest(); 
        }
    else if ( event == TMPXCollectionMessage::EBroadcastEvent && 
              ( type == EMcMsgRefreshEnd || type == EMcMsgFormatEnd ||
               type == EMcMsgDiskRemoved || type == EMcMsgDiskInserted ||
               type == EMcMsgUSBMassStorageEnd || type == EMcMsgUSBMTPEnd )
            )
        {
        iRefreshOperation = EFalse;
        iCollectionUtility->Collection().CancelRequest();
        iCollectionUtility->Collection().MediaL( *iAllSongsPath , 
        	        iDefaultAttrs.Array() );
        }                       
    }       

// ---------------------------------------------------------------------------
// Publishes a number to the required destination
// ---------------------------------------------------------------------------
//

void CMpxCollectionPublisher::PublishNumberL( const TInt aNumb )
	{
	if ( iCPSInterface )
		{
		CLiwGenericParamList * inParam = CLiwGenericParamList::NewLC();
		CLiwGenericParamList * outParam = CLiwGenericParamList::NewLC();

		TLiwGenericParam cptype( KType, TLiwVariant( KCpData ) );
		inParam->AppendL( cptype );
		CLiwDefaultMap * cpdatamap = CLiwDefaultMap::NewLC();
		CLiwDefaultMap * map = CLiwDefaultMap::NewLC();

		map->InsertL( KMCPDataKey, TLiwVariant( TInt32( aNumb ) ) );

		cpdatamap->InsertL( KPublisherId, TLiwVariant( KMCPublisher ) );
		cpdatamap->InsertL( KContentType, TLiwVariant( KMCPType ) );
		cpdatamap->InsertL( KContentId, TLiwVariant( KMCPContent ) );
		cpdatamap->InsertL( KDataMap, TLiwVariant( map ) );

		TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ) );
		inParam->AppendL( item );

		iCPSInterface->ExecuteCmdL( KAdd, *inParam, *outParam );

		CleanupStack::PopAndDestroy( map );
		CleanupStack::PopAndDestroy( cpdatamap );
		CleanupStack::PopAndDestroy( outParam );
		CleanupStack::PopAndDestroy( inParam );
		}
	}


//  End of File

