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
* Description:  Music Player album art editor dialog declaration
*
*/


#ifndef MPXALBUMARTEDITORDIALOG_H
#define MPXALBUMARTEDITORDIALOG_H

//  INCLUDES
#include <AknDialog.h>
#include <mpxcollectionobserver.h>
#include <mpxcommonframeworkdefs.h>
#include <AknProgressDialog.h>  // MProgressDialogCallback

#include "mpxalbumarteventobserver.h"

// FORWARD DECLARATIONS
class CMPXAlbumArtEditorDialogCustomControl;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class MMPXCollectionUtility;
class CMPXAlbumArtUtil;
class CMPXMedia;
class CMPXMediaArray;
class CMPXCommonUiHelper;

// CLASS DECLARATION

/**
 *  CMPXAlbumArtEditorDialog dialog class for Changing/Restoring
 *  Album Art
 */
NONSHARABLE_CLASS( CMPXAlbumArtEditorDialog ) :  public CAknDialog,
                                                 public MMPXCollectionObserver,
                                                 public MProgressDialogCallback,
                                                 public MMPXAlbumArtEventObserver,
                                                 public MMPXCollectionFindObserver
    {
public:  // Constructors and destructor

    /**
     * Symbian OS two-phased constructor
     * @return MP Album Art Dialog.
     */
    IMPORT_C static CMPXAlbumArtEditorDialog* NewL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CMPXAlbumArtEditorDialog();

    /**
     * Parameter passed in from viewframework
     *
     * @param aParam Parameter pass in from viewframework.
     */
    IMPORT_C void SetParamL( const TDesC* aParam );

private:

    /**
     * C++ default constructor.
     */
    CMPXAlbumArtEditorDialog();

    void ConstructL( TInt aResource );

private: // from base class MMPXCollectionObserver

    /**
     * Handle collection message.
     *
     * @param aMessage Collection message
     * @param aErr system error code.
     */
    void HandleCollectionMessage(
        CMPXMessage* aMessage, TInt aError );

    /**
     * Handles the collection entries being opened. Typically called
     * when client has Open()'d a folder
     *
     * @param aEntries collection entries opened
     * @param aIndex focused entry
     * @param aComplete ETrue no more entries. EFalse more entries
     *                  expected
     * @param aError error code
     */
    void HandleOpenL(
        const CMPXMedia& aEntries,
        TInt aIndex, TBool aComplete, TInt aError);

    /**
     * Handles the item being opened. Typically called
     * when client has Open()'d an item. Client typically responds by
     * 'playing' the item via the playlist
     *
     * @param aPlaylist collection playlist, owner ship is transfered
     * @param aError error code
     */
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

    /**
     * Handle collection media
     *
     * @param aMedia media
     * @param aError error code
     */
    void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError);

    /**
    * From MMPXCollectionObserver
    */
    void HandleCommandComplete(CMPXCommand* aCommandResult,
                               TInt aError);
private:    // New functions

    /**
     * Sends request to determine if it's album level
     * or track level
     */
    void GetCurrentModeL();

    /**
     * Backs up Status Pane.
     */
    void BackupPreviousStatusPaneL();

    /**
     * Restores Previous Status Pane.
     */
    void RestorePreviousStatusPaneL();

    /**
     * Find the components custom control.
     */
    CMPXAlbumArtEditorDialogCustomControl *GetAlbumArtEditorDialogCustomControl() const;

    /**
     * loads album art information from database
     * @param aShowWaitNote ETrue if the wait note is shown, EFalse otherwise
     */
    void LoadAlbumArtL( TBool aShowWaitNote );

    /**
     * extracts the album art
     *
     * @param aMedia media object containing album art info
     * @param aDismissWaitNoteOnError ETrue to dismiss wait note if error occurs,
     *                                EFalse otherwise
     */
    void ExtractAlbumArtL( const CMPXMedia& aMedia,
        TBool aDismissWaitNoteOnError = ETrue );

    /**
     * control extract of multiple album art
     */
    void ExtractMultipleAlbumArtL();

    /**
     * Saves the updated media
     *
     * @param aFileName the file name for the album art
     * @param aIsRestore ETrue if this is a restore operation,
     *                   EFalse if this is a change operation
     * @return NULL if media is broken, CMPXMedia* ownership if saving is required
     */
    CMPXMedia* SaveMediaLC( const TDesC& aFileName, TBool aIsRestore );

    /**
     * Saves multiple media
     *
     * @param aIsRestore ETrue if this is a restore operation,
     *                   EFalse if this is a change operation
     */
    void SaveMultipleMediaL( TBool aIsRestore );

    /**
     * Handle save operation complete
     *
     * @param aError Error code, if any
     */
    void DoHandleOperationCompleteL( TInt aError );

    /**
     * Handle collection message
     *
     * @param aMessage collection message
     */
    void DoHandleCollectionMessageL( const CMPXMessage& aMessage );

    /**
     * Updates a media to the collection
     * @param aMedia media to update
     * @param aSync synchronous operation or not
     */
    void DoSetCollectionL( CMPXMedia* aMedia, TBool aSync );

    /**
     * Appends the common attributes to attributes array
     *
     * @param aAryAttribute Array to append to
     */
    void AppendCommonAttributes( RArray<TMPXAttribute>& aAryAttribute );

