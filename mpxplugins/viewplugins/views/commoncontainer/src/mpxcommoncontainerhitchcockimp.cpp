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
* Description:  Implementation of common container for the
*                Hitchcock platform.
*
*/



// INCLUDE FILES
#include <barsread.h>
#include <stringloader.h>
#include <akniconarray.h>
#include <gulicon.h>
#include <featmgr.h>
#include <aknsfld.h>
#include <aknutils.h>
#include <aknlists.h>
#include <aknconsts.h>
#include <avkon.mbg>
#include <eikclbd.h>
#include <eikcmobs.h>
#include <bautils.h>
#include <glib/gstring.h> //for handling the Ustring memory leak
#include <aknview.h>

// Uiaccelerator Includes
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <alf/alfroster.h>
#include <alf/alfwidgetcontrol.h>   // To get CAlfControl from CAlfWidgetControl
#include <alf/alfwidgetenvextension.h>

#include <alf/ialfviewwidget.h>
#include <alf/ialfcontainerwidget.h>
#include <alf/ialfmap.h>
#include <alf/ialfwidgetfactory.h>  // Widget factory
#include <osn/ustring.h>          // ustring
#include <osn/osnnew.h>
#include <osn/alfptrvector.h>

// MUL Includes
#include <mul/imulwidget.h>             // For Templates
#include <mul/imullistwidget.h>         // List widget
#include <mul/imulmodel.h>              // Data model
#include <mul/mulvisualitem.h>          // For Visual Item
#include <mul/imulwidgetdef.h>

#include <data_caging_path_literals.hrh>
#include <mpxlog.h>
#include <mpxuser.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxpodcastdefs.h>
#include <mpxcommonuihelper.h>

#include <mpxcommoncontainer.rsg>

#include "mpxcommonlistboxarraybase.h"
#include "mpxcommoncontainermediahelper.h"
#include "mpxcommoncontainertexturemanager.h"
#include "mpxcommoncontainerhitchcockimp.h"
#include "mpxcommoncontainerfilteredlist.h"
#include "mpxcommoncontainer.hrh"

#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

using namespace Alf;
//using namespace mulvisualitem;

// CONSTANTS
//const TInt KMPXFindboxTextLimit = 40;
const TInt KMPXListSelectionCount = 5;
const TInt KListControlGroupIdMusic = 270531711;  // Need to be unique. Using music collection dll UID.
const TInt KListControlGroupIdPodcast = 270531738;  // Need to be unique. Using podcast collection dll UID.

const TInt KMPXMinSecSeparatorIndex = 2;
const TInt KOneHourInSeconds = 3600;
const TInt KMPXTimeIndicatorLength = 16;
_LIT( KMPXDash, " - " );
_LIT( KMPXZeroDurationMark, "--");
_LIT( KMPXAlbumMimeType, "image/jpeg" );

// AK - Temporary Begin - These need to be localized
_LIT( KUnknown, "Unknown" );
_LIT( KSong, " song" );
_LIT( KSongs, " songs" );
_LIT( KEpisode, " episode" );
_LIT( KEpisodes, " episodes" );
_LIT( KAlbum, " album" );
_LIT( KAlbums, " albums" );
_LIT( KSongDash, " song - " );
_LIT( KSongsDash, " songs - " );
// AK - Temporary End

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CMPXCommonContainerHitchcockImp::CMPXCommonContainerHitchcockImp() :
    iContext( EContextUnknown ),
    iIsFindEnabled( EFalse ),
    iIsMarkingEnabled( ETrue ),
    iTopIndex( KErrNotFound ),
    iBottomIndex( KErrNotFound ),
    iAlfEnvCreated( EFalse ),
    iContainerActive( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCommonContainerHitchcockImp::~CMPXCommonContainerHitchcockImp()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::~CMPXCommonContainerHitchcockImp" );

    FeatureManager::UnInitializeLib();

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }

    if ( iListWidget )
        {
        iListWidget->RemoveEventHandler(*this);
        }
//    delete iEnv;

    delete iListSelections;
    delete iListBoxArray;
    delete iIconArray;

    if ( iTextureManager )
        {
        iTextureManager->ClearCache();
        }
    delete iTextureManager;
    delete iCommonUiHelper;

    delete iThumbnailManager;
    iThumbnailReqArray.Close();

    delete iFilteredList;
    delete iSearchFieldWrapper;
    }

// ---------------------------------------------------------------------------
// Return Coe Control
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXCommonContainerHitchcockImp::CoeControl()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::CoeControl" );
    return this;
    }

// ---------------------------------------------------------------------------
// Set View Container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetViewContainer(
    MMPXViewContainer* aViewContainer )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetViewContainer" );
    iViewContainer = aViewContainer;
    }

// ---------------------------------------------------------------------------
// Set command observer for the container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetCommandObserver(
    MEikCommandObserver* aCommandObserver )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetCommandObserver" );
    iView = aCommandObserver;
    }

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetListBoxObserver(
    MEikListBoxObserver* aListBoxObserver )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetListBoxObserver" );
    iListBoxObserver = aListBoxObserver;
    }

// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::UpdateLayout()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::UpdateLayout" );
    }

// -----------------------------------------------------------------------------
// Returns a pointer to the listboxarray
// -----------------------------------------------------------------------------
//
CMPXCommonListBoxArrayBase* CMPXCommonContainerHitchcockImp::ListBoxArray() const
    {
    return iListBoxArray;
    }

// -----------------------------------------------------------------------------
// Sets listbox array
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetListBoxArrayL(
    CMPXCommonListBoxArrayBase* aListBoxArray )
    {
    iListBoxArray = aListBoxArray;
    }

// -----------------------------------------------------------------------------
// Return number of listbox items shown in the current listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHitchcockImp::CurrentListItemCount() const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::CurrentListItemCount" );
    TInt count = 0;
    if ( iWidgetModel )
        {
        count = iWidgetModel->Count();
        }
    return count;
    }

// -----------------------------------------------------------------------------
// Return total number of listbox items in the listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHitchcockImp::TotalListItemCount() const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::TotalListItemCount" );
    TInt count = 0;
    if ( iWidgetModel )
        {
        count = iWidgetModel->Count();
        }
    return count;
    }

// -----------------------------------------------------------------------------
// Gets top listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHitchcockImp::TopLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::TopLbxItemIndex" );
    return iTopIndex;
    }

// -----------------------------------------------------------------------------
// Gets bottom listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHitchcockImp::BottomLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::BottomLbxItemIndex" );
    return iBottomIndex;
    }

