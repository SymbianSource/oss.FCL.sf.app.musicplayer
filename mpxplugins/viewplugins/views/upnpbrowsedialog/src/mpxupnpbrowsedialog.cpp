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
* Description:  Implementation of UPnP players browse Dialog
*
*/



// INCLUDE FILES
#include <aknPopupHeadingPane.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>  // KDC_APP_RESOURCE_DIR
#include <bautils.h>  // BaflUtils
#include <AknsConstants.h>
#include <aknnotewrappers.h>

#include "mpxupnpbrowsedialog.h"
#include <mpxupnpbrowsedialog.rsg>
#include <mpxmessagegeneraldefs.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include "mpxplayermanagerstub.h"
#include "mpxlog.h"

#include <aknconsts.h>
#include <avkon.mbg>
#include <errorres.rsg>
#include <wlanerrorcodes.h>

#include <mpxmedia.h>
#include <mpxplaybackutility.h> // MMPXPlayerManager, MMPXPlaybackUtility
#include <mpxplaybackmessage.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxcommandgeneraldefs.h> // KMPXCommandGeneralDoSync
#include <mpxcollectionplaylist.h>
#include <mpxuser.h>

// CONSTANTS
_LIT( KMPlayerUPnPBrowserDialogRscPath, "mpxupnpbrowsedialog.rsc" );

_LIT( KUPnPBrowseListboxString, "%S\t%d\t" );
_LIT(KTab,"\t");
_LIT(KSpace, " ");

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
EXPORT_C CMPXUPnPBrowseDialog* CMPXUPnPBrowseDialog::NewL(TInt aPlayerUid)
    {
    CMPXUPnPBrowseDialog* self = new (ELeave) CMPXUPnPBrowseDialog(aPlayerUid);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::ConstructL( )
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::ConstructL" );

    // Get the playback utility instance from engine.
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );

    // Load resource file
    //
    TParse parse;
    parse.Set( KMPlayerUPnPBrowserDialogRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile( parse.FullName() );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(),
                                    resourceFile );
    iResourceOffset = CCoeEnv::Static()->AddResourceFileL( resourceFile );

    iViewUtility = MMPXViewUtility::UtilityL();
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::CMPXUPnPBrowseDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
CMPXUPnPBrowseDialog::CMPXUPnPBrowseDialog(TInt aPlayerUid) :
    iPlayerUid(aPlayerUid)
    {
    }

