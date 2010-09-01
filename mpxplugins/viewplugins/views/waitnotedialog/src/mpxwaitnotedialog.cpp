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
* Description:  Wait note dialog 
*
*/


#include <e32base.h>
#include <AknIconUtils.h>
#include <eikimage.h>
#include <eikenv.h>
#include <bautils.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <aknnotewrappers.h>
#include <data_caging_path_literals.hrh>
#include <mpxlog.h>
#include <mpxcollectionutility.h>
#include <aknSDData.h>
#include <AknMediatorFacade.h>
//#include <S32STRM.H>
#include <mplayersecondarydisplayapi.h>

#include <mpxwaitnotedialog.rsg>
#include <mpxuser.h>
#include "mpxwaitnotedialog.h"
#include <mpxlog.h>

// CONSTANTS
_LIT( KMPXWaitNoteRscPath, "mpxwaitnotedialog.rsc" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CMPXWaitNoteDialog::CMPXWaitNoteDialog( MMPXWaitNoteObserver* aObs,
                                        TWaitNoteType aType,
                                        TBool aVisibilityDelayOff  )
                                        : iObserver( aObs ),
                                          iWaitNoteType( aType ),
                                          iVisDelayOff( aVisibilityDelayOff ),
                                          iResourceOffset(0)
    {

    }

// ---------------------------------------------------------------------------
// Base Second Phase Constructor
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::BaseConstructL()
    {
    // Collection Utility
    iCollection = MMPXCollectionUtility::NewL( this );

    // Resource file
    CEikonEnv* eikEnv = CEikonEnv::Static();
    TParse parse;
    parse.Set( KMPXWaitNoteRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( eikEnv->FsSession(), resourceFile );
    iResourceOffset = eikEnv->AddResourceFileL( resourceFile );
    }


// ---------------------------------------------------------------------------
// Virtual destructor
// ---------------------------------------------------------------------------
//
CMPXWaitNoteDialog::~CMPXWaitNoteDialog()
    {
    MPX_DEBUG1("CMPXWaitNoteDialog::~CMPXWaitNoteDialog <---");
    if( iWaitDialog )
        {
        iWaitDialog->SetCallback( NULL );
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        }
    if( iProcessWaitDialog )
        {
        TRAP_IGNORE( iProcessWaitDialog->ProcessFinishedL() );
        }

    if( iResourceOffset )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResourceOffset );
        }

    // Close any queries, if any
    TRAP_IGNORE( DismissQueryNoteL() );

    if( iCollection )
        {
        iCollection->Close();
        }
    MPX_DEBUG1("CMPXWaitNoteDialog::~CMPXWaitNoteDialog --->");
    }

// ---------------------------------------------------------------------------
// Execute the wait note
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::ExecuteLD()
    {
    MPX_DEBUG1("CMPXWaitNoteDialog::ExecuteLD <--");
    // Construct the wait note
    if(iWaitDialog )
        {
        MPX_DEBUG1("CMPXWaitNoteDialog::ExecuteLD -- deleting dlg");
        iWaitDialog->SetCallback( NULL );
        iWaitDialog->ProcessFinishedL();
        iWaitDialog = NULL;
        }

    MPX_DEBUG1("CMPXWaitNoteDialog::ExecuteLD -- new dlg");
    iWaitDialog = new ( ELeave ) CAknWaitDialog(
                                        ( CEikDialog** )&iWaitDialog,
                                        iVisDelayOff );
    TInt dialogIndex = KErrNotFound;
    switch ( iWaitNoteType )
        {
        case EMPXScanningNote:
            {
            dialogIndex = EMPlayerNoteCreatingDb;
            break;
            }

        case EMPXOpeningNote:
            {
            dialogIndex = EMPlayerNoteOpeningDb;
            break;
            }

        case EMPXRefreshingNote:
            {
            dialogIndex = EMPlayerNoteUpdatingDb;
            break;
            }

        case EMPXCorruptScanningNote:
            {
            dialogIndex = EMPlayerNoteRebuildingDb;
            break;
            }

        case EMPXUsbEventNote:
            {
            dialogIndex = EMPlayerUSBConnectionInProgress;
            break;
            }

        case EMPXMTPEventNote:
            {
            dialogIndex = EMPlayerMTPConnectionInProgress;
            break;
            }

        case EMPXFormatScanningNote:
            {
            dialogIndex = EMPlayerFormatting;
            break;
            }

        case EMPXMediaNotAvailableNote:
        case EMPXNoteNotDefined:
        default:
            break;
        };

    if ( dialogIndex != KErrNotFound )
        {
        iWaitDialog->PublishDialogL(
            dialogIndex,
            KMPlayerNoteCategory );

        CAknMediatorFacade* covercl = AknMediatorFacade( iWaitDialog );
        if ( covercl )
            {
            covercl->BufStream().WriteInt32L( 0 );
            covercl->BufStream().CommitL();
            }
        }
    iWaitDialog->SetCallback( this );
    iWaitDialog->PrepareLC( R_MPX_GENERIC_WAIT_NOTE );

    PreNoteDisplayHandleL();

    iWaitDialog->RunLD();
    MPX_DEBUG1("CMPXWaitNoteDialog::ExecuteLD -->");
    }

