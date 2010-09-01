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
* Description:  Document class required by AVKON application framework.
*
*/


// INCLUDE FILES
#include <AiwGenericParam.h>
#include <mpxviewutility.h>

#include "mpxdocument.h"
#include "mpxappui.h"
#include "mpxtlshelper.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CMPXDocument::CMPXDocument( CEikApplication& aApp )
    : CAiwGenericParamConsumer( aApp )
    {
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXDocument::ConstructL()
    {
    // Initialize TLS
    MPXTlsHelper::InitializeL();

    // Get view utility. View utility must be created before AppUi.
    iViewUtility = MMPXViewUtility::UtilityL();
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXDocument* CMPXDocument::NewL(
    CEikApplication& aApp )
    {
    CMPXDocument* self = new ( ELeave ) CMPXDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXDocument::~CMPXDocument()
    {
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }

    // Release resources in TLS
    MPXTlsHelper::Uninitialize();
    }


// ---------------------------------------------------------------------------
// Parse input paramaters from AIW interface.
// ---------------------------------------------------------------------------
//
TBool CMPXDocument::ParseInputParameters()
    {
    TBool openFile( ETrue );
    TPtrC buf;
    TPtrC name;

    const CAiwGenericParamList* params = GetInputParameters();
    if ( params )
        {
        TInt index( 0 );
        const TAiwGenericParam* param = params->FindFirst(
            index,
            EGenericParamPlayerInitMode,
            EVariantTypeTInt32 );

        if ( !param )
            {
            index = 0;
            param = params->FindFirst(
                index,
                EGenericParamUnspecified,
                EVariantTypeTInt32 );
            }

        if ( index >= 0 && param )
            {
            TInt32 value( 0 );
            param->Value().Get( value );
            TRAP_IGNORE( MPXTlsHelper::SetLaunchModeL(
                static_cast<TMPXLaunchMode>( value ) ) );
            }

        // Get album contents, if available
        index = 0;
        param = params->FindFirst(
            index,
            EGenericParamAlbumItems,
            EVariantTypeDesC );

        if ( index >= 0 && param )
            {
            buf.Set( param->Value().AsDes() );
            }

        // Get album name, if available
        index = 0;
        param = params->FindFirst(
            index,
            EGenericParamAlbumName,
            EVariantTypeDesC );

        if ( index >= 0 && param )
            {
            name.Set( param->Value().AsDes() );
            }

        // Get host application UID
        index = 0;
        param = params->FindFirst(
            index,
            EGenericParamApplication,
            EVariantTypeTUid );

        if ( index >= 0 && param )
            {
            TUid uid( KNullUid );
            param->Value().Get( uid );
            TRAP_IGNORE( MPXTlsHelper::SetHostUidL( uid ) );
            }

        // Check allow save flag
        index = 0;
        param = params->FindFirst(
            index,
            EGenericParamAllowSave );

        if ( index >= 0 && param )
            {
            TInt32 value( 0 );
            param->Value().Get( value );
            MPXTlsHelper::SetNeedSave( value );
            }

        // Check allow move flag
        index = 0;
        param = params->FindFirst(
            index,
            EGenericParamAllowMove,
            EVariantTypeTInt32 );

        if ( index >= 0 && param )
            {
            TInt32 value( 0 );
            param->Value().Get( value );
            MPXTlsHelper::SetAllowMove( value );
            }
        }

    if ( buf.Length() > 0 )
        {
        // TODO: Support for album items
        openFile = EFalse;
        }

    return openFile;
    }


// ---------------------------------------------------------------------------
// From class CEikDocument.
// Constructs CMPXAppUi
// ---------------------------------------------------------------------------
//
CEikAppUi* CMPXDocument::CreateAppUiL()
    {
    return new ( ELeave ) CMPXAppUi;
    }


// ---------------------------------------------------------------------------
// From class CAknDocument.
// Called by the framework to open a file.
// ---------------------------------------------------------------------------
//
CFileStore* CMPXDocument::OpenFileL(
    TBool aDoOpen,
    const TDesC& aFilename,
    RFs& aFs )
    {
    if ( aDoOpen )
        {
        if ( ParseInputParameters() )
            {
            iAppUi->OpenFileL( aFilename );
            }
        }

    return CAknDocument::OpenFileL( aDoOpen, aFilename, aFs );
    }


// ---------------------------------------------------------------------------
// From class CAknDocument.
// Called by the framework to open a file.
// ---------------------------------------------------------------------------
//
void CMPXDocument::OpenFileL(
    CFileStore*& aFileStore,
    RFile& aFile )
    {
    if ( ParseInputParameters() )
        {
        const CAiwGenericParamList* params = GetInputParameters();
        static_cast<CMPXAppUi*>(iAppUi)->OpenFileL( aFileStore, aFile, params );
        }

    return CAknDocument::OpenFileL( aFileStore, aFile );
    }


// End of File
