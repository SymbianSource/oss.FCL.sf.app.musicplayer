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
* Description:  MPX Collection view container implementation
*
*/



#ifndef CMPXCOLLECTIONVIEWHGCONTAINER_H
#define CMPXCOLLECTIONVIEWHGCONTAINER_H


// INCLUDES
#include <coecntrl.h>

#include <ganes/HgScrollBufferObserverIface.h>
#include <ganes/HgSelectionObserverIface.h>
#include <ganes/HgVgMediaWallObserverIface.h>
#include <ganes/HgItem.h>
#include <mpxcommonframeworkdefs.h> // TMPXItemId
#include "mpxcollectionviewhgtnloader.h"
#include "mpxcommonlistboxarrayobserver.h"
#include "mpxcommonlistboxarraybase.h"

// FORWARD DECLARATION
class CAknContextPane;
class CAknIconArray;
class CAknsBasicBackgroundControlContext;
class CEikMenuPane;
class CHgScroller;
class CHgScroller;
class CHgVgMediaWall;
class CMPXCollectionViewHgPlaylistHelper;
class CMPXCommonUiHelper;
class CMPXMediaArray;
class MCoeControlObserver;
class MEikCommandObserver;
class MEikListBoxObserver;
class MMPXCollectionUiHelper;
class MMPXCollectionUtility;
class MMPXPlaybackUtility;
class MMpxCbaHandler;
class TAknsItemID;
class CAsyncOneShot;
class MMPXSelectedAlbumHandler;

// CLASS DECLARATION

/**
 *  MPX Collection view container.
 *
 *  @lib mpxcollectionviewhg.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMPXCollectionViewHgContainer )
    : public CCoeControl
    , public MCoeControlObserver
    , public MMPXCommonListBoxArrayObserver
	, public MHgScrollBufferObserver
	, public MHgSelectionObserver
    , public MMPXCollectionFindObserver
    , public MHgVgMediaWallObserver
    , public MMpxTNLoaderObserver
    , public MCoeForegroundObserver
	{
public:

    /**
     * Two-phased constructor.
     *
     * @since 5.1
     * @param aView Command observer.
     * @param aObserver MEikListBoxObserver
     * @param aIsEmbedded ETrue to indicate that view is embedded
     * @return Pointer to newly created object.
     */
    static CMPXCollectionViewHgContainer* NewL(
        MEikCommandObserver* aView,
        MEikListBoxObserver* aObserver,
        MMPXSelectedAlbumHandler* aSelectedAlbumHandler,
        TBool aIsEmbedded );

    /**
     * Destructor.
     */
    virtual ~CMPXCollectionViewHgContainer();

    /**
     * Sets playback status with item id
     *
     * @param aId item id of the song
     * @param aStatus status of the song
     */
    void SetPlaybackStatusByIdL( TMPXItemId aId, TMPXPlaybackState aStatus, TInt aIndex = 0 );

    /**
     * Get current playback index
     *
     * @return Current playback index, KErrNotFound if nothing is playing
     */
    TInt PlaybackIndex();

    /**
     * Activates reorder mode
     *
     * @param aEnable ETrue to enable reorder mode, EFalse to disable
     */
    void ActivateReorderMode( TBool aEnable );

    /**
     * Set reorder mode
     *
     * @param aEnable ETrue to enable reorder mode, EFalse to disable reorder mode
     * @param aIndex current selected index
     */
    void SetReorderGrabbedMode( TBool aEnable, TInt aIndex = 0 );

    /**
     * Determines if it's currently in reorder mode
     *
     * @return ETrue if in reorder mode, EFalse otherwise
     */
    TBool IsInReorderMode();

    /**
     * Move current reorder index relatively
     *
     * @param aDirection +1 to move down, -1 to move up
     */
    void MoveReorderIndex( TInt aDirection );

    /**
     * Set current reorder index
     *
     * @param aIndex current reorder index
     */
    void SetReorderIndex( TInt aIndex );

    /**
     * Confirm reorder
     */
    void ConfirmReorderL();

    /**
     * Cancel reorder
     */
    void CancelReorder();

    /**
     * Gets original index of currently selected item in reorder mode
     */
    TInt GetOriginalIndex();

    /**
     * Update Titlepane icon to reorder image
     */
    void UpdateReorderTitleIconL();

    /**
     * Update titlepane to use original icon
     */
    void RestoreOriginalTitleIconL();

    /**
     * Content is ready
     */
    void ContentIsReady( TBool aReady );

    /**
     * Returns list box array, ownership not transfered
     *
     * @since 3.1
     * @return A pointer to the list box array
     */
    CMPXCommonListBoxArrayBase* ListBoxArray() const;

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
     * Gets first listbox item index on screen(relative to the original listbox).
     * @return first listbox item index on screen
     *
     * 
     */
    TInt FirstIndexOnScreen() const;

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
    
	/*
     * Mark the grabbed item for reordering the playlist.
     */
    void MarkGrabedItemL( TInt aIndex ); 

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


