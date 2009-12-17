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
* Description:  Implementation of common container for AVKON platform.
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

#include <data_caging_path_literals.hrh>
#include <mpxcommoncontainer.rsg>
#include <mpxlog.h>
#include <mpxuser.h>

#include "mpxcommonlistboxarraybase.h"
#include "mpxcommoncontaineravkonimp.h"
#include "mpxcommoncontainer.hrh"
#include <mpxlbxextendedfeatures.h>

// CONSTANTS
const TInt KMPXFindboxTextLimit = 40;

_LIT( KMPXCommonContainerRscPath, "mpxcommoncontainer.rsc" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CMPXCommonContainerAvkonImp::CMPXCommonContainerAvkonImp() :
    iIsFindEnabled( ETrue ),
    iIsMarkingEnabled( ETrue ),
    iContainerActive( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXCommonContainerAvkonImp::~CMPXCommonContainerAvkonImp()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::~CMPXCommonContainerAvkonImp" );    
    
    FeatureManager::UnInitializeLib();        
        
    if ( iResourceOffset )
        {
        iEikonEnv->DeleteResourceFile( iResourceOffset );
        }
        
    delete iListBox;
    delete iFindBox;
    delete iLbxExtFeat;
    }

// ---------------------------------------------------------------------------
// Return Coe Control
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXCommonContainerAvkonImp::CoeControl()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::CoeControl" );
    return this;
    }

// ---------------------------------------------------------------------------
// Set View Container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetViewContainer(
    MMPXViewContainer* aViewContainer )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetViewContainer" );
    iViewContainer = aViewContainer;
    }

// ---------------------------------------------------------------------------
// Set command observer for the container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetCommandObserver(    
    MEikCommandObserver* aCommandObserver )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetCommandObserver" );
    iView = aCommandObserver;
    }

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetListBoxObserver(
    MEikListBoxObserver* aListBoxObserver )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetListBoxObserver" );
    iListBoxObserver = aListBoxObserver;
    }

// ---------------------------------------------------------------------------
// Set layout for all elements and set sizes for icons.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::UpdateLayout()
    {        
    MPX_FUNC( "CMPXCommonContainerAvkonImp::UpdateLayout" );
    }

// -----------------------------------------------------------------------------
// Returns a pointer to the listboxarray
// -----------------------------------------------------------------------------
//
CMPXCommonListBoxArrayBase* CMPXCommonContainerAvkonImp::ListBoxArray() const
    {
    ASSERT( iListBoxArray );
    return iListBoxArray;
    }

// -----------------------------------------------------------------------------
// Sets listbox array
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetListBoxArrayL(
    CMPXCommonListBoxArrayBase* aListBoxArray )
    {
    iListBoxArray = aListBoxArray;
    if ( iListBoxArray )
        {
        CTextListBoxModel* listboxModel = iListBox->Model();
        listboxModel->SetItemTextArray( iListBoxArray );
        listboxModel->SetOwnershipType( ELbmOwnsItemArray );
        }
    }

// -----------------------------------------------------------------------------
// Return number of listbox items shown in the current listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerAvkonImp::CurrentListItemCount() const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::CurrentListItemCount" );
    ASSERT( iFindBoxFilterItems );
    return iFindBoxFilterItems->FilteredNumberOfItems();
    }

// -----------------------------------------------------------------------------
// Return total number of listbox items in the listbox.
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerAvkonImp::TotalListItemCount() const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::TotalListItemCount" );
    ASSERT( iFindBoxFilterItems );
    return iFindBoxFilterItems->NonFilteredNumberOfItems();
    }

// -----------------------------------------------------------------------------
// Gets top listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerAvkonImp::TopLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::TopLbxItemIndex" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    TInt topItem( iListBox->TopItemIndex() );

    // Make sure there's at least one item in the listbox
    if ( iFindBoxFilterItems->FilteredNumberOfItems() )
        {
        // Get the real listbox index
        topItem = iFindBoxFilterItems->FilteredItemIndex( topItem );
        }
    return topItem;
    }
    
