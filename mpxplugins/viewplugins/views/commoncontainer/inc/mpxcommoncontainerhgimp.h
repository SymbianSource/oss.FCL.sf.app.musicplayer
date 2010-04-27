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
* Description:  MPX common container definition for hg lib.
*
*/



#ifndef CMPXCOMMONCONTAINERHGIMP_H
#define CMPXCOMMONCONTAINERHGIMP_H


// INCLUDES
#include <e32hashtab.h>
#include <ganes/HgScrollBufferObserverIface.h>
#include <ganes/HgSelectionObserverIface.h>
#include <ganes/HgItem.h>
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>

#include <mpxattribute.h>
#include "mpxcommoncontainer.h"


// FORWARD DECLARATIONS
class MEikCommandObserver;
class CEikMenuPane;
class MEikListBoxObserver;
class CAknIconArray;
class CMPXCommonListBoxArrayBase;
class CMPXMediaArray;
class CAknSearchField;
class MCoeControlObserver;
class CMPXCommonContainerMediaHelper;
class CMPXCommonContainerTextureManager2;
class CMPXCommonUiHelper;
class CHgScroller;
class CHgScrollerWithTitle;
class CAknsBasicBackgroundControlContext;

//Albumart request queue
struct TAlbumArtRequest
	{
  TInt        iIndex;
	HBufC*		iAlbumArtUri;
  TSglQueLink iLink;
	};


// CLASS DECLARATION

/**
 *  Container class for Hg common container.
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
NONSHARABLE_CLASS( CMPXCommonContainerHgImp )
    : public CCoeControl
    , public MCoeControlObserver
    , public MMPXCommonContainer
	, public MHgScrollBufferObserver
	, public MHgSelectionObserver
    , public MThumbnailManagerObserver
    {
public:

    /**
     * default C++ constructor.
     */
    CMPXCommonContainerHgImp();

    /**
     * Destructor.
     */
    virtual ~CMPXCommonContainerHgImp();


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
    void DrawLbxItemL( TInt aIndex );

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
     * @param aRect the rectangle that defines the control's extent.
     */
    void ConstructContainerL( TMPXCommonContainerCollectionType /*aCollectionTyp*/ );

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
    void HandleMarkableListDynInitMenuPaneL(
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

    /**
     * From CoeControl.
     * Get the control input capabilities
     */
	TCoeInputCapabilities InputCapabilities() const;

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


// from base class MHgScrollBufferObserver

    /**
     * from MHgScrollBufferObserver
     * Buffer position changed.
     * The items between the aBufferStart and aBufferEnd should be loaded as soon as possible.
     *
     * @param aBufferStart The index of the first item in buffer.
     * @param aBufferEnd The index of the last item in buffer.
     * @param aDirection report direction of scrolling
     */
    void Request(TInt aBufferStart, TInt aBufferEnd, THgScrollDirection aDirection);

// from base class MHgBufferOwner

    /**
     * from MHgBufferOwner
     *
     *
     */
    void Release(TInt aReleaseStart, TInt aReleaseEnd);


// from base class MHgSelectionObserver

    /**
     * Handle selection.
     *
     * @param aIndex Selected item index.
     */
    void HandleSelectL( TInt aIndex );

    /**
     * Handle opening.
     *
     * @param aIndex Opened item index.
     */
    void HandleOpenL( TInt aIndex );

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

    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);


