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
* Description:  MPX common container definition for Hitchcock platform.
*
*/



#ifndef CMPXCOMMONCONTAINERHITCHCOCKIMP_H
#define CMPXCOMMONCONTAINERHITCHCOCKIMP_H


// INCLUDES
#include <alf/alfenv.h>
#include <alf/ialfwidgeteventhandler.h>
#include <mul/mulevent.h>
#include <mul/mulvisualitem.h>      // For Visual Item
#include <mul/imulmodelprovider.h>

#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>

#include <mpxattribute.h>
#include "mpxcommoncontainer.h"
#include "mpxcommoncontainersearchfieldwrapper.h"

// FORWARD DECLARATIONS
namespace Alf
    {
    class IMulListWidget;
    class IMulModel;
    class IAlfViewWidget;
    class MulVisualItem;
    }
class CAlfEnv;
class MEikCommandObserver;
class CEikMenuPane;
class MEikListBoxObserver;
class CAknIconArray;
class CMPXCommonListBoxArrayBase;
class CMPXMediaArray;
class CAknSearchField;
class MCoeControlObserver;
class CMPXCommonContainerMediaHelper;
class CMPXCommonContainerTextureManager;
class CMPXCommonUiHelper;
class CMPXCommonContainerFilteredList;

// CLASS DECLARATION

/**
 *  Container class for Hitchcok common container.
 *  The view class should implement the following virtual function
 *  from MEikCommandObserver 
 *
 *  virtual void ProcessCommandL(TInt aCommandId);
 *
 *  common container uses this to pass commands back to the view class
 *  the commands supported are defined in mpxcommoncontainer.hrh
 *
 *  view container class owns this class to access List widget commands, receive
 *  CoeControl events and GetHelpContext(). The view container class should
 *  implement the following virtual functions from MViewContainer
 *
 *  virtual void HandleKeyEvent();
 *  virtual void HandleHelpContent();
 *
 *  common container uses this to pass CoeControl events back to the view
 *  container class. 
 *
 *  view class should handle the following commands in ProcessCommandL
 *  from MEikCommandObserver: 
 *  
 *  EMPXCmdCommonDelete  // delete key pressed
 *  EMPXCmdCommonResourceChange  // resource change
 *  EMPXCmdCommonEnterKey  // enter key pressed
 *
 *  @lib mpxcommoncontainer.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CMPXCommonContainerHitchcockImp )
    : public CCoeControl
    , public MCoeControlObserver
    , public MMPXCommonContainer
    , public IAlfWidgetEventHandler
    , public Alf::IMulModelProvider
    , public MThumbnailManagerObserver
    , public MMPXCommonContainerSearchFieldObserver
    {
public:

    /**
     * default C++ constructor.
     */
    CMPXCommonContainerHitchcockImp();
    
    /**
     * Destructor.
     */
    virtual ~CMPXCommonContainerHitchcockImp();