// ---------------------------------------------------------------------------
// Cancel the wait note
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::CancelNoteL()
    {
    MPX_DEBUG1("CMPXWaitNoteDialog::CancelNoteL <---");
    if( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        }
    }

// ---------------------------------------------------------------------------
// Check the destruction status of wait dialog
// ---------------------------------------------------------------------------
//
TBool CMPXWaitNoteDialog::ReadyToDestroy()
    {
    // iWaitDialog will be updated by CAknWaitDialog self-destruction mech
    return iWaitDialog == NULL;
    }

// ---------------------------------------------------------------------------
// Do anything after the wait note is dismissed
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::PostNoteHandleL( TInt /*aButtonId*/ )
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// Set the wait note CBA buttons
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::SetCBAL( TInt aCBAId )
    {
    if( iWaitDialog )
        {
        MEikButtonGroup* buttonGroup =
        iWaitDialog->ButtonGroupContainer().ButtonGroup();
        buttonGroup->SetCommandSetL( aCBAId );
        buttonGroup->AsControl()->DrawDeferred();
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// Set the wait note CBA text
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::SetTextL( const TDesC& aText )
    {
    if( iWaitDialog )
        {
        iWaitDialog->SetTextL( aText );
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// Set the wait note Icon
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::SetIconL( TNoteIconInfo& aIconInfo )
    {
    if( iWaitDialog )
        {
        MAknsSkinInstance* skin( AknsUtils::SkinInstance() );

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;

        AknsUtils::CreateIconLC(
            skin,
            KAknsIIDNone,
            bitmap,
            mask,
            aIconInfo.bmpfile,
            aIconInfo.bitmapId,
            aIconInfo.maskId );

        CEikImage* icon = new ( ELeave ) CEikImage();
        CleanupStack::PushL( icon );
        icon->SetPicture( bitmap, mask );
        icon->SetPictureOwnedExternally( EFalse );
        iWaitDialog->SetImageL( icon );
        CleanupStack::Pop( 3 ); // icon, bitmap, mask
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// ---------------------------------------------------------------------------
// MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::HandleCollectionMediaL(const CMPXMedia& /*aMedia*/,
                                                TInt /*aError*/)
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::HandleCollectionMessage(
    CMPXMessage* /*aMessage*/, TInt /*aError*/ )
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::HandleOpenL(const CMPXMedia& /*aEntries*/,
                                     TInt /*aIndex*/,TBool /*aComplete*/,TInt /*aError*/)
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::HandleOpenL(const CMPXCollectionPlaylist& /*aPlaylist*/,
                                     TInt /*aError*/)
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// MProgressDialogCallback
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::DialogDismissedL( TInt aButtonId )
    {
    // Tell inherited class abut the event
    PostNoteHandleL( aButtonId );

    // Tell view that wait dialog is finished
    iObserver->HandleWaitNoteDismissedL();
    }

// ---------------------------------------------------------------------------
// Display a confirmation note
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::DisplayConfirmationNoteL( const TDesC& aLabel )
    {
    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote();
    note->ExecuteLD( aLabel );
    }

// ---------------------------------------------------------------------------
// Display a information note
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::DisplayInformationNoteL( const TDesC& aLabel )
    {
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote();
    note->ExecuteLD( aLabel );
    }

// ---------------------------------------------------------------------------
// Display a query note
// ---------------------------------------------------------------------------
//
TInt CMPXWaitNoteDialog::DisplayQueryNoteL( const TDesC& aQuery )
    {
    CAknQueryDialog* query = new( ELeave ) CAknQueryDialog();
    iQueryDialog = query;

    TInt rtn = query->ExecuteLD( R_MPX_QUERY_YES_NO ,
                                 aQuery );
    iQueryDialog = NULL;  // Dialog destroyed
    return rtn;
    }

// ---------------------------------------------------------------------------
// Dismiss the current query note
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::DismissQueryNoteL()
    {
    if( iQueryDialog )
        {
        iQueryDialog->DismissQueryL();
        }
    }

// ---------------------------------------------------------------------------
// Display a generic note dialog
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::DisplayNoteDialogL( TInt aDlgRsc, const TDesC& aLabel,
                                             CAknNoteDialog::TTone aTone )
    {
    iObserver->NoteDialogL( iNoteDialog, aDlgRsc, aTone );
    if ( iNoteDialog )
        {
        iNoteDialog->SetTextL( aLabel );
        }
    }

// ---------------------------------------------------------------------------
// Display a generic note dialog
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::DisplayNoteDialogL( TInt aDlgRsc, TInt aTextRsc,
                                             TInt aItemsAdded, const TDesC& aLabel,
                                             CAknNoteDialog::TTone aTone )
    {
    MPX_DEBUG1("CMPXWaitNoteDialog::DisplayNoteDialogL for COVER DISPLAY--->");

    CAknNoteDialog* note = new ( ELeave ) CAknNoteDialog( aTone );
    note->PrepareLC( aDlgRsc );
    note->SetTextL( aLabel );
    if ( aTextRsc == R_MPX_REFRESHING_DB_STOPPED_TXT )
        {
        note->PublishDialogL(
            EMPlayerNoteDbUpdateStopped,
            KMPlayerNoteCategory);
        }
    else if ( aTextRsc == R_MPX_SCANNING_STOPPED_TXT )
        {
        note->PublishDialogL(
            EMPlayerNoteDbSearchStoppedItemFound,
            KMPlayerNoteCategory);
        }
    else if ( aTextRsc == R_MPX_SCANNING_COMPLETE_TXT )
        {
        TInt noteId( EMPlayerNoteDbCreationCompleteEmpty );
        if ( aItemsAdded > 0 )
            {
            noteId = EMPlayerNoteDbCreationComplete;
            };

        note->PublishDialogL(
            noteId,
            KMPlayerNoteCategory);
        }
    else if ( aTextRsc == R_MPX_REFRESHING_DB_COMPLETE_TXT )
        {
        note->PublishDialogL(
            EMPlayerNoteDbUpdateComplete,
            KMPlayerNoteCategory);
        }

    if ( aTextRsc != KErrNotFound )
        {
        CAknMediatorFacade* covercl = AknMediatorFacade( note );
        if ( covercl )
            {
            covercl->BufStream().WriteInt32L( aItemsAdded );
            covercl->BufStream().CommitL();
            }
        }

    note->RunLD();
    MPX_DEBUG1("<--CMPXWaitNoteDialog::DisplayNoteDialogL for COVER DISPLAY End");
    }

// ---------------------------------------------------------------------------
// Display a generic process wait dialog
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::DisplayProcessWaitDialogL( TInt aDlgRsc, 
                                             const TDesC& aLabel,
                                             CAknWaitDialog::TTone aTone )
    {
    if(iProcessWaitDialog )
        {        
        iProcessWaitDialog->ProcessFinishedL();
        iProcessWaitDialog = NULL;
        }

    iProcessWaitDialog = new ( ELeave ) CAknWaitDialog(
                                    ( CEikDialog** )&iProcessWaitDialog,
                                    EFalse );
    
    iProcessWaitDialog->PrepareLC( aDlgRsc );
    iProcessWaitDialog->SetTextL( aLabel );
    iProcessWaitDialog->RunLD();
    }

// ---------------------------------------------------------------------------
// Cancel the process wait dialog
// ---------------------------------------------------------------------------
//
void CMPXWaitNoteDialog::CancelProcessWaitDialogL()
    {
    if( iProcessWaitDialog )
        {
        iProcessWaitDialog->ProcessFinishedL();
        }
    }
// End of file

