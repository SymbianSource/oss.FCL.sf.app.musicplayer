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
* Description:  Music Player album art editor dialog implementation
*
*/


// INCLUDE FILES
#include <StringLoader.h>
#include <MGFetch.h>
#include <aknnotewrappers.h>

#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include "mplayersecondarydisplayapi.h"

#include <mpxmedia.h>
#include <mpxattribute.h>
#include <mpxalbumarteditordialog.rsg>
#include <mpxdrmmediautility.h>
#include <mpxmediadrmdefs.h>
#include <mpxlog.h>

#include "mpxalbumarteditordialogimagethumbnailutil.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogImageThumbnailUtil::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogImageThumbnailUtil*
    CMPXAlbumArtEditorDialogImageThumbnailUtil::NewL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogImageThumbnailUtil::NewL" );
    CMPXAlbumArtEditorDialogImageThumbnailUtil* self = new ( ELeave )
        CMPXAlbumArtEditorDialogImageThumbnailUtil();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogImageThumbnailUtil::~CMPXAlbumArtEditorDialogImageThumbnailUtil
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogImageThumbnailUtil::
    ~CMPXAlbumArtEditorDialogImageThumbnailUtil()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogImageThumbnailUtil::~CMPXAlbumArtEditorDialogImageThumbnailUtil" );
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogImageThumbnailUtil::FetchThumbnailL
// Fetch thumbnail from the device
// ----------------------------------------------------------------------------
//
TBool CMPXAlbumArtEditorDialogImageThumbnailUtil::FetchThumbnailL(
    CDesCArray& aFilesArray )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogImageThumbnailUtil::FetchThumbnailL" );
    // fetch thumbnail from media gallery
    HBufC* title = StringLoader::LoadLC( R_MPX_CUI_ALBUM_ART_DLG_TITLE );

    TBool res = MGFetch::RunL( aFilesArray, EImageFile, EFalse,
            KNullDesC, *title, this );

    CleanupStack::PopAndDestroy( title );
    return res;
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogImageThumbnailUtil::CMPXAlbumArtEditorDialogImageThumbnailUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogImageThumbnailUtil::
    CMPXAlbumArtEditorDialogImageThumbnailUtil()
    {
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogImageThumbnailUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialogImageThumbnailUtil::ConstructL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogImageThumbnailUtil::ConstructL" );
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialogImageThumbnailUtil::VerifySelectionL
// Verify thumbnail selection
// ----------------------------------------------------------------------------
//
TBool CMPXAlbumArtEditorDialogImageThumbnailUtil::VerifySelectionL(
    const MDesCArray* aSelectedFiles )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialogImageThumbnailUtil::VerifySelectionL" );
    TBool result = ETrue;

    if ( aSelectedFiles && aSelectedFiles->MdcaCount() > 0 )
        {
        // DRM for phonebook thumbnail fetch
        TPtrC fileName = aSelectedFiles->MdcaPoint( 0 );
        CMPXDrmMediaUtility* drmUtil = CMPXDrmMediaUtility::NewLC();
        drmUtil->InitL( fileName );

        const CMPXMedia* media = drmUtil->GetMediaL( EMPXMediaDrmProtected );
        if ( media )
            {
            if ( media->IsSupported( KMPXMediaDrmProtected ) )
                {
                TBool isProtected( media->ValueTObjectL<TBool>(
                    KMPXMediaDrmProtected ) );
                if ( isProtected )
                    {
                    // show user copyright note
                    HBufC* prompt = StringLoader::LoadLC( R_MPX_QTN_MP_DRM_NOT_ALLOWED );
                    CAknInformationNote* dlg = new( ELeave ) CAknInformationNote( ETrue );
                    CleanupStack::PushL(dlg);
                    dlg->PublishDialogL( EMPlayerNoteDrmNotAllowed, KMPlayerNoteCategory );
                    CleanupStack::Pop(dlg);
                    dlg->ExecuteLD( *prompt );
                    CleanupStack::PopAndDestroy( prompt );
                    result = EFalse;
                    }
                }
            }
        CleanupStack::PopAndDestroy( drmUtil );
        }
    return result;
    }

//  End of File