// -----------------------------------------------------------------------------
// Gets current listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHitchcockImp::CurrentLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::CurrentLbxItemIndex" );
    const TInt count = CurrentListItemCount();
    TInt index( KErrNotFound );
    if( count > 0 )
        {
        index = iWidgetModel->Highlight();
        if ( iFilteredList )
            {
            index = iFilteredList->FilteredIndexToRealIndex(index);
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// Get current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
const CArrayFix<TInt>* CMPXCommonContainerHitchcockImp::
    CurrentSelectionIndicesL() const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::CurrentSelectionIndicesL" );
    iListSelections->Reset();
    if ( iWidgetModel )
        {
        std::vector<int> itemsSelected = iWidgetModel->MarkedIndices();
        int count = itemsSelected.size();
        if ( iFilteredList )
            {
            for( int i = 0; i < count; i++ )
                {
                TInt index = iFilteredList->FilteredIndexToRealIndex( static_cast<TInt>( itemsSelected[i] ) );
                iListSelections->AppendL( index );
                }
            }
        else
            {
            for( int i = 0; i < count; i++ )
                {
                iListSelections->AppendL( static_cast<TInt>( itemsSelected[i] ) );
                }
            }
        }
    return iListSelections;
    }

// -----------------------------------------------------------------------------
// Get current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetCurrentSelectionIndicesL(
    CArrayFix<TInt>* aIndices ) const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetCurrentSelectionIndicesL" );
    TInt count = aIndices->Count();
    if ( count > 0 && iWidgetModel )
        {
        std::vector<int> itemsToSelect;
        for( TInt i = 0; i < count; i++ )
            {
            itemsToSelect.push_back( (*aIndices)[i] );
            }
        iWidgetModel->SetMarkedIndices(MulWidgetDef::EMulMark, itemsToSelect);
        }
    }

// -----------------------------------------------------------------------------
// Set top listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetLbxTopItemIndex( TInt /*aIndex*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetLbxTopItemIndex" );
    // Not required for Hitchcock
    }

// -----------------------------------------------------------------------------
// Set current listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetLbxCurrentItemIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetLbxCurrentItemIndex" );
    if ( iWidgetModel )
        {
        iWidgetModel->SetHighlight( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// Set current listbox item index and highlight it.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetLbxCurrentItemIndexAndDraw(
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetLbxCurrentItemIndexAndDraw" );
    if ( iWidgetModel )
        {
        if ( (aIndex >= 0) && (aIndex < iWidgetModel->Count()) )
            {
            iWidgetModel->SetHighlight( aIndex );
            }
        }
    }

// -----------------------------------------------------------------------------
// Clear listbox selection.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ClearLbxSelection()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::ClearLbxSelection" );
    if ( iWidgetModel )
        {
        iWidgetModel->SetMarkedIndices(MulWidgetDef::EMulUnmarkAll);
        }
    }

// -----------------------------------------------------------------------------
// Sets the text for an empty list box
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetLbxEmptyTextL( const TDesC& aText )
    {
    if ( aText == KNullDesC )
        {
        return;
        }
    if ( iListWidget )
        {
        HBufC8* buf = MPXUser::Alloc8ZL( aText );
        TPtr8 ptr( buf->Des() );
        UString emptyText( (char*)ptr.PtrZ() );
        CleanupStack::PushL( buf );
        iListWidget->SetEmptyText( emptyText );
        CleanupStack::PopAndDestroy( buf );
        }
    }

// -----------------------------------------------------------------------------
// Draws a list box item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::DrawLbxItem( TInt aIndex )
    {
    MPX_DEBUG2( "CMPXCommonContainerHitchcockImp::DrawLbxItem (aIndex=%d)", aIndex );

    // Just inform the model about the update. Model will request for data if necessary.
    // Ignore if find box is activated
    if ( iWidgetModel && !FindBoxVisibility() )
        {
        TInt count = iWidgetModel->Count();
        if ( aIndex >= 0 && aIndex < count )
            {
            iWidgetModel->Update(aIndex, 1);
            }
        }
    }

// -----------------------------------------------------------------------------
// Set Find box's focus.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetFindBoxFocus( TBool /*aFocus*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetFindBoxFocus" );
    // Not used
    }

// -----------------------------------------------------------------------------
// Handle listbox item addition.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleLbxItemAdditionL()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::HandleLbxItemAdditionL" );
    const char* KLoadNameModel("mulmodel");
    HandleLbxItemRemovalL();
    IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv);
    iWidgetModel = widgetFactory.createModel<IMulModel>( KLoadNameModel );
    iWidgetModel->SetDataProvider(*this);
    SetCollectionContextL();

    TInt count = 0;
    if ( iFilteredList )
        {
        iFilteredList->ReplaceMediaListL( iListBoxArray->MediaArray() );
        count = iFilteredList->FilteredCount();
        }
    else
        {
        count = iListBoxArray->MediaArray().Count();
        }
    if ( count > 0 )
        {
        iWidgetModel->Insert(0, count);
        }
    if ( iListWidget )
        {
        iListWidget->SetModel( iWidgetModel->Accessor() );
        }
    else
        {
        // Create list widget. CAlfEnv owns widget
        MPX_DEBUG1("CMPXCommonContainerHitchcockImp - Create List widget.");
        const char* KLoadNameList("mullistwidget");
        const char* KListWidgetName("mpxmullistwidget");
        const char* KListWidgetNamePod("mpxmullistwidgetpod");

        // Create view widget. CAlfEnv owns widget
        if ( iCollectionType == MMPXCommonContainer::EMPXCommonContainerMusic )
            {
            iListWidget = widgetFactory.createWidget<IMulListWidget>(KLoadNameList, KListWidgetName, *iViewWidget, NULL );
            }
        else if ( iCollectionType == MMPXCommonContainer::EMPXCommonContainerPodcast )
            {
            iListWidget = widgetFactory.createWidget<IMulListWidget>(KLoadNameList, KListWidgetNamePod, *iViewWidget, NULL );
            }

        iListWidget->AddEventHandler(*this);
        iListWidget->SetModel( iWidgetModel->Accessor() );

        // This will set icon area background to transparent
        iListWidget->SetVisualColor(IMulWidget::EThumbnailBackgroundBrush, TRgb(0xFFFFFFFF, 255));

        // Enable multiselect and cyclic scrolling
        iListWidget->SetFlags( IMulMultiItemWidget::EMulWidgetFlagCyclic |
                IMulMultiItemWidget::EMulWidgetFlagFastScroll | IMulMultiItemWidget::EMulWidgetFlagNaviScroll |
                IMulMultiItemWidget::EMulWidgetMarkingMode |
                IMulMultiItemWidget::EMulWidgetTitleMarqueeOn );

        // Fetch icon and indicator sizes
        iListWidget->GetIconSize( mulwidget::KTemplate3, mulvisualitem::KMulIcon1, iIconHeight, iIconWidth );
        iListWidget->GetIconSize( mulwidget::KTemplate3, mulvisualitem::KMulIndicator1, iIndicatorHeight, iIndicatorWidth );
        iThumbnailManager->SetThumbnailSizeL( TSize(iIconHeight, iIconWidth) );
        }
    }

