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
* Description:  Application UI class required by AVKON application architecture.
*
*/

// INCLUDE FILES
#include <barsread.h>
#include <StringLoader.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <featmgr.h>
#include <aknsfld.h>
#include <AknUtils.h>
#include <aknlists.h>
#include <aknconsts.h>
#include <avkon.mbg>
#include <eikclbd.h>
#include <eikcmobs.h>
#include <bautils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <s32file.h>

#include <ganes/HgDoubleGraphicList.h>
#include <ganes/HgSingleGraphicList.h>
#include <ganes/HgItem.h>
#include <ganes/HgScrollerWithTitle.h>

#include <data_caging_path_literals.hrh>
#include <mpxlog.h>
#include <mpxuser.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxcommonuihelper.h>
#include <mpxpodcastdefs.h>

#include <mpxcommoncontainer.rsg>
#include <mpxcollectionview.rsg>

#include "mpxcommonlistboxarraybase.h"
#include "mpxcommoncontainermediahelper.h"
#include "mpxcommoncontainertexturemanager2.h"
#include "mpxcommoncontainerhgimp.h"
#include "mpxcommoncontainer.hrh"

#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>
#include <aknview.h>


// CONSTANTS
_LIT( KMPXCommonContainerRscPath, "mpxcommoncontainer.rsc" );

const TInt KMPXListSelectionCount = 5;
const TInt KMPXMinSecSeparatorIndex = 2;
const TInt KOneHourInSeconds = 3600;
const TInt KMPXTimeIndicatorLength = 16;
const TInt KMPXListBufferSize = 400;
const TInt KMPXMaxFileLength = 256;
const TReal KIconFactor = 0.7;
const int KMaxThumbnailReq = 100;

_LIT( KMPXDash, " - " );
#ifdef HG_MP_LOC_AVAILABLE
_LIT( KMPXSpace, " ");
#endif //HG_MP_LOC_AVAILABLE
_LIT( KMPXZeroDurationMark, "--");

//#ifndef HG_MP_LOC_AVAILABLE
_LIT( KUnknown, "Unknown" );
_LIT( KSong, " song" );
_LIT( KSongs, " songs" );
_LIT( KEpisode, " episode" );
_LIT( KEpisodes, " episodes" );
_LIT( KAlbum, " album" );
_LIT( KAlbums, " albums" );
_LIT( KSongDash, " song - " );
_LIT( KSongsDash, " songs - " );
//#endif //HG_MP_LOC_AVAILABLE

_LIT( KMPXDriveC, "c:" );
_LIT( KMPXSelectedAlbumItemFileName, "mpxalbumitem.dat");

#define DSA_PARAM NULL
//#define DSA_PARAM &iEikonEnv->WsSession()



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CMPXCommonContainerHgImp::CMPXCommonContainerHgImp() :
    iContext( EContextUnknown ),
    iTopIndex( 0 ),
    iBottomIndex( KErrNotFound ),
    iCurrentDefaultIcon( EMPXDefaultIconNotSet ),
    iRequestCount( 0 ),
    iAlbumArtRequest(NULL),
    iRequestQueue(_FOFF( TAlbumArtRequest, iLink )),
    iQueueCounter(0)
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCommonContainerHgImp::~CMPXCommonContainerHgImp()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::~CMPXCommonContainerHgImp" );

    delete iEpisodeTitle;
    delete iSongTitle;
    delete iAlbumTitle;

    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }
    delete iListSelections;
    delete iListBoxArray;
    delete iIconArray;
    if ( iScrollerWithTitle )
        {
//	    delete iListWidget;
        delete iScrollerWithTitle;
        if ( iPrevListWidget )
            {
	        delete iPrevListWidget;
            }
        }
    else
        {
	    delete iListWidget;
	    }
 ///   delete iTextureManager;
    delete iCommonUiHelper;
    CancelTNRequest();
    iThumbnailReqHashMap.Close();
    iThumbnailReqMap.Close();
    delete iThumbnailManager;
    delete iBgContext;
    delete iSelectedMediaInAlbumView;
    CleanAlbumArtReqQueue();
    }


// ---------------------------------------------------------------------------
// Return Coe Control
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXCommonContainerHgImp::CoeControl()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CoeControl" );
    return this;
    }


// ---------------------------------------------------------------------------
// Set View Container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetViewContainer(
    MMPXViewContainer* aViewContainer )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetViewContainer" );
    iViewContainer = aViewContainer;
    }


// ---------------------------------------------------------------------------
// Set command observer for the container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetCommandObserver(
    MEikCommandObserver* aCommandObserver )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetCommandObserver" );
    iView = aCommandObserver;
    }


// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetListBoxObserver(
    MEikListBoxObserver* aListBoxObserver )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetListBoxObserver" );
    iListBoxObserver = aListBoxObserver;
    }


// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::UpdateLayout()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::UpdateLayout" );
    }


// -----------------------------------------------------------------------------
// Returns a pointer to the listboxarray
// -----------------------------------------------------------------------------
//
CMPXCommonListBoxArrayBase* CMPXCommonContainerHgImp::ListBoxArray() const
    {
    return iListBoxArray;
    }


// -----------------------------------------------------------------------------
// Sets listbox array
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetListBoxArrayL(
    CMPXCommonListBoxArrayBase* aListBoxArray )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetListBoxArrayL" );
    iListBoxArray = aListBoxArray;
    }


// -----------------------------------------------------------------------------
// Return number of listbox items shown in the current listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHgImp::CurrentListItemCount() const
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CurrentListItemCount" );
    TInt count = 0;
    if ( iListWidget )
        {
        count = iListWidget->ItemCount();
        }
    return count;
    }


// -----------------------------------------------------------------------------
// Return total number of listbox items in the listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHgImp::TotalListItemCount() const
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::TotalListItemCount" );
    TInt count = 0;
    if ( iListBoxArray )
        {
        count = iListBoxArray->MediaArray().Count();
        }
    return count;
    }


// -----------------------------------------------------------------------------
// Gets top listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHgImp::TopLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::TopLbxItemIndex" );
    return iTopIndex;
    }


// -----------------------------------------------------------------------------
// Gets bottom listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHgImp::BottomLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::BottomLbxItemIndex" );
    return iBottomIndex;
    }

// -----------------------------------------------------------------------------
// Hides listbox
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HideListView()
    {
    if ( iListWidget )
           {
           iListWidget->MakeVisible(EFalse);
           }
    }

// -----------------------------------------------------------------------------
// Gets current listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHgImp::CurrentLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CurrentLbxItemIndex" );
	TInt index( KErrNotFound );
    if ( iListWidget )
		{
		const TInt count = CurrentListItemCount();
		if ( count > 0 )
			{
			index = iListWidget->SelectedIndex();
			}
		}
    return index;
    }


// -----------------------------------------------------------------------------
// Get current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
const CArrayFix<TInt>* CMPXCommonContainerHgImp::CurrentSelectionIndicesL() const
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CurrentSelectionIndicesL" );
    if ( iListWidget )
		{
		iListSelections->Reset();
		RArray<TInt> selectionArray;
        CleanupClosePushL( selectionArray );
		iListWidget->GetMarkedItemsL(selectionArray);
		int count = selectionArray.Count();
		for( int i = 0; i < count; i++ )
			{
			iListSelections->AppendL( selectionArray[i] );
			}
        CleanupStack::PopAndDestroy( &selectionArray );
		}
    return iListSelections;
    }


// -----------------------------------------------------------------------------
// Set current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetCurrentSelectionIndicesL(
    CArrayFix<TInt>* aIndices ) const
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetCurrentSelectionIndicesL" );
    TInt count = aIndices->Count();
    if ( count > 0 && iListWidget )
        {
        for( TInt i = 0; i < count; i++ )
            {
            iListWidget->Mark( (*aIndices)[i] );
            }
        iListWidget->RefreshScreen( (*aIndices)[0] );
        }
    }


// -----------------------------------------------------------------------------
// Set top listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetLbxTopItemIndex( TInt /*aIndex*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetLbxTopItemIndex" );
    }


// -----------------------------------------------------------------------------
// Set current listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetLbxCurrentItemIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetLbxCurrentItemIndex" );
    if ( iListWidget )
        {
        iListWidget->SetSelectedIndex( aIndex );
        }
    }


// -----------------------------------------------------------------------------
// Set current listbox item index and highlight it.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetLbxCurrentItemIndexAndDraw(
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetLbxCurrentItemIndexAndDraw" );
	SetLbxCurrentItemIndex(aIndex);
    }


// -----------------------------------------------------------------------------
// Clear listbox selection.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ClearLbxSelection()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::ClearLbxSelection" );
    if ( iListWidget )
        {
        iListWidget->UnMarkAll();
        }
    }


// -----------------------------------------------------------------------------
// Sets the text for an empty list box
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetLbxEmptyTextL( const TDesC& aText )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetLbxEmptyTextL" );

    if ( iListWidget && aText != KNullDesC )
        {
        iListWidget->SetEmptyTextL( aText );
        }
    }

