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


#ifndef MPXMETADATAEDITORDIALOG_H
#define MPXMETADATAEDITORDIALOG_H

// INCLUDES
#include <AknForm.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionuihelperobserver.h>
#include <mpxattribute.h>
#include <AknQueryDialog.h>
#include <DRMHelper.h>

// CONSTANTS
const TInt KMPXMetadataTextFieldMaxLen = 255;

// FORWARD DECLARATIONS
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknQueryValueText;
class CAknQueryValueTextArray;
class MMPXCollectionUtility;
class CMPXCommonUiHelper;
class CMPXMedia;
class MMPXCollectionUiHelper;
class CAknPopupField;
class CIdle;

// CLASS DECLARATION
/*
* Provides query dialog for changing track's library.
*/
NONSHARABLE_CLASS( CMPXQueryDialog ) : public CAknQueryDialog
    {
public:
    /**
     * Two-phased constructor.
     */
    static CMPXQueryDialog* NewL();

     /**
     * From CAknDialog
     * Handle key events. When a key event occurs,
     * until one of them returns EKeyWasConsumed to indicate that it processed the key event.
     * CONE calls this function for each control on the control stack,
     * @param aKeyEvent  The key event.
     * @param aType      The type of the event: EEventKey, EEventKeyUp or EEventKeyDown.
     * @return           Indicates whether or not the key event was used by this control.
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    };

/*
* Provides factory to create concrete document object.
*/
NONSHARABLE_CLASS( CMPXMetadataEditorDialog ) : public CAknForm,
                                                public MMPXCollectionObserver,
                                                public MMPXCollectionFindObserver,
                                                public MMPXCHelperObserver
    {
public:
    // File Details headings
    enum TMPXSongsFileDetailsHeadings
        {
        EMPXSongsFileDetailsHeadingFilename = 0, // Start from index 0
        EMPXSongsFileDetailsHeadingFormat,
        EMPXSongsFileDetailsHeadingDuration,
        EMPXSongsFileDetailsHeadingBitrate,
        EMPXSongsFileDetailsHeadingSamplingRate,
        EMPXSongsFileDetailsHeadingSize,
        EMPXSongsFileDetailsHeadingModified,
        EMPXSongsFileDetailsHeadingCopyright,
        EMPXSongsFileDetailsHeadingUrl,
        EMPXSongsFileDetailsHeadingCount
        };

    enum TMPXPodcastsFileDetailsHeadings
        {
        EMPXPodcastsFileDetailsHeadingFilename = 0, // Start from index 0
        EMPXPodcastsFileDetailsHeadingFormat,
        EMPXPodcastsFileDetailsHeadingDuration,
        EMPXPodcastsFileDetailsHeadingBitrate,
        EMPXPodcastsFileDetailsHeadingSamplingRate,
        EMPXPodcastsFileDetailsHeadingSize,
        EMPXPodcastsFileDetailsHeadingLastPlaybackPosition,
        EMPXPodcastsFileDetailsHeadingPublished,
        EMPXPodcastsFileDetailsHeadingModified,
        EMPXPodcastsFileDetailsHeadingCopyright,
        EMPXPodcastsFileDetailsHeadingUrl,
        EMPXPodcastsFileDetailsHeadingCount
        };

public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CMPXMetadataEditorDialog* NewL();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CMPXMetadataEditorDialog();

    /**
     * Parameter passed in from viewframework
     *
     * @param aParam Parameter pass in from viewframework.
     */
    IMPORT_C void SetParamL( const TDesC* aParam );

    /**
     * From CAknDialog update member variables of CAknExEditorDialog.
     * @param aButtonId The ID of the button that was activated.
     * @return Should return ETrue if the dialog should exit,
     *    and EFalse if it should not
     */
    TBool OkToExitL(TInt aButtonId);

    /**
     * From CAknDialog.
     *
     * @param aResourceId The resource ID of the dialog to load.
     * @return Zero, unless it is a waiting dialog. For a waiting dialog,
     *         the return value is the ID of the button that closed the
     *         dialog, or zero if it was the cancel button
     *         (@c EEikBidCancel).
     */
    TInt ExecuteLD( TInt aResourceId );

    /**
     * Display file details popup window
     */
    void ViewFileDetailsPopupL();

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
     * @param aPlaylist collection playlist
     * @param aError error code
     */
    void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError );

    /**
     * Handle media properties
     *
     * @param aMedia media
     * @param aError error code
     */
    void HandleCollectionMediaL(
        const CMPXMedia& aMedia,
        TInt aError);

private:  // Constructor
    /**
     * Default constructor.
     */
    CMPXMetadataEditorDialog();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

private: // New functions

    /**
     * Clean up function for genre related members.
     */
    void CleanUpGenreText();

    /**
     * Populates genre popup with genre list for tracks.
     */
    void FetchGenreL();

    /**
     *  Handle media properties.
     *
     *  @param aMedia media properties
     *  @param aError error code
     */
    void DoHandleMediaL(
        const CMPXMedia& aMedia,
        TInt aError );

    /**
     * Saves media back to collection engine
     *
     * @param aMedia media property to save back
     */
    void SaveMediaPropertiesL( CMPXMedia* aMedia );

    /**
     * Appends the value to the array, if the vaule has zero length,
     * appends "unavailable"
     *
     * @param aArray array to append to
     * @param aValue value to append
     */
    void AppendStringToArrayL( CDesCArrayFlat* aArray, const TDesC& aValue );

    /**
     * Sets control text
     *
     * @param aControlId The ID of the control
     * @param aValue value to set
     * @param aAlternateValue if aValue is zero length,
     *                        this value will be used instead
     *
     */
    void SetControlTextL( TInt aControlId, const TDesC& aValue,
        const TDesC& aAlternateValue );

    /**
     * Sets control text
     *
     * @param aControlId The ID of the control
     * @param aValue value to set
     * @param aMinValue minimum value allowed for this control
     * @param aMaxValue maximum value allowed for this control
     *
     */
    void SetControlNumberL( TInt aControlId, const TDesC& aValue,
        TInt aMinValue, TInt aMaxValue );

    /**
     * prompt user for library change
     *
     * @param aMoveToLibrary enum for library to move to
     * @return ETrue if the user confirms the change, EFalse if cancel
     */
    TBool PromptForLibraryChangeL( TInt aMoveToLibrary );

    /**
     * Updates media object with current control text, this will also
     * update iMedia
     *
     * @param aControlId the id of the control
     * @param aMedia media object to update
     * @param aAttribute attribute to save to
     *
     * @return EFalse if the control being checked has not been changed (equal)
     *         ETrue if the control being checked has been changed
     */
    TBool UpdateMediaObjectWithControlL(
        TInt aControlId, CMPXMedia* aMedia, TMPXAttribute aAttribute );

    static TInt DeferredExitCallback( TAny* aPtr );

    static TInt DeferredFileDetailsCallback( TAny* aPtr );

#ifdef __ENABLE_MSK
    /**
     * Update softkey.
     */
    void UpdateSoftkeyL();
#endif // __ENABLE_MSK

private:  // Functions from base class

#ifdef __ENABLE_MSK
    /**
     * From CAknForm, handles key-events.
     * @param aKeyEvent Key event.
     * @param aType Type of key event(EEventKey, EEventKeyUp or
     *              EEventKeyDown).
     * @return If key-event is consumed, EKeyWasConsumed. Else
     *         EKeyWasNotConsumed.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );
#endif // __ENABLE_MSK

    /**
     * From CAknForm
     * Save the contents of the form.
     */
    TBool SaveFormDataL();

    /**
     * From CAknForm
     * Does not save the contents of the form.
     */
    void DoNotSaveFormDataL();

    /**
     * Store the old title pane text and set the new one
     */
    void SetTitlePaneL();

    /**
     * Restore the old title pane text
     */
    void RestoreTitlePaneL();

    /**
     * Store the old title navi label and set the new one
     */
    void SetNaviLabelL();

    /**
     * Restore the old navi label text
     */
    void RestoreNaviLabelL();

    /**
     * From MEikMenuObserver.
     * Dynamically initialises a menu pane. This function can be overridden
     * by the derived class for its own purpose.
     * @param aResourceId Resource ID identifying the menu pane to initialise.
     * @param aMenuPane The in-memory representation of the menu pane.
     */
    void DynInitMenuPaneL( TInt aResourceId,
                           CEikMenuPane* aMenuPane );

    /**
     * Act on the menu selection if menu is showing
     */
    void ProcessCommandL(TInt aCommandId);

    /**
     * Populates file details window for songs
     * @param aHeadingsArray array of the headers
     * @param aDataArray array to populate
     */
    void PopulateFileDetailsL(
        CDesCArrayFlat* aHeadingsArray, CDesCArrayFlat* aDataArray );

    /**
     * Populates file details window for podcasts
     * @param aHeadingsArray array of the headers
     * @param aDataArray array to populate
     */
    void PopulatePodcastFileDetailsL(
        CDesCArrayFlat* aHeadingsArray, CDesCArrayFlat* aDataArray );

    /**
     * Constructs DRM items.
     * @param aHeadingsArray array of the headers
     * @param aDataArray array to populate
     */
    void MakeDrmItemsL(
        CDesCArrayFlat* aHeadingsArray, CDesCArrayFlat* aDataArray );

    /**
     * Constructs restricted drm items. E.g. count, interval
     * @param aRights Drm right type "Play"
     * @param aCurrentIndex current index of the listbox to insert into
     * @param aHeadingsArray array of the headers
     * @param aDataArray array to populate
     */
    void MakeRestrictedDrmItemsL(
        HBufC* aRights, TInt& aCurrentIndex,
        CDesCArrayFlat* aHeadingsArray, CDesCArrayFlat* aDataArray );
    
	
	/**
     * Populates file details window for songs
     */
    void PopulateFileDetailsL();


    /**
     * Populates file details window for podcasts
     */
    void PopulatePodcastFileDetailsL();

	
    /**
     * Add parts Of time.
     * @param aIntYrs year information
     * @param aIntMon month information
     * @param aIntDay day information
     * @param aIntHrs hour information
     * @param aIntMin minute information
     * @param aIntSec second information
     * @return  Buffer where to add the elements
     */
    HBufC* AddPartsOfTimeLC( TInt aIntYrs, TInt aIntMon, TInt aIntDay, 
                             TInt aIntHrs, TInt aIntMin, TInt aIntSec );
    
    /**
     * Add a single element of time to the string array
     * @param aNumOfElements         Number of elements of the interval
     * @param aResourceIdSingle      Resource id to be used in case of
     *                               single unit
     * @param aResourceIdOneFinal    Resource id to be used in case of
     *                               units ending with 1, from 21
     *                               (requested by Slavic languages)
     * @param aResourceIdTwoFour     Resource id to be used in case of
     *                               units included from two to four, except
     *                               12-14 (requested by Slavic languages)
     * @param aResourceIdFiveZero    Resource id to be used in case of
     *                               units ending from 5 to 9, plus range
     *                               from 11 to 14 (requested by Slavic
     *                               languages)
     * @param aStrings               DesCArrayFlat where to add the element
     */
    
    void AddSinglePartOfTimeL( TInt aNumOfElements,
            TInt aResourceIdSingle,
            TInt aResourceIdOneFinal, 
            TInt aResourceIdTwoFour,
            TInt aResourceIdFiveZero, 
            CDesCArrayFlat* aStrings );
     
     /**
     * Split TimeIntervalSeconds into  parts Of time.
     * @param aIntYrs year information
     * @param aIntMon month information
     * @param aIntDay day information
     * @param aIntHrs hour information
     * @param aIntMin minute information
     * @param aIntSec second information
     * @return  void
     */
    void SplitTime( const TTimeIntervalSeconds& aInterval,
            TInt& aIntYrs, TInt& aIntMon, TInt& aIntDay,
            TInt& aIntHrs, TInt& aIntMin, TInt& aIntSec );
    /**
     * Add a single element of time to the string array
     * @param aNumOfElements         Number of elements of the interval
     * @param aResourceIdSingle      Resource id to be used in case of
     *                               single unit
     * @param aResourceIdOneFinal    Resource id to be used in case of
     *                               units ending with 1, from 21
     *                               (requested by Slavic languages)
     * @param aResourceIdTwoFour     Resource id to be used in case of
     *                               units included from two to four, except
     *                               12-14 (requested by Slavic languages)
     * @param aResourceIdFiveZero    Resource id to be used in case of
     *                               units ending from 5 to 9, plus range
     *                               from 11 to 14 (requested by Slavic
     *                               languages)
     * @param aStrings               Buffer where to add the element
     */
    void AddSinglePartOfTimeL( TInt   aNumOfElements,
                               TInt   aResourceIdSingle,
                               TInt   aResourceIdOneFinal,
                               TInt   aResourceIdTwoFour,
                               TInt   aResourceIdFiveZero,
                               HBufC*& aStrings );

    /**
     * Converts between arabic-indic digits and european digits.
     * @param aText numbers to be converted.
     * @return void
     */
    void LanguageSpecificNumberConversion( TDes& aText ) const;

    /**
     * Converts the time value to localtime
     * @param aTime contains the UTC time to be converted, and the converted value on exit
     */
    void ConvertToLocalTimeL(TTime& aTime );

    /**
     * Adds header and value to list.
     * @param aHeading Heading
     * @param aValue Value
     * @param aItemArray Array where value and header are added.
     * @return void
     */
    void AddItemToListBoxL( const TDesC& aHeading,
                            const TDesC& aValue,
                            CDesCArray* aItemArray );

    /**
     * This is used to decide when to save changes. If a leave occurs the
     * framework generates a Symbian Leave code.
     * @param aControlId. Control id.
     */
    void HandleControlStateChangeL( TInt aControlId );

    /**
     * Function from @c CEikDialog and thence from c@ MEikDialogPageObserver.
     * Checks if either year or track control id's are empty, and if so set to 0.
     */
    void PrepareForFocusTransitionL();

    /**
     * Required for help.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

// from MMPXCollectionFindObserver

    /**
    *  Handle callback for "find" operation
    *  @param aEntries, CMPXMedia to be returned
    *  @param aComplete ETrue no more entries. EFalse more entries
    *                   expected
    *  @param aError error code
    */
    void HandleFindAllL( const CMPXMedia& aResults,
        TBool aComplete,TInt aError );

// from MMPXCHelperObserver

    /**
    *  Handle callback for the ui helper
    *  @param aOperation operation performed
    *  @param aErr error code
    *  @param aArgument argument returned
    */
    void HandleOperationCompleteL( TCHelperOperation aOperation,
                                   TInt aErr,
                                   void* aArgument );

private: // From CEikDialog
    /**
     * From CEikDialog, PreLayoutDynInitL.
     * Initialises the dialog's controls before the dialog is sized
     * and layed out.
     */
    void PreLayoutDynInitL();

    /**
     * From CEikDialog, PostLayoutDynInitL.
     * Initialises the dialog's controls after the dialog has been sized
     * but before it has been activated.
     */
    void PostLayoutDynInitL();

private:
    enum TMPXMetadataEditorCurrentMediaLOp
        {
        EMPXMetadataEditorIdle = 0,
        EMPXMetadataEditorGetSongInfo,
        EMPXMetadataEditorGetDrmInfo
        };

private:
    CAknTitlePane*                          iTitlePane;
    CAknNavigationControlContainer*         iNaviPane;
    CAknNavigationDecorator*                iNaviDecorator; // Navi decorator
    CAknNavigationDecorator*                iEmptyNaviDecorator; // Navi decorator
    CAknPopupField*                         iPopup;
    CAknQueryValueText*                     iTextValues;
    CAknQueryValueTextArray*                iValueTextArray;
    CDesCArray*                             iGenreArr;
    CAknQueryValueText*                     iLibraryTextValues;
    CAknQueryValueTextArray*                iLibraryValueTextArray;
    CDesCArray*                             iLibraryArr;

    HBufC*                                  iTitlePaneText;
    HBufC*                                  iParam;

    MMPXCollectionUiHelper*                 iCollectionUiHelper;
    MMPXCollectionUtility*                  iCollectionUtility;
    CMPXMedia*                              iMedia;  // own
    CMPXMedia*                              iDrmInfo;
    CMPXCommonUiHelper*                     iCommonUiHelper;    // own

    TInt                                    iResourceOffset; // must be freed
    TInt                                    iDrmResourceOffset; // must be freed
    TInt                                    iCurrentLibrary;
    TInt                                    iYear; // special handling needed for year
    TInt                                    iCurrentMediaLOp;
    TInt                                    iIsDrmProtected;

    TBool                                   iUnknownGenre;
    TBool                                   iDrmDetails; // Flags if DRMRightsManager.rsc was loaded
    TBool                                   iDisablePodcasting;
    CIdle*                                  iIdle;
    TBool                                   iTryingExit;
    };

#endif  // MPXMETADATAEDITORDIALOG_H

// End of File