// -----------------------------------------------------------------------------
// Handle listbox item addition, preserving the current display index
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleLbxItemAdditionPreserveIndexL()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::HandleLbxItemAdditionPreserveIndexL" );
    HandleLbxItemAdditionL();
    }

// -----------------------------------------------------------------------------
// Handle listbox item removal.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleLbxItemRemovalL()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::HandleLbxItemRemovalL" );
    iTopIndex = iBottomIndex = KErrNotFound;
    if ( iTextureManager )
        {
        // Enabling the following will free up the memory, but not clearing the cache
        // will improve performance when we come back to the list again.
        // iTextureManager->ClearCache(CMPXCommonContainerTextureManager::EMPXTextureImage);
        }
    iThumbnailReqArray.Reset();
    }

// -----------------------------------------------------------------------------
// Show/hide find box.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetFindBoxVisibilityL( TBool aIsVisible )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetFindBoxVisibilityL" );
    if ( aIsVisible )
        {
        if ( !FindBoxVisibility() )
            {
            iSearchFieldWrapper = CMPXCommonContainerSearchFieldWrapper::NewL(this);
            iFilteredList = CMPXCommonContainerFilteredList::NewL();
            iFilteredList->ReplaceMediaListL( iListBoxArray->MediaArray() );
            }
        ActivateSearchFieldL();
        }
    else
        {
        if ( FindBoxVisibility() )
            {
            TInt index = iWidgetModel->Highlight();
            if ( index != KErrNotFound )
                {
                index = iFilteredList->FilteredIndexToRealIndex(index);
                }

            DeactivateSearchField();

            delete iFilteredList;
            iFilteredList = NULL;
            delete iSearchFieldWrapper;
            iSearchFieldWrapper = NULL;

            // Restore original list
            TInt count = iListBoxArray->MediaArray().Count();

            if( count > 0)
            	{
            	iWidgetModel->Refresh( count );
            	if ( index != KErrNotFound )
                	{
                	iWidgetModel->SetHighlight( index );
                	}
				}
            iThumbnailReqArray.Reset();
            }
        }
    }

// -----------------------------------------------------------------------------
// Determine find box visibility
// -----------------------------------------------------------------------------
//
TBool CMPXCommonContainerHitchcockImp::FindBoxVisibility()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::FindBoxVisibility" );
    TBool visible = EFalse;
    if ( iSearchFieldWrapper )
        {
        visible = iSearchFieldWrapper->IsSearchFieldActive();
        }
    return visible;
    }

// -----------------------------------------------------------------------------
// Restore the PopupCBA of find box
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::RestoreFindBoxPopupCBA()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::RestoreFindBoxPopupCBA" );
    // Not required for Hitchcock
    }

// -----------------------------------------------------------------------------
// Calculate the top index of the visible items
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHitchcockImp::CalculateTopIndex( TInt /*aBottomIndex*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::CalculateTopIndex" );
    // Not required for Hitchcock
    return 0;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ConstructContainerL( TMPXCommonContainerCollectionType aCollectionType )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::ConstructContainerL" );

	/*
    CCoeEnv* coeEnv( iEikonEnv );
    TParse parse;
    parse.Set( KMPXCollectionRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );
	*/

    iCollectionType = aCollectionType;
    CreateWindowL();
    CreateListBoxL();
    FeatureManager::InitializeLibL();
    ConstructFindBoxL( ETrue );

    // Common methods used for 2-line list support
    iCommonUiHelper = CMPXCommonUiHelper::NewL();
    iListSelections = new (ELeave) CArrayFixFlat<TInt>( KMPXListSelectionCount );
    iTextureManager = new (ELeave) CMPXCommonContainerTextureManager( iEnv->TextureManager() );

    iThumbnailManager = CThumbnailManager::NewL( *this );
    iThumbnailManager->SetFlagsL( CThumbnailManager::EDefaultFlags );
    iThumbnailManager->SetQualityPreferenceL( CThumbnailManager::EOptimizeForQuality );
    //iThumbnailManager->SetThumbnailSizeL( TSize(iIconHeight, iIconWidth) );
    }

// ---------------------------------------------------------------------------
// Enable/disable find box
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::EnableFindBox( TBool aEnable )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::EnableFindBox" );
    iIsFindEnabled = aEnable;
    }

// ---------------------------------------------------------------------------
// Enable/disable marking
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::EnableMarking( TBool aEnable )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::EnableMarking" );
    iIsMarkingEnabled = aEnable;
    if( iListWidget )
		{
		if( iIsMarkingEnabled )
			{
			iListWidget->SetFlags( IMulMultiItemWidget::EMulWidgetMarkingMode );
			}
		else
			{
			iListWidget->ClearFlags( IMulMultiItemWidget::EMulWidgetMarkingMode );
			}
		}
    }

// ---------------------------------------------------------------------------
// Custom handling of commands for markable lists.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleMarkableListProcessCommandL(
    TInt aCommand )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::HandleMarkableListProcessCommandL" );

	if ( iWidgetModel )
		{
		switch ( aCommand )
			{
			case EAknCmdMark:
				{
				std::vector<int> itemIndex;
				int highlight = iWidgetModel->Highlight();
				itemIndex.push_back( highlight);
				iWidgetModel->SetMarkedIndices(MulWidgetDef::EMulMark, itemIndex);
				}
				break;
			case EAknCmdUnmark:
			    {
			    std::vector<int> itemIndex;
				int highlight = iWidgetModel->Highlight();
				itemIndex.push_back(highlight);
				//mark the selected item
				iWidgetModel->SetMarkedIndices(MulWidgetDef::EMulUnmark, itemIndex);
			    }
			    break;
			case EAknMarkAll:
				iWidgetModel->SetMarkedIndices(MulWidgetDef::EMulMarkAll);
				break;
			case EAknUnmarkAll:
				iWidgetModel->SetMarkedIndices(MulWidgetDef::EMulUnmarkAll);
				break;
			}
		}
    }

// ---------------------------------------------------------------------------
// Custom handling of menu pane for markable lists
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleMarkableListDynInitMenuPane(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::HandleMarkableListDynInitMenuPane" );

    if ( aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST )
        {
        TInt currentItem = CurrentLbxItemIndex();
        if ( currentItem == KErrNotFound )
            {
            aMenuPane->SetItemDimmed( EAknCmdEditListMenu, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdEditListMenu, EFalse );
            }
        }
    else if ( aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST_IMPLEMENTATION )
        {
        TInt currentItem = CurrentLbxItemIndex();
        if ( currentItem != KErrNotFound )
            {
            TBool currentItemMarked = EFalse;
            std::vector<int> itemsSelected = iWidgetModel->MarkedIndices();
            int markedCount = itemsSelected.size();
            for( int i = 0; i < markedCount; i++ )
                {
                if ( currentItem == itemsSelected[i] )
                    {
                    currentItemMarked = ETrue;
                    break;
                    }
                }
            TInt totalCount = iWidgetModel->Count();
            aMenuPane->SetItemDimmed( EAknCmdMark, currentItemMarked );
            aMenuPane->SetItemDimmed( EAknCmdUnmark, !currentItemMarked );
            aMenuPane->SetItemDimmed( EAknMarkAll, markedCount == totalCount );
            aMenuPane->SetItemDimmed( EAknUnmarkAll, markedCount == 0 );
            }
        }
    }