// -----------------------------------------------------------------------------
// Draws a list box item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::DrawLbxItemL( TInt aIndex )
    {
    MPX_DEBUG2( "CMPXCommonContainerHgImp::DrawLbxItemL (aIndex=%d)", aIndex );
    if ( iListWidget && iBottomIndex > 0 )
        {
        CHgItem* item = &iListWidget->ItemL(aIndex);
        SetDetailIndicatorL(item, aIndex);
        iListWidget->RefreshScreen(aIndex);
        }
    }


// -----------------------------------------------------------------------------
// Set Find box's focus.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetFindBoxFocus( TBool /*aFocus*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetFindBoxFocus" );
    }


// -----------------------------------------------------------------------------
// Handle listbox item addition.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleLbxItemAdditionL()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleLbxItemAdditionL" );
    SetCollectionContextL();
    iLastValidMediaItemIndex = 0;
    if ( iListWidget )
        {
        const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
        const TInt count( mediaArray.Count() );
        MPX_DEBUG2( "CMPXCommonContainerHgImp::HandleLbxItemAdditionL count=%d", count);
        HandleLbxItemRemovalL();

		iThumbnailReqMap.Reset();
		for ( TInt i = 0; i < count; i++ )
			{
    		iThumbnailReqMap.Append( EFalse );
			}

        if ( iContext == EContextItemAlbum && iContext != iPrevContext )
            {
            // Save previous list so won't cause flickering when back.
            // If album view comes back again, we don't want to delete
            // title_list so skip this block for the same album track view.
            //iListWidget->SetFlags( CHgScroller::EHgScrollerScreenFreeze );
            iListWidget->Reset();
            iListWidget->SetFocus(EFalse);
            // this is needed to make list passive when we switch to list with title
            iListWidget->DisableScrollBuffer();
            iPrevListWidget = iListWidget;
            iListWidget = NULL;
            CreateListBoxWithTitleL( count );
            ProvideDataWithoutThumbnailsL(mediaArray);
            iListWidget->SetFocus(ETrue);
            }
        else
            {
            if ( iScrollerWithTitle && iContext != EContextItemAlbum )
                {
                // Delete title_list when you are not in album_track view
                delete iScrollerWithTitle;
                iScrollerWithTitle = NULL;
                iListWidget = iPrevListWidget;
                // this is needed to make list active when we switch from list with title
                iListWidget->EnableScrollBufferL(*this, KMPXListBufferSize, KMPXListBufferSize/4);
                iListWidget->Reset();
                iListWidget->SetFocus(ETrue);
           	    iListWidget->ResizeL( count );
                ProvideDataWithoutThumbnailsL(mediaArray);
                }
            else
                {
				TInt index = iListWidget->SelectedIndex();
        	    iListWidget->Reset();
            	if ( count )
            	    {
            	    iListWidget->ResizeL( count );
            	    ProvideDataWithoutThumbnailsL(mediaArray);
                    iListWidget->SetSelectedIndex( index );
            	    }
            	else
            	    {
            	    iListWidget->RefreshScreen(0);
            	    }
            	}
    	    }
        SetDefaultIconL();
	    SetScrollbarTypeL();
    	}
    }


// -----------------------------------------------------------------------------
// Handle listbox item addition, preserving the current display index
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleLbxItemAdditionPreserveIndexL()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleLbxItemAdditionPreserveIndexL" );
    HandleLbxItemAdditionL();
    }


// -----------------------------------------------------------------------------
// Handle listbox item removal.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleLbxItemRemovalL()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleLbxItemRemovalL" );
    iTopIndex = 0;
    iBottomIndex = KErrNotFound;
    if ( iContext != iPrevContext )
        {
        CancelTNRequest();
        iThumbnailReqHashMap.Close();
        iThumbnailReqMap.Close();
        CleanAlbumArtReqQueue();
        iRequestCount = 0;

	    }
    }


// -----------------------------------------------------------------------------
// Show/hide find box.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetFindBoxVisibilityL( TBool /*aIsVisible*/ )
    {
    }


// -----------------------------------------------------------------------------
// Determine find box visibility
// -----------------------------------------------------------------------------
//
TBool CMPXCommonContainerHgImp::FindBoxVisibility()
    {
    return EFalse;
    }


// -----------------------------------------------------------------------------
// Restore the PopupCBA of find box
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::RestoreFindBoxPopupCBA()
    {
    }


// -----------------------------------------------------------------------------
// Calculate the top index of the visible items
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerHgImp::CalculateTopIndex( TInt /*aBottomIndex*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CalculateTopIndex" );
    return 0;
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ConstructContainerL( TMPXCommonContainerCollectionType /*aCollectionType*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::ConstructContainerL" );


    CCoeEnv* coeEnv( iEikonEnv );
    TParse parse;
    parse.Set( KMPXCommonContainerRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );

#ifdef HG_MP_LOC_AVAILABLE
    iAlbumTitle = StringLoader::LoadL( R_MPX_COMMONCONTAINER_DETAILS_ONE_ALBUM );
    iEpisodeTitle = StringLoader::LoadL( R_MPX_COMMONCONTAINER_DETAILS_ONE_EPISODE );
    iSongTitle = StringLoader::LoadL( R_MPX_COMMONCONTAINER_DETAILS_ONE_SONG );
#endif //HG_MP_LOC_AVAILABLE


    CreateWindowL();
    TRect mainPane( 0,0,0,0 );
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgScreen, mainPane, ETrue);
    iBgContext->SetRect(CEikonEnv::Static()->EikAppUi()->ApplicationRect());

    // Common methods used for 2-line list support
    iCommonUiHelper = CMPXCommonUiHelper::NewL();
    iListSelections = new (ELeave) CArrayFixFlat<TInt>( KMPXListSelectionCount );
  ////  iTextureManager = new (ELeave) CMPXCommonContainerTextureManager2();

    //Album art request queue
    //iRequestQueue = new TSglQue<TAlbumArtRequest>(_FOFF(TAlbumArtRequest, iLink));
    iThumbnailManager = CThumbnailManager::NewL( *this );
    iThumbnailManager->SetFlagsL( CThumbnailManager::EDefaultFlags );
    iThumbnailManager->SetQualityPreferenceL( CThumbnailManager::EOptimizeForQuality );
    iThumbnailManager->SetThumbnailSizeL( EAudioListThumbnailSize );

    iImageSize = CHgDoubleGraphicList::PreferredImageSize();

    TFileName pathWithoutDrive;
    iEikonEnv->FsSession().PrivatePath( pathWithoutDrive );
    iSelectedAlbumItemFileName.Copy( KMPXDriveC );
    iSelectedAlbumItemFileName.Append( pathWithoutDrive );
    iSelectedAlbumItemFileName.Append( KMPXSelectedAlbumItemFileName );


    CreateListBoxL( 0 );
    }


// ---------------------------------------------------------------------------
// Enable/disable find box
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::EnableFindBox( TBool /*aEnable*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::EnableFindBox" );
    }


// ---------------------------------------------------------------------------
// Enable/disable marking
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::EnableMarking( TBool /*aEnable*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::EnableMarking" );
    }


// ---------------------------------------------------------------------------
// Custom handling of commands for markable lists.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleMarkableListProcessCommandL(
    TInt aCommand )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleMarkableListProcessCommandL" );

	if ( iListWidget )
		{
		switch ( aCommand )
			{
			case EAknCmdMark:
				iListWidget->Mark(iListWidget->SelectedIndex());
				break;
			case EAknCmdUnmark:
				iListWidget->UnMark(iListWidget->SelectedIndex());
			    break;
			case EAknMarkAll:
				iListWidget->MarkAll();
				break;
			case EAknUnmarkAll:
				iListWidget->UnMarkAll();
				break;
			}
        iListWidget->RefreshScreen(iListWidget->SelectedIndex());
		}
    }


// ---------------------------------------------------------------------------
// Custom handling of menu pane for markable lists
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleMarkableListDynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleMarkableListDynInitMenuPane" );

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
        CurrentSelectionIndicesL();
        TInt currentItem = CurrentLbxItemIndex();
        if ( currentItem != KErrNotFound )
            {
            TBool currentItemMarked = EFalse;
            for( int i = 0; i < iListSelections->Count(); i++ )
                {
                if ( currentItem == iListSelections->At(i) )
                    {
                    currentItemMarked = ETrue;
                    break;
                    }
                }

            aMenuPane->SetItemDimmed( EAknCmdMark, currentItemMarked );
            aMenuPane->SetItemDimmed( EAknCmdUnmark, !currentItemMarked );
            aMenuPane->SetItemDimmed( EAknMarkAll, iListSelections->Count() == iListWidget->ItemCount() );
            aMenuPane->SetItemDimmed( EAknUnmarkAll, iListSelections->Count() == 0 );
            }
        }
    }


// ---------------------------------------------------------------------------
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerHgImp::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    MPX_DEBUG5( "CMPXCommonContainerHgImp::HandleKeyEventL(iCode=%d, iScanCode=%d, iModifiers=0x%x, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iModifiers, aType );

    TKeyResponse response( EKeyWasNotConsumed );
    if ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter )
        {
        if( CurrentLbxItemIndex() >= 0 )
            {
            SaveSelectedAlbumItemL();
            }
        // Handle enter key pressed
        iView->ProcessCommandL( EMPXCmdCommonEnterKey );
        }
    if ( iListWidget )
        {
        iListWidget->OfferKeyEventL( aKeyEvent, aType );
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
void CMPXCommonContainerHgImp::ActivateContainerL()
    {
    // Nothing to do
    }


// ---------------------------------------------------------------------------
// From MMPXCommonListBoxArrayObserver
// Handle listbox array events.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleListBoxArrayEventL(
    MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleListBoxArrayEventL" );
    if ( aEvent == MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventIconArrayChange )
        {
        delete iIconArray;
        iIconArray = NULL;
        CreateIconArrayL();
        }
    else if ( aEvent == MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventMediaArrayChange )
        {

		// Update list widget with new items in the media array.
        // We start the update from last valid media item index.
		if ( iListWidget )
			{
			ProvideDataWithoutThumbnailsL(iListBoxArray->MediaArray(), iLastValidMediaItemIndex);

            if ( ( iLastValidMediaItemIndex >= iRequestStart )
                 && ( iLastValidMediaItemIndex >= iRequestEnd )
                 && ( iRequestStart != iRequestEnd ) )
                {
                Request( iRequestStart, iRequestEnd, EHgBufferReset );
                }

			iListWidget->RefreshScreen(iListWidget->FirstIndexOnScreen());
			}
        }
    }


// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerHgImp::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    ASSERT( iViewContainer );
    MPX_DEBUG5( "CMPXCommonContainerHgImp::OfferKeyEventL(iCode=%d, iScanCode=%d, iModifiers=0x%x, aType=%d)",
        aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iModifiers, aType );

	// Let the view container handle the key event.
	return iViewContainer->HandleKeyEventL( aKeyEvent, aType );
    }


// ---------------------------------------------------------------------------
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    ASSERT( iViewContainer );
    MPX_FUNC( "CMPXCommonContainerHgImp::GetHelpContext" );
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
void CMPXCommonContainerHgImp::HandleResourceChange( TInt aType )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleResourceChange" );
    CCoeControl::HandleResourceChange( aType );

    TRAP_IGNORE(
        if ( aType == KAknsMessageSkinChange )
            {
            iListWidget->SetDefaultIconL(NULL);
            delete iIconArray;
            iIconArray = NULL;
            CreateIconArrayL();
			iListWidget->EnableScrollBufferL(*this, KMPXListBufferSize, KMPXListBufferSize/4);
            }
        else if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            TRect clientRect = ((CAknView*)iView)->ClientRect();
            iBgContext->SetRect(CEikonEnv::Static()->EikAppUi()->ApplicationRect());
            if ( iScrollerWithTitle )
                {
            	iScrollerWithTitle->InitScreenL(clientRect);
            	iPrevListWidget->InitScreenL(clientRect);
                }
            else
                {
            	iListWidget->InitScreenL(clientRect);
                }
            SetScrollbarTypeL();
            }
        );
    }


// ---------------------------------------------------------------------------
// Draw this application's view to the screen
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::Draw(const TRect& /*aRect*/) const
    {
    // Do nothing so that list model will be drawn by hg list
    }


