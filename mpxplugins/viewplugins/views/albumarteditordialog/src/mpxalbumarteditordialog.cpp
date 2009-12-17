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
#include <akntitle.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <aknViewAppUi.h>
#include <aknnavilabel.h>
#include <coeutils.h>
#include <hlplch.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

#include <mpxcollectionmessage.h>
#include <mpxcollectionutility.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcollectionpath.h>
#include <mpxalbumarteditordialog.rsg>
#include <mpxmessagegeneraldefs.h>
#include <mpxconstants.h>
#include <mpxlog.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxuser.h>

#include "mpxalbumartutil.h"
#include "mpxcommonuihelper.h"
#include "mpxalbumarteditordialog.hlp.hrh"
#include "mpxalbumarteditordialog.hrh"
#include "mpxalbumarteditordialogcustomcontrol.h"
#include "mpxalbumarteditorcustomcontrolcontainer.h"
#include "mpxalbumarteditordialogimagethumbnailutil.h"
#include "mpxalbumarteditordialog.h"
#include "mpxviewprivatepskeys.h"

// CONSTANTS
const TInt KMPXAlbumArtBorder = 24;

_LIT( KMPXAlubmArtEditorRscPath, "mpxalbumarteditordialog.rsc" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXAlbumArtEditorDialog* CMPXAlbumArtEditorDialog::NewL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::NewL" );
    CMPXAlbumArtEditorDialog* self =
        new ( ELeave ) CMPXAlbumArtEditorDialog();
    CleanupStack::PushL( self );
    self->ConstructL( R_MPX_CUI_ALBUM_ART_DIALOG_MENUBAR );
    CleanupStack::Pop( self );  // this, it will be PushL-d by executeLD...
    return self;
    }

