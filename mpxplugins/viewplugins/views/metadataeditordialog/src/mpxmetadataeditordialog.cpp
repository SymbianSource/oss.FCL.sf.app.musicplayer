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
* Description:  Music Player metadata editor dialog implementation
*
*/


// INCLUDE FILES
#include <eikmenub.h>
#include <eikcapc.h>
#include <eikdialg.h> // for dialog functions
#include <eikedwin.h> // for edwins
#include <eikmfne.h>  // for number editor
#include <AknPopupFieldText.h>
#include <AknQueryValueText.h>
#include <aknappui.h>
#include <eikspane.h> // for status pane update/restore
#include <akntitle.h> // for title pane
#include <aknnavilabel.h> // for navi label
#include <aknnavi.h>
#include <aknnavide.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <aknlists.h>
#include <aknclearer.h>
#include <hlplch.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <drmrightsmanager.rsg>
#include <centralrepository.h> // for podcasting cenrep key
#include <tz.h>

#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include <mplayersecondarydisplayapi.h>
#include <aknEditStateIndicator.h>
#include <AknIndicatorContainer.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <mpxmetadataeditordialog.rsg>
#include <mpxcollectionutility.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxpodcastdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>

#include <mpxcollectionframeworkdefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionplaylist.h>
#include <mpxconstants.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionplugin.hrh>
#include <mpxinternalcrkeys.h>
#include <mpxuser.h>
#include "mpxcommonuihelper.h"
#include "mpxmetadataeditordialog.h"
#include "mpxmetadataeditordialog.hrh"
#include "mpxmetadataeditordialog.hlp.hrh"
#include "mpxlog.h"
#include <drmuihandling.h>
#include <mpxplaybackutility.h>

// CONSTANTS
const TInt KMPXFileDetailsMaxTitleLen = 32;
const TInt KMPXFileDetailsMaxBufferLen = 255;
const TInt KMPXMinNumDateTrack = 0;
const TInt KMPXFileDetailsExtraBufferLen = 10;

const TInt KMPXMaxTimeLength = 36;
const TInt KMPXMaxVal = 512;
const TInt KMPXOneDigit = 10;
const TInt KMPXTwoDigits = 100;

const TInt KMPXBitrateFactor = 1000;
const TInt KSecondInMilliseconds = 1000;
const TInt KOneHourInSeconds = 3600;



const TInt KDigitFive = 5;
const TInt KDigitNine = 9;
const TInt KDigitTen = 10;
const TInt KDigitEleven = 11;
const TInt KDigitFourTeen = 14;
const TInt KDigitHundred = 100;

const TInt KMaxDaysInOneMonths = 31;
const TInt KMaxDaysInTwoMonths = 62;
const TInt KMaxDaysInThreeMonths = 92;
const TInt KMaxDaysInFourMonths = 123;
const TInt KMaxDaysInFiveMonths = 153;
const TInt KMaxDaysInSixMonths = 184;
const TInt KMaxDaysInSevenMonths= 215;
const TInt KMaxDaysInEightMonths = 245;
const TInt KMaxDaysInNineMonths = 276;
const TInt KMaxDaysInTenMonths = 306;
const TInt KMaxDaysInElevenMonths = 337;
const TInt KMaxDaysInTwelveMonths = 366;

// MACROS
_LIT( KMPXMetadataEditorRscPath, "mpxmetadataeditordialog.rsc" );
_LIT( KMPXDrmManagerRscPath, "drmrightsmanager.rsc" );
_LIT( KMPXSpace, " ");

const TInt KMPXDurationDisplayResvLen( 10 );
const TInt64 KSecondInMicroSeconds( 1000000 );

// FUNCTION PROTOTYPES

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CMPXQueryDialog::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXQueryDialog* CMPXQueryDialog::NewL()
    {
    CMPXQueryDialog* self = new (ELeave) CMPXQueryDialog();
    return self;
    }

// ----------------------------------------------------------------------------
// CMPXQueryDialog::OfferKeyEventL
// Keyevent handler.
// ----------------------------------------------------------------------------
//
TKeyResponse CMPXQueryDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    // exit from query dialog whever lost forground, currently implementing by mornitor
    // application key and end key
    if (( aKeyEvent.iScanCode == EStdKeyNo ||
        aKeyEvent.iScanCode == EStdKeyApplication0  &&
        aType == EEventKey ) ||
        ( aKeyEvent.iScanCode == EStdKeyApplication6  &&
        aType == EEventKeyDown ))
        {
        TryExitL(EEikBidCancel);
        return EKeyWasNotConsumed;
        }
    else
        {
        return CAknQueryDialog::OfferKeyEventL( aKeyEvent,aType );
        }
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXMetadataEditorDialog* CMPXMetadataEditorDialog::NewL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::NewL" );
    CMPXMetadataEditorDialog* self =
          new (ELeave) CMPXMetadataEditorDialog();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::~CMPXMetadataEditorDialog
// Destructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CMPXMetadataEditorDialog::~CMPXMetadataEditorDialog()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::~CMPXMetadataEditorDialog" );
    delete iEmptyNaviDecorator;
    delete iTitlePaneText;
    delete iCommonUiHelper;
    delete iParam;
    delete iIdle;
    delete iDrmInfo;
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }
    if ( iCollectionUiHelper )
        {
        iCollectionUiHelper->Close();
        }
    if ( iMedia )
        {
        delete iMedia;
        }
    if ( iTextValues )
        {
        delete iTextValues;
        }
    if ( iValueTextArray )
        {
        delete iValueTextArray;
        }
    if ( iGenreArr )
        {
        delete iGenreArr;
        }
    if ( iLibraryTextValues )
        {
        delete iLibraryTextValues;
        }
    if ( iLibraryValueTextArray )
        {
        delete iLibraryValueTextArray;
        }
    if ( iLibraryArr )
        {
        delete iLibraryArr;
        }
    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }
    if ( iDrmResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iDrmResourceOffset );
        }
    if(iDrmUiHandler)
        {
        delete iDrmUiHandler;
        }
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SetParamL
// parameter passed in from viewframework
// ----------------------------------------------------------------------------
//
EXPORT_C void CMPXMetadataEditorDialog::SetParamL( const TDesC* aParam )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SetParamL" );
    delete iParam;
    iParam = NULL;
    iParam = aParam->AllocL();
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::OkToExitL
// This function ALWAYS returns ETrue
// -----------------------------------------------------------------------------
//
TBool CMPXMetadataEditorDialog::OkToExitL(TInt aButtonId)
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::OkToExitL" );
    TBool res = EFalse;

    switch ( aButtonId )
        {
        case EAknSoftkeyDone:
            {
            SaveFormDataL();
            RestoreNaviLabelL();
            RestoreTitlePaneL();
            res = ETrue;
            break;
            }
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
            {
            RestoreNaviLabelL();
            RestoreTitlePaneL();
            res = ETrue;
            break;
            }
        case EAknSoftkeyOptions:
        default:
            {
            // display the options menu
            res = CAknForm::OkToExitL( aButtonId );
            break;
            }
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::ExecuteLD
// -----------------------------------------------------------------------------
//
TInt CMPXMetadataEditorDialog::ExecuteLD( TInt /*aResourceId*/ )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::ExecuteLD" );
    TInt resId = R_MPX_CUI_SONG_DETAILS_DIALOG;
    if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
        {
        resId = R_MPX_CUI_PODCAST_DETAILS_DIALOG;
        }
    else if( iDisablePodcasting )
        {
        resId = R_MPX_CUI_SONG_DETAILS_DIALOG_NO_MOVE_LIBRARY;
        }
    return CAknForm::ExecuteLD( resId );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::ViewFileDetailsPopupL
// Display file details popup window
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::ViewFileDetailsPopupL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::ViewFileDetailsPopupL" );
    CAknDoublePopupMenuStyleListBox* listBox =
        new (ELeave) CAknDoublePopupMenuStyleListBox();
    CleanupStack::PushL( listBox );
    CAknPopupList* popupList = CAknPopupList::NewL(
                                listBox,
                                R_AVKON_SOFTKEYS_OK_EMPTY,
                                AknPopupLayouts::EMenuDoubleWindow );
    CleanupStack::PushL( popupList );

    listBox->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Enable Marquee
    static_cast<CEikFormattedCellListBox*>
        ( listBox )->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    // Retrieve heading array
    CDesCArrayFlat* headingsArray = NULL;
    TInt itemCount = 0;
    if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
        {
        headingsArray = iCoeEnv->ReadDesCArrayResourceL(
            R_MPX_CUI_FD_HEADINGS_SONGS );
        itemCount = EMPXSongsFileDetailsHeadingCount;
        }
    else if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
        {
        headingsArray = iCoeEnv->ReadDesCArrayResourceL(
            R_MPX_CUI_FD_HEADINGS_PODCASTS );
        itemCount = EMPXPodcastsFileDetailsHeadingCount;
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PushL( headingsArray );

    CAknLocalScreenClearer* clearer = CAknLocalScreenClearer::NewLC( EFalse );
    CDesCArray* itemArray =
        static_cast<CDesCArray*>( listBox->Model()->ItemTextArray() );
    CDesCArrayFlat* dataArray =
        new ( ELeave ) CDesCArrayFlat( itemCount );
    CleanupStack::PushL( dataArray );

    if ( iMedia )
        {
        PopulateFileDetailsL( headingsArray, dataArray );
        // item count might have been updated due to addition of
        // DRM content details
        itemCount = headingsArray->MdcaCount();
        }
    else
        {
        // properties not available
        for ( TInt i = 0; i < itemCount; i++ )
            {
            dataArray->AppendL( KNullDesC );
            }
        }

    for ( TInt i = 0; i < itemCount; i++ )
        {
        AddItemToListBoxL( headingsArray->MdcaPoint( i ), dataArray->MdcaPoint( i ), itemArray );
        }
    CleanupStack::PopAndDestroy( dataArray );
    CleanupStack::PopAndDestroy( clearer );
    CleanupStack::PopAndDestroy( headingsArray );

    // Set title
    TBuf<KMPXFileDetailsMaxTitleLen> title;
    iCoeEnv->ReadResourceL( title, R_MPX_CUI_FD_TITLE );
    popupList->SetTitleL( title );

    // Show popup list
    popupList->ExecuteLD();
    CleanupStack::Pop();
    CleanupStack::PopAndDestroy( listBox );
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection message.
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleCollectionMessage(
    CMPXMessage* /*aMessage*/, TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d a folder
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleOpenL(
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
void CMPXMetadataEditorDialog::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle media properties.
// Notes: The client is responsible for delete the object of aMedia.
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::HandleCollectionMediaL" );
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ) );
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::CMPXMetadataEditorDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMPXMetadataEditorDialog::CMPXMetadataEditorDialog()
    {
    // do nothing
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::ConstructL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::ConstructL" );

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXMetadataEditorRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    parse.Set( KMPXDrmManagerRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName drmResourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( drmResourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), drmResourceFile );
    TRAPD(drmResourceError,iDrmResourceOffset = coeEnv->AddResourceFileL( drmResourceFile ));
    iDrmDetails = (drmResourceError == KErrNone) ? ETrue: EFalse;

    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iCommonUiHelper = CMPXCommonUiHelper::NewL();
    
    CAknForm::ConstructL( R_MPX_CUI_SONG_DETAILS_MENUBAR );

    iCurrentLibrary = EMPXMetadataEditorDlgCollection;
    // check if it's podcast collection
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    if ( cpath->Levels() > 0 )
        {
        RArray<TUid> ary;
        CleanupClosePushL( ary );
        ary.AppendL( TUid::Uid(EMPXCollectionPluginPodCast) );
        TUid podcastUid = iCollectionUtility->CollectionIDL( ary.Array() );
        CleanupStack::PopAndDestroy( &ary );

        if ( cpath->Id( 0 ) == podcastUid.iUid )
            {
            MPX_DEBUG1( "CMPXMetadataEditorDialog::ConstructL Podcast collection" );
            iCurrentLibrary = EMPXMetadataEditorDlgPodcast;
            }
        }
    iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    CleanupStack::PopAndDestroy( cpath );

    // Get podcasting key
    TInt val(0);
    CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
    repository->Get( KMPXMPLocalVariation, val );
    iDisablePodcasting = val&KMPXDisablePodcastingOption ? ETrue : EFalse;
    delete repository;
    
    //DRM information popup
    iDrmUiHandler = DRM::CDrmUiHandling::NewL();
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::CleanUpGenreText
// Clean up function for genre related members.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::CleanUpGenreText()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::CleanUpGenreText" );

    if ( iGenreArr )
        {
        delete iGenreArr;
        iGenreArr = NULL;
        }
    if ( iValueTextArray )
        {
        delete iValueTextArray;
        iValueTextArray = NULL;
        }
    if ( iTextValues )
        {
        delete iTextValues;
        iTextValues = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::FetchGenreL
// Populates genre popup with genre list for tracks.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::FetchGenreL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::FetchGenreL" );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( KMPXMediaGeneralTitle );
    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, TUid::Uid( 0x101FFC3A ) );
    criteria->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXGroup );
    criteria->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXGenre );
    iCollectionUtility->Collection().FindAllL( *criteria, attrs.Array(), *this );
    CleanupStack::PopAndDestroy( criteria );
    CleanupStack::PopAndDestroy( &attrs );
    }