// ---------------------------------------------------------------------------
// From MCoeControlObserver
// Handle control event
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleControlEventL(
    CCoeControl* /*aControl*/,
    TCoeEvent /*aEventType*/ )
    {
    }


// ---------------------------------------------------------------------------
// From CCoeControl
// Provide control input capabilities
// ---------------------------------------------------------------------------
//
TCoeInputCapabilities CMPXCommonContainerHgImp::InputCapabilities() const
	{
	if( iListWidget )
		{
		return iListWidget->InputCapabilities();
		}
	return CCoeControl::InputCapabilities();
	}


// ---------------------------------------------------------------------------
// From MHgSelectionObserver
// Handle Item Selection
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleSelectL( TInt /*aIndex*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleSelect" );
    }


// ---------------------------------------------------------------------------
// From MHgSelectionObserver
// Handle Item opened
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::HandleOpenL( TInt /*aIndex*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::HandleOpenL" );

    SaveSelectedAlbumItemL();
	iView->ProcessCommandL( EMPXCmdCommonEnterKey );
    }


// -----------------------------------------------------------------------------
// Provides the data to the model
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::Request(
	TInt aBufferStart,
	TInt aBufferEnd,
	THgScrollDirection aDirection )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::Request" );
   	MPX_DEBUG3( "CMPXCommonContainerHgImp::Request aBufferStart = %d, aBufferEnd = %d",
            aBufferStart, aBufferEnd );

	// This should not happen but if not handled, bad things will happen.
	if ( aBufferStart < 0 || aBufferEnd < 0 )
   	    return;

  
  
    //iBottomIndex = aBufferEnd;

    if ( aBufferStart > iLastValidMediaItemIndex || aBufferEnd > iLastValidMediaItemIndex )
		{
		// We cannot provide data for the requested range at this time.
		// Remember the request so we can handle it when there's enough data.
		iRequestStart = aBufferStart;
    	iRequestEnd = aBufferEnd;
    	return;
		}
    iDirection = aDirection;

    for ( TInt i = aBufferStart; i <= aBufferEnd; i++ )
        {
        iThumbnailReqMap[ i ] = EFalse;
        }

    MPX_TRAPD( err,
            if ( aDirection == EHgBufferReset || aDirection == EHgBufferScrollDown || aDirection == EHgBufferScrollUp)
                {
                // For index range in visible-area
                ProvideDataIntersectL(aBufferStart, aBufferEnd);
                // For index range elsewhere
                ProvideDataDifferenceL(aBufferStart, aBufferEnd);
                }
            else
                {
                ProvideDataL(aBufferStart, aBufferEnd);
                } );
    
    if( err != KErrNone )
    	{
        MPX_DEBUG2( "CMPXCommonContainerHgImp::Request leave err = %d", err );
    	}
	//Refresh(aBufferStart);
    // When in main collection view, list can be refresh
    // without waiting on thumbnails since this list doesn't
    // require thumbnails.
    if ( iContext == EContextGroupCollection ||
         iContext == EContextGroupPlaylist ||
         iContext == EContextGroupGenre ||
         iContext == EContextGroupComposer )
        {
        iListWidget->RefreshScreen( aBufferStart );
        }
    iTopIndex = aBufferStart;
    iBottomIndex = aBufferEnd;    
    }

// -----------------------------------------------------------------------------
// Check if request range also covers the visible area and provide data
// to it first.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ProvideDataIntersectL(
	TInt aBufferStart,
	TInt aBufferEnd )
    {
    TInt firstOnScreen = iListWidget->FirstIndexOnScreen();
    TInt lastOnScreen = firstOnScreen + iListWidget->ItemsOnScreen();

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();

    for ( TInt i = firstOnScreen; i <= lastOnScreen; i++)
        {
        for ( TInt j = aBufferStart; j <= aBufferEnd; j++)
            {
            if ( i == j )
                {
                // Just get the exiting item and update the fields + icon.
                CHgItem* item = &iListWidget->ItemL(i);

                CMPXMedia* currentMedia( mediaArray.AtL( i ) );

                // Add detailed text to the list item, if available
                AddDetailToDisplayedItemL( item, currentMedia, i );
                AddThumbnailToDisplayedItemL( item, currentMedia, i );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Handles request range outside of visible area.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ProvideDataDifferenceL(
	TInt aBufferStart,
	TInt aBufferEnd)
    {
    TBool found = EFalse;
    TInt firstOnScreen = iListWidget->FirstIndexOnScreen();
    TInt lastOnScreen = firstOnScreen + iListWidget->ItemsOnScreen();

    if (aBufferStart == firstOnScreen && aBufferEnd == lastOnScreen)
        return;

    const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    for ( TInt i = aBufferStart; i <= aBufferEnd; i++)
        {
        found = EFalse;
        // look for index in visible range
        for ( TInt j = firstOnScreen; j <= lastOnScreen; j++)
            {
            if ( i == j )
                {
                found = ETrue;
                break;
                }
            }
        if (!found)
            {
            // Just get the exiting item and update the fields + icon.
            CHgItem* item = &iListWidget->ItemL(i);

            CMPXMedia* currentMedia( mediaArray.AtL( i ) );

            // Add detailed text to the list item, if available
            AddDetailToDisplayedItemL( item, currentMedia, i );
            AddThumbnailToDisplayedItemL( item, currentMedia, i );
            }
        }
    }
// -----------------------------------------------------------------------------
// CMPXCommonContainerHgImp::ProvideDataWithouThumbnailsL
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ProvideDataWithoutThumbnailsL(
	const CMPXMediaArray& aMediaArray,
	TInt aStartIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::ProvideDataWithouThumbnailsL" );

    for ( TInt i = aStartIndex; i < aMediaArray.Count() ; i++ )
        {
        CMPXMedia* currentMedia( aMediaArray.AtL( i ) );

		if ( currentMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ==
			KMPXInvalidItemId )
			{
		
			break;
			}

        // Just get the exiting item and update the fields + icon.
        CHgItem* item = &iListWidget->ItemL(i);
        SetTitleL( item, currentMedia );
        AddDetailToDisplayedItemL( item, currentMedia, i );
		iLastValidMediaItemIndex = i;
	
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonContainerHgImp::ProvideDataL
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ProvideDataL( TInt aStart, TInt aEnd )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::ProvideDataL" );
   	MPX_DEBUG3( "CMPXCommonContainerHgImp::ProvideDataL aStart = %d, aEnd = %d",
            aStart, aEnd );
   	if ( aStart < 0 ) aStart = 0;
   	if ( aEnd < 0 ) aEnd = 0;

	const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
    for ( TInt i = aStart; i <= aEnd ; i++ )
        {
        // Just get the exiting item and update the fields + icon.
        CHgItem* item = &iListWidget->ItemL(i);

        CMPXMedia* currentMedia( mediaArray.AtL( i ) );

        // Add detailed text to the list item, if available
        AddDetailToDisplayedItemL( item, currentMedia, i );
        AddThumbnailToDisplayedItemL( item, currentMedia, i );
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonContainerHgImp::Release
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::Release(TInt aReleaseStart, TInt aReleaseEnd)
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::Release" );

    TInt queuecount=0;

  	if (iQueueCounter !=0)
		{
		for (TInt i=0; i<iQueueCounter; i++ )
			{
          	iAlbumArtRequest = iRequestQueue.First();
          	iRequestQueue.Remove(*iAlbumArtRequest);

          	if ( (iAlbumArtRequest->iIndex) >= aReleaseStart
               &&  (iAlbumArtRequest->iIndex) <=aReleaseEnd )
				{
               	delete iAlbumArtRequest->iAlbumArtUri;  //not sure needed or not
               	delete iAlbumArtRequest;
               	}
			else
				{
				iRequestQueue.AddLast(*iAlbumArtRequest);
				++queuecount;
				}
			}
		iQueueCounter = queuecount;
       }
    }


// -----------------------------------------------------------------------------
// Preview thumbnail generation or loading is complete.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ThumbnailPreviewReady(
	MThumbnailData& /*aThumbnail*/,
	TThumbnailRequestId /*aId*/ )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::ThumbnailPreviewReady" );
    // Not used
    }