// -----------------------------------------------------------------------------
// Gets bottom listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerAvkonImp::BottomLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::BottomLbxItemIndex" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    TInt bottomItem( iListBox->BottomItemIndex() );

    // Make sure there's at least one item in the listbox
    if ( iFindBoxFilterItems->FilteredNumberOfItems() )
        {
        // Get the real listbox index
        bottomItem = iFindBoxFilterItems->FilteredItemIndex( bottomItem );
        }
    return bottomItem;
    }    

// -----------------------------------------------------------------------------
// Gets current listbox item index (relative to the original listbox).
// -----------------------------------------------------------------------------
//
TInt CMPXCommonContainerAvkonImp::CurrentLbxItemIndex() const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::CurrentLbxItemIndex" );    
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    TInt currentItem( iListBox->CurrentItemIndex() );

    // Make sure there's at least one item in the listbox
    if ( iFindBoxFilterItems->FilteredNumberOfItems() )
        {
        // Get the real listbox index
        currentItem = iFindBoxFilterItems->FilteredItemIndex( currentItem );
        }
    return currentItem;
    }

// -----------------------------------------------------------------------------
// Get current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
const CArrayFix<TInt>* CMPXCommonContainerAvkonImp::
    CurrentSelectionIndicesL() const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::CurrentSelectionIndicesL" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    const CArrayFix<TInt>* selectionArray( iListBox->SelectionIndexes() );

    if ( iFindBoxFilterItems->FilteredNumberOfItems() )
        {
        // Get the real listbox indices
        iFindBoxFilterItems->SelectionIndexes()->Reset();
        iFindBoxFilterItems->UpdateSelectionIndexesL();
        selectionArray = iFindBoxFilterItems->SelectionIndexes();
        }
    return selectionArray;
    }

// -----------------------------------------------------------------------------
// Get current selected listbox items indices (relative to the original listbox)
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetCurrentSelectionIndicesL(
    CArrayFix<TInt>* aIndices ) const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetCurrentSelectionIndicesL" );    
    ASSERT( iListBox );
    iListBox->SetSelectionIndexesL( aIndices ); // ownership not transfered
    }

// -----------------------------------------------------------------------------
// Set top listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetLbxTopItemIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetLbxTopItemIndex" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    TInt index( iFindBoxFilterItems->VisibleItemIndex( aIndex ) );
    if ( index == KErrNotFound )
        {
        index = 0;
        }
    iListBox->SetTopItemIndex( index );
    }

// -----------------------------------------------------------------------------
// Set current listbox item index.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetLbxCurrentItemIndex( TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetLbxCurrentItemIndex" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    TInt index( iFindBoxFilterItems->VisibleItemIndex( aIndex ) );
    if ( index == KErrNotFound )
        {
        index = 0;
        }
    iListBox->SetCurrentItemIndex( index );
    }

// -----------------------------------------------------------------------------
// Set current listbox item index and highlight it.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetLbxCurrentItemIndexAndDraw( 
    TInt aIndex )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetLbxCurrentItemIndexAndDraw" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    TInt index( iFindBoxFilterItems->VisibleItemIndex( aIndex ) );
    if ( index == KErrNotFound )
        {
        index = 0;
        }
    iListBox->SetCurrentItemIndexAndDraw( index );
    }

// -----------------------------------------------------------------------------
// Clear listbox selection.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::ClearLbxSelection()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::ClearLbxSelection" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    iListBox->ClearSelection();
    iFindBoxFilterItems->SelectionIndexes()->Reset();
    }

// -----------------------------------------------------------------------------
// Sets the text for an empty list box
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetLbxEmptyTextL( const TDesC& aText )
    {
    ASSERT( iListBox );
    iListBox->View()->SetListEmptyTextL( aText );
    }