private:

    /**
     * Create list box
     */
    void CreateListBoxL( TInt count );

    /**
     * Create list box with Title
     */
    void CreateListBoxWithTitleL( TInt count );


    /**
     * Creates icon array, populates iIconArray to be used by listbox
     */
    void CreateIconArrayL();

    /**
     * Sets default icon for HgList
     */
    void SetDefaultIconL();

    /**
     * Sets default icon for list item
     */
    void SetDefaultIconL(TInt aIndex);

    /*
	 * Adds detail to the list item at the specified index
     */
    void AddDetailToDisplayedItemL( CHgItem* aVisualItem, CMPXMedia* aMedia, TInt aIndex);
    void AddThumbnailToDisplayedItemL( CHgItem* aVisualItem, CMPXMedia* aMedia, TInt aIndex);
    void SetTitleL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailCountL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailArtistL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailAlbumL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailDurationL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void UpdateTimeIndicatorsL( TDes& aBuf, TInt aDuration );
    void SetDetailIndicatorL( CHgItem* aVisualItem, TInt aIndex );
    void SetDetailIconL( CHgItem* aVisualItem, TInt aIndex );
    void SetDetailThumbnailL( CMPXMedia* aMedia, TInt aIndex = 0 );

    /**
     * Set the collection context
     */
    void SetCollectionContextL();

    void ProvideDataL( TInt aStart, TInt aEnd );

    void SetScrollbarTypeL();

    void CancelTNRequest();

    /**
      * Save the selected album item
      *
      */
    void SaveSelectedAlbumItemL();

    /**
     * Write the album name, artist name and album art in media data to file
     *
     * @param aMedia         the selected media data in Album view
     */
    void WriteToStreamFileL( const CMPXMedia* aMedia );

    /**
     * Read the album name, artist name and album art from file
     *
     * @param aMedia         the media data will be returned
     */
    void ReadFromStreamFileL( CMPXMedia* aMedia );

    /**
     * Refresh list as needed
     */
    void Refresh(TInt aIndex);

    /**
     * Refresh list as needed
     */
    void RefreshNoThumbnailL(TInt aIndex);
    
	/**
	* Clean AlbumArt request Queue
	*/
    void CleanAlbumArtReqQueue();

    /**
     * Handle requests in visible area
     * @param aBufferStart  the starting buffer index
     * @param aBufferEnd  the ending buffer index
     */
    void ProvideDataIntersectL(TInt aBufferStart, TInt aBufferEnd);

    /**
     * Refresh list as needed
     * @param aBufferStart  the starting buffer index
     * @param aBufferEnd  the ending buffer index
     */
    void ProvideDataDifferenceL(TInt aBufferStart, TInt aBufferEnd);
    void ProvideDataWithoutThumbnailsL(const CMPXMediaArray& aMediaArray, TInt aStartIndex = 0);

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

    enum TDefaultIcon
        {
        EMPXDefaultIconNotSet = -1,
        EMPXDefaultIconArtist,
        EMPXDefaultIconAlbum,
        EMPXDefaultIconPlaylist,
        EMPXDefaultIconSongs,
        EMPXDefaultIconPodcasts,
        EMPXDefaultIconGenre,
        EMPXDefaultIconComposer,
        EMPXDefaultTitleIconPodcasts,
        EMPXDefaultIconEmpty
        };

    MMPXViewContainer*          iViewContainer;         // Not owned
    MEikCommandObserver*        iView;                  // Not owned
    MEikListBoxObserver*        iListBoxObserver;       // Not owned
    CMPXCommonListBoxArrayBase* iListBoxArray;          // Owned
    CArrayFixFlat<TInt>*        iListSelections;        // Owned
    CAknIconArray*              iIconArray;             // Owned

    CMPXCommonContainerTextureManager2*  iTextureManager;// Owned
	CMPXCommonUiHelper*         iCommonUiHelper;        // Owned

    CHgScroller* 				iListWidget;  		    // Owned
    CHgScroller* 				iPrevListWidget;  		    // Owned
    CHgScrollerWithTitle*       iScrollerWithTitle;
    CThumbnailManager*          iThumbnailManager;      // Owned

    TContext                    iContext;
    TContext                    iPrevContext;
    TInt                        iResourceOffset;        // Must be freed
    TBool                       iIsMarkingEnabled;
	TInt						iTopIndex;
	TInt						iBottomIndex;
	TSize						iImageSize;
    CAknsBasicBackgroundControlContext* iBgContext; 	// Owned

    typedef RHashMap<TInt, TInt> RThumbnailReqHashMap;
    RThumbnailReqHashMap iThumbnailReqHashMap;

    HBufC* iEpisodeTitle;
    HBufC* iAlbumTitle;
    HBufC* iSongTitle;

    TBool iPodcastContext;
    TBool iTitleSet;
    TDefaultIcon iCurrentDefaultIcon;
    TFileName iSelectedAlbumItemFileName;
    CMPXMedia* iSelectedMediaInAlbumView;
    // Queue for Albumart asynchronous requests
    TSglQue<TAlbumArtRequest>   iRequestQueue;
    TAlbumArtRequest*           iAlbumArtRequest;
    TInt                        iRequestCount;
    TInt  iQueueCounter;
    
    THgScrollDirection iDirection;
    RArray<TBool>  iThumbnailReqMap;
    TInt  iLastValidMediaItemIndex;
    TInt iRequestStart;
    TInt iRequestEnd;

    };

#endif  // CMPXCOMMONCONTAINERHGIMP_H

// End of File