// -----------------------------------------------------------------------------
// Final thumbnail bitmap generation or loading is complete.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::ThumbnailReady(
	TInt aError, MThumbnailData& aThumbnail,
	TThumbnailRequestId aId )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::ThumbnailReady" );
    MPX_DEBUG2( "CMPXCommonContainerHgImp::ThumbnailReady error = %d", aError);
   
    TInt index = 0;
    // Find the index
    const TInt *ret = iThumbnailReqHashMap.Find( (TInt)aId);
     if ( ret )
		 {
		 index = *ret;
		 }

    if ( aError == KErrNone && ret )
        {
            CMPXMedia* currentMedia( NULL );
            const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();
            MPX_TRAPD( err, currentMedia = mediaArray.AtL( index ) );
            if( err != KErrNone )
            	{
                __ASSERT_DEBUG( EFalse, User::Panic( _L( "CMPXCommonContainerHgImp::ThumbnailReady()" ), err ) );
                return;
            	}
            
            if ( currentMedia->IsSupported( KMPXMediaGeneralId ) )
                {
                CFbsBitmap* bmap = aThumbnail.DetachBitmap();
                if ( bmap )
                    {
					CGulIcon* icon( NULL );
					MPX_TRAP( err,  icon = CGulIcon::NewL( bmap, NULL ) );
					if( err != KErrNone )
						{
						__ASSERT_DEBUG( EFalse, User::Panic( _L( "CMPXCommonContainerHgImp::ThumbnailReady()" ), err ) );
						return;
						}
					
                    if ( iScrollerWithTitle && !iTitleSet )
                        {
                        iScrollerWithTitle->TitleItem().SetIcon(icon);
                        iScrollerWithTitle->MakeVisible(ETrue);
                        iListWidget->MakeVisible( ETrue );

                        iScrollerWithTitle->DrawNow();
                        iTitleSet = ETrue;
                        }
                    else
                        {
                        MPX_TRAPD( err, iListWidget->ItemL(index).SetIcon(icon); )
                        if( err != KErrNone )
                        	{
                            MPX_DEBUG2( "CMPXCommonContainerHgImp::ThumbnailReady--iListWidget->ItemL(index).SetIcon(icon) leave err%d", err);
                        	}
                        iThumbnailReqMap[index] = ETrue;
                        Refresh( index );
                        }
                    }
				}
			iThumbnailReqHashMap.Remove( (TInt)aId);

			// if request queue is not empty, we can send one or more requests
			if ( !iRequestQueue.IsEmpty() )
				{
				iAlbumArtRequest = iRequestQueue.First();
				iRequestQueue.Remove(*iAlbumArtRequest);
				--iQueueCounter;
				TInt index = iAlbumArtRequest->iIndex;

				const TDesC&   albumArtUri = *(iAlbumArtRequest->iAlbumArtUri);
				TRAP_IGNORE
				(
				// ThumbnailManager has became stricter on mimetype, if it does not match, it would fail.
				// so it is better to give KNullDesC if not sure
				CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( albumArtUri, KNullDesC );
				delete iAlbumArtRequest->iAlbumArtUri;
				delete iAlbumArtRequest;
				// Using negative index as priority will ensure that thumbnail requests
				// are processed in the order they were requested.
				TInt ret = NULL;
				TInt reqId;
				MPX_TRAP(err, reqId = (TInt) iThumbnailManager->GetThumbnailL( *source, (TAny*)ret, -index ));
				if ( err == KErrNone)
					{
					iThumbnailReqHashMap.InsertL( reqId, index );
					}
				CleanupStack::PopAndDestroy( source );
				);
				}
			 else
				{
				//request queue is empty, decrease outstanding requests count
			iRequestCount--;
			}
		}

	else
		{

		 if ( ret )
			 {
			 index = *ret;
			 iThumbnailReqHashMap.Remove( (TInt)aId);
			 }
		else
			{
			return;
			}
		TRAP_IGNORE
		(
		//no albumart supported
		SetDefaultIconL(index);
        if ( iScrollerWithTitle && !iTitleSet )
            {
            iScrollerWithTitle->MakeVisible(ETrue);
            iListWidget->MakeVisible( ETrue );
            iScrollerWithTitle->DrawNow();
            iTitleSet = ETrue;
            }
        else
            {
            iThumbnailReqMap[index] = ETrue;
            Refresh(index);
            }
        );
        }

    }

// ---------------------------------------------------------------------------
// Refresh for item without thumbnail
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::RefreshNoThumbnailL(TInt aIndex)
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::RefreshNoThumbnail" );

    if ( iScrollerWithTitle && !iTitleSet )
        {
        iScrollerWithTitle->MakeVisible(ETrue);
        iListWidget->MakeVisible( ETrue );
        iScrollerWithTitle->DrawNow();
        iTitleSet = ETrue;
        }
    else
        {
        Refresh(aIndex);
        }
    }
// ---------------------------------------------------------------------------
// Refresh list as needed.
// This function tries to determine the best time to refresh the screen as to
// avoid too many redraws. In some cases, multiple refresh is unavoidable.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::Refresh(TInt aIndex)
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::Refresh" );

	TInt mediaCount = iListBoxArray->MediaArray().Count();

	TInt firstOnScreen = iListWidget->FirstIndexOnScreen();
	TInt lastOnScreen = firstOnScreen + iListWidget->ItemsOnScreen();

	if (lastOnScreen > (mediaCount - 1))
		lastOnScreen = mediaCount - 1;

	// Refresh screen if the item is within view and no more thumbnails expected.
	if ( aIndex >= firstOnScreen && aIndex <= lastOnScreen )
		{
		TBool canRefresh(ETrue);
		for (TInt i= firstOnScreen; i <= lastOnScreen; i++)
			{
			if ( !iThumbnailReqMap[i] )
				{
				canRefresh = EFalse;
				break;
				}
			}
		if ( canRefresh )
		  {
			iListWidget->RefreshScreen( firstOnScreen );
		}
    }
    }