// from base class MMPXCommonContainer

    /**
     * Return Coe Control
     *
     * @since 3.2
     */
    CCoeControl* CoeControl();

    /**
     * Set View Container
     *
     * @since 3.2
     */
    void SetViewContainer( MMPXViewContainer* aViewContainer );

    /**
     * Set command observer for container
     *
     * @since 3.2
     */
    void SetCommandObserver( MEikCommandObserver* aCommandObserver );

    /**
     * Set list box observer for container
     *
     * @since 3.2
     */
    void SetListBoxObserver( MEikListBoxObserver* aObserver );

    /**
     * Set layout for all elements and set sizes for icons.
     *
     * @since 3.1
     */
    void UpdateLayout();

    /**
     * Returns list box array, ownership not transfered
     *
     * @since 3.1
     * @return A pointer to the list box array
     */
    CMPXCommonListBoxArrayBase* ListBoxArray() const;
    
    /**
     * Sets up the listbox array, ownership not transferred
     */
    void SetListBoxArrayL( CMPXCommonListBoxArrayBase* aListBoxArray );

    /**
     * Return number of listbox items shown in the current listbox.
     *
     * @since 3.1
     * @return The number of items shown in the list box
     */
    TInt CurrentListItemCount() const;

    /**
     * Return total number of listbox items in the listbox.
     *
     * @since 3.1
     * @return Total items in the list box
     */
    TInt TotalListItemCount() const;

    /**
     * Gets top listbox item index (relative to the original listbox).
     * @return Top listbox item index
     *
     * @since 3.1
     */
    TInt TopLbxItemIndex() const;
    
    /**
     * Gets bottom listbox item index (relative to the original listbox).
     * @return Bottom listbox item index
     *
     * @since 3.1
     */
    TInt BottomLbxItemIndex() const;

    /**
     * Gets current listbox item index (relative to the original listbox).
     * @return Current listbox item index
     *
     * @since 3.1
     */
    TInt CurrentLbxItemIndex() const;

    /**
     * Get current selected listbox items indices (relative to the original
     * listbox). Ownership not transferred.
     *
     * @since 3.1
     * @return Current listbox item indices
     */
    const CArrayFix<TInt>* CurrentSelectionIndicesL() const;
    
    /**
     * Sewt current selected listbox items indices
     *
     * @since 3.1
     * @param aIndices Array of selection indices, ownership not transfered
     */
    void SetCurrentSelectionIndicesL( CArrayFix<TInt>* aIndices ) const;

    /**
     * Set top listbox item index.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    void SetLbxTopItemIndex( TInt aIndex );

    /**
     * Set current listbox item index.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    void SetLbxCurrentItemIndex( TInt aIndex );

    /**
     * Set current listbox item index and highlight it.
     *
     * @since 3.1
     * @param aIndex Actual listbox index.
     */
    void SetLbxCurrentItemIndexAndDraw( TInt aIndex );

    /**
     * Clear listbox selection.
     *
     * @since 3.1
     */
    void ClearLbxSelection();

    /**
     * Sets list box backround text. This text is visible if the list box 
     * has no items.
     *
     * @param aText The text for the empty list box background.
     * @since 3.2
     */
	void SetLbxEmptyTextL( const TDesC& aText );

    /**
     * Draws list box item index
     *
     * @since 3.2
     */
    void DrawLbxItem( TInt aIndex );
    
    /**
     * Set Find box's focus.
     *
     * @since 3.1
     * @param aFocus ETrue if set focus on FindBox. Otherwise, EFalse.
     */
    void SetFindBoxFocus( TBool aFocus );

    /**
     * Handle listbox item addition.
     *
     * @since 3.1
     */
    void HandleLbxItemAdditionL();
    
    /**
     * Handle listbox item addition, preserving the current display index.
     *
     * @since 3.1
     */
    void HandleLbxItemAdditionPreserveIndexL();
    
    /**
     * Handle listbox item removal.
     *
     * @since 3.1
     */
    void HandleLbxItemRemovalL();

    /**
     * Show/hide find box
     *
     * @since 3.1
     * @param aIsVisible ETrue to set findbox visible. Otherwise, EFalse
     */
    void SetFindBoxVisibilityL( TBool aIsVisible );

    /**
     * Determine find box visibility
     *
     * @since 3.1
     * @return ETrue if find box is currently visible, EFalse otherwise
     */
    TBool FindBoxVisibility();

    /**
     * Restore the PopupCBA of find box
     */
    void RestoreFindBoxPopupCBA();

    /**
     * Calculate the top index of the visible items
     *
     * @since 3.1
     * @param aBottomIndex the bottom index of the visible items
     * @return top index
     */
    TInt CalculateTopIndex( TInt aBottomIndex );

    /**
     * Creates the container
     *
     * @since 3.1
     * @param aCollectionType collection type
     */
    void ConstructContainerL( TMPXCommonContainerCollectionType aCollectionType );

    /**
     * Enable/disable find box     
     * 
     * @since 3.1
     * @aEnable ETrue to enable find box, EFalse to disable
     */
    void EnableFindBox( TBool aEnable );
    
    /**
     * Enable/disable marking     
     * 
     * @since 3.1
     * @aEnable ETrue to enable marking, EFalse to disable
     */
    void EnableMarking( TBool aEnable );

    /**
     * Custom handling of commands for markable lists.
     */
    void HandleMarkableListProcessCommandL( TInt aCommand );

    /**
     * Custom handling of menu pane for markable lists
     */
    void HandleMarkableListDynInitMenuPane(
        TInt aResourceId,
        CEikMenuPane* aMenuPane );
        
    /**
     * Handle listbox array events
     *
     *  @param aEvent list box event
     */
    void HandleListBoxArrayEventL( 
        MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent );

    /**
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );
    
    /**
     * Activate the container
     */
    void ActivateContainerL();

    /**
     * Deactivate the container
     */
    void DeactivateContainerL();

    /**
     * Checks whether the container is active
     */
    TBool IsContainerActive();