// ---------------------------------------------------------------------------
// Handle media properties.
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::DoHandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::DoHandleMediaL" );

    if ( iCurrentMediaLOp == EMPXMetadataEditorGetSongInfo )
        {
        delete iMedia;
        iMedia = NULL;
        if ( aError == KErrNone )
            {
            iMedia = CMPXMedia::NewL( aMedia );
            // common for both music and podcast
            // Fetch the artist

            if ( iMedia->IsSupported( KMPXMediaDrmProtected ) )
                {
                iIsDrmProtected = iMedia->ValueTObjectL<TBool>( KMPXMediaDrmProtected );
                }

            SetControlTextL( EMPXMetadataEditorDlgCtrlIdArtist,
                iMedia->ValueText( KMPXMediaMusicArtist ), KNullDesC );

            // Fetch the album
            SetControlTextL( EMPXMetadataEditorDlgCtrlIdAlbum,
                iMedia->ValueText( KMPXMediaMusicAlbum ), KNullDesC );

            // Fetch the track number
            SetControlNumberL( EMPXMetadataEditorDlgCtrlIdTrackNumber,
                iMedia->ValueText( KMPXMediaMusicAlbumTrack ),
                KMPXSongDetailsTrackNumMin, KMPXSongDetailsTrackNumMax );

            // Fetch the year
            TInt64 year(0);
            if ( iMedia->IsSupported (KMPXMediaMusicYear ) )
                {
                year = iMedia->ValueTObjectL<TInt64>( KMPXMediaMusicYear );
                }
            TTime yearTime( year);
            iYear = yearTime.DateTime().Year ( );
            HBufC* yearBuf = HBufC::NewLC ( KMPXMaxTimeLength );
            yearBuf->Des().AppendNum ( iYear );
            SetControlNumberL ( EMPXMetadataEditorDlgCtrlIdYear, *yearBuf,
                    KMPXSongDetailsYearMin, KMPXSongDetailsYearMax );
            CleanupStack::PopAndDestroy ( yearBuf );

            // Fetch genre
            FetchGenreL();

            // Fetch the comment
            SetControlTextL( EMPXMetadataEditorDlgCtrlIdComment,
                iMedia->ValueText( KMPXMediaGeneralComment ), KNullDesC );

            // Fetch the song name
            TParsePtrC fileDetail( iMedia->ValueText( KMPXMediaGeneralUri ) );
            SetControlTextL( EMPXMetadataEditorDlgCtrlIdSongName,
                iMedia->ValueText( KMPXMediaGeneralTitle ), fileDetail.Name() );

            if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
                {
                // Fetch the composer
                SetControlTextL( EMPXMetadataEditorDlgCtrlIdComposer,
                    iMedia->ValueText( KMPXMediaMusicComposer ), KNullDesC );
                }

            PopulateFileDetailsL();            
            DrawNow();
            }
        else
            {
            // if error getting metadata, exit editor
            iCommonUiHelper->HandleErrorL( aError );
            if ( iIdle )
                {
                iIdle->Cancel();
                delete iIdle;
                iIdle = NULL;
                }
            iIdle = CIdle::NewL( CActive::EPriorityStandard );
            iIdle->Start( TCallBack( CMPXMetadataEditorDialog::DeferredExitCallback, this ) );
            }
        }
    else if ( iCurrentMediaLOp == EMPXMetadataEditorGetDrmInfo )
        {
        delete iDrmInfo;
        iDrmInfo = NULL;
        if ( aError == KErrNone )
            {
            iDrmInfo = CMPXMedia::NewL( aMedia );
            }
        else
            {
            iDrmInfo = CMPXMedia::NewL();
            }
        if ( iIdle )
            {
            iIdle->Cancel();
            delete iIdle;
            iIdle = NULL;
            }
        iIdle = CIdle::NewL( CActive::EPriorityStandard );
        iIdle->Start( TCallBack( CMPXMetadataEditorDialog::DeferredFileDetailsCallback, this ) );
        }
    else
        {
        // Should not reach here
        ASSERT( 0 );
        }
    iCurrentMediaLOp = EMPXMetadataEditorIdle;
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SaveFormDataL
// Save form data.
// ----------------------------------------------------------------------------
//
TBool CMPXMetadataEditorDialog::SaveFormDataL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SaveFormDataL" );
    if ( iMedia )
        {
        MPX_DEBUG1( "CMPXMetadataEditorDialog::SaveFormDataL Saving.." );
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL( media );

        TBool changed = EFalse;

        // common to both music and podcast
        // update local copy of media as well because getting media prop is slow
        changed = UpdateMediaObjectWithControlL( EMPXMetadataEditorDlgCtrlIdSongName,
            media, KMPXMediaGeneralTitle );

        changed = UpdateMediaObjectWithControlL( EMPXMetadataEditorDlgCtrlIdArtist,
            media, KMPXMediaMusicArtist ) || changed;

        changed = UpdateMediaObjectWithControlL( EMPXMetadataEditorDlgCtrlIdAlbum,
            media, KMPXMediaMusicAlbum ) || changed;

        changed = UpdateMediaObjectWithControlL( EMPXMetadataEditorDlgCtrlIdTrackNumber,
            media, KMPXMediaMusicAlbumTrack ) || changed;

        changed = UpdateMediaObjectWithControlL( EMPXMetadataEditorDlgCtrlIdComment,
            media, KMPXMediaGeneralComment ) || changed;

        // special handling needed for year
        CCoeControl* coeControl = ControlOrNull( EMPXMetadataEditorDlgCtrlIdYear );
        if ( coeControl )
            {
            CEikNumberEditor* control = static_cast<CEikNumberEditor*>( coeControl );
            if ( control )
                {
                TInt num = control->Number();
                if ( num != iYear )
                    {
                    MPX_DEBUG3( "CMPXMetadataEditorDialog::SaveFormDataL year changed from %d to %d", iYear, num );
                    TDateTime newYear;
                    newYear.Set( num, EJanuary, 0, 0, 0, 0, 0 );
                    TTime year( newYear );
                    iMedia->SetTObjectValueL<TInt64>(
                        KMPXMediaMusicYear, year.Int64() );
                    media->SetTObjectValueL<TInt64>(
                        KMPXMediaMusicYear, year.Int64() );
                    iYear = num;
                    changed = ETrue;
                    }
                else
                    {
                    MPX_DEBUG1( "CMPXMetadataEditorDialog::SaveFormDataL year not changed" );
                    }
                }
            }

        if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
            {
            changed = UpdateMediaObjectWithControlL( EMPXMetadataEditorDlgCtrlIdComposer,
                media, KMPXMediaMusicComposer ) || changed;
            }
        else if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
            {
            // no special handling, all covered in common
            }
        if ( changed )
            {
            // save it back to collection engine
            SaveMediaPropertiesL( media );
            MPX_DEBUG1( "CMPXMetadataEditorDialog::SaveFormDataL Saved" );
            }
#ifdef _DEBUG
        else
            {
            MPX_DEBUG1( "CMPXMetadataEditorDialog::SaveFormDataL All equal, not saved" );
            }
#endif
        CleanupStack::PopAndDestroy( media );
        }
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::DoNotSaveFormDataL
// Does not save form data.
// ----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::DoNotSaveFormDataL()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SetTitlePane
// Set Title Pane from descriptor.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::SetTitlePaneL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SetTitlePaneL" );
    // Saving the current title before it's overwritten
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();

    iTitlePane =
        static_cast<CAknTitlePane*>
            ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    const TDesC* text = iTitlePane->Text();
    iTitlePaneText = text->AllocL();

    HBufC* newText = NULL;
    if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
        {
        newText = StringLoader::LoadLC( R_MPX_CUI_SONG_DETAILS_TITLE );
        }
    else if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
        {
        newText = StringLoader::LoadLC( R_MPX_CUI_SONG_DETAILS_EPISODE );
        }

    // Update Title Pane
    if ( newText )
        {
        iTitlePane->SetTextL( *newText );
        }
    CleanupStack::PopAndDestroy( newText );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::RestoreTitlePane
// Restore Title Pane from the member data.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::RestoreTitlePaneL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::RestoreTitlePaneL" );
    // Update Title Pane
    iTitlePane->SetTextL( *iTitlePaneText );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SetNaviLabelL