// ---------------------------------------------------------------------------
// Create list box
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::CreateListBoxL(TInt count)
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CreateListBox" );

    TRect clientRect = ((CAknView*)iView)->ClientRect();
    iListWidget = CHgDoubleGraphicList::NewL (
    		clientRect,
    		count,
    		NULL,
    		DSA_PARAM );
    iListWidget->SetMopParent(this);
    iListWidget->SetSelectionObserver(*this);
    iListWidget->ClearFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
    iListWidget->SetFocus(ETrue);
    iListWidget->EnableScrollBufferL(*this, KMPXListBufferSize, KMPXListBufferSize/4);
    iListWidget->SetScrollBarTypeL( CHgScroller::EHgScrollerScrollBar );
    }


// ---------------------------------------------------------------------------
// Create list box with Title
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::CreateListBoxWithTitleL( TInt count )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CreateListBoxWithTitleL" );

    TRect clientRect = ((CAknView*)iView)->ClientRect();
    iScrollerWithTitle = CHgScrollerWithTitle::NewL (
            clientRect,
            count,
            CHgScrollerWithTitle::EHgScrollerDoubleTextList,
            NULL);

    if ( iSelectedMediaInAlbumView )
        {
        delete iSelectedMediaInAlbumView;
        iSelectedMediaInAlbumView = NULL;
        }

    iSelectedMediaInAlbumView = CMPXMedia::NewL();

    ReadFromStreamFileL( iSelectedMediaInAlbumView );

    CHgItem *item = &iScrollerWithTitle->TitleItem();
    SetDetailAlbumL( item, iSelectedMediaInAlbumView );
    SetDetailArtistL( item, iSelectedMediaInAlbumView );
    SetDetailIconL( item, 1 );


    iTitleSet = EFalse;

    iScrollerWithTitle->SetMopParent(this);
    iScrollerWithTitle->SetFocus(ETrue);
    iScrollerWithTitle->InitScreenL( clientRect );
    iScrollerWithTitle->MakeVisible(EFalse);

    iListWidget = &iScrollerWithTitle->Scroller();
    iListWidget->SetSelectionObserver(*this);
    iListWidget->EnableScrollBufferL(*this, KMPXListBufferSize, KMPXListBufferSize/4);
    iListWidget->SetScrollBarTypeL( CHgScroller::EHgScrollerScrollBar );
    iListWidget->MakeVisible(EFalse);
    }


// -----------------------------------------------------------------------------
// Creates icon array, populates iIconArray to be used by listbox
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::CreateIconArrayL()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::CreateIconArrayL" );
    if ( iListBoxArray )
        {
        iIconArray = iListBoxArray->CreateIconArrayL();
        }
    SetDefaultIconL();
    }
// -----------------------------------------------------------------------------
// Sets default icon to the HgList
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDefaultIconL()
	{

	TDefaultIcon defaultIcon( EMPXDefaultIconEmpty );
	TInt iconIndex( EMPXClvIconEmpty );

	switch( iContext )
		{
		case EContextGroupArtist:
		case EContextItemArtist:
		case EContextGroupAlbum:
			{
			break;
			}
		case EContextGroupPodcast:
		case EContextItemPodcast:
			{
			defaultIcon = EMPXDefaultTitleIconPodcasts;
			iconIndex = EMPXPodClvIconTitle; // Podcast default icon
			break;
			}
		case EContextGroupPlaylist:
			{
			defaultIcon = EMPXDefaultIconPlaylist;
			iconIndex = EMPXClvIconPlaylist;
			break;
			}
		case EContextGroupGenre:
			{
			defaultIcon = EMPXDefaultIconGenre;
			iconIndex = EMPXClvIconGenre;
			break;
			}
		case EContextGroupComposer:
			{
			defaultIcon = EMPXDefaultIconComposer;
			iconIndex = EMPXClvIconComposer;
			break;
			}
		case EContextItemComposer:
		case EContextGroupSong:
		case EContextItemSong:
		case EContextItemPlaylist:
		case EContextItemGenre:
		case EContextItemAlbum:
		case EContextGroupCollection:
		case EContextUnknown:
		default:
			{
			// no default icon for main menu items
			break;
			}
		}

	if ( iListWidget )
		{

		CGulIcon* icon = (*iIconArray)[iconIndex];
    	CFbsBitmap* bitmap = icon->Bitmap();
    	CFbsBitmap* mask = icon->Mask();

     	TSize size(iImageSize.iWidth, iImageSize.iHeight);
	    AknIconUtils::SetSize( bitmap, size );
	    AknIconUtils::SetSize( mask, size );

		CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
		iconCopy->SetBitmapsOwnedExternally(ETrue);

		iListWidget->SetDefaultIconL(iconCopy);
		iCurrentDefaultIcon = defaultIcon;
		}
	}

// -----------------------------------------------------------------------------
// Sets default icon to list item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDefaultIconL(TInt aIndex)
	{

	TDefaultIcon defaultIcon( EMPXDefaultIconNotSet );
	TInt iconIndex( EMPXClvIconNone );

	switch( iContext )
		{
		case EContextGroupArtist:
		case EContextItemArtist:
		    {
		    defaultIcon = EMPXDefaultIconArtist;
		    iconIndex = EMPXClvIconArtist;
		    break;
		    }
		case EContextGroupAlbum:
			{
			defaultIcon = EMPXDefaultIconAlbum;
			iconIndex = EMPXClvIconAlbum;
			break;
			}
		case EContextGroupPodcast:
		case EContextItemPodcast:
			{
			defaultIcon = EMPXDefaultTitleIconPodcasts;
			iconIndex = EMPXPodClvIconTitle; // Podcast default icon
			break;
			}
		case EContextItemComposer:
		case EContextGroupSong:
		case EContextItemSong:
		case EContextItemPlaylist:
		case EContextItemGenre:
		case EContextItemAlbum:
			{
			defaultIcon = EMPXDefaultIconSongs;
			iconIndex = EMPXClvIconSongs;
			break;
			}
		default:
			{
			break;
			}
		}

	if ( iListWidget && defaultIcon != EMPXDefaultIconNotSet )
		{


		CGulIcon* icon = (*iIconArray)[iconIndex];
    	CFbsBitmap* bitmap = icon->Bitmap();
    	CFbsBitmap* mask = icon->Mask();

     	TSize size(iImageSize.iWidth, iImageSize.iHeight);
	    AknIconUtils::SetSize( bitmap, size );
	    AknIconUtils::SetSize( mask, size );

		CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
		iconCopy->SetBitmapsOwnedExternally(ETrue);
        iListWidget->ItemL(aIndex).SetIcon(iconCopy);
		}
	}