// Destructor
CMPXUPnPBrowseDialog::~CMPXUPnPBrowseDialog()
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::Destructor" );
    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
    delete iListBox;

    //Already cancelled in the destructor
    delete iPlayerManagerStub;

    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    delete iCurrentlySelectedSubPlayerName;

    if ( iViewUtility )
        {
        iViewUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// Executes the popup list
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMPXUPnPBrowseDialog::ExecuteLD(TInt aPlayerUid)
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::ExecuteLD()" );
    TInt errorSelectSubPlayer = KErrNone;
    TBool okToExit = EFalse;
    TInt selectedSubplayerIndex = KErrNotFound;
    iPlayerUnavailableError = EFalse;

    iPlayerUid = aPlayerUid;

    while ( !okToExit )
        {
        RetrieveCurrentlySelectSubplayerNameL();
    	CreatePopupL();
        TInt returnValue = StartPopupL();
        MPX_DEBUG2( "CMPXUPnPBrowseDialog::ExecuteLD() after StartPopupL() returnValue %d", returnValue );

        // if 1, Select was pressed
        if ( returnValue )
            {
            TInt currentItemIndex = iListBox->CurrentItemIndex();
            TInt numberOfItems = iListBox->Model()->NumberOfItems();

            // check for boundaries
            // if currentItemIndex is valid
            if ( ( currentItemIndex >= 0 ) &&
                 ( currentItemIndex < numberOfItems ) )
                {
                MPX_DEBUG2( "CMPXUPnPBrowseDialog::ExecuteLD() currentItemIndex %d", currentItemIndex );
                MPX_DEBUG2( "CMPXUPnPBrowseDialog::ExecuteLD() numberOfItems %d", numberOfItems );

                // if a subplayer was selected (and not <search again>)
                HBufC* listText =
                        StringLoader::LoadLC(R_UPNP_LIST_UPNP_MORE_SUBPLAYERS);
                TInt isSelectedItemASubplayerName =
                        listText->CompareF(
                             (iListBox->Model()->ItemText(currentItemIndex)) );
                MPX_DEBUG2( "CMPXUPnPBrowseDialog::ExecuteLD() isSelectedItemASubplayerName %d", isSelectedItemASubplayerName );
                CleanupStack::PopAndDestroy( listText );

                if ( isSelectedItemASubplayerName )
                    {
                    if ( iPlaybackUtility )
                        {
                        iPlaybackUtility->CancelRequest();
                        }

                    TBool selecDifferentSubPlayer = ETrue;

                    if ( iCurrentlySelectedSubPlayerName )
                        {
                        TFileName listName(iListBox->Model()->ItemText(currentItemIndex));
                        TInt index = listName.Find(KTab);
                        if ( index >= 0 )
                            {
                            TFileName subPlayerName = listName.LeftTPtr(index);
                            selecDifferentSubPlayer =
                              ( ( subPlayerName.CompareF(
                                  *iCurrentlySelectedSubPlayerName ) ) &&
                                  ( aPlayerUid == iCurrentlyUsedPlayer.iUid) );
                            }
                        else
                            {
                            selecDifferentSubPlayer =
                              ( ( listName.CompareF(
                                  *iCurrentlySelectedSubPlayerName ) ) &&
                                  ( aPlayerUid == iCurrentlyUsedPlayer.iUid) );
                            }
                        MPX_DEBUG2( "CMPXUPnPBrowseDialog::ExecuteLD() selecDifferentSubPlayer %d", selecDifferentSubPlayer);
                        }

                    MMPXSource* source = iPlaybackUtility->Source();
                    CMPXCollectionPlaylist* playlist = NULL;
                    if ( source )
                        {
                        playlist = source->PlaylistL();
                        if ( playlist )
                            {
                            CleanupStack::PushL( playlist );
                            }
                        }

                    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
                    // user selects a different remote player
                    if ( iCurrentlySelectedSubPlayerName &&
                         selecDifferentSubPlayer )
                        {
                        // need to clean up the current plugin
                        TRAP_IGNORE( manager.ClearSelectPlayersL() );
                        }

                    MPX_DEBUG3( "CMPXUPnPBrowseDialog::ExecuteLD() before call to manager.SelectSubPlayerL(%d, %d)", iPlayerUid, currentItemIndex );
                    MPX_TRAP( errorSelectSubPlayer,
                          manager.SelectSubPlayerL( TUid::Uid(iPlayerUid),
                                                    currentItemIndex ) );

                    MPX_DEBUG2( "CMPXUPnPBrowseDialog::ExecuteLD() after manager.SelectSubPlayerL()", errorSelectSubPlayer );
                    if ( errorSelectSubPlayer == KErrNone )
                        {
                        okToExit = ETrue;

                        if ( selecDifferentSubPlayer )
                            {
                            selectedSubplayerIndex = currentItemIndex;
                            }
                        else // subplayer selected is the same as the current one
                            {
                            selectedSubplayerIndex = KErrInUse;
                            }
                        }
                    else
                        {
                        TRAP_IGNORE( manager.ClearSelectPlayersL() );
                        if ( playlist )
                            {
                            iPlaybackUtility->InitL( *playlist, EFalse );
                            }

                        DisplayErrorNoteL( R_UPNP_ERROR_PLAYER_UNAVAILABLE );
                        }

                    if ( playlist )
                        {
                        CleanupStack::PopAndDestroy( playlist );
                        }
                    } // if ( isSelectedItemASubplayerName )
                } // check for boundaries
            else
                {
                okToExit = ETrue;
                selectedSubplayerIndex = KErrCancel;
                }
            } // if ( returnValue )
        else // Cancel was selected
            {
            if ( !iPlayerUnavailableError )
                {
                if ( iPlaybackUtility )
                    {
                    iPlaybackUtility->CancelRequest();
                    }

                CMPXCommand* cmd = CMPXCommand::NewL();
                CleanupStack::PushL(cmd);
                cmd->SetTObjectValueL<TInt>(KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral);
                // All of current commands are sync
                cmd->SetTObjectValueL<TBool>(KMPXCommandGeneralDoSync, ETrue);
                cmd->SetTObjectValueL<TInt>(KMPXCommandPlaybackGeneralType, EPbCmdUnloadNonActivePlugin);
                cmd->SetTObjectValueL<TInt>(KMPXCommandPlaybackGeneralData, aPlayerUid);
                iPlaybackUtility->CommandL(*cmd, NULL);
                CleanupStack::PopAndDestroy(cmd);

                okToExit = ETrue;
                selectedSubplayerIndex = KErrCancel;
                }
            // else we display the popup again
            else
                {
                iPlayerUnavailableError = EFalse;
                }
            }
        } // while ( !okToExit )

    return selectedSubplayerIndex;
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// ---------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::PreLayoutDynInitL()
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::PreLayoutDynInitL()" );

    // Browse dialog title text
    HBufC* dialogTitle = StringLoader::LoadLC(R_UPNP_MENU_TITLE_SELECT_PLAYER);
    iPopup->SetTitleL(*dialogTitle);
    CleanupStack::PopAndDestroy(dialogTitle);

    // Previous UPNP code

    CAknIconArray* icons = new ( ELeave ) CAknIconArray(2);
    CleanupStack::PushL(icons);

    // Mif icons
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName( ) );

    // Media server icon
    AppendIconToArrayL( icons, skin,
                        KAvkonBitmapFile,
                        KAknsIIDQgnIndiMarkedAdd,
                        EMbmAvkonQgn_indi_marked_add,
                        EMbmAvkonQgn_indi_marked_add_mask );
    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    CleanupStack::Pop(icons);

    // Previous UPNP code

    LoadListItemsL();
    }


