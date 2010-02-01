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
* Description:  Implementation of mpxaddtracksdialogimp
*
*/


// INCLUDE FILES
#include <eikclb.h>
#include <eikclbd.h>
#include <eikimage.h>
#include <eiktxlbx.h>
#include <aknappui.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <akncontext.h>
#include <aknlists.h>
#include <aknsfld.h>
#include <gulicon.h>
#include <AknIconArray.h>
#include <AknIconUtils.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <e32base.h>
#include <barsread.h>
#include <bautils.h>
#include <aknnavilabel.h>
#include <data_caging_path_literals.hrh>
#include <textresolver.h>

#include <mpxlog.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxaddtracksdialog.rsg>
#include <mpxaddtracksdialog.mbg>

#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionplaylist.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmedia.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaarray.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessage.h>
#include <mpxuser.h>

#include "mpxcommonuihelper.h"
#include "mpxaddtracksdialogcommon.h"
#include "mpxaddtracksdialog.hrh"
#include "mpxaddtracksdialog.h"
#include "mpxaddtrackslistmodel.h"
#include "mpxaddtrackslbxarray.h"
#include <mpxlbxextendedfeatures.h>

// CONSTANTS
const TInt KMPXATMaxExpandedGroupNum = 1;

// Animation timer defined in UI spec
const TInt KAnimationFrame1Timer = 50000;
const TInt KAnimationFrame2Timer = 100000;
const TInt KAnimationFrame3Timer = 100000;
const TInt KAnimationFrame4Timer = 500000;
const TInt KAnimationFrameTimerInterval = 1;

const TInt KMaxTitleLength = 261;  // 255 + 6 for tab