// -----------------------------------------------------------------------------
// Adds detail to the list item at the specified index
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::AddDetailToDisplayedItemL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::AddDetailToDisplayedItemL" );

    switch ( iContext )
        {
        case EContextGroupCollection:
            {
            SetDetailIconL( aVisualItem, aIndex );
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupArtist:
            {
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupAlbum:
        	{
        	SetDetailArtistL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupPlaylist:
            {
            SetDetailDurationL( aVisualItem, aMedia );
            SetDetailIndicatorL( aVisualItem, aIndex );
          //  SetDetailIconL( aVisualItem, EMPXDefaultIconPlaylist ); // playlist icon
            break;
            }
        case EContextItemAlbum:
            {
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
        case EContextGroupSong:
        case EContextItemPlaylist:
        case EContextItemGenre:
        case EContextItemComposer:
        	{
            SetDetailArtistL( aVisualItem, aMedia );
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
        case EContextGroupPodcast:
            {
            SetDetailCountL( aVisualItem, aMedia );
          //  SetDetailIconL( aVisualItem, EMPXDefaultTitleIconPodcasts );
            break;
            }
        case EContextGroupGenre:
            {
          //  SetDetailIconL( aVisualItem, EMPXDefaultIconGenre ); //genre icon
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextGroupComposer:
            {
           // SetDetailIconL( aVisualItem, EMPXDefaultIconComposer ); //composer icon
            SetDetailCountL( aVisualItem, aMedia );
            break;
            }
        case EContextItemArtist:
        	{
            SetDetailCountL(aVisualItem, aMedia);
            break;
            }
        case EContextItemSong:
        	{
            SetDetailAlbumL( aVisualItem, aMedia );
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
        case EContextItemPodcast:
        	{
            SetDetailIndicatorL( aVisualItem, aIndex );
            break;
            }
		}
	}

// -----------------------------------------------------------------------------
// Adds thumbnail to the list item at the specified index
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::AddThumbnailToDisplayedItemL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::AddThumbnailToDisplayedItemL" );


    switch ( iContext )
        {
        case EContextGroupCollection:
             {

             SetDetailIconL( aVisualItem, aIndex );
             break;
             }
        case EContextGroupAlbum:
        case EContextGroupArtist:
        case EContextGroupSong:
        case EContextItemPlaylist:
        case EContextItemGenre:
        case EContextItemComposer:
        case EContextItemArtist:
        case EContextItemSong:
        case EContextItemPodcast:
            {
            SetDetailThumbnailL( aMedia, aIndex );
            break;
            }
        case EContextItemAlbum:
        	{
            // This is needed for this context because we want the album art
            // thumbnail to be in list title area.
            SetDetailThumbnailL( iSelectedMediaInAlbumView, aIndex );
            break;
			}
        }
    }

// -----------------------------------------------------------------------------
// Set title to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetTitleL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetTitleL" );
	if ( aMedia->IsSupported( KMPXMediaGeneralTitle ) )
		{
		const TDesC& title = aMedia->ValueText( KMPXMediaGeneralTitle );
#ifdef HG_MP_LOC_AVAILABLE
        HBufC* unknownText =
                StringLoader::LoadLC( R_MPX_COMMONCONTAINER_UNKNOWN );
		if ( title.Compare( KNullDesC ) != 0 )
			{
    		aVisualItem->SetTitleL( title );
			}
	    else
	        {
    		aVisualItem->SetTitleL( *unknownText );
	        }
        CleanupStack::PopAndDestroy( unknownText );
#else //HG_MP_LOC_AVAILABLE
		TBuf<KMPXMaxFileLength> titleText( KUnknown );
		if ( title.Compare( KNullDesC ) != 0 )
			{
			titleText.Copy(title.Left(titleText.MaxLength()));
			}
		aVisualItem->SetTitleL( titleText );
#endif //HG_MP_LOC_AVAILABLE
		}
    }

// -----------------------------------------------------------------------------
// Set Detail - Count to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDetailCountL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetDetailCountL" );
	if ( aMedia->IsSupported( KMPXMediaGeneralCount ) )
		{
		TInt count( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount ) );

#ifdef HG_MP_LOC_AVAILABLE
        if ( iContext == EContextGroupPodcast )
			{
			if ( count > 1 )
			    {
                HBufC* episodesTitle = StringLoader::LoadLC( R_MPX_COMMONCONTAINER_DETAILS_NUMBER_OF_EPISODES, count );
                TPtr ptr = episodesTitle->Des();
                AknTextUtils::LanguageSpecificNumberConversion( ptr );
                aVisualItem->SetTextL( ptr );
                CleanupStack::PopAndDestroy( episodesTitle );
			    }
			else
			    {
        		aVisualItem->SetTextL( *iEpisodeTitle );
			    }
			}
        else if ( iContext == EContextGroupArtist )
			{
			if ( count > 1 )
			    {
                HBufC* albumsTitle = StringLoader::LoadLC( R_MPX_COMMONCONTAINER_DETAILS_NUMBER_OF_SONGS, // mod by anjokela
                    count );
                TPtr ptr = albumsTitle->Des();
                AknTextUtils::LanguageSpecificNumberConversion( ptr );
                aVisualItem->SetTextL( ptr );
                CleanupStack::PopAndDestroy( albumsTitle );
			    }
			else
			    {
        		aVisualItem->SetTextL( *iSongTitle ); // mod by anjokela
			    }
			}
		else
			{
			if ( count > 1 )
			    {
                HBufC* songsTitle = StringLoader::LoadLC( R_MPX_COMMONCONTAINER_DETAILS_NUMBER_OF_SONGS, count );
                TPtr ptr = songsTitle->Des();
                AknTextUtils::LanguageSpecificNumberConversion( ptr );
        		aVisualItem->SetTextL( ptr );
                CleanupStack::PopAndDestroy( songsTitle );
			    }
			else
			    {
        		aVisualItem->SetTextL( *iSongTitle );
			    }
			}
#else //HG_MP_LOC_AVAILABLE

        TBuf<10> temp;
        temp.AppendNum( count );
        AknTextUtils::LanguageSpecificNumberConversion( temp );

		TBuf<KMPXMaxFileLength> detailText;
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
		aVisualItem->SetTextL( detailText );
#endif //HG_MP_LOC_AVAILABLE

		}
    }

// -----------------------------------------------------------------------------
// Set Detail - Artist to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDetailArtistL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetDetailArtistL" );
	if ( aMedia->IsSupported( KMPXMediaMusicArtist ) )
		{
		// AK - Needs to be localized
		TBuf<KMPXMaxFileLength> detailText;
		const TDesC& artist = aMedia->ValueText( KMPXMediaMusicArtist );
		if ( artist.Compare( KNullDesC ) != 0 )
			{
			detailText.Copy(artist.Left(detailText.MaxLength()));
	        aVisualItem->SetTextL( detailText );
			}
	    else
	        {
            HBufC* unknownText =
                StringLoader::LoadLC( R_MPX_COMMONCONTAINER_UNKNOWN );
    		aVisualItem->SetTextL( *unknownText );
            CleanupStack::PopAndDestroy( unknownText );
            }
		}
    }

// -----------------------------------------------------------------------------
// Set Detail - Album to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDetailAlbumL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetDetailAlbumL" );
	if ( aMedia->IsSupported( KMPXMediaMusicAlbum ) )
		{
		TBuf<KMPXMaxFileLength> detailText;
		const TDesC& album = aMedia->ValueText( KMPXMediaMusicAlbum );
		if ( album.Compare( KNullDesC ) != 0 )
			{
			detailText.Copy( album.Left(detailText.MaxLength()));
			if ( iScrollerWithTitle )
			    {
    	        aVisualItem->SetTitleL( detailText );
			    }
			else
			    {
    	        aVisualItem->SetTextL( detailText );
			    }
			}
	    else
	        {
            HBufC* unknownText =
                StringLoader::LoadLC( R_MPX_COMMONCONTAINER_UNKNOWN );
    		aVisualItem->SetTextL( *unknownText );
            CleanupStack::PopAndDestroy( unknownText );
            }
		}
    }


// -----------------------------------------------------------------------------
// Set Detail - Count + Duration to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDetailDurationL(
    CHgItem* aVisualItem,
    CMPXMedia* aMedia )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetDetailDurationL" );
    if ( iContext == EContextGroupPlaylist &&
         aMedia->IsSupported( KMPXMediaGeneralDuration ) &&
	     aMedia->IsSupported( KMPXMediaGeneralCount ) )
		{
		// AK - Needs to be localized
		TBuf<KMPXMaxFileLength> detailText;
		TInt count( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount ) );
   		TInt duration = aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration );

#ifdef HG_MP_LOC_AVAILABLE
    	if ( count > 1 || count == 0 )
    	    {
            HBufC* songsTitle = StringLoader::LoadLC( R_MPX_COMMONCONTAINER_DETAILS_NUMBER_OF_SONGS, count );
    	    detailText.Append(*songsTitle);
    	    detailText.Append(KMPXDash);
            UpdateTimeIndicatorsL(detailText, duration);
    		aVisualItem->SetTextL( detailText );
            CleanupStack::PopAndDestroy( songsTitle );
    	    }
    	else
    	    {
    	    detailText.Append(*iSongTitle);
    	    detailText.Append(KMPXDash);
            UpdateTimeIndicatorsL(detailText, duration);
    		aVisualItem->SetTextL( detailText );
    	    }
#else //HG_MP_LOC_AVAILABLE
		TBuf<10> temp;
		temp.AppendNum( count );
        AknTextUtils::LanguageSpecificNumberConversion( temp );
        detailText.Append( temp );
		detailText.Append( (count > 1 ) ? KSongsDash() : KSongDash() );
        UpdateTimeIndicatorsL(detailText, duration);
        aVisualItem->SetTextL( detailText );
#endif //HG_MP_LOC_AVAILABLE
		}
    else if ( iContext == EContextItemPodcast &&
         aMedia->IsSupported( KMPXMediaGeneralDuration ) &&
         aMedia->IsSupported( KMPXMediaGeneralSize ) )
        {
        TBuf<KMPXMaxFileLength> detailText;
        TInt duration = aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralDuration );
        UpdateTimeIndicatorsL(detailText, duration);
        detailText.Append( KMPXDash() );

        TInt size( aMedia->ValueTObjectL<TInt>( KMPXMediaGeneralSize ) );
        HBufC* sizeBuf = iCommonUiHelper->UnitConversionL( size, ETrue );
        TPtr sizePtr( sizeBuf->Des() );
        detailText.Append( sizePtr );
        delete sizeBuf;

        aVisualItem->SetTextL( detailText );
        }
    }