// Set Navi Label from descriptor.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::SetNaviLabelL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SetNaviLabelL" );
    MAknEditingStateIndicator *editingStateIndicator = CAknEnv::Static()->EditingStateIndicator();
    CAknIndicatorContainer* indicContainerEditing = editingStateIndicator->IndicatorContainer();
    indicContainerEditing->SetIndicatorValueL( TUid::Uid( EAknNaviPaneEditorIndicatorMessageInfo), KNullDesC );
    // Saving the current title before it's overwritten
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();

    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    // store the latest navi pane
    iNaviDecorator = iNaviPane->Top();
    
    if(iNaviDecorator)
        {   
        // set the new navi label
        iEmptyNaviDecorator =
        iNaviPane->CreateNavigationLabelL(KNullDesC);

        iNaviPane->PushL( *iEmptyNaviDecorator );
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::RestoreNaviLabelL
// Restore Navi Label from the member data.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::RestoreNaviLabelL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::RestoreNaviLabelL" );
    iNaviPane->Pop( iEmptyNaviDecorator ); // pop out the empty navi
    if ( iNaviDecorator )
        {
        iNaviPane->PushL( *iNaviDecorator );
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::DynInitMenuPaneL
// Dynamically initialises a menu pane.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane)
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::DynInitMenuPaneL" );
    CAknForm::DynInitMenuPaneL(aResourceId, aMenuPane);

    if ( aResourceId == R_AVKON_FORM_MENUPANE )
        {
        if ( ( IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdGenre ) ||
            ( IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdLibrary ) )
            {
            aMenuPane->SetItemDimmed( EMPXCmdSongDetailsChange, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMPXCmdSongDetailsChange, ETrue );
            }
        aMenuPane->SetItemDimmed( EMPXCmdSongDetails, ETrue );
        
        // Dim out the Avkon menu items
        aMenuPane->SetItemDimmed( EAknFormCmdEdit, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
        }
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::ProcessCommandL
// Act on the menu selection if menu is showing.
// ----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::ProcessCommandL( TInt aCommandId )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::ProcessCommandL" );
    CAknForm::ProcessCommandL(aCommandId);

    switch (aCommandId)
        {
        case EMPXCmdSongDetailsChange:
            {
            if ( IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdGenre )
                {
                iPopup = static_cast<CAknPopupField*>
                    ( ControlOrNull( EMPXMetadataEditorDlgCtrlIdGenre ) );
                }
            else if ( IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdLibrary )
                {
                iPopup = static_cast<CAknPopupField*>
                    ( ControlOrNull( EMPXMetadataEditorDlgCtrlIdLibrary ) );
                }
            if ( iPopup )
                {
                iPopup->ActivateSelectionListL();
                }

            break;
            }
        case EMPXCmdSongDetails:
            {
            if ( iIsDrmProtected && !iDrmInfo )
                {
                CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL( cpath );
                if ( iParam )
                    {
                    TLex yearLex( iParam->Des() );
                    TInt lexToInt = NULL;
                    if ( yearLex.Val( lexToInt ) == KErrNone )
                        {
                        cpath->Set( lexToInt );
                        RArray<TMPXAttribute> attrs;
                        CleanupClosePushL(attrs);
                        attrs.Append( KMPXMediaGeneralUri );
                        attrs.Append( KMPXMediaDrmAll );
                        iCurrentMediaLOp = EMPXMetadataEditorGetDrmInfo;
                        iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
                        CleanupStack::PopAndDestroy( &attrs );
                        }
                    }
                CleanupStack::PopAndDestroy( cpath );
                }
            else
                {
                if( iMedia )
                    {
                    ViewFileDetailsPopupL();
                    }
                }
            break;
            }
        case EAknCmdHelp:
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
        default :
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::PopulateFileDetailsL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::PopulateFileDetailsL(
    CDesCArrayFlat* aHeadingsArray, CDesCArrayFlat* aDataArray )
    {

    MPX_FUNC( "CMPXMetadataEditorDialog::PopulateFileDetailsL" );

    TBool drmProtected( iMedia->ValueTObjectL<TBool>( KMPXMediaDrmProtected ) );
    MPX_DEBUG2( "CMPXMetadataEditorDialog::PopulateFileDetailsL drm protected: %d", drmProtected );

    if ( drmProtected )
        {
        MakeDrmItemsL( aHeadingsArray, aDataArray );
        }

    // Get filename
    const TDesC& uri = iMedia->ValueText( KMPXMediaGeneralUri );
    TParsePtrC parse(uri);

    AppendStringToArrayL( aDataArray, parse.Name() );

    // Get mime type
    const TDesC& mimeType = iMedia->ValueText( KMPXMediaGeneralMimeType );
    AppendStringToArrayL( aDataArray, mimeType );

    // Get duration
    TInt duration( iMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration ) );
    if ( duration > 0 )
        {
        // convert milliseconds to seconds
        duration = duration / KSecondInMilliseconds;
        CMPXCommonUiHelper::TMPXDuratDisplayMode durationMode =
            CMPXCommonUiHelper::EMPXDuratAuto;
        if ( duration > KOneHourInSeconds )
            {
            durationMode = CMPXCommonUiHelper::EMPXDuratHMS;
            }
        HBufC* stringBuf = iCommonUiHelper->DisplayableDurationL(
            duration, durationMode );
        CleanupStack::PushL( stringBuf );
        aDataArray->AppendL( *stringBuf );
        CleanupStack::PopAndDestroy( stringBuf );
        }
    else
        {
        aDataArray->AppendL( KNullDesC );
        }

    // Get bitrate
    TInt bitrate( iMedia->ValueTObjectL<TInt>( KMPXMediaAudioBitrate ) );
    if ( bitrate > 0 )
        {
        // convert bitrate to kpbs
        bitrate = bitrate / KMPXBitrateFactor;
        HBufC* bitrateText = StringLoader::LoadLC(
                        R_MPX_CUI_METADATAEDITOR_BITRATE_TXT, bitrate );
        aDataArray->AppendL( bitrateText->Des() );
        CleanupStack::PopAndDestroy( bitrateText );
        }
    else
        {
        aDataArray->AppendL( KNullDesC );
        }

    // Get sampling rate
    TInt samplingRateNum( ( TInt )iMedia->ValueTObjectL<TInt>( KMPXMediaAudioSamplerate ) );
    if ( samplingRateNum > 0 )
        {
        HBufC* stringBuf = HBufC::NewLC( KMPXFileDetailsMaxBufferLen );
        TPtr stringBufPtr = stringBuf->Des();
        stringBufPtr.AppendNum( samplingRateNum );
        HBufC* samplingrate = StringLoader::LoadLC(
                        R_MPX_CUI_METADATAEDITOR_SAMPLINGRATE_TXT, stringBufPtr );
        aDataArray->AppendL( samplingrate->Des() );
        CleanupStack::PopAndDestroy( samplingrate );
        CleanupStack::PopAndDestroy( stringBuf );
        }
    else
        {
        aDataArray->AppendL( KNullDesC );
        }

    // Get size, not stored in collection, have to use RFile
    //
    RFs& fs = CEikonEnv::Static()->FsSession();
    RFile file;
    TInt sizeNum(0);
    TInt err ( file.Open( fs, uri, EFileRead | EFileShareReadersOrWriters ) );
    if ( err == KErrNone )
        {
        file.Size(sizeNum);
        file.Close();
        }

    if ( sizeNum > 0 )
        {
        HBufC* stringBuf = iCommonUiHelper->UnitConversionL( sizeNum );
        CleanupStack::PushL( stringBuf );
        aDataArray->AppendL( *stringBuf );
        CleanupStack::PopAndDestroy( stringBuf );
        }
    else
        {
        aDataArray->AppendL( KNullDesC );
        }

    if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
        {
        PopulatePodcastFileDetailsL( aHeadingsArray, aDataArray );
        }

    // Get modified
    if ( iMedia->IsSupported( KMPXMediaGeneralDate ) )
        {
        TInt64 timeInt( ( TInt64 )iMedia->ValueTObjectL<TInt64>(
            KMPXMediaGeneralDate ) );
        TTime time( timeInt );
        ConvertToLocalTimeL(time);
        HBufC* modDateTime = HBufC::NewLC(
            KMPXMaxTimeLength + KMPXDurationDisplayResvLen );
        HBufC* format = StringLoader::LoadLC(
            R_QTN_DATE_USUAL_WITH_ZERO );
        TPtr modDatePtr = modDateTime->Des();
        MPX_TRAPD( err, time.FormatL( modDatePtr, *format ) );
        CleanupStack::PopAndDestroy( format );
        if (err != KErrNone)
            {
            aDataArray->AppendL( KNullDesC );
            }
        else
            {
            format = StringLoader::LoadLC(
                R_QTN_TIME_USUAL_WITH_ZERO );
            HBufC* modTime = HBufC::NewLC(
                format->Length() + KMPXDurationDisplayResvLen );
            TPtr modTimePtr = modTime->Des();
            MPX_TRAPD( err, time.FormatL( modTimePtr, *format ) );
            if ( err != KErrNone )
                {
                aDataArray->AppendL( KNullDesC );
                }
            else
                {
                modDatePtr.Append( KMPXSpace );
                modDatePtr.Append( modTimePtr );
                aDataArray->AppendL( modDatePtr );
                }
            CleanupStack::PopAndDestroy( modTime );
            CleanupStack::PopAndDestroy( format );
            }
        CleanupStack::PopAndDestroy( modDateTime );
        }
    else
        {
        aDataArray->AppendL( KNullDesC );
        }

    // Get filename
    AppendStringToArrayL( aDataArray, iMedia->ValueText(
        KMPXMediaGeneralCopyright ) );

    // Get URL
    AppendStringToArrayL( aDataArray, iMedia->ValueText(
        KMPXMediaMusicURL ) );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::PopulatePodcastFileDetailsL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::PopulatePodcastFileDetailsL(
    CDesCArrayFlat* /*aHeadingsArray*/, CDesCArrayFlat* aDataArray )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::PopulatePodcastFileDetailsL" );
    // Get last playback position

    TInt lastPbPosition( ( TInt )iMedia->ValueTObjectL<TInt>(
        KMPXMediaGeneralLastPlaybackPosition ) );

    if ( lastPbPosition > 0 )
        {
        // convert milliseconds to seconds
        lastPbPosition = lastPbPosition / KSecondInMilliseconds;
        CMPXCommonUiHelper::TMPXDuratDisplayMode lastPbPositionMode =
            CMPXCommonUiHelper::EMPXDuratAuto;
        if ( lastPbPosition > KOneHourInSeconds )
            {
            lastPbPositionMode = CMPXCommonUiHelper::EMPXDuratHMS;
            }
        HBufC* stringBuf = iCommonUiHelper->DisplayableDurationL(
            lastPbPosition, lastPbPositionMode );
        CleanupStack::PushL( stringBuf );
        aDataArray->AppendL( *stringBuf );
        CleanupStack::PopAndDestroy( stringBuf );
        }
    else if(lastPbPosition == 0 &&
            iMedia->IsSupported(KMPXMediaGeneralPlayCount) &&
            iMedia->ValueTObjectL<TInt>(KMPXMediaGeneralPlayCount) > 0 )
        {
        HBufC* stringBuf = StringLoader::LoadLC( R_MPX_CUI_METADATAEDITOR_PLAYBACK_COMPLETE );
        aDataArray->AppendL( *stringBuf );
        CleanupStack::PopAndDestroy( stringBuf );

        }
    else
        {
        aDataArray->AppendL( KNullDesC );
        }


    // Get published
    if ( iMedia->IsSupported( TMPXAttribute( KMPXMediaIdPodcast,
            EMPXMediaPodcastPubDate ) ) )
        {
        TInt64 timeInt( ( TInt64 )iMedia->ValueTObjectL<TInt64>(
            TMPXAttribute( KMPXMediaIdPodcast, EMPXMediaPodcastPubDate ) ) );
        TTime time( timeInt );
        ConvertToLocalTimeL(time);
        HBufC* modDateTime = HBufC::NewLC(
            KMPXMaxTimeLength + KMPXDurationDisplayResvLen );
        HBufC* format = StringLoader::LoadLC(
            R_QTN_DATE_USUAL_WITH_ZERO );
        TPtr modDatePtr = modDateTime->Des();
        MPX_TRAPD( err, time.FormatL( modDatePtr, *format ) );
        CleanupStack::PopAndDestroy( format );
        if (err != KErrNone ||
            time == 0)
            {
            aDataArray->AppendL( KNullDesC );
            }
        else
            {
            format = StringLoader::LoadLC(
                R_QTN_TIME_USUAL_WITH_ZERO );
            HBufC* modTime = HBufC::NewLC(
                format->Length() + KMPXDurationDisplayResvLen );
            TPtr modTimePtr = modTime->Des();
            MPX_TRAPD( err, time.FormatL( modTimePtr, *format ) );
            if ( err != KErrNone )
                {
                aDataArray->AppendL( KNullDesC );
                }
            else
                {
                modDatePtr.Append( KMPXSpace );
                modDatePtr.Append( modTimePtr );
                aDataArray->AppendL( modDatePtr );
                }
            CleanupStack::PopAndDestroy( modTime );
            CleanupStack::PopAndDestroy( format );
            }
        CleanupStack::PopAndDestroy( modDateTime );
        }
    else
        {
        aDataArray->AppendL( KNullDesC );
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::MakeDrmItemsL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::MakeDrmItemsL(
    CDesCArrayFlat* aHeadingsArray, CDesCArrayFlat* aDataArray )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::MakeDrmItemsL" );
    if (iDrmDetails )
        {
        TInt itemCount( 0);
        if ( iDrmInfo->IsSupported( KMPXMediaDrmType ) )
            {
            MPX_DEBUG2( "CMPXMetadataEditorDialog::MakeDrmItemsL drm type: %d",
                    iDrmInfo->ValueTObjectL<TInt>( KMPXMediaDrmType ) );
            TInt rightStatus( iDrmInfo->ValueTObjectL<TInt>( KMPXMediaDrmRightsStatus ) );
            HBufC* rights( StringLoader::LoadLC( R_QTN_DRM_MGR_DET2_PLAY ) );
            HBufC* heading( NULL);
            HBufC* value( NULL);

            switch ( rightStatus )
                {
                case EMPXDrmRightsFull:
                    {
                    // status
                    heading = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_STAT );
                    value = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_VALID );
                    aHeadingsArray->InsertL ( itemCount, heading->Des ( ) );
                    aDataArray->InsertL ( itemCount, value->Des ( ) );
                    CleanupStack::PopAndDestroy ( value );
                    CleanupStack::PopAndDestroy ( heading );
                    itemCount++;

                    // full rights
                    heading = StringLoader::LoadLC (
                        R_QTN_DRM_MGR_DET_FULL_X, *rights );
                    value = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_UNLIMITED );
                    aHeadingsArray->InsertL ( itemCount, heading->Des ( ) );
                    aDataArray->InsertL ( itemCount, value->Des ( ) );
                    CleanupStack::PopAndDestroy ( value );
                    CleanupStack::PopAndDestroy ( heading );
                    itemCount++;
                    break;
                    }
                case EMPXDrmRightsRestricted:
                case EMPXDrmRightsPreview:
                    {
                    // status
                    heading = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_STAT );
                    value = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_VALID );
                    aHeadingsArray->InsertL ( itemCount, heading->Des ( ) );
                    aDataArray->InsertL ( itemCount, value->Des ( ) );
                    CleanupStack::PopAndDestroy ( value );
                    CleanupStack::PopAndDestroy ( heading );
                    itemCount++;
                    // rights
                    MakeRestrictedDrmItemsL(
                        rights, itemCount, aHeadingsArray, aDataArray );
                    break;
                    }
                case EMPXDrmRightsMissing:
                    {
                    // status
                    heading = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_STAT );
                    value = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_EXP );
                    aHeadingsArray->InsertL ( itemCount, heading->Des ( ) );
                    aDataArray->InsertL ( itemCount, value->Des ( ) );
                    CleanupStack::PopAndDestroy ( value );
                    CleanupStack::PopAndDestroy ( heading );
                    itemCount++;
                    break;
                    }
                case EMPXDrmRightsExpired:
                    {
                    // status
                    heading = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_STAT );
                    value = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_EXP );
                    aHeadingsArray->InsertL ( itemCount, heading->Des ( ) );
                    aDataArray->InsertL ( itemCount, value->Des ( ) );
                    CleanupStack::PopAndDestroy ( value );
                    CleanupStack::PopAndDestroy ( heading );
                    itemCount++;
                    // make rest of the drm items
                    MakeRestrictedDrmItemsL(
                        rights, itemCount, aHeadingsArray, aDataArray );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }

            heading = NULL;
            value = NULL;

            // sending
            heading = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_CS );
            TBool forwardAllowed( iDrmInfo->ValueTObjectL<TBool>( KMPXMediaDrmSendingAllowed ) );
            if ( forwardAllowed )
                {
                value = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_ALLOWED );
                }
            else
                {
                value = StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_FORBID );
                }
            aHeadingsArray->InsertL ( itemCount, heading->Des ( ) );
            aDataArray->InsertL ( itemCount, value->Des ( ) );
            CleanupStack::PopAndDestroy ( value );
            CleanupStack::PopAndDestroy ( heading );
            CleanupStack::PopAndDestroy ( rights ); // heading, value, rights
            }
        else
            {
            // if error getting drm info, drm type will not be valid
            // by default assume the rights are invalid (since we can't read it)
            MPX_DEBUG1( "CMPXMetadataEditorDialog::MakeDrmItemsL unable to get drm data" );
            HBufC* heading( StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_STAT ));
            HBufC* value( StringLoader::LoadLC ( R_QTN_DRM_MGR_DET_INVALID ));
            aHeadingsArray->InsertL ( 0, heading->Des ( ) );
            aDataArray->InsertL ( 0, value->Des ( ) );
            CleanupStack::PopAndDestroy ( value );
            CleanupStack::PopAndDestroy ( heading );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(
    HBufC* aRights, TInt& aCurrentIndex,
    CDesCArrayFlat* aHeadingsArray, CDesCArrayFlat* aDataArray )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL" );
    TMPXMediaDrmRightsType expireConstraint( EMPXDrmRightsTypeNoRestriction );
    if ( iDrmInfo->IsSupported( KMPXMediaDrmRightsType ) )
        {
        expireConstraint = iDrmInfo->ValueTObjectL<TMPXMediaDrmRightsType>(
            KMPXMediaDrmRightsType );
        }
    MPX_DEBUG2("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): ExpireConstraint = %d", expireConstraint);
    TInt64 intervalTime( KErrNotFound );
    if ( iDrmInfo->IsSupported( KMPXMediaDrmIntervalStartTime ) )
        {
        MPX_DEBUG1("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): Interval Start Time");
        intervalTime = (TInt64)iDrmInfo->ValueTObjectL<TInt64>( KMPXMediaDrmIntervalStartTime );
        }
    HBufC* heading = NULL;
    HBufC* value = NULL;
    // acculated time 
    if ( expireConstraint ==  EMPXDrmRightsTypeAccTime )
      	{
      	MPX_DEBUG1("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): AccTime based");
        heading = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_ACCUM_TIME_LEFT  );

        TTimeIntervalSeconds  accTime = iDrmInfo->ValueTObjectL<TTimeIntervalSeconds>(  KMPXMediaDrmAccumulatedTime );
        TInt years( 0 );
        TInt months( 0 );
        TInt days( 0 );
        TInt hours( 0 );
        TInt minutes( 0 );
        TInt seconds( 0 );
        SplitTime( accTime, years, months, days, hours, minutes, seconds );
        value = AddPartsOfTimeLC( years, months, days, hours, minutes, seconds );      
        
        TPtr text = value->Des();
        LanguageSpecificNumberConversion( text );   
      	aHeadingsArray->InsertL( aCurrentIndex, heading->Des() );  
        aDataArray->InsertL( aCurrentIndex, value->Des() ); 
        
        CleanupStack::PopAndDestroy( value );
        CleanupStack::PopAndDestroy( heading );         
        aCurrentIndex++;   
      	}

    // count
    if ( expireConstraint == EMPXDrmRightsTypeCount ||
         expireConstraint == EMPXDrmRightsTypeTimeCount )
        {
        MPX_DEBUG1("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): Count based");
        heading = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_UTL_X, *aRights );

        TInt counts( iDrmInfo->ValueTObjectL<TInt>( KMPXMediaDrmCount ) );
        if ( counts == 1 )
            {
            value = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_1_COUNT );
            }
        else
            {
            value = StringLoader::LoadLC(
                R_QTN_DRM_MGR_DET_N_COUNTS, counts );
            }
        TPtr text = value->Des();
        LanguageSpecificNumberConversion( text );
        aHeadingsArray->InsertL( aCurrentIndex, heading->Des() );
        aDataArray->InsertL( aCurrentIndex, value->Des() );
        CleanupStack::PopAndDestroy( value );
        CleanupStack::PopAndDestroy( heading );
        aCurrentIndex++;
        }

    if ( expireConstraint == EMPXDrmRightsTypeTime ||
         expireConstraint == EMPXDrmRightsTypeTimeCount ||
         ( expireConstraint == EMPXDrmRightsTypeInterval &&
         intervalTime > 0 ) )
        {
        MPX_DEBUG1("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): Time based");
        HBufC* dateFormatString = StringLoader::LoadLC(
                R_QTN_DATE_USUAL_WITH_ZERO );
        HBufC* timeFormatString = StringLoader::LoadLC( R_QTN_TIME_USUAL );

        // from
        if ( iDrmInfo->IsSupported( KMPXMediaDrmStartTime ) )
            {
            MPX_DEBUG1("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): Start time found");
            TTime from( iDrmInfo->ValueTObjectL<TInt64>( KMPXMediaDrmStartTime ) );
            heading = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_RVF_X, *aRights );
            HBufC* datePointer = HBufC::NewLC( KMaxFileName );
            TPtr date = datePointer->Des();
            from.FormatL( date, *dateFormatString );
            TBuf<KMPXMaxTimeLength> time;
            from.FormatL( time, *timeFormatString );
            date.Append( KMPXSpace );
            date.Append( time );
            LanguageSpecificNumberConversion( date );
            aHeadingsArray->InsertL( aCurrentIndex, heading->Des() );
            aDataArray->InsertL( aCurrentIndex, date );
            CleanupStack::PopAndDestroy( datePointer );
            CleanupStack::PopAndDestroy( heading );
            aCurrentIndex++;
            }

        // until
        if ( iDrmInfo->IsSupported( KMPXMediaDrmEndTime ) )
            {
            MPX_DEBUG1("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): End time found");
            TTime until( iDrmInfo->ValueTObjectL<TInt64>( KMPXMediaDrmEndTime ) );
            heading = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_RVT_X, *aRights );
            HBufC* dateToPntr = HBufC::NewLC( KMaxFileName );
            TPtr dateTo = dateToPntr->Des();
            until.FormatL( dateTo, *dateFormatString );
            TBuf<KMPXMaxTimeLength> timeTo;
            until.FormatL( timeTo, *timeFormatString );
            dateTo.Append( KMPXSpace );
            dateTo.Append( timeTo );
            LanguageSpecificNumberConversion( dateTo );
            aHeadingsArray->InsertL( aCurrentIndex, heading->Des() );
            aDataArray->InsertL( aCurrentIndex, dateTo );
            CleanupStack::PopAndDestroy( dateToPntr );
            CleanupStack::PopAndDestroy( heading );
            aCurrentIndex++;
            }
        CleanupStack::PopAndDestroy( timeFormatString );
        CleanupStack::PopAndDestroy( dateFormatString );
        }
    else if ( expireConstraint == EMPXDrmRightsTypeInterval &&
         intervalTime < 0 )
        {
        MPX_DEBUG1("CMPXMetadataEditorDialog::MakeRestrictedDrmItemsL(): Time not activated");
        // times not activated
        heading = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_UTS_X, *aRights );
        value  = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_NOT_ACT );
        aHeadingsArray->InsertL( aCurrentIndex, heading->Des() );
        aDataArray->InsertL( aCurrentIndex, value->Des() );
        CleanupStack::PopAndDestroy( value );
        CleanupStack::PopAndDestroy( heading );
        aCurrentIndex++;

        // query what the current time is
        TTime currentTime;
        currentTime.HomeTime();
        // calculate rights end time
        TTimeIntervalSeconds interval = iDrmInfo->ValueTObjectL<TInt64>( KMPXMediaDrmInterval );
        TTime endTime( currentTime.Int64() + (TInt64) interval.Int() * KSecondInMicroSeconds );
        TBuf<KMPXMaxVal> val;
        TInt count = 0;

        // years
        TTimeIntervalYears years = endTime.YearsFrom( currentTime );
        if ( years.Int() )
            {
            AddSinglePartOfTimeL( years.Int(),
                R_QTN_DRM_NBR_OF_YEARS_ONE,
                R_QTN_DRM_NBR_OF_YEARS_ONE_FINAL,
                R_QTN_DRM_NBR_OF_YEARS_TWO_FOUR,
                R_QTN_DRM_NBR_OF_YEARS_FIVE_ZERO,
                value );
            count++;
            val.Append( *value );
            endTime -= years;
            CleanupStack::PopAndDestroy( value );
            }

        // months
        TTimeIntervalMonths months = endTime.MonthsFrom( currentTime );
        if ( months.Int() )
            {
            AddSinglePartOfTimeL( months.Int(),
                R_QTN_DRM_NBR_OF_MONTHS_ONE,
                0,
                R_QTN_DRM_NBR_OF_MONTHS_TWO_FOUR,
                R_QTN_DRM_NBR_OF_MONTHS_FIVE_ZERO,
                value );
             if ( count )
                {
                val.Append( KMPXSpace );
                }

            count++;
            val.Append( *value );
            endTime -= months;
            CleanupStack::PopAndDestroy( value );
            }

        // days
        TTimeIntervalDays days = endTime.DaysFrom( currentTime );
        if ( days.Int() && count < 2 )
            {
            AddSinglePartOfTimeL( days.Int(),
                R_QTN_DRM_NBR_OF_DAYS_ONE,
                R_QTN_DRM_NBR_OF_DAYS_ONE_FINAL,
                R_QTN_DRM_NBR_OF_DAYS_TWO_FOUR,
                R_QTN_DRM_NBR_OF_DAYS_FIVE_ZERO,
                value );
            if ( count )
                {
                val.Append( KMPXSpace );
                }

            val.Append( *value );
            endTime -= days;
            CleanupStack::PopAndDestroy( value );
            count++;
            }

        // hours
        TTimeIntervalHours hours;
        TInt ret = endTime.HoursFrom( currentTime, hours );
        if ( ret == KErrNone && count < 2 && hours.Int() )
            {
            AddSinglePartOfTimeL( hours.Int(),
                R_QTN_DRM_NBR_OF_HOURS_ONE,
                R_QTN_DRM_NBR_OF_HOURS_ONE_FINAL,
                R_QTN_DRM_NBR_OF_HOURS_TWO_FOUR,
                R_QTN_DRM_NBR_OF_HOURS_FIVE_ZERO,
                value );
            if ( count )
                {
                val.Append( KMPXSpace );
                }

            val.Append( *value );
            endTime -= hours;
            CleanupStack::PopAndDestroy( value );
            count++;
            }

        // mins
        TTimeIntervalMinutes minutes;
        ret = endTime.MinutesFrom( currentTime, minutes );
        if ( ret == KErrNone && count < 2 && minutes.Int() )
            {
            AddSinglePartOfTimeL( minutes.Int(),
                R_QTN_DRM_NBR_OF_MINS_ONE,
                R_QTN_DRM_NBR_OF_MINS_ONE_FINAL,
                R_QTN_DRM_NBR_OF_MINS_TWO_FOUR,
                R_QTN_DRM_NBR_OF_MINS_FIVE_ZERO,
                value );
            if ( count )
                {
                val.Append( KMPXSpace );
                }

            val.Append( *value );
            endTime -= minutes;
            CleanupStack::PopAndDestroy( value );
            count++;
            }

        // seconds
        TTimeIntervalSeconds seconds;
        ret = endTime.SecondsFrom( currentTime,seconds );
        if ( ret == KErrNone && count < 2 && seconds.Int() )
            {
            AddSinglePartOfTimeL( seconds.Int(),
                R_QTN_DRM_NBR_OF_SECS_ONE,
                R_QTN_DRM_NBR_OF_SECS_ONE_FINAL,
                R_QTN_DRM_NBR_OF_SECS_TWO_FOUR,
                R_QTN_DRM_NBR_OF_SECS_FIVE_ZERO,
                value );

            if ( count )
                {
                val.Append( KMPXSpace );
                }

            val.Append( *value );
            CleanupStack::PopAndDestroy(); // value
            count++;
            }

        heading = StringLoader::LoadLC( R_QTN_DRM_MGR_DET_UDL_X, *aRights );
        LanguageSpecificNumberConversion( val );
        aHeadingsArray->InsertL( aCurrentIndex, heading->Des() );
        aDataArray->InsertL( aCurrentIndex, val );
        CleanupStack::PopAndDestroy( heading );
        aCurrentIndex++;
        }
    }
  
// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::AddPartsOfTimeLC
// -----------------------------------------------------------------------------
//
HBufC* CMPXMetadataEditorDialog::AddPartsOfTimeLC( TInt aIntYrs, TInt aIntMon, TInt aIntDay, 
                         TInt aIntHrs, TInt aIntMin, TInt aIntSec )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::AddPartsOfTimeLC" );   
    // Only the two most meaningful data will be showed
    TInt numOfData( 0 );
    const TInt KMaxDataItems = 2;

    CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( KMaxDataItems );
    CleanupStack::PushL( strings );

    if ( aIntYrs > 0 )
        {
        AddSinglePartOfTimeL( aIntYrs, 
            R_QTN_DRM_NBR_OF_YEARS_ONE,
            R_QTN_DRM_NBR_OF_YEARS_ONE_FINAL,
            R_QTN_DRM_NBR_OF_YEARS_TWO_FOUR,
            R_QTN_DRM_NBR_OF_YEARS_FIVE_ZERO,
            strings );
        numOfData++;
        }

    if ( aIntMon > 0 )
        {
        //  Second type not provided because 11 is the maximum
        AddSinglePartOfTimeL( aIntMon, 
            R_QTN_DRM_NBR_OF_MONTHS_ONE,
            0,       
            R_QTN_DRM_NBR_OF_MONTHS_TWO_FOUR, 
            R_QTN_DRM_NBR_OF_MONTHS_FIVE_ZERO, 
            strings );
        numOfData++;
        }

    // Only if years or months were missing
    if ( aIntDay > 0 && numOfData < KMaxDataItems )
        {
        AddSinglePartOfTimeL( aIntDay, 
            R_QTN_DRM_NBR_OF_DAYS_ONE,
            R_QTN_DRM_NBR_OF_DAYS_ONE_FINAL,
            R_QTN_DRM_NBR_OF_DAYS_TWO_FOUR, 
            R_QTN_DRM_NBR_OF_DAYS_FIVE_ZERO, 
            strings );
        numOfData++;
        }

    if ( aIntHrs > 0 && numOfData < KMaxDataItems )
        {
        AddSinglePartOfTimeL( aIntHrs, 
            R_QTN_DRM_NBR_OF_HOURS_ONE, 
            R_QTN_DRM_NBR_OF_HOURS_ONE_FINAL,
            R_QTN_DRM_NBR_OF_HOURS_TWO_FOUR, 
            R_QTN_DRM_NBR_OF_HOURS_FIVE_ZERO, 
            strings );
        numOfData++;
        }

    if ( aIntMin > 0 && numOfData < KMaxDataItems )
        {
        AddSinglePartOfTimeL( aIntMin, 
            R_QTN_DRM_NBR_OF_MINS_ONE,
            R_QTN_DRM_NBR_OF_MINS_ONE_FINAL,
            R_QTN_DRM_NBR_OF_MINS_TWO_FOUR, 
            R_QTN_DRM_NBR_OF_MINS_FIVE_ZERO, 
            strings );
        numOfData++;
        }

    // If interval is 0, then it shows "0 seconds" anyway
    if ( ( aIntSec > 0 && numOfData < KMaxDataItems ) || numOfData == 0 )
        {
        AddSinglePartOfTimeL( aIntSec, 
            R_QTN_DRM_NBR_OF_SECS_ONE, 
            R_QTN_DRM_NBR_OF_SECS_ONE_FINAL,
            R_QTN_DRM_NBR_OF_SECS_TWO_FOUR, 
            R_QTN_DRM_NBR_OF_SECS_FIVE_ZERO, 
            strings );
        numOfData++;
        }

    HBufC* stringHolder = NULL;
    if ( numOfData == 1 )
        {
        stringHolder = StringLoader::LoadL( R_QTN_DRM_MGR_DET_INTER,
            strings->MdcaPoint(0) );
        }
    else
        {
        stringHolder = StringLoader::LoadL( R_QTN_DRM_MGR_DET_INTER_TWO,
            *strings );
        }

    CleanupStack::PopAndDestroy( strings );
    
    CleanupStack::PushL( stringHolder );

    return stringHolder;
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::AddSinglePartOfTimeL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::AddSinglePartOfTimeL( TInt aNumOfElements,
        TInt aResourceIdSingle,
        TInt aResourceIdOneFinal, 
        TInt aResourceIdTwoFour,
        TInt aResourceIdFiveZero, 
        CDesCArrayFlat* aStrings )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::AddSinglePartOfTimeL" );   
   
    HBufC* stringHolder = NULL;
    TInt finalOneDigit( aNumOfElements % KDigitTen );
    TInt finalTwoDigits( aNumOfElements % KDigitHundred );

    if ( aNumOfElements == 1 )
        {
        stringHolder = StringLoader::LoadLC( aResourceIdSingle );
        }
    else if ( finalOneDigit == 1 && finalTwoDigits != KDigitEleven )
        {
        stringHolder = StringLoader::LoadLC( aResourceIdOneFinal, 
            aNumOfElements );
        }
    else if ( finalOneDigit == 0 || 
             ( finalOneDigit >= KDigitFive && finalOneDigit <= KDigitNine ) ||
             ( finalTwoDigits >= KDigitEleven && finalTwoDigits <= KDigitFourTeen ) )
        {
        stringHolder = StringLoader::LoadLC( aResourceIdFiveZero,
            aNumOfElements );
        }
    else
        {
        stringHolder = StringLoader::LoadLC( aResourceIdTwoFour,
            aNumOfElements );
        }

    if ( aStrings )
        {
        aStrings->AppendL( *stringHolder );
        }

    CleanupStack::PopAndDestroy ( stringHolder );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SplitTime
