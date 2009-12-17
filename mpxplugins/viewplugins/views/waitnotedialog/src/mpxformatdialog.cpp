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
* Description:  Wait note dialog for formatting
*
*/


#include <e32base.h>
#include <StringLoader.h>
#include <avkon.rsg>
#include <avkon.mbg>
#include <aknconsts.h>
#include <mpxcollectionmessage.h>
#include <mpxwaitnotedialog.rsg>
#include <mpxcollectionframeworkdefs.h>
#include <mpxlog.h>
#include <mpxcollectionutility.h>
#include <mpxmessagegeneraldefs.h>
#include "mpxformatdialog.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXFormattingWaitDialog::CMPXFormattingWaitDialog( MMPXWaitNoteObserver* aObs )
                                     : CMPXWaitNoteDialog ( aObs, 
                                                           EMPXFormatScanningNote,
                                                           EFalse )
    {
    }


// ---------------------------------------------------------------------------
// 2nd Phased constructor
// ---------------------------------------------------------------------------
//
void CMPXFormattingWaitDialog::ConstructL()
    {
    BaseConstructL();
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXFormattingWaitDialog* CMPXFormattingWaitDialog::NewL( MMPXWaitNoteObserver* aObs )
    {
    CMPXFormattingWaitDialog* self = new(ELeave) CMPXFormattingWaitDialog( aObs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXFormattingWaitDialog::~CMPXFormattingWaitDialog()
    {
    }


// ---------------------------------------------------------------------------
// Setup Wait Notes
// ---------------------------------------------------------------------------
//
void CMPXFormattingWaitDialog::PreNoteDisplayHandleL()
    {
    // CBA
    SetCBAL( R_AVKON_SOFTKEYS_EMPTY );
    
    // Text
    HBufC* text = StringLoader::LoadLC( R_MPX_FORMATTING_TXT );
    SetTextL( *text );
    CleanupStack::PopAndDestroy( text );
    
    // Icon
    TNoteIconInfo icon( (TInt)EMbmAvkonQgn_note_progress, 
                        (TInt)EMbmAvkonQgn_note_progress_mask, 
                        TFileName(KAvkonBitmapFile) );
    SetIconL( icon );
    }


// ---------------------------------------------------------------------------
// Handle Collection Messages
// ---------------------------------------------------------------------------
//
void CMPXFormattingWaitDialog::HandleCollectionMessage( CMPXMessage* aMessage, 
                                                        TInt aError )
    {
    TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage, aError ) );
    }

// ---------------------------------------------------------------------------
// Handle Collection Messages
// ---------------------------------------------------------------------------
//
void CMPXFormattingWaitDialog::DoHandleCollectionMessageL( const CMPXMessage& aMessage, 
                                                           TInt /*aError*/ )
    {
    MPX_FUNC( "CMPXFormattingWaitDialog::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt op( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        
        if( event == TMPXCollectionMessage::EBroadcastEvent && 
            op == EMcMsgFormatEnd )
            {
            MPX_DEBUG1("CMPXFormattingWaitDialog::DoHandleCollectionMessageL -- killing dialog");
            if( iWaitDialog )
                {
                iWaitDialog->ProcessFinishedL();    
                }
            }
        }
    }
            
// End of File