// from base class CCoeControl

    /**
     * From CCoeControl.
     * Handles key events.
     *
     * @param aKeyEvent The key event.
     * @param aType The type of key event.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );
    
    /**
     * From CCoeControl.
     * Gets the control's help context.
     *
     * @param aContext The control's help context.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    
    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );
    
    /**
     * From CoeControl.
     * Draw this application's view to the screen
     *
     * @param aRect the rectangle of this view that needs updating
     */
    void Draw( const TRect& aRect ) const;
    
// from base class MCoeControlObserver

    /**
    * From MCoeControlObserver, Acts upon changes in the hosted control's state.
    * This class's implementation is trivial and should be able to be safely 
    * re-implemented in directly client-derived classes. For non-base setting
    * page classes, a call to the base class should be made.
    * @param aControl Control that caused the event. (not used in default implementation)
    * @param aEventType Type of the event. 
    */
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

// from base class IAlfWidgetEventHandler

    /**
     * From IAlfWidgetEventHandler
     * Indicates whether event is accepted
     *
     * @param aControl widget control
     * @param aEvent the ALF event
     */
    bool accept( CAlfWidgetControl& aControl, const TAlfEvent& aEvent ) const;

    /**
     * From IAlfWidgetEventHandler
     * Sets the active states of the event handler
     *
     * @param aStates active states
     */
    void setActiveStates( unsigned int aStates );

    /**
     * From IAlfWidgetEventHandler
     * Performs the appropriate action for the ALF event
     *
     * @param aControl widget control
     * @param aEvent the ALF event.
     * @return TRUE if event consumed
     */
    AlfEventStatus offerEvent( CAlfWidgetControl& aControl, const TAlfEvent& aEvent );

    /**
     * From IAlfWidgetEventHandler
     * Make event handler interface using the specified type
     * @param aType
     */
    IAlfInterfaceBase* makeInterface( const IfId& aType );

    /**
     * From IAlfWidgetEventHandler
     * Sets the event handler data
     *
     * @param aData event handler initialization data
     */
    void setEventHandlerData(const AlfWidgetEventHandlerInitData& aData);

    /**
     * From IAlfWidgetEventHandler
     * Returns pointer to the event handler data
     */
    AlfWidgetEventHandlerInitData* eventHandlerData();

    /**
     * From IAlfWidgetEventHandler
     * Returns the type of EventHandler.
     * @see IAlfWidgetEventHandler::AlfEventHandlerType
     *
     * @return The type of event handler.
     */
    AlfEventHandlerType eventHandlerType();

    /**
     * From IAlfWidgetEventHandler
     * Returns information about the phase in the event handling cycle in
     * which the event hadler will be executed.
     * @see IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase
     *
     * @return Event handler execution phase.
     */
    AlfEventHandlerExecutionPhase eventExecutionPhase();

public: // from base class IMulModelProvider

    void ProvideData( int aStartIndex, int aCount, MulDataPath aPath );

public: // from base class MThumbnailManagerObserver

    /**
     * From MThumbnailManagerObserver
     * Preview thumbnail generation or loading is complete.
     *
     * @param aThumbnail     An object representing the resulting thumbnail.
     * @param aId            Request ID for the operation
     */
    void ThumbnailPreviewReady( MThumbnailData& aThumbnail, TThumbnailRequestId aId );

    /**
     * From MThumbnailManagerObserver
     * Final thumbnail bitmap generation or loading is complete.
     *
     * @param aError         Error code.
     * @param aThumbnail     An object representing the resulting thumbnail.
     * @param aId            Request ID for the operation.
     */
    void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail, TThumbnailRequestId aId );

public: // from base class MMPXCommonContainerSearchFieldObserver

    /**
     * From MMPXCommonContainerSearchFieldObserver
     * Call when filtering word changes. Causes filtering to be updated.
     * Calling with zero-length filtering word stops filtering.
     *
     * @param aWord New filtering word.
     */
    void NewFilteringWordL( const TDesC& aWord );