// -----------------------------------------------------------------------------
// Draws a list box item
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::DrawLbxItem( TInt aIndex )
    {
    ASSERT( iListBox );
    TInt topIndex( TopLbxItemIndex() );
    TInt bottomIndex( BottomLbxItemIndex() );
    if ( aIndex >= topIndex && aIndex <= bottomIndex )
        {
        iListBox->DrawItem( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// Set Find box's focus.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetFindBoxFocus( TBool aFocus )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetFindBoxFocus" );
    ASSERT( iFindBox );
    if ( aFocus )
        {
        iFindBox->MakeVisible( ETrue );
        }
    iFindBox->SetFocus( aFocus );
    }

// -----------------------------------------------------------------------------
// Handle listbox item addition.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::HandleLbxItemAdditionL()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::HandleLbxItemAdditionL" );
    ASSERT( iListBox );
    ASSERT( iFindBoxFilterItems );
    iFindBoxFilterItems->HandleItemArrayChangeL();
    iListBox->HandleItemAdditionL();
    }
    
// -----------------------------------------------------------------------------
// Handle listbox item addition, preserving the current display index
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::HandleLbxItemAdditionPreserveIndexL()
    {        
    MPX_FUNC( "CMPXCommonContainerAvkonImp::HandleLbxItemAdditionPreserveIndexL" );
    TInt currentItem( CurrentLbxItemIndex() );
    TInt currentBottomIndex( BottomLbxItemIndex() );
    TInt topIndex( CalculateTopIndex( currentBottomIndex ) );
    HandleLbxItemAdditionL();
    if ( currentItem > 0 )
        {
        // only redraw if the selected index is > 0, since 
        // HandleLbxItemAdditionL already redraws it and set index
        // to 0
        SetLbxTopItemIndex( topIndex );
        SetLbxCurrentItemIndexAndDraw( currentItem );
        }
    }

// -----------------------------------------------------------------------------
// Handle listbox item removal.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::HandleLbxItemRemovalL()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::HandleLbxItemRemovalL" );
    ASSERT( iListBox );
    iListBox->HandleItemRemovalL();
    }
    
// -----------------------------------------------------------------------------
// Show/hide find box.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SetFindBoxVisibilityL( TBool aIsVisible )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SetFindBoxVisibilityL" );
    ASSERT( iListBox );
    ASSERT( iFindBox );
    if (aIsVisible != iFindBox->IsVisible())
        {
        iFindBox->MakeVisible( aIsVisible );
        iFindBox->SetFocus( aIsVisible );
        if ( !aIsVisible )
            {
            iFindBox->ResetL();
            iFindBoxFilterItems->HandleOfferkeyEventL();
            iListBox->SetFocus( ETrue );
            }
        SizeChanged();
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// Determine find box visibility
// -----------------------------------------------------------------------------
//
TBool CMPXCommonContainerAvkonImp::FindBoxVisibility()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::FindBoxVisibility" );
    ASSERT( iFindBox );
    return iFindBox->IsVisible();
    }

// -----------------------------------------------------------------------------
// Restore the PopupCBA of find box
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::RestoreFindBoxPopupCBA()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::RestoreFindBoxPopupCBA" );
    ASSERT( iFindBox );
    iFindBox->SetFocus( ETrue );
    iFindBox->SetFocus( EFalse );
    }
    
// -----------------------------------------------------------------------------
// Calculate the top index of the visible items
// -----------------------------------------------------------------------------
//    
TInt CMPXCommonContainerAvkonImp::CalculateTopIndex( TInt aBottomIndex )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::CalculateTopIndex" );
    ASSERT( iListBox );
    TInt numItems( iListBox->View()->NumberOfItemsThatFitInRect( 
        iListBox->View()->ViewRect() ) - 1 ); // to make it 0 based
    return ( ( aBottomIndex > numItems ) ? aBottomIndex - numItems : 0 );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::ConstructContainerL( TMPXCommonContainerCollectionType /*aCollectionType*/ )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::ConstructContainerL" );    
    
    CCoeEnv* coeEnv( iEikonEnv );
    TParse parse;
    parse.Set( KMPXCommonContainerRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );    
    
    CreateWindowL();
    
    CreateListBoxL();
    
    FeatureManager::InitializeLibL();
    
    ConstructFindBoxL( ETrue );                
    }