// from base class MMPXViewContainer

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
     * Handles the retrieval of the view container's help context.
     *
     * @param aContext The control's help context.
     */
    void HandleHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * Returns the indicators for the specified item within the view container
     *
     * @param aIndex specified array index
     * @return Indicator icon indices
     */
    RArray<TInt> IndicatorsL( TInt aIndex );

    void SetCbaHandler( MMpxCbaHandler* aCbaHandler );

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

    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    void HandleGainingForeground();
    
    void HandleLosingForeground();

// from base class MCoeControlObserver

    /**
    * From MCoeControlObserver, Acts upon changes in the hosted control's state.
    * This class's implementation is trivial and should be able to be safely
    * re-implemented in directly client-derived classes. For non-base setting
    * page classes, a call to the base class should be made.
    * @param aControl Control that caused the event. (not used in default implementation)
    * @param aEventType Type of the event.
    */
    void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);


    /**
     * From MMPXCommonListBoxArrayObserver.
     * Handle listbox array events
     *
     *  @param aEvent list box event
     */
    void HandleListBoxArrayEventL(
        MMPXCommonListBoxArrayObserver::TMPXCommonListBoxArrayEvents aEvent );


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

    void HandleSelectL( TInt aIndex, CCoeControl* aControl );
    /**
     * Handle opening.
     *
     * @param aIndex Opened item index.
     */
    void HandleOpenL( TInt aIndex );
    void HandleOpenL( TInt aIndex, CCoeControl* aControl );


    /**
     *  From MMPXCollectionFindObserver
     *  Handle callback for "find" operation
     *
     *  @param aEntries, CMPXMedia to be returned
     *  @param aComplete ETrue no more entries. EFalse more entries
     *                   expected
     *  @param aError error code
     */
    void HandleFindAllL( const CMPXMedia& aResults, TBool aComplete, TInt aError );

    void TNReadyL(TInt aError, CFbsBitmap* aBitmap, CFbsBitmap* aMask, TInt aIndex);

    /**
     * Determines if current selected list item is a song
     *
     * @return ETrue if is a song, EFalse otherwise
     */
	TBool IsSelectedItemASong();
	
	void HandleItemCommandL( TInt aCommand );
	
	CMPXMedia* SelectedItemMediaL();
	
	/**
     * Determines if current view is TBoneview
     *
     * @return ETrue if current view is TBoneview, EFalse otherwise
     */
	TBool IsTBoneView();

	TBool OfferCommandL(TInt aCommand);

	/**
	 * Records begin state for the transition animation.
	 */
	void BeginFullScreenAnimation();

	/**
	 * Set the previous view Uid
	 *
	 * @param aViewUid UId of the previous view.
	 */
	void SetPreviousViewId(TUid aViewUid);

	/**
	 * Hides container window controls.
	 */
    void HideContainerWindow(); 
	
