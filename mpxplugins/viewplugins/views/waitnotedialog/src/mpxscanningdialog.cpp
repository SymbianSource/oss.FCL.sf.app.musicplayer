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
    iInitialMMCCount = 0;
    iTotalNewTracks = 0;
    }


// ---------------------------------------------------------------------------
// 2nd Phased constructor
// ---------------------------------------------------------------------------
//
void CMPXScanningWaitDialog::ConstructL()
    {
    iHarvesterUtil = CMPXHarvesterFactory::NewL();
    BaseConstructL();
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
    iHarvesterUtil->ScanL();
    iScanningError = KErrNone;
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
        iScanningError = KErrCancel;
        // If harvester crashed,..... restart it.
        MPX_TRAPD( err, iHarvesterUtil->CancelScanL() );
        if( err != KErrNone )
            {
            iHarvesterUtil->Close();
            iHarvesterUtil = NULL;
            iHarvesterUtil = CMPXHarvesterFactory::NewL(); 
            }
        }
    else if( iScanningError >= KErrNone )
        {
        // Show completed scan note
        //
        if( iWaitNoteType == EMPXScanningNote || 
            iWaitNoteType == EMPXCorruptScanningNote )
            {
            text = StringLoader::LoadLC( R_MPX_SCANNING_COMPLETE_TXT, 
                                         iNumItemsAdded );
            textRsc = R_MPX_SCANNING_COMPLETE_TXT;                                         
            }
        else // iWaitNoteType == EMPXRefreshNote
            {
            text = StringLoader::LoadLC( R_MPX_REFRESHING_DB_COMPLETE_TXT, 
                                         iNumItemsAdded ); 
            textRsc = R_MPX_REFRESHING_DB_COMPLETE_TXT;                    
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
    iHarvesterUtil->RecreateDatabasesL();
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
            TInt mmcCount = 0;
            if ( iWaitNoteType == EMPXScanningNote)
                { 
                //get removable drive number
                TInt removableDrive( EDriveF );                            
                #ifdef RD_MULTIPLE_DRIVE
                    User::LeaveIfError( DriveInfo::GetDefaultDrive(
                        DriveInfo::EDefaultRemovableMassStorage,
                        removableDrive ) );
                #endif // RD_MULTIPLE_DRIVE
 
                //get count from music db
            
                CMPXCommand* cmdCount = CMPXMedia::NewL();
                CleanupStack::PushL( cmdCount );
                cmdCount->SetTObjectValueL<TMPXCommandId>( 
                        KMPXCommandGeneralId, 
                        KMPXCommandCollectionGetCount );
                cmdCount->SetTObjectValueL<TBool>( 
                        KMPXCommandGeneralDoSync,
                        ETrue );
                cmdCount->SetTObjectValueL<TInt>( 
                        KMPXCommandCollectionCountDrive,
                        removableDrive);
                
                cmdCount->SetTObjectValueL<TInt>( 
                        KMPXCommandCollectionCountTable,
                        EMPXCollectionCountTotal );
                                   
                // Get the collection UIDs
                RArray<TUid> ary;
                CleanupClosePushL( ary );
                ary.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
                TUid musicCollectionId = 
                iCollection->CollectionIDL( ary.Array() );
                CleanupStack::PopAndDestroy( &ary );
                cmdCount->SetTObjectValueL<TInt>( 
                        KMPXCommandGeneralCollectionId,
                        musicCollectionId.iUid );
    
                TRAPD(err, iCollection->Collection().CommandL( *cmdCount ) );
                
                if ( KErrNotFound != err )
                    {
                    User::LeaveIfError( err );
                    }

                // returned command should contain count
                if ( cmdCount->IsSupported( KMPXCommandCollectionCountValue ) )
                    {                        
                    mmcCount = 
                        cmdCount->ValueTObjectL<TInt>( 
                                KMPXCommandCollectionCountValue );
                    }
                CleanupStack::PopAndDestroy( cmdCount );
                }
            if( op == EMcMsgRefreshStart )
                {
                MPX_DEBUG1("refreshStart store the initial count");
                iInitialMMCCount = mmcCount;
                }

            if( op == EMcMsgRefreshEnd )                                    
            {
            MPX_DEBUG1("CMPXScanningWaitDialog::HandleCollectionMessageL refresh end");
            iNumItemsAdded = mmcCount;
            switch ( iWaitNoteType )
                {
                case EMPXScanningNote:
                    {
                    //total new tracks on external memory 
                    TInt totalNewOnMMC = iNumItemsAdded - iInitialMMCCount;

                    //total new tracks on internal memory 
                    TInt newOnPhone = iTotalNewTracks - totalNewOnMMC; 

                    //total new tracks to dispaly 
                    iNumItemsAdded += newOnPhone;

                    break;
                    }

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
                    if( data >= KErrNone )
                        {
                        iNumItemsAdded = data;
                        }
                    break;
                    }
                };
            
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
                // Update wait note text if refreshing
                if( iWaitNoteType == EMPXRefreshingNote &&
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
    
// END OF FILE

