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
* Description:  Controller for MPX Music player
*
*/


#include <e32base.h>
#include <mpx.rsg>
#include <mpxlog.h>

// Member Variables
#include <mpxcollectionutility.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxviewpluginmanager.h>
#include <mpxviewplugin.h>
#include <mpxplaybackutility.h>

// MPX Attributes
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>

// Other MPX
#include <mpxmedia.h>
#include <mpxcollectionpath.h>
#include <mpxmusicplayer.hrh>

// Cover UI start
//#ifdef __COVER_DISPLAY
#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include <mplayersecondarydisplayapi.h>
//#endif
// Cover UI end

// System
#include <StringLoader.h>
#include <textresolver.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>

// Header
#include "mpxcontroller.h"

// CONSTANTS
const TInt KWaitNoteImpUid = 0x101FFC6C;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXController::CMPXController( TBool aDisablePodcasting ) 
                                 : iCurSystemEvent(KErrNotFound),
                                   iCurPlugin(KErrNotFound),
                                   iDisablePodcasting(aDisablePodcasting)
    {
    
    }


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CMPXController::ConstructL()
    {
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iViewUtility = MMPXViewUtility::UtilityL();
    iIdle = CIdle::NewL( CActive::EPriorityStandard );
    
    // Monitor for when the app is first activated
    CEikonEnv::Static()->AppUi()->AddViewActivationObserverL( this );
    
    // Fetch the music collection UID
    RArray<TUid> uid;
    CleanupClosePushL( uid );
    uid.AppendL( TUid::Uid(EMPXCollectionPluginMusic) );
    iMusicCollectionId = iCollectionUtility->CollectionIDL( uid.Array() );
    
    // Fetch the podcast collection UID
    if( !iDisablePodcasting )
        {
        uid.Reset();
        uid.AppendL( TUid::Uid(EMPXCollectionPluginPodCast) );
        iPodcastCollectionId = iCollectionUtility->CollectionIDL( uid.Array() );;
        }
    CleanupStack::PopAndDestroy( &uid );

    // Do the initial checking
    DoRetrieveDetailsL();
    }


// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//
CMPXController* CMPXController::NewL( TBool aDisablePodcasting )
    {
    CMPXController* self = new( ELeave ) CMPXController( aDisablePodcasting );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXController::~CMPXController()
    {
    CEikonEnv::Static()->AppUi()->RemoveViewActivationObserver( this );
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }
    
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }
    
    if ( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }
        
    delete iCurPath;
    delete iIdle;
    iCurAtts.Close();
    }

    
// ---------------------------------------------------------------------------
// Handle an idle callback event
// ---------------------------------------------------------------------------
//
TInt CMPXController::IdleCallback( TAny* ptr )
    {
    TRAP_IGNORE( ( (CMPXController*) ptr )->HandleIdleEventL() );
    return 0;
    }