private:

    /**
     * C++ constructor.
     */
    CMPXCollectionViewHgContainer(
        MEikCommandObserver* aView,
        MEikListBoxObserver* aObserver,
        MMPXSelectedAlbumHandler* aSelectedAlbumHandler,
        TBool aIsEmbedded);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Creates listbox array
     */
    CMPXCommonListBoxArrayBase* CreateListBoxArrayL();

    /**
     * From MHgVgMediaWallObserver
     */
    void HandleMediaWallEvent(TInt aEvent, CHgVgMediaWall* aSender);

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
    void SetSongTitleL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailCountL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailArtistL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailAlbumL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void SetDetailDurationL( CHgItem* aVisualItem, CMPXMedia* aMedia );
    void UpdateTimeIndicatorsL( TDes& aBuf, TInt aDuration );
    void SetDetailIndicatorL( CHgItem* aVisualItem, TInt aIndex );
    void SetDetailIconL( CHgItem* aVisualItem, TInt aIndex );
    void SetDetailThumbnailL( CMPXMedia* aMedia, TInt aIndex = 0 );
	CGulIcon* DefaultIconL();
	void SetDetailIconShuffleL();
	TBool SetShuffleItemTextL(CHgItem* aItem, CMPXMedia* aMedia);
	void SetShuffleItemToListL( CHgScroller* aList );

    /**
     * Set the collection context
     */
    void SetCollectionContextL();

    void ProvideDataL( TInt aStart, TInt aEnd );

    void SetScrollbarType();

    void CancelTNRequest();

    /**
      * Save the selected album item
      *
      */
    void SaveSelectedAlbumItemL(TInt aIndex);

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

	void RestoreSelectedAlbumItemL(const CMPXMediaArray& aMediaArray);

    /**
     * Refresh list as needed
     */
    void RefreshL(TInt aIndex);

    /**
     * Refresh list as needed
     */
    void RefreshNoThumbnailL(TInt aIndex);

	/**
	* Clean AlbumArt request Queue
	*/
    void CleanAlbumArtReqQueue();

    /**
     * Handle data providing in requests in visible area
     * @param aBufferStart  the starting buffer index
     * @param aBufferEnd  the ending buffer index
     */
    void ProvideDataIntersectL(TInt aBufferStart, TInt aBufferEnd);

    /**
     * Handle data providing in requests in non visible area
     * @param aBufferStart  the starting buffer index
     * @param aBufferEnd  the ending buffer index
     */
    void ProvideDataDifferenceL(TInt aBufferStart, TInt aBufferEnd);

    /**
     * Provide data without providing thumbnails to list
     * @param aMediaArray  the media array to pull data from
     * @param aStartIndex  the starting index index
     */
    void ProvideDataWithoutThumbnailsL(const CMPXMediaArray& aMediaArray, TInt aStartIndex = 0);

    /**
     * Provide data without providing thumbnails to media wall
     * @param aMediaArray  the media array to pull data from
     * @param aStartIndex  the starting index index
     */
    void ProvideDataWithoutThumbnailsMwL(const CMPXMediaArray& aMediaArray, TInt aStartIndex = 0);

    /**
     * Handle data providing in specified range.
     * @param aBufferStart  the starting buffer index
     * @param aBufferEnd  the ending buffer index
     */
    void ProvideDataForRangeL( TInt aBufferStart, TInt aBufferEnd );

    /**
     * Prepare media wall to be shown
     * @param aMediaArray  the media array to pull data from
     * @param aCount  Number of items in media wall
     */
	void PrepareMediaWallL(const CMPXMediaArray& aMediaArray, TInt aCount);

    /**
     * Prepare TBone view to be shown
     * @param aMediaArray  the media array to pull data from
     * @param aCount  Number of items in media wall portion of tbone.
     */
	void PrepareMediaWallWithListL(const CMPXMediaArray& aMediaArray, TInt aCount);

    /**
     * Prepare list to be shown
     * @param aMediaArray  the media array to pull data from
     * @param aCount  Number of items in list
     */
	void PrepareListL(const CMPXMediaArray& aMediaArray, TInt aCount);

    /**
     * Handle list resizing
     * @param aMediaArray  the media array to pull data from
     * @param aCount  Number of items in list
     */
	void ResizeListL(const CMPXMediaArray& aMediaArray, TInt aCount);

    /**
     * Get the current list instance
     * @param aIndex Index from list control
     * @return Instance of scroller object
     */
	CHgScroller* CurrentListWidget();

    /**
     * Resolve the type of view based on current browsing context
     * @param aCount Count from media array
     */
	void ResolveCurrentViewType(TInt aCount);

    /**
     * Clean up the previous view's resources.
     */
	void CleanPrevView();

    /**
     * Initiate shuffle play all operation
     * @return ETrue if shuffle operation is started.
     */
	TBool ShufflePlayAllL(TInt aIndex);

    /**
     * Update the collection path and open it to start playback.
     * @param aIndex The selected media array index to start playing.
     * @param aSelectAll Indicate if all items should be selected.
     */
	void UpdatePathAndOpenL(TInt aIndex, TBool aSelectAll = EFalse);

    /**
     * Construct a collection path with the current selection and
     * construct a playlist from path to start playback.
     * @param aResults The media results from find operation.
     */
	void UpdatePathAndOpenPlaylistL( const CMPXMedia& aResults );

    /**
     * Find songs belonging to album.
     * @param aIndex Index in media array of albums
     */
	void FindAlbumSongsL(TInt aIndex);

    /**
     * Find songs belonging to playlist.
     * @param aIndex Index in media array of playlists
     */
	void FindPlaylistSongsL(TInt aIndex);

    /**
     * Find songs belonging to genre.
     * @param aIndex Index in media array of genres
     */
	void FindGenreSongsL(TInt aIndex);

    /**
     * Check if the currently playing track is in path
     * @param aPath Collection path of playlist
     * @return ETrue if current track is playing
     */
	TBool IsPlayingCurrentIndexL(CMPXCollectionPath* aPath);

    /**
     * Show list of songs in an album in a dialog popup.
     * @param aAlbum Album container with list of songs
     */
	void ShowAlbumSongsDialogL( const CMPXMedia& aAlbum );

    /**
     * Show list of songs in an album
     * @param aAlbum Album container with list of songs
     */
    void ShowAlbumSongsL( const CMPXMedia& aAlbum );

    /**
     * Open the album to show its contents
     * @param aIndex Index of the album in media array
     */
    void OpenAlbumL(TInt aIndex);

    /**
     * Play the album
     * @param aIndex Index of the album in media array
     */
    void PlayAlbumL(TInt aIndex);

    /**
     * Play the playlist
     * @param aIndex Index of the album in media array
     */
    void PlayPlaylistL(TInt aIndex);

    /**
     * Play the genre
     * @param aIndex Index of the album in media array
     */
    void PlayGenreL(TInt aIndex);

    static TInt AsyncCallback( TAny* aPtr );

    /**
     * Prepare to show the TBone view.
     */
	void PrepareTboneViewL();

    /**
     * Check if shuffle item is present in the view.
     * @return ETrue if shuffle item is present.
     */
	TBool ShuffleItemPresent();

    /**
     * Mapping function to convert from list index to media index.
     * @param aIndex Index from list control
     * @return media array index
     */
	TInt MediaIndex(TInt aIndex) const;

	/**
     * Load and set empty text.
     */
	void LoadAndSetEmptyTextL();

    /**
     * End fullscreen animation effects.
     */
    void EndFullScreenAnimation();

    /**
     * Resolve popup list rectangle size.
     */
    void ResolvePopupListSizeL();

    /**
     * Setup transition type
	 * @param aType Type of transition to be used.
	 * @param aPrevViewWasPlayback ETrue if previous view was playback.
	 *     This information used to decide animation direction.
	 */
    void SetupTransitionType( TInt aType, TBool aPrevViewWasPlayback = EFalse );

