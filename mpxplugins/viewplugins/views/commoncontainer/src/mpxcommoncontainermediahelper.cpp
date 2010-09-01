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
* Description:  Implementation of MPX common container Media helper.
*
*/


#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxlog.h>
#include "mpxcommoncontainer.h"
#include "mpxcommoncontainermediahelper.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// Two-phased constructor
// ----------------------------------------------------------------------------
//
CMPXCommonContainerMediaHelper* CMPXCommonContainerMediaHelper::NewL( 
    MMPXCommonContainer* aCommonContainer )
    {    
    CMPXCommonContainerMediaHelper* self = 
        new (ELeave) CMPXCommonContainerMediaHelper( aCommonContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor. 
// ----------------------------------------------------------------------------
//
CMPXCommonContainerMediaHelper::CMPXCommonContainerMediaHelper(
    MMPXCommonContainer* aCommonContainer )
    : iCommonContainer( aCommonContainer )
    {
    }

// ----------------------------------------------------------------------------
// 2nd phase constructor. 
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerMediaHelper::ConstructL()
    {
    iCollectionUtility = MMPXCollectionUtility::NewL( this );

#ifdef __USE_MESSAGE_SUBSCRIPTION
    // This class does not care about collection messages
    iCollectionUtility->Collection().ClearSubscriptionsL();
#endif
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXCommonContainerMediaHelper::~CMPXCommonContainerMediaHelper()
    {
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }
    }
    
// ----------------------------------------------------------------------------
// Retrieves the media given the current path, and upon return,
// either calls back the observer, or broadcasts the message to 
// the given client list
// ----------------------------------------------------------------------------
void CMPXCommonContainerMediaHelper::MediaL( 
    const TMPXItemId& aItemId, 
    RArray<TMPXAttribute> aAttrs,
    CMPXMedia* aAttrSpecs )
    {
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::MediaL(): Entering");

    CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();  
    CleanupStack::PushL( cPath );
    cPath->Set( aItemId );
    iCollectionUtility->Collection().MediaL( *cPath, aAttrs.Array(), aAttrSpecs );
    CleanupStack::PopAndDestroy( cPath );

    MPX_DEBUG1("CMPXCommonContainerMediaHelper::MediaL(): Exiting");
    }

// ----------------------------------------------------------------------------
// Updates the specified media with the results from the search
// criteria
// ----------------------------------------------------------------------------
void CMPXCommonContainerMediaHelper::UpdateMediaL( 
    CMPXMedia* aMedia, 
    RArray<TMPXAttribute> aSearchAttrs,
    CMPXMedia* aSearchAttrSpecs )
    {
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::UpdateMediaL(): Entering");

    CMPXMedia* mediaQuery = CMPXMedia::NewL();
    CleanupStack::PushL( mediaQuery );
    mediaQuery->SetTObjectValueL(KMPXMediaGeneralType, EMPXGroup);        
    mediaQuery->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXSong);
    TMPXItemId mediaId = *aMedia->Value<TMPXItemId>( KMPXMediaGeneralId );
    mediaQuery->SetTObjectValueL(KMPXMediaGeneralId, mediaId );
    mediaQuery->SetTObjectValueL(KMPXMediaGeneralContainerId, mediaId );
    mediaQuery->SetCObjectValueL<CMPXAttributeSpecs>(
        KMPXCommandMediaAttributeSpecs, aSearchAttrSpecs );

    // Find the attributes associated with the media
    CMPXMedia* queryResult = iCollectionUtility->Collection().FindAllL( 
        *mediaQuery, aSearchAttrs.Array() );
    CleanupStack::PopAndDestroy( mediaQuery );
    
    CleanupStack::PushL( queryResult );
    
    // update media with the query results
    const CMPXMediaArray* medias =
        queryResult->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    User::LeaveIfNull(const_cast<CMPXMediaArray*>(medias));
    
    aMedia->SetCObjectValueL(
        KMPXMediaArrayContents, const_cast<CMPXMediaArray*>(medias));
    aMedia->SetTObjectValueL(
        KMPXMediaArrayCount, medias->Count());
        
    CleanupStack::PopAndDestroy( queryResult );

    MPX_DEBUG1("CMPXCommonContainerMediaHelper::UpdateMediaL(): Exiting");
    }

// ----------------------------------------------------------------------------
// Cancels all outstanding requests
// ----------------------------------------------------------------------------
void CMPXCommonContainerMediaHelper::CancelRequests()
    {
    MPX_FUNC_EX("CMPXCommonContainerMediaHelper::CancelRequests()");
    if ( iCollectionUtility )
        {
        iCollectionUtility->Collection().CancelRequest();
        }
    }

// ----------------------------------------------------------------------------
// From MMPXCollectionObserver
// ----------------------------------------------------------------------------
void CMPXCommonContainerMediaHelper::HandleCollectionMessage( 
    CMPXMessage* /*aMsg*/,
    TInt /*aErr*/)
    {
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleCollectionMessageL(CMPXMessage): Entering");
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleCollectionMessageL(CMPXMessage): Exiting");
    }

// ----------------------------------------------------------------------------
// From MMPXCollectionObserver
// ----------------------------------------------------------------------------
void CMPXCommonContainerMediaHelper::HandleOpenL( 
    const CMPXMedia& /*aEntries*/,
    TInt /*aIndex*/, 
    TBool /*aComplete*/, 
    TInt /*aError*/ )
    {
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleOpenL(): Entering");
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleOpenL(): Exiting");
    }
    
// ----------------------------------------------------------------------------
// From MMPXCollectionObserver
// ----------------------------------------------------------------------------
void CMPXCommonContainerMediaHelper::HandleOpenL( 
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleOpenL(): Entering");
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleOpenL(): Exiting");
    }

// ----------------------------------------------------------------------------
// From MMPXCollectionObserver
// ----------------------------------------------------------------------------
void CMPXCommonContainerMediaHelper::HandleCommandComplete( 
    CMPXCommand* /*aCommandResult*/, 
    TInt /*aError*/ )
    {
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleCommandComplete(): Entering");
    MPX_DEBUG1("CMPXCommonContainerMediaHelper::HandleCommandComplete(): Exiting");
    }

// ----------------------------------------------------------------------------
// From MMPXCollectionMediaObserver
// ----------------------------------------------------------------------------
//
void CMPXCommonContainerMediaHelper::HandleCollectionMediaL(
    const CMPXMedia& aMedia, 
    TInt aError )    
    {
    MPX_FUNC_EX("CMPXCommonContainerMediaHelper::HandleCollectionMediaL");
    MPX_DEBUG2("CMPXCommonContainerMediaHelper::HandleCollectionMediaL(): error %d", aError);

    if( aError == KErrNone )
        {
        // Find the media in the common container
        if( aMedia.IsSupported( KMPXMediaGeneralId ) )
            {
            const TMPXItemId mediaId( 
                aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

            const CMPXMediaArray& mediaArray = 
                iCommonContainer->ListBoxArray()->MediaArray();
            const TInt count( mediaArray.Count() );
            
            for( TInt index = 0; index < count; index++ )
                {
                CMPXMedia* media( mediaArray.AtL( index ) );
                if( media->IsSupported( KMPXMediaGeneralId ) )
                    {
                    TMPXItemId containerId(
                        media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                    if ( mediaId == containerId )
                        {
                        // Update the list box item
                        *media = aMedia;
                        // Refresh the list box item
                        iCommonContainer->DrawLbxItemL( index );
                        break;
                        }
                    }
                }
            }
        }
    }

//End of file