// ---------------------------------------------------------------------------
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerHitchcockImp::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG5( "CMPXCommonContainerHitchcockImp::HandleKeyEventL(iCode=%d, iScanCode=%d, iModifiers=0x%x, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iModifiers, aType );

    if ( iSearchFieldWrapper && aType == EEventKeyUp )
        {
#ifdef __ENABLE_MSK
        // Update the middle softkey for marking commands
        iView->ProcessCommandL( EMPXCmdCommonUpdateMiddleSoftKey );
#endif // __ENABLE_MSK
        }
    if ( iSearchFieldWrapper && aType == EEventKey )
        {
        if ( iSearchFieldWrapper->IsSearchFieldActive() )
            {
            if ( iSearchFieldWrapper->OfferKeyEventL( aKeyEvent, aType ) == EKeyWasConsumed )
                {
                return EKeyWasConsumed;
                }
            }
        }
    // TODO: Temporary test code, what key codes should start find?
    else if ( ( aKeyEvent.iScanCode >= 48  && aKeyEvent.iScanCode <= 57 ) ||
              ( aKeyEvent.iScanCode >= 65  && aKeyEvent.iScanCode <= 90 ) ||
              ( aKeyEvent.iScanCode >= 121 && aKeyEvent.iScanCode <= 127 ) ||
              ( aKeyEvent.iScanCode == 5 ) || ( aKeyEvent.iScanCode == 125 ) ||
              ( aKeyEvent.iScanCode == 127 ) || ( aKeyEvent.iScanCode == 130 ) ||
              ( aKeyEvent.iScanCode == 133 ) || ( aKeyEvent.iScanCode == 135 ) )
        {
        if ( iContext != EContextGroupCollection )
            {
            iView->ProcessCommandL( EMPXCmdCommonFind );
            return EKeyWasConsumed;
            }
        }

    TKeyResponse response( EKeyWasNotConsumed );
    if ( aKeyEvent.iCode == EKeyOK )
        {
        // Handle enter key pressed
        iView->ProcessCommandL( EMPXCmdCommonEnterKey );
        }

    if ( response == EKeyWasNotConsumed &&
         aType == EEventKey &&
         aKeyEvent.iCode == EKeyBackspace && iView )
        {
        iView->ProcessCommandL( EMPXCmdCommonDelete );
        response = EKeyWasConsumed;
        }

    return response;
    }

// ---------------------------------------------------------------------------
// Activate container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ActivateContainerL()
    {
    TRect clientRect = ((CAknView*)iView)->ClientRect();
    iViewWidget->setRect( clientRect );
    iViewWidget->show(true);
    iContainerActive = ETrue;
    if( iListWidget )
    	{
		iListWidget->ShowWidget(true);
		}
    }

// ---------------------------------------------------------------------------
// Deactivate container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::DeactivateContainerL()
    {
    iViewWidget->show(false);
    if ( iListWidget )
        {
        iListWidget->SetModel(NULL);
        iWidgetModel = NULL;
        iListWidget->ShowWidget(false);
        }
    iContainerActive = EFalse;
    }

// ---------------------------------------------------------------------------
// Return container state
// ---------------------------------------------------------------------------
//
TBool CMPXCommonContainerHitchcockImp::IsContainerActive()
    {
    return iContainerActive;
    }