_LIT( KMPXAddTracksDialogRscPath, "mpxaddtracksdialog.rsc" );
_LIT(KMPXSpace," ");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXAddTracksDialog* CMPXAddTracksDialog::NewL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::NewL" );
    CMPXAddTracksDialog* self = CMPXAddTracksDialog::NewLC();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXAddTracksDialog* CMPXAddTracksDialog::NewLC()
    {
    MPX_FUNC( "CMPXAddTracksDialog::NewLC" );
    CMPXAddTracksDialog* self = new ( ELeave ) CMPXAddTracksDialog();
    CleanupStack::PushL( self );
    self->LoadResourceL();
    self->ConstructL( R_AVKON_SOFTKEYS_EMPTY );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXAddTracksDialog::~CMPXAddTracksDialog()
    {
    MPX_FUNC( "CMPXAddTracksDialog::~CMPXAddTracksDialog" );

    TRAP_IGNORE( RestorePreviousStatusPaneL() );

    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }
    if ( iCollectionUiHelper )
        {
        iCollectionUiHelper->Cancel();
        iCollectionUiHelper->Close();
        }
    if ( iOrigIcon )
        {
        delete iOrigIcon;
        }
    if ( iNewIcon )
        {
        delete iNewIcon;
        }
    if ( iLastFindTxt )
        {
        delete iLastFindTxt;
        }
    iMatchedSongArray.Reset();
    iMatchedGroupArray.Reset();

    delete iNaviDecorator;
    delete iOrigTitle;
    delete iNaviLabelPane;
    delete iListModel;
    delete iNoSongText;
    delete iNoMatchText;

    if( iPeriodic )
        {
        delete iPeriodic;
        }
    delete iLbxExtFeat;

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }

    if ( iCategoryArray )
        {
        iCategoryArray->Reset();
        delete iCategoryArray;
        }

    if ( iAllSongsArray )
        {
        iAllSongsArray->Reset();
        delete iAllSongsArray;
        }

    delete iCommonUiHelper;
    delete iIdle;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::CMPXAddTracksDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPXAddTracksDialog::CMPXAddTracksDialog():
    CAknSelectionListDialog( iLastIndex, NULL, 0 ),
    iAnimIconIndex( 0 ),
    iSongAllGroupExist( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::ConstructL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::ConstructL( TInt aResource )
    {
    MPX_FUNC( "CMPXAddTracksDialog::ConstructL" );
    CAknSelectionListDialog::ConstructL( aResource );
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    iCommonUiHelper = CMPXCommonUiHelper::NewL();
    iNoSongText = StringLoader::LoadL( R_MPX_CUI_CATEGORY_ADDSONGVIEW_LBX_NOSONGTEXT );
    iNoMatchText = StringLoader::LoadL( R_MPX_CUI_CATEGORY_ADDSONGVIEW_LBX_EMPTYTEXT );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::BackupPreviousStatusPaneL()
// Backup previous status pane to internal members.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::BackupPreviousStatusPaneL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::BackupPreviousStatusPaneL" );
    CEikStatusPane* sp = iAvkonAppUi->StatusPane();

    // Backup navi pane
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iOrigNaviPane = iNaviPane->Top();

    iNaviPane->PushDefaultL();

    // Backup title pane
    iTitlePane = static_cast<CAknTitlePane*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    iOrigTitle = iTitlePane->Text()->AllocL();

    // Backup context pane only if new context icon is created
    if ( iNewIcon )
        {
        iContextPane = static_cast<CAknContextPane*>
            ( sp->ControlL( TUid::Uid( EEikStatusPaneUidContext ) ) );
        iOrigIcon = iContextPane->SwapPicture( iNewIcon );
        iNewIcon = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::RestorePreviousStatusPaneL()
// Restore previous status pane from internal members.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::RestorePreviousStatusPaneL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::RestorePreviousStatusPaneL" );
    if ( iOrigTitle )
        {
        // Set original title pane
        iTitlePane->SetTextL( *iOrigTitle );
        }

    // Set original navi pane
    iNaviPane->Pop( iNaviLabelPane );
    if ( iOrigNaviPane )
        {
        iNaviPane->PushL( *iOrigNaviPane );
        }
    else
        {
        iNaviPane->PushDefaultL();
        }

    // Restore original context icon
    if ( iOrigIcon )
        {
        iNewIcon = iContextPane->SwapPicture( iOrigIcon );
        iOrigIcon = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::CreateContextIconL()
// Create context icon for add tracks dialog.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::CreateContextIconL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::CreateContextIconL" );
    TParse parse;
    parse.Set( KMPXAddTracksBmpFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName iconFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( iconFile ) );

    MAknsSkinInstance* skin( AknsUtils::SkinInstance() );
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC(
            skin,
            KAknsIIDNone,
            bitmap,
            mask,
            iconFile,
            EMbmMpxaddtracksdialogQgn_graf_mup_ctx_addsong,
            EMbmMpxaddtracksdialogQgn_graf_mup_ctx_addsong_mask );

    iNewIcon = new ( ELeave ) CEikImage();
    iNewIcon->SetPicture( bitmap, mask );
    iNewIcon->SetPictureOwnedExternally( EFalse );

    CleanupStack::Pop( 2 ); // bitmap, mask
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::UpdateSoftkeyL()
// Update softkey.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::UpdateSoftkeyL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::UpdateSoftkeyL" );
    TInt resId( 0 );
    TInt index = CurrentLbxItemIndex();
    CAknSearchField* findBox = FindBox();
    TBool findBoxVisible = ( findBox->IsVisible() ) && ( findBox->IsFocused() );

    MPX_DEBUG2( "CMPXAddTracksDialog::UpdateSoftkeyL index = %d", index);

    iListBox->View()->SetListEmptyTextL(
        findBoxVisible? *iNoMatchText : *iNoSongText );
    if ( iListBox->Model()->NumberOfItems() == 0 )
        {
        // redraw empty text
        iListBox->DrawDeferred();
        }

    if ( index == KErrNotFound )
        {
        resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_EMPTY_DONE;
        if ( findBoxVisible )
            {
            resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_EMPTY_CANCEL;
            }
        }
    else
        {
        TInt id = iListModel->TrackTypeL( index );
        if ( id == EMPXGroup )
            {
            resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_EXPAND_DONE;
            if ( findBoxVisible )
                {
                resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_EXPAND_CANCEL;
                }
            if ( iListModel->IsExpandedIndex( index ) )
                {
                resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_COLLAPSE_DONE;
                if ( findBoxVisible )
                    {
                    resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_COLLAPSE_CANCEL;
                    }
                }
            }
        else
            {
            resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_ADD_DONE;
            if ( findBoxVisible )
                {
                resId = R_MPX_CUI_ADDTRACKS_SOFTKEYS_ADD_CANCEL;
                }
            }
        }
    ButtonGroupContainer().SetCommandSetL( resId );
    
    // A temporary RSK command observer is registered so that FindBox 
    // cannot register its own RSK command observer. This way it will
    // not handle the RSK but the ProcessCommandL method in this dialog
    // will do it. The observer must not be removed, because if there 
    // is no observer registered when the find box is closed, then 
    // the execution of CAknSearchField::RestorePopupCBA will raise a
    // panic.
   	if( !ButtonGroupContainer().UpdatedCommandObserverExists( 
		CEikButtonGroupContainer::ERightSoftkeyPosition ) )
   		{
   		MPX_DEBUG1( "CMPXAddTracksDialog::UpdateSoftkeyL: Adding RSK observer" );
   		ButtonGroupContainer().UpdateCommandObserverL( 2, *this );
   		}
    ButtonGroupContainer().DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::UpdateListboxByCategoryL()
// Update listbox content by category.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::UpdateListboxByCategoryL( const CMPXMedia& aMedia  )
    {
    MPX_FUNC( "CMPXAddTracksDialog::UpdateListboxByCategoryL" );
    // Reset tracks list
    iListModel->ResetTracksList();

    // Insert "Songs (all)" to model
    CMPXMediaArray* media =
        const_cast<CMPXMediaArray*>(aMedia.Value<CMPXMediaArray>(
        KMPXMediaArrayContents ) );
    User::LeaveIfNull( media );

    delete iCategoryArray;
    iCategoryArray = NULL;
    iCategoryArray = CMPXMediaArray::NewL();
    TInt count = media->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CMPXMedia* orig( media->AtL( i ) );
        User::LeaveIfNull( orig );
        CMPXMedia* copy( CMPXMedia::NewL( *orig ) );
        CleanupStack::PushL( copy );
        iCategoryArray->AppendL( copy );
        CleanupStack::Pop( copy );
        }
    if ( count > 0 )
        {
        // change display text to unknown if the last entry is null
        CMPXMedia* aryMedia( media->AtL( count - 1 ) );
        User::LeaveIfNull( aryMedia );
        if ( aryMedia->ValueText( KMPXMediaGeneralTitle ).Length() == 0 )
            {
            HBufC* text = StringLoader::LoadLC(
                R_MPX_QTN_MP_UNKNOWN );
            CMPXMedia* catMedia( iCategoryArray->AtL( count - 1 ) );
            User::LeaveIfNull( catMedia );
            catMedia->SetTextValueL( KMPXMediaGeneralTitle, *text );
            aryMedia->SetTextValueL( KMPXMediaGeneralTitle, *text );
            CleanupStack::PopAndDestroy( text );
            }
        CMPXMedia* entry = CMPXMedia::NewL();
        CleanupStack::PushL( entry );
        HBufC* string =
            StringLoader::LoadLC( R_MPX_CUI_ADDSONGS_ADD_ALLSONGS_TXT );
        entry->SetTextValueL(
            KMPXMediaGeneralTitle, *string );
        media->Insert( entry, 0 );
        CleanupStack::PopAndDestroy( string );
        CleanupStack::Pop( entry );
        // Insert artists to model
        iListModel->InsertGroupsL( *media, -1, EMPXSong );
        HandleItemArrayChangeL();
        iSongAllGroupExist = ETrue;
        }
    else
        {
        MPX_DEBUG1( "CMPXAddTracksDialog::UpdateListboxByCategoryL no song" );
        // no song
        iSongAllGroupExist = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::UpdateListboxByCategoryforFindL()
// Update listbox content by category after find.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::UpdateListboxByCategoryforFindL(
    TBool aFind, TInt aSongNum)
    {
    MPX_FUNC( "CMPXAddTracksDialog::UpdateListboxByCategoryforFindL" );
    if( aFind )
        {
        if( aSongNum != 0 )
            {
            // Insert "Songs (%N)" to list box model, change "%N" to matched songs' number
            CMPXMediaArray* media = CMPXMediaArray::NewL();
            CleanupStack::PushL( media );
            CMPXMedia* entry = CMPXMedia::NewL();
            CleanupStack::PushL( entry );
            HBufC* string = string = StringLoader::LoadLC(
                            R_MPX_CUI_ADDSONGS_ADD_SONGS_NUM_TXT, aSongNum );
            entry->SetTextValueL( KMPXMediaGeneralTitle, *string );
            media->AppendL( entry );
            iListModel->InsertGroupsL( *media, 0, EMPXSong );
            CleanupStack::PopAndDestroy( string );
            CleanupStack::Pop( entry );
            CleanupStack::PopAndDestroy( media );
            iSongAllGroupExist = ETrue;
            }
        else
            {
            iSongAllGroupExist = EFalse;
            }

        // Insert found groups
        TInt matchedGroupCount = iMatchedGroupArray.Count();
        if( matchedGroupCount > 0 )
            {
            CMPXMediaArray* media = CMPXMediaArray::NewL();
            CleanupStack::PushL( media );
            for ( TInt i = 0; i < matchedGroupCount; i++ )
                {
                TInt catIndex = iMatchedGroupArray[ i ];
                CMPXMedia* catMedia( iCategoryArray->AtL( catIndex ) );
                User::LeaveIfNull( catMedia );
                CMPXMedia* entry = CMPXMedia::NewL( *catMedia  );
                CleanupStack::PushL( entry );
                media->AppendL( entry );
                CleanupStack::Pop( entry );
                }
            if( !iSongAllGroupExist )
                {
                iListModel->InsertGroupsL( *media, 0 );
                }
            else
                {
                iListModel->InsertGroupsL( *media, 1 );
                }
            CleanupStack::PopAndDestroy( media );
            }
        }
    else
        {
        iListModel->ResetTracksList();
        }

    HandleItemArrayChangeL();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::CurrentLbxItemIndex
// Gets current listbox item index.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksDialog::CurrentLbxItemIndex() const
    {
    MPX_FUNC( "CMPXAddTracksDialog::CurrentLbxItemIndex" );
    CTextListBoxModel* lbxModel = iListBox->Model();
    CAknSearchField* findBox = FindBox();
    TInt currentItem = iListBox->CurrentItemIndex();
    TInt count = lbxModel->NumberOfItems();

    // Make sure there's at least one item in the listbox
    if ( findBox && count )
        {
        CAknFilteredTextListBoxModel* filteredLbxModel =
            static_cast<CAknFilteredTextListBoxModel*>( lbxModel );
        CAknListBoxFilterItems* filterItems = filteredLbxModel->Filter();

        // Get the real listbox index
        currentItem = filterItems->FilteredItemIndex( currentItem );
        }
    return currentItem;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::CurrentListItemCount
// Return number of listbox items shown in the current listbox.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksDialog::CurrentListItemCount() const
    {
    MPX_FUNC( "CMPXAddTracksDialog::CurrentListItemCount" );
    CTextListBoxModel* lbxModel = iListBox->Model();
    TInt count = lbxModel->NumberOfItems();
    CAknSearchField* findBox = FindBox();

    if ( findBox && count )
        {
        CAknFilteredTextListBoxModel* filteredLbxModel =
            static_cast<CAknFilteredTextListBoxModel*>( lbxModel );
        CAknListBoxFilterItems* filterItems = filteredLbxModel->Filter();

        count = filterItems->FilteredNumberOfItems();
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::HandleItemArrayChangeL
// When you change the list item array you should call this method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleItemArrayChangeL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::HandleItemArrayChangeL" );
    TInt currentIdx = iListBox->CurrentItemIndex();
    TInt topIdx = iListBox->TopItemIndex();
    CListBoxView* lbxView = iListBox->View();
    TInt offset =
        lbxView->NumberOfItemsThatFitInRect( lbxView->ViewRect() ) - 1;

    CAknFilteredTextListBoxModel* lbxModel =
        static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
    CAknSearchField* findBox = FindBox();
    lbxModel->Filter()->SetSearchField( NULL );//for find the items
    // Update item array
    iLbxFilterItems->HandleItemArrayChangeL();
    lbxModel->Filter()->SetSearchField( findBox );// for laylout of list

    TInt itemCount = CurrentListItemCount();
    if ( itemCount )
        {
        TInt lastItemIdx = itemCount - 1;
        if ( topIdx + offset > lastItemIdx )
            {
            topIdx = lastItemIdx - offset;
            }
        if ( topIdx < 0 )
            {
            topIdx = 0;
            }
        if ( currentIdx < 0 )
            {
            currentIdx = 0;
            }
        if ( currentIdx > lastItemIdx )
            {
            currentIdx = lastItemIdx;
            }

        iListBox->SetTopItemIndex( topIdx );
        iListBox->SetCurrentItemIndexAndDraw( currentIdx );
        iListBox->DrawDeferred();
        }
    UpdateSoftkeyL();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::HighlightListItem
// Highlight list item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HighlightListItem( TInt aIndex )
    {
    MPX_FUNC( "CMPXAddTracksDialog::HighlightListItem" );
    const TInt KMPItemOffset = 1;
    TInt itemCount = CurrentListItemCount();

    if ( itemCount )
        {
        TInt bottomIdx = iListBox->BottomItemIndex();
        TInt topIdx = iListBox->TopItemIndex();

        // Rearrange the index if aIndex is the bottom item
        if ( aIndex == bottomIdx )
            {
            TInt lastItemIdx = itemCount - 1;
            if ( aIndex != lastItemIdx )
                {
                topIdx += KMPItemOffset;
                if ( topIdx > lastItemIdx )
                    {
                    topIdx = lastItemIdx;
                    }
                }
            }

        if ( topIdx < 0 )
            {
            topIdx = 0;
            }

        iListBox->SetTopItemIndex( topIdx );
        iListBox->SetCurrentItemIndexAndDraw( aIndex );
        iListBox->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::ExpandCurrentGroupL()
// Expand current group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::ExpandCurrentGroupL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::ExpandCurrentGroupL" );
    TInt index = -1;
    if ( iExpandAllSongs )
        {
        index = EMPXAddTracksAddAllSongs;
        }
    else
        {
        index = CurrentLbxItemIndex();
        }
    TInt oldIndex = index;

    // Make sure current item is expandable
    if ( ( index != KErrNotFound ) &&
        ( iListModel->TrackTypeL( index ) == EMPXGroup ) &&
        ( !iListModel->IsExpandedIndex( index ) ) )
        {
        // Make sure the expanded group count is
        // under KMPXATMaxExpandedGroupNum.
        while ( iListModel->ExpandedGroupsCount() >=
                KMPXATMaxExpandedGroupNum )
            {
            // Remove all tracks from the oldest expanded group
            oldIndex = iListModel->OldestExpandedGroupIndex();
            TInt count = iListModel->RemoveAllTracksFromGroupL( oldIndex );
            // Update the index accordingly
            if ( index > oldIndex )
                {
                index -= count;
                }
            }

        if ( index == EMPXAddTracksAddAllSongs && iSongAllGroupExist )
            {
            if ( iAllSongsArray )
                {
                // Insert tracks for all songs
                iExpandAllSongs = EFalse;
                TInt matchedSongCount = iMatchedSongArray.Count();
                if ( matchedSongCount == 0 )
                    {
                    iListModel->InsertTracksUnderGroupL( *iAllSongsArray, index );
                    TInt diff = oldIndex - index;
                    TInt newCurrentIdx = iListBox->CurrentItemIndex() - diff;
                    if ( newCurrentIdx < 0 )
                        {
                        newCurrentIdx = 0;
                        }
                    TInt newTopIdx = iListBox->TopItemIndex() - diff;
                    if ( newTopIdx < 0 )
                        {
                        newTopIdx = 0;
                        }
                    iListBox->SetTopItemIndex( newTopIdx );
                    iListBox->SetCurrentItemIndex( newCurrentIdx );
                    }
                else
                    {
                    TInt matchedSongCount = iMatchedSongArray.Count();
                    for ( TInt i = 0; i < matchedSongCount; i++ )
                        {
                        TInt songIndex = iMatchedSongArray[ i ];
                        CMPXMedia* origMedia( iAllSongsArray->AtL( songIndex ) );
                        User::LeaveIfNull( origMedia );
                        iListModel->InsertTrackL( *origMedia, index + 1 + i );
                        }
                    iListModel->MarkExpandedL( index, matchedSongCount );
                    }
                HandleItemArrayChangeL();
                HighlightListItem( index );
                UpdateSoftkeyL();
                }
            else
                {
                MPX_DEBUG1( "CMPXAddTracksDialog::ExpandCurrentGroupL all songs NOT READY" );
                iExpandAllSongs = ETrue;
                HBufC* string = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_NOTE_OPENING );
                iCommonUiHelper->ShowWaitNoteL( *string,
                    R_AVKON_SOFTKEYS_EMPTY, EFalse, NULL );
                CleanupStack::PopAndDestroy( string );
                }
            }
        else
            {
            if ( iCurrentOp == EMPXOpIdle )
                {
                HandleItemArrayChangeL();
                HBufC* string = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_NOTE_OPENING );
                iCommonUiHelper->ShowWaitNoteL( *string,
                    R_AVKON_SOFTKEYS_EMPTY, EFalse, NULL );
                CleanupStack::PopAndDestroy( string );
                CMPXMedia* media = CMPXMedia::NewL();
                CleanupStack::PushL( media );
                media->SetTObjectValueL<TMPXGeneralType>(
                    KMPXMediaGeneralType, EMPXGroup );
                media->SetTObjectValueL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory, EMPXSong );
                media->SetTObjectValueL<TMPXItemId>(
                    KMPXMediaGeneralId, iListModel->TrackIdL( index ) );
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL( attrs );
                attrs.Append(
                    TMPXAttribute( KMPXMediaIdGeneral,
                        EMPXMediaGeneralTitle | EMPXMediaGeneralId
                        | EMPXMediaGeneralType |  EMPXMediaGeneralCategory |
                        EMPXMediaGeneralFlags ) );
                iCurrentOp = EMPXOpGetSongs;
                iCurrentGroupIndex = index;
                iCollectionUtility->Collection().FindAllL( *media, attrs.Array(), *this );
                CleanupStack::PopAndDestroy( &attrs );
                CleanupStack::PopAndDestroy( media );
                }
            else
                {
                MPX_DEBUG1( "CMPXAddTracksDialog::ExpandCurrentGroupL find all busy, ignoring" );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::GotoClosestGroupL()
// Goto the closest group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::GotoClosestGroupL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::GotoClosestGroupL" );
    // Get the closest expanded index
    TInt origIndex = CurrentLbxItemIndex();
    if ( origIndex != KErrNotFound )
        {
        TInt index = iListModel->ClosestExpandedGroupIdx( origIndex );
        if ( index != KErrNotFound )
            {
            // Highlight on the group
            HighlightListItem( index );

            UpdateSoftkeyL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::CollapseCurrentGroupL()
// Collapse current group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::CollapseCurrentGroupL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::CollapseCurrentGroupL" );
    TInt index = CurrentLbxItemIndex();

    if ( ( index != KErrNotFound ) &&
        ( iListModel->IsExpandedIndex( index ) ) )
        {
        // Remove all tracks from this group
        iListModel->RemoveAllTracksFromGroupL( index );

        // Update listbox and softkey
        HandleItemArrayChangeL();
        HighlightListItem( index );
        UpdateSoftkeyL();
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::AddCurrentTrackToPlaylistL()
// Add current track to playlist.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::AddCurrentTrackToPlaylistL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::AddCurrentTrackToPlaylistL" );
    TInt index = CurrentLbxItemIndex();
    iSelectIndex = index;

    if ( index != KErrNotFound && iPlaylistId != 0 )
        {
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL<TMPXItemId>(
            KMPXMediaGeneralId, iPlaylistId );
        media->SetTObjectValueL<TMPXGeneralType>(
            KMPXMediaGeneralType, EMPXItem );
        media->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXPlaylist );

        CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
        CleanupStack::PushL( mediaArray );
        CMPXMedia* song = CMPXMedia::NewL( iListModel->MediaL( index ) );
        CleanupStack::PushL( song );
        mediaArray->AppendL( song );
        CleanupStack::Pop( song );
        media->SetCObjectValueL( KMPXMediaArrayContents, mediaArray );
        media->SetTObjectValueL( KMPXMediaArrayCount, 1 );
        iCollectionUiHelper->AddL( *media, this );
        CleanupStack::PopAndDestroy( 2, media );

        // Enable animation
        // Animation icon index is 3 -> 5
        iAnimIconIndex = CMPXAddTracksLbxArray::EMPXATLbxIconSongAddedAnim1;
        iListModel->SetAnimationIconNum( iSelectIndex, iAnimIconIndex );
        HighlightListItem( iSelectIndex );

        // first icon animation
        if ( iPeriodic->IsActive() )
            {
            iPeriodic->Cancel();
            }
        iPeriodic->Start( TTimeIntervalMicroSeconds32(KAnimationFrame1Timer),
                            TTimeIntervalMicroSeconds32(KAnimationFrameTimerInterval),
                            TCallBack( AnimExpireL, this ) );
        }
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::DoFindL
// Execute find for group and songs.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::DoFindL( void )
    {
    MPX_FUNC( "CMPXAddTracksDialog::DoFindL" );

    if ( !iAllSongsArray )
        {
        iDoFind = ETrue;
        HBufC* string = StringLoader::LoadLC(
            R_MPX_QTN_NMP_NOTE_OPENING );
        iCommonUiHelper->ShowWaitNoteL( *string,
            R_AVKON_SOFTKEYS_EMPTY, EFalse, NULL );
        CleanupStack::PopAndDestroy( string );
        return;
        }

    // reset all group to default state
    iListModel->ResetTracksList();

    iMatchedSongArray.Reset();
    iMatchedGroupArray.Reset();

    // start find
    TInt searchTextNum = iLastFindTxt->Length();
    TInt songAllMatchedNum = 0;

    TBufC<KMaxTitleLength> bufSearchText;
    TPtr searchText = bufSearchText.Des();
    TBufC<KMaxTitleLength> bufSearchTextwithSpace;
    TPtr searchTextwithSpace = bufSearchTextwithSpace.Des();
    TBool find = EFalse;


    // Reset find result
    iListModel->SetFindSongsFlag( EFalse );
    // current searching is to compare the text string with list box text
    // if they are match, means found;otherwise, not found
    // searching way can be changed if needed
    if( searchTextNum != 0 )
        {
        TInt listCount = iCategoryArray->Count();
        for( TInt i = 0; i < listCount; i ++ )
            {
            CMPXMedia* media( iCategoryArray->AtL( i ) );
            User::LeaveIfNull( media );
            const TDesC& constTitle = media->ValueText(
                KMPXMediaGeneralTitle );
            TBufC<KMaxTitleLength> titleBuf( constTitle );
            TPtr title = titleBuf.Des();
            searchText = iLastFindTxt->Left( searchTextNum );

            title.LowerCase();
            searchText.LowerCase();

            if( title.Find( KMPXSpace ) != KErrNotFound )
                {
                searchTextwithSpace = searchText;
                // insert space if there is not, to make every search same
                searchTextwithSpace.Insert( 0, KMPXSpace );
                }

            // find way: match first several characters, if there is space in the
            // middle of the track name, match the string also after the space
            if ( ( ( searchTextwithSpace.Length() > 0 ) &&
                ( title.Find( searchTextwithSpace ) != KErrNotFound ) ) ||
                ( title.Left( searchTextNum ) == searchText ) )
                {
                iMatchedGroupArray.Append( i );
                find = ETrue;
                }
            }

        TInt songCount = iAllSongsArray->Count();
        for ( TInt i = 0; i < songCount; i++ )
            {
            CMPXMedia* media( iAllSongsArray->AtL( i ) );
            User::LeaveIfNull( media );
            const TDesC& constTitle = media->ValueText(
                KMPXMediaGeneralTitle );
            TBufC<KMaxTitleLength> titleBuf( constTitle );
            TPtr title = titleBuf.Des();
            searchText = iLastFindTxt->Left( searchTextNum );

            title.LowerCase();
            searchText.LowerCase();

            if( title.Find( KMPXSpace ) != KErrNotFound )
                {
                searchTextwithSpace = searchText;
                // insert space if there is not, to make every search same
                searchTextwithSpace.Insert( 0, KMPXSpace );
                }

            // find way: match first several characters, if there is space in the
            // middle of the track name, match the string also after the space
            if ( ( ( searchTextwithSpace.Length() > 0 ) &&
                ( title.Find( searchTextwithSpace ) != KErrNotFound ) ) ||
                ( title.Left( searchTextNum ) == searchText ) )
                {
                // matched songs
                songAllMatchedNum ++;
                iMatchedSongArray.Append( i );
                find = ETrue;
                iListModel->SetFindSongsFlag( ETrue );
                }
            }
        // update according to searching result
        UpdateListboxByCategoryforFindL( find, songAllMatchedNum );
        }
    else if ( iAllSongsArray->Count() > 0 )
        {
        // Songs(All) node is shown
        iListModel->SetFindSongsFlag( ETrue );
        CMPXMediaArray* media = CMPXMediaArray::NewL();
        CleanupStack::PushL( media );
        CMPXMedia* entry = CMPXMedia::NewL();
        CleanupStack::PushL( entry );
        HBufC* string =
            StringLoader::LoadLC( R_MPX_CUI_ADDSONGS_ADD_ALLSONGS_TXT );
        entry->SetTextValueL( KMPXMediaGeneralTitle, *string );
        media->Insert( entry, 0 );
        CleanupStack::PopAndDestroy( string );
        CleanupStack::Pop( entry );
        iListModel->InsertGroupsL( *media, 0, EMPXSong );
        iListModel->InsertGroupsL( *iCategoryArray, 1 );
        CleanupStack::PopAndDestroy( media );
        iSongAllGroupExist = ETrue;
        HandleItemArrayChangeL();
        }

    // Always highlight the first item
    HighlightListItem( 0 );
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::DoAnimL
// Execute animation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::DoAnimL(void)
    {
    MPX_FUNC( "CMPXAddTracksDialog::DoAnimL" );
    iPeriodic->Cancel();

    // remove animation icon
    if( iAnimIconIndex ==
            CMPXAddTracksLbxArray::EMPXATLbxIconSongAddedAnim4 )
        {
        return;
        }
    iAnimIconIndex ++;
    iListModel->SetAnimationIconNum( iSelectIndex, iAnimIconIndex );
    HighlightListItem(iSelectIndex);

    TTimeIntervalMicroSeconds32 interval = 0;
    if( iAnimIconIndex ==
                CMPXAddTracksLbxArray::EMPXATLbxIconSongAddedAnim2 )
        {
        interval = KAnimationFrame2Timer;
        }
    else if(iAnimIconIndex ==
                CMPXAddTracksLbxArray::EMPXATLbxIconSongAddedAnim3 )
        {
        interval = KAnimationFrame3Timer;
        }
    else if(iAnimIconIndex ==
                CMPXAddTracksLbxArray::EMPXATLbxIconSongAddedAnim4 )
        {
        interval = KAnimationFrame4Timer;
        }
    iPeriodic->Start( interval,
                    TTimeIntervalMicroSeconds32(KAnimationFrameTimerInterval ),
                    TCallBack( AnimExpireL, this ) );

    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::AnimExpireL
// Animation call back method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMPXAddTracksDialog::AnimExpireL( TAny* aObject )
    {
    MPX_FUNC( "CMPXAddTracksDialog::AnimExpireL" );
    ( ( CMPXAddTracksDialog* )( aObject ) )->DoAnimL();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::StopAnimL
// Stop animation.
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::StopAnimL()
    {
    if( ( iPeriodic->IsActive() ||
        iAnimIconIndex ==
            CMPXAddTracksLbxArray::EMPXATLbxIconSongAddedAnim4 ) )
        {
        iAnimIconIndex = 0;
        iPeriodic->Cancel();
        iListModel->SetAnimationIconNum( iSelectIndex, 0 );
        TInt bottomIdx = iListBox->BottomItemIndex();
        TInt topIdx = iListBox->TopItemIndex();
        if ( topIdx <= iSelectIndex && iSelectIndex <= bottomIdx )
            {
            HighlightListItem( iSelectIndex );
            }
        // restore origional navi pane text
        iNaviPane->Pop( iNaviLabelPane );
        iNaviPane->PushDefaultL();
        }
    }

// ---------------------------------------------------------------------------
// load resources
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::LoadResourceL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::LoadResourceL" );
    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXAddTracksDialogRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );
    }

// ---------------------------------------------------------------------------
// start find categories
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::LoadCategoriesL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::LoadCategoriesL" );
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXGroup );
    media->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXArtist );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append(
        TMPXAttribute( KMPXMediaIdGeneral,
            EMPXMediaGeneralTitle | EMPXMediaGeneralId
            | EMPXMediaGeneralType |  EMPXMediaGeneralCategory |
            EMPXMediaGeneralFlags ) );
    iCurrentOp = EMPXOpGetCategory;
    iCollectionUtility->Collection().FindAllL( *media, attrs.Array(), *this );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( media );
    }

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::DoHandleCollectionMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXAddTracksDialog::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        if( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) ==
            TMPXCollectionMessage::EBroadcastEvent )
            {
            TInt op( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
            if ( op == EMcMsgUSBMTPEnd || op == EMcMsgRefreshEnd || op == EMcMsgUSBMassStorageEnd )
                {
                MPX_DEBUG1( "CMPXAddTracksDialog::DoHandleCollectionMessageL Library updated, reloading" );
                // library updated, reload
                LoadCategoriesL();
                }
            else if ( op == EMcMsgDiskRemoved )
                {
                // since collection view can't close this dialog due to CMPXCollectionViewImp::DoIncrementalOpenL
                // later, schedule the dialog to be closed by itself
                iIdle = CIdle::NewL( CActive::EPriorityStandard );
                iIdle->Start( TCallBack( CMPXAddTracksDialog::DeferredExitCallback, this ) );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection message
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleCollectionMessage(
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
void CMPXAddTracksDialog::HandleOpenL(
    const CMPXMedia& /*aEntries*/,
    TInt /*aIndex*/,
    TBool /*aComplete*/,
    TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d an item. Client typically responds by
// 'playing' the item
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/ )
    {
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection media
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleCollectionMediaL(
    const CMPXMedia& aMedia, TInt aError)
    {
    MPX_FUNC( "CMPXAddTracksDialog::HandleCollectionMediaL" );

    if ( aError == KErrNone )
        {
        iPlaylistId = aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        MPX_DEBUG2( "CMPXAddTracksDialog::HandleCollectionMediaL playlist ID = 0x%x", iPlaylistId.iId1 );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionFindObserver
// Handle callback for "find" operation
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleFindAllL(
    const CMPXMedia& aResults, TBool /*aComplete*/, TInt aError )
    {
    MPX_DEBUG2( "-->CMPXAddTracksDialog::HandleFindAllL aError = %d", aError );

    if ( aError == KErrNone )
        {
        switch ( iCurrentOp )
            {
            case EMPXOpGetCategory:
                {
                UpdateListboxByCategoryL( aResults );
                CMPXMedia* media = CMPXMedia::NewL();
                CleanupStack::PushL( media );
                media->SetTObjectValueL<TMPXGeneralType>(
                    KMPXMediaGeneralType, EMPXGroup );
                media->SetTObjectValueL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory, EMPXSong );
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL( attrs );
                attrs.Append(
                    TMPXAttribute( KMPXMediaIdGeneral,
                        EMPXMediaGeneralTitle | EMPXMediaGeneralId
                        | EMPXMediaGeneralType |  EMPXMediaGeneralCategory
                        | EMPXMediaGeneralFlags ) );
                iCurrentOp = EMPXOpGetAllSongs;
                iCollectionUtility->Collection().FindAllL( *media, attrs.Array(), *this );
                CleanupStack::PopAndDestroy( &attrs );
                CleanupStack::PopAndDestroy( media );
                break;
                }
            case EMPXOpGetAllSongs:
                {
                delete iAllSongsArray;
                iAllSongsArray = NULL;
                CMPXMediaArray* mediaAry(
                    const_cast<CMPXMediaArray*>( aResults.Value<CMPXMediaArray>(
                        KMPXMediaArrayContents ) ) );
                User::LeaveIfNull( mediaAry );
                iAllSongsArray = CMPXMediaArray::NewL( *mediaAry );
                iCurrentOp = EMPXOpIdle;
                if ( iExpandAllSongs )
                    {
                    iCommonUiHelper->DismissWaitNoteL();
                    iDoFind = EFalse;
                    ExpandCurrentGroupL();
                    }
                else if ( iDoFind )
                    {
                    iCommonUiHelper->DismissWaitNoteL();
                    iDoFind = EFalse;
                    DoFindL();
                    }
                break;
                }
            case EMPXOpGetSongs:
                {
                CMPXMediaArray* media =
                    const_cast<CMPXMediaArray*>(aResults.Value<CMPXMediaArray>(
                        KMPXMediaArrayContents ) );
                User::LeaveIfNull( media );
                iListModel->InsertTracksUnderGroupL( *media, iCurrentGroupIndex );

                TInt diff = iListModel->OldestExpandedGroupIndex() - iCurrentGroupIndex;
                TInt newCurrentIdx = iListBox->CurrentItemIndex() - diff;
                if ( newCurrentIdx < 0 )
                    {
                    newCurrentIdx = 0;
                    }
                TInt newTopIdx = iListBox->TopItemIndex() - diff;
                if ( newTopIdx < 0 )
                    {
                    newTopIdx = 0;
                    }
                iListBox->SetTopItemIndex( newTopIdx );
                iListBox->SetCurrentItemIndex( newCurrentIdx );
                HandleItemArrayChangeL();
                HighlightListItem( iCurrentGroupIndex );
                UpdateSoftkeyL();
                iCurrentOp = EMPXOpIdle;
                iCommonUiHelper->DismissWaitNoteL();
                break;
                }
            default:
                {
                iCurrentOp = EMPXOpIdle;
                break;
                }
            }
        }
    MPX_DEBUG1( "<--CMPXAddTracksDialog::HandleFindAllL" );
    }

// ---------------------------------------------------------------------------
// From MMPXCHelperObserver
// Handles the completion of adding a playlist event
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleOperationCompleteL( TCHelperOperation /*aOperation*/,
                                                    TInt aError,
                                                    void* aArgument )
    {
    MPX_DEBUG2("-->CMPXAddTracksDialog::HandleOperationCompleteL error = %d", aError);
    if ( aError != KErrNone )
        {
        StopAnimL();
        // Fix for err EJKG-7CFD89 & EJKG-7DZGGL 
        if (aError != KErrNotReady && aError != KErrArgument )
            {
            iCommonUiHelper->HandleErrorL( aError );
            }
        }
    else
        {
        // Song added text display in navi pane
        iNaviPane->PushL( *iNaviDecorator );
        }
    CMPXMedia* media = (CMPXMedia*) aArgument;
    delete media;
    MPX_DEBUG1("<--CMPXAddTracksDialog::HandleOperationCompleteL");
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::PreLayoutDynInitL()
// Prepare for layout.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMPXAddTracksDialog::PreLayoutDynInitL()
    {
    MPX_FUNC( "CMPXAddTracksDialog::PreLayoutDynInitL" );
    // Create context icon
    CreateContextIconL();

    // Backup previous status pane
    BackupPreviousStatusPaneL();

    // Set new title pane
    HBufC* title = StringLoader::LoadLC( R_MPX_CUI_ADDSONGS_TITLE );
    iTitlePane->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    // Set find type
    CAknSelectionListDialog::PreLayoutDynInitL();

    // Setup listbox content
    iListBox = static_cast<CEikTextListBox*>( Control( ESelectionListControl ) );
    CAknFilteredTextListBoxModel*
    lbxModel = static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
    iLbxFilterItems = lbxModel->Filter();

    // Initialize last find text
    iLastFindTxt = HBufC::NewL( 0 );

    // Create tracks list model & listbox array
    iListModel = CMPXAddTracksListModel::NewL();
    CMPXAddTracksLbxArray* lbxArray = CMPXAddTracksLbxArray::NewL( iListModel );

    // Set item array to listbox model, transfer ownership
    CTextListBoxModel* listboxModel = iListBox->Model();
    listboxModel->SetItemTextArray( lbxArray );
    listboxModel->SetOwnershipType( ELbmOwnsItemArray );

    // Create and set icon array. Ownership transferred.
    CArrayPtr<CGulIcon>* icons = lbxArray->CreateIconArrayL();
    SetIconArrayL( icons );

    // Set scrollbar
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                        CEikScrollBarFrame::EAuto );

    // Enable Marquee
    static_cast<CEikColumnListBox*>( iListBox )->EnableExtendedDrawingL();
    static_cast<CEikColumnListBox*>( iListBox )->ItemDrawer()->
                                                ColumnData()->EnableMarqueeL( ETrue );

    // Enable speed scrolling
    iLbxExtFeat = CMPXLbxExtendedFeatures::NewL( iListBox, EFalse );
    iLbxExtFeat->EnableSpeedScrollL( ETrue );

    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    iNaviPane = static_cast<CAknNavigationControlContainer*>
                ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    // read the navigation pane text resource
    HBufC* text = StringLoader::LoadLC( R_MPX_CUI_ADDSONGS_SONG_ADDED );
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( *text );
    CleanupStack::PopAndDestroy( text );

    // Animation icon timer
    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );

    // Set empty text
    iListBox->View()->SetListEmptyTextL( *iNoSongText );
    iListModel->SetFindSongsFlag( ETrue );

    // get current playlist ID
    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cpath );
    cpath->Back();
    MPX_DEBUG_PATH( *cpath );
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL(attrs);
    attrs.Append( KMPXMediaGeneralId );
    iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( cpath );

    // Update listbox
    LoadCategoriesL();

    // Update softkey
    UpdateSoftkeyL();
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::OkToExitL
// Check if it's ok to exit.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMPXAddTracksDialog::OkToExitL( TInt aButtonId )
    {
    MPX_FUNC( "CMPXAddTracksDialog::OkToExitL" );
    MPX_DEBUG2( "CMPXAddTracksDialog::OkToExitL aButtonId = 0x%x", aButtonId );
    TBool retVal = EFalse;

    switch (aButtonId)
        {
        case EMPXCmdAddTracksExpand:
            {
            ExpandCurrentGroupL();
            break;
            }
        case EMPXCmdAddTracksCollapse:
            {
            CollapseCurrentGroupL();
            break;
            }
        case EMPXCmdAddTracksAdd:
            {
            // Add track to the playlist
            AddCurrentTrackToPlaylistL();
            break;
            }
        case EMPXCmdAddTracksDone:
            {
            retVal = ETrue;
            break;
            }
        case EMPXCmdAddTracksFindCancel:
            {
            CAknSearchField* findBox = FindBox();
            findBox->MakeVisible( EFalse );
            findBox->SetFocus( EFalse );
            TRAP_IGNORE( findBox->ResetL() );
            iListBox->SetFocus( ETrue );
            
            CAknFilteredTextListBoxModel* lbxModel =
                static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
            lbxModel->Filter()->SetSearchField( findBox );// for laylout of list
            TRect mainPane;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
            SetSizeAndPosition( mainPane.Size() );
            DrawDeferred();
            break;
            }
        default:
            {
            retVal = CAknSelectionListDialog::OkToExitL( aButtonId );
            break;
            }
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// CMPXAddTracksDialog::OfferKeyEventL
// Handles key-events.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CMPXAddTracksDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_FUNC( "CMPXAddTracksDialog::OfferKeyEventL" );
    TKeyResponse res = EKeyWasNotConsumed;
    TBool updateCBA = EFalse;
    CAknSearchField* findbox = FindBox();
    
    CAknFilteredTextListBoxModel* lbxModel =
        static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
    lbxModel->Filter()->SetSearchField( NULL );//for find the items

    if ( aType == EEventKey )
        {
        StopAnimL();

        switch ( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                updateCBA = ETrue;
                break;
                }
            case EKeyLeftArrow:
                {
                TInt index = CurrentLbxItemIndex();
                if ( index != KErrNotFound )
                    {
                    if ( iListModel->TrackTypeL( index ) != EMPXGroup )
                        {
                        GotoClosestGroupL();
                        }
                    else
                        {
                        CollapseCurrentGroupL();
                        }
                    }
                res = EKeyWasConsumed;
                break;
                }
            case EKeyRightArrow:
                {
                ExpandCurrentGroupL();
                res = EKeyWasConsumed;
                break;
                }
            case EKeyOK:
            case EKeyEnter:
                {
                // Do the right thing on right item
                TInt index = CurrentLbxItemIndex();
                if ( index != KErrNotFound )
                    {
                    if ( iListModel->TrackTypeL( index ) == EMPXGroup )
                        {
                        if ( iListModel->IsExpandedIndex( index ) )
                            {
                            CollapseCurrentGroupL();
                            }
                        else
                            {
                            ExpandCurrentGroupL();
                            }
                        }
                    else
                        {
                        AddCurrentTrackToPlaylistL();
                        }
                    }
                res = EKeyWasConsumed;
                break;
                }
            default:
                {
                break;
                }
            }
        }
    TBool offerEventToBaseClass( EFalse );

    if ( res == EKeyWasNotConsumed )
        {
        if ( ( aKeyEvent.iCode == EKeyUpArrow ) ||
            ( aKeyEvent.iScanCode == EStdKeyUpArrow ) ||
            ( aKeyEvent.iCode == EKeyDownArrow ) ||
            ( aKeyEvent.iScanCode == EStdKeyDownArrow ) )
            {
            res = iLbxExtFeat->HandleLbxKeyEventL( aKeyEvent, aType );
            }
        // handle right soft key
        else if( findbox->IsVisible() &&
            ( aKeyEvent.iCode == EKeyDevice1 ||
              aKeyEvent.iScanCode == EStdKeyDevice1 ) )
            {
            StopAnimL();
            res = EKeyWasConsumed;
            }
        else
            {
            if ( findbox )
                {
                TName text;
                findbox->GetSearchText( text );
                updateCBA = ETrue;
                if ( *iLastFindTxt != text )
                    {
                    // Update CBA if find text is changed
                    delete iLastFindTxt;
                    iLastFindTxt = NULL;
                    iLastFindTxt = text.AllocL();
                    DoFindL();
                    }
                if ( iLastFindTxt->Length() == 0 && aKeyEvent.iCode == EKeyBackspace )
                    {
                    CAknSearchField* findBox = FindBox();
                    findBox->MakeVisible( EFalse );
                    findBox->SetFocus( EFalse );
                    TRAP_IGNORE( findBox->ResetL() );
                    iListBox->SetFocus( ETrue );
                    
                    lbxModel->Filter()->SetSearchField( findBox );// for laylout of list
                    
                    TRect mainPane;
                    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
                    SetSizeAndPosition( mainPane.Size() );
                    iListBox->DrawDeferred();
                    res = EKeyWasConsumed;
                    }
                else
                    {
                    offerEventToBaseClass = ETrue;
                    }
                }
            else
                {
                offerEventToBaseClass = ETrue;
                }
            }
        }
    if ( updateCBA )
        {
        UpdateSoftkeyL();
        }
    if ( offerEventToBaseClass )
        {
        // this has to be done after cba is updated
        res = CAknSelectionListDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    return res;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleResourceChange( TInt aType )
    {
    MPX_FUNC( "CMPXAddTracksDialog::HandleResourceChange" );
    CAknSelectionListDialog::HandleResourceChange( aType );

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        CAknSearchField* findBox = FindBox();    
        CAknFilteredTextListBoxModel* lbxModel =
            static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
        lbxModel->Filter()->SetSearchField( findBox );// for laylout of list

        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        SetSizeAndPosition( mainPane.Size() );
        }

    if ( aType == KAknsMessageSkinChange )
        {
        // Create and set icon array. Ownership transferred.
        CTextListBoxModel* listboxModel = iListBox->Model();
        MDesCArray* lbxArray = listboxModel->ItemTextArray();
        CMPXAddTracksLbxArray* addtracksLbxArray =
            static_cast<CMPXAddTracksLbxArray*>( lbxArray );
        TRAP_IGNORE(
            CArrayPtr<CGulIcon>* icons = addtracksLbxArray->CreateIconArrayL();
            SetIconArrayL( icons );
            );
        }
    }

// ---------------------------------------------------------------------------
// From MEikListBoxObserver
// Handles listbox events.
// ---------------------------------------------------------------------------
//
void CMPXAddTracksDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType)
    {
    MPX_FUNC( "CMPXAddTracksDialog::HandleListBoxEventL" );
    StopAnimL();
    if ( aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked
#ifdef SINGLE_CLICK_INCLUDED
         || aEventType == EEventItemSingleClicked
#endif
        )
        {
		// Do the right thing on right item
		TInt index = CurrentLbxItemIndex();
		if ( index != KErrNotFound )
			{
			if ( iListModel->TrackTypeL( index ) == EMPXGroup )
				{
				if ( iListModel->IsExpandedIndex( index ) )
					{
					CollapseCurrentGroupL();
					}
				else
					{
					ExpandCurrentGroupL();
					}
				}
			else
				{
				AddCurrentTrackToPlaylistL();
				}
			}
        }
    else
	    {
	    iNaviPane->PushDefaultL();
        UpdateSoftkeyL();
	    }
    }

// ---------------------------------------------------------------------------
// Function called by CIdle to close the dialog itself when collection view
// can not handle the closing of dialog due to waitnote displayed by
// CMPXCollectionViewImp::DoIncrementalOpenL
// ---------------------------------------------------------------------------
//
TInt CMPXAddTracksDialog::DeferredExitCallback( TAny* aPtr )
    { 
    MPX_DEBUG1( "CMPXAddTracksDialog::DeferredExitCallback" );  
    CMPXAddTracksDialog* self = 
        static_cast<CMPXAddTracksDialog*>( aPtr );
    TRAP_IGNORE( self->TryExitL( EMPXCmdAddTracksDone ) );
    return KErrNone;
    }

// ProcessCommandL has been implemented here for the sole
// of handling RSK command when findbox is shown. For other
// cases it goes along the same path that is used by hardware
// keys, e.g. by call to TryToExitL.
void CMPXAddTracksDialog::ProcessCommandL( TInt aCommandId )
	{
	CAknSelectionListDialog::ProcessCommandL( aCommandId );
	TryExitL( aCommandId );
	
	// Check that the dialog is not being closed
	if( aCommandId != EMPXCmdAddTracksDone )
		{
		// Reset view so that all songs are shown
		TName text;
		CAknSearchField* findbox = FindBox();
		if( findbox )
			{
			findbox->GetSearchText( text );
			delete iLastFindTxt;
			iLastFindTxt = NULL;
			iLastFindTxt = text.AllocL();
			DoFindL();
			HandleItemArrayChangeL();
			}
		}
	}
//  End of File