// ---------------------------------------------------------------------------
// Enable/disable find box  
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::EnableFindBox( TBool aEnable )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::EnableFindBox" );    
    iIsFindEnabled = aEnable;
    if ( !aEnable )
        {
        TRAP_IGNORE(SetFindBoxVisibilityL( EFalse ));
        };
    }

// ---------------------------------------------------------------------------
// Enable/disable marking  
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::EnableMarking( TBool aEnable )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::EnableMarking" );    
    iIsMarkingEnabled = aEnable;
    }
    
// ---------------------------------------------------------------------------
// Custom handling of commands for markable lists.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::HandleMarkableListProcessCommandL( TInt aCommand )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::HandleMarkableListProcessCommandL" );
    ASSERT( iListBox );

    // Handle in helper function
    AknSelectionService::HandleMarkableListProcessCommandL(
        aCommand,
        iListBox );
    }

// ---------------------------------------------------------------------------
// Custom handling of menu pane for markable lists
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::HandleMarkableListDynInitMenuPane(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::HandleMarkableListDynInitMenuPane" );
    ASSERT( iListBox );
    
    // Handle in helper function
    AknSelectionService::HandleMarkableListDynInitMenuPane(
        aResourceId,
        aMenuPane,
        iListBox );
    }
    
// ---------------------------------------------------------------------------
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerAvkonImp::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    ASSERT( iListBox );
    ASSERT( iFindBox );
    MPX_DEBUG5( "CMPXCommonContainerAvkonImp::HandleKeyEventL(iCode=%d, iScanCode=%d, iModifiers=0x%x, aType=%d)", 
        aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iModifiers, aType );

    TKeyResponse response( EKeyWasNotConsumed );

    // Pass the key event to the common container components
    if ( iIsFindEnabled &&
        ( ( iFindBox->IsVisible() ) || 
            ( aKeyEvent.iCode != EKeyBackspace ) ) )
        {
        TBool needRefresh( EFalse );

        if ( aKeyEvent.iCode == EKeyBackspace && 
            iFindBox->TextLength() == 0 )
            {
            // dismiss find box
            SetFindBoxVisibilityL( EFalse );
            response = EKeyWasConsumed;
            }
        else
            {
            // Offers the key event to find box.
            response = AknFind::HandleFindOfferKeyEventL( 
                aKeyEvent, aType, this, iListBox, iFindBox, iIsFindPopup, 
                needRefresh );
            }        

        if ( response == EKeyWasConsumed )
            {
            if ( needRefresh )
                {
                SizeChanged();
                DrawNow();
                }
            // If it's consumed by the findbox, there's no need to check
            // with listbox.
            return response;
            }
        }
    if ( ( aKeyEvent.iCode != EKeyLeftArrow ) && 
        ( aKeyEvent.iCode != EKeyRightArrow ) &&
        ( aKeyEvent.iCode != EKeyBackspace ) )
        {            
        // now it's iListBox's job to process the key event            
        if ( !iIsMarkingEnabled && 
            ( aKeyEvent.iCode == EKeyUpArrow || 
                aKeyEvent.iCode == EKeyDownArrow ||
                aKeyEvent.iCode == EKeyOK ) )
            {
            // if marking is not enabled, check if this is marking command 
            if ( aKeyEvent.iModifiers & EModifierLeftShift ||
                aKeyEvent.iModifiers & EModifierRightShift ||
                aKeyEvent.iModifiers & EModifierShift )
                {
                // strip the marking command, and let list box handle it
                TKeyEvent modKeyEvent( aKeyEvent );
                modKeyEvent.iModifiers &= ~EModifierLeftShift;
                modKeyEvent.iModifiers &= ~EModifierRightShift;
                modKeyEvent.iModifiers &= ~EModifierShift;
                response = iLbxExtFeat->HandleLbxKeyEventL( modKeyEvent, aType );                    
                }
            else
                {
                // not marking command
                response = iLbxExtFeat->HandleLbxKeyEventL( aKeyEvent, aType );
                }
            }
        else // marking is enabled
            {
            response = iLbxExtFeat->HandleLbxKeyEventL( aKeyEvent, aType );
    
#ifdef __ENABLE_MSK
            // check for marking command 
            if ( ( aKeyEvent.iModifiers & EModifierLeftShift ||
                aKeyEvent.iModifiers & EModifierRightShift ||
                aKeyEvent.iModifiers & EModifierShift ||
                aKeyEvent.iScanCode == EStdKeyDevice3 ||
                iIsFindPopup ) && aType == EEventKeyUp )
                {
                // Update the middle softkey for marking commands
                iView->ProcessCommandL( EMPXCmdCommonUpdateMiddleSoftKey );
                }
#endif // __ENABLE_MSK
            }
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
void CMPXCommonContainerAvkonImp::ActivateContainerL()
    {
    // Activate the container via Coe Control
    CCoeControl::ActivateL();
    iContainerActive = ETrue;
    }

// ---------------------------------------------------------------------------
// Deactivate container
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::DeactivateContainerL()
    {
    iContainerActive = EFalse;
    }

// ---------------------------------------------------------------------------
// Return container state
// ---------------------------------------------------------------------------
//
TBool CMPXCommonContainerAvkonImp::IsContainerActive()
    {
    return iContainerActive;
    }

// ---------------------------------------------------------------------------
// From MMPXCommonListBoxArrayObserver
// Handle listbox array events.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::HandleListBoxArrayEventL( 
    MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::HandleListBoxArrayEventL" );
    if ( aEvent == MMPXCommonListBoxArrayObserver::EMPXCommonListBoxArrayEventIconArrayChange )
        {
        delete iIconArray;
        iIconArray = NULL;
        CreateIconArrayL();
        static_cast<CEikColumnListBox*>( iListBox )->ItemDrawer()->ColumnData()->
            SetIconArray( iIconArray );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Responds to changes to the size and position of the contents of this 
// control.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::SizeChanged()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::SizeChanged" );

    if ( iIsFindPopup )
        {
        AknFind::HandlePopupFindSizeChanged( 
            this, 
            static_cast<CAknColumnListBox*>( iListBox ), 
            iFindBox );                
        }
    else
        {
        AknFind::HandleFixedFindSizeChanged( 
            this, 
            static_cast<CAknColumnListBox*>( iListBox ), 
            iFindBox );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// ---------------------------------------------------------------------------
//
TInt CMPXCommonContainerAvkonImp::CountComponentControls() const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::CountComponentControls" );
    ASSERT( iListBox );
    ASSERT( iFindBox );
    return 2; // number of components
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// ---------------------------------------------------------------------------
//
CCoeControl* CMPXCommonContainerAvkonImp::ComponentControl( TInt aIndex ) const
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::ComponentControl" );
    CCoeControl* control( NULL );
    switch ( aIndex )
        {
        case 0:
            {
            control = iListBox; // Returns the pointer to listbox object.            
            break;
            }        
        case 1:
            {
            control = iFindBox; // Returns the pointer to findbox object.
            break;
            }
        default:
            {
            MPX_DEBUG1( "CMPXCommonContainerAvkonImp::ComponentControl out of range" );
            ASSERT( 0 );
            break;
            }
        }
    return control;
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// ---------------------------------------------------------------------------
//
TKeyResponse CMPXCommonContainerAvkonImp::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    ASSERT( iViewContainer );
    MPX_DEBUG5( "CMPXCommonContainerAvkonImp::OfferKeyEventL(iCode=%d, iScanCode=%d, iModifiers=0x%x, aType=%d)", 
        aKeyEvent.iCode, aKeyEvent.iScanCode, aKeyEvent.iModifiers, aType );

    // Let the view container handle the key event.
    return iViewContainer->HandleKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// Gets the control's help context.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
    ASSERT( iViewContainer );
    MPX_FUNC( "CMPXCommonContainerAvkonImp::GetHelpContext" );
    // Let the view container handle the help context retrieval
    iViewContainer->HandleHelpContext( aContext );
    }

// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::HandleResourceChange( TInt aType )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::HandleResourceChange" );
    CCoeControl::HandleResourceChange( aType );
    
    TRAP_IGNORE(
        if ( aType == KAknsMessageSkinChange )
            {
            delete iIconArray;
            iIconArray = NULL;
            CreateIconArrayL();          
            static_cast<CEikColumnListBox*>( iListBox )->ItemDrawer()->ColumnData()->
                SetIconArray( iIconArray );
            }
        else if ( aType == KEikDynamicLayoutVariantSwitch && iView )
            {                        
            iView->ProcessCommandL( EMPXCmdCommonResourceChange );
            } );    
    }

// ---------------------------------------------------------------------------
// Create list box
// ---------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::CreateListBoxL()
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::CreateListBox" );
    
    iListBox = new ( ELeave ) CAknSingleGraphicStyleListBox();    
    iListBox->SetContainerWindowL( *this ); 
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_MPX_COMMON_CONTAINER_LBX );
    // construct's iListBox from resource file
    iListBox->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); // reader
    
    // Enable Marquee
    CEikColumnListBox* eikListbox = static_cast<CEikColumnListBox*>( iListBox );
    eikListbox->EnableExtendedDrawingL();
    eikListbox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    eikListbox->ItemDrawer()->ColumnData()->SetMarqueeParams(
        KMaxTInt, 6, 1000000); // Magic: loop forever, other params as default

    iListBox->SetListBoxObserver( iListBoxObserver );

    // Speed Scrolling
    delete iLbxExtFeat;
    iLbxExtFeat = NULL;
    iLbxExtFeat = CMPXLbxExtendedFeatures::NewL( iListBox, ETrue );
    iLbxExtFeat->EnableSpeedScrollL( ETrue );
    
    }