// ---------------------------------------------------------------------------
// From MMPXCommonListBoxArrayObserver
// Handle listbox array events.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleListBoxArrayEventL(
    MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::HandleListBoxArrayEventL" );
    if ( aEvent == MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventIconArrayChange )
        {
        delete iIconArray;
        iIconArray = NULL;
        CreateIconArrayL();
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerHitchcockImp::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG5( "CMPXCommonContainerHitchcockImp::OfferKeyEventL(iCode=%d, iScanCode=%d, iModifiers=0x%x, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iModifiers, aType );
    MPX_DEBUG3("CMPXCommonContainerHitchcockImp::OfferKeyEventL, iType=%d, iActive=%d", iCollectionType, iContainerActive);
    if ( iWidgetModel )
        {
        TAlfEvent event( iEnv->PrimaryDisplay(), aKeyEvent, aType );
        if ( iListWidget->control()->processEvent( event ) == EEventHandled )
            {
            return EKeyWasConsumed;
            }
        else
            {
            // Let the view container handle the key event.
            return iViewContainer->HandleKeyEventL( aKeyEvent, aType );
            }
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::GetHelpContext" );
    // Let the view container handle the help context retrieval
    if ( iViewContainer )
        {
        iViewContainer->HandleHelpContext( aContext );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleResourceChange( TInt aType )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::HandleResourceChange" );
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange )
        {
        delete iIconArray;
        iIconArray = NULL;
        TRAP_IGNORE(CreateIconArrayL());
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect clientRect = ((CAknView*)iView)->ClientRect();
        if ( FindBoxVisibility() )
            {
            TRect clientRect = ((CAknView*)iView)->ClientRect();
            iViewWidget->setRect(
                    iSearchFieldWrapper->GetListRect( ETrue, clientRect ) );
            iSearchFieldWrapper->SetRect(
                    iSearchFieldWrapper->GetSearchFieldRect( ETrue, clientRect ) );
            }
        else
            {
            iViewWidget->setRect( clientRect );
            }
        }
    }

// ---------------------------------------------------------------------------
// Draw this application's view to the screen
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::Draw(const TRect& /*aRect*/) const
    {
    // Do nothing so that list model will be drawn by Hitchcock framework
    }

// ---------------------------------------------------------------------------
// From MCoeControlObserver
// Handle control event
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::HandleControlEventL(
    CCoeControl* /*aControl*/,
    TCoeEvent /*aEventType*/ )
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Indicates whether event is accepted
// ---------------------------------------------------------------------------
//
bool CMPXCommonContainerHitchcockImp::accept(
    CAlfWidgetControl& /*aControl*/,
    const TAlfEvent& /*aEvent*/ ) const
    {
    return true;
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Sets the active states of the event handler
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::setActiveStates( unsigned int /*aStates*/ )
    {
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Performs the appropriate action for the ALF event
// ---------------------------------------------------------------------------
//
AlfEventStatus CMPXCommonContainerHitchcockImp::offerEvent(
    CAlfWidgetControl& /*aControl*/,
    const TAlfEvent& aEvent )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::offerEvent" );

    AlfEventStatus status = EEventNotHandled;

    if( aEvent.IsCustomEvent() )
        {
        uint eventID = aEvent.CustomParameter();

        switch( eventID )
            {
            case ETypeMark:
                {
                switch( aEvent.CustomEventData() )
                    {
                    case EMarkStart:
                    case EMarkStop :
                        {
                        status = EEventConsumed;
                        }
                        break;
                    default:
                        break;
                    }
                break;
                }
            case ETypeHighlight:
                {
                if( iWidgetModel && ( iWidgetModel->Count() > 0 ) )
                    {
                    status = EEventConsumed;
                    }
                break;
                }
            case ETypeSelect:
                {
                //Handling for this needs to be done, once it is implemented in widget
                if( iWidgetModel && ( iWidgetModel->Count() > 0 ) )
                    {
                    // Handle enter key pressed
                    iView->ProcessCommandL( EMPXCmdCommonEnterKey );
                    status = EEventConsumed;
                    }
                break;
                }
            default:
                status = EEventNotHandled;
                break;
            }
        }

    return status;
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Make event handler interface using the specified type
// ---------------------------------------------------------------------------
//
IAlfInterfaceBase* CMPXCommonContainerHitchcockImp::makeInterface(
    const IfId& /*aType*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Sets the event handler data
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::setEventHandlerData(
    const AlfWidgetEventHandlerInitData& /*aData*/ )
    {
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Returns pointer to the event handler data
// ---------------------------------------------------------------------------
//
AlfWidgetEventHandlerInitData*
    CMPXCommonContainerHitchcockImp::eventHandlerData()
   {
   return NULL;
   }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Sets the event handler data
// ---------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerType CMPXCommonContainerHitchcockImp::eventHandlerType()
    {
    return IAlfWidgetEventHandler::EPresentationEventHandler;
    }

// ---------------------------------------------------------------------------
// From IAlfWidgetEventHandler
// Sets the event handler data
// ---------------------------------------------------------------------------
//
IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase CMPXCommonContainerHitchcockImp::eventExecutionPhase()
    {
    return EBubblingPhaseEventHandler;
    }

// -----------------------------------------------------------------------------
// Provides the data to the model
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ProvideData(
    int aStartIndex,
    int aCount,
    MulDataPath /*aPath*/ )
    {
    TInt numOfItems = 0;
    if ( iFilteredList )
        {
        numOfItems = iFilteredList->FilteredCount();
        }
    else
        {
        numOfItems = iListBoxArray->MediaArray().Count();
        }

    for ( int i = 0; i <  aCount ; ++i )
        {
        int index = aStartIndex+i;
        if ( index >= 0 && index < numOfItems )
            {
            MulVisualItem* tempItem = CreateDisplayedItemL(index );
            if ( tempItem )
                {
                auto_ptr<MulVisualItem> dataItem ( new (EMM) MulVisualItem(*tempItem));
                iWidgetModel->SetData(index, dataItem);
                delete tempItem;
                }
            }
        }
    iTopIndex = aStartIndex;
    iBottomIndex = aStartIndex + aCount - 1;
    }

// -----------------------------------------------------------------------------
// Preview thumbnail generation or loading is complete.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ThumbnailPreviewReady( MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    // Not used
    }

// -----------------------------------------------------------------------------
// Final thumbnail bitmap generation or loading is complete.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ThumbnailReady( TInt aError, MThumbnailData& aThumbnail, TThumbnailRequestId aId )
    {
    if ( aError == KErrNone && iContainerActive )
        {
        // Find the index
        TInt index = KErrNotFound;
        TInt count = iThumbnailReqArray.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            TThumnailReq req = iThumbnailReqArray[i];
            if ( req.iReqId == aId )
                {
                index = req.iIndex;
                break;
                }
            }

        if ( index != KErrNotFound )
            {
            const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
            TInt realIndex = index;
            if ( iFilteredList )
                {
                realIndex = iFilteredList->FilteredIndexToRealIndex(index);
                }
            CMPXMedia* currentMedia( mediaArray.AtL( realIndex ) );
            if ( currentMedia->IsSupported( KMPXMediaGeneralId ) )
                {
                TMPXItemId currentId( currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
                CFbsBitmap* bitmap = aThumbnail.DetachBitmap();

                // Create the texture. The texture id will be
                // set synchronously when the texture manager is notified
                CAlfTexture& texture = iTextureManager->CreateTextureL(
                                        realIndex, currentId, bitmap, NULL );
                TInt textureId = texture.Id();
                //iWidgetModel->Update(index, 1);
                MulVisualItem* visualItem = new (EMM) MulVisualItem();
                visualItem->SetAttribute( mulvisualitem::KMulIcon1, textureId, 0 );
                iWidgetModel->SetData(index, auto_ptr<MulVisualItem>(visualItem) );
                }
            }
        }
    }
// ---------------------------------------------------------------------------
// Create list box
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::NewFilteringWordL( const TDesC& aWord )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::NewFilteringWordL" );
    if ( iFilteredList->HasMediaList() )
        {
        iFilteredList->SetFilteringWordL( aWord );
        TInt filteredCount = iFilteredList->FilteredCount();
        if ( filteredCount > 0 )
            {
            TInt index = iWidgetModel->Highlight();
            if ( index >=  filteredCount )
                {
                iWidgetModel->SetHighlight( 0 );
                }
            iWidgetModel->Refresh( filteredCount );
            }
        else
            {
            TInt widgetCount = iWidgetModel->Count();
            if ( widgetCount > 0 )
                {
                iWidgetModel->Remove( 0, widgetCount );
                }
            }
        iThumbnailReqArray.Reset();
        }
    }

// ---------------------------------------------------------------------------
// Create list box
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::CreateListBoxL()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::CreateListBox" );
    const char* KLoadNameList("mullistwidget");
    const char* KLoadNameModel("mulmodel");
    const char* KListViewNameMusic("mpxmullistviewmusic");
    const char* KListViewNamePodcast("mpxmullistviewpodcast");

    if ( iAlfEnvCreated )
        {
        return;
        }
    iAlfEnvCreated = ETrue;

    TRect clientRect = ((CAknView*)iView)->ClientRect();

    // Creating the Alf Environment
    // Should be the first thing to be done before widgets can be created
    iEnv = CAlfEnv::Static();

    // This api will force glib to use the normal malloc for
    // memory allocation,preventing ustring memory leak.
    g_setenv("G_SLICE","always-malloc",1);

    // Creating the Alf Display
    // Display is needed to show anything related to Alf
    // Avkon perceives the Alf Display as one CoeControl.
    // It can't see widgets inside the Display
    CAlfDisplay& disp = iEnv->NewDisplayL( clientRect, CAlfEnv::ENewDisplayAsCoeControl );
    disp.SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

    try
        {
        // Get widget factory from CAlfEnv
        // Factory is then used to create the individual widgets & data model
        IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv);

        // Create view widget. CAlfEnv owns widget
        if ( !iViewWidget && iCollectionType == MMPXCommonContainer::EMPXCommonContainerMusic )
            {
            iViewWidget = widgetFactory.createViewWidget(KListViewNameMusic, KListControlGroupIdMusic);
            }
        else if ( !iViewWidget && iCollectionType == MMPXCommonContainer::EMPXCommonContainerPodcast )
            {
            iViewWidget = widgetFactory.createViewWidget(KListViewNamePodcast, KListControlGroupIdPodcast);
            }
        else
            {
            User::Leave(KErrArgument);
            }
        }
    catch (...)
        {
        User::Leave(KErrGeneral);
        }
    SizeChanged();
    }

// -----------------------------------------------------------------------------
// Constructs Find box.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ConstructFindBoxL( TBool /*aIsFindPopup*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::ConstructFindBoxL" );
    // Not used
    }