// 
// -----------------------------------------------------------------------------
//

void CMPXMetadataEditorDialog::SplitTime( const TTimeIntervalSeconds& aInterval,
        TInt& aIntYrs, TInt& aIntMon, TInt& aIntDay,
        TInt& aIntHrs, TInt& aIntMin, TInt& aIntSec )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SplitTime" );       
    const TInt KSecsInMin( 60 );
    const TInt KSecsInHour( KSecsInMin * 60 );
    const TInt KSecsInDay( KSecsInHour * 24 );

    // includes leap year day

    const TInt KMaxDaysInMonths[] = {
    		KMaxDaysInOneMonths,
    		KMaxDaysInTwoMonths,
    		KMaxDaysInThreeMonths,
    		KMaxDaysInFourMonths,
    		KMaxDaysInFiveMonths,
    		KMaxDaysInSixMonths,
    		KMaxDaysInSevenMonths,
    		KMaxDaysInEightMonths,
    		KMaxDaysInNineMonths,
    		KMaxDaysInTenMonths,
    		KMaxDaysInElevenMonths,
    		KMaxDaysInEightMonths }; 

    // calculate full days
    TInt temp( aInterval.Int() / KSecsInDay );

    // calculate full years, calculate without leap year for user to get the 
    // longest time possible
    aIntYrs = temp / ( KMaxDaysInMonths[KDigitEleven] - 1 );

    // calc remainder days
    temp = temp % ( KMaxDaysInMonths[KDigitEleven] - 1 );

    aIntMon = 0;

    TInt i( 0 );
    if ( temp >= KMaxDaysInMonths[0] )
        {
        for ( i = 0; i < KDigitEleven; i++ )
            {
            // found correct amount of months
            if ( temp >= KMaxDaysInMonths[i] && temp < KMaxDaysInMonths[i+1] )
                {
                // i now contains amount of full months (+1 because of table index)
                aIntMon = i + 1;
                break;
                }
            }
        }

    // calc remainder days = allSecs - secsInFullYears - secsInFullMonts
    if( temp >= KMaxDaysInMonths[i] )
        {
        aIntDay = temp - KMaxDaysInMonths[i];
        }
    else
        {
        aIntDay = temp;
        }

    // calculate remainder secs
    temp = aInterval.Int() % KSecsInDay;

    aIntHrs = temp / KSecsInHour;

    // calculate remainder secs
    temp = temp % KSecsInHour;

    aIntMin = temp / KSecsInMin;

    // calculate remainder secs
    aIntSec = temp % KSecsInMin;
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::AddSinglePartOfTimeL
// Language specific time formats
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::AddSinglePartOfTimeL(
    TInt aNumOfElements,
    TInt aResourceIdSingle,
    TInt aResourceIdOneFinal,
    TInt aResourceIdTwoFour,
    TInt aResourceIdFiveZero,
    HBufC*& aStrings )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::AddSinglePartOfTimeL" );
    TInt finalOneDigit = aNumOfElements % KMPXOneDigit;
    TInt finalTwoDigits = aNumOfElements % KMPXTwoDigits;

    if ( aNumOfElements == 1 )
        {
        aStrings = StringLoader::LoadLC( aResourceIdSingle );
        }
    else if ( finalOneDigit == 1 && finalTwoDigits != KDigitEleven )
        {
        // Used for period of years ending with 1 from 21 (21, 31, 41, etc.)
        aStrings = StringLoader::LoadLC( aResourceIdOneFinal, aNumOfElements );
        }
    else if ( finalOneDigit == 0 ||
            ( finalOneDigit >= KDigitFive && finalOneDigit <= KDigitNine ) ||
            ( finalTwoDigits >= KDigitEleven && finalTwoDigits <= KDigitFourTeen ) )
        {
        // Used for period of minutes ending from 5 to 0 plus range between 11
        // and 14 (5-20, 25-30, 35-40, 45-50, 53-59)
        aStrings = StringLoader::LoadLC( aResourceIdFiveZero, aNumOfElements );
        }
    else
        {
        // Used for period of minutes ending from 2 to 4, excluded 12-14 (2-4,
        // 22-24, 32-34, 42-44, 52-54)
        aStrings = StringLoader::LoadLC( aResourceIdTwoFour, aNumOfElements );
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::LanguageSpecificNumberConversion
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::LanguageSpecificNumberConversion( TDes& aText ) const
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::LanguageSpecificNumberConversion" );
    if ( AknTextUtils::DigitModeQuery( AknTextUtils::EDigitModeShownToUser ) )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aText );
        }
    }