// --------------------------------------------------------------------------
// CUPnPBrowsePopup::LoadListItemsL
// Sends the browse request to MPXPlayerManager. When result set arrives,
// Playback callback will call the "HandleSubPlayerNamesL" call back method,
// which is implemented below.
// --------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::LoadListItemsL()
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::LoadListItemsL()" );

    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();

    DisplayWaitingTextL();

    if ( iPlayerManagerStub )
        {
        iPlayerManagerStub->SubPlayerNamesL(*this, TUid::Uid(iPlayerUid));
        }
    else
        {
        MPX_DEBUG1( "CMPXUPnPBrowseDialog::LoadListItemsL() before call to manager.SubPlayerNamesL()" );
        manager.SubPlayerNamesL(*this, TUid::Uid(iPlayerUid));
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsePopup::DisplaySubPlayersNamesL
// --------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::DisplaySubPlayersNamesL( const MDesCArray* aSubPlayers,
                                                    const TBool aComplete )
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::DisplaySubPlayersNamesL()" );

    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>(textArray);
    TInt subPlayersCount = 0;

    listBoxItems->Reset();

    if ( aSubPlayers )
        {
        subPlayersCount = aSubPlayers->MdcaCount();
        MPX_DEBUG2( "CMPXUPnPBrowseDialog::DisplaySubPlayersNamesL(), aSubPlayer exists %d",
                    subPlayersCount );

        if ( subPlayersCount > 0 )
            {
            for (TInt i=0; i < subPlayersCount; i++)
                {
                TPtrC temp = aSubPlayers->MdcaPoint(i);
                TFileName subPlayerName;
                
                subPlayerName.Copy(temp);
                subPlayerName.TrimAll();        // trim all leading and trailing spaces (including tab)
                
                TInt tabPos = subPlayerName.Find(KTab);
                
                // replace tab with single space
                while (tabPos != KErrNotFound)
                    {
                    subPlayerName.Replace(tabPos, 1, KSpace);
                    tabPos = subPlayerName.Find(KTab);
                    }                
                
                // Append name of the subplayer in the listbox
                if ( iCurrentlySelectedSubPlayerName )
                    {
                    if ( !subPlayerName.CompareF( *iCurrentlySelectedSubPlayerName ) )
                        {
                        // names are the same and we add the selection icon
                        TFileName item;
                        
                        item.Format(KUPnPBrowseListboxString, &subPlayerName, 0);
                        listBoxItems->AppendL(item);
                        }
                    else
                        {
                        listBoxItems->AppendL(subPlayerName);
                        }
                    }
                else
                    {
                    listBoxItems->AppendL(subPlayerName);
                    }
                }

            // if search is complete we add the <search again> listbox item last
            if ( aComplete )
                {
                HBufC* listText = StringLoader::LoadLC(R_UPNP_LIST_UPNP_MORE_SUBPLAYERS);
                listBoxItems->AppendL(*listText);
                CleanupStack::PopAndDestroy( listText );
                }

            iPopup->ButtonGroupContainer()->SetCommandSetL(
                                                R_AVKON_SOFTKEYS_SELECT_CANCEL);
            iPopup->ButtonGroupContainer()->DrawDeferred();

            iListBox->HandleItemAdditionL(); // Update listbox
            iListBox->SetCurrentItemIndexAndDraw(0); // select new item
            }
        }

    if ( !aSubPlayers || (subPlayersCount <= 0) )
        {
        if ( aComplete ) // search is complete
            {
            HBufC* searchAgainText = StringLoader::LoadLC(R_UPNP_LIST_UPNP_MORE_SUBPLAYERS);
            listBoxItems->AppendL(*searchAgainText);
            CleanupStack::PopAndDestroy(searchAgainText);

            iPopup->ButtonGroupContainer()->SetCommandSetL(
                                                R_AVKON_SOFTKEYS_SELECT_CANCEL);
            iPopup->ButtonGroupContainer()->DrawDeferred();
            iListBox->HandleItemAdditionL(); // Update listbox
            iListBox->SetCurrentItemIndexAndDraw(0); // select new item
            }
        else  // not complete, we're still searching
            {
            DisplayWaitingTextL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::HandlePropertyL
// Handle playback property.
// ---------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::HandlePropertyL( TMPXPlaybackProperty /*aProperty*/,
                                            TInt /*aValue*/,
                                            TInt /*aError*/)
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::HandlePropertyL()" );
    // empty
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::HandleSubPlayerNamesL
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time.
// ---------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::HandleSubPlayerNamesL( TUid /*aPlayer*/,
                            const MDesCArray* aSubPlayers,
                            TBool aComplete,
                            TInt aError)
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::HandleSubPlayerNamesL()" );
    MPX_DEBUG3( "CMPXUPnPBrowseDialog::HandleSubPlayerNamesL(complete %d, error: %d)",
                aComplete, aError );

    if ( aError == KErrNone )
        {
        DisplaySubPlayersNamesL(aSubPlayers, aComplete);
        }
    else
        {
        if ( ( aError == KErrServerTerminated ) ||
             ( aError == KErrCouldNotConnect ) ||
             ( aError == KErrWlanNetworkNotFound ))
            {
            // defined in errorres.rsg
            DisplayErrorNoteL( R_ERRE_GE_WLAN_WLAN_NETWORK_LOST );
            }

        // No error note specified, we can't get list of subplayers so
        // we just exit the dialog
        iPopup->CancelPopup();
        }
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::HandleMediaL
// Call back of media request
// ---------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::HandleMediaL( const CMPXMedia& /*aProperties*/,
                                         TInt /*aError*/)
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::HandleMediaL()" );
    // empty
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::AppendIconToArrayL
// Load a possibly skinned icon (with mask) and append it to an
// icon array.
// ---------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::AppendIconToArrayL(CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId)
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::AppendIconToArrayL()" );

    __ASSERT_DEBUG(aArray != NULL, User::Leave(KErrArgument));

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC(aSkin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId);

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    icon->SetBitmapsOwnedExternally(EFalse);

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(2); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL(icon);
    aArray->AppendL(icon);

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::CreatePopupL
// Creates a selection popup.
// ---------------------------------------------------------------------------
void CMPXUPnPBrowseDialog::CreatePopupL()
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::CreatePopupL()" );

    // Create and configure the list box
    if ( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }
    iListBox = new (ELeave) CAknSinglePopupMenuStyleListBox;
    iPopup = CAknPopupList::NewL(iListBox,
                                 R_MPX_SOFTKEYS_UPNP_EMPTY_CANCEL,
                                 AknPopupLayouts::EDynMenuWindow);
    iListBox->ConstructL(iPopup, EAknListBoxSelectionList);
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );
    // Enable Marquee
    CEikFormattedCellListBox* eikListbox = 
    static_cast<CEikFormattedCellListBox*>( iListBox );
    eikListbox->EnableExtendedDrawingL();
    eikListbox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
    PreLayoutDynInitL();
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::StartPopupL
// Parses the friendly names from the list of UPnPDevices and
// executes the selection dialog. Returns 0 or 1 (Cancel or Select)
// ---------------------------------------------------------------------------
//
TInt CMPXUPnPBrowseDialog::StartPopupL()
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::StartPopupL()" );

    TInt popupOk = iPopup->ExecuteLD();
    iPopup = NULL;
    return popupOk;
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXUPnPBrowseDialog::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::HandlePlaybackMessage()" );

    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::RetrieveCurrentlySelectSubplayerNameL