// -----------------------------------------------------------------------------
// Creates icon array, populates iIconArray to be used by listbox
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::CreateIconArrayL()
    {
    if ( iListBoxArray )
        {
        iIconArray = iListBoxArray->CreateIconArrayL();
        }
    }

// -----------------------------------------------------------------------------
// Creates the display item from the media
// -----------------------------------------------------------------------------
//
MulVisualItem* CMPXCommonContainerHitchcockImp::CreateDisplayedItemL(
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::CreateDisplayedItemL" );

    MulVisualItem* visualItem = new (EMM) MulVisualItem();

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    CMPXMedia* currentMedia = NULL;
    if ( iFilteredList )
        {
        TInt realIndex = iFilteredList->FilteredIndexToRealIndex(aIndex);
        currentMedia = mediaArray.AtL( realIndex );
        }
    else
        {
        currentMedia = mediaArray.AtL( aIndex );
        }

    SetTitleL( visualItem, currentMedia );
    // Add detailed text to the list item, if available
    AddDetailToDisplayedItemL( visualItem, currentMedia, aIndex );

    return visualItem;
    }

// -----------------------------------------------------------------------------
// Adds detail to the list item at the specified index
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::AddDetailToDisplayedItemL(
    MulVisualItem* aVisualItem,
    CMPXMedia* aMedia,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::AddDetailToDisplayedItemL" );

    switch ( iContext )
        {
        case EContextGroupCollection:
            SetDetailIconL(aVisualItem, aIndex);
            break;
        case EContextGroupArtist:
            SetDetailCountL(aVisualItem, aMedia);
            break;
        case EContextGroupAlbum:
        	SetDetailArtistL(aVisualItem, aMedia);
        	SetDetailThumbnailL(aVisualItem, aMedia, aIndex);
            break;
        case EContextGroupPlaylist:
            SetDetailDurationL(aVisualItem, aMedia);
            SetDetailIndicatorL(aVisualItem, aIndex);
            break;
        case EContextGroupSong:
        case EContextItemAlbum:
        case EContextItemPlaylist:
        case EContextItemGenre:
        case EContextItemComposer:
            SetDetailArtistL(aVisualItem, aMedia);
            SetDetailIndicatorL(aVisualItem, aIndex);
            SetDetailThumbnailL(aVisualItem, aMedia, aIndex);
            break;
        case EContextGroupPodcast:
            SetDetailCountL(aVisualItem, aMedia);
            break;
        case EContextGroupGenre:
            SetDetailCountL(aVisualItem, aMedia);
            break;
        case EContextGroupComposer:
            SetDetailCountL(aVisualItem, aMedia);
            break;
        case EContextItemArtist:
            SetDetailCountL(aVisualItem, aMedia);
            SetDetailThumbnailL(aVisualItem, aMedia, aIndex);
            break;
        case EContextItemSong:
            SetDetailAlbumL(aVisualItem, aMedia);
            SetDetailIndicatorL(aVisualItem, aIndex);
            SetDetailThumbnailL(aVisualItem, aMedia, aIndex);
            break;
        case EContextItemPodcast:
            SetDetailDurationL(aVisualItem, aMedia);
            SetDetailIndicatorL(aVisualItem, aIndex);
            SetDetailThumbnailL(aVisualItem, aMedia, aIndex);
            break;
		}
	}

// -----------------------------------------------------------------------------
// Set title to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetTitleL(
    MulVisualItem* aVisualItem,
    CMPXMedia* aMedia )
    {
	if ( aMedia->IsSupported( KMPXMediaGeneralTitle ) )
		{
		// AK - Needs to be localized
		TFileName titleText( KUnknown );
		const TDesC& title = aMedia->ValueText( KMPXMediaGeneralTitle );
		if ( title.Compare( KNullDesC ) != 0 )
			{
			titleText.Copy(title.Left(titleText.MaxLength()));
			}
		aVisualItem->SetAttribute( mulvisualitem::KMulTitle, titleText, 0);
		}
    }

// -----------------------------------------------------------------------------
// Set Detail - Count to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetDetailCountL(
    MulVisualItem* aVisualItem,
    CMPXMedia* aMedia )
    {
	if ( aMedia->IsSupported( KMPXMediaGeneralCount ) )
		{
		// AK - Needs to be localized
		TFileName detailText;
		TInt count( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount ) );
        TBuf<10> temp;
        temp.AppendNum( count );
        AknTextUtils::LanguageSpecificNumberConversion( temp );
        detailText.Append( temp );

        if ( iContext == EContextGroupPodcast )
			{
			detailText.Append( (count > 1 ) ? KEpisodes() : KEpisode() );
			}
        else if ( iContext == EContextGroupArtist )
			{
			detailText.Append( (count > 1 ) ? KAlbums() : KAlbum() );
			}
		else
			{
			detailText.Append( (count > 1 ) ? KSongs() : KSong() );
			}
		aVisualItem->SetAttribute( mulvisualitem::KMulDetail, detailText, 0);
		}
    }

// -----------------------------------------------------------------------------
// Set Detail - Artist to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetDetailArtistL(
    MulVisualItem* aVisualItem,
    CMPXMedia* aMedia )
    {
	if ( aMedia->IsSupported( KMPXMediaMusicArtist ) )
		{
		// AK - Needs to be localized
		TFileName detailText;
		const TDesC& artist = aMedia->ValueText( KMPXMediaMusicArtist );
		if ( artist.Compare( KNullDesC ) != 0 )
			{
			detailText.Copy(artist.Left(detailText.MaxLength()));
	        aVisualItem->SetAttribute( mulvisualitem::KMulDetail, detailText, 0);
			}
		}
    }

// -----------------------------------------------------------------------------
// Set Detail - Album to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetDetailAlbumL(
    MulVisualItem* aVisualItem,
    CMPXMedia* aMedia )
    {
	if ( aMedia->IsSupported( KMPXMediaMusicAlbum ) )
		{
		// AK - Needs to be localized
		TFileName detailText;
		const TDesC& album = aMedia->ValueText( KMPXMediaMusicAlbum );
		if ( album.Compare( KNullDesC ) != 0 )
			{
			detailText.Copy(album.Left(detailText.MaxLength()));
	        aVisualItem->SetAttribute( mulvisualitem::KMulDetail, detailText, 0);
			}
		}
    }

