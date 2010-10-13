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
* Description:  rename dialog for Common UI
*
*/


// INCLUDE FILES
#include <coeutils.h>
#include <eikapp.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcommonui.rsg>
#include <mpxlog.h>

#include "mpxcommonuihelper.h"
#include "mpxfilenamequerydlg.h"

// CONSTANTS
const TInt KMPXMaxPlaylistPathLen ( 200 );

_LIT( KBackslash, "\\" );
_LIT( KDirectionalChars, "\x202A\x202B\x202C\x202D\x200E\x200F" );

const TUint16 KIllegalChars[] = {
    '<', '>', '"', '/', '\\', '|', ':', '*', '?',
    0xFF02, // Full width quote
    0xFF0A, // Full width asterisk
    0xFF0F, // Full width slash
    0xFF1A, // Full width colon
    0xFF1C, // Full width left arrow
    0xFF1E, // Full width right arrow
    0xFF1F, // Full width question mark
    0xFF3C, // Full width back slash
    0xFF5C, // Full width pipe
    0x201C, // Left quote
    0x201D, // Right quote
    0x201F, // Reversed quote
    0, // Array terminator
};


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXFileNameQueryDlg* CMPXFileNameQueryDlg::NewL(
        const TDesC& aOldName,
        TDes& aNewName,
        const TDesC& aPath,
        TMPXGeneralCategory aCategory,
        CMPXCommonUiHelper* aParent )
    {
    MPX_FUNC( "CMPXFileNameQueryDlg::NewL" );
    CMPXFileNameQueryDlg* self =
        new( ELeave ) CMPXFileNameQueryDlg( aNewName, aCategory, aParent );

    CleanupStack::PushL( self );
    self->ConstructL( aOldName, aPath );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXFileNameQueryDlg::CMPXFileNameQueryDlg(
    TDes& aNewName,
    TMPXGeneralCategory aCategory,
    CMPXCommonUiHelper* aParent ) :
    CAknTextQueryDialog( aNewName ),
    iParent( aParent ),
    iCategory( aCategory )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXFileNameQueryDlg::ConstructL(
    const TDesC& aOldName, const TDesC& aPath )
    {
    MPX_FUNC( "CMPXFileNameQueryDlg::ConstructL" );
    TParsePtrC name( aOldName );
    iOldName = aOldName.AllocL();
    iPath = aPath.AllocL();
    // Strip any directionality markers to get pure name
    TPtr ptr( iOldName->Des() );
    AknTextUtils::StripCharacters( ptr, KDirectionalChars );
    TInt extLength( 0 );
    if ( iCategory == EMPXPlaylist )
        {
        Text().Copy( name.NameAndExt() );
        }
    else
        {
        Text().Copy( name.Name() );
        if ( name.ExtPresent() )
            {
            extLength = name.Ext().Length();
            }
        }
    TInt maxLength( KMPXMaxPlaylistPathLen - aPath.Length() - extLength );
    MPX_DEBUG2( "CMPXFileNameQueryDlg::ConstructL setting max length = %d", maxLength );
    SetMaxLength( maxLength );
    iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXFileNameQueryDlg::~CMPXFileNameQueryDlg()
    {
    if ( iCollectionUiHelper )
        {
        iCollectionUiHelper->Close();
        }
    delete iOldName;
    delete iPath;
    }

// ---------------------------------------------------------------------------
// CFileManagerFileNameQueryDlg::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CMPXFileNameQueryDlg::OkToExitL( TInt aButtonId )
    {
    MPX_FUNC( "CMPXFileNameQueryDlg::OkToExitL" );
    TBool result( CAknTextQueryDialog::OkToExitL( aButtonId ) );

    HBufC* userText = Text().AllocLC();
    TPtr ptrUserText( userText->Des() );

    // Strip any directionality markers to get pure name
    AknTextUtils::StripCharacters( ptrUserText, KDirectionalChars );

    TParsePtrC currentPath( *iPath );
    // Check file name
    TBool isValidName = IsValidName( currentPath.DriveAndPath(), *userText, EFalse );
    if( !isValidName )
        {
        if( IllegalChars( *userText ) )
            {
            iParent->DisplayInfoNoteL(
                R_MPX_QTN_FLDR_ILLEGAL_CHARACTERS );
            }
        else
            {
            iParent->DisplayInfoNoteL(
                R_MPX_QTN_FLDR_BAD_FILE_NAME );
            }
        CAknQueryControl* queryControl = QueryControl();
        if ( queryControl )
            {
            CEikEdwin* edwin = static_cast< CEikEdwin* >(
                queryControl->ControlByLayoutOrNull( EDataLayout ) );
            if (edwin)
                {
                edwin->SetSelectionL( edwin->TextLength(), 0 );
                }
            }
        CleanupStack::PopAndDestroy( userText );
        return EFalse;
        }

    // if some other entry found with same name
    // not ok except if name same as original
    if ( iCategory != EMPXPlaylist )
        {
        if ( iCollectionUiHelper->TitleExistsL( iCategory, *userText ) )
            {
            TBool overWrite( EFalse );
            HBufC* text = StringLoader::LoadLC( R_MPX_QTN_FLDR_OVERWRITE_QUERY, Text() );
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            overWrite = dlg->ExecuteLD( R_MPX_CONFIRM_QUERY_WITH_YES_NO, *text );
            CleanupStack::PopAndDestroy( text );
            if( !overWrite )
                {
                CAknQueryControl* queryControl = QueryControl();
                if (queryControl)
                    {
                    CEikEdwin* edwin = static_cast< CEikEdwin* >(
                        queryControl->ControlByLayoutOrNull( EDataLayout ) );

                    HBufC* newTitle = iParent->GenerateTitleL( iCategory, *userText );
                    CleanupStack::PushL(newTitle);

                    if ( edwin )
                        {
                        edwin->SetTextL( newTitle );
                        edwin->SetSelectionL( edwin->TextLength(), 0 );
                        }
                    CleanupStack::PopAndDestroy(newTitle);
                    }
                result = EFalse;
                }
            }
        }
    CleanupStack::PopAndDestroy( userText );
    if ( result && iCategory != EMPXPlaylist )
        {
        // playlist has no extension, don't need to do this
        TParsePtrC oldName( *iOldName );
        iDataText.Append( oldName.Ext() );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CMPXFileNameQueryDlg::OfferKeyEventL
// 
// -----------------------------------------------------------------------------
// 
TKeyResponse CMPXFileNameQueryDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType ) 
    { 
    TKeyResponse response = EKeyWasNotConsumed;
    // consume 'enter' so it won't acknowledge the dialog
    TBool vkbOpen = EFalse;
    CAknQueryControl* queryControl = QueryControl();
    if ( queryControl )
        {
        CEikEdwin* edwin = static_cast< CEikEdwin* >(
            queryControl->ControlByLayoutOrNull( EDataLayout ) );
        if ( edwin )
            {            
            if ( edwin->AknEdwinFlags() & EAknEditorFlagTouchInputModeOpened )
                {
                // virtual keyboard is active.                
                vkbOpen = ETrue;
                }
            }
        }    
    
    if ( vkbOpen && aType == EEventKey && aKeyEvent.iCode == EKeyEnter ) 
        { 
        response = EKeyWasConsumed;
        } 
    else
        {
        response = CAknTextQueryDialog::OfferKeyEventL(aKeyEvent, aType); 
        }
    return response;
    }

// -----------------------------------------------------------------------------
// CMPXFileNameQueryDlg::IsValidName
// -----------------------------------------------------------------------------
//
TBool CMPXFileNameQueryDlg::IsValidName(
        const TDesC& aDriveAndPath, const TDesC& aName, TBool aIsFolder ) const
    {
    MPX_FUNC( "CMPXFileNameQueryDlg::IsValidName" );
    // Check name for bad chars
    const TUint16 KMinAllowedChar = 0x0020;
    const TUint16 KParagraphSeparator = 0x2029;
    const TUint16 KDot = '.';
    TInt nameLen( aName.Length() );
    if ( !nameLen )
        {
        return EFalse;
        }
    for ( TInt i( 0 ); i < nameLen; i++ )
        {
        TUint16 ch( aName[ i ] );
        if ( ch < KMinAllowedChar || ch == KParagraphSeparator )
            {
            return EFalse;
            }
        }
    // File system ignores totally dot in the end of name, so
    // we set here as not valid name, so that user gets correctly informed
    if ( aName[ nameLen - 1 ] == KDot || IllegalChars( aName ) )
        {
        return EFalse;
        }
    // Get full path length
    TPtrC pathPtr( aDriveAndPath );
    TInt pathLen( pathPtr.Length() );
    if ( !pathLen )
        {
        return EFalse;
        }
    TInt fullPathLen( pathLen );
    if ( pathPtr[ pathLen - 1 ] != KBackslash()[ 0 ] )
        {
        ++fullPathLen; // Add backslash before name
        }
    fullPathLen += nameLen;
    if ( aIsFolder )
        {
        ++fullPathLen; // Add folder final backslash
        }

    TBool ret( EFalse );
    if ( fullPathLen <= KMaxFileName )
        {
        // Check full path
        HBufC* fullPath = HBufC::New( KMaxFileName );
        if ( fullPath )
            {
            TPtr ptr( fullPath->Des() );
            ptr.Copy( pathPtr );
            if ( pathPtr[ pathLen - 1 ] != KBackslash()[ 0 ] )
                {
                ptr.Append( KBackslash ); // Add backslash before name
                }
            ptr.Append( aName );
            RFs fs;
            TInt connectError = fs.Connect();
            if ( connectError == KErrNone )
            {
                ret = fs.IsValidName( ptr );
                fs.Close();
            }
            delete fullPath;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMPXFileNameQueryDlg::IllegalChars
// -----------------------------------------------------------------------------
//
TBool CMPXFileNameQueryDlg::IllegalChars( const TDesC& aName ) const
    {
    MPX_FUNC( "CMPXFileNameQueryDlg::IllegalChars" );
    for ( TInt i( 0 ); KIllegalChars[ i ]; i++ )
        {
        if ( aName.Locate( KIllegalChars[ i ] ) != KErrNotFound )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

//  End of File