// ----------------------------------------------------------------------------
// Converts the time value to localtime
// ----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::ConvertToLocalTimeL(TTime& aTime )
    {
    RTz timeConverter;
    CleanupClosePushL(timeConverter);
    User::LeaveIfError( timeConverter.Connect() );
    User::LeaveIfError( timeConverter.ConvertToLocalTime( aTime ) );
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::AddItemToListBoxL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::AddItemToListBoxL(
    const TDesC& aHeading,
    const TDesC& aValue,
    CDesCArray* aItemArray )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::AddItemToListBoxL" );
    CDesCArrayFlat* items = new (ELeave) CDesCArrayFlat( 2 );
    CleanupStack::PushL( items );
    items->AppendL( aHeading ); //First string (heading)
    items->AppendL( aValue );   //Second string (value)
    HBufC* headingAndValue =
            StringLoader::LoadLC( R_MPX_CUI_METADATAEDITOR_FILE_DETAILS_ROW_FORMAT, *items );

    aItemArray->AppendL( *headingAndValue );
    CleanupStack::PopAndDestroy( headingAndValue );
    CleanupStack::PopAndDestroy( items );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::HandleControlStateChangeL
// Handle State Changes in Genre popup.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleControlStateChangeL( TInt aControlId )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::HandleControlStateChangeL" );
    if( !iTryingExit )
        {
        if ( aControlId == EMPXMetadataEditorDlgCtrlIdGenre )
            {
            // check to see if the genre pop has item custom selected
            if ( iTextValues->CurrentValueIndex() == 0 )
                {
                HBufC* genreName = HBufC::NewLC( KMPXSongDetailsTextMaxLen );
                TPtr genrePtr = genreName->Des();
                CAknTextQueryDialog* queryDlg =
                    CAknTextQueryDialog::NewL( genrePtr );
                CleanupStack::PushL( queryDlg );
                HBufC* promptTxt = NULL;
                if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
                    {
                    promptTxt = StringLoader::LoadLC( R_MPX_CUI_CUSTOM_GENRE_RENAME_TEXT );
                    }
                else
                    {
                    promptTxt = StringLoader::LoadLC( R_MPX_CUI_CUSTOM_CATEGORY_RENAME_TEXT );
                    }
                queryDlg->SetPromptL( *promptTxt );
                queryDlg->SetPredictiveTextInputPermitted( ETrue );
                CleanupStack::PopAndDestroy( promptTxt );
                CleanupStack::Pop( queryDlg );
                if ( queryDlg->ExecuteLD( R_MPX_CUI_CUSTOM_GENRE_NAME_QUERY_DLG ) )
                    {
                    TBool isHandled = EFalse;
                    TBool tryExit = EFalse;
                    if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
                        {
                        // in music libray, if genre is set to podcast, ask user
                        // to confirm move to podcast database
                        HBufC* podcastText = StringLoader::LoadLC( R_MPX_QTN_NMP_GENRE_PODCAST );
                        if ( genreName->Des().CompareF( *podcastText ) == 0 &&
                             !iDisablePodcasting )
                            {
                            isHandled = ETrue;
                            // genre is set to podcast
                            if ( PromptForLibraryChangeL( EMPXMetadataEditorDlgPodcast ) )
                                {
                                MPX_DEBUG1( "CMPXMetadataEditorDialog::HandleControlStateChangeL Move to a different library" );
                                iCollectionUiHelper->MoveL( iMedia, TUid::Uid(EMPXCollectionPluginPodCast) , this);
                                tryExit = ETrue;
                                iTryingExit = ETrue;
                                }
                            }
                        CleanupStack::PopAndDestroy( podcastText );
                        }
                    if ( !isHandled )
                        {
                        // ok selected
                        CMPXMedia* media = CMPXMedia::NewL();
                        CleanupStack::PushL( media );
                        iMedia->SetTextValueL(
                            KMPXMediaMusicGenre, genreName->Des() );
                        media->SetTextValueL(
                            KMPXMediaMusicGenre, genreName->Des() );
                        SaveMediaPropertiesL( media );
                        CleanupStack::PopAndDestroy( media );
                        }
                    if ( !tryExit )
                        {
                        FetchGenreL();
                        }
                    }
                else
                    {
                    // Text query cancelled.  Display popup list again.
                    iPopup->ActivateSelectionListL();
                    
                    // reset to original index
                    TInt genreIndex;
                    iGenreArr->Find( iMedia->ValueText( KMPXMediaMusicGenre ), genreIndex );
                    iTextValues->SetCurrentValueIndex( genreIndex );
                    }
                CleanupStack::PopAndDestroy( genreName );
                }
            else
                {
                if ( iUnknownGenre &&
                    ( iTextValues->CurrentValueIndex() == ( iTextValues->MdcArray()->MdcaCount() - 1 ) ) )
                    {
                    MPX_DEBUG1( "CMPXMetadataEditorDialog::HandleControlStateChangeL last entry with unknown genre, do nothing" );
                    }
                else
                    {
                    CMPXMedia* media = CMPXMedia::NewL();
                    CleanupStack::PushL( media );
                    HBufC* currentTxt = iTextValues->CurrentValueTextLC();
                    iMedia->SetTextValueL( KMPXMediaMusicGenre, *currentTxt );
                    media->SetTextValueL( KMPXMediaMusicGenre, *currentTxt );
                    CleanupStack::PopAndDestroy( currentTxt );
                    SaveMediaPropertiesL( media );
                    CleanupStack::PopAndDestroy( media );
                    FetchGenreL();
                    }
                }
            }
        else if ( aControlId == EMPXMetadataEditorDlgCtrlIdLibrary )
            {
            TInt currentIndex = iLibraryTextValues->CurrentValueIndex();
            if ( iCurrentLibrary != currentIndex )
                {
                if ( PromptForLibraryChangeL( currentIndex ) )
                    {
                    MPX_DEBUG1( "CMPXMetadataEditorDialog::HandleControlStateChangeL Move to a different library" );
                    TUid targetCollection = TUid::Uid( EMPXCollectionPluginMusic );
                    if ( currentIndex == EMPXMetadataEditorDlgPodcast )
                        {
                        targetCollection = TUid::Uid(EMPXCollectionPluginPodCast);
                        }
                    // before moving, save current data
                    SaveFormDataL();
                    iCurrentLibrary = currentIndex; // Fix for ELWG-7AACMS prevent the if clause to be reentried.
                    iCollectionUiHelper->MoveL( iMedia, targetCollection, this );
                    iTryingExit = ETrue;
                    }
                else
                    {
                    iLibraryTextValues->SetCurrentValueIndex( iCurrentLibrary );
                    // Fix EIZU-7QQAZN, Let it display back to "Music" immediately.
                    if ( IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdLibrary )
                        {
                        iPopup = static_cast<CAknPopupField*>
                                        ( ControlOrNull( EMPXMetadataEditorDlgCtrlIdLibrary ) );
                        if ( iPopup )
                            {
                            iPopup->SetQueryValueL( iLibraryTextValues );
                            iPopup->DrawDeferred();
                            }
                        }
                    }
                }
            }
        }
    CAknForm::HandleControlStateChangeL( aControlId );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::PrepareForFocusTransitionL
// Handle Focus changes for track and year controls.
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::PrepareForFocusTransitionL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::PrepareForFocusTransitionL" );
    TInt err = KErrNone;

    CEikNumberEditor* myTrackNumberEditor = static_cast<CEikNumberEditor*>
            ( ControlOrNull( EMPXMetadataEditorDlgCtrlIdTrackNumber ) );

    CEikNumberEditor* myYearNumberEditor = static_cast<CEikNumberEditor*>
            ( ControlOrNull( EMPXMetadataEditorDlgCtrlIdYear ) );

    if ( myTrackNumberEditor )
        {
        // if length 0 trap error and set to 0.
        MPX_TRAP( err, myTrackNumberEditor->Number() );
        if ( err )
            {
            myTrackNumberEditor->SetNumber( KMPXMinNumDateTrack );
            }
        }

    if ( myYearNumberEditor )
        {
        // if length 0 trap error and set to 0.
        MPX_TRAP( err, myYearNumberEditor->Number() );
        if ( err )
            {
            myYearNumberEditor->SetNumber( KMPXMinNumDateTrack );
            }
        }

    CAknForm::PrepareForFocusTransitionL();
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::GetHelpContext
// Gets Help
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::GetHelpContext" );
    aContext.iMajor = KAppUidMusicPlayerX;
    aContext.iContext = KMUS_HLP_METADATA_EDITING;
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::HandleFindAllL
// Handle callback for "find" operation
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleFindAllL(const CMPXMedia& aResults,
                               TBool /*aComplete*/, TInt aError)
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::HandleFindAllL" );
    iPopup = static_cast<CAknPopupField*>
        ( ControlOrNull( EMPXMetadataEditorDlgCtrlIdGenre ) );
    CleanUpGenreText();
    // populate predefined genre
    TResourceReader reader;
    if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
        {
        if( !iDisablePodcasting )
            {
            iEikonEnv->CreateResourceReaderLC( reader, R_MPX_MUSIC_GENRE_STRINGS );
            }
        else
            {
            iEikonEnv->CreateResourceReaderLC( reader, R_MPX_MUSIC_GENRE_STRINGS_PODCAST );
            }
        }
    else if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
        {
        iEikonEnv->CreateResourceReaderLC(
            reader, R_MPX_PODCAST_CATEGORY_STRINGS );
        }
    iGenreArr = reader.ReadDesCArrayL( );
    CleanupStack::PopAndDestroy(); // reader

    // populate genres from database
    if ( aError == KErrNone )
        {
        const CMPXMediaArray* mediaArray =
            aResults.Value<CMPXMediaArray>( KMPXMediaArrayContents );
        User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));
        
        TInt entriesCount( mediaArray->Count() );
        MPX_DEBUG2( "CMPXMetadataEditorDialog::HandleFindAllL genres from database count = %d", entriesCount );
        for ( TInt i = 0; i < entriesCount; i++ )
            {
            CMPXMedia* media = mediaArray->AtL( i );
            TInt location( 0 );
            const TDesC& genre = media->ValueText( KMPXMediaGeneralTitle );
            if ( i == entriesCount - 1)
                {
                if ( genre.Compare( KNullDesC ) != 0 )
                    {
                    // if last item is a null descriptor, don't append to array
                    if ( iGenreArr->Find( genre, location ) != 0 )
                        {
                        // a match is not found
                        MPX_DEBUG1( "CMPXMetadataEditorDialog::HandleFindAllL adding to genre list" );
                        iGenreArr->AppendL( genre );
                        }
                    }
                }
            else
                {
                if ( iGenreArr->Find( genre, location ) != 0 )
                    {
                    // a match is not found
                    MPX_DEBUG1( "CMPXMetadataEditorDialog::HandleFindAllL adding to genre list" );
                    iGenreArr->AppendL( genre );
                    }
                }
            }
        }
    iGenreArr->Sort(ECmpCollated);
    
    if ( iCurrentLibrary == EMPXMetadataEditorDlgCollection )
        {
            HBufC* custTxt =
            StringLoader::LoadLC( R_MPX_CUI_CUSTOM_GENRE_PROMPT_TEXT );
            iGenreArr->InsertL( 0, *custTxt );
            CleanupStack::PopAndDestroy( custTxt );
        }
    else if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
        {
            HBufC* custTxt =
            StringLoader::LoadLC( R_MPX_CUI_PODCAST_CUSTOM_GENRE_PROMPT_TEXT );
            iGenreArr->InsertL( 0, *custTxt );
            CleanupStack::PopAndDestroy( custTxt );
        }

    iValueTextArray = CAknQueryValueTextArray::NewL();
    iValueTextArray->SetArray( *iGenreArr );

    iTextValues = CAknQueryValueText::NewL();
    iTextValues->SetArrayL( iValueTextArray );
    if ( iMedia->ValueText( KMPXMediaMusicGenre ).Length() == 0 )
        {
        // no genre
        iUnknownGenre = ETrue;
        HBufC* unknownGenre( StringLoader::LoadLC( R_MPX_CUI_UNKNOWN_GENRE_TEXT ) );
        iGenreArr->AppendL( *unknownGenre );
        CleanupStack::PopAndDestroy( unknownGenre );
        iTextValues->SetCurrentValueIndex( iGenreArr->Count() - 1 );
        }
    else
        {
        // Find the genre
        iUnknownGenre = EFalse;
        TInt genreIndex;
        iGenreArr->Find( iMedia->ValueText( KMPXMediaMusicGenre ), genreIndex );
        iTextValues->SetCurrentValueIndex( genreIndex );
        }
    // Set values into popup fields.
    iPopup->SetQueryValueL( iTextValues ); // Moved up from below.
    iPopup->DrawDeferred();
    }

// ----------------------------------------------------------------------------
// void CMPXMetadataEditorDialog::HandleOperationCompleteL
// Handle callback for the ui helper.
// ----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleOperationCompleteL(
    TCHelperOperation aOperation,
    TInt aErr, void* aArgument )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::HandleOperationCompleteL" );
    if ( aOperation == EMoveOp )
        {
        if ( aErr != KErrNone )
            {
            // TO-DO: error handling
            }
        // move complete, quit the editor
        if ( iIdle )
            {
            iIdle->Cancel();
            delete iIdle;
            iIdle = NULL;
            }
        iIdle = CIdle::NewL( CActive::EPriorityStandard );
        iIdle->Start( TCallBack( CMPXMetadataEditorDialog::DeferredExitCallback, this ) );
        }
    if ( aArgument )
        {
        delete (CBase*)aArgument;
        }
    }

// ----------------------------------------------------------------------------
// void CMPXMetadataEditorDialog::DeferredExitCallback
// call back function for deferred exit.
// ----------------------------------------------------------------------------
//
TInt CMPXMetadataEditorDialog::DeferredExitCallback( TAny* aPtr )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::DeferredExitCallback" );
    CMPXMetadataEditorDialog* self =
        static_cast<CMPXMetadataEditorDialog*>( aPtr );
    TRAP_IGNORE( self->TryExitL( EAknSoftkeyExit ) );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// void CMPXMetadataEditorDialog::DeferredFileDetailsCallback
// call back function for deferred file details dialog.
// ----------------------------------------------------------------------------
//
TInt CMPXMetadataEditorDialog::DeferredFileDetailsCallback( TAny* aPtr )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::DeferredFileDetailsCallback" );
    CMPXMetadataEditorDialog* self =
        static_cast<CMPXMetadataEditorDialog*>( aPtr );
    TRAP_IGNORE( self->ViewFileDetailsPopupL() );
    return KErrNone;
    }

#ifdef __ENABLE_MSK
// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::UpdateSoftkeyL()
// Update softkey.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::UpdateSoftkeyL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::UpdateSoftkeyL" );
    TInt resId( 0 );

    if ( ( IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdGenre ) ||
        ( IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdLibrary ) )
        {
        resId = R_MPX_CUI_SONG_DETAILS_SOFTKEYS_OPTIONS_CHANGE_DONE;
        }
    else
        {
        resId = R_MPX_CUI_SONG_DETAILS_SOFTKEYS_OPTIONS_EMPTY_BACK;
        }

    ButtonGroupContainer().SetCommandSetL( resId );
    ButtonGroupContainer().DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::OfferKeyEventL
// Handles key-events.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXMetadataEditorDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::OfferKeyEventL" );
    TKeyResponse res = CAknForm::OfferKeyEventL( aKeyEvent, aType );

    if ( aType == EEventKey )
        {
        if (aKeyEvent.iScanCode && (aKeyEvent.iScanCode == EStdKeyUpArrow
                || aKeyEvent.iScanCode == EStdKeyDownArrow))
            {
            UpdateSoftkeyL();
            }

        else if ((aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter)
                && (IdOfFocusControl() == EMPXMetadataEditorDlgCtrlIdDRMDetail))
            {
            LaunchDrmInfoL();
            }
        }

    return res;
    }