// Retrieves the name and other info of the currently used subplayer
// ---------------------------------------------------------------------------
//
void CMPXUPnPBrowseDialog::RetrieveCurrentlySelectSubplayerNameL()
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::RetrieveCurrentlySelectSubplayerName()" );

    if ( iCurrentlySelectedSubPlayerName )
        {
        delete iCurrentlySelectedSubPlayerName;
        iCurrentlySelectedSubPlayerName = NULL;
        }

    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
    TMPXPlaybackPlayerType playerType;
    TInt currentlyUsedSubPlayer;
    MPX_DEBUG1( "CMPXUPnPBrowseDialog::RetrieveCurrentlySelectSubplayerName() before call to manager.GetSelectionL()" );
    MPX_TRAPD( errorSelectSubPlayer, manager.GetSelectionL( playerType,
                                                        iCurrentlyUsedPlayer,
                                                        currentlyUsedSubPlayer,
                                                        iCurrentlySelectedSubPlayerName));
    MPX_DEBUG4( "CMPXUPnPBrowseDialog::RetrieveCurrentlySelectSubplayerName() after call to manager.GetSelectionL(%d, %d, %d)",
                                                        playerType,
                                                        iCurrentlyUsedPlayer.iUid,
                                                        currentlyUsedSubPlayer );
    if (iCurrentlySelectedSubPlayerName)
        {
        MPX_DEBUG2( "CMPXUPnPBrowseDialog::RetrieveCurrentlySelectSubplayerName() after call to manager.GetSelectionL(%S)",
                                                        iCurrentlySelectedSubPlayerName );
        }
    if ( errorSelectSubPlayer != KErrNone  && iCurrentlySelectedSubPlayerName )
        {
        MPX_DEBUG2( "CMPXUPnPBrowseDialog::RetrieveCurrentlySelectSubplayerName() after call to manager.GetSelectionL() error %d",
                                                        errorSelectSubPlayer );
        delete iCurrentlySelectedSubPlayerName;
        iCurrentlySelectedSubPlayerName = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::DisplayErrorNoteL
// Retrieves the name and other info of the currently used subplayer
// ---------------------------------------------------------------------------
//
void CMPXUPnPBrowseDialog::DisplayErrorNoteL( TInt aResourceId )
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::DisplayErrorNoteL()" );

    // error note
    HBufC* dialogText = StringLoader::LoadLC( aResourceId );
    CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
    errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
    errNote->ExecuteLD( *dialogText );
    CleanupStack::PopAndDestroy( dialogText );
    }