// -----------------------------------------------------------------------------
// Set Detail - Count + Duration to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetDetailDurationL(
    MulVisualItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    if ( iContext == EContextGroupPlaylist &&
         aMedia->IsSupported( KMPXMediaGeneralDuration ) &&
	     aMedia->IsSupported( KMPXMediaGeneralCount ) )
		{
		// AK - Needs to be localized
		TBuf<85> detailText;
		TInt count( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount ) );
		TBuf<10> temp;
		temp.AppendNum( count );
        AknTextUtils::LanguageSpecificNumberConversion( temp );
        detailText.Append( temp );
		detailText.Append( (count > 1 ) ? KSongsDash() : KSongDash() );

		TInt duration = aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration );
        UpdateTimeIndicatorsL(detailText, duration);
		aVisualItem->SetAttribute( mulvisualitem::KMulDetail, detailText, 0);
		}
    else if ( iContext == EContextItemPodcast &&
         aMedia->IsSupported( KMPXMediaGeneralDuration ) &&
         aMedia->IsSupported( KMPXMediaGeneralSize ) )
        {
        TBuf<85> detailText;
        TInt duration = aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration );
        UpdateTimeIndicatorsL(detailText, duration);
        detailText.Append( KMPXDash() );

        TInt size( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralSize ) );
        HBufC* sizeBuf = iCommonUiHelper->UnitConversionL( size );
        TPtr sizePtr( sizeBuf->Des() );
        detailText.Append( sizePtr );
        delete sizeBuf;

        aVisualItem->SetAttribute( mulvisualitem::KMulDetail, detailText, 0);
        }
    }

// -----------------------------------------------------------------------------
// Format the duration to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::UpdateTimeIndicatorsL(
    TDes& aBuf,
    TInt aDuration )
    {
    TInt durationInSeconds = aDuration;
    if ( durationInSeconds > 0 )
        {
        durationInSeconds /= 1000;   // MilliSeconds to second conversion
        }
    else if ( durationInSeconds < 0 )
        {
        durationInSeconds = 0;
        }
    if ( durationInSeconds == 0 )
        {
        TLocale locale;
        TBuf<KMPXTimeIndicatorLength> pos;
        TChar separator = locale.TimeSeparator( KMPXMinSecSeparatorIndex );
        aBuf.Append(KMPXZeroDurationMark);
        aBuf.Append( separator );
        aBuf.Append(KMPXZeroDurationMark);
        }
    else
        {
        CMPXCommonUiHelper::TMPXDuratDisplayMode durationMode = CMPXCommonUiHelper::EMPXDuratAuto;
        if ( durationInSeconds > KOneHourInSeconds )
            {
            durationMode = CMPXCommonUiHelper::EMPXDuratHMS;
            }
        // Convert ellapsed time to texts
        HBufC* position = iCommonUiHelper->DisplayableDurationL( durationInSeconds, durationMode );
        aBuf.Append(position->Des());
        delete position;
        }
    }

// -----------------------------------------------------------------------------
// Set Detail - Indicator to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetDetailIndicatorL(
    MulVisualItem* aVisualItem,
    TInt aIndex )
    {
    // Reset the indicators
    aVisualItem->SetAttribute( mulvisualitem::KMulIndicator1, -1, 0 );
    aVisualItem->SetAttribute( mulvisualitem::KMulIndicator2, -1, 0 );

    // Get icon indices
    TInt realIndex = aIndex;
    if ( iFilteredList )
        {
		realIndex = iFilteredList->FilteredIndexToRealIndex(aIndex);
        }
    RArray<TInt> iconIndices = iViewContainer->IndicatorsL( realIndex );
    CleanupClosePushL( iconIndices );

    TInt iconCount = iconIndices.Count();
    if ( iconCount > 2 )
        {
        // Restrict max number of icons to 2
        iconCount = 2;
        }
    if ( iconCount > 0 )
    	{
		for ( TInt iconIndex = 0; iconIndex < iconCount; iconIndex++ )
			{
			TInt index = iconIndices[iconIndex];
			TInt textureIndex = index;
			if ( iContext == EContextGroupPodcast || iContext == EContextItemPodcast )
			    {
			    // Shift the range for podcast icons to avoid textureId
			    // clash with collection icons
			    textureIndex += 100;
			    }

			// Check if texture already exists for the icon
			TInt textureId( iTextureManager->IconTextureId( textureIndex ) );
			if ( textureId == KErrNotFound )
				{
				CGulIcon* icon = (*iIconArray)[index];
				CFbsBitmap* bitmap = icon->Bitmap();
				CFbsBitmap* mask = icon->Mask();

				if( bitmap && mask )
					{
					// Need to set the size of the bitmap used in CGulIcon to enable display
					AknIconUtils::SetSize( bitmap, TSize(iIndicatorHeight, iIndicatorWidth) );
					AknIconUtils::SetSize( mask, TSize(iIndicatorHeight, iIndicatorWidth) );

					// Bitmap ownership is transferred to the texture manager.
					CFbsBitmap* bitmapTemp = new (ELeave) CFbsBitmap();
					CFbsBitmap* maskTemp = new (ELeave) CFbsBitmap();
					bitmapTemp->Duplicate( bitmap->Handle() );
					maskTemp->Duplicate( mask->Handle() );

					// Create the texture. The texture id will be
					// set synchronously when the texture manager is notified
					CAlfTexture& texture = iTextureManager->CreateIconTextureL(
					        textureIndex, bitmapTemp, maskTemp );
					textureId = texture.Id();
					}
				}

			// Set the texture id
            switch ( iconIndex )
                {
                case 0:
                    aVisualItem->SetAttribute( mulvisualitem::KMulIndicator1, textureId, 0 );
                    break;
                case 1:
                    aVisualItem->SetAttribute( mulvisualitem::KMulIndicator2, textureId, 0 );
                    break;
                }
			}
		}
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// Set Detail - Icon to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetDetailIconL(
    MulVisualItem* aVisualItem,
    TInt aIndex )
    {
    // Get icon indices
    TInt iconIndex = aIndex;
     switch ( aIndex )
        {
        case 0:
            iconIndex = EMPXClvIconArtist;
            break;
        case 1:
            iconIndex = EMPXClvIconAlbum;
            break;
        case 2:
            iconIndex = EMPXClvIconPlaylist;
            break;
        case 3:
            iconIndex = EMPXClvIconSongs;
            break;
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        case 4:
            iconIndex = EMPXClvIconPodcasts;
            break;
#endif
        case 5:
            iconIndex = EMPXClvIconGenre;
            break;
        case 6:
            iconIndex = EMPXClvIconComposer;
            break;
        }

    // Check if texture already exists for the icon
    TInt textureId( iTextureManager->IconTextureId( iconIndex ) );
    if ( textureId == KErrNotFound )
        {
        CGulIcon* icon = (*iIconArray)[iconIndex];
        CFbsBitmap* bitmap = icon->Bitmap();
        CFbsBitmap* mask = icon->Mask();

        if( bitmap && mask )
            {
			// Need to set the size of the bitmap used in CGulIcon to enable display
			AknIconUtils::SetSize( bitmap, TSize(iIconHeight, iIconWidth) );
    		AknIconUtils::SetSize( mask, TSize(iIconHeight, iIconWidth) );

            // Bitmap ownership is transferred to the texture manager.
            CFbsBitmap* bitmapTemp = new (ELeave) CFbsBitmap();
            CFbsBitmap* maskTemp = new (ELeave) CFbsBitmap();
            bitmapTemp->Duplicate( bitmap->Handle() );
            maskTemp->Duplicate( mask->Handle() );

            // Create the texture. The texture id will be
            // set synchronously when the texture manager is notified
            CAlfTexture& texture = iTextureManager->CreateIconTextureL(
                    iconIndex, bitmapTemp, maskTemp );
            textureId = texture.Id();
            }
        }

    // Set the texture id
    aVisualItem->SetAttribute( mulvisualitem::KMulIcon1, textureId, 0 );
    }