// -----------------------------------------------------------------------------
// Format the duration to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::UpdateTimeIndicatorsL(
    TDes& aBuf,
    TInt aDuration )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::UpdateTimeIndicatorsL" );
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
void CMPXCommonContainerHgImp::SetDetailIndicatorL(
    CHgItem* aVisualItem,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetDetailIndicatorL" );
    // Get icon indices
    RArray<TInt> iconIndices = iViewContainer->IndicatorsL( aIndex );
    CleanupClosePushL( iconIndices );

    const TInt iconCount = iconIndices.Count();
    TInt flags = aVisualItem->Flags() & CHgItem::EHgItemFlagMarked;
    aVisualItem->ClearFlags(aVisualItem->Flags());
    if ( iconCount > 0 )
    	{
        TInt iconId = 0;
		for ( TInt iconIndex = 0; iconIndex < iconCount; iconIndex++ )
			{
			iconId = iconIndices[iconIndex];
			if ( !iPodcastContext )
			    {
                switch (iconId)
                    {
                    case EMPXClvIconMMC:
                        {
                        flags |= CHgItem::EHgItemFlagsMmc;
                        break;
                        }
                    case EMPXClvIconPlay:
                        {
                        flags |= CHgItem::EHgItemFlagsPlaybackIndicator;
                        break;
                        }
                    case EMPXClvIconPause:
                        {
                        flags |= CHgItem::EHgItemFlagsPauseIndicator;
                        break;
                        }
                    case EMPXClvIconBrokenTrack:
                        {
                        flags |= CHgItem::EHgItemFlagsBrokenTrackIndicator;
                        break;
                        }
                    case EMPXClvIconCorruptTrack:
                        {
                        flags |= CHgItem::EHgItemFlagsCorruptedTrackIndicator;
                        break;
                        }
                    case EMPXClvIconDrmExpired:
                        {
                        flags |= CHgItem::EHgItemFlagsDrmRightsExpired;
                        break;
                        }
                    case EMPXClvIconBrokenPlaylist:
                        {
                        flags |= CHgItem::EHgItemFlagsBrokenPlaylistIndicator;
                        break;
                        }
                    case EMPXClvIconReorderDown:
                        {
                    	flags |= CHgItem::EHgItemFlagsMoveDownIndicator;
                    	break;
                        }
                    case EMPXClvIconReorderUp:
                        {
                    	flags |= CHgItem::EHgItemFlagsMoveUpIndicator;
                    	break;
                        }
                    case EMPXClvIconReorderUpDown:
                        {
                    	flags |= CHgItem::EHgItemFlagsMoveUpDownIndicator;
                    	break;
                        }
                    }
			    }
            else // podcast icons
                {
                switch (iconId)
                    {
                    case EMPXPodClvIconMMC:
                        {
                        flags |= CHgItem::EHgItemFlagsMmc;
                        break;
                        }
                    case EMPXPodClvIconPlay:
                        {
                        flags |= CHgItem::EHgItemFlagsPlaybackIndicator;
                        break;
                        }
                    case EMPXPodClvIconPause:
                        {
                        flags |= CHgItem::EHgItemFlagsPauseIndicator;
                        break;
                        }
                    case EMPXPodClvIconBroken:
                        {
                        flags |= CHgItem::EHgItemFlagsBrokenTrackIndicator;
                        break;
                        }
                    case EMPXPodClvIconCorrupt:
                        {
                        flags |= CHgItem::EHgItemFlagsCorruptedTrackIndicator;
                        break;
                        }
                    case EMPXPodClvIconNew:
                        {
                        flags |= CHgItem::EHgItemFlagsNewPodcastIndicator;
                        break;
                        }
                    case EMPXPodClvIconCompplay:
                        {
                        flags |= CHgItem::EHgItemFlagsCompletelyPlayedPodcastIndicator;
                        break;
                        }
                    case EMPXPodClvIconPartplay:
                        {
                        flags |= CHgItem::EHgItemFlagsPlayedPodcastIndicator;
                        break;
                        }
                    }
                }
			}
		}
    aVisualItem->SetFlags( flags );
    CleanupStack::PopAndDestroy();
    }


// -----------------------------------------------------------------------------
// Set Detail - Icon to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDetailIconL(
    CHgItem* aVisualItem,
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetDetailIconL" );

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
        case 4:
            iconIndex = EMPXClvIconPodcasts;
            break;
        case 5:
            iconIndex = EMPXClvIconGenre;
            break;
        case 6:
            iconIndex = EMPXClvIconComposer;
            break;
        case 7: //EMPXDefaultTitleIconPodcasts
            iconIndex = 10;
            break;
		}

    CGulIcon* icon = (*iIconArray)[iconIndex];

	CFbsBitmap* bitmap = icon->Bitmap();
	CFbsBitmap* mask = icon->Mask();

	TSize size(iImageSize.iWidth, iImageSize.iHeight);
    if ( iContext == EContextGroupCollection )
        { // shrink smaller to look same as in Antriksh
        size.iWidth = iImageSize.iWidth * KIconFactor;
        size.iHeight = iImageSize.iHeight * KIconFactor;
        }

    AknIconUtils::SetSize( bitmap, size );
    AknIconUtils::SetSize( mask, size );

    CGulIcon* iconCopy = CGulIcon::NewL(bitmap, mask);
	iconCopy->SetBitmapsOwnedExternally(ETrue);
    aVisualItem->SetIcon( iconCopy );
    }