// ---------------------------------------------------------------------------
// CMPXUPnPBrowseDialog::DisplayWaitingTextL
// Displays the waiting text during retrieval of subplayer names
// ---------------------------------------------------------------------------
//
void CMPXUPnPBrowseDialog::DisplayWaitingTextL()
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::DisplayWaitingTextL()" );

    if ( iListBox && iListBox->View() )
        {
        HBufC* waitText = StringLoader::LoadLC(R_UPNP_DIALOG_WAITING_FOR_DEVICES);
        iListBox->View()->SetListEmptyTextL(*waitText);
        CleanupStack::PopAndDestroy(waitText);
        }
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXUPnPBrowseDialog::DoHandlePlaybackMessageL(
    const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXUPnPBrowseDialog::DoHandlePlaybackMessageL" );

    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EPlayerUnavailable:
                {
                MPX_DEBUG2( "HandlePlaybackMessageL - EPlayerUnavailable(%d)", type );
                TUid activeView = iViewUtility->ActiveViewType();
                if ( activeView == TUid::Uid( KMPXPluginTypeUPnPBrowseDialogUid ) )
                    {
                    iPlayerUnavailableError = ETrue;
                    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
                    TRAP_IGNORE( manager.ClearSelectPlayersL() );
                    DisplayErrorNoteL( R_UPNP_ERROR_PLAYER_UNAVAILABLE );

                    iPopup->CancelPopup();
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// End of file