// -----------------------------------------------------------------------------
// Set Detail - Thumbnail to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetDetailThumbnailL(
    MulVisualItem* aVisualItem,
    CMPXMedia* aMedia,
    TInt aIndex )
    {
    TMPXItemId currentId( aMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    // Check if texture already exists for the thumbnail
    TInt textureId( iTextureManager->TextureId( currentId ) );
    if ( textureId != KErrNotFound )
        {
        // Set the texture id
        aVisualItem->SetAttribute( mulvisualitem::KMulIcon1, textureId, 0 );
        }
    else
        {
        if ( aMedia->IsSupported( KMPXMediaMusicAlbumArtFileName ) )
            {
            const TDesC& album = aMedia->ValueText( KMPXMediaMusicAlbumArtFileName );
            if ( album.Compare( KNullDesC ) != 0 )
                {
                CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( album, KMPXAlbumMimeType );
                TInt ret = NULL;
                TThumnailReq req;
                req.iIndex = aIndex;
                // Set priority based on the index so that the smaller indexes are loaded first
                req.iReqId = iThumbnailManager->GetThumbnailL( *source, (TAny*)ret, -aIndex );
                iThumbnailReqArray.AppendL(req);
                CleanupStack::PopAndDestroy( source );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Sets the current category.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::SetCollectionContextL()
    {
    MPX_FUNC( "CMPXCommonContainerHitchcockImp::SetCollectionContextL" );

    const CMPXMedia& media = iListBoxArray->ContainerMedia();

    TMPXGeneralType containerType( EMPXNoType );
    if ( media.IsSupported( KMPXMediaGeneralType ) )
        {
        containerType = media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
        }

    TMPXGeneralCategory containerCategory( EMPXNoCategory );
    if( media.IsSupported( KMPXMediaGeneralCategory ) )
        {
        containerCategory = media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
        }
    MPX_DEBUG3( "CMPXCommonContainerHitchcockImp::SetCollectionContextL container type = %d, category = %d",
            containerType, containerCategory );

    TBool isPodcast = EFalse;
    TMPXPodcastCategory podcastCategory( EMPXUnknown );
    if ( media.IsSupported( KMPXMediaPodcastType ) )
        {
        isPodcast = ETrue;
        if( media.IsSupported( KMPXMediaPodcastCategoryGroup ) )
            {
            podcastCategory = media.ValueTObjectL<TMPXPodcastCategory>( KMPXMediaPodcastCategoryGroup );
            }
        }
    MPX_DEBUG3( "CMPXCommonContainerHitchcockImp::SetCollectionContextL container podcast = %d, category = %d",
            isPodcast, podcastCategory );

    if ( !isPodcast )
        {
        if ( containerType == EMPXGroup )
            {
            switch (containerCategory)
                {
                case EMPXCollection:
                    iContext = EContextGroupCollection;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
                    break;
                case EMPXArtist:
                    iContext = EContextGroupArtist;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate2 );
                    break;
                case EMPXAlbum:
                    iContext = EContextGroupAlbum;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate6 );
                    break;
                case EMPXPlaylist:
                    iContext = EContextGroupPlaylist;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate2 );
                    break;
                case EMPXSong:
                    iContext = EContextGroupSong;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
                    break;
                case EMPXGenre:
                    iContext = EContextGroupGenre;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate2 );
                    break;
                case EMPXComposer:
                    iContext = EContextGroupComposer;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate2 );
                    break;
                default:
                    User::Leave(KErrNotSupported);
                    break;
                }
            }
        else if ( containerType == EMPXItem )
            {
            switch (containerCategory)
                {
                case EMPXArtist:
                    iContext = EContextItemArtist;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate6 );
                    break;
                case EMPXAlbum:
                    iContext = EContextItemAlbum;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
                    break;
                case EMPXPlaylist:
                    iContext = EContextItemPlaylist;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
                    break;
                case EMPXSong:
                    iContext = EContextItemSong;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
                    break;
                case EMPXGenre:
                    iContext = EContextItemGenre;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
                    break;
                case EMPXComposer:
                    iContext = EContextItemComposer;
                    iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
                    break;
                default:
                    User::Leave(KErrNotSupported);
                    break;
                }
            }
        }
    else
        {
        if ( (containerType == EMPXGroup) && (podcastCategory == EMPXTitle) )
            {
            iContext = EContextGroupPodcast;
            iWidgetModel->SetTemplate( mulwidget::KTemplate2 );
            }
        else if ( (containerType == EMPXItem) && (podcastCategory == EMPXEpisode) )
            {
            iContext = EContextItemPodcast;
            iWidgetModel->SetTemplate( mulwidget::KTemplate3 );
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::ActivateSearchFieldL()
    {
    if ( ! iSearchFieldWrapper->IsSearchFieldActive() )
        {
        TRect clientRect = ((CAknView*)iView)->ClientRect();
        iSearchFieldWrapper->ActivateSearchFieldL();

        iViewWidget->setRect(
            iSearchFieldWrapper->GetListRect( ETrue, clientRect ) );
        iSearchFieldWrapper->SetRect(
            iSearchFieldWrapper->GetSearchFieldRect( ETrue, clientRect ) );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHitchcockImp::DeactivateSearchField()
    {
    TRect clientRect = ((CAknView*)iView)->ClientRect();
    iViewWidget->setRect(
        iSearchFieldWrapper->GetListRect( EFalse, clientRect ) );
    }

//  End of File