// -----------------------------------------------------------------------------
// Constructs Find box.
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::ConstructFindBoxL( TBool aIsFindPopup )
    {
    MPX_FUNC( "CMPXCommonContainerAvkonImp::ConstructFindBoxL" );
    CAknFilteredTextListBoxModel* lbxModel = 
        static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );

    iIsFindPopup = aIsFindPopup;

    CAknSearchField::TSearchFieldStyle flag( 
        iIsFindPopup ? CAknSearchField::EPopup : CAknSearchField::ESearch );

    iFindBox = CAknSearchField::NewL( 
        *this, flag, NULL, KMPXFindboxTextLimit );
    lbxModel->CreateFilterL( iListBox, iFindBox );
    iFindBoxFilterItems = lbxModel->Filter();

    // Limit Japanese input modes
    if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )
        {
        CEikEdwin& findEditor = iFindBox->Editor();
       
        findEditor.SetAknEditorInputMode( EAknEditorKatakanaInputMode );
       
        findEditor.SetAknEditorAllowedInputModes(
            EAknEditorKatakanaInputMode |
            EAknEditorHalfWidthTextInputMode |
            EAknEditorNumericInputMode );
        }

    if ( iIsFindPopup )
        {
        SetFindBoxVisibilityL( EFalse );
        }

    }

// -----------------------------------------------------------------------------
// Creates icon array, populates iIconArray to be used by listbox
// -----------------------------------------------------------------------------
//
void CMPXCommonContainerAvkonImp::CreateIconArrayL()
    {
    if ( iListBoxArray )
        {
        iIconArray = iListBoxArray->CreateIconArrayL();
        }
    }   

//  End of File