private:

    /**
     * Create list box
     */
    void CreateListBoxL();

    /**
     * Constructs Find box.
     *
     * @param aIsFindPopup ETrue if findbox is a popup type.
     *                     EFalse if finsbox is a fixed type.
     */
    void ConstructFindBoxL( TBool aIsFindPopup );

    /**
     * Creates icon array, populates iIconArray to be used by listbox
     */
    void CreateIconArrayL();

    /**
     * Creates the display item from the media
     */
    MulVisualItem* CreateDisplayedItemL( TInt aIndex );

    /**
     * Adds detail to the list item at the specified index
     */
    void AddDetailToDisplayedItemL( MulVisualItem* aVisualItem, CMPXMedia* aMedia, TInt aIndex );

    void SetTitleL( MulVisualItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailCountL( MulVisualItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailArtistL( MulVisualItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailAlbumL( MulVisualItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailDurationL( MulVisualItem* aVisualItem, CMPXMedia* aMedia );
    void UpdateTimeIndicatorsL( TDes& aBuf, TInt aDuration );
    void SetDetailIndicatorL( MulVisualItem* aVisualItem, TInt aIndex );
    void SetDetailIconL( MulVisualItem* aVisualItem, TInt aIndex );
    void SetDetailThumbnailL( MulVisualItem* aVisualItem, CMPXMedia* aMedia, TInt aIndex );

    /**
     * Set the collection context
     */
    void SetCollectionContextL();

    /**
     * Opens search field.
     */
    void ActivateSearchFieldL();

    /**
     * Closes search field, if open.
     */
    void DeactivateSearchField();

private: // data

    /**
     * Context of collection view. It's the exact location within collection navigation
     * based on category type + category.
     */
    enum TContext
        {
        EContextUnknown,
        EContextGroupCollection,
        EContextGroupArtist,
        EContextGroupAlbum,
        EContextGroupPlaylist,
        EContextGroupSong,
        EContextGroupPodcast,
        EContextGroupGenre,
        EContextGroupComposer,
        EContextItemArtist,
        EContextItemAlbum,
        EContextItemPlaylist,
        EContextItemSong,
        EContextItemPodcast,
        EContextItemGenre,
        EContextItemComposer
        };

    struct TThumnailReq
        {
        TThumbnailRequestId iReqId;
        TInt                iIndex;
        };

    MMPXViewContainer*          iViewContainer;         // Not owned
    MEikCommandObserver*        iView;                  // Not owned
    MEikListBoxObserver*        iListBoxObserver;       // Not owned
    CMPXCommonListBoxArrayBase* iListBoxArray;          // Owned
    CArrayFixFlat<TInt>*        iListSelections;        // Owned
    CAknIconArray*              iIconArray;             // Owned

//    CAknSearchField*            iFindBox;  // Owned

    CMPXCommonContainerTextureManager*  iTextureManager;// Owned
	CMPXCommonUiHelper*         iCommonUiHelper;        // Owned

    CAlfEnv*                    iEnv;                   // Not owned
    IMulListWidget*             iListWidget;            // Not owned
    IMulModel*                  iWidgetModel;           // Not owned
    IAlfViewWidget*             iViewWidget;            // Not owned

    CThumbnailManager*          iThumbnailManager;      // Owned
    RArray<TThumnailReq>        iThumbnailReqArray;     // Owned

    CMPXCommonContainerSearchFieldWrapper* iSearchFieldWrapper;  // Owned
    CMPXCommonContainerFilteredList* iFilteredList;     // Owned

    TContext                    iContext;
    TInt                        iResourceOffset;        // Must be freed

    TBool                       iIsFindPopup;
    TBool                       iIsFindEnabled;
    TBool                       iIsMarkingEnabled;

	TInt						iTopIndex;
	TInt						iBottomIndex;
	TInt						iIconHeight;
	TInt						iIconWidth;
	TInt						iIndicatorHeight;
	TInt						iIndicatorWidth;

    TBool                       iAlfEnvCreated;
    TMPXCommonContainerCollectionType  iCollectionType;
    TBool                       iContainerActive;
    };

#endif  // CMPXCOMMONCONTAINERHITCHCOCKIMP_H

// End of File