#endif // __ENABLE_MSK

// ----------------------------------------------------------------------------
// void CMPXMetadataEditorDialog::PreLayoutDynInitL
// Initialises the dialog's controls before the dialog is sized and layed out.
// ----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::PreLayoutDynInitL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::PreLayoutDynInitL" );
    SetTitlePaneL();
    SetNaviLabelL();

    _LIT( KZero, "0" );
    
    SetControlNumberL( EMPXMetadataEditorDlgCtrlIdTrackNumber, KZero,
    	KMPXSongDetailsTrackNumMin, KMPXSongDetailsTrackNumMax );

    SetControlNumberL ( EMPXMetadataEditorDlgCtrlIdYear, KZero,
    	KMPXSongDetailsYearMin, KMPXSongDetailsYearMax );

    // Get media property for the current song
    
    if ( iParam ) // Dialog launched from CollectionView
        {
		CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( cpath );
        TLex yearLex( iParam->Des() );
        TInt lexToInt = NULL;
        if ( yearLex.Val( lexToInt ) == KErrNone )
            {
            cpath->Set( lexToInt );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append( KMPXMediaGeneralAll );
            attrs.Append( KMPXMediaAudioAudioAll );
            attrs.Append( KMPXMediaMusicAll );
            attrs.Append( KMPXMediaDrmProtected );
            if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
                {
                attrs.Append(
                    TMPXAttribute( KMPXMediaIdPodcast, EMPXMediaPodcastAll ) );
                }
            iCurrentMediaLOp = EMPXMetadataEditorGetSongInfo;
            iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
            CleanupStack::PopAndDestroy( &attrs );
            }
		CleanupStack::PopAndDestroy( cpath );	
        }
    else // Dialog launched from NowPlayingView
        {
        // Get the playback utility instance from engine.
        MMPXPlaybackUtility* playbackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
        MMPXSource* s = playbackUtility->Source();
        if ( s )
            {
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append( KMPXMediaGeneralAll );
            attrs.Append( KMPXMediaAudioAudioAll );
            attrs.Append( KMPXMediaMusicAll );
            attrs.Append( KMPXMediaDrmProtected );
            if ( iCurrentLibrary == EMPXMetadataEditorDlgPodcast )
                {
                 attrs.Append(
                      TMPXAttribute( KMPXMediaIdPodcast, EMPXMediaPodcastAll ) );
                }
                iCurrentMediaLOp = EMPXMetadataEditorGetSongInfo;
                s->MediaL(attrs.Array(), *this);
                CleanupStack::PopAndDestroy( &attrs );
            }  
        playbackUtility->Close();
        }
    

    // Podcasting is enabled
    if ( !iDisablePodcasting )
        {
        iPopup = static_cast<CAknPopupField*>
            ( ControlOrNull( EMPXMetadataEditorDlgCtrlIdLibrary ) );
        iLibraryArr = new (ELeave) CDesCArrayFlat( 1 );
        HBufC* custTxt = StringLoader::LoadLC( R_MPX_CUI_GENRE_SELECTION_MUSIC_TEXT );
        iLibraryArr->AppendL( *custTxt );
        CleanupStack::PopAndDestroy( custTxt );
        custTxt = StringLoader::LoadLC( R_MPX_CUI_GENRE_SELECTION_PODCAST_TEXT );
        iLibraryArr->AppendL( *custTxt );
        CleanupStack::PopAndDestroy( custTxt );
        iLibraryValueTextArray = CAknQueryValueTextArray::NewL();
        iLibraryValueTextArray->SetArray( *iLibraryArr );
        iLibraryTextValues = CAknQueryValueText::NewL();
        iLibraryTextValues->SetArrayL( iLibraryValueTextArray );
        iLibraryTextValues->SetCurrentValueIndex( iCurrentLibrary );
        // Set values into popup fields
        iPopup->SetQueryValueL( iLibraryTextValues ); // Moved up from below
        }
    }

// ----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::PostLayoutDynInitL
// Set default field value to member data.
// ----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::PostLayoutDynInitL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::PostLayoutDynInitL" );
    CAknForm::PostLayoutDynInitL();
    SetEditableL( EFalse );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SaveMediaPropertiesL
// Saves media back to collection engine
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::SaveMediaPropertiesL( CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SaveMediaPropertiesL" );
    if ( iMedia->IsSupported( KMPXMediaGeneralUri ) )
        {
        aMedia->SetTextValueL(
            KMPXMediaGeneralUri, iMedia->ValueText( KMPXMediaGeneralUri ) );
        }

    if ( iMedia->IsSupported( KMPXMediaGeneralType ) )
        {
        aMedia->SetTObjectValueL<TInt>(
            KMPXMediaGeneralType, iMedia->ValueTObjectL<TInt>( KMPXMediaGeneralType ) );
        }

    if ( iMedia->IsSupported( KMPXMediaGeneralCategory ) )
        {
        aMedia->SetTObjectValueL<TInt>(
            KMPXMediaGeneralCategory,
            iMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCategory ) );
        }

    if ( iMedia->IsSupported( KMPXMediaGeneralId ) )
        {
        aMedia->SetTObjectValueL<TMPXItemId>(
            KMPXMediaGeneralId, iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        }

    if ( iMedia->IsSupported( KMPXMediaGeneralCollectionId ) )
        {
        aMedia->SetTObjectValueL<TUid>(
            KMPXMediaGeneralCollectionId,
            iMedia->ValueTObjectL<TUid>( KMPXMediaGeneralCollectionId ) );
        }
    
    if ( iMedia->IsSupported( KMPXMediaMusicAlbumArtFileName ) )
        {
        aMedia->SetTextValueL(
            KMPXMediaMusicAlbumArtFileName,
            iMedia->ValueText( KMPXMediaMusicAlbumArtFileName ) );
        }

    // toggle the modified flag for the internal media reference
    // or else any subsequent FindAllL calls won't be able to
    // match against this object
    iMedia->SetTObjectValueL<TBool>(KMPXMediaGeneralModified, ETrue);

    // Set to collection via command
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, KMPXCommandIdCollectionSet );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );
    TUid colId(iMedia->ValueTObjectL<TUid>(KMPXMediaGeneralCollectionId));
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, aMedia );

    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::AppendStringToArrayL
// Appends the value to the array, if the vaule has zero length,
// appends "unavailable"
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::AppendStringToArrayL(
    CDesCArrayFlat* aArray, const TDesC& aValue )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::AppendStringToArrayL" );
    if ( aValue.Length() > 0 )
        {
        HBufC* stringBuf = HBufC::NewLC(
            aValue.Length() + KMPXFileDetailsExtraBufferLen );
        TPtr stringBufPtr = stringBuf->Des();
        stringBufPtr.Append( aValue );
        aArray->AppendL( stringBufPtr );
        CleanupStack::PopAndDestroy( stringBuf );
        }
    else
        {
        aArray->AppendL( KNullDesC );
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SetControlTextL
// Sets control text
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::SetControlTextL( TInt aControlId,
    const TDesC& aValue, const TDesC& aAlternateValue )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SetControlTextL" );
    CEikEdwin* myEdwin = static_cast< CEikEdwin* >
        ( ControlOrNull( aControlId ) );
    if ( myEdwin )
        {
        if ( aValue.Length() > 0 )
            {
            myEdwin->SetTextL( &( aValue ) );
            }
        else if ( aAlternateValue.Length() > 0 )
            {
            myEdwin->SetTextL( &( aAlternateValue ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::SetControlNumberL
// Sets control number
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::SetControlNumberL( TInt aControlId,
    const TDesC& aValue, TInt aMinValue, TInt aMaxValue )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::SetControlNumberL" );
    CEikNumberEditor* myNumberEditor = static_cast< CEikNumberEditor* >
        ( ControlOrNull( aControlId ) );

    TInt defaultValue( 0 );
    if ( aMinValue > 0  )
        {
        defaultValue = aMinValue;
        }

    if ( myNumberEditor )
        {
        if ( aValue.Length() > 0 )
            {
            TLex trackNumLex( aValue );
            TInt LexToInt;
            if ( trackNumLex.Val( LexToInt ) == KErrNone )
                {
                if ( LexToInt > aMaxValue )
                    {
                    LexToInt = aMaxValue;
                    }
                if ( LexToInt < aMinValue )
                    {
                    LexToInt = aMinValue;
                    }
                myNumberEditor->SetNumber( LexToInt );
                }
            else
                {
                myNumberEditor->SetNumber( defaultValue );
                }
            }
        else
            {
            myNumberEditor->SetNumber( defaultValue );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::PromptForLibraryChangeL
// prompt user for library change
// -----------------------------------------------------------------------------
//
TBool CMPXMetadataEditorDialog::PromptForLibraryChangeL( TInt aMoveToLibrary )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::PromptForLibraryChangeL" );
    CMPXQueryDialog* confirmationDlg = CMPXQueryDialog::NewL();
    CleanupStack::PushL( confirmationDlg );
    HBufC* libraryName = NULL;
    if ( aMoveToLibrary == EMPXMetadataEditorDlgCollection )
        {
        libraryName =
            StringLoader::LoadLC( R_MPX_CUI_GENRE_SELECTION_MUSIC_TEXT );
        }
    else if ( aMoveToLibrary == EMPXMetadataEditorDlgPodcast )
        {
        libraryName =
            StringLoader::LoadLC( R_MPX_CUI_GENRE_SELECTION_PODCAST_TEXT );
        }
    else
        {
        // should not reach here
        User::Leave( KErrArgument );
        }
    HBufC* promptTxt = StringLoader::LoadLC(
        R_MPX_CUI_QUERY_MOVE_TO_LIBRARY, libraryName->Des() );
    confirmationDlg->SetPromptL( *promptTxt );
    CleanupStack::PopAndDestroy( promptTxt );


    confirmationDlg->PublishDialogL(
        EMPlayerQueryMoveToLibrary,
        KMPlayerNoteCategory);

    CAknMediatorFacade* covercl(
        AknMediatorFacade( confirmationDlg ) );
    if ( covercl )
        {
        covercl->BufStream() << libraryName->Des();
        }
    CleanupStack::PopAndDestroy( libraryName );
    CleanupStack::Pop( confirmationDlg );
    return confirmationDlg->ExecuteLD( R_MPX_CUI_MOVE_TO_LIBRARY_CONFIRMATION_QUERY );
    }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::UpdateMediaObjectWithControlL
// Updates media object with current control text, this will also
// update iMedia
// -----------------------------------------------------------------------------
//
TBool CMPXMetadataEditorDialog::UpdateMediaObjectWithControlL(
    TInt aControlId, CMPXMedia* aMedia, TMPXAttribute aAttribute )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::UpdateMediaObjectWithControlL" );
    TBool ret = EFalse;
    CCoeControl* coeControl = ControlOrNull( aControlId );
    if ( coeControl )
        {
        HBufC* buf = NULL;
        switch ( aControlId )
            {
            case EMPXMetadataEditorDlgCtrlIdSongName:
            case EMPXMetadataEditorDlgCtrlIdArtist:
            case EMPXMetadataEditorDlgCtrlIdAlbum:
            case EMPXMetadataEditorDlgCtrlIdComment:
            case EMPXMetadataEditorDlgCtrlIdComposer:
                {
                CEikEdwin* control = static_cast<CEikEdwin*>( coeControl );
                if ( control )
                    {
                    buf = control->GetTextInHBufL();
                    if ( !buf )
                        {
                        // the text contains nothing
                        // special handling is needed for song name
                        if ( aControlId == EMPXMetadataEditorDlgCtrlIdSongName && iMedia )
                            {
                            if ( iMedia->IsSupported( KMPXMediaGeneralUri ) )
                                {
                                // if URI is defined, use filename as song name
                                TParsePtrC fn( iMedia->ValueText( KMPXMediaGeneralUri ) );
                                buf = fn.Name().AllocL();
                                }
                            else
                                {
                                buf = HBufC::NewL( 0 );
                                }
                            }
                        else
                            {
                            buf = HBufC::NewL( 0 );
                            }
                        }
                    CleanupStack::PushL( buf );
                    }
                else
                    {
                    // should not reach here
                    User::Leave( KErrArgument );
                    }
                break;
                }
            case EMPXMetadataEditorDlgCtrlIdTrackNumber:
                {
                CEikNumberEditor* control = static_cast<CEikNumberEditor*>( coeControl );
                if ( control )
                    {
                    TInt num = control->Number();
                    MPX_DEBUG2( "CMPXMetadataEditorDialog::UpdateMediaObjectWithControlL number from control %d", num );
                    buf = HBufC::NewLC( KMPXFileDetailsMaxBufferLen );
                    TPtr bufPtr = buf->Des();
                    bufPtr.AppendNum( num );
                    }
                else
                    {
                    // should not reach here
                    User::Leave( KErrArgument );
                    }
                break;
                }
            default:
                {
                // should not reach here
                User::Leave( KErrArgument );
                break;
                }
            }
        if ( buf )
            {
            const TDesC& origValue = iMedia->ValueText( aAttribute );
            if ( origValue.Compare( *buf ) != 0 )
                {
	                //check if the track number is actually changed
	                if (aControlId == EMPXMetadataEditorDlgCtrlIdTrackNumber &&
	                	origValue.Compare(_L("")) == 0 &&
	                	buf->Compare(_L("0")) == 0)
	                	{
	                	//nothing to do
	                	}
	            	else
	            		{
		                ret = ETrue;
		                iMedia->SetTextValueL( aAttribute , *buf );
		                aMedia->SetTextValueL( aAttribute , *buf );
	            		}
                }
            CleanupStack::PopAndDestroy( buf );
            }
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::PopulateFileDetailsL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::PopulateFileDetailsL()
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::PopulateFileDetailsL" );
    
    ASSERT(iMedia);
    TBool drmProtected(iMedia->ValueTObjectL<TBool> (KMPXMediaDrmProtected));
    MPX_DEBUG2( "CMPXMetadataEditorDialog::PopulateFileDetailsL drm protected: %d", drmProtected );

    if (drmProtected)
        {
        HBufC* detail = StringLoader::LoadLC( R_MPX_CUI_LICENCE_DET_LINK );
        InsertLineL(9,R_MPX_CUI_LICENCE_INFO,ActivePageId() );
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdDRMDetail, detail->Des(),
                KNullDesC);
        CleanupStack::PopAndDestroy( detail );	
        }
   
    // Get filename
    const TDesC& uri = iMedia->ValueText(KMPXMediaGeneralUri);
    TParsePtrC parse(uri);

    SetControlTextL(EMPXMetadataEditorDlgCtrlIdFileName, parse.Name(),
            KNullDesC);

    // Get mime type
    SetControlTextL(EMPXMetadataEditorDlgCtrlIdFileFormat, iMedia->ValueText(
            KMPXMediaGeneralMimeType), KNullDesC);

    // Get duration
    TInt duration(iMedia->ValueTObjectL<TInt> (KMPXMediaGeneralDuration));
    if (duration > 0)
        {
        // convert milliseconds to seconds
        duration = duration / KSecondInMilliseconds;
        CMPXCommonUiHelper::TMPXDuratDisplayMode durationMode =
        CMPXCommonUiHelper::EMPXDuratAuto;
        if (duration > KOneHourInSeconds)
            {
            durationMode = CMPXCommonUiHelper::EMPXDuratHMS;
            }
        HBufC* stringBuf = iCommonUiHelper->DisplayableDurationL(duration,
                durationMode);
        CleanupStack::PushL(stringBuf);
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdFileDuration, *stringBuf,
                KNullDesC);
        CleanupStack::PopAndDestroy(stringBuf);
        }
    else
        {
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdFileDuration, KNullDesC,
                KNullDesC);
        }

    // Get bitrate
    TInt bitrate(iMedia->ValueTObjectL<TInt> (KMPXMediaAudioBitrate));
    if (bitrate > 0)
        {
        // convert bitrate to kpbs
        bitrate = bitrate / KMPXBitrateFactor;
        HBufC* bitrateText = StringLoader::LoadLC(
                R_MPX_CUI_METADATAEDITOR_BITRATE_TXT, bitrate);
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdFileBitrate,
                bitrateText->Des(), KNullDesC);
        CleanupStack::PopAndDestroy(bitrateText);
        }
    else
        {
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdFileBitrate, KNullDesC,
                KNullDesC);
        }

    // Get sampling rate
    TInt samplingRateNum((TInt) iMedia->ValueTObjectL<TInt> (
            KMPXMediaAudioSamplerate));
    if (samplingRateNum > 0)
        {
        HBufC* stringBuf = HBufC::NewLC(KMPXFileDetailsMaxBufferLen);
        TPtr stringBufPtr = stringBuf->Des();
        stringBufPtr.AppendNum(samplingRateNum);
        HBufC* samplingrate = StringLoader::LoadLC(
                R_MPX_CUI_METADATAEDITOR_SAMPLINGRATE_TXT, stringBufPtr);
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdSamplingrate,
                samplingrate->Des(), KNullDesC);
        CleanupStack::PopAndDestroy(samplingrate);
        CleanupStack::PopAndDestroy(stringBuf);
        }
    else
        {
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdSamplingrate, KNullDesC,
                KNullDesC);
        }

    // Get size, not stored in collection, have to use RFile
    //
    RFs& fs = CEikonEnv::Static()->FsSession();
    RFile file;
    TInt sizeNum(0);
    TInt err(file.Open(fs, uri, EFileRead | EFileShareReadersOrWriters));
    if (err == KErrNone)
        {
        file.Size(sizeNum);
        file.Close();
        }

    if (sizeNum > 0)
        {
        HBufC* stringBuf = iCommonUiHelper->UnitConversionL(sizeNum);
        CleanupStack::PushL(stringBuf);
        TPtr stringBufPtr = stringBuf->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(stringBufPtr);
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdSize, stringBufPtr,
                KNullDesC);
        CleanupStack::PopAndDestroy(stringBuf);
        }
    else
        {
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdSize, KNullDesC, KNullDesC);
        }

    if (iCurrentLibrary == EMPXMetadataEditorDlgPodcast)
        {
          PopulatePodcastFileDetailsL();
        }
