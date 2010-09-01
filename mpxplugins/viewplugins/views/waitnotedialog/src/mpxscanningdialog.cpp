/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wait note dialog for scanning
*
*/


#include <e32base.h>
#include <StringLoader.h>
#include <avkon.mbg>
#include <aknconsts.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <featmgr.h>
#include <driveinfo.h>

#include <mpxlog.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionmessage.h>
#include <mpxwaitnotedialog.rsg>
#include <mpxharvesterutility.h>
#include <mpxcollectionutility.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxcollectionmessagedefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include "mpxscanningdialog.h"


// ---------------------------------------------------------------------------
// Default Constructor
// ---------------------------------------------------------------------------
//
CMPXScanningWaitDialog::CMPXScanningWaitDialog( MMPXWaitNoteObserver* aObs,
                                                TWaitNoteType aType ) : 
                                             CMPXWaitNoteDialog( aObs, aType )
    {
    iNumItemsAdded = 0;
    iTotalNewTracks = 0;
    iInitialCount = 0;
    isCollectionDBChanged = EFalse;
    iAsyncEvent = ECmdIdle;
    iRefreshEnd = EFalse;

    }


// ---------------------------------------------------------------------------
// 2nd Phased constructor
// ---------------------------------------------------------------------------
//
void CMPXScanningWaitDialog::ConstructL()
    {
	 MPX_DEBUG1("CMPXScanningWaitDialog::ConstructL <---");
    BaseConstructL();

	TCallBack callback( CMPXScanningWaitDialog::AsyncCallHarvesterEventL, this );
	iAsyncCallBack = new (ELeave) CAsyncCallBack( CActive::EPriorityHigh );
	iAsyncCallBack->Set( callback );
    MPX_DEBUG1("CMPXScanningWaitDialog::ConstructL --->");
    }