// -----------------------------------------------------------------------------
// Set Detail - Thumbnail to the visual item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetDetailThumbnailL(
    CMPXMedia* aMedia,
    TInt aIndex )
    {
	  MPX_FUNC("CMPXCommonContainerHgImp::SetDetailThumbnailL(CMPXMedia* aMedia,TInt aIndex)");
    TMPXItemId currentId( aMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

    if ( currentId == KMPXInvalidItemId )
        {
        return;
        }

	if ( aMedia->IsSupported( KMPXMediaMusicAlbumArtFileName ) )
		{
		const TDesC& album = aMedia->ValueText( KMPXMediaMusicAlbumArtFileName );
		#ifdef _DEBUG
		RDebug::RawPrint(album);
		#endif
		if ( album.Compare( KNullDesC ) != 0 )
			{
			//send request only when under TNM max loads

			if ( iRequestCount < KMaxThumbnailReq )
				{
				// ThumbnailManager has became stricter on mimetype, if it does not match, it would fail.
				// so it is better to give KNullDesC if not sure
                CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( album, KNullDesC );
				TInt reqId;
				TInt ret = NULL;
				// Set priority based on the index so that the smaller indexes are loaded first.
				TRAPD(err, reqId = (TInt) iThumbnailManager->GetThumbnailL( *source, (TAny*)ret, -aIndex));
				iRequestCount++;

				if ( err == KErrNone)
					  {
					  iThumbnailReqHashMap.InsertL( reqId, aIndex );
					  }
				CleanupStack::PopAndDestroy( source );
				}
				 //can not send request since max TNM load, add album art requests to queue
			else
				{
				TAlbumArtRequest* albumArtRequest = NULL;

				TRAPD(err,  albumArtRequest = new (ELeave) TAlbumArtRequest;);
				if ( err != KErrNone )
					{
					return;
					}
				albumArtRequest->iIndex = aIndex;
				albumArtRequest->iAlbumArtUri = album.AllocL();

				// Outstanding request exits; add it to the queue
				iRequestQueue.AddLast(*albumArtRequest);
				++iQueueCounter;

				}
			}
		else
			{
			//no albumart
			SetDefaultIconL(aIndex);
    		iThumbnailReqMap[aIndex] = ETrue;
    		RefreshNoThumbnailL(aIndex);
			}
		}
	else
	    {
		// no album art supported
        SetDefaultIconL(aIndex);
    	iThumbnailReqMap[aIndex] = ETrue;
        RefreshNoThumbnailL(aIndex);
	    }
    }


// -----------------------------------------------------------------------------
// Sets the current category.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetCollectionContextL()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetCollectionContextL" );

    const CMPXMedia& media = iListBoxArray->ContainerMedia();

    TMPXGeneralType containerType( EMPXNoType );
    if ( media.IsSupported( KMPXMediaGeneralType ) )
        {
        containerType = media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
        }

    TMPXGeneralCategory containerCategory( EMPXNoCategory );
    if ( media.IsSupported( KMPXMediaGeneralCategory ) )
        {
        containerCategory = media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
        }
    MPX_DEBUG3( "CMPXCommonContainerHgImp::SetCollectionContextL container type = %d, category = %d",
            containerType, containerCategory );

    iPodcastContext = EFalse;
    TMPXPodcastCategory podcastCategory( EMPXUnknown );
    if ( media.IsSupported( KMPXMediaPodcastType ) )
        {
        iPodcastContext = ETrue;
        if ( media.IsSupported( KMPXMediaPodcastCategoryGroup ) )
            {
            podcastCategory = media.ValueTObjectL<TMPXPodcastCategory>( KMPXMediaPodcastCategoryGroup );
            }
        }
    MPX_DEBUG3( "CMPXCommonContainerHitchcockImp::SetCollectionContextL container podcast = %d, category = %d",
            iPodcastContext, podcastCategory );

    iPrevContext = iContext;

    iListWidget->ClearFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
    if ( !iPodcastContext )
        {
        if ( containerType == EMPXGroup )
            {
            switch (containerCategory)
                {
                case EMPXCollection:
                    iContext = EContextGroupCollection;
                    iListWidget->SetFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
                    break;
                case EMPXArtist:
                    iContext = EContextGroupArtist;
                    break;
                case EMPXAlbum:
                    iContext = EContextGroupAlbum;
                    break;
                case EMPXPlaylist:
                    iContext = EContextGroupPlaylist;
                    break;
                case EMPXSong:
                    iContext = EContextGroupSong;
                    break;
                case EMPXGenre:
                    iContext = EContextGroupGenre;
                    break;
                case EMPXComposer:
                    iContext = EContextGroupComposer;
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
                    break;
                case EMPXAlbum:
                    iContext = EContextItemAlbum;
                    break;
                case EMPXPlaylist:
                    iContext = EContextItemPlaylist;
                    break;
                case EMPXSong:
                    iContext = EContextItemSong;
                    break;
                case EMPXGenre:
                    iContext = EContextItemGenre;
                    break;
                case EMPXComposer:
                    iContext = EContextItemComposer;
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
            iListWidget->SetFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
            }
        else if ( (containerType == EMPXItem) && (podcastCategory == EMPXEpisode) )
            {
            iContext = EContextItemPodcast;
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXCommonContainerHgImp::MopSupplyObject
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CMPXCommonContainerHgImp::MopSupplyObject( TTypeUid aId )
	{
    MPX_FUNC( "CMPXCommonContainerHgImp::MopSupplyObject" );
	return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}

// -----------------------------------------------------------------------------
// CMPXCommonContainerHgImp::SetScrollbarType
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerHgImp::SetScrollbarTypeL()
    {
    MPX_FUNC( "CMPXCommonContainerHgImp::SetScrollbarType" );

    if ( iContext == EContextGroupCollection ||
         iContext == EContextGroupPlaylist ||
         iContext == EContextItemAlbum ||
         iContext == EContextItemPlaylist )
        {
        iListWidget->SetScrollBarTypeL( CHgScroller::EHgScrollerScrollBar );
        }
    else
        {
        iListWidget->SetScrollBarTypeL( CHgScroller::EHgScrollerLetterStripLite );
        }
    }

// ----------------------------------------------------------------------------
// Clears the TN request if any
// ----------------------------------------------------------------------------
void CMPXCommonContainerHgImp::CancelTNRequest()
    {
    MPX_FUNC("CMPXCommonContainerHgImp::CancelTNRequest");

    if ( iThumbnailReqHashMap.Count() == 0 )
        {
        return;
        }

	THashMapIter<TInt, TInt> iter(iThumbnailReqHashMap);
	TInt* id = iter.CurrentValue();
    if (id && *id >= 0)
        {
        iThumbnailManager->CancelRequest((TThumbnailRequestId)id);
        }
	while ( TInt const* id = iter.NextValue() )
		{
		iThumbnailManager->CancelRequest((TThumbnailRequestId)*id);
		}
    }

// ----------------------------------------------------------------------------
// Save the selected album item
// ----------------------------------------------------------------------------
void CMPXCommonContainerHgImp::SaveSelectedAlbumItemL()
    {
    if ( iContext == EContextGroupAlbum || iContext == EContextItemArtist )
        {
        const CMPXMediaArray& mediaArray = iListBoxArray->MediaArray();

        if ( iSelectedMediaInAlbumView )
            {
            delete iSelectedMediaInAlbumView;
            iSelectedMediaInAlbumView = NULL;
            }

        iSelectedMediaInAlbumView = CMPXMedia::NewL( *mediaArray.AtL( CurrentLbxItemIndex() ) );

        if ( iSelectedMediaInAlbumView->IsSupported( KMPXMediaGeneralTitle ) )
            {
            const TDesC& title = iSelectedMediaInAlbumView->ValueText( KMPXMediaGeneralTitle );
            if ( title == KNullDesC )
                {
                // Cannot use reference to KNullDesC, must
                // use the literal directly.
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicAlbum, KNullDesC );
                }
            else
                {
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicAlbum, title );
                }
            }

        if ( iContext == EContextItemArtist )
            {
            const CMPXMedia& containerMedia = iListBoxArray->ContainerMedia();
            const TDesC& artist = containerMedia.ValueText( KMPXMediaGeneralTitle );

            HBufC* unknownArtistText =
                    StringLoader::LoadLC( R_MPX_QTN_NMP_UNKNOWN_ARTIST );

            if ( artist != unknownArtistText->Des() )
                {
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicArtist, artist );
                }
            else
                {
                iSelectedMediaInAlbumView->SetTextValueL( KMPXMediaMusicArtist, KNullDesC );
                }

            CleanupStack::PopAndDestroy( unknownArtistText );
            }

        WriteToStreamFileL( iSelectedMediaInAlbumView );
        }
    }

// ----------------------------------------------------------------------------
// Write the album name, artist name and album art in media data to file
// ----------------------------------------------------------------------------
void CMPXCommonContainerHgImp::WriteToStreamFileL( const CMPXMedia* aMedia )
   {
    MPX_FUNC( "CMPXCommonContainerHgImp::WriteToStreamFileL" );

    CCoeEnv* coeEnv( iEikonEnv );
    RFs& fs = coeEnv->FsSession();
    fs.MkDirAll( iSelectedAlbumItemFileName );
    TParse parsedName;

    fs.Parse( iSelectedAlbumItemFileName, parsedName );
    CFileStore* store( NULL );
    MPX_TRAPD( error, store = CDirectFileStore::ReplaceL(fs,
                                                 parsedName.FullName(),
                                                 EFileWrite); );

    if ( error == KErrNone )
        {
        CleanupStack::PushL( store );
        store->SetTypeL( KDirectFileStoreLayoutUid );

        RStoreWriteStream wstream;
        TStreamId stream = wstream.CreateLC( *store );
        store->SetRootL( stream );

        if ( aMedia->IsSupported( KMPXMediaMusicAlbum ) )
            {
            const TDesC& album = aMedia->ValueText( KMPXMediaMusicAlbum );
            wstream << album;
            }
        else
            {
            wstream << KNullDesC;
            }

        if ( aMedia->IsSupported( KMPXMediaMusicAlbumArtFileName ) )
            {
            const TDesC& art = aMedia->ValueText( KMPXMediaMusicAlbumArtFileName );
            wstream << art;
            }
        else
            {
            wstream << KNullDesC;
            }

        if ( aMedia->IsSupported( KMPXMediaMusicArtist ) )
            {
            const TDesC& artist = aMedia->ValueText( KMPXMediaMusicArtist );
            wstream << artist;
            }
        else
            {
            wstream << KNullDesC;
            }

        wstream.CommitL();
        CleanupStack::PopAndDestroy( &wstream );
        CleanupStack::PopAndDestroy( store );
        }
    else
        {
        fs.Delete( iSelectedAlbumItemFileName );
        }
   }

// ----------------------------------------------------------------------------
// Read the album name, artist name and album art from file
// ----------------------------------------------------------------------------
void CMPXCommonContainerHgImp::ReadFromStreamFileL( CMPXMedia* aMedia )
    {
    MPX_FUNC("CMPXCommonContainerHgImp::ReadFromStreamFileL");

    CCoeEnv* coeEnv( iEikonEnv );
    RFs& fs = coeEnv->FsSession();
    fs.MkDirAll( iSelectedAlbumItemFileName );
    TParse parsedName;

    fs.Parse( iSelectedAlbumItemFileName, parsedName );
    CFileStore* store( NULL );
    MPX_TRAPD( error, store = CDirectFileStore::OpenL(fs,
                                                 parsedName.FullName(),
                                                 EFileRead ); );
    if ( error == KErrNone )
        {
        CleanupStack::PushL( store );

        RStoreReadStream stream;
        stream.OpenLC( *store, store->Root() );

        TBuf<KMPXMaxFileLength> album( KNullDesC );
        stream >> album;
        aMedia->SetTextValueL( KMPXMediaMusicAlbum, album );

        TBuf<KMPXMaxFileLength> art( KNullDesC );
        stream >> art;
        aMedia->SetTextValueL( KMPXMediaMusicAlbumArtFileName, art );

        TBuf<KMPXMaxFileLength> artist( KNullDesC );
        stream >> artist;
        aMedia->SetTextValueL( KMPXMediaMusicArtist, artist );

        CleanupStack::PopAndDestroy( &stream );
        CleanupStack::PopAndDestroy( store );
        }
    else
        {
        aMedia->SetTextValueL( KMPXMediaMusicAlbum, KNullDesC );
        aMedia->SetTextValueL( KMPXMediaMusicAlbumArtFileName, KNullDesC );
        aMedia->SetTextValueL( KMPXMediaMusicArtist, KNullDesC );
        }
    }

// ----------------------------------------------------------------------------
// Clean Albumart request queue
// ----------------------------------------------------------------------------
void CMPXCommonContainerHgImp::CleanAlbumArtReqQueue()
    {
    MPX_FUNC("CMPXCommonContainerHgImp::CleanAlbumArtReqQueue");
    while ( !iRequestQueue.IsEmpty() )
        {
        iAlbumArtRequest = iRequestQueue.First();
        iRequestQueue.Remove(*iAlbumArtRequest);
        delete iAlbumArtRequest->iAlbumArtUri;  //not sure needed or not
        delete iAlbumArtRequest;
        }
        iQueueCounter = 0;
    }

//  End of File