private: // data member

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

    enum TViewType
        {
        EMPXViewUnknown,
        EMPXViewMediawall,
        EMPXViewList,
        EMPXViewTBone
        };

    enum TFindOp
        {
        EMPXNoOp,
        EMPXOpenAlbum,
        EMPXPlayAlbum,
        EMPXPlayPlaylist,
        EMPXPlayGenre
        };

    MEikCommandObserver*            iView; // not owned
    MEikListBoxObserver*            iListBoxObserver; // not owned
    CAknContextPane*                iContextPane;    // Not owned
    CMPXCollectionViewListBoxArray* iListBoxArray; // owned
    CEikImage*                      iNewIcon;  // New context icon, owned
    CEikImage*                      iOrigIcon;  // New context icon, not owned
    TBool                           iReorder;
    TBool                           iReady;
    TBool                           iIsEmbedded;

    CAknPopupList* iDialog; // Album tracks dielog. owned.
    
    THgScrollDirection iDirection;
    RArray<TBool> iThumbnailReqMap;

    HBufC* iAlbumTitle;
    HBufC* iSongTitle;

    TBool iPodcastContext;
    TBool iTitleSet;
    TDefaultIcon iCurrentDefaultIcon;

    TFileName iSelectedAlbumItemFileName;
    CMPXMedia* iSelectedMediaInAlbumView;

	CMPXCommonUiHelper*         iCommonUiHelper;        // Owned
    MMPXCollectionUtility* 		iCollectionUtility;		// Owned
    MMPXPlaybackUtility* 		iPlaybackUtility;       // Owned

    CHgScroller* 				iMwListWidget;  		// Owned
    CHgScroller* 				iListWidget;  		    // Owned
    CHgVgMediaWall* 			iMediaWall;             // Owned
    CMpxTNLoader*               iThumbnailManager;      // Owned

    TContext                    iContext;
    TContext                    iPrevContext;
    TInt                        iResourceOffset;        // Must be freed
    TBool                       iIsMarkingEnabled;
	TInt						iTopIndex;
	TInt						iBottomIndex;
	TSize						iImageSize;
    CAknsBasicBackgroundControlContext* iBgContext; 	// Owned

    CArrayFixFlat<TInt>*        iListSelections;        // Owned
    CAknIconArray*              iIconArray;             // Owned

    MMPXCollectionUiHelper* iCollectionUiHelper;
    CMPXCollectionPlaylist* iPlaylist;
    CMPXCollectionViewHgPlaylistHelper* iPlaylistHelper;

    // List/Media wall management
    TInt iLastValidMediaItemIndex;
    TInt iRequestStart;
    TInt iRequestEnd;
    TInt iAlbumIndex; // opened album index in mediawall
    TInt iSelectedAlbumIndex; // selected album index in mediawall
    TInt iRestoredAlbumIndex; // restored selected album index
    MMPXSelectedAlbumHandler* iSelectedAlbumHandler;
    TBool iDefaultIconSet;
    MMpxCbaHandler* iCbaHandler;
    TViewType iCurrentViewType;
    TViewType iPrevViewType;
    TFindOp iFindOp;
    TBool iIsForeground;
    TBool iDelayedControlCreation;
    CAsyncCallBack* iAsyncCallBack;
    TBool iOpenAlbumTracks;
    TInt iShuffleItem;
    TBool iSetEmptyTextNeeded;
    TTransitionType iTranstionType;
    TBool iLayoutSwitch;
    TRect iPopupListRect;
    TUid iPreviousViewId;
    TBool iPreviousOrientation;
    TBool iDrawBackGround;
    TBool iIsNowPlayingView;
    };

#endif  // CMPXCOLLECTIONVIEWHGCONTAINER_H

// End of File
