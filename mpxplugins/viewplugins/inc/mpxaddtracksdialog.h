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
* Description:  Class declarations of CMPXAddTracksDialog
*
*/


#ifndef C_CMPXADDTRACKSDIALOG_H
#define C_CMPXADDTRACKSDIALOG_H

// INCLUDES
#include <aknselectionlist.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionpath.h> // TMPXItemId
#include <mpxcollectionuihelper.h>
#include <mpxcollectionuihelperobserver.h>

// FORWARD DECLARATIONS
class CEikTextListBox;
class CAknTitlePane;
class CAknContextPane;
class CEikImage;
class CMPXLbxExtendedFeatures;
class CMPXAddTracksListModel;
class MMPXCollectionUtility;
class CMPXMediaArray;
class CMPXCommonUiHelper;

// CLASS DECLARATION

/**
 *  CMPXAddTracksDialog class
 *
 *  Dialog class for Playlist Add Tracks Mode
 */
NONSHARABLE_CLASS( CMPXAddTracksDialog ) : public CAknSelectionListDialog,
                                           public MMPXCollectionObserver,
                                           public MMPXCollectionFindObserver,
                                           public MMPXCHelperObserver
    {
public:

    enum TMPXAddTracksListItems
        {
        EMPXAddTracksAddAllSongs = 0,
        EMPXAddTracksAddArtistsBegin
        };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @return A pointer to a newly created object
     */
    IMPORT_C static CMPXAddTracksDialog* NewL();

    /**
     * Two-phased constructor.
     * @return A pointer to a newly created object
     */
    IMPORT_C static CMPXAddTracksDialog* NewLC();

    /**
     * C++ default destructor.
     */
    IMPORT_C virtual ~CMPXAddTracksDialog();

private:    // Constructors

    /**
     * C++ default constructor.
     */
    CMPXAddTracksDialog();

    /*
     *  Two phased constructor
     */
    void ConstructL( TInt aResource );

public: // From CAknSelectionListDialog

	void ProcessCommandL( TInt aCommandId );

private:     // New functions

    /**
     * Backup previous status pane to internal members.
     */
    void BackupPreviousStatusPaneL();

    /**
     * Restore previous status pane from internal members.
     */
    void RestorePreviousStatusPaneL();

    /**
     * Create context icon for add tracks dialog.
     */
    void CreateContextIconL();

    /**
     * Update softkey.
     */
    void UpdateSoftkeyL();

    /**
     * Update listbox content by category.
     *
     * @aMedia media array containing the categories
     */
    void UpdateListboxByCategoryL( const CMPXMedia& aMedia );

	/**
     * Update listbox content by category after find
     * @param aFind if find is activated
     * @param aSongNum found song number
     */
    void UpdateListboxByCategoryforFindL( TBool aFind, TInt aSongNum );

    /**
     * Gets current listbox item index.
     * @return Current listbox item index
     */
    TInt CurrentLbxItemIndex() const;

    /**
     * Return number of listbox items shown in the current listbox.
     * @return Visible items in the list box
     */
    TInt CurrentListItemCount() const;

    /**
     * When you change the list item array you should call this method.
     */
    void HandleItemArrayChangeL();

    /**
     * Highlight list item.
     * @param aIndex The index to be highlighted
     */
    void HighlightListItem( TInt aIndex );

    /**
     * Expand current group.
     */
    void ExpandCurrentGroupL();

    /**
     * Goto the closest group.
     */
    void GotoClosestGroupL();

    /**
     * Collapse current group.
     */
    void CollapseCurrentGroupL();

    /**
     * Add current track to playlist.
     */
    void AddCurrentTrackToPlaylistL();

    /**
     * Find support in artist category and all songs category.
     */
    void DoFindL( void );

    /**
     * Animation execution
     */
    void DoAnimL( void );

    /**
     * Animation call back method
     * @param aObject call back method
     */
    static TInt AnimExpireL( TAny* aObject );

    /**
     * Stop the Animation execution
     * @param aHighlightCurrent EFalse if no highlight is required to the selected item, by default highlighted.
     */
    void StopAnimL( TBool aHighlightCurrent = ETrue );

    /**
     * load resources.
     */
    void LoadResourceL();

    /**
     * Start find categories
     */
    void LoadCategoriesL();

    /**
     * Handle collection message
     *
     * @param aMessage collection message
     */
    void DoHandleCollectionMessageL( const CMPXMessage& aMessage );
    
     /**
     * Function called by CIdle to close the dialog itself when collection view
     * can not handle the closing of dialog due to waitnote displayed by
     * CMPXCollectionViewImp::DoIncrementalOpenL
     * 
     * @param aPtr pointer to this dialog itself
     */
    static TInt DeferredExitCallback( TAny* aPtr );

private:    // Functions from base classes

    /**
     * From MMPXCollectionObserver
     * Handle collection message.
     *
     * @param aMessage Collection message
     * @param aErr system error code.
     */
    void HandleCollectionMessage(
        CMPXMessage* aMessage, TInt aError );

    /**
     *  From MMPXCollectionObserver
     *  Handles the collection entries being opened. Typically called
     *  when client has Open()'d a folder
     *
     *  @param aEntries collection entries opened
     *  @param aIndex focused entry
     *  @param aComplete ETrue no more entries. EFalse more entries
     *                   expected
     *  @param aError error code
     */
    void HandleOpenL(
        const CMPXMedia& aEntries,
        TInt aIndex,
        TBool aComplete,
        TInt aError );

    /**
     *  From MMPXCollectionObserver
     *  Handles the collection entries being opened. Typically called
     *  when client has Open()'d an item. Client typically responds by
     *  'playing' the item
     *
     *  @param aPlaylist collection path to item
     *  @param aError error code
     */
    void HandleOpenL(
        const CMPXCollectionPlaylist& aPlaylist,
        TInt aError );

    /**
     *
     *  From MMPXCollectionObserver
     *  Handle media properties
     *
     *  @param aMedia media
     *  @param aError error code
     */
    void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     *  From MMPXCollectionFindObserver
     *  Handle callback for "find" operation
     *
     *  @param aEntries, CMPXMedia to be returned
     *  @param aComplete ETrue no more entries. EFalse more entries
     *                   expected
     *  @param aError error code
     */
    void HandleFindAllL(
        const CMPXMedia& aResults, TBool aComplete, TInt aError );

    /**
     * From MMPXCHelperObserver
     * Handles the completion of any collection helper event.
     *
     * @param aOperation, operation completed
     * @param aErr. the error code
     * @param aArgument Argument returned from the operation
     */
    void HandleOperationCompleteL( TCHelperOperation aOperation,
                                   TInt aError,
                                   void* aArgument );

    /**
     * From CEikDialog
     * Prepare for layout.
     */
    void PreLayoutDynInitL();

    /**
     * From CEikDialog
     * Check if it's ok to exit.
     * @param aButtonId Softkey Id
     * @return ETrue if it's ok to exit, otherwise, EFalse.
     */
    TBool OkToExitL( TInt aButtonId );

    /**
     * From CCoeControl, handles key-events.
     * @param aKeyEvent Key event.
     * @param aType Type of key event(EEventKey, EEventKeyUp or
     *              EEventKeyDown).
     * @return If key-event is consumed, EKeyWasConsumed. Else
     *         EKeyWasNotConsumed.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * From CoeControl.
     * Handles a change to the control's resources.
     *
     * @param aType A message UID value.
     */
    void HandleResourceChange( TInt aType );

    /**
     * From MEikListBoxObserver
     * Handles listbox events.
     *
     * @param aListBox Listbox where the event occurred.
     * @param aEventType Event type.
     */
     void HandleListBoxEventL(
        CEikListBox* aListBox,
        TListBoxEvent aEventType );


private:    // Data

    MMPXCollectionUtility*                  iCollectionUtility;  // not own
    MMPXCollectionUiHelper*                 iCollectionUiHelper;
    CMPXCommonUiHelper*                     iCommonUiHelper;
    CMPXMediaArray*                         iCategoryArray;
    CMPXMediaArray*                         iAllSongsArray;

    CAknContextPane*                        iContextPane;    // Not owned
    CAknTitlePane*                          iTitlePane;      // Not owned
    CEikTextListBox*                        iListBox;        // Not owned
    CAknListBoxFilterItems*                 iLbxFilterItems; // Not owned

    CEikImage*                              iOrigIcon;       // Original context icon
    CEikImage*                              iNewIcon;        // New context icon
    CPeriodic*                              iPeriodic; 	 	    // Animation timer
    CMPXLbxExtendedFeatures*                iLbxExtFeat;     // Speed scrolling
    CMPXAddTracksListModel*                 iListModel;      // Tracks list model
    HBufC*                                  iOrigTitle;      // Original title text
    HBufC*                                  iLastFindTxt;    // Last find text
    HBufC*                                  iNoSongText;
    HBufC*                                  iNoMatchText;

    RArray<TInt>                            iMatchedSongArray;
    RArray<TInt>                            iMatchedGroupArray;
    TMPXItemId                              iPlaylistId;
    TInt                                    iAnimIconIndex;
    TInt                                    iSelectIndex;
    TInt                                    iResourceOffset;  // must be freed
    TInt                                    iCurrentOp;
    TInt                                    iCurrentGroupIndex;
    TInt                                    iLastIndex;      // Not used but required by base class
    TBool                                   iSongAllGroupExist;
    TBool                                   iExpandAllSongs;
    TBool                                   iDoFind;
        
    CIdle*                                  iIdle;  // used to close dialog itself
    };

#endif  // C_CMPXADDTRACKSDIALOG_H

// End of File