private: // Functions from base classes

    /**
    * From MMPXAlbumArtEventObserver
    * Handle album art events
    *
    * @param aEvent album art event
    * @param aError error code
    */
    void HandleAlbumArtEventL( TMPXAlbumArtEvents aEvent, TInt aError );

    /**
    *  from MMPXCollectionFindObserver
    *  Handle callback for "find" operation
    *  @param aEntries, CMPXMedia to be returned
    *  @param aComplete ETrue no more entries. EFalse more entries
    *                   expected
    *  @param aError error code
    */
    void HandleFindAllL(const CMPXMedia& aResults,
                        TBool aComplete,TInt aError);

    /**
     * Process menu command
     * @param aCommandId command ID
     */
	void ProcessCommandL( TInt aCommandId ) ;

    /**
     * Creates custom control for the dialog
     * @param aControlType control type
     */
    SEikControlInfo CreateCustomControlL( TInt aControlType );

    /**
     * Initialises the dialog's controls before the dialog is sized and layed out.
     */
    void PreLayoutDynInitL();

    /**
     * This function ALWAYS returns ETrue.
     * @param aButtonId button ID
     */
    TBool OkToExitL( TInt aButtonId );

	/**
	 * Called when key events happen
	 */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                 TEventCode aType );

    /**
     * Dynamically initializes the menu pane.
     * @param aResourceId resource identifier.
     * @param aMenuPane menu pane
     */
    void DynInitMenuPaneL( TInt aResourceId,
                           CEikMenuPane* aMenuPane );

    /**
     * Processes menu command.
     * @param aCommandId menu command
     */
    void DoProcessCommandL( TInt aCommandId );

    /**
     * Returns the number of component controls.
     */
    TInt CountComponentControls() const;

    /**
     * Returns the component control.
     * @param aIndex index of the component
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * Updates the dialogs navipane.
     */
    void UpdateNaviPaneL(const TDesC& aText);

    /**
     * Required for help.
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
     * From MProgressDialogCallback
     * Callback method
     * Get's called when a dialog is dismissed.
     */
    void DialogDismissedL( TInt aButtonId );


    /**
     * Callback function
     * Get called by iAsyncCallNext.
     */
    static TInt ExtractNextAlbumArtCallBackL(TAny* aPtr);

    /**
     * Extract next Album Art
     * Get called by ExtractNextAlbumArtCallBackL.
     */
    void ExtractNextAlbumArtL();

private:    // Data

    HBufC*                                  iOrigTitle;             // Original title text
    HBufC*                                  iParam;
    HBufC*                                  iNewFileName;

    CAknTitlePane*                          iTitlePane;             // Not owned
    CAknNavigationControlContainer*         iNaviPane;              // Not owned
    CAknNavigationDecorator*                iNaviLabelPane;         // New navi label pane

    MMPXCollectionUtility*                  iCollectionUtility;  // not own
    CMPXAlbumArtUtil*                       iMPXUtility;   // own
    CMPXMedia*                              iMedia;
    CMPXMedia*                              iAlbumInfo;
    CMPXMediaArray*                         iMediaArray;
    CMPXCommonUiHelper*                     iCommonUiHelper;

    TRect                                   iModRect;
    TInt                                    iResourceOffset; // must be freed
    TInt                                    iCurrentMediaLOp;   // current mediaL operation
    TInt                                    iCurrentFindAllLOp;   // current FindAllL operation
    TInt                                    iAlbumArtMode;
    TInt                                    iCurrentAlbumArtIndex;

    TBool                                   iAlbumArtChanged;
    TBool                                   iIsSaving;
    TBool                                   iInvalidFileExist;
    TBool                                   iOpCanceled;
    CAsyncCallBack*                         iAsyncCallNext;
    TBool                                   iFetchingPicture; // To indicate if picture fetching is ongoing
    };

#endif      // MPXALBUMARTEDITORDIALOG_H

// End of File