// Get modified
if ( iMedia->IsSupported( KMPXMediaGeneralDate ) )
    {
    TInt64 timeInt( ( TInt64 )iMedia->ValueTObjectL<TInt64>(
            KMPXMediaGeneralDate ) );
    TTime time( timeInt );
    ConvertToLocalTimeL(time);
    HBufC* modDateTime = HBufC::NewLC(
            KMPXMaxTimeLength + KMPXDurationDisplayResvLen );
    HBufC* format = StringLoader::LoadLC(
            R_QTN_DATE_USUAL_WITH_ZERO );
    TPtr modDatePtr = modDateTime->Des();
    MPX_TRAPD( err, time.FormatL( modDatePtr, *format ) );
    CleanupStack::PopAndDestroy( format );
    if (err != KErrNone)
        {
        SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastModified,
                KNullDesC, KNullDesC);

        }
    else
        {
        format = StringLoader::LoadLC(
                R_QTN_TIME_USUAL_WITH_ZERO );
        HBufC* modTime = HBufC::NewLC(
                format->Length() + KMPXDurationDisplayResvLen );
        TPtr modTimePtr = modTime->Des();
        MPX_TRAPD( err, time.FormatL( modTimePtr, *format ) );
        if ( err != KErrNone )
            {
            SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastModified,
                    KNullDesC, KNullDesC);

            }
        else
            {
            modDatePtr.Append( KMPXSpace );
            modDatePtr.Append( modTimePtr );
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion(modDatePtr);
            SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastModified,
                    modDatePtr,
                    KNullDesC);
            }
        CleanupStack::PopAndDestroy( modTime );
        CleanupStack::PopAndDestroy( format );
        }
    CleanupStack::PopAndDestroy( modDateTime );
    }
else
    {
    SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastModified,
            KNullDesC, KNullDesC);

    }

// Get filename
SetControlTextL(EMPXMetadataEditorDlgCtrlIdCopyright,
        iMedia->ValueText(
                KMPXMediaGeneralCopyright ), KNullDesC);

// Get URL
SetControlTextL(EMPXMetadataEditorDlgCtrlIdWebaddress,
        iMedia->ValueText(
                KMPXMediaMusicURL ), KNullDesC);

CAknForm::SetInitialCurrentLine();
}

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::PopulatePodcastFileDetailsL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::PopulatePodcastFileDetailsL()
    {
      InsertLineL(13,R_MPX_CUI_LAST_PLAYBACK_POSITION,ActivePageId() );
                    
            TInt lastPbPosition((TInt) iMedia->ValueTObjectL<TInt> (
                    KMPXMediaGeneralLastPlaybackPosition));

            if (lastPbPosition > 0)
                {
                // convert milliseconds to seconds
                lastPbPosition = lastPbPosition / KSecondInMilliseconds;
                CMPXCommonUiHelper::TMPXDuratDisplayMode lastPbPositionMode =
                CMPXCommonUiHelper::EMPXDuratAuto;
                if (lastPbPosition > KOneHourInSeconds)
                    {
                    lastPbPositionMode = CMPXCommonUiHelper::EMPXDuratHMS;
                    }
                HBufC* stringBuf = iCommonUiHelper->DisplayableDurationL(
                        lastPbPosition, lastPbPositionMode);
                CleanupStack::PushL(stringBuf);
                SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastPlayedPosition,
                        *stringBuf, KNullDesC);
                CleanupStack::PopAndDestroy(stringBuf);
                }
            else if (lastPbPosition == 0 && iMedia->IsSupported(
                    KMPXMediaGeneralPlayCount) && iMedia->ValueTObjectL<TInt> (
                            KMPXMediaGeneralPlayCount) > 0)
                {
                HBufC* stringBuf = StringLoader::LoadLC(
                        R_MPX_CUI_METADATAEDITOR_PLAYBACK_COMPLETE);
                SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastPlayedPosition,
                        *stringBuf, KNullDesC);
                CleanupStack::PopAndDestroy(stringBuf);

                }
            else
                {
                SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastPlayedPosition,
                        KNullDesC, KNullDesC);
                }
           
            InsertLineL(14,R_MPX_CUI_DETAILS_PUBLISHED,ActivePageId() );
            
            // Get published
            if (iMedia->IsSupported(TMPXAttribute(KMPXMediaIdPodcast,
                    EMPXMediaPodcastPubDate)))
                {
                TInt64 timeInt(
                        (TInt64) iMedia->ValueTObjectL<TInt64> (TMPXAttribute(
                                KMPXMediaIdPodcast, EMPXMediaPodcastPubDate)));
                TTime time(timeInt);
                ConvertToLocalTimeL(time);
                HBufC* modDateTime = HBufC::NewLC(KMPXMaxTimeLength
                        + KMPXDurationDisplayResvLen);
                HBufC* format = StringLoader::LoadLC(R_QTN_DATE_USUAL_WITH_ZERO);
                TPtr modDatePtr = modDateTime->Des();
                MPX_TRAPD( err, time.FormatL( modDatePtr, *format ) );
                CleanupStack::PopAndDestroy(format);
                if (err != KErrNone || time == 0)
                    {
                    SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastPublished,
                            KNullDesC, KNullDesC);
                    }
                else
                    {
                    format = StringLoader::LoadLC(R_QTN_TIME_USUAL_WITH_ZERO);
                    HBufC* modTime = HBufC::NewLC(format->Length()
                            + KMPXDurationDisplayResvLen);
                    TPtr modTimePtr = modTime->Des();
                    MPX_TRAPD( err, time.FormatL( modTimePtr, *format ) );
                    if (err != KErrNone)
                        {
                        SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastPublished,
                                KNullDesC, KNullDesC);
                        }
                    else
                        {
                        modDatePtr.Append(KMPXSpace);
                        modDatePtr.Append(modTimePtr);
                        SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastPublished,
                                modTimePtr, KNullDesC);
                        }
                    CleanupStack::PopAndDestroy(modTime);
                    CleanupStack::PopAndDestroy(format);
                    }
                CleanupStack::PopAndDestroy(modDateTime);
                }
            else
                {
                SetControlTextL(EMPXMetadataEditorDlgCtrlIdLastPublished,
                        KNullDesC, KNullDesC);
                }
      }

// -----------------------------------------------------------------------------
// CMPXMetadataEditorDialog::LaunchDrmInfoL
// -----------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::LaunchDrmInfoL()

    {
    MPX_FUNC( "CMPXMetadataEditorDialog::LaunchDrmInfoL" );
    const TDesC& aFileName = iMedia->ValueText(KMPXMediaGeneralUri);
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    RFile64 drmFile;
    User::LeaveIfError( drmFile.Open( 
            fs, aFileName, EFileRead | EFileShareReadersOrWriters ) );
    CleanupClosePushL( drmFile );
    iDrmUiHandler->ShowDetailsViewL( drmFile );
    CleanupStack::PopAndDestroy( &drmFile );
    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandlePropertyL(
    TMPXPlaybackProperty aProperty,
    TInt aValue,
    TInt aError )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::HandleSubPlayerNamesL" );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */ )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::HandleSubPlayerNamesL" );
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media properties.
// Notes: The client is responsible for delete the object of aMedia.
// ---------------------------------------------------------------------------
//
void CMPXMetadataEditorDialog::HandleMediaL(
    const CMPXMedia& aMedia,
    TInt aError )
    {
    MPX_FUNC( "CMPXMetadataEditorDialog::HandleMediaL" );
    TRAP_IGNORE( DoHandleMediaL( aMedia, aError ) );
    }
// End of File