// ---------------------------------------------------------------------------
// Two Phased constructor
// ---------------------------------------------------------------------------
//
CMPXScanningWaitDialog* CMPXScanningWaitDialog::NewL( MMPXWaitNoteObserver* aObs, 
                                                      TWaitNoteType aType  )
    {
    CMPXScanningWaitDialog* self = new(ELeave)CMPXScanningWaitDialog( aObs, 
                                                                      aType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Virtual destructor
// ---------------------------------------------------------------------------
//
CMPXScanningWaitDialog::~CMPXScanningWaitDialog()
    {
    if( iHarvesterUtil )
        {
        iHarvesterUtil->Close();
        }
    iHarvesterUtil = NULL;
    delete iAsyncCallBack;
    iAsyncCallBack = NULL;
    }

// ---------------------------------------------------------------------------
// CMPXScanningWaitDialog::PreNoteDisplayHandleL()
// ---------------------------------------------------------------------------
//
void CMPXScanningWaitDialog::PreNoteDisplayHandleL()
    {
    // CBA
    SetCBAL( R_MPX_WAITNOTE_SOFTKEYS_EMPTY_STOP );
    
    // Text
    HBufC* text = NULL;
    if( iWaitNoteType == EMPXScanningNote )
        {
        text = StringLoader::LoadLC( R_MPX_SCANNING_DB_TXT );
        }
    else if( iWaitNoteType == EMPXRefreshingNote )
        {
        text = StringLoader::LoadLC( R_MPX_REFRESHING_DB_TXT, 0 );
        }
    else // iWaitNoteType == EMPXCorruptScanningNote
        {
        text = StringLoader::LoadLC( R_MPX_REPAIRING_CORRUPT_DB );    
        }
        
    SetTextL( *text );
    CleanupStack::PopAndDestroy( text );
    
    // Icon
    TNoteIconInfo icon( (TInt)EMbmAvkonQgn_note_progress, 
                    (TInt)EMbmAvkonQgn_note_progress_mask, 
                    TFileName(KAvkonBitmapFile) );
    SetIconL( icon );
    
    // If we are repairing a corrupt db, we first clean up the old ones
    //
    if( iWaitNoteType == EMPXCorruptScanningNote )
        {
        HandleDatabaseCorruptionL();
        }
        
    // Start the scanning in harvester
    // Just to be faster, lets just create a small async callback and then
    // call ScanL   
    if( !iAsyncCallBack->IsActive() )
    	{
		iAsyncEvent = ECmdScan;
		iCancelScan = EFalse;
		iAsyncCallBack->CallBack();
    	}
    }

// ---------------------------------------------------------------------------
// CMPXScanningWaitDialog::PostNoteHandleL()
// ---------------------------------------------------------------------------
//
void CMPXScanningWaitDialog::PostNoteHandleL( TInt aButtonId )
    {
    MPX_DEBUG1("CMPXScanningWaitDialog::PostNoteHandleL --->");
    
    MPX_DEBUG1("    BUGHUNT: cover display");
    TInt textRsc( KErrNotFound );
    
    HBufC* text = NULL;
    if( aButtonId == EAknSoftkeyCancel )
        {
		// cancel scan in harvester
		// Just to be faster, lets just create a small async callback and then
		// call CancelScanL   
		if( !iAsyncCallBack->IsActive() )
			{
			iAsyncEvent = ECmdCancleScan;
			iAsyncCallBack->CallBack();
			}      
		else
			{
		    iCancelScan = ETrue;
			}

		if (!iRefreshEnd)
			{
			// When stop refreshing library, prompt a process waiting dialog.
			// Inform user to wait for the updating library.
			HBufC* waitText = StringLoader::LoadLC( R_MPX_UPDATING_LIBRARY_TXT );
			DisplayProcessWaitDialogL( R_MPX_GENERIC_WAIT_NOTE, *waitText );
			CleanupStack::PopAndDestroy( waitText );
			}
        }
    else if( iScanningError >= KErrNone )
        {
        // Show completed scan note
        //
        if( iWaitNoteType == EMPXScanningNote || 
            iWaitNoteType == EMPXCorruptScanningNote )
            {
            if ( isCollectionDBChanged )
                { 
                text = StringLoader::LoadLC( R_MPX_SCANNING_COMPLETE_TXT, 
                                                     iNumItemsAdded );
                textRsc = R_MPX_SCANNING_COMPLETE_TXT;                
                }
            else
                {
                text = StringLoader::LoadLC( R_MPX_UPTODATE_TXT );
                textRsc = R_MPX_UPTODATE_TXT;
                }                               
            }
        else // iWaitNoteType == EMPXRefreshNote
            {
            if ( isCollectionDBChanged )
                {
                text = StringLoader::LoadLC( R_MPX_REFRESHING_DB_COMPLETE_TXT, 
                                         iNumItemsAdded ); 
                textRsc = R_MPX_REFRESHING_DB_COMPLETE_TXT;              
                }
            else
                {
                text = StringLoader::LoadLC( R_MPX_UPTODATE_TXT );
                textRsc = R_MPX_UPTODATE_TXT; 
                }
            }
        }
    else if( iScanningError == KErrDiskFull )
        {
        // If the error was disk full
        //
        text = StringLoader::LoadLC( R_MPX_MEM_LO_NOT_ENOUGH_MEMORY, 
                                     iNumItemsAdded );
        }
    else 
        {
        // Show stopped notes    
        //
        if( iWaitNoteType == EMPXScanningNote || 
            iWaitNoteType == EMPXCorruptScanningNote )
            {
            textRsc = R_MPX_SCANNING_STOPPED_TXT;
            text = StringLoader::LoadLC( R_MPX_SCANNING_STOPPED_TXT, 
                                         iNumItemsAdded  );
            }
        else // iWaitNoteType == EMPXRefreshNote
            {
            textRsc = R_MPX_REFRESHING_DB_STOPPED_TXT;
            text = StringLoader::LoadLC( R_MPX_REFRESHING_DB_STOPPED_TXT, 
                                         iNumItemsAdded );    
            }
        }
    
    // Show the note
    if( text )
        {
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        DisplayNoteDialogL( R_MPX_EMPTY_CLOSE_NOTE, textRsc, iNumItemsAdded,
                            *text, CAknNoteDialog::EConfirmationTone );
        }
    else
        {
        // When finish updating library, cancle the process waiting 
        // dialog and then prompt the refreshing completion dialog.
        CancelProcessWaitDialogL();
        DisplayNoteDialogL( R_MPX_EMPTY_CLOSE_NOTE, *text, 
                            CAknNoteDialog::EConfirmationTone );
        }
        CleanupStack::PopAndDestroy( text );
        }

    MPX_DEBUG1("<--CMPXScanningWaitDialog::PostNoteHandleL");
    }

// ---------------------------------------------------------------------------
// Handle the collection message
// ---------------------------------------------------------------------------
//
void CMPXScanningWaitDialog::HandleCollectionMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// Cleanup both collection and harvester databases
// ---------------------------------------------------------------------------
//
void CMPXScanningWaitDialog::HandleDatabaseCorruptionL()
    {
    // Cleanup Harvester
	if( iHarvesterUtil != NULL )
		{
	    iHarvesterUtil->RecreateDatabasesL();
		}
    }

// ---------------------------------------------------------------------------
// MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXScanningWaitDialog::DoHandleCollectionMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXScanningWaitDialog::DoHandleCollectionMessageL" );
    
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt op( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        
        if(event == TMPXCollectionMessage::EBroadcastEvent &&
                ((op == EMcMsgRefreshStart) || (op == EMcMsgRefreshEnd)))
            {
            TInt songTotal = 0;
            
            if ( iWaitNoteType == EMPXScanningNote )
                {      
                RArray<TUid> ary;
                CleanupClosePushL( ary );
                ary.AppendL(TUid::Uid( EMPXCollectionPluginPodCast ) );
                TUid podcastCollectionId = iCollection->CollectionIDL( ary.Array() );
                ary.Reset();
                ary.AppendL(TUid::Uid(EMPXCollectionPluginMusic));				
                TUid musicCollectionId = iCollection->CollectionIDL( ary.Array() );
                CleanupStack::PopAndDestroy(&ary);
				
				TInt removableDrive( EDriveF );                            
                #ifdef RD_MULTIPLE_DRIVE
                    User::LeaveIfError( DriveInfo::GetDefaultDrive(
                        DriveInfo::EDefaultRemovableMassStorage,
                        removableDrive ) );
                #endif // RD_MULTIPLE_DRIVE 
                
                TInt massStorageDrive( EDriveE );
                User::LeaveIfError( DriveInfo::GetDefaultDrive(
                        DriveInfo::EDefaultMassStorage,
                        massStorageDrive ) );
                
                songTotal = GetTrackCountL( removableDrive, 
                        podcastCollectionId.iUid, EMPXCollectionCountTrack );
                songTotal += GetTrackCountL( massStorageDrive,
                        podcastCollectionId.iUid, EMPXCollectionCountTrack );
                
                songTotal += GetTrackCountL( removableDrive,
                        musicCollectionId.iUid, EMPXCollectionCountTotal );
                songTotal += GetTrackCountL( massStorageDrive,
                        musicCollectionId.iUid, EMPXCollectionCountTotal );
                
                }
            if( op == EMcMsgRefreshStart )
                {
				iRefreshEnd = EFalse;
                isCollectionDBChanged = EFalse;
                MPX_DEBUG1("refreshStart store the initial count");
                iInitialCount = songTotal;
                }

            if( op == EMcMsgRefreshEnd )                                    
            {
            MPX_DEBUG1("CMPXScanningWaitDialog::HandleCollectionMessageL refresh end");
            // Do not reset iNumItemsAdded while KErrLocked or KErrDiskFull.
            if( data != KErrLocked && data != KErrDiskFull ) 
            	{
            	iNumItemsAdded = songTotal ;
            	}
            switch ( iWaitNoteType )
                {
                case EMPXScanningNote:
                case EMPXOpeningNote:
                case EMPXRefreshingNote:
                case EMPXCorruptScanningNote:
                case EMPXUsbEventNote:
                case EMPXMTPEventNote:
                case EMPXFormatScanningNote:
                case EMPXMediaNotAvailableNote:
                case EMPXNoteNotDefined:
                    // fall through
                default:
                    {
                    // Synchronize the number of items added if we had no errors
                    //
                    if( data > KErrNone || iTotalNewTracks )
                        {
                        isCollectionDBChanged = ETrue;
                        iNumItemsAdded = iTotalNewTracks;
                        }
                    else
                    	{
                        isCollectionDBChanged = EFalse;
                    	}
                    break;
                    }
                };
            
			iRefreshEnd = ETrue;
            // If cancel was pressed, iWaitDialog is NULL
            //
            if( iWaitDialog )
                {
                MPX_DEBUG1("CMPXScanningWaitDialog::HandleCollectionMessageL killing dialog");
                iScanningError = data;
                iWaitDialog->ProcessFinishedL();
                }
                
            // Delay showing the number added dialog
            if( iScanningError == KErrCancel )
                {
                PostNoteHandleL( EAknSoftkeyOk );
                }
            
            // Save the error code
            iScanningError = data;
            }
        }
        else if( event == TMPXCollectionMessage::EBroadcastEvent && 
                 op == EMcMsgDiskInserted )
            {
            // Dismiss this scanning note because scanning will be restarted
            //
            MPX_DEBUG1("CMPXScanningWaitDialog::HandleCollectionMessageL \
                        dismissing scan because of refresh msg ");
            if( iWaitDialog )
                {
                MPX_DEBUG1("CMPXScanningWaitDialog::HandleCollectionMessageL killing dialog");
                iWaitDialog->ProcessFinishedL();
                }
            
            iScanningError = KErrNone;  
            }
        }
    else if(id == KMPXMessageIdItemChanged)
        {
        // Loop through messages for arrays.
        //
        if (aMessage.IsSupported(KMPXMessageArrayContents))
            {
            const CMPXMessageArray* messageArray =
                        aMessage.Value<CMPXMessageArray>(KMPXMessageArrayContents);
            User::LeaveIfNull(const_cast<CMPXMessageArray*>(messageArray));
            
            for( TInt i=0; i<messageArray->Count(); ++i )
                  {
                  HandleCollectionMessage( messageArray->AtL( i ), KErrNone );  
                  } 
            }
        // Single item
        else
            {
            MPX_DEBUG1("CMPXScanningWaitDialog::HandleCollectionMessageL KMPXMessageIdItemChanged");
            TMPXChangeEventType changeType( aMessage.ValueTObjectL<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
            TMPXGeneralCategory cat(aMessage.ValueTObjectL<TMPXGeneralCategory>(KMPXMessageMediaGeneralCategory));
            if( changeType == EMPXItemInserted && 
                (cat == EMPXSong || cat == EMPXPlaylist || cat == EMPXPodcast) )
                {
                iNumItemsAdded++;
                iTotalNewTracks++;
                // Update wait note text if refreshing or scaning
                if( iWaitNoteType == EMPXRefreshingNote || iWaitNoteType == EMPXScanningNote || iWaitNoteType == EMPXCorruptScanningNote &&
                    iScanningError == KErrNone )
                    {
                    HBufC* text = StringLoader::LoadLC( R_MPX_REFRESHING_DB_TXT, 
                                                        iNumItemsAdded );
                    SetTextL( *text ); 
                    CleanupStack::PopAndDestroy( text );
                    }
                }    
            }
        } 
    }
// ---------------------------------------------------------------------------
// Get track count for given table in the db
// ---------------------------------------------------------------------------
//
TInt CMPXScanningWaitDialog::GetTrackCountL(TInt aDrive,TInt aColDbId, TInt aColTable)
    {
    MPX_DEBUG2("--->CMPXScanningWaitDialog::GetTrackCountL() aDrive = %d", aDrive );
    TInt count(0);

    //get count from db
    CMPXCommand* cmdCountM = CMPXMedia::NewL();
    CleanupStack::PushL(cmdCountM);
    cmdCountM->SetTObjectValueL<TMPXCommandId>( 
	        KMPXCommandGeneralId, 
	        KMPXCommandCollectionGetCount );
    cmdCountM->SetTObjectValueL<TBool>(
            KMPXCommandGeneralDoSync, 
            ETrue );
    cmdCountM->SetTObjectValueL<TInt>(
            KMPXCommandGeneralCollectionId,
            aColDbId );
    cmdCountM->SetTObjectValueL<TInt>(
            KMPXCommandCollectionCountDrive,
            aDrive );
    cmdCountM->SetTObjectValueL<TInt>(
            KMPXCommandCollectionCountTable,
            aColTable );

    TRAPD( err, iCollection->Collection().CommandL( *cmdCountM ) );

    // returned command should contain count
    if ( err == KErrNone && cmdCountM->IsSupported(KMPXCommandCollectionCountValue ) )
        {
        count = cmdCountM->ValueTObjectL<TInt>(KMPXCommandCollectionCountValue);
        }
    
    CleanupStack::PopAndDestroy(cmdCountM);
    MPX_DEBUG2("--->CMPXScanningWaitDialog::GetTrackCountL() count = %d", count );

    return count;
    }
// ---------------------------------------------------------------------------
// async callback 
// ---------------------------------------------------------------------------
//
TInt CMPXScanningWaitDialog::AsyncCallHarvesterEventL( TAny* aSelf )
    {
	MPX_DEBUG1("CMPXScanningWaitDialog::CallHarvesterScanL <---");
	CMPXScanningWaitDialog* self = static_cast<CMPXScanningWaitDialog*>( aSelf );
	if( self->iHarvesterUtil == NULL )
		{
	    self->iHarvesterUtil = CMPXHarvesterFactory::NewL();
		}
	if( self->iAsyncEvent == ECmdScan )
		{
	    if( !self->iCancelScan )
	    	{
		    self->iHarvesterUtil->ScanL();
		    self->iScanningError = KErrNone;
	    	}
	    else
	    	{
	        self->iCancelScan = EFalse;
	    	}
		}
	
	if( self->iAsyncEvent == ECmdCancleScan || self->iCancelScan )
		{
	    self->iScanningError = KErrCancel; 
        // If harvester crashed,..... restart it.
		MPX_TRAPD( err, self->iHarvesterUtil->CancelScanL() );
		if( err != KErrNone )
			{
		    self->iHarvesterUtil->Close();
		    self->iHarvesterUtil = NULL;
		    self->iHarvesterUtil = CMPXHarvesterFactory::NewL(); 
			}
		}
	self->iAsyncEvent = ECmdIdle;
    MPX_DEBUG1("CMPXScanningWaitDialog::CallHarvesterScanL --->");
    return KErrNone;
    }
// END OF FILE