// Destructor
EXPORT_C CMPXAlbumArtEditorDialog::~CMPXAlbumArtEditorDialog()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::~CMPXAlbumArtEditorDialog" );
    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    delete iCommonUiHelper;
    delete iOrigTitle;
    delete iNaviLabelPane;
    delete iMPXUtility;
    delete iParam;
    delete iMedia;
    delete iAlbumInfo;
    delete iMediaArray;
    delete iNewFileName;
    delete iAsyncCallNext;
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXAlbumArtEditorDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                         TEventCode aType)
	{
	if( iFetchingPicture )
		{
        // fix for ou1cimx1#176793
	    if ( aKeyEvent.iScanCode == EStdKeyEnter || aKeyEvent.iCode == EKeyEnter )
	        {
	        return EKeyWasConsumed;
	        }
		// When the fetcher is open ignore all key presses.
		// Needed to avoid closing of the dialog before the fetcher is closed.
		return EKeyWasNotConsumed;
        }

    //response enter key as select key
    if( aKeyEvent.iScanCode == EStdKeyEnter || aKeyEvent.iScanCode == EKeyEnter )
        {
        if( aType == EEventKeyUp )
            {
            DoProcessCommandL( EMPXCmdAlbumArtEditorDlgChange );
            }

        return EKeyWasConsumed;
        }

    return CAknDialog::OfferKeyEventL(aKeyEvent,aType);
	}

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::CMPXAlbumArtEditorDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialog::CMPXAlbumArtEditorDialog() :
    iNewFileName( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::SetParamL
// parameter passed in from viewframework
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXAlbumArtEditorDialog::SetParamL( const TDesC* aParam )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::SetParamL" );
    delete iParam;
    iParam = NULL;
    iParam = aParam->AllocL();
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::ConstructL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::ConstructL( TInt aResource )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::ConstructL" );
    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXAlubmArtEditorRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iMPXUtility = CMPXAlbumArtUtil::NewL();

    iCommonUiHelper = CMPXCommonUiHelper::NewL();

    CAknDialog::ConstructL( aResource );

    iAsyncCallNext = new ( ELeave ) CAsyncCallBack(
        TCallBack( CMPXAlbumArtEditorDialog::ExtractNextAlbumArtCallBackL, this ),
        CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection message.
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleCollectionMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d a folder
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleOpenL(
        const CMPXMedia& /*aEntries*/,
        TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the item being opened. Typically called
// when client has Open()'d an item. Client typically responds by
// 'playing' the item via the playlist
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleOpenL(
        const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection media.
// Notes: The client is responsible for delete the object of aMedia.
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::HandleCollectionMediaL" );
    if ( KErrNone == aError )
        {
        switch ( iCurrentMediaLOp )
            {
            case EMPXOpMediaLGetCurrentMode:
                {
                TMPXGeneralType type( aMedia.ValueTObjectL<TMPXGeneralType>(
                    KMPXMediaGeneralType ) );
                TMPXGeneralCategory category(
                    aMedia.ValueTObjectL<TMPXGeneralCategory>(
                        KMPXMediaGeneralCategory ) );

                if ( type != EMPXItem || category != EMPXSong )
                    {
                    delete iMedia;
                    iMedia = NULL;
                    iMedia = CMPXMedia::NewL();
                    TMPXItemId albumId( aMedia.ValueTObjectL<TMPXItemId>(
                        KMPXMediaGeneralId ) );
                    iMedia->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralId,
                        albumId );
                    // not on tracks level, get container ID
                    iCurrentMediaLOp = EMPXOpMediaLGetContainerId;
                    CMPXCollectionPath* path( iCollectionUtility->Collection().PathL() );
                    CleanupStack::PushL( path );
                    path->Back();
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL( attrs );
                    // get attributes as if it's tracks level so that we don't need to
                    // make another async call if that's the case.  if it's album level, those
                    // attributes are ignored
                    attrs.Append(
                        TMPXAttribute( KMPXMediaIdGeneral,
                        EMPXMediaGeneralId | EMPXMediaGeneralCategory ) );
                    iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
                    CleanupStack::PopAndDestroy( &attrs );
                    CleanupStack::PopAndDestroy( path );
                    }
                else
                    {
                    delete iMedia;
                    iMedia = NULL;
                    iMedia = CMPXMedia::NewL( aMedia );
                    delete iAlbumInfo;
                    iAlbumInfo = NULL;
                    iAlbumInfo = CMPXMedia::NewL( aMedia );
                    const TDesC& albumArtFileName =
                        aMedia.ValueText( KMPXMediaMusicAlbumArtFileName );
                    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleCollectionMediaL album art file name = %S", &albumArtFileName );
                    const TDesC& origAlbumArtFileName =
                        aMedia.ValueText( KMPXMediaMusicOriginalAlbumArtFileName );
                    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleCollectionMediaL original album art file name = %S", &origAlbumArtFileName );
                    if ( albumArtFileName.Length() > 0 )
                        {
                        if ( albumArtFileName.Compare( origAlbumArtFileName ) == 0 )
                            {
                            iAlbumArtChanged = EFalse;
                            }
                        else
                            {
                            iAlbumArtChanged = ETrue;
                            }
                        }
                    ExtractAlbumArtL( aMedia );
                    iCurrentMediaLOp = EMPXOpMediaLIdle;
                    }
                // Navipane title should be the title of the artist/album/genre/composer or song/item
                //
                UpdateNaviPaneL(aMedia.ValueText(KMPXMediaGeneralTitle ));
                break;
                }
            case EMPXOpMediaLGetContainerId:
                {
                delete iAlbumInfo;
                iAlbumInfo = NULL;
                iAlbumInfo = CMPXMedia::NewL();
                TMPXGeneralCategory category(
                    aMedia.ValueTObjectL<TMPXGeneralCategory>(
                        KMPXMediaGeneralCategory ) );
                TMPXItemId albumId(
                    iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                iAlbumInfo->SetTObjectValueL<TMPXItemId>(
                    KMPXMediaGeneralId, albumId );
                MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleCollectionMediaL album id = 0x%x", albumId.iId1 );
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL( attrs );
                AppendCommonAttributes( attrs );
                CMPXMedia* criteria = CMPXMedia::NewL();
                CleanupStack::PushL( criteria );
                criteria->SetTObjectValueL<TMPXGeneralType>(
                    KMPXMediaGeneralType, EMPXGroup );
                criteria->SetTObjectValueL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory, EMPXSong );
                criteria->SetTObjectValueL<TMPXItemId>(
                    KMPXMediaGeneralId, albumId );
                if ( category == EMPXAlbum )
                    {
                    // in albums view
                    iAlbumArtMode = EMPXModeAlbumView;
                    }
                else
                    {
                    // in artist/albums view
                    iAlbumArtMode = EMPXModeArtistAlbumView;
                    TMPXItemId containerId( aMedia.ValueTObjectL<TMPXItemId>(
                        KMPXMediaGeneralId ) );
                    iAlbumInfo->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralContainerId, containerId );
                    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleCollectionMediaL container id = 0x%x", containerId.iId1 );
                    criteria->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralContainerId, containerId );
                    }
                iCurrentFindAllLOp = EMPXOpFindAllLGetAlbumArtInfo;
                iCollectionUtility->Collection().FindAllL( *criteria, attrs.Array(), *this );
                CleanupStack::PopAndDestroy( criteria );
                CleanupStack::PopAndDestroy( &attrs );
                iCurrentMediaLOp = EMPXOpMediaLIdle;
                break;
                }
            case EMPXOpMediaLGetAlbumArtInfo:
                {
                delete iMedia;
                iMedia = NULL;
                iMedia = CMPXMedia::NewL( aMedia );
                const TDesC& albumArtFileName =
                    aMedia.ValueText( KMPXMediaMusicAlbumArtFileName );
                if ( albumArtFileName.Length() > 0 )
                    {
                    if ( albumArtFileName.Compare(
                        aMedia.ValueText( KMPXMediaMusicOriginalAlbumArtFileName ) ) == 0 )
                        {
                        iAlbumArtChanged = EFalse;
                        }
                    else
                        {
                        iAlbumArtChanged = ETrue;
                        }
                    }
                ExtractAlbumArtL( aMedia );
                iCurrentMediaLOp = EMPXOpMediaLIdle;
                break;
                }
            default:
                {
                iCurrentMediaLOp = EMPXOpMediaLIdle;
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::HandleCommandComplete
// Handle completion of async commands
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleCommandComplete(CMPXCommand* /*aCommandResult*/,
                                                     TInt aError )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::HandleCommandComplete" );
    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleCommandComplete aError = %d", aError );
    MPX_DEBUG1( "CMPXAlbumArtEditorDialog::HandleCommandComplete dismiss wait note" );
    if( !iInvalidFileExist )
        {
        TRAP_IGNORE( DoHandleOperationCompleteL( aError ) );
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::GetCurrentModeL
// Sends request to determine if it's album level or track level
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::GetCurrentModeL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::GetCurrentModeL" );
    iCurrentMediaLOp = EMPXOpMediaLGetCurrentMode;
    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( path );
    if ( iParam )
        {
        TLex indexLex( iParam->Des() );
        TInt lexToInt = NULL;
        if ( indexLex.Val( lexToInt ) == KErrNone )
            {
            path->Set( lexToInt );
            }
        }
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    // get attributes as if it's tracks level so that we don't need to
    // make another async call if that's the case.  if it's album level, those
    // attributes are ignored
    AppendCommonAttributes( attrs );
    attrs.Append( KMPXMediaGeneralTitle );
    iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( path );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::BackupPreviousStatusPaneL
// Backs up Status Pane information.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::BackupPreviousStatusPaneL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::BackupPreviousStatusPaneL" );
    CEikStatusPane* sp = iAvkonAppUi->StatusPane();
    // Create and Push navi lable
    iNaviPane = static_cast< CAknNavigationControlContainer* >
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iNaviLabelPane = iNaviPane->CreateNavigationLabelL();
    iNaviPane->PushL( *iNaviLabelPane );
    // Backup title pane
    iTitlePane = static_cast< CAknTitlePane* >
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    iOrigTitle = iTitlePane->Text()->AllocL();
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::RestorePreviousStatusPaneL()
// Restores previous status pane.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::RestorePreviousStatusPaneL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::RestorePreviousStatusPaneL" );
    if ( iOrigTitle )
        {
        // Set original title pane
        iTitlePane->SetTextL( *iOrigTitle );
        }
    // Restore original navi pane
    iNaviPane->Pop( iNaviLabelPane );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::FindCustomControlContainer
// Returns the Custom Control Container.
// -----------------------------------------------------------------------------
//
CMPXAlbumArtEditorDialogCustomControl *CMPXAlbumArtEditorDialog::
    GetAlbumArtEditorDialogCustomControl() const
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::GetAlbumArtEditorDialogCustomControl" );
    CMPXAlbumArtEditorCustomControlContainer *control =
        ( CMPXAlbumArtEditorCustomControlContainer* )
        ControlOrNull ( EMPXAlbumArtEditorDlgCtrlId );
    if ( control )
        {
        return control->iDialogCustomControl;
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::LoadAlbumArtL
// loads album art information from the database
// ----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::LoadAlbumArtL( TBool aShowWaitNote )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::LoadAlbumArtL" );
    HBufC* string = StringLoader::LoadLC( R_MPX_QTN_NMP_NOTE_OPENING );

    if( aShowWaitNote ) // show wait note only when needed
		{
		iCommonUiHelper->ShowWaitNoteL( *string, R_AVKON_SOFTKEYS_EMPTY, EFalse, this );
		}

    CleanupStack::PopAndDestroy( string );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    if ( !iAlbumInfo )
        {
        if ( iCurrentMediaLOp == EMPXOpMediaLIdle )
            {
            MPX_DEBUG1( "CMPXAlbumArtEditorDialog::LoadAlbumArtL Using MediaL" );
            if ( iAlbumArtMode == EMPXModeTracksView )
                {
                iCurrentMediaLOp = EMPXOpMediaLGetAlbumArtInfo;
                AppendCommonAttributes( attrs );
                }
            else
                {
                iCurrentMediaLOp = EMPXOpMediaLGetCurrentMode;
                // get attributes as if it's tracks level so that we don't need to
                // make another async call if that's the case.  if it's album level, those
                // attributes are ignored
                attrs.Append(
                    TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralType |
                    EMPXMediaGeneralCategory | EMPXMediaGeneralId | EMPXMediaGeneralTitle ) );
                }

            CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL( cpath );
            if ( iParam )
                {
                TLex indexLex( iParam->Des() );
                TInt lexToInt = NULL;
                if ( indexLex.Val( lexToInt ) == KErrNone )
                    {
                    cpath->Set( lexToInt );
                    }
                }
            iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
            CleanupStack::PopAndDestroy( cpath );
            }
        else
            {
            MPX_DEBUG1( "CMPXAlbumArtEditorDialog::LoadAlbumArtL is busy, not loading again" );
            }
        }
    else
        {
        if ( iCurrentFindAllLOp == EMPXOpFindAllLIdle )
            {
            MPX_DEBUG1( "CMPXAlbumArtEditorDialog::LoadAlbumArtL Using FindAllL" );
            // iAlbumInfo is valid, something has been fetched before
            CMPXMedia* criteria = CMPXMedia::NewL();
            CleanupStack::PushL( criteria );
            criteria->SetTObjectValueL<TMPXGeneralCategory>(
                KMPXMediaGeneralCategory, EMPXSong );
            TMPXItemId itemId(
                iAlbumInfo->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            criteria->SetTObjectValueL<TMPXItemId>(
                KMPXMediaGeneralId, itemId );
            AppendCommonAttributes( attrs );
            switch ( iAlbumArtMode )
                {
                case EMPXModeTracksView:
                    {
                    MPX_DEBUG1( "CMPXAlbumArtEditorDialog::LoadAlbumArtL EMPXModeTracksView" );
                    criteria->SetTObjectValueL<TMPXGeneralType>(
                        KMPXMediaGeneralType, EMPXItem );
                    iCurrentFindAllLOp = EMPXOpFindAllLGetTrackArtInfo;
                    break;
                    }
                case EMPXModeArtistAlbumView:
                    {
                    MPX_DEBUG1( "CMPXAlbumArtEditorDialog::LoadAlbumArtL EMPXModeArtistAlbumView" );
                    TMPXItemId containerId(
                        iAlbumInfo->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralContainerId ) );
                    criteria->SetTObjectValueL<TMPXItemId>(
                        KMPXMediaGeneralContainerId, containerId );
                    } // fall through on purpose
                case EMPXModeAlbumView:
                    {
                    MPX_DEBUG1( "CMPXAlbumArtEditorDialog::LoadAlbumArtL EMPXModeAlbumView" );
                    criteria->SetTObjectValueL<TMPXGeneralType>(
                        KMPXMediaGeneralType, EMPXGroup );
                    iCurrentFindAllLOp = EMPXOpFindAllLGetAlbumArtInfo;
                    break;
                    }
                }
            iCollectionUtility->Collection().FindAllL( *criteria, attrs.Array(), *this );
            CleanupStack::PopAndDestroy( criteria );
            }
        else
            {
            MPX_DEBUG1( "CMPXAlbumArtEditorDialog::LoadAlbumArtL is busy, not loading again" );
            }
        }

    CleanupStack::PopAndDestroy( &attrs );
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::ExtractAlbumArtL
// extracts the album art
// ----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::ExtractAlbumArtL(
    const CMPXMedia& aMedia, TBool aDismissWaitNoteOnError )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::ExtractAlbumArtL" );
    TInt err( KErrNone );

    CMPXAlbumArtEditorDialogCustomControl* control =
        GetAlbumArtEditorDialogCustomControl();
    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::ExtractAlbumArtL filename length = %d", aMedia.ValueText( KMPXMediaMusicAlbumArtFileName ).Length() );
    MPX_TRAP( err,
        iMPXUtility->ExtractAlbumArtL(
            aMedia,
            *control,
            iModRect.Size() ) );
    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::ExtractAlbumArtL err = %d", err );
    if ( KErrNone != err )
        {
        if ( aDismissWaitNoteOnError )
            {
            // If error, show default album art
            GetAlbumArtEditorDialogCustomControl()->SetModeL(
                CMPXAlbumArtEditorDialogCustomControl::
                EMPXAlbumArtEditorDialogNoAlbumArt );
            MPX_DEBUG1( "CMPXAlbumArtEditorDialog::ExtractAlbumArtL show default album art" );
            iCommonUiHelper->DismissWaitNoteL();
            DrawDeferred();
            }
        else
            {
            User::Leave( err );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::ExtractMultipleAlbumArtL
// control extract of multiple album art
// ----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::ExtractMultipleAlbumArtL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::ExtractMultipleAlbumArtL" );
    if ( iMediaArray )
        {
        if ( !iAsyncCallNext->IsActive() )
            {
            iAsyncCallNext->Call();
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::SaveMediaL
// Saves the updated media.
// -----------------------------------------------------------------------------
//
CMPXMedia* CMPXAlbumArtEditorDialog::SaveMediaLC(
    const TDesC& aFileName, TBool aIsRestore )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::SaveMediaL" );
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    TBool ret( ETrue );

    if ( iMedia->IsSupported( KMPXMediaGeneralUri ) )
        {
        const TDesC& uri = iMedia->ValueText( KMPXMediaGeneralUri );
        TBool isCorrupted( EFalse );
        if ( iMedia->IsSupported( KMPXMediaGeneralFlags ) )
            {
            TUint flags = iMedia->ValueTObjectL<TUint>( KMPXMediaGeneralFlags );
            isCorrupted = ( ( flags ) & ( KMPXMediaGeneralFlagsIsCorrupted ) );
            }
        if ( ConeUtils::FileExists( uri ) && !isCorrupted )
            {
            media->SetTextValueL( KMPXMediaGeneralUri, uri );
            }
        else
            {
            iInvalidFileExist = ETrue;
            ret = EFalse;
            if ( !isCorrupted )
                {
                // set invalid flag in database if it's invalid
                TUint flags( KMPXMediaGeneralFlagsSetOrUnsetBit );
                flags |= KMPXMediaGeneralFlagsIsInvalid;
                CMPXMedia* entry = CMPXMedia::NewL();
                CleanupStack::PushL( entry );
                TMPXItemId id( iMedia->ValueTObjectL<TMPXItemId>(
                    KMPXMediaGeneralId ) );
                TMPXGeneralType type(
                    iMedia->ValueTObjectL<TMPXGeneralType>(
                        KMPXMediaGeneralType ) );
                TMPXGeneralCategory category(
                    iMedia->ValueTObjectL<TMPXGeneralCategory>(
                        KMPXMediaGeneralCategory ) );
                TUid collectionId( iMedia->ValueTObjectL<TUid>(
                    KMPXMediaGeneralCollectionId ) );
                entry->SetTObjectValueL<TMPXItemId>(
                    KMPXMediaGeneralId, id );
                entry->SetTObjectValueL<TMPXGeneralType>(
                    KMPXMediaGeneralType, type );
                entry->SetTObjectValueL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory, category );
                entry->SetTObjectValueL<TUid>(
                    KMPXMediaGeneralCollectionId, collectionId );
                entry->SetTObjectValueL<TUint>(
                    KMPXMediaGeneralFlags, flags );

                DoSetCollectionL( entry, ETrue );
                CleanupStack::PopAndDestroy( entry );
                }
            }
        }
    if ( ret )
        {
        if ( iMedia->IsSupported( KMPXMediaGeneralType ) )
            {
            media->SetTObjectValueL<TInt>(
                KMPXMediaGeneralType,
                *iMedia->Value<TInt>( KMPXMediaGeneralType ) );
            }

        if ( iMedia->IsSupported( KMPXMediaGeneralCategory ) )
            {
            media->SetTObjectValueL<TInt>(
                KMPXMediaGeneralCategory,
                *iMedia->Value<TInt>( KMPXMediaGeneralCategory ) );
            }

        if ( iMedia->IsSupported( KMPXMediaGeneralId ) )
            {
            TMPXItemId id( iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
            media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
            }

        if ( iMedia->IsSupported( KMPXMediaGeneralCollectionId ) )
            {
            media->SetTObjectValueL<TUid>(
                KMPXMediaGeneralCollectionId,
                *iMedia->Value<TUid>( KMPXMediaGeneralCollectionId ) );
            }

        media->SetTextValueL(
            KMPXMediaMusicAlbumArtFileName, aFileName );

        }
    else // Broken link, cannot ben saved
        {
        CleanupStack::PopAndDestroy( media );
        media = NULL;
        }

    iAlbumArtChanged = !aIsRestore;
    return media; // ownership transferred
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::SaveMultipleMediaL
// Saves multiple media.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::SaveMultipleMediaL( TBool aIsRestore )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::SaveMultipleMediaL" );
    TInt currentAlbumArtIndex = iMediaArray->Count();

    CMPXMediaArray* setArray( CMPXMediaArray::NewL() );
    CleanupStack::PushL( setArray );
    for ( TInt i = 0; i < currentAlbumArtIndex && iIsSaving; i++ )
        {
        delete iMedia;
        iMedia = NULL;
        CMPXMedia* origMedia( iMediaArray->AtL( i ) );
        iMedia = CMPXMedia::NewL( *origMedia );
        if ( iNewFileName )
            {
            MPX_DEBUG2( "CMPXAlbumArtEditorDialog::SaveMultipleMediaL setting %d", i );
            CMPXMedia* media = SaveMediaLC( *iNewFileName, aIsRestore );
            if( media )
                {
                setArray->AppendL( *media );
                CleanupStack::PopAndDestroy();
                }
            }
        else
            {
            const TDesC& originalAlbumArtFileName =
                iMedia->ValueText( KMPXMediaMusicOriginalAlbumArtFileName );
            MPX_DEBUG2( "CMPXAlbumArtEditorDialog::SaveMultipleMediaL restoring %d", i );
            CMPXMedia* media = SaveMediaLC( originalAlbumArtFileName, aIsRestore );
            if( media )
                {
                setArray->AppendL( *media );
                CleanupStack::PopAndDestroy();
                }
            }
            }
    // Save the list of changes
    //
    if( setArray->Count() > 0 )
        {
        CMPXMedia* groupMedia( CMPXMedia::NewL() );
        CleanupStack::PushL( groupMedia );
        groupMedia->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
        groupMedia->SetCObjectValueL<CMPXMediaArray>(KMPXMediaArrayContents, setArray );
        CMPXMedia* first( setArray->AtL( 0 ) );
        TUid colId( first->ValueTObjectL<TUid>(KMPXMediaGeneralCollectionId) );
        groupMedia->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, colId );
        DoSetCollectionL( groupMedia, EFalse );
        CleanupStack::PopAndDestroy( groupMedia );
        }
    CleanupStack::PopAndDestroy( setArray );

    if ( iInvalidFileExist )
        {
        if ( !aIsRestore )
            {
            // if it's a change operation and nothing is changed
            iAlbumArtChanged = EFalse;
            }
        // all songs are invalid, show note right now instead
        // there will be no callback from collection
        MPX_DEBUG1("CMPXAlbumArtEditorDialog::SaveMultipleMediaL DoHandleOperationCompleteL");
        DoHandleOperationCompleteL( KErrNone );
        }
    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::SaveMultipleMediaL valid count = %d", currentAlbumArtIndex  );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::DoHandleOperationCompleteL
// Handle save operation complete.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::DoHandleOperationCompleteL( TInt aError )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::DoHandleOperationCompleteL" );
    iCurrentAlbumArtIndex = KErrNotFound;
    iIsSaving = EFalse;
    iCommonUiHelper->DismissWaitNoteL();
    if (( aError == KErrNone ) && (!iInvalidFileExist ))
        {
        if (iAlbumArtMode == EMPXModeTracksView )
            {
            if (iMedia )
                {
                if (iNewFileName )
                    {
                    iMedia->SetTextValueL (KMPXMediaMusicAlbumArtFileName,
                            *iNewFileName );
                    }
                else
                    {
                    const TDesC& originalAlbumArtFileName =
                     iMedia->ValueText ( KMPXMediaMusicOriginalAlbumArtFileName );
                    iMedia->SetTextValueL (KMPXMediaMusicAlbumArtFileName,
                            originalAlbumArtFileName );
                    }
                }
            }
        else
            {
            if (iMediaArray )
                {
                TInt currentAlbumArtIndex = iMediaArray->Count ( );

                for (TInt i = 0; i < currentAlbumArtIndex && iIsSaving; i++ )
                    {
                    delete iMedia;
                    iMedia = NULL;
                    CMPXMedia* origMedia( iMediaArray->AtL ( i ));
                    iMedia = CMPXMedia::NewL ( *origMedia );
                    if ( iNewFileName )
                        {
                        iMedia->SetTextValueL (KMPXMediaMusicAlbumArtFileName,
                                *iNewFileName );
                        }
                    else
                        {
                        const TDesC& originalAlbumArtFileName =
                         iMedia->ValueText ( KMPXMediaMusicOriginalAlbumArtFileName );
                        iMedia->SetTextValueL (KMPXMediaMusicAlbumArtFileName,
                                originalAlbumArtFileName );
                        }
                    }
                }
            }
        }
    MPX_DEBUG2( "CMPXAlbumArtEditorDialog::DoHandleOperationCompleteL iOpCanceled = %d", iOpCanceled );
    if ( aError == KErrNone )
        {
        if ( !iOpCanceled )
            {
            HBufC* string = NULL;
            if ( iInvalidFileExist )
                {
                if ( iNewFileName )
                    {
                    // new filename is defined, albumart change
                    // invalid file exist
                    string = StringLoader::LoadLC(
                        R_MPX_QTN_NMP_NOTE_CHANGE_WITH_INVALID );
                    }
                else
                    {
                    // new file name not defined, albumart restore
                    // invalid file exist
                    string = StringLoader::LoadLC(
                        R_MPX_QTN_NMP_NOTE_RESTORE_WITH_INVALID );
                    }
                iCommonUiHelper->DisplayInfoNoteL( *string );
                }
            else
                {
                if ( iNewFileName )
                    {
                    // new filename is defined, albumart change
                    string = StringLoader::LoadLC(
                        R_MPX_QTN_NMP_NOTE_ART_CHANGED );
                    }
                else
                    {
                    // new file name not defined, albumart restore
                    string = StringLoader::LoadLC(
                        R_MPX_QTN_NMP_NOTE_ART_RESTORED );
                    }
                iCommonUiHelper->DisplayConfirmNoteL( *string );
                }
            CleanupStack::PopAndDestroy( string );
            }
        }
    else
        {
        iCommonUiHelper->HandleErrorL( aError );
        }
    iOpCanceled = EFalse;
    iCurrentFindAllLOp = EMPXOpFindAllLIdle;
    LoadAlbumArtL( ETrue );
    }

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::DoHandleCollectionMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        MPX_DEBUG2( "CMPXAlbumArtEditorDialog::DoHandleCollectionMessageL Event = %d", event );
        if ( event == TMPXCollectionMessage::EItemChanged )
            {
            if ( iOpCanceled )
                {
                DoHandleOperationCompleteL( KErrNone );
                }
            }
        else if ( KMPXMessageIdItemChanged == id )
            {
            if ( !iIsSaving )
                {
                // ignore all the item change events when saving
                LoadAlbumArtL( ETrue );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::DoSetCollectionL
// Perform a set operation
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::DoSetCollectionL( CMPXMedia* aMedia, TBool aSync )
    {
    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, KMPXCommandIdCollectionSet );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, aSync );
    TUid colId( aMedia->ValueTObjectL<TUid>( KMPXMediaGeneralCollectionId ) );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, aMedia );

    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::AppendCommonAttributes
// Appends the common attributes to attributes array
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::AppendCommonAttributes(
    RArray<TMPXAttribute>& aAryAttribute )
    {
    aAryAttribute.Append(
        TMPXAttribute( KMPXMediaIdGeneral, EMPXMediaGeneralUri
        | EMPXMediaGeneralType | EMPXMediaGeneralCategory
        | EMPXMediaGeneralId | EMPXMediaGeneralCollectionId
        | EMPXMediaGeneralFlags ) );
    aAryAttribute.Append( TMPXAttribute( KMPXMediaIdMusic,
        EMPXMediaMusicAlbumArtFileName |
        EMPXMediaMusicOriginalAlbumArtFileName ) );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::HandleAlbumArtEventL
// Handle album art events
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleAlbumArtEventL(
    TMPXAlbumArtEvents /*aEvent*/, TInt aError )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::HandleAlbumArtEventL" );
    if ( aError != KErrNone && iAlbumArtMode != EMPXModeTracksView )
        {
        // continue to extract next album art if current one is not found
        ExtractMultipleAlbumArtL();
        }
    else
        {
        MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleAlbumArtEventL extract Error = %d", aError );
        iCommonUiHelper->DismissWaitNoteL();
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::HandleFindAllL
// Handle callback for "find" operation
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleFindAllL(
    const CMPXMedia& aResults,
    TBool /*aComplete*/,
    TInt aError )
    {
    MPX_DEBUG2( "-->CMPXAlbumArtEditorDialog::HandleFindAllL Error = %d", aError );
    switch ( iCurrentFindAllLOp )
        {
        case EMPXOpFindAllLGetAlbumArtInfo:
            {
            if ( aError == KErrNone )
                {
                delete iMediaArray;
                iMediaArray= NULL;
                CMPXMediaArray* mediaAry( aResults.Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
                if (!mediaAry )
                    {
                    iCommonUiHelper->DismissWaitNoteL();
                    User::Leave(KErrNoMemory);
                    }
                iMediaArray = CMPXMediaArray::NewL( *mediaAry );
                if ( iMediaArray )
                    {
                    TInt count = iMediaArray->Count();
                    for ( TInt i = 0; i < count && !iAlbumArtChanged; i++ )
                        {
                        // check if album art has changed
                        CMPXMedia* media = iMediaArray->AtL( i );
                        const TDesC& albumArtFileName =
                            media->ValueText( KMPXMediaMusicAlbumArtFileName );
                        if ( albumArtFileName.Length() > 0 )
                            {
                            if ( albumArtFileName.Compare( media->ValueText(
                                KMPXMediaMusicOriginalAlbumArtFileName ) ) != 0 )
                                {
                                iAlbumArtChanged = ETrue;
                                }
                            }
                        }
                    iCurrentAlbumArtIndex = 0;
                    ExtractMultipleAlbumArtL();
                    iCurrentFindAllLOp = EMPXOpFindAllLIdle;
                    return;
                    }
                }
            iCommonUiHelper->DismissWaitNoteL();
            break;
            }
        case EMPXOpFindAllLGetTrackArtInfo:
            {
            CMPXMediaArray* tempArray =
                aResults.Value<CMPXMediaArray>( KMPXMediaArrayContents );
            if (!tempArray )
                {
                iCommonUiHelper->DismissWaitNoteL();
                User::Leave (KErrNoMemory );
                }
            if ( tempArray->Count() == 1 )
                {
                delete iMedia;
                iMedia = NULL;
                CMPXMedia* origMedia( tempArray->AtL( 0 ) );
                iMedia = CMPXMedia::NewL( *origMedia );
                const TDesC& albumArtFileName =
                    iMedia->ValueText( KMPXMediaMusicAlbumArtFileName );
                if ( albumArtFileName.Length() > 0 )
                    {
                    if ( albumArtFileName.Compare(
                        iMedia->ValueText( KMPXMediaMusicOriginalAlbumArtFileName ) ) == 0 )
                        {
                        iAlbumArtChanged = EFalse;
                        }
                    else
                        {
                        iAlbumArtChanged = ETrue;
                        }
                    }
                ExtractAlbumArtL( *iMedia );
                }
            else
                {
                MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleFindAllL invalid array length %d", tempArray->Count() );
                iCommonUiHelper->DismissWaitNoteL();
                }
            break;
            }
        default:
            {
            MPX_DEBUG2( "CMPXAlbumArtEditorDialog::HandleFindAllL unknown op %d", iCurrentFindAllLOp );
            iCommonUiHelper->DismissWaitNoteL();
            break;
            }
        }
    iCurrentFindAllLOp = EMPXOpFindAllLIdle;
    MPX_DEBUG1( "<--CMPXAlbumArtEditorDialog::HandleFindAllL" );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::ProcessCommandL
// Act on the menu selection if menu is showing.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::ProcessCommandL( TInt aCommandId )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::ProcessCommandL" );
    CAknDialog::ProcessCommandL( aCommandId );
    if ( MenuShowing() )
        {
        HideMenu();
        }
    DoProcessCommandL( aCommandId );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::CreateCustomControlL
// Creates a Custom Control for the dialog.
// -----------------------------------------------------------------------------
//
SEikControlInfo CMPXAlbumArtEditorDialog::CreateCustomControlL(
    TInt aControlType )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::CreateCustomControlL" );
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;

    switch (aControlType)
        {
        case EMPXAlbumArtEditorDlg:
            {
            controlInfo.iControl = new ( ELeave )
                CMPXAlbumArtEditorCustomControlContainer;
            break;
            }
        default:
            {
            break;
            }
        }
    return controlInfo;
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::PreLayoutDynInitL
// Initialises the dialog's controls before the dialog is sized and layed out.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::PreLayoutDynInitL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::PreLayoutDynInitL" );

    // determine current mode
    GetCurrentModeL();

    AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, iModRect );

    CMPXAlbumArtEditorCustomControlContainer *control1 =
        ( CMPXAlbumArtEditorCustomControlContainer* ) ControlOrNull (
            EMPXAlbumArtEditorDlgCtrlId );
    CMPXAlbumArtEditorDialogCustomControl *control = NULL;

    if ( control1 )
        {
        control = CMPXAlbumArtEditorDialogCustomControl::NewL(
            *control1, iModRect, this );
        control1->iDialogCustomControl = control;
        GetAlbumArtEditorDialogCustomControl()->MakeVisible( ETrue );
        }

    BackupPreviousStatusPaneL();

    // Set new title pane
    HBufC* title = StringLoader::LoadLC(
        R_MPX_CUI_COLLECTION_ALBUM_ART_DIALOG_TITLE );
    iTitlePane->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    TSize size( iModRect.Width() - KMPXAlbumArtBorder,
        iModRect.Height() - KMPXAlbumArtBorder );
    CAknDialog::PreLayoutDynInitL();
    iAvkonAppUi->AddToStackL( this );

    HBufC* string = StringLoader::LoadLC( R_MPX_QTN_NMP_NOTE_OPENING );
    // display waiting note
    iCommonUiHelper->ShowWaitNoteL( *string, R_AVKON_SOFTKEYS_EMPTY,
        EFalse, this );
    CleanupStack::PopAndDestroy( string );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::OkToExitL
// This function ALWAYS returns ETrue.
// -----------------------------------------------------------------------------
//
TBool CMPXAlbumArtEditorDialog::OkToExitL( TInt aButtonId )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::OkToExitL" );
    if ( iFetchingPicture )
       {
       return EFalse;
       }
    TBool retVal = EFalse;
    switch (aButtonId)
        {
        case EAknSoftkeyBack:
            {
            retVal = ETrue;
            break;
            }
        case EAknSoftkeyOk:
            {
            // middle softkey should not dismiss dialog
            retVal = EFalse;
            break;
            }
         case EAknSoftkeyChange:
            {
             DoProcessCommandL(EMPXCmdAlbumArtEditorDlgChange);
             break;
             }
        default:
            {
            retVal = CAknDialog::OkToExitL( aButtonId );
            break;
            }
        }
    if (retVal) // if dialog is dismissed
        {
        RestorePreviousStatusPaneL();
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::DynInitMenuPaneL
// Dynamically initialises a menu pane.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::DynInitMenuPaneL(
     TInt aResourceId,
     CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::DynInitMenuPaneL" );
    if ( aResourceId == R_MPX_CUI_ALBUM_ART_DIALOG_MENU )
        {
        aMenuPane->SetItemDimmed( EAknCmdExit, ETrue ); 
        TInt usbUnblockingStatus;
        RProperty::Get( KMPXViewPSUid,
                        KMPXUSBUnblockingPSStatus, 
                        usbUnblockingStatus );

        if ( usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive )
            {
            aMenuPane->SetItemDimmed( EMPXCmdAlbumArtEditorDlgChange, ETrue );
            aMenuPane->SetItemDimmed( EMPXCmdAlbumArtEditorDlgRestore, ETrue );
            }
        if ( !iAlbumArtChanged )
            {
            aMenuPane->SetItemDimmed( EMPXCmdAlbumArtEditorDlgRestore, ETrue );
            }
        }
    CAknDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::DoProcessCommandL
// Process menu command.
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::DoProcessCommandL( TInt aCommandId )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::DoProcessCommandL" );
    switch ( aCommandId )
        {
        case EMPXCmdAlbumArtEditorDlgChange:
            {
            // Renders Image Selection Query
            CMPXAlbumArtEditorDialogImageThumbnailUtil* thumbnailCmd =
                CMPXAlbumArtEditorDialogImageThumbnailUtil::NewL();
            CleanupStack::PushL( thumbnailCmd );
            CDesCArray* selectedFile = new ( ELeave ) CDesCArrayFlat( 1 );
            CleanupStack::PushL( selectedFile );
            // selectedFile includes the pathname to the img
            // Check if any img is selected
            iFetchingPicture = ETrue; // Set the flag for async retrieval
            TBool fecthOK( EFalse );
            TRAP_IGNORE( fecthOK = thumbnailCmd->FetchThumbnailL( *selectedFile ) )
            iFetchingPicture = EFalse; // Now we can safely process events
			// Restore dialog size
			AknLayoutUtils::LayoutMetricsRect(
				AknLayoutUtils::EMainPane, iModRect );
			CMPXAlbumArtEditorCustomControlContainer *control =
				( CMPXAlbumArtEditorCustomControlContainer* )
				ControlOrNull ( EMPXAlbumArtEditorDlgCtrlId );
			if( control )
				{
				control->SetRect( iModRect );
				control->iDialogCustomControl->SetRect( iModRect );
				SetRect(iModRect);
				}

            if ( fecthOK )
                {
                TInt err = KErrNone;
                if ( iAlbumArtMode != EMPXModeTracksView )
                    {
                    // show confirmation in album level
                    CAknQueryDialog* confirmationDlg = CAknQueryDialog::NewL();
                    CleanupStack::PushL( confirmationDlg );

                    HBufC* promptTxt = StringLoader::LoadLC(
                        R_MPX_CUI_CHANGE_ALBUM_ART_CONFIRMATION );

                    confirmationDlg->SetPromptL( *promptTxt );
                    CleanupStack::PopAndDestroy( promptTxt );
                    CleanupStack::Pop( confirmationDlg );

                    if ( !confirmationDlg->ExecuteLD(
                        R_MPX_CUI_RESTORE_ALBUM_ART_CONFIRMATION_QUERY ) )
                        {
                        err = KErrCancel;
                        }
                    }
                if ( err == KErrNone )
                    {
                    HBufC* string = StringLoader::LoadLC(
                        R_MPX_QTN_NMP_NOTE_ADDING_ALBUM_ART );
                    // display waiting note
                    iCommonUiHelper->ShowWaitNoteL( *string,
                        R_MPX_WAITNOTE_SOFTKEYS_EMPTY_STOP, EFalse, this );
                    CleanupStack::PopAndDestroy( string );
                    string = NULL;
                    const TDesC& fileName = selectedFile->MdcaPoint( 0 );
                    delete iNewFileName;
                    iNewFileName = NULL;
                    iNewFileName = fileName.AllocL();
                    iInvalidFileExist = EFalse;
                    if ( iMediaArray )
                        {
                        iIsSaving = ETrue;
                        SaveMultipleMediaL( EFalse );
                        }
                    else
                        {
                        CMPXMedia* media = SaveMediaLC( fileName, EFalse );
                        if( media )
                            {
                            DoSetCollectionL( media, EFalse );
                            CleanupStack::PopAndDestroy( media );
                            }
                        }
                    }
                }
            CleanupStack::PopAndDestroy( selectedFile );
            CleanupStack::PopAndDestroy( thumbnailCmd );
            break;
            }
        case EMPXCmdAlbumArtEditorDlgRestore:
            {
            TInt err = KErrNone;
            if ( iAlbumArtMode != EMPXModeTracksView )
                {
                // show confirmation in album level
                CAknQueryDialog* confirmationDlg = CAknQueryDialog::NewL();
                CleanupStack::PushL( confirmationDlg );

                HBufC* promptTxt = StringLoader::LoadLC(
                    R_MPX_CUI_RESTORE_ALBUM_ART_CONFIRMATION );

                confirmationDlg->SetPromptL( *promptTxt );
                CleanupStack::PopAndDestroy( promptTxt );
                CleanupStack::Pop( confirmationDlg );

                if ( !confirmationDlg->ExecuteLD(
                    R_MPX_CUI_RESTORE_ALBUM_ART_CONFIRMATION_QUERY ) )
                    {
                    err = KErrCancel;
                    }
                }
            if ( err == KErrNone )
                {
                HBufC* string = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_NOTE_RESTORING_ALBUM_ART );
                // display waiting note
                iCommonUiHelper->ShowWaitNoteL( *string,
                    R_MPX_WAITNOTE_SOFTKEYS_EMPTY_STOP, EFalse, this );
                CleanupStack::PopAndDestroy( string );
                string = NULL;
                delete iNewFileName;
                iNewFileName = NULL;
                iInvalidFileExist = EFalse;
                if ( iMediaArray )
                    {
                    // save for all songs
                    iIsSaving = ETrue;
                    SaveMultipleMediaL( ETrue );
                    }
                else
                    {
                    // Everything ok, now the original album art can be restored
                    const TDesC& originalAlbumArtFileName =
                        iMedia->ValueText( KMPXMediaMusicOriginalAlbumArtFileName );
                    CMPXMedia* media = SaveMediaLC( originalAlbumArtFileName, ETrue );
                    if( media )
                        {
                        DoSetCollectionL( media, EFalse );
                        CleanupStack::PopAndDestroy( media );
                        }
                    }
                }
            break;
            }
        case EMPXCmdAlbumArtEditorDlgHelp:
            {
            TCoeHelpContext helpContext;
            GetHelpContext( helpContext );

            CArrayFixFlat<TCoeHelpContext>* array =
                new(ELeave)CArrayFixFlat<TCoeHelpContext>( 1 );

            CleanupStack::PushL( array );
            array->AppendL( helpContext );
            CleanupStack::Pop(); // array

            // HlpLauncher deletes array
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), array );
            break;
            }
        case EAknCmdExit:
            {
            static_cast<MEikCommandObserver*>(iEikonEnv->EikAppUi())->
                ProcessCommandL(aCommandId);
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::CountComponentControls
// Counts the number of components.
// -----------------------------------------------------------------------------
//
TInt CMPXAlbumArtEditorDialog::CountComponentControls() const
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::CountComponentControls" );
    TInt newitems = 0;
    if ( GetAlbumArtEditorDialogCustomControl() ) newitems++;
    return CAknDialog::CountComponentControls() + newitems;
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::ComponentControl
// Returns component control.
// -----------------------------------------------------------------------------
//
CCoeControl* CMPXAlbumArtEditorDialog::ComponentControl( TInt aIndex ) const
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::ComponentControl" );
    if (aIndex == CAknDialog::CountComponentControls() )
        {
        return GetAlbumArtEditorDialogCustomControl();
        }
    return CAknDialog::ComponentControl( aIndex );
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::UpdateNaviPane
// Update the dialogs navipane.
// -----------------------------------------------------------------------------
//

void CMPXAlbumArtEditorDialog::UpdateNaviPaneL(const TDesC& aText)
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::UpdateNaviPane" );

    // Set the navi text
    CAknNaviLabel* naviLabel = static_cast<CAknNaviLabel*>
        ( iNaviLabelPane->DecoratedControl() );
    naviLabel->SetTextL( aText );
    iNaviLabelPane->DrawNow();
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::GetHelpContext
// Gets Help
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::GetHelpContext" );
    aContext.iMajor = KAppUidMusicPlayerX;
    aContext.iContext = KMUS_HLP_ALBUM_ART_EDITING; 
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::HandleResourceChange( TInt aType )
    {
    MPX_DEBUG2( "-->CMPXAlbumArtEditorDialog::HandleResourceChange type = 0x%x", aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, iModRect );

        CMPXAlbumArtEditorCustomControlContainer *control =
            ( CMPXAlbumArtEditorCustomControlContainer* )
            ControlOrNull ( EMPXAlbumArtEditorDlgCtrlId );
        if ( control )
            {
            TRAP_IGNORE(
                control->iDialogCustomControl->SetModeL(
                CMPXAlbumArtEditorDialogCustomControl::EMPXAlbumArtEditorDialogBlank)
                );
            control->SetRect( iModRect );
            control->iDialogCustomControl->SetRect( iModRect );
            }
        TRAP_IGNORE( LoadAlbumArtL( EFalse ) );
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        CMPXAlbumArtEditorCustomControlContainer *control =
            ( CMPXAlbumArtEditorCustomControlContainer* )
            ControlOrNull ( EMPXAlbumArtEditorDlgCtrlId );
        if ( control )
            {
            TRAP_IGNORE(
                control->iDialogCustomControl->HandleSkinChangedL() );
            }
        }
    CAknDialog::HandleResourceChange( aType );
    MPX_DEBUG1( "<--CMPXAlbumArtEditorDialog::HandleResourceChange" );
    }

// ---------------------------------------------------------------------------
// From MProgressDialogCallback
// Callback method. Get's called when a dialog is dismissed
// ---------------------------------------------------------------------------
//
void CMPXAlbumArtEditorDialog::DialogDismissedL( TInt aButtonId )
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::DialogDismissedL" );
    switch ( aButtonId )
        {
        case EAknSoftkeyCancel:
            {
            MPX_DEBUG1( "CMPXAlbumArtEditorDialog::DialogDismissedL canceled, sending cancel requet" );
            // after canceling, the current request will still be completed
            // handle the canceling in the callback
            iCollectionUtility->Collection().CancelRequest();
            // setup flag so that confirmation note is not displayed
            iOpCanceled = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    }

TInt CMPXAlbumArtEditorDialog::ExtractNextAlbumArtCallBackL(TAny* aPtr)
    {
	static_cast<CMPXAlbumArtEditorDialog*>(aPtr)->ExtractNextAlbumArtL();
	return KErrNone;
    }

void CMPXAlbumArtEditorDialog::ExtractNextAlbumArtL()
    {
    MPX_FUNC( "CMPXAlbumArtEditorDialog::ExtractNextAlbumArtL" );
    if ( iCurrentAlbumArtIndex < iMediaArray->Count() )
        {
        MPX_DEBUG2( "CMPXAlbumArtEditorDialog::ExtractNextAlbumArtL extracting %d", iCurrentAlbumArtIndex );
        CMPXMedia* media( iMediaArray->AtL( iCurrentAlbumArtIndex ) );
        MPX_TRAPD( err, ExtractAlbumArtL( *media, EFalse ) );
        iCurrentAlbumArtIndex++;
        if (err != KErrNone)
            {
            iAsyncCallNext->Call();
            }
        }
    else
        {
        // when no Album art found:
        GetAlbumArtEditorDialogCustomControl()->SetModeL(
            CMPXAlbumArtEditorDialogCustomControl::
            EMPXAlbumArtEditorDialogNoAlbumArt );
        MPX_DEBUG1( "CMPXAlbumArtEditorDialog::ExtractNextAlbumArtL show default album art" );
        iCommonUiHelper->DismissWaitNoteL();
        DrawDeferred();
        }
    }


//  End of File