// ---------------------------------------------------------------------------
// Handle an idle callback event
// ---------------------------------------------------------------------------
//    
void CMPXController::HandleIdleEventL()
    {
    MPX_FUNC( "CMPXController::HandleIdleEventL" );
    if( iRestartWaitDialog )
        {
        DoHandleBroadcastMsgL( iCurSystemEvent );
        iRestartWaitDialog = EFalse;
        }
    else
        {
        TInt event = iDelayedUsbRefresh ? EMcMsgUSBMassStorageStart : iCurSystemEvent;
        switch( event )
            {
            case EMcMsgUSBMassStorageStart:
                {
                MPX_DEBUG1("CMPXController::HandleIdleEventL -- Mass Storage dialog");
                iCurSystemEvent = KErrNotFound;
                iDelayedUsbRefresh = EFalse;

                HBufC* title = StringLoader::LoadLC( R_MPX_REFRESH_AFTER_SYNC );
                CAknQueryDialog* query = new( ELeave ) CAknQueryDialog();
                iQueryDialog = query;

// Cover UI start
//#ifdef __COVER_DISPLAY            
		            query->PublishDialogL(
		                EMPlayerNoteUSBSyncRefresh,
		                KMPlayerNoteCategory); 
//#endif           
// Cover UI end         
                 
                TInt rtn = query->ExecuteLD( R_MPX_QUERY_YES_NO , 
                                             *title );
                CleanupStack::PopAndDestroy( title );
                iQueryDialog = NULL;  // Dialog destroyed
                
                if ( rtn == EAknSoftkeyYes )
                    {
                    StartWaitNoteL( EMPXRefreshingNote );  
                    }
                else
                    {
                    MPX_DEBUG1("CMPXController::HandleIdleEventL -- refreshing view");
                    
                    // Refresh the UI view in this case
                    // 
                    if ( !CEikonEnv::Static()->StartedAsServerApp() )
                        {
                        CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
                        CleanupStack::PushL( cPath );
                        if( cPath->Levels() > 1 && 
                            iViewUtility->ActiveViewType() == TUid::Uid(KMPXPluginTypeCollectionUid) ) 
                            {
                            MPX_DEBUG1("CMPXController::HandleIdleEventL() Refreshing UI");
                            cPath->Back();
                            iCollectionUtility->Collection().OpenL( *cPath );
                            }
                        CleanupStack::PopAndDestroy( cPath );
                        }
                    
                    DoRetrieveDetailsL();
                    }
                break;
                }
            default:
                {
                break;    
                }
            
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// ---------------------------------------------------------------------------
//   
void CMPXController::HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXController::HandleOpenL( const CMPXMedia& /*aEntries*/,
                                  TInt /*aIndex*/,
                                  TBool /*aComplete*/,
                                  TInt /*aError*/ )
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXController::HandleOpenL( const CMPXCollectionPlaylist& /*aPlaylist*/,
                                   TInt /*aError*/ )
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXController::HandleCollectionMediaL( const CMPXMedia& aMedia,
                                             TInt aError )
    {
    MPX_FUNC( "CMPXController::HandleCollectionMediaL" );
    
    // Handle Each error condition
    //
    if (aError == KErrNoMemory)
        {
        TRAP_IGNORE(
                HandleErrorL( aError );
                );
        CAknEnv::RunAppShutter();
        User::Exit(KErrNoMemory);
        }
    else if( aError == KErrCorrupt )
        {
        DoHandleCorruptMsgL();
        }
    else if (aError == KErrDiskFull)
        {
        iOutOfDisk = ETrue;
        CloseWaitNoteL();
        iCurPlugin = KErrNotFound;
        DoRetrieveDetailsL(ETrue);         
        } 
    else // !KErrNoMemory, !KErrCorrupt, !KErrDiskFull
        {
        if(!iOutOfDisk)
            {
            // Check to see if database has been created for this item
            //
            TBool dbCreated = ETrue;
            if( aMedia.IsSupported(KMPXMediaColDetailDBCreated))
                {
                dbCreated = aMedia.ValueTObjectL<TBool>(KMPXMediaColDetailDBCreated);    
                MPX_DEBUG2("CMPXController::HandleCollectionMediaL dbcreated %i", dbCreated);
                }
            iInitDBNeeded |= !dbCreated;  //lint !e514 
       
            // Check to see if database was corrupted for this item
            //
            TBool dbCorrupted = EFalse;
            if( aMedia.IsSupported(KMPXMediaColDetailDBCorrupted))
                {
                dbCorrupted = aMedia.ValueTObjectL<TBool>(KMPXMediaColDetailDBCorrupted);
                }
            iInitDBCorrupted |= dbCorrupted;
            }
        
        // Update the list box item, iCurPlugin++ because first item is pb state
        //
        iCurPlugin++;
        
        // Check to see if we need to do initial scan or refresh due to db corruption
        //    
        if( iCurPlugin == iCurPath->Count() )
            {
            MPX_DEBUG1("CMPXController::HandleCollectionMediaL plugins MediaL loaded");
            
            if( (iInitDBNeeded || iInitDBCorrupted) && !iOutOfDisk )
                {
                MPX_DEBUG2("CMPXController::HandleCollectionMediaL iUIReady %d", iUIReady );
                //If UI isn't ready,we should wait until UI is ready.
                //Otherwise we would likely see greyed out status pane.
                if( iUIReady )
                    {
                    MPX_DEBUG1("CMPXController::HandleCollectionMediaL starting scan");
                    TWaitNoteType noteType = iInitDBCorrupted ? EMPXCorruptScanningNote :
                                                                EMPXScanningNote;
    
                    StartWaitNoteL( noteType );
                    iInitDBNeeded = EFalse; 
                    iInitDBCorrupted = EFalse;
                    iRefreshingCollection = ETrue;
                    iDelayedUsbRefresh = EFalse;
                    }
                else
                    {
                    iDelayedRefreshForUIReady = ETrue;
                    }
                }
            else if( iDelayedUsbRefresh )
                {
                DoHandleBroadcastMsgL( EMcMsgUSBMassStorageEnd );
                }
            iCurPlugin = KErrNotFound;
            }  
        else if( iCurPlugin < iCurPath->Count() )  // Fetch next
            {
            // Set which plugin
            iCurPath->Set(iCurPlugin);
            
            MPX_DEBUG1("CMPXController::HandleCollectionMediaL fetch next");
            
            // Only fetch next if we haven't been interrupted by another event
            //
            if( (iCurSystemEvent == KErrNotFound) && !iOutOfDisk )
                {
                iCollectionUtility->Collection().MediaL(*iCurPath, iCurAtts.Array() );
                }
            else
                {
                // Reset state machine
                iCurPlugin = KErrNotFound;
                }
            }
            
        if ( aError != KErrNone ) //lint !e961      
            {
            HandleErrorL( aError );
            }
        }
    }

// ---------------------------------------------------------------------------
// Wait until the view is ready
// ---------------------------------------------------------------------------
//
void CMPXController::HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId,
                                           const TVwsViewId& /*aViewIdToBeDeactivated*/)
    {
    if( !iUIReady && aNewlyActivatedViewId.iAppUid == TUid::Uid(KMusicPlayerAppUidConstant) )
        {
        iUIReady = ETrue;
        
        if( iDelayedUsbRefresh )
            {
            TRAP_IGNORE( DoHandleBroadcastMsgL( EMcMsgUSBMassStorageEnd ) );
            }
        else if( iCurSystemEvent == EMcMsgUSBMassStorageStart ||
                 iCurSystemEvent == EMcMsgUSBMTPStart )
            {
            // If the view has not been active, we would likely see 
            // greyed out note. So we re-start the dlg
            //
            TRAP_IGNORE( CloseWaitNoteL() );
            
            iRestartWaitDialog = ETrue;
            if( iIdle->IsActive() )
                {
                iIdle->Cancel();
                }
            TCallBack cb( &IdleCallback, this );
            iIdle->Start( cb );
            }
        else if( iDelayedRefreshForUIReady )
            {
            TRAP_IGNORE( DoRetrieveDetailsL() );
            }
        }
    }

// ---------------------------------------------------------------------------
// Start a refreshing note
// ---------------------------------------------------------------------------
//
void CMPXController::StartWaitNoteL( TWaitNoteType aNoteType )
    {
    CloseWaitNoteL();
    TUid waitnoteId = TUid::Uid( KMPXPluginTypeWaitNoteDialogUid );
    TPckg<TWaitNoteType> note = aNoteType;
    HBufC* arg = MPXUser::AllocL( note );
    CleanupStack::PushL( arg );
    iViewUtility->ActivateViewL( waitnoteId, arg );
    CleanupStack::PopAndDestroy( arg );
    }

// ---------------------------------------------------------------------------
// Close waitnote dialog
// ---------------------------------------------------------------------------
//
void CMPXController::CloseWaitNoteL(TBool aSkipCheckIfActive)
    {
    TUid waitnoteId = TUid::Uid( KMPXPluginTypeWaitNoteDialogUid );
    TUid activeView = iViewUtility->ActiveViewType();
    if(( activeView == waitnoteId ) || (aSkipCheckIfActive))
        {
        CMPXViewPlugin* pi = 
            iViewUtility->ViewPluginManager().PluginL( TUid::Uid(KWaitNoteImpUid) );
        pi->DeactivateView();
        }
    }

// ---------------------------------------------------------------------------
// Calls MediaL to retrieve item details
// ---------------------------------------------------------------------------
//
void CMPXController::DoRetrieveDetailsL(TBool aSkipAttribute)
    {
    MPX_DEBUG2("CMPXController::DoRetrieveDetailsL %i", iCurPlugin);
    
    // Make sure we don't overlap 2 sets of MediaL()
    //
    if( iCurPlugin == KErrNotFound && iCurSystemEvent == KErrNotFound )
        {
        delete iCurPath;
        iCurPath = NULL;                
        
        // Default path consisting of the music and podcast plugins
        RArray<TMPXItemId> ids;
        CleanupClosePushL( ids );
        
        // Root level path
        iCurPath = CMPXCollectionPath::NewL();
        ids.AppendL( TMPXItemId(iMusicCollectionId.iUid) );
        if( !iDisablePodcasting )
            {
            ids.AppendL( TMPXItemId(iPodcastCollectionId.iUid) );
            }
        iCurPath->AppendL( ids.Array() );
        
        CleanupStack::PopAndDestroy( &ids );

        if( iCurPath->Levels() == 1 && iCurPath->Count() )
            {
            iCurPath->SetToFirst();
            iCurPlugin = 0;
            
            // Do a MediaL to re-retrieve details
            //
            iCurAtts.Reset();
            if(!aSkipAttribute)
                {
                iCurAtts.Append(KMPXMediaColDetailDBCreated);
                iCurAtts.Append(KMPXMediaColDetailDBCorrupted);
                }

            iCollectionUtility->Collection().MediaL(*iCurPath, iCurAtts.Array() );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handle collection messages.
// ---------------------------------------------------------------------------
//
void CMPXController::DoHandleCollectionMessageL( 
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXCommandHandler::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id ) 
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt op( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );    
        
        // End of refresh message, update the list box text
        //
        if( event == TMPXCollectionMessage::EBroadcastEvent && 
            op == EMcMsgRefreshEnd )
            {
            MPX_DEBUG1("CMPXController::HandleCollectionMessageL - refresh end");
            iRefreshingCollection = EFalse;
            iOutOfDisk = data == KErrDiskFull ? ETrue : EFalse; 
            
            iCurPlugin = KErrNotFound;
            iInitDBNeeded = EFalse;
            iInitDBCorrupted = EFalse;
            
            DoRetrieveDetailsL(data != KErrNone);
            }
        else if(event == TMPXCollectionMessage::EBroadcastEvent && 
                op == EMcMsgRefreshStart)
            {
            iRefreshingCollection = ETrue;
            }
        // Handle other broadcast messages
        //
        else if( event == TMPXCollectionMessage::EBroadcastEvent )
            {
            MPX_DEBUG1("CMPXController::HandleCollectionMessageL - broadcast");
            DoHandleBroadcastMsgL( op );
            }
        }
    }

// ---------------------------------------------------------------------------
// Any processing to handle broadcast events
// ---------------------------------------------------------------------------
//
void CMPXController::DoHandleBroadcastMsgL( TInt aEvent )
    {
    MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg<--");
    switch( aEvent )
        {
        case EMcMsgFormatStart:
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgFormatStart");
            
            // If we were in MTP sync, we stay in mtp sync mode
            if( iCurSystemEvent == KErrNotFound )
                {
                // Show a formatting wait note
                iCurSystemEvent = EMcMsgFormatStart;
                iDiskDismountDuringFormat = EFalse;
                iPlaybackUtility->CommandL( EPbCmdStop );
                
                StartWaitNoteL( EMPXFormatScanningNote );
                }
            break;
            }
        case EMcMsgFormatEnd:
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgFormatEnd");
            if( iCurSystemEvent == EMcMsgFormatStart )
                {
                iCurSystemEvent = KErrNotFound;
                
                if( !iDiskDismountDuringFormat )
                    {
                    MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - Sync db after format");
                    
                    if( !iRefreshingCollection )
                        {
                        StartWaitNoteL( EMPXScanningNote );
                        iRefreshingCollection = ETrue;  
                        }
                    }
                }
            break;
            }    
        case EMcMsgDiskRemoved:
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgDiskRemoved");
            iPlaybackUtility->CommandL( EPbCmdStop );

            if( iCurSystemEvent == EMcMsgFormatStart )
                {
                iDiskDismountDuringFormat = ETrue;
                }
            else
                {
                // Check the database flags
                DoRetrieveDetailsL();
                }
            break;
            }
        case EMcMsgDiskInserted:
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgDiskInserted");
            // Only show the query if we are not processing a usb event
            // USB dismounts and re-mounts the drive several times
            //
            // Ignore the disk insert during format, after format a fake event will be sent.
            //
            if( iCurSystemEvent != EMcMsgUSBMassStorageStart &&
                iCurSystemEvent != EMcMsgUSBMTPStart &&
                iCurSystemEvent != EMcMsgFormatStart )
                {
                if( iQueryDialog )
                    {
                    MPX_DEBUG1("Main View -- Dismissing Query");
                    iQueryDialog->DismissQueryL();
                    }
                
                // Always start a refresh
                //
                if( !iRefreshingCollection )
                    {
                    StartWaitNoteL( EMPXScanningNote );
                    iRefreshingCollection = ETrue;  
                    }
                }
            break;
            }
        case EMcMsgUSBMassStorageEnd:
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgUSBMassStorageEnd");
            
            // Show query dialog to ask if they want to refresh
            //
            MPX_DEBUG1( "CMPXController::DoHandleBroadcastMsg - EMcMsgUSBMassStorageEnd" );

            if( iUIReady )
                {
                CloseWaitNoteL(ETrue);
                if( iIdle->IsActive() )
                    {
                    iIdle->Cancel();
                    }
                TCallBack cb( &IdleCallback, this );
                iIdle->Start( cb );
                }
            else
                {
                iDelayedUsbRefresh = ETrue;
                iCurSystemEvent = KErrNotFound;
                }
            break;
            }
        case EMcMsgUSBMassStorageStart:
            {
            //Sometimes, this message will be received more than 1 time with usb plugin once.
            //Just handle the first one. And only this message has such abnormal behaviour.
            if ( iCurSystemEvent == EMcMsgUSBMassStorageStart )
                {                
                break;
                }
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgUSBMassStorageStart");
            
            // Close playback framework and start wait note
            if( iQueryDialog )
                {
                MPX_DEBUG1("Main View -- Dismissing Query");
                iQueryDialog->DismissQueryL();
                }
            iCurSystemEvent = EMcMsgUSBMassStorageStart;
            iPlaybackUtility->CommandL( EPbCmdStop );
            StartWaitNoteL( EMPXUsbEventNote );
            break;
            }
        case EMcMsgUSBMTPStart:
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgUSBMTPStart");
            // Close playback framework and start wait note
            if( iQueryDialog )
                {
                MPX_DEBUG1("Main View -- Dismissing Query");
                iQueryDialog->DismissQueryL();
                }
            iCurSystemEvent = EMcMsgUSBMTPStart;
            iPlaybackUtility->CommandL( EPbCmdStop );
            StartWaitNoteL( EMPXMTPEventNote );
            break;
            }
        case EMcMsgUSBMTPEnd:
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg EMcMsgUSBMTPEnd");
            iCurSystemEvent = KErrNotFound;
            CloseWaitNoteL(ETrue);
            break;
            }
        case EMcMsgRefreshEnd: // fall through
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgRefreshEnd");
            iCurSystemEvent = KErrNotFound;
            break;
            }
        case EMcMsgSystemEventMax:  
        case EMcMsgRefreshStart:    // fall through
            {
            MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg - EMcMsgSystemEventMax, EMcMsgRefreshStart");
            break;        
            }
        default: 
            {
            break;
            }
        }
    MPX_DEBUG1("CMPXController::DoHandleBroadcastMsg -->");
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXController::HandleErrorL( TInt aError )
    {
    if ( aError )
        {
        MPX_DEBUG2( "CMPXController::HandleErrorL(%d)", aError );
        
        // TextResolver instance for error resolving.
        CTextResolver* textresolver = CTextResolver::NewLC();
        // Resolve the error text
        const TDesC& text =
            textresolver->ResolveErrorString( aError );

        CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote( ETrue );
        dlg->ExecuteLD( text );
        CleanupStack::PopAndDestroy( textresolver );
        }
    }

// ---------------------------------------------------------------------------
// Handle a corrupt message 
// ---------------------------------------------------------------------------
//
void CMPXController::DoHandleCorruptMsgL()
    {
    // Start the corrupt note. The corrupt note will delete 
    // the old databases and restart everything from SCRATCH
    //
    MPX_DEBUG1("CMPXController::DoHandleCorruptMsgL <-- starting scan");
    
    StartWaitNoteL( EMPXCorruptScanningNote );
    iInitDBNeeded = EFalse; 
    iInitDBCorrupted = EFalse;
    }

// End of File
