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
* Description:  Implementation of Podcast Collection view
*
*/


// INCLUDE FILES
#include <aknlists.h>
#include <aknViewAppUi.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <aknnavilabel.h>
#include <aknnavide.h>
#include <eikapp.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>   // Icon no skin
#include <akntitle.h>
#include <aknconsts.h>
#include <avkon.mbg>
#include <textresolver.h>
#include <aknnotewrappers.h>
#include <aknnavi.h>
#include <hlplch.h>
#include <bautils.h>
#include <eikspmod.h>
#include <eikclb.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <sendui.h>
#include <CMessageData.h>
#include <centralrepository.h>
#include <avkon.rsg>
#include <eikbtgpc.h>
#include <AknWaitDialog.h>
#include <apgcli.h>     // RApaLsSession
#include <akncontext.h> // Context Pane
#include <fbs.h>        // CFbsBitmap
#include <bldvariant.hrh>
#include <coeutils.h>
#include <AknDlgShut.h>
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE
#include <mpxuser.h>

#ifdef UPNP_INCLUDED
#include <upnpcopycommand.h>
#endif
#include <e32const.h>
#include <AiwServiceHandler.h>  //Copy to remote feature

//#ifdef __COVER_DISPLAY
#include <aknSDData.h>
#include <AknMediatorFacade.h>
#include "mplayersecondarydisplayapi.h"
//#endif

#include <mpxappui.hrh>
#include <data_caging_path_literals.hrh>
#include <layoutmetadata.cdl.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxcollectionmessage.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxpodcastcollectionview.rsg>
#include <mpxcollectionview.mbg>
#include <mpxcollectionutility.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmedia.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionplaylist.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediaarray.h>
#include <mpxpodcastdefs.h> // Podcast specific
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxconstants.h>
#include "mpxwaitnotedefs.h"

#include <mpxinternalcrkeys.h>
#include "mpxpodcastcollectionviewlistboxarray.h"
#include "mpxcommoncontainer.hrh"
#include "mpxcommoncontainer.h"
#include "mpxcommonuihelper.h"
#include "mpxpodcastcollectionviewcontainer.h"
#include "mpxpodcastcollectionview.hrh"
#include "mpxpodcastcollectionview.hlp.hrh"
#include "mpxpodcastcollectionviewimp.h"
#include "mpxlog.h"
#include "mpxviewprivatepskeys.h"


// CONSTANTS
_LIT(KMPXPodcastCollectionRscPath, "mpxpodcastcollectionview.rsc");

// Nokia Podcasting Application Uid
const TUid KNPodAppUid = { 0x1028190B };

const TInt KMilliSecondsToSeconds(1000);
const TInt KMPXMaxBufferLength(160);
const TInt KMPXMaxTimeLength(36);
const TInt KMPXDurationDisplayResvLen(10);
const TInt KPodcastCollectionMenuLevel( 2 );

//#ifdef __UPNP_FRAMEWORK_2_0_
const TInt KMPXLastDirectoryLevel( 2 );
//#endif //__UPNP_FRAMEWORK_2_0_
// MACROS
_LIT(KMPXCollDetailsItemsFormat, "%S\t%S");

#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
#define KMusicCollectionUid 0x101FFC3A
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewImp* CMPXPodcastCollectionViewImp::NewL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::NewL");
    CMPXPodcastCollectionViewImp* self = CMPXPodcastCollectionViewImp::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewImp* CMPXPodcastCollectionViewImp::NewLC()
    {
    CMPXPodcastCollectionViewImp* self = new (ELeave) CMPXPodcastCollectionViewImp();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewImp::~CMPXPodcastCollectionViewImp()
    {
    MPX_DEBUG1("CMPXPodcastCollectionViewImp::~CMPXPodcastCollectionViewImp entering");

    if (iCollectionUtility)
        {
        iCollectionUtility->Close();
        }

    if (iPlaybackUtility)
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }

    if (iViewUtility)
        {
        iViewUtility->Close();
        }

    if (iCollectionUiHelper)
        {
        iCollectionUiHelper->Close();
        }

    if ( iUpnpFrameworkSupport )
        {
        MPX_DEBUG1(_L("CMPXCollectionViewImp::~CMPXCollectionViewImp Detaching 'Copy to external' menu service..."));
        if ((iServiceHandler) &&
             (iCurrentlyAttachedCopyToMenuPane != KErrNotFound))
            {
            iServiceHandler->DetachMenu(iCurrentlyAttachedCopyToMenuPane,
                                 R_MPX_UPNP_COPY_TO_EXTERNAL_MENU_INTEREST);
            }
        delete iServiceHandler;
        iPlayersList.Close();
        delete iSubPlayerName;
        }
#ifdef UPNP_INCLUDED
    if (iUpnpCopyCommand)
        {
        delete iUpnpCopyCommand;
        }
#endif
    if (iResourceOffset)
        {
        iEikonEnv->DeleteResourceFile(iResourceOffset);
        }

    if (iContainer)
        {
        AppUi()->RemoveFromStack( iContainer->Common()->CoeControl() );
        delete iContainer;
        }

    if (iNaviDecorator)
        {
        delete iNaviDecorator;
        }

    if (iMediaToSet)
        {
        delete iMediaToSet;
        }

    delete iCommonUiHelper;
    delete iSendUi;
    delete iTitle;
    delete iNumEpisode;
    delete iDuration;
    delete iOriginalTitle;
    delete iOriginalDuration;
    delete iBottomIndex;
    delete iCba;


    MPX_DEBUG1("CMPXPodcastCollectionViewImp::~CMPXPodcastCollectionViewImp exiting");
    }

// ---------------------------------------------------------------------------
// Delete the selected items
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DeleteSelectedItemsL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DeleteSelectedItemsL");

    TBool isIgnore(EFalse);
    if (iContainer->Common()->CurrentListItemCount() == 0)
        {
        // list is empty
        isIgnore = ETrue;
        }

    if (!isIgnore)
        {
        CMPXCommonListBoxArrayBase* listboxArray =
            iContainer->Common()->ListBoxArray();
        TInt currentIndex = iContainer->Common()->CurrentLbxItemIndex();
        // Create a copy of collection path
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL(path);
        CAknQueryDialog* confirmationDlg = NULL;
        HBufC* promptTxt = NULL;
        HBufC* waitNoteText = NULL;
//        TInt waitNoteCBA = R_AVKON_SOFTKEYS_EMPTY;

        // Marked indicies
        const CArrayFix<TInt>* array =
            iContainer->Common()->CurrentSelectionIndicesL();
        TInt arrayCount = array->Count();
        MPX_DEBUG2("CMPXPodcastCollectionViewImp::DeleteSelectedItemsL delete array count = %d", arrayCount);

        if ( arrayCount > 1 )
            {
            waitNoteText = StringLoader::LoadLC(
                R_MPX_QTN_NMP_DEL_SONGS_WAIT_NOTE);
            promptTxt = StringLoader::LoadLC(
                R_MPX_QTN_NMP_DEL_SONGS_QUERY,
                    arrayCount);
//            waitNoteCBA = R_MPX_COLLECTION_WAITNOTE_SOFTKEYS_EMPTY_STOP;
            confirmationDlg = CAknQueryDialog::NewL(
                CAknQueryDialog::EConfirmationTone);
            CleanupStack::PushL(confirmationDlg);

//#ifdef __COVER_DISPLAY
            confirmationDlg->PublishDialogL(
                EMPlayerQueryDeleteTracks,
                KMPlayerNoteCategory);
            CAknMediatorFacade* covercl =
                AknMediatorFacade(confirmationDlg);
            if (covercl)
                {
                covercl->BufStream().WriteInt32L(arrayCount);
                }
//#endif //__COVER_DISPLAY

            for (TInt i = 0; i < arrayCount; i++)
                {
                MPX_DEBUG2("CMPXPodcastCollectionViewImp::DeleteSelectedItemsL delete array index = %d", array->At(i));
                path->SelectL(array->At(i));
                }
            }
        else
            {
            const CMPXMedia& media( listboxArray->MediaL(
                ( arrayCount > 0 ) ? array->At( 0 ) : currentIndex ) );
            const TDesC& title = media.ValueText(KMPXMediaGeneralTitle);

            TMPXGeneralType type = EMPXNoType;
            TMPXGeneralCategory category = EMPXNoCategory;

            if (media.IsSupported(KMPXMediaGeneralType))
                {
                type =
                    media.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);
                }
            if (media.IsSupported(KMPXMediaGeneralCategory))
                {
                category =
                    media.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);
                }

            if (type == EMPXItem && category == EMPXPodcast)
                {
                // tracks level
                waitNoteText = StringLoader::LoadLC(
                        R_MPX_QTN_ALBUM_WAITING_DELETING, title);
                promptTxt = StringLoader::LoadLC(
                    R_MPX_QTN_QUERY_COMMON_CONF_DELETE,
                    title);
                }
            else
                {
                waitNoteText = StringLoader::LoadLC(
                        R_MPX_QTN_MUS_QUERY_CONF_DELETE_ALL, title);
//                waitNoteCBA = R_MPX_COLLECTION_WAITNOTE_SOFTKEYS_EMPTY_STOP;
                promptTxt = StringLoader::LoadLC(
                    R_MPX_QTN_NMP_QUERY_CONF_DELETE_GROUP,
                    title);
                }
            confirmationDlg = CAknQueryDialog::NewL(
                CAknQueryDialog::EConfirmationTone);
            CleanupStack::PushL(confirmationDlg);
//#ifdef __COVER_DISPLAY
            if (type == EMPXItem && category == EMPXPodcast)
                {
                // tracks level
                confirmationDlg->PublishDialogL(
                    EMPlayerQueryDeleteCategory,
                    KMPlayerNoteCategory);
                }
            else
                {
                confirmationDlg->PublishDialogL(
                    EMPlayerQueryDeleteTrack,
                    KMPlayerNoteCategory);
                }
            CAknMediatorFacade* covercl =
                AknMediatorFacade(confirmationDlg);
            if (covercl)
                {
                covercl->BufStream() << title;
                }
//#endif //__COVER_DISPLAY
            path->Set( arrayCount > 0 ? array->At( 0 ) : currentIndex );
            }
        confirmationDlg->SetPromptL(*promptTxt);
        CleanupStack::Pop(confirmationDlg);
        CleanupStack::PopAndDestroy(promptTxt);
        if (confirmationDlg->ExecuteLD(R_MPX_CUI_DELETE_CONFIRMATION_QUERY))
            {
            HandleCommandL( EMPXCmdIgnoreExternalCommand );
            StartDeleteWaitNoteL();
            TPtr buf = waitNoteText->Des();
			UpdateProcessL(buf);
            iCollectionUiHelper->DeleteL(*path, this);
            iIsDeleting = ETrue;

          }
        CleanupStack::PopAndDestroy(waitNoteText);
        CleanupStack::PopAndDestroy(path);
        }
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMPXPodcastCollectionViewImp::CMPXPodcastCollectionViewImp()
    : iMediaToSet(NULL)
    , iCurrNotYetPlayedListIndex(-1)
#ifdef __ENABLE_MSK
    , iCurrentMskId(KErrNotFound)
    , iShowContextMenu(EFalse)
#endif // __ENABLE_MSK
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::CMPXPodcastCollectionViewImp");
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::ConstructL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::ConstructL");

    CCoeEnv* coeEnv( iEikonEnv );
    TParse parse;
    parse.Set(KMPXPodcastCollectionRscPath, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile(parse.FullName());
    User::LeaveIfError(MPXUser::CompleteWithDllPath(resourceFile));
    BaflUtils::NearestLanguageFile(coeEnv->FsSession(), resourceFile);
    iResourceOffset = coeEnv->AddResourceFileL(resourceFile);

    BaseConstructL(R_MPX_PODCAST_COLLECTION_VIEW);

    // Get the collection utility instance from engine.
    iCollectionUtility = MMPXCollectionUtility::NewL(this, KMcModeDefault);
    iBackOneLevel = EFalse;
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );
    iViewUtility = MMPXViewUtility::UtilityL();
    iViewUtility->AddObserverL(this);
    iBottomIndex = new (ELeave) CArrayFixFlat<TInt>(1);

    iCommonUiHelper = CMPXCommonUiHelper::NewL(iCollectionUtility);
    iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();

    // Monitor for view activation
    AppUi()->AddViewActivationObserverL( this );
#ifdef UPNP_INCLUDED
  	if(!iUpnpCopyCommand)
  	    {
  	    MPX_TRAPD ( error, iUpnpCopyCommand = CUpnpCopyCommand::NewL() );   
		    if ( error == KErrNone )
            {
            iUpnpFrameworkSupport = ETrue;
            iServiceHandler = CAiwServiceHandler::NewL();
            MPX_DEBUG1("CMPXPodcastCollectionViewImp::ConstructL() Attaching 'Copy to external' menu service...");
            MPX_TRAP(iErrorAttachCopyMenu, iServiceHandler->AttachMenuL(R_MPX_PODCAST_COLLECTION_VIEW_MENU_1,
                                                   R_MPX_UPNP_COPY_TO_EXTERNAL_MENU_INTEREST));
            if ( iErrorAttachCopyMenu == KErrNotSupported )
                {
                // when CCoeEnv is not available
                User::Leave( iErrorAttachCopyMenu );
                }
            MPX_DEBUG2("CMPXPodcastCollectionViewImp::ConstructL(): attach Copy menu error: %d", iErrorAttachCopyMenu);
            }
        else
            {
            iUpnpFrameworkSupport = EFalse;
			iUpnpCopyCommand = NULL;
            }
        }
#endif

    TUid naviPaneUid;
    naviPaneUid.iUid = EEikStatusPaneUidNavi;
    iNaviPane =
        static_cast<CAknNavigationControlContainer*>
        (iAvkonViewAppUi->StatusPane()->ControlL(naviPaneUid));
    iNaviDecorator = iNaviPane->CreateNavigationLabelL(KNullDesC);
    //Create label to change text in Navi pane
    iNaviLabel = static_cast<CAknNaviLabel*>
            (iNaviDecorator->DecoratedControl());

    TInt flags(0);
    CRepository* repository = CRepository::NewL(KCRUidMPXMPFeatures);
    repository->Get(KMPXMPLocalVariation, flags);
    delete repository;
    iChangeRTForAllProfiles =
        static_cast<TBool>(flags & KMPXChangeRTForAll);

    iLastDepth = 1;
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::ConstructL iLastDepth = %d", iLastDepth);
    iBottomIndex->AppendL(0);

#ifdef _DEBUG
    iExitOptionHidden = EFalse;
#else // _DEBUG
    iExitOptionHidden = iCommonUiHelper->ExitOptionHiddenL();
#endif // _DEBUG

    // used to know if HandleOpen was called -> we cannot update the playback
    // status of listbox items before the list box is updated in HandleOpen
    iHandleOpenProcessed = EFalse;
    iSelectedItem = KMPXInvalidItemId;
    }

// ---------------------------------------------------------------------------
// Updates list box
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateListBoxL(
    CMPXMedia& aEntries,
    TInt aIndex)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdateListBox");
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateListBox aIndex = %d", aIndex);
    if (iContainer)
        {
        CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL(cpath);

        CMPXCommonListBoxArrayBase* array =
            iContainer->Common()->ListBoxArray();
        array->ResetMediaArrayL();
        iContainer->Common()->HandleLbxItemRemovalL();


        array->AppendMediaL(aEntries);
        // this needs to be before HandleLbxItemAdditionL so that the items
        // are redrawn properly after playback status has been updated.
        UpdatePlaybackStatusL(EFalse);

        iContainer->Common()->HandleLbxItemAdditionL();
        MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateListBox List box contains %d items", iContainer->Common()->TotalListItemCount());

        // Display empty List Box string if list box is empty
        // Display different empty string if Podcast App does not exist
        HBufC* emptyText = NULL;
        if (CheckPodcastAppL() == KErrNone)
            {
            emptyText = StringLoader::LoadLC(
                R_MPX_QTN_NMP_NO_EPISODES_GO_TO_APP);
            }
        else
            {
            emptyText = StringLoader::LoadLC(
                R_MPX_QTN_NMP_NO_EPISODES);
            }
        iContainer->Common()->SetLbxEmptyTextL(*emptyText);
        CleanupStack::PopAndDestroy(emptyText);

        if (iBackOneLevel || iPossibleJump)
            {
            if (cpath->Levels() == (iLastDepth))
                {
                iLastDepth--;
                TInt topIndex = iContainer->Common()->CalculateTopIndex(
                    iBottomIndex->At(iLastDepth));
                iContainer->Common()->SetLbxTopItemIndex(topIndex);
                iContainer->Common()->SetLbxCurrentItemIndexAndDraw(aIndex);
                iBottomIndex->Delete(iLastDepth);
                }
            else
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::UpdateListBox Invalid history, rebuilding");
                // invalid path, rebuild with all 0. possibily caused by a jump in views
                iBottomIndex->Reset();
                iLastDepth = cpath->Levels();
                for (TInt i = 0; i < iLastDepth; i++)
                    {
                    iBottomIndex->AppendL(0);
                    }
                iContainer->Common()->SetLbxCurrentItemIndexAndDraw(aIndex);
                }
            iPossibleJump = EFalse;
            iBackOneLevel = EFalse;
            }
        else
            {
            if ((aIndex > 0) &&
                (aIndex < iContainer->Common()->CurrentListItemCount()))
                {
                iContainer->Common()->SetLbxCurrentItemIndexAndDraw(aIndex);
                }
            else if(aIndex == 0 &&
                    iCurrNotYetPlayedListIndex > 0 &&
                    iCurrNotYetPlayedListIndex < iContainer->Common()->CurrentListItemCount())
                {
                iContainer->Common()->SetLbxCurrentItemIndexAndDraw(iCurrNotYetPlayedListIndex);
                }
            else
                {
                if ( iSelectedItem != KMPXInvalidItemId )
                    {
                    iContainer->Common()->SetLbxCurrentItemIndexAndDraw(cpath->IndexOfId(iSelectedItem));
                    }
                else
                    {
                    iContainer->Common()->SetLbxCurrentItemIndexAndDraw(0);
                    }
                }
            iCurrNotYetPlayedListIndex = -1;
            }
        CleanupStack::PopAndDestroy(cpath);
        }
    }

// ---------------------------------------------------------------------------
// Displays error notes.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleErrorL(TInt aError)
    {
    MPX_DEBUG2( "CMPXPodcastCollectionViewImp::HandleError(%d)", aError );
    ASSERT( aError );

    // only display error message if postcastcollection view is in the foreground
    if ( iContainer )
        {
        TRAP_IGNORE( iCommonUiHelper->HandleErrorL( aError ) );
        }
    }

// ---------------------------------------------------------------------------
// Updates the navi pane
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateNaviPaneL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdateNaviPaneL");
    TUid activeView = iViewUtility->ActiveViewType();
    if (activeView == TUid::Uid(KMPXPluginTypeCollectionUid) ||
        activeView == TUid::Uid(KMPXPluginTypeWaitNoteDialogUid)) // the harvester
                                                           // might have a
                                                           // scanning note
                                                           // displayed
        {
        MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateNaviPaneL updating %d", iNumEpisode);
        if (iNumEpisode)
            {
            iNaviLabel->SetTextL(*iNumEpisode);
            iNaviPane->PushL(*iNaviDecorator);
            }
        else
            {
            iNaviPane->Pop(iNaviDecorator);
            }
        iNaviPane->DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// Updates the title pane
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateTitlePaneL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdateTitlePaneL");
    // Set title
    TUid activeView = iViewUtility->ActiveViewType();
    if (activeView == TUid::Uid(KMPXPluginTypeCollectionUid))
        {
        CAknTitlePane* title(NULL);

        TRAP_IGNORE(
            {
            title = static_cast<CAknTitlePane*>
                (StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
            });

        if (title)
            {
            if (iTitle)
                {
                MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateTitlePaneL Title is %S", iTitle);
                if (iTitle->Length() == 0)
                    {
                    HBufC* titleText = StringLoader::LoadLC(R_MPX_QTN_NMP_UNKNOWN_TITLE);
                    title->SetTextL(*titleText);
                    CleanupStack::PopAndDestroy(titleText);
                    }
                else
                    {
                    title->SetTextL(*iTitle);
                    }
                }
            else
                {
                CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL(cpath);
                TInt currentDepth = cpath->Levels();
                if(currentDepth == 2)
                    {
                    // use default title if no metadata is available
                    HBufC* titleText = StringLoader::LoadLC(
                        R_MPX_PODCAST_COLLECTION_VIEW_TITLE);
                    title->SetTextL(*titleText);
                    CleanupStack::PopAndDestroy(titleText);
                    }
                CleanupStack::PopAndDestroy(cpath);
                } // if (iTitle)
            }
        }
    }

// ---------------------------------------------------------------------------
// Updates the option menu
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateOptionMenuL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdateOptionMenuL");
    if(iContainer)
        {
        // Check collection path level and change title resource id
        CEikMenuBar* menubar = MenuBar();
        TInt attachCopyToMenu = KErrNotFound;

        if ( menubar )
            {
            CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL(cpath);
            MPX_DEBUG_PATH(*cpath);
            TInt depth = cpath->Levels();
            MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateOptionMenuL cpath depth = %d", depth);
            // root
            if ( depth == 2 )
                {
                iContainer->Common()->EnableMarking( EFalse );
                iContainer->SetCurrentView(EMPXTitlesView);
                menubar->SetMenuTitleResourceId(R_MPX_PODCAST_COLLECTION_TITLES_VIEW_MENUBAR);
                if ( iUpnpFrameworkSupport )
                    {
                    attachCopyToMenu = R_MPX_PODCAST_COLLECTION_VIEW_MENU_1;
                    }
                }
            else if ( depth == 3 )
                {
#ifdef SINGLE_CLICK_INCLUDED
                iContainer->Common()->EnableMarking( EFalse );
#else
                iContainer->Common()->EnableMarking( ETrue );
#endif
                iContainer->SetCurrentView(EMPXEpisodesView);
#ifdef SINGLE_CLICK_INCLUDED
                menubar->SetMenuTitleResourceId(R_MPX_PODCAST_COLLECTION_EPISODES_VIEW_NO_MARKING);
#else
                menubar->SetMenuTitleResourceId(R_MPX_PODCAST_COLLECTION_EPISODES_VIEW_MENUBAR);
#endif
                if ( iUpnpFrameworkSupport )
                    {
                    attachCopyToMenu = R_MPX_PODCAST_COLLECTION_VIEW_MENU_2;
                    }
                }
            else // (depth > 3)
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::UpdateOptionMenuL ERROR cpath depth > 3");
                ASSERT(0);
                }
            CleanupStack::PopAndDestroy(cpath);
            }

        if ( iUpnpFrameworkSupport )
            {
            UpdateCopyToRemoteMenu(attachCopyToMenu);
            }
        }
    else
        {
        MPX_DEBUG1("CMPXPodcastCollectionViewImp::UpdateOptionMenuL NULL iContainer");
        }
    }

//#ifdef __UPNP_FRAMEWORK_2_0_
// ---------------------------------------------------------------------------
// Updates the copy to remote menu attachment
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateCopyToRemoteMenu(TInt aMenuPane)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdateCopyToRemoteMenu");
    MPX_DEBUG3("CMPXPodcastCollectionViewImp::UpdateCopyToRemoteMenu(): aMenuPane %d, iCurrentlyAttachedCopyToMenuPane %d", aMenuPane, iCurrentlyAttachedCopyToMenuPane);

    if (iServiceHandler &&
         (iCurrentlyAttachedCopyToMenuPane != aMenuPane))
        {
        if (iCurrentlyAttachedCopyToMenuPane != KErrNotFound)
            {
            MPX_DEBUG1(_L("CMPXCollectionViewImp::UpdateCopyToRemoteMenu() Detaching 'Copy to external' menu service..."));
            iServiceHandler->DetachMenu(iCurrentlyAttachedCopyToMenuPane,
                                 R_MPX_UPNP_COPY_TO_EXTERNAL_MENU_INTEREST);
            }

        if (aMenuPane != KErrNotFound)
            {
            MPX_DEBUG1("CMPXPodcastCollectionViewImp::UpdateCopyToRemoteMenu() Attaching 'Copy to external' menu service...");
            MPX_TRAP(iErrorAttachCopyMenu, iServiceHandler->AttachMenuL(aMenuPane,
                                                       R_MPX_UPNP_COPY_TO_EXTERNAL_MENU_INTEREST));
            if (iErrorAttachCopyMenu == KErrNone)
                {
                iCurrentlyAttachedCopyToMenuPane = aMenuPane;
                }
            else
                {
                iCurrentlyAttachedCopyToMenuPane = KErrNotFound;
                }
            MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateCopyToRemoteMenu(): iErrorAttachCopyMenu: %d", iErrorAttachCopyMenu);
            }
        else
            {
            iCurrentlyAttachedCopyToMenuPane = aMenuPane;
            }
        }
    }
//#endif //__UPNP_FRAMEWORK_2_0_

// ---------------------------------------------------------------------------
// Updates playback status indicatior
// ---------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewImp::UpdatePlaybackStatusL(TBool aStatusChangeFlag)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdatePlaybackStatusL");
    TInt ret( KErrNotFound );  // default selection
    if ( iContainer )
        {
        TMPXPlaybackState pbState( EPbStateNotInitialised );
        TInt selectedIndex( KErrNotFound );
        TMPXItemId selectedItemId( KMPXInvalidItemId );
        CMPXCollectionPath* cpath( iCollectionUtility->Collection().PathL() );
        CleanupStack::PushL( cpath );

        MMPXSource* source( iPlaybackUtility->Source() );
        if ( source )
            {
            CMPXCollectionPlaylist* playlist( source->PlaylistL() );
            if ( playlist )
                {
                CleanupStack::PushL( playlist );
                CMPXCollectionPath* pbPath(
                    CMPXCollectionPath::NewL( playlist->Path() ) );
                CleanupStack::PushL( pbPath );
                TInt playbackPathCount( pbPath->Levels() );
                if ( cpath->Levels() == playbackPathCount )
                    {
                    // not comparing the index
                    TBool isEqual( ETrue );
                    for ( TInt i = 0; i < playbackPathCount - 1; i++ )
                        {
                        if ( cpath->Id( i ) != pbPath->Id( i ) )
                            {
                            isEqual = EFalse;
                            break;
                            }
                        }
                    if ( isEqual )
                        {
                        selectedIndex = pbPath->Index();
                        selectedItemId = pbPath->Id();
                        ret = selectedIndex;
                        pbState = iPlaybackUtility->StateL();
                        }
                    }
                CleanupStack::PopAndDestroy( pbPath );
                CleanupStack::PopAndDestroy( playlist );
                }
            }
        CleanupStack::PopAndDestroy( cpath );

        CMPXCommonListBoxArrayBase* listboxArray =
            iContainer->Common()->ListBoxArray();
        const CMPXMedia* containerMedia = &listboxArray->ContainerMedia();

        if (!containerMedia ||
            !containerMedia->IsSupported(KMPXMediaPodcastCategoryGroup) ||
            EMPXNotYetPlayed != containerMedia->ValueTObjectL<TMPXPodcastCategory>(KMPXMediaPodcastCategoryGroup))
            {
            if ( iHandleOpenProcessed )
                {
                // Might make performance worse if there are lots of episodes
                iContainer->SetPlaybackStatusByIdL( selectedItemId, pbState );
                }
            }

        if (iMediaToSet &&
            aStatusChangeFlag &&
            (pbState == EPbStateStopped ||
             pbState == EPbStateNotInitialised))
            {
            DoSetCollectionL(iMediaToSet, EFalse );
            delete iMediaToSet;
            iMediaToSet = NULL;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Start a delete wait note
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::StartDeleteWaitNoteL()	
	{
    iWaitDialog = new (ELeave) CAknWaitDialog(
		(REINTERPRET_CAST(CEikDialog**, &iWaitDialog)),
		ETrue);
    iWaitDialog->PrepareLC(R_MPX_DELETE_WAIT_NOTE);
    iWaitDialog->SetCallback(this);
    iWaitDialog->RunLD();
	}
 
// ---------------------------------------------------------------------------
// Update the delete wait note
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateProcessL(const TDesC& aDeleteNoteText)
	{
	if(iWaitDialog)
		{
        iWaitDialog->SetTextL(aDeleteNoteText);	
    	}
	}

// -----------------------------------------------------------------------------
// Change the button group
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::SetNewCbaL(TInt aResId)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::SetNewCbaL");
    if (!iCba)
        {
        CCoeControl* coeControl = iContainer->Common()->CoeControl();
        iCba = CEikButtonGroupContainer::NewL(
            CEikButtonGroupContainer::ECba,
            CEikButtonGroupContainer::EHorizontal,
            this,
            aResId, *coeControl );
        }
    else
        {
        iCba->SetCommandSetL(aResId);
        iCba->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// Display collection details
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::ShowCollectionDetailsL(const CMPXMedia& aMedia)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::ShowCollectionDetailsL");
    CAknSingleHeadingPopupMenuStyleListBox* list =
        new (ELeave) CAknSingleHeadingPopupMenuStyleListBox;

    CleanupStack::PushL(list);
    CAknPopupList* popupList = CAknPopupList::NewL(
        list, R_AVKON_SOFTKEYS_OK_EMPTY,
        AknPopupLayouts::EMenuGraphicHeadingWindow);

    CleanupStack::PushL(popupList);
    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto);

    CDesCArrayFlat* dataArray =
        new (ELeave) CDesCArrayFlat(EMPXCollectionDetailsCount);
    CleanupStack::PushL(dataArray);

    // Enable Marquee
    static_cast<CEikFormattedCellListBox*>(list)->ItemDrawer()->ColumnData()->
        EnableMarqueeL(ETrue);

    // number of items
    HBufC* dataToAppend = HBufC::NewLC(KMPXMaxBufferLength);
    TInt songsCount = 0;
    if (aMedia.IsSupported(KMPXMediaColDetailNumberOfItems))
        {
        songsCount = aMedia.ValueTObjectL<TInt>(KMPXMediaColDetailNumberOfItems);
        }
    TPtr ptr = dataToAppend->Des();
    ptr.AppendNum(songsCount);
    AknTextUtils::LanguageSpecificNumberConversion(ptr);
    dataArray->AppendL(ptr);
    CleanupStack::PopAndDestroy(dataToAppend);
    dataToAppend = NULL;

    // total duration
    TInt duration = 0;
    if (aMedia.IsSupported(KMPXMediaColDetailDuration))
        {
        duration = aMedia.ValueTObjectL<TInt>(KMPXMediaColDetailDuration);
        }
    if (duration > 0)
        {
        dataToAppend = iCommonUiHelper->DisplayableDurationInTextL(duration/KMilliSecondsToSeconds);
        CleanupStack::PushL(dataToAppend);
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion(ptr);
        dataArray->AppendL(ptr);
        CleanupStack::PopAndDestroy(dataToAppend);
        dataToAppend = NULL;
        }
    else
        {
        dataArray->AppendL(KNullDesC);
        }

#ifdef RD_MULTIPLE_DRIVE
    RFs& fileSession( iCoeEnv->FsSession() );
    TDriveList driveList;
    TInt driveCount(0);
    TInt64 freePhoneMemory(KErrNotFound);
    TInt64 freeInternalMemory(KErrNotFound);
    TInt64 freeExternalMemory(KErrNotFound);
    TVolumeInfo volInfo;

    // Get all visible drives
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives(
           fileSession, driveList, driveCount ) );
    MPX_DEBUG2 ("CMPXCollectionViewImp::ShowCollectionDetailsL - driveCount = %d", driveCount);

    for( TInt driveNum = EDriveA; driveNum <= EDriveZ; driveNum++ )
        {
        if (driveList[driveNum])
            {
            // Get the volume information
            if ( fileSession.Volume( volInfo, driveNum ) == KErrNone )
                {
                // Get the drive status
                TUint driveStatus(0);
                DriveInfo::GetDriveStatus( fileSession, driveNum, driveStatus );

                // Add up free memory depending on memory location
                if ( driveNum == EDriveC )
                    {
                    if ( freePhoneMemory == KErrNotFound )
                        {
                        freePhoneMemory = 0;
                        }
                    freePhoneMemory += volInfo.iFree;
                    }
                else if ( driveStatus & DriveInfo::EDriveInternal )
                    {
                    if ( freeInternalMemory == KErrNotFound )
                        {
                        freeInternalMemory = 0;
                        }
                    freeInternalMemory += volInfo.iFree;
                    }
                else if ( driveStatus &
                    (DriveInfo::EDriveRemovable | DriveInfo::EDriveRemote) )
                    {
                    if ( freeExternalMemory == KErrNotFound )
                        {
                        freeExternalMemory = 0;
                        }
                    freeExternalMemory += volInfo.iFree;
                    }
                }
            }
        }

    // phone memory free
    if ( freePhoneMemory != KErrNotFound )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(
            freePhoneMemory, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr);
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_MEMORY_UNAVAILABLE );
        ptr.Set(dataToAppend->Des());
        }

    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    // internal memory (mass storage) free
    if ( freeInternalMemory != KErrNotFound )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(
            freeInternalMemory, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr); 
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_MEMORY_UNAVAILABLE );
        ptr.Set(dataToAppend->Des());
        }

    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );

    // removable/remote (memory card) memory free
    if ( freeExternalMemory != KErrNotFound )
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(
            freeExternalMemory, ETrue );
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr); 
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_CARD_UNAVAILABLE_ITEM );
        ptr.Set(dataToAppend->Des());
        }
    CleanupStack::PushL( dataToAppend );
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy( dataToAppend );
#else
    // phone memory free
    HBufC* driveLetter = StringLoader::LoadLC(R_MPX_COLLECTION_PHONE_MEMORY_ROOT_PATH);
    CEikonEnv* coeEnv = CEikonEnv::Static();
    RFs& fileSession = coeEnv->FsSession();
    TVolumeInfo volInfo;
    TDriveUnit driveUnit(*driveLetter); // Get the drive
    CleanupStack::PopAndDestroy(driveLetter);
    driveLetter = NULL;
    TInt err = fileSession.Volume(volInfo, (TInt)driveUnit);
    MPX_DEBUG2("CMPXCollectionViewImp::ShowCollectionDetailsL phonemem err = %d", err);
    if (KErrNone == err)
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(volInfo.iFree, ETrue);
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr);
        }
    else
        {
        dataToAppend = StringLoader::LoadL(
            R_MPX_CUI_COLLECTION_DETAILS_CARD_UNAVAILABLE_ITEM);
        ptr.Set(dataToAppend->Des());
        }
    CleanupStack::PushL(dataToAppend);
    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy(dataToAppend);
    dataToAppend = NULL;

    // memory card
    driveLetter = StringLoader::LoadLC(R_MPX_COLLECTION_MEMORY_CARD_ROOT_PATH);
    driveUnit = *driveLetter; // Get the drive
    CleanupStack::PopAndDestroy(driveLetter);
    driveLetter = NULL;
    err = fileSession.Volume(volInfo, (TInt)driveUnit);
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::ShowCollectionDetailsL memcard err = %d", err);
    if (err == KErrNone)
        {
        dataToAppend = iCommonUiHelper->UnitConversionL(volInfo.iFree, ETrue);
        ptr.Set(dataToAppend->Des());
        AknTextUtils::LanguageSpecificNumberConversion( ptr); 
        }
    else if (err == KErrNotReady || err == KErrCorrupt)
        {
        dataToAppend = StringLoader::LoadLC(
            R_MPX_CUI_COLLECTION_DETAILS_CARD_UNAVAILABLE_ITEM);
        ptr.Set(dataToAppend->Des());
        }
    // else ignore
    CleanupStack::PushL( dataToAppend );

    dataArray->AppendL( ptr );
    CleanupStack::PopAndDestroy(dataToAppend);
    dataToAppend = NULL;
#endif // RD_MULTIPLE_DRIVE

    // last refreshed
    TInt64 lastRefreshed( 0 );
    if (aMedia.IsSupported(KMPXMediaColDetailLastRefreshed))
        {
        lastRefreshed = aMedia.ValueTObjectL<TInt64>(KMPXMediaColDetailLastRefreshed);
        }
    TTime time(lastRefreshed);
    dataToAppend = HBufC::NewLC(
        KMPXMaxTimeLength + KMPXDurationDisplayResvLen);
    HBufC* format = StringLoader::LoadLC(R_QTN_DATE_USUAL_WITH_ZERO);
    TPtr modDatePtr = dataToAppend->Des();
    time.FormatL(modDatePtr, *format);
    CleanupStack::PopAndDestroy(format);
    format = NULL;
    AknTextUtils::LanguageSpecificNumberConversion( modDatePtr ); 
    dataArray->AppendL(modDatePtr);
    CleanupStack::PopAndDestroy(dataToAppend);
    dataToAppend = NULL;

    // Retrieve heading array
    CDesCArrayFlat* headingsArray = NULL;
    headingsArray = iCoeEnv->ReadDesCArrayResourceL(
            R_MPX_PODCAST_CUI_COLLECTION_DETAILS_HEADINGS);
    CleanupStack::PushL(headingsArray);

    // Item array combines heading array and data array
    CDesCArrayFlat* itemArray =
        new (ELeave) CDesCArrayFlat(headingsArray->Count());
    CleanupStack::PushL(itemArray);

    for (TInt i = 0; i < headingsArray->Count(); i++)
        {
        HBufC *item = HBufC::NewLC(
            headingsArray->MdcaPoint(i).Length() +
            dataArray->MdcaPoint(i).Length() +
            KMPXDurationDisplayResvLen);

        TPtrC tempPtr1 = headingsArray->MdcaPoint(i);
        TPtrC tempPtr2 = dataArray->MdcaPoint(i);

        item->Des().Format(KMPXCollDetailsItemsFormat,
                            &tempPtr1,
                            &tempPtr2);

        itemArray->AppendL(*item);
        CleanupStack::PopAndDestroy(item);
        }

    CleanupStack::Pop(itemArray);
    CleanupStack::PopAndDestroy(headingsArray);
    CleanupStack::PopAndDestroy(dataArray);

    // Set list items
    CTextListBoxModel* model = list->Model();
    model->SetOwnershipType(ELbmOwnsItemArray);
    model->SetItemTextArray(itemArray);

    // Set title
    HBufC* title =
        StringLoader::LoadLC(R_MPX_QTN_NMP_TITLE_COLLECTION_DETAILS);
    popupList->SetTitleL(*title);
    CleanupStack::PopAndDestroy(title);

    // Show popup list
    CleanupStack::Pop(popupList);
    popupList->ExecuteLD();
    CleanupStack::PopAndDestroy(list);
    }

//#ifdef __UPNP_FRAMEWORK_2_0_
// ---------------------------------------------------------------------------
// Handles Upnp menus from DynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleDynInitUpnpL(
    TInt aResourceId,
    CEikMenuPane& aMenuPane )
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleDynInitUpnpL()");

    if ( !IsUpnpVisibleL() )
        {
        GetCurrentPlayerDetails();

        if ( iCurrentPlayerType == EPbLocal )
            {
            aMenuPane.SetItemDimmed(
                EMPXCmdUpnpPlayVia,
                ETrue);
            }
        aMenuPane.SetItemDimmed(
            EMPXCmdUPnPAiwCmdCopyToExternalCriteria,
            ETrue);
        }
    else
        {
        MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleDynInitUpnpL(): UPnP visible and media");
        TInt currentItem( iContainer->Common()->CurrentLbxItemIndex() );

        const CMPXMedia& media =
            (iContainer->Common()->ListBoxArray())->MediaL( currentItem );
        TMPXGeneralType type( EMPXNoType );
        if (media.IsSupported(KMPXMediaGeneralType))
            {
            type =
                media.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);
            }

        if ( ( type == EMPXItem ) ||
             ( type == EMPXGroup ) )
            {
            MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleDynInitUpnpL(): folder or song");
            if (iErrorAttachCopyMenu != KErrNone)
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleDynInitUpnpL(): error, Dim Copy");
                aMenuPane.SetItemDimmed(
                    EMPXCmdUPnPAiwCmdCopyToExternalCriteria,
                    ETrue);
                }
            else
                {
                if (iServiceHandler->HandleSubmenuL(aMenuPane))
                    {
                    return;
                    }

                if (iServiceHandler->IsAiwMenu(aResourceId))
                    {
                    MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleDynInitUpnpL(): Aiw menu");
                    CAiwGenericParamList& in = iServiceHandler->InParamListL();

                    iServiceHandler->InitializeMenuPaneL(aMenuPane, aResourceId, EMPXCmdUPnPAiwCmdCopyToExternalCriteria, in);
                    MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleDynInitUpnpL(): after InitializeMenuPaneL");
                    }
                }
            }
         else
            {
            aMenuPane.SetItemDimmed(
                EMPXCmdUpnpPlayVia,
                ETrue);
            aMenuPane.SetItemDimmed(
                EMPXCmdUPnPAiwCmdCopyToExternalCriteria,
                ETrue);
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::IsUpnpVisible
// Checks if UPnP access point is defined
// -----------------------------------------------------------------------------
//
TBool CMPXPodcastCollectionViewImp::IsUpnpVisibleL()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewImp::IsUpnpVisibleL" );
    
    TBool returnValue = EFalse;
#ifdef UPNP_INCLUDED
   	if ( iUpnpCopyCommand && iUpnpFrameworkSupport ) 	  
        {
        returnValue = iUpnpCopyCommand->IsAvailableL();   
        }
#endif
    return returnValue; 
    
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::AddPlayersNamesToMenuL
// Taken from Gallery upnp support implementation
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::AddPlayersNamesToMenuL(CEikMenuPane& aMenuPane)
    {
    RArray<TMPXPlaybackPlayerType> playerTypes;
    CleanupClosePushL(playerTypes);

    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
    manager.GetPlayerTypesL(playerTypes);

    GetCurrentPlayerDetails();

    TInt countPlayersTypes( playerTypes.Count() );
    TInt countLocalType(0);
    TInt countRemoteType(0);

    if ( countPlayersTypes >= 1 )
        {
        iPlayersList.Close();

        for ( TInt i = 0; i < countPlayersTypes; i++ )
            {
            MPX_DEBUG3( "playerTypes[%d]: %d", i, playerTypes[i] );
            switch (playerTypes[i])
                {
                case EPbLocal:
                    {
                    // we only show 1 specific type once in the menu
                    if ( countLocalType == 0 )
                        {
                        AddPlayerNameToMenuL( aMenuPane,
                                            EMPXCmdUpnpPlayViaLocal,
                                            manager,
                                            playerTypes[i] );

                        countLocalType++;
                        }
                    break;
                    }
                case EPbRemote:
                    {
                    // we only show 1 specific type once in the menu
                    if ( countRemoteType == 0 )
                        {
                        AddPlayerNameToMenuL( aMenuPane,
                                            EMPXCmdUpnpPlayViaRemotePlayer,
                                            manager,
                                            playerTypes[i] );
                        countRemoteType++;
                        }
                    break;
                    }
                default:
                    {
                    // default case is handled in the next loop, we want
                    // to add any "other" player at the end of the list
                    break;
                    }
                }
            } // for loop

        for ( TInt i = 0; i < countPlayersTypes; i++ )
            {
            MPX_DEBUG3( "playerTypes[%d]: %d", i, playerTypes[i] );
            // EPbLocal and EPbRemote were already added to the
            // submenu in the previous loop
            if ( playerTypes[i] != EPbLocal &&
                 playerTypes[i] != EPbRemote )
                {
                // EPbUnknown or else
                TBool alreadyInTheList( EFalse );

                for ( TInt j = 0; j < i; j++)
                    {
                    // if the new playertype is already in the list
                    if ( playerTypes[i] == playerTypes[j] )
                        {
                        alreadyInTheList = ETrue;
                        break;
                        }
                    }

                if ( !alreadyInTheList )
                    {
                    HBufC* buf = manager.PlayerTypeDisplayNameL(playerTypes[i]);

                    if ( buf )
                        {
                        CleanupStack::PushL(buf);
                        AddPlayerNameToMenuL( aMenuPane,
                                            EMPXCmdUpnpPlayViaRemotePlayer,
                                            manager,
                                            playerTypes[i],
                                            *buf );
                        CleanupStack::PopAndDestroy(buf);
                        }
                    // else we don't do anything. other player type is not
                    // currently supported. Implementation will be finalized
                    // when the requirement is defined.
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(&playerTypes);

    if ( iPlayersList.Count() <= 1 )
        {
        aMenuPane.SetItemDimmed( EMPXCmdUpnpPlayViaRemotePlayer,
                                  ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::AddPlayerNameToMenuL
// Taken from Gallery upnp support implementation
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::AddPlayerNameToMenuL(CEikMenuPane& aMenuPane,
                                                TInt aCommandId,
                                                MMPXPlayerManager& aPlayerManager,
                                                TMPXPlaybackPlayerType& aPlayerType,
                                                const TDesC& aMenuText)
    {
    RArray<TUid> players;
    CleanupClosePushL(players);
    aPlayerManager.GetPlayerListL(players, aPlayerType);

    // For now we only keep the first player we find
    // Local player is always the first one in the list
    // Ui spec limitation
    MPX_DEBUG2( "players[0]: %d", players[0].iUid );
    if ( aCommandId == EMPXCmdUpnpPlayViaLocal )
        {
        iPlayersList.Insert(players[0], 0);
        }
    else if ( ( aCommandId == EMPXCmdUpnpPlayViaRemotePlayer ) &&
              (iPlayersList.Count() > 1) )
        {
        iPlayersList.Insert(players[0], 1);
        }
    else
        {
        iPlayersList.AppendL(players[0]);
        }
    CleanupStack::PopAndDestroy(&players);

    // if the player is of unknown type
    if ( ( aCommandId != EMPXCmdUpnpPlayViaRemotePlayer ) &&
         ( aCommandId != EMPXCmdUpnpPlayViaLocal ) )
        {
        CEikMenuPaneItem::SData menuItem;
        menuItem.iText.Copy( aMenuText );
        menuItem.iCascadeId = 0;
        menuItem.iFlags = EEikMenuItemRadioEnd;

        menuItem.iCommandId = aCommandId + (iPlayersList.Count() - 1);
        aCommandId = menuItem.iCommandId;

        aMenuPane.AddMenuItemL( menuItem );
        }

    if ( iCurrentPlayerType == aPlayerType )
        {
        aMenuPane.SetItemButtonState( aCommandId, EEikMenuItemSymbolOn );
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::GetCurrentPlayerDetails
// Retrieves the current player name and type
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::GetCurrentPlayerDetails()
    {
    MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
    TUid currentlyUsedPlayer;
    TInt currentlyUsedSubPlayer;
    iCurrentPlayerType = EPbLocal;
    delete iSubPlayerName;
    iSubPlayerName = NULL;
    TRAP_IGNORE( manager.GetSelectionL( iCurrentPlayerType,
                                        currentlyUsedPlayer,
                                        currentlyUsedSubPlayer,
                                        iSubPlayerName));
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::SelectNewPlayerL
// Selects a new player for audio playback
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::SelectNewPlayerL(TInt aCommand)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::SelectNewPlayerL()");
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::SelectNewPlayerL(%d)", aCommand);

    TInt errorSelectPlayer( KErrCancel );

    switch (aCommand)
        {
        case EMPXCmdUpnpPlayViaLocal:
            {
            // if Local is not already the current player, select it
            if ( iCurrentPlayerType != EPbLocal )
                {
                MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
                MPX_TRAP( errorSelectPlayer, manager.ClearSelectPlayersL() );
                }
            else // no need to reselect it but always start playback
                {
                errorSelectPlayer = KErrNone;
                }
            break;
            }
        default:
            {
            if ( aCommand >= EMPXCmdUpnpPlayViaRemotePlayer )
                {
                HBufC* buf = HBufC::NewLC( KMaxUidName );
                buf->Des().AppendNum(
                     (iPlayersList[aCommand -
                                   EMPXCmdUpnpPlayViaLocal]).iUid );

                // ActivateViewL leaves with KErrCancel if the dialog is
                // cancelled
                // ActivateViewL leaves with KErrInUse if the selected player
                // is already the active player
                MPX_TRAP(errorSelectPlayer, iViewUtility->ActivateViewL(
                                TUid::Uid( KMPXPluginTypeUPnPBrowseDialogUid ),
                                buf ));
                CleanupStack::PopAndDestroy( buf );
                }
            break;
            }
        }

    GetCurrentPlayerDetails();

    // If InUse, the player selected is already the current one, we start playback anyway
    // contrary to Playback view
    if ( ( errorSelectPlayer == KErrNone ) ||
         ( errorSelectPlayer == KErrInUse ) )
        {
        // Starts playback by calling OpenL() for the selected track
        CMPXCommonListBoxArrayBase* array = iContainer->Common()->ListBoxArray();
        if ( array )
            {
            MPX_DEBUG1("CMPXPodcastCollectionViewImp::SelectNewPlayerL() before MediaL()");

            TInt currentListBoxItemIndex(
                iContainer->Common()->CurrentLbxItemIndex() );

            if ( currentListBoxItemIndex >= 0 )
                {
                const CMPXMedia& media = array->MediaL( currentListBoxItemIndex );

                TMPXGeneralType type( EMPXNoType );
                TMPXGeneralCategory category( EMPXNoCategory );

                if ( media.IsSupported( KMPXMediaGeneralType ) )
                    {
                    type = media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
                    }

                if ( media.IsSupported( KMPXMediaGeneralCategory ) )
                    {
                    category = media.ValueTObjectL<TMPXGeneralCategory>(
                                    KMPXMediaGeneralCategory );
                    }

                // Needed to reset the status of iPreservedState
                iPlaybackUtility->CommandL(EPbCmdResetPreserveState);
                RArray<TMPXAttribute> attrs;
                CleanupClosePushL(attrs);
				// Append the signal to set IsPlaying flag
                MPX_DEBUG2("CMPXPodcastCollectionViewImp::SelectNewPlayerL() currentIndex %d", currentListBoxItemIndex );

                if ((type == EMPXItem) && (category == EMPXPodcast))
                    {
                    MPX_DEBUG1("CMPXPodcastCollectionViewImp::SelectNewPlayerL() type is EMPXItem");
                    iCollectionUtility->Collection().OpenL( currentListBoxItemIndex, attrs.Array() );
                    }
                else // if a folder is selected
                    {
                    MPX_DEBUG1("CMPXPodcastCollectionViewImp::SelectNewPlayerL() type is EMPXGroup");
                    iCollectionUtility->Collection().OpenL( currentListBoxItemIndex,
                                                            EMPXOpenPlaylistOnly);
                    }
                CleanupStack::PopAndDestroy(&attrs);
                }
            }
        }
    else if ( errorSelectPlayer != KErrCancel )
        {
        User::LeaveIfError(errorSelectPlayer);
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::CopySelectedItemsToRemoteL
// Copies selected file(s) to remote player
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::CopySelectedItemsToRemoteL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::CopySelectedItemsToRemoteL()");

    TInt index( iContainer->Common()->CurrentLbxItemIndex() );
    if ( index >= 0 )
        {
        CMPXCommonListBoxArrayBase* array =
            iContainer->Common()->ListBoxArray();
        const CMPXMedia& media = array->MediaL( index );

        TMPXGeneralType type( EMPXNoType );
        TMPXGeneralCategory category( EMPXNoCategory );

       if ( media.IsSupported( KMPXMediaGeneralType ) )
            {
            type =
                media.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
            }
        if ( media.IsSupported( KMPXMediaGeneralCategory ) )
            {
            category =
                media.ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
            }

        CMPXCollectionPath* cpath =
                              iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( cpath );

        if ( ( type == EMPXItem ) && ( category == EMPXPodcast ) )
            {
            const CArrayFix<TInt>* arrayIndex =
                iContainer->Common()->CurrentSelectionIndicesL();
            TInt arrayCount( arrayIndex->Count() );

            cpath->ClearSelection();
            if ( arrayCount > 0 )
                {
                for (TInt index=0; index < arrayCount; index++)
                    {
                    cpath->SelectL(arrayIndex->At(index));
                    }
                }
            else
                {
                cpath->Set( index );
                }

            // Ask for the list of selected song paths: will return in HandleMedia()
            ASSERT( iCurrentMediaLOp == EMPXOpMediaLIdle );
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append( KMPXMediaGeneralUri );
            iCollectionUtility->Collection().MediaL( *cpath, attrs.Array() );
            iCurrentMediaLOp = EMPXOpMediaLCopyToRemote;
            CleanupStack::PopAndDestroy( &attrs );
            }
        else // if a folder is selected
            {
            TMPXItemId id(0);
            if ( media.IsSupported( KMPXMediaGeneralId ) )
                {
                id = media.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                }

            RArray<TInt> supportedIds;
            CleanupClosePushL(supportedIds);
            supportedIds.AppendL(KMPXMediaIdGeneral);

            CMPXMedia* entry = CMPXMedia::NewL(supportedIds.Array());
            CleanupStack::PushL(entry);
            entry->SetTObjectValueL( KMPXMediaGeneralType, EMPXGroup );

            entry->SetTObjectValueL( KMPXMediaGeneralCategory, EMPXPodcast );

            entry->SetTObjectValueL( KMPXMediaGeneralId, id );

            TInt depth( cpath->Levels() );
            // last level, we need to specify the container Id for the search
            if(depth > KMPXLastDirectoryLevel)
                {
                const TMPXItemId& containerId = cpath->Id(depth-1);
                entry->SetTObjectValueL( KMPXMediaGeneralContainerId,
                                            containerId);
                }

            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append( KMPXMediaGeneralUri );

            ASSERT( iCurrentFindAllLOp == EMPXOpFindAllLIdle );
            iCollectionUtility->Collection().FindAllL( *entry,
                                                       attrs.Array(),
                                                       *this );
            iCurrentFindAllLOp = EMPXOpFindAllLUpnp;

            CleanupStack::PopAndDestroy( &attrs );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( &supportedIds );
            }
        CleanupStack::PopAndDestroy( cpath );
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::DoHandleCopyToRemoteL
// Handle call back from collectionframework for Copy to Remote command
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoHandleCopyToRemoteL(
    const CMPXMedia& aMedia, TBool aComplete )
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DoHandleCopyToRemoteL()");

    CMPXMediaArray* mediaArray( NULL );
    TInt fileCount( 0 );

    if ( aMedia.IsSupported( KMPXMediaArrayContents ) )
        {
        mediaArray =
            aMedia.Value<CMPXMediaArray>( KMPXMediaArrayContents );
        if ( mediaArray )
            {
            fileCount = mediaArray->Count();
            }
        }
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::DoHandleCopyToRemoteL Entry count = %d", fileCount);

    // Change this to Global will be needed if FindAll() is called
    // repeatedly until aComplete is ETrue (Inc9?)
    CAiwGenericParamList* aiwCopyInParamList = CAiwGenericParamList::NewLC();

    // multiple files or folder
    if ( fileCount > 0 )
        {
        if ( mediaArray )
            {
            for ( TInt i = 0; i < fileCount; i++ )
                {
                CMPXMedia* media( mediaArray->AtL( i ) );

                if ( media->IsSupported( KMPXMediaGeneralUri ) )
                    {
                    const TDesC& location =
                        media->ValueText( KMPXMediaGeneralUri );
                        MPX_DEBUG3( "CMPXPodcastCollectionViewImp::DoHandleCopyToRemote %d filePath = %S", i, &location );

                    TAiwVariant path( location );
                    TAiwGenericParam fileParameter( EGenericParamFile,
                                                    path );
                    aiwCopyInParamList->AppendL( fileParameter );
                    }
                }
            }
        }
    else // single file
        {
        TMPXGeneralType type( EMPXNoType );

        if ( aMedia.IsSupported( KMPXMediaGeneralType ) )
            {
            type = aMedia.ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
            }

        if ( type == EMPXItem )
            {
            if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& location =
                    aMedia.ValueText( KMPXMediaGeneralUri );
                TAiwVariant path( location );
                MPX_DEBUG2( "CMPXPodcastCollectionViewImp::DoHandleCopyToRemote filePath = %S", &location );

                TAiwGenericParam fileParameter( EGenericParamFile, path );
                aiwCopyInParamList->AppendL( fileParameter );
                }
            }
        }

    if ( ( aiwCopyInParamList->Count() > 0 ) &&
         aComplete )
        {
        CAiwGenericParamList* outParamList = CAiwGenericParamList::NewLC();
        // do not allow the copy dialog to be dismissed by external command
        HandleCommandL( EMPXCmdIgnoreExternalCommand );
        iServiceHandler->ExecuteMenuCmdL(
                  EMPXCmdUPnPAiwCmdCopyToExternalCriteria, /* command id */
                  *aiwCopyInParamList,   /* in parameters */
                  *outParamList,          /* out parameters */
                  0,                      /* options */
                  NULL );                 /* call back */
        HandleCommandL( EMPXCmdHandleExternalCommand );

        CleanupStack::PopAndDestroy( outParamList );
        }

    // Clear selection
    if( iContainer )
        {
        iContainer->Common()->ClearLbxSelection();
        }

    CleanupStack::PopAndDestroy( aiwCopyInParamList );
    }
//#endif //__UPNP_FRAMEWORK_2_0_

// -----------------------------------------------------------------------------
// checks if send option should be shown the option is selected.
// -----------------------------------------------------------------------------
//
TBool CMPXPodcastCollectionViewImp::SendOptionVisibilityL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::SendOptionVisibilityL");
    TBool isHidden( iContainer->Common()->TotalListItemCount() < 1 );

    if ( !isHidden )
        {
        CMPXCommonListBoxArrayBase* baseArray =
            iContainer->Common()->ListBoxArray();
        CMPXPodcastCollectionViewListBoxArray* array =
            static_cast<CMPXPodcastCollectionViewListBoxArray*>( baseArray );
        const CArrayFix<TInt>* selectedIndexArray =
            iContainer->Common()->CurrentSelectionIndicesL();
        TInt selectionCount( selectedIndexArray->Count() );

        if ( selectionCount > 0 )
            {
            TBool isValid( EFalse );
            for ( TInt i = 0; i < selectionCount && !isValid; i++ )
                {
                // multiple selection
                // corrupted song can be sent in this case
                isValid = !array->IsItemBrokenLinkL(
                    selectedIndexArray->At( i ) ) &&
                    !array->IsItemCorruptedL(
                    selectedIndexArray->At( i ) );
                }
            // if anything is valid, display the item
            isHidden = !isValid;
            }
        else
            {
            //single selection
            TInt currentItem( iContainer->Common()->CurrentLbxItemIndex() );
            isHidden = array->IsItemBrokenLinkL( currentItem ) ||
                array->IsItemCorruptedL( currentItem );
            }
        }

    return isHidden;
    }

// -----------------------------------------------------------------------------
// Checks if file details option should be shown
// -----------------------------------------------------------------------------
//
TBool CMPXPodcastCollectionViewImp::FileDetailsOptionVisibilityL()
    {
    MPX_FUNC( "CMPXPodcastCollectionViewImp::FileDetailsOptionVisibilityL" );
    TBool isHidden( iContainer->Common()->TotalListItemCount() < 1 ||
        iContainer->Common()->CurrentSelectionIndicesL()->Count() );
    if ( !isHidden )
        {
        CMPXPodcastCollectionViewListBoxArray* array =
            static_cast<CMPXPodcastCollectionViewListBoxArray*>(
            iContainer->Common()->ListBoxArray() );
        const CMPXMedia& media =
            array->MediaL( iContainer->Common()->CurrentLbxItemIndex() );

        if ( media.IsSupported( KMPXMediaGeneralFlags ) )
            {
            TUint flags( media.ValueTObjectL<TUint>( KMPXMediaGeneralFlags ) );
            if ( ( flags ) & ( KMPXMediaGeneralFlagsIsMissingDetails ) )
                {
                isHidden = ETrue;
                }
            }
        }
    return isHidden;
    }
// -----------------------------------------------------------------------------
// Handle send command.
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoSendL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DoSendL");
    iCurrentMediaLOp = EMPXOpMediaLSend;
    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL(path);

    // Marked indicies
    const CArrayFix<TInt>* array =
        iContainer->Common()->CurrentSelectionIndicesL();
    TInt arrayCount = array->Count();
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::DoSendL array count = %d", arrayCount);
    if (arrayCount > 1)
        {
        // if more than 1 selected
        for (TInt i = 0; i < arrayCount; i++)
            {
            MPX_DEBUG2("CMPXCollectionViewImp::DoSendL array index = %d", array->At(i));
            path->SelectL(array->At(i));
            }
        }
    else if (arrayCount == 1)
        {
        // if only 1 item is marked, treat it as single selection
        path->Set(array->At(0));
        }
    else
        {
        path->Set(iContainer->Common()->CurrentLbxItemIndex());
        }


    RArray<TMPXAttribute> attrs;
    CleanupClosePushL(attrs);
    attrs.Append(
        TMPXAttribute(KMPXMediaIdGeneral,
            EMPXMediaGeneralUri |
            EMPXMediaGeneralSize |
            EMPXMediaGeneralCollectionId |
            EMPXMediaGeneralFlags |
            EMPXMediaGeneralId |
            EMPXMediaGeneralType |
            EMPXMediaGeneralCategory));
    iCollectionUtility->Collection().MediaL(*path, attrs.Array());
    CleanupStack::PopAndDestroy(&attrs);
    CleanupStack::PopAndDestroy(path);
    }


// -----------------------------------------------------------------------------
// Handle call back from collectionframework for send command
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoHandleSendL(
    const CMPXMedia& aMedia, TInt aError)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DoHandleSendL");
    if (aError == KErrNone)
        {
        TInt invalidFileCount(0);
        TInt corruptedFileCount(0);
        TInt maxSize(0);
        CMessageData* messageData = CMessageData::NewLC();

        TInt fileCount(0);
        if(aMedia.IsSupported(KMPXMediaArrayContents))
            {
            const CMPXMediaArray* mediaArray =
                aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
            User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));
            
            fileCount = mediaArray->Count();
            }
        MPX_DEBUG2("CMPXCollectionViewImp::DoHandleSendL Entry count = %d", fileCount);

        if (fileCount > 0)
            {
            const CMPXMediaArray* mediaArray =
                aMedia.Value<CMPXMediaArray>(KMPXMediaArrayContents);
            User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));
            
            for (TInt i = 0; i < fileCount; i++)
                {
                CMPXMedia* media( mediaArray->AtL( i ) );
                TUint flags = 0;

                if (aMedia.IsSupported(KMPXMediaGeneralFlags))
                    {
                    flags = media->ValueTObjectL<TUint>(KMPXMediaGeneralFlags);
                    }

                TUint isCorrupted( ( flags ) & ( KMPXMediaGeneralFlagsIsCorrupted ) );
                TUint isInvalid( ( flags ) & ( KMPXMediaGeneralFlagsIsInvalid ) );
                const TDesC& location = media->ValueText(KMPXMediaGeneralUri);

                if (!isCorrupted && !isInvalid && ConeUtils::FileExists(location))
                    {
                    TInt fileSize = 0;
                    if (isCorrupted)
                        {
                        corruptedFileCount++;
                        }
                    if (aMedia.IsSupported(KMPXMediaGeneralSize))
                        {
                        fileSize = media->ValueTObjectL<TInt>(KMPXMediaGeneralSize);
                        }
                    else
                        {
                        RFs& fileSession = iCoeEnv->FsSession();
                        TEntry pl;
                        fileSession.Entry(location, pl);
                        fileSize = pl.iSize;
                        }

                    if (maxSize < fileSize)
                        {
                        maxSize = fileSize;
                        }
                    messageData->AppendAttachmentL(location);
                    }
                else
                    {
                    UpdateDatabaseFlagL(KErrNotFound, *media,
                        KMPXMediaGeneralFlagsIsInvalid, ETrue, EFalse);
                    invalidFileCount++;
                    }
                }
            }
        else
            {
            // single file
            fileCount = 1;
            TUint flags = 0;

            if (aMedia.IsSupported(KMPXMediaGeneralFlags))
                {
                flags = aMedia.ValueTObjectL<TUint>(KMPXMediaGeneralFlags);
                }

            TUint isCorrupted( ( flags ) & ( KMPXMediaGeneralFlagsIsCorrupted ) );
            TUint isInvalid( ( flags ) & ( KMPXMediaGeneralFlagsIsInvalid ) );
            const TDesC& location = aMedia.ValueText(KMPXMediaGeneralUri);

            if (!isInvalid && ConeUtils::FileExists(location))
                {
                TInt fileSize(0);
                if (isCorrupted)
                    {
                    corruptedFileCount++;
                    }
                if (aMedia.IsSupported(KMPXMediaGeneralSize))
                    {
                    fileSize = aMedia.ValueTObjectL<TInt>(KMPXMediaGeneralSize);
                    }
                else
                    {
                    RFs& fileSession = iCoeEnv->FsSession();
                    TEntry pl;
                    fileSession.Entry(location, pl);
                    fileSize = pl.iSize;
                    }

                if (maxSize < fileSize)
                    {
                    maxSize = fileSize;
                    }
                messageData->AppendAttachmentL(location);
                }
            else
                {
                UpdateDatabaseFlagL(KErrNotFound, aMedia,
                    KMPXMediaGeneralFlagsIsInvalid, ETrue, EFalse);
                invalidFileCount++;
                }
            }

        TBool confirmOk = EFalse;
        MPX_DEBUG4("CMPXCollectionViewImp::DoHandleSendL Invalid Count = %d, Corrupted Count = %d, Total Count = %d", invalidFileCount, corruptedFileCount, fileCount);
        if (invalidFileCount || corruptedFileCount)
            {
            if (invalidFileCount == fileCount)
                {
                // all files are invalid
                if (invalidFileCount == 1)
                    {
                    iCommonUiHelper->DisplayInfoNoteL(
                        R_MPX_COLLECTION_INFO_FILE_NOT_FOUND);
                    }
                else
                    {
                    // Show information note if all files are invalid
                    iCommonUiHelper->DisplayInfoNoteL(
                        R_MPX_COLLECTION_SEND_ALL_INVALID_SONGS_NOTE_TXT);
                    }
                }
            else
                {
                // Show confirmation query if invalid/corrupted file found
                HBufC* queryTxt = StringLoader::LoadLC(
                    R_MPX_COLLECTION_QUERY_SEND_INVALID_SONGS_TXT);
                CAknQueryDialog* query = CAknQueryDialog::NewL(
                    CAknQueryDialog::EConfirmationTone);
//#ifdef __COVER_DISPLAY
                CleanupStack::PushL(query);
                query->PublishDialogL(EMPlayerNoteSendInvalidSongs,
                                    KMPlayerNoteCategory);
                CleanupStack::Pop(query);
//#endif  //__COVER_DISPLAY
                if (query->ExecuteLD(
                    R_MPX_COLLECTION_GENERIC_CONFIRMATION_QUERY,
                    *queryTxt))
                    {
                    confirmOk = ETrue;
                    }
                CleanupStack::PopAndDestroy(queryTxt);
                }
            }

        if ((!invalidFileCount && !corruptedFileCount) || // everything is ok
            (invalidFileCount && confirmOk) || // invalid files exist
            (corruptedFileCount && confirmOk)) // courrupt files exist
            {
            if (!iSendUi)
                {
                iSendUi = CSendUi::NewL();
                }
            TSendingCapabilities capabilities(
                0,
                maxSize,
                TSendingCapabilities::ESupportsAttachments);
            HandleCommandL( EMPXCmdIgnoreExternalCommand );
            MPX_TRAPD(err, iSendUi->ShowQueryAndSendL(messageData, capabilities));
            if ( err != KErrNone )
                {
                HandleErrorL( err );
                }
            HandleCommandL( EMPXCmdHandleExternalCommand );
            }

        // Clear selection
        if (iContainer)
            {
            iContainer->Common()->ClearLbxSelection();
            }
        CleanupStack::PopAndDestroy(messageData);
        }
    else
        {
        HandleErrorL(aError);
        }
    }


// -----------------------------------------------------------------------------
// Prepares media object for selected items
// -----------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewImp::PrepareMediaForSelectedItemsL(CMPXMedia* aMedia)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::PrepareMediaForSelectedItemsL");
    TInt ret(KErrNone);

    if (aMedia)
        {
        CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
        CleanupStack::PushL(mediaArray);
        const CArrayFix<TInt>* array =
            iContainer->Common()->CurrentSelectionIndicesL();
        CMPXCommonListBoxArrayBase* listBoxArray =
            iContainer->Common()->ListBoxArray();
        TInt count = array->Count();
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL(path);
        TMPXItemId id = path->Id(0);
        MPX_DEBUG2("CMPXCollectionViewImp::PrepareMediaForSelectedItemsL collection ID = 0x%x", id.iId1);
        CleanupStack::PopAndDestroy(path);
        if (count > 0)
            {
            for (TInt i = 0; i < count; i++)
                {
                const CMPXMedia& listBoxMedia = listBoxArray->MediaL(array->At(i));
                CMPXMedia* entry = CMPXMedia::NewL();
                CleanupStack::PushL(entry);
                entry->SetTextValueL(KMPXMediaGeneralTitle,
                    listBoxMedia.ValueText(KMPXMediaGeneralTitle));
                entry->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
                entry->SetTObjectValueL(KMPXMediaGeneralCategory,
                    EMPXSong);

                if (listBoxMedia.IsSupported(KMPXMediaGeneralId))
                    {
                    entry->SetTObjectValueL(
                        KMPXMediaGeneralId,
                        listBoxMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                    }
                else
                    {
                    User::Leave(KErrArgument);
                    }

                entry->SetTObjectValueL(KMPXMediaGeneralCollectionId, id);

                mediaArray->AppendL(entry);
                CleanupStack::Pop(entry);
                }
            }
        else
            {
            count = 1;
            TInt currentItem = iContainer->Common()->CurrentLbxItemIndex();
            const CMPXMedia& listBoxMedia = listBoxArray->MediaL(currentItem);
            CMPXMedia* entry = CMPXMedia::NewL();
            CleanupStack::PushL(entry);
            entry->SetTextValueL(
                KMPXMediaGeneralTitle,
                listBoxMedia.ValueText(
                    KMPXMediaGeneralTitle));
            entry->SetTObjectValueL(KMPXMediaGeneralType, EMPXItem);
            entry->SetTObjectValueL(KMPXMediaGeneralCategory, EMPXSong);

            if (listBoxMedia.IsSupported(KMPXMediaGeneralId))
                {
                entry->SetTObjectValueL(KMPXMediaGeneralId,
                    listBoxMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
                }
            else
                {
                User::Leave(KErrArgument);
                }

            entry->SetTObjectValueL(KMPXMediaGeneralCollectionId, id);

            mediaArray->AppendL(entry);
            CleanupStack::Pop(entry);
            }
        aMedia->SetTObjectValueL(KMPXMediaGeneralCollectionId, id);

        aMedia->SetCObjectValueL(KMPXMediaArrayContents,
            mediaArray);
        aMedia->SetTObjectValueL(KMPXMediaArrayCount,
            mediaArray->Count());
        CleanupStack::PopAndDestroy(mediaArray);
        }
    else
        {
        ret = KErrArgument;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Set/clears the flags for item in database
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateDatabaseFlagL(TInt aIndex,
                                                       const CMPXMedia& aMedia,
                                                       TUint aFlag,
                                                       TBool aSet,
                                                       TBool aEnableInfoDialog)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdateDatabaseFlagL");
    CMPXCommonListBoxArrayBase* array = iContainer->Common()->ListBoxArray();
    if (array)
        {
        // set the item as invalid
        TUint flags(0);
        if (aSet)
            {
            flags = KMPXMediaGeneralFlagsSetOrUnsetBit;
            }
        flags |= aFlag;

        MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateDatabaseFlagL flag after 0x%x", flags);
        CMPXMedia* entry = CMPXMedia::NewL();
        CleanupStack::PushL(entry);
        if (aIndex > KErrNotFound)
            {
            MPX_DEBUG2("CMPXPodcastCollectionViewImp::UpdateDatabaseFlagL using aIndex = %d", aIndex);
            const CMPXMedia& media = array->MediaL(aIndex);

            TMPXItemId id( 0 );
            TMPXGeneralType type( EMPXNoType );
            TMPXGeneralCategory category( EMPXNoCategory );

            if (media.IsSupported(KMPXMediaGeneralId))
                {
                id = media.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
                }

            if (media.IsSupported(KMPXMediaGeneralType))
                {
                type =
                    media.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);
                }

            if (media.IsSupported(KMPXMediaGeneralCategory))
                {
                category =
                    media.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory);
                }

            entry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, id);
            entry->SetTObjectValueL<TMPXGeneralType>(
                KMPXMediaGeneralType,
                type);
            entry->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory,
                category);
            }
        else
            {
            // try to look for the info inside aMedia
            if (aMedia.IsSupported(KMPXMediaGeneralId))
                {
                TMPXItemId id( aMedia.ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId) );
                entry->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, id);
                }
            if (aMedia.IsSupported(KMPXMediaGeneralType))
                {
                TMPXGeneralType type(
                    aMedia.ValueTObjectL<TMPXGeneralType>(
                        KMPXMediaGeneralType) );
                entry->SetTObjectValueL<TMPXGeneralType>(
                    KMPXMediaGeneralType,
                    type);
                }
            if (aMedia.IsSupported(KMPXMediaGeneralCategory))
                {
                TMPXGeneralCategory category(
                    aMedia.ValueTObjectL<TMPXGeneralCategory>(KMPXMediaGeneralCategory) );
                entry->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory,
                    category);
                }
            }

        TUid collectionId = TUid::Uid(0);
        if (aMedia.IsSupported(KMPXMediaGeneralCollectionId))
            {
            collectionId = aMedia.ValueTObjectL<TUid>(KMPXMediaGeneralCollectionId);
            }
        else
            {
            User::Leave(KErrArgument);
            }

        entry->SetTObjectValueL<TUid>(KMPXMediaGeneralCollectionId,
            collectionId);
        entry->SetTObjectValueL<TUint>(KMPXMediaGeneralFlags, flags);
        DoSetCollectionL( entry, ETrue );
        CleanupStack::PopAndDestroy(entry);
        }
    if (aEnableInfoDialog)
        {
        if (KMPXMediaGeneralFlagsIsInvalid == aFlag)
            {
            iCommonUiHelper->DisplayInfoNoteL(R_MPX_COLLECTION_INFO_FILE_NOT_FOUND);
            }
        }
    }

// -----------------------------------------------------------------------------
// Handle playback message
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewImp::DoHandlePlaybackMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        switch ( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EActivePlayerChanged:
            case TMPXPlaybackMessage::EStateChanged:
                {
                MPX_DEBUG2("CMPXPodcastCollectionViewImp::DoHandlePlaybackMessageL - EStateChanged(%d)", type );
                UpdatePlaybackStatusL(ETrue);
                break;
                }
            case TMPXPlaybackMessage::EError:
                {
                if (iCommonUiHelper)
                    {
                    iCommonUiHelper->DismissWaitNoteL();
                    HandleCommandL( EMPXCmdHandleExternalCommand );
                    }
                if (iWaitDialog)
					{
                    iWaitDialog->ProcessFinishedL();
					}
                if (iIsDeleting)
                    {
                    iCollectionUiHelper->Cancel();
                    iIsDeleting = EFalse;
                    // if delete is interrupted, reopen
                    iCollectionUtility->Collection().OpenL();
                    }
                MPX_DEBUG2("CMPXCollectionViewImp::DoHandlePlaybackMessageL Error = %d", data );
                break;
                }
            case TMPXPlaybackMessage::EPlayerUnavailable:
                {
//#ifdef __UPNP_FRAMEWORK_2_0_
                if ( iUpnpFrameworkSupport )
                    {
                    if ( data == KErrNotFound)
                        {
                        if ( iViewUtility->ActiveViewType() ==
                            TUid::Uid( KMPXPluginTypeCollectionUid ) )
                            {
                            if ( iSubPlayerName )
                                {
                                HBufC* dialogText = StringLoader::LoadLC(
                                    R_MPX_COLLECTION_NOTE_REMOTE_CONNECTION_FAILED,
                                    *iSubPlayerName);
                                CAknErrorNote* errNote = new(ELeave) CAknErrorNote(ETrue);
                                errNote->SetTimeout(CAknNoteDialog::ELongTimeout);
                                errNote->ExecuteLD( *dialogText );
                                CleanupStack::PopAndDestroy( dialogText );
                                }
                            }
                        }
                    }
//#endif // __UPNP_FRAMEWORK_2_0_
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoHandleCollectionMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewImp::DoHandleCollectionMessageL" );
    TMPXMessageId id( aMessage.ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        TInt event( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralType ) );
        TInt data( aMessage.ValueTObjectL<TInt>( KMPXMessageGeneralData ) );
        MPX_DEBUG3( "CMPXPodcastCollectionViewImp::HandleCollectionMessage Event = %d, Type() = %d",
            event, type );

        // Only handle collection messages while collection view is in focus
        // and not transitioning views
        if( iContainer )
            {
            if ( event == TMPXCollectionMessage::EPathChanged &&
                type == EMcPathChangedByOpen &&
                data == EMcContainerOpened)
                { // Open new entries
                iCollectionUtility->Collection().OpenL();
                }
            else if ( event == TMPXCollectionMessage::EPathChanged &&
                     type == EMcPathChangedByCollectionChange)
                {
                if(!iIsDeleting)
                    {
                     // Re-Fetch entries only if we are in playlist
                    iCollectionUtility->Collection().OpenL();
                    }
                }
            else if ( event == TMPXCollectionMessage::EPathChanged &&
                     type == EMcPathChangedByOpen &&
                     data == EMcItemOpened)
                {
                // opened a song, revert back one level
                iBackOneLevel = ETrue;
                }
            else if( event == TMPXCollectionMessage::EBroadcastEvent &&
                     iViewUtility->ActiveViewType() != TUid::Uid(KMPXPluginTypeWaitNoteDialogUid))
                {
                MPX_DEBUG2( "CMPXPodcastCollectionViewImp::DoHandleCollectionMessageL - broadcast type = %d", type );
                if ( type == EMcMsgDiskRemoved )
                    {
                    AknDialogShutter::ShutDialogsL( *CEikonEnv::Static() );
                    }
                }
#ifdef __COVER_DISPLAY                          
            else if ( event == TMPXCollectionMessage::EFocusChanged )
                {
                if ( data != KErrNotFound &&
                    data < iContainer->Common()->CurrentListItemCount() &&
                    !iIgnoreNextFocusChangedMessage )
                    {
                    iContainer->Common()->SetLbxCurrentItemIndexAndDraw( data );
                    }
                iIgnoreNextFocusChangedMessage = EFalse;
                }
#endif // __COVER_DISPLAY                            
            // else ignore
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXAlbumArtEditorDialog::DoSetCollectionL
// Perform a set operation
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoSetCollectionL( CMPXMedia* aMedia, TBool aSync )
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DoSetCollectionL");
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, KMPXCommandIdCollectionSet );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, aSync );
    TUid colId( aMedia->ValueTObjectL<TUid>(KMPXMediaGeneralCollectionId) );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, aMedia );

    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }

#ifdef __ENABLE_MSK
// ---------------------------------------------------------------------------
// Updates the middle softkey display
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::UpdateMiddleSoftKeyDisplayL(TInt aMskId)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::UpdateMiddleSoftKeyDisplayL");
    if (iContainer)
        {
        CEikButtonGroupContainer* cba = Cba();
        if (cba)
            {
            if (!iContainer->Common()->CurrentListItemCount()) // list is empty
                {
                iCommonUiHelper->RemoveMiddleSoftKeyLabel(*cba);
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::UpdateMiddleSoftKeyDisplayL - remove label, list is empty");
                }
            else if (iContainer->Common()->CurrentSelectionIndicesL()->Count()) // marked items
			{
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(), 
                KAknsIIDQgnPropMskMenu, 
                KAknsIIDQsnComponentColors, 
                EAknsCIQsnComponentColorsCG13,
                bitmap, 
                mask,
                AknIconUtils::AvkonIconFileName(),
                EMbmAvkonQgn_prop_msk_menu, 
                EMbmAvkonQgn_prop_msk_menu_mask, 
                KRgbBlack); 

            CleanupStack::PushL(bitmap);
            CleanupStack::PushL(mask);
        
            // set middle softkey icon 
            iCommonUiHelper->SetMiddleSoftKeyIconL(*cba, bitmap, mask);
        
            CleanupStack::PopAndDestroy(2, bitmap);
            MPX_DEBUG1("CMPXPodcastCollectionViewImp::UpdateMiddleSoftKeyDisplayL - items marked, show icon");
			}
            else
                {
                iCommonUiHelper->SetMiddleSoftKeyLabelL(
                    *cba,
                    aMskId,
                    EAknSoftkeyForwardKeyEvent);
                iCurrentMskId = aMskId;
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::UpdateMiddleSoftKeyDisplayL - new label displayed");
                }
            cba->DrawDeferred();
            }
        }
    }
#endif // __ENABLE_MSK

// ---------------------------------------------------------------------------
// From MProgressDialogCallback
// Callback method. Get's called when a dialog is dismissed
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DialogDismissedL(TInt aButtonId)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DialogDismissedL");
    switch (aButtonId)
        {
        case EAknSoftkeyCancel:
            {
            iIsWaitNoteCanceled = ETrue;
            if(iIsDeleting)
                {
                iCollectionUiHelper->Cancel();
                iIsDeleting = EFalse;
                // if delete is interrupted, reopen
                iCollectionUtility->Collection().OpenL();
                }

            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection message
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleCollectionMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandleCollectionMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d a folder
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleOpenL(
    const CMPXMedia& aEntries,
    TInt aIndex,
    TBool /*aComplete*/,
    TInt aError)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleOpenL 4");
    iHandlingKeyEvent = EFalse;

    if(aError == KErrNone)
        {
        if ( !iContainer )
            {
            // View is not active. Ignore.
            return;
            }
#ifdef __ENABLE_MSK
        TInt mskId(R_QTN_MSK_OPEN);
    TMPXPodcastCategory pCategory = aEntries.ValueTObjectL<TMPXPodcastCategory>(KMPXMediaPodcastCategoryGroup);
    TMPXPodcastType pType = aEntries.ValueTObjectL<TMPXPodcastType>(KMPXMediaPodcastType);
    if ((pCategory == EMPXAll) || (pCategory == EMPXRecentlyAdded) ||
        (pCategory == EMPXNotYetPlayed)||
        ((pCategory == EMPXEpisode) && (pType == EMPXPodcastItem)))
        {
		    mskId = R_QTN_MSK_PLAY;
		    }
#endif // __ENABLE_MSK
        UpdateOptionMenuL();
        MPX_DEBUG2("CMPXPodcastCollectionViewImp::HandleOpenL Number of Entries: %d", aEntries.Count());
        UpdateListBoxL(const_cast<CMPXMedia&>(aEntries), aIndex);

        delete iNumEpisode;
        iNumEpisode = NULL;

        if(aEntries.IsSupported(KMPXMediaGeneralCount))
            {
            CMPXMediaArray* mediaArray =
                const_cast<CMPXMediaArray*>(aEntries.Value<CMPXMediaArray>(KMPXMediaArrayContents));
            User::LeaveIfNull(const_cast<CMPXMediaArray*>(mediaArray));
             
            TInt number( mediaArray->Count() );
            HBufC* text( NULL );

            if (number == 1)
                {
                text = StringLoader::LoadLC(R_MPX_QTN_NMP_ONE_EPISODE);
                }
            else if (number > 1)
                {
                text = StringLoader::LoadLC(R_MPX_QTN_NMP_NUM_EPISODES, number);
                }
            // else ignore

            if ((mskId == R_QTN_MSK_PLAY) && (number <= 0))
                    {
                    mskId = 0;
                    }
            if (number >= 1)
                {
#ifdef __ENABLE_MSK
#endif // __ENABLE_MSK
                iNumEpisode = text->Alloc();
                CleanupStack::PopAndDestroy(text);
                }
            }

        UpdateNaviPaneL();

        if ( iContainer )
            {
            // this has to be done after the list box media is properly
            // updated
            if ( !iContainer->Common()->FindBoxVisibility() )
                {
                MPX_DEBUG1( "CMPXPodcastCollectionViewImp::HandleOpenL find box not visible" );

                CEikButtonGroupContainer* cba = Cba();
                if ( cba )
                    {
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
                    cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
#else
                    cba->SetCommandSetL(
                        ( iViewUtility->ViewHistoryDepth() == 1) ?
                        R_AVKON_SOFTKEYS_OPTIONS_EXIT : R_AVKON_SOFTKEYS_OPTIONS_BACK );

#endif

    #ifdef __ENABLE_MSK
                    if (mskId != 0)
                        {
                        UpdateMiddleSoftKeyDisplayL(mskId);
                        }
    #endif // __ENABLE_MSK

                    cba->DrawDeferred();
                    }
                }
            else
                {
                MPX_DEBUG1( "CMPXPodcastCollectionViewImp::HandleOpenL find box visible" );
                }

            CMPXCommonListBoxArrayBase* array(
                iContainer->Common()->ListBoxArray() );
            const CMPXMedia& containerMedia = array->ContainerMedia();

            delete iTitle;
            iTitle = NULL;

            if (containerMedia.IsSupported( KMPXMediaGeneralTitle ))
                {
                iTitle =
                    containerMedia.ValueText( KMPXMediaGeneralTitle ).AllocL();
                }
            UpdateTitlePaneL();
            }

        iHandleOpenProcessed = ETrue;
        }
    else
        {
		if ( iContainer )
            {
            CEikButtonGroupContainer* cba = Cba();
            if ( cba )
                {
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
                cba->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
#else
                cba->SetCommandSetL(
                    ( iViewUtility->ViewHistoryDepth() == 1 ) ?
                    R_AVKON_SOFTKEYS_OPTIONS_EXIT : R_AVKON_SOFTKEYS_OPTIONS_BACK );
#endif
                cba->DrawDeferred();
                }
            }

        HandleErrorL(aError);
        }

    if (iIsDeleting)
        {
        // nothing else to delete
        iIsDeleting = EFalse;
        if (iWaitDialog)
			{
            iWaitDialog->ProcessFinishedL();
			}
        HandleCommandL( EMPXCmdHandleExternalCommand );
        }
    MPX_PERF_CHECKPT("Collection View opened");
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d an item. Client typically responds by
// 'playing' the item
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleOpenL(
    const CMPXCollectionPlaylist& /*aPlaylist*/,
    TInt /*aError*/)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleOpenL 2");
    iHandlingKeyEvent = EFalse;
    // Do Nothing: playback/fetch client should handle this stage
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle media properties
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleCollectionMediaL(
    const CMPXMedia& aMedia,
    TInt aError)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleCollectionMediaL");
    if (aError == KErrNone)
        {
        switch (iCurrentMediaLOp)
            {
            case EMPXOpMediaLGetContainerInfo:
                {
                delete iTitle;
                iTitle = NULL;

                TMPXGeneralType type = EMPXNoType;
                if (aMedia.IsSupported(KMPXMediaGeneralType))
                    {
                    type =
                        aMedia.ValueTObjectL<TMPXGeneralType>(KMPXMediaGeneralType);
                    }

                if (aMedia.IsSupported(KMPXMediaGeneralTitle) &&
                    type != EMPXItem)
                    {
                    iTitle = aMedia.ValueText(KMPXMediaGeneralTitle).AllocL();
                    MPX_DEBUG2("CMPXPodcastCollectionViewImp::HandleCollectionMediaL Title is %S", iTitle);
                    }
                UpdateTitlePaneL();
                break;
                }
            case EMPXOpMediaLSongDetailsFileCheck:
                {
                const TDesC& location = aMedia.ValueText(KMPXMediaGeneralUri);

                TInt currentLbxItemIndex(
                    iContainer->Common()->CurrentLbxItemIndex());

                if (location.Length() == 0 || ConeUtils::FileExists(location))
                    {
                    HBufC* buf = HBufC::NewLC(5); // magic number
                    buf->Des().AppendNum(currentLbxItemIndex);
                    // Activate metadata editor dialog via View Framework
                    iViewUtility->ActivateViewL(
                        TUid::Uid(KMPXPluginTypeMetadataEditorUid), buf);
                    CleanupStack::PopAndDestroy(buf);
                    }
                else
                    {
                    // mark database for invalid
                    UpdateDatabaseFlagL(currentLbxItemIndex,
                        aMedia, KMPXMediaGeneralFlagsIsInvalid, ETrue);
                    }
                break;
                }
            case EMPXOpMediaLSend:
                {
                DoHandleSendL(aMedia, aError);
                break;
                }
            case EMPXOpMediaLCollectionDetails:
                {
                ShowCollectionDetailsL(aMedia);
                break;
                }
//#ifdef __UPNP_FRAMEWORK_2_0_
            case EMPXOpMediaLCopyToRemote:
                {
                if ( iUpnpFrameworkSupport )
                    {
                    DoHandleCopyToRemoteL( aMedia );
                    }
                break;
                }
//#endif //__UPNP_FRAMEWORK_2_0_
            default:
                {
                break;
                }
            }
        }
    else
        {
        HandleErrorL(aError);
        }
    iCurrentMediaLOp = EMPXOpMediaLIdle;
    }

// ---------------------------------------------------------------------------
// From MMPXCHelperObserver
// Handles the completion of helper events
// ---------------------------------------------------------------------------
//

void CMPXPodcastCollectionViewImp::HandleOperationCompleteL(
                                           TCHelperOperation aOperation,
                                           TInt aErr,
                                           void* aArgument)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleOperationCompleteL");
    switch(aOperation)
        {
        case EDeleteOp:
            {
            // do not dismiss the wait note until HandleOpenL is called,
            if(aErr == KErrInUse)
                {
                iIsDeleting = EFalse;
				if (iWaitDialog)
					{
                    iWaitDialog->ProcessFinishedL();
					}
                HandleCommandL( EMPXCmdHandleExternalCommand );
                aErr = KErrNone; //handled here
                HBufC* text(NULL);
                if(iContainer->Common()->CurrentSelectionIndicesL()->Count() == 0)
                    {
                    // Single selection
                    text = StringLoader::LoadLC(R_MPX_COLLECTION_NOTE_DELETE_FAIL);
                    }
                else
                    {
                    // Multi selection
                    text = StringLoader::LoadLC(
                        R_MPX_COLLECTION_NOTE_DELETE_FAIL_MULTI_SELECTION);
                    }

                CAknErrorNote* dlg = new (ELeave) CAknErrorNote(ETrue);
                dlg->ExecuteLD(*text);
                CleanupStack::PopAndDestroy(text);
                }
            else if (aErr != KErrNone)
                {
                iIsDeleting = EFalse;
                if(!iIsWaitNoteCanceled)
                    {
                    if (iWaitDialog)
                    	{
                        iWaitDialog->ProcessFinishedL();
						}
                    HandleCommandL( EMPXCmdHandleExternalCommand );
                    iIsWaitNoteCanceled = EFalse;
                    }

                HandleErrorL(aErr);
                }
            // else do nothing

            iContainer->Common()->ClearLbxSelection();

             // reopen collection
            iCollectionUtility->Collection().OpenL();
            break;
            }
        case ERenameOp: // fall through
        case ESetOp:
        case EAddOp:
        default:
        break;
        }


    if(aArgument)
        {
        delete (CBase*)aArgument;
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle playback property.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandlePropertyL(
    TMPXPlaybackProperty /*aProperty*/,
    TInt /*aValue*/,
    TInt /*aError*/)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandlePropertyL");
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Method is called continously until aComplete=ETrue, signifying that
// it is done and there will be no more callbacks
// Only new items are passed each time
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleSubPlayerNamesL(
    TUid /* aPlayer */,
    const MDesCArray* /* aSubPlayers */,
    TBool /* aComplete */,
    TInt /* aError */)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleSubPlayerNamesL");
    // do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media event.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleMediaL(
    const CMPXMedia& /*aMedia*/,
    TInt /*aError*/)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleMediaL");
    // do nothing
    }

// ---------------------------------------------------------------------------
// From CAknView
// Returns views id.
// ---------------------------------------------------------------------------
//
TUid CMPXPodcastCollectionViewImp::Id() const
    {
    return TUid::Uid(KMPXPodcastCollectionViewId);
    }

// ---------------------------------------------------------------------------
// From CAknView
// Command handling function.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleCommandL(TInt aCommand)
    {
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::HandleCommandL(%d) entering",
        aCommand);

    switch (aCommand)
        {
        case EAknCmdMark:
        case EAknCmdUnmark:
        case EAknMarkAll:
        case EAknUnmarkAll:
            {
            // Custom handling of markable lists
            iContainer->Common()->HandleMarkableListProcessCommandL(
                aCommand );
#ifdef __ENABLE_MSK
            // Determine middle soft key labeling
            UpdateMiddleSoftKeyDisplayL(iCurrentMskId);
#endif // __ENABLE_MSK
            break;
            }
        case EMPXCmdGoToNowPlaying:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        case EMPXCmdAddSongs:
            {
            iViewUtility->ActivateViewL(
                TUid::Uid(KMPXPluginTypeAddSongsEditorUid));
            iCollectionUtility->Collection().OpenL();
            break;
            }
        case EMPXCmdDelete: // fall through
        case EMPXCmdRemove:
            {
            StoreListboxItemIndexL();
            DeleteSelectedItemsL();
            break;
            }
        case EMPXCmdSend:
            {
            DoSendL();
            break;
            }
        case EMPXCmdEpisodeDetails:
            {
            StoreListboxItemIndexL();
            
            iCurrentMediaLOp = EMPXOpMediaLSongDetailsFileCheck;

            iSelectedItem = KMPXInvalidItemId;
            TInt currentIndex = iContainer->Common()->CurrentLbxItemIndex();
            CMPXCommonListBoxArrayBase* listboxArray = iContainer->Common()->ListBoxArray();
            const CMPXMedia& currentSelectedMedia = listboxArray->MediaL(currentIndex);
            iSelectedItem = currentSelectedMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL(path);

            path->Set(iContainer->Common()->CurrentLbxItemIndex());

            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append(
                TMPXAttribute(KMPXMediaIdGeneral,
                    EMPXMediaGeneralUri | EMPXMediaGeneralCollectionId));
            iCollectionUtility->Collection().MediaL(*path, attrs.Array());
            CleanupStack::PopAndDestroy(&attrs);
            CleanupStack::PopAndDestroy(path);
            break;
            }
        case EMPXCmdPodcasting:
            {
            LaunchPodcastAppL();
            break;
            }
        case EMPXCmdLibraryDetails:
            {
            iCurrentMediaLOp = EMPXOpMediaLCollectionDetails;
            CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
            CleanupStack::PushL(path);
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            attrs.Append(KMPXMediaColDetailNumberOfItems);
            attrs.Append(KMPXMediaColDetailDuration);
            attrs.Append(KMPXMediaColDetailLastRefreshed);
            path->Back();
            iCollectionUtility->Collection().MediaL(*path, attrs.Array());
            CleanupStack::PopAndDestroy(&attrs);
            CleanupStack::PopAndDestroy(path);
            break;
            }
         case EMPXCmdSetPlayed:
            {
            SetPlayedStatusL( ETrue );
            break;
            }
        case EMPXCmdSetUnplayed:
            {
            SetPlayedStatusL( EFalse );
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(
                iEikonEnv->WsSession(),
                AppUi()->AppHelpContextL());
            break;
            }
        case EAknSoftkeyBack:
            {
            if(!iHandlingKeyEvent)
                {
                iContainer->Common()->ClearLbxSelection();
                CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL(cpath);
                TInt currentDepth = cpath->Levels();
                cpath->Back();
                cpath->Back();

#ifndef __ENABLE_PODCAST_IN_MUSIC_MENU
                iCollectionUtility->Collection().BackL();
#endif

                if (currentDepth == 2)
                    {

					TInt val(0);
					CRepository* repository = CRepository::NewL( KCRUidMPXMPFeatures );
					repository->Get( KMPXMPLocalVariation, val );
					TBool startInAlbumMenu = val&KMPXStartMusicPlayerinArtistsandAlbums ? ETrue : EFalse;
					delete repository;

					if ( startInAlbumMenu )
						{
						CMPXCollectionPath* path = CMPXCollectionPath::NewL();
						CleanupStack::PushL( path );
						path->AppendL(KMusicCollectionUid);
						path->AppendL(3); // Artist & Albums
						iCollectionUtility->Collection().OpenL( *path );
						CleanupStack::PopAndDestroy( path );

						// activate the collection view
						RArray<TUid> uids;
						CleanupClosePushL( uids );
						uids.AppendL( TUid::Uid( KMPXPluginTypeCollectionUid ) );
						uids.AppendL( TUid::Uid( KMusicCollectionUid ));
						iViewUtility->ActivateViewL(uids);

						CleanupStack::PopAndDestroy( &uids );
						}
					else
						{

#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
						// open the podcast collection db plugin
						CMPXCollectionPath* mainPodcastMenu = CMPXCollectionPath::NewL();
						CleanupStack::PushL( mainPodcastMenu );
						mainPodcastMenu->AppendL(KMusicCollectionUid);
						iCollectionUtility->Collection().OpenL( *mainPodcastMenu );
						CleanupStack::PopAndDestroy( mainPodcastMenu );

						// activate the podcast collection view
						RArray<TUid> uids;
						CleanupClosePushL( uids );
						uids.AppendL( TUid::Uid( KMPXPluginTypeCollectionUid ) );
						uids.AppendL( TUid::Uid( KMusicCollectionUid ));
						iViewUtility->ActivateViewL(uids);

						CleanupStack::PopAndDestroy( &uids );
#else
						AppUi()->HandleCommandL(aCommand);
#endif
						}
                    }
                else if (currentDepth == 3) // jumping back to the main podcast menu
                    {
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
                    iCollectionUtility->Collection().BackL();
#endif
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL(attrs);
                    iHandlingKeyEvent = ETrue;
                    iCurrentMediaLOp = EMPXOpMediaLGetContainerInfo;
                    attrs.Append(
                        TMPXAttribute(KMPXMediaIdGeneral,
                            EMPXMediaGeneralDuration|
                            EMPXMediaGeneralCount));
                    attrs.Append(
                        TMPXAttribute(KMPXMediaIdPodcast,
                            EMPXMediaPodcastType|
                            EMPXMediaPodcastCategoryGroup));
                    iCollectionUtility->Collection().MediaL(*cpath, attrs.Array());
                    CleanupStack::PopAndDestroy(&attrs);
                    iBackOneLevel = ETrue;
                    }
                else
                    {
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
                    iCollectionUtility->Collection().BackL();
#endif
                    RArray<TMPXAttribute> attrs;
                    CleanupClosePushL(attrs);
                    iHandlingKeyEvent = ETrue;
                    iCurrentMediaLOp = EMPXOpMediaLGetContainerInfo;
                    attrs.Append(
                        TMPXAttribute(KMPXMediaIdGeneral,
                            EMPXMediaGeneralTitle |
                            EMPXMediaGeneralDuration |
                            EMPXMediaGeneralCount));
                    attrs.Append(
                        TMPXAttribute(KMPXMediaIdPodcast,
                            EMPXMediaPodcastType |
                            EMPXMediaPodcastCategoryGroup));
                    iCollectionUtility->Collection().MediaL(*cpath, attrs.Array());
                    CleanupStack::PopAndDestroy(&attrs);
                    iBackOneLevel = ETrue;
                    }
                CleanupStack::PopAndDestroy(cpath);
                }
            else
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleCommandL Busy, ignoring command");
                }
            break;
            }
        case EAknCmdExit:
        case EAknSoftkeyExit:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
//#ifdef __UPNP_FRAMEWORK_2_0_
        case EMPXCmdUpnpPlayViaLocal:
            {
            if ( iUpnpFrameworkSupport )
                {
                SelectNewPlayerL( aCommand );
                }
            break;
            }
        case EMPXCmdUPnPAiwCmdCopyToExternalCriteria:
            {
            if ( iUpnpFrameworkSupport )
                {
                CopySelectedItemsToRemoteL();
                }
            break;
            }
//#endif //__UPNP_FRAMEWORK_2_0_
        case EMPXCmdPlay:
        case EMPXCmdPlayPause:
            {
            TMPXPlaybackState state(iPlaybackUtility->StateL());
            if ((EPbStateNotInitialised == state ||
                EPbStateStopped == state) &&
                iContainer->Common()->CurrentListItemCount() > 0)
                {
                // Needed to reset the status of iPreservedState
                if ( EPbStateStopped == state )
                    {
                    iPlaybackUtility->CommandL(EPbCmdResetPreserveState);
                    }

                RArray<TMPXAttribute> attrs;
                CleanupClosePushL(attrs);

                // specifying this attribute acts as a signal to the
                // podcast collection plugin that this media file
                // is currently being played

                TInt currentItem(iContainer->Common()->CurrentLbxItemIndex());
                MPX_DEBUG2("CMPXPodcastCollectionViewImp::HandleCommandL playing index %d", currentItem);
                if (currentItem != KErrNotFound)
                    {
                    iCollectionUtility->Collection().OpenL(currentItem, attrs.Array(), EMPXOpenPlaylistOnly);
                    }
                // else ignore
                CleanupStack::PopAndDestroy(&attrs);
                }
            else
                {
                AppUi()->HandleCommandL(aCommand);
                }
            break;
            }
        case EMPXCmdPause:
        case EMPXCmdStop:
        case EMPXCmdNext:
        case EMPXCmdPrevious:
        case EMPXCmdSeekForward:
        case EMPXCmdSeekBackward:
        case EMPXCmdStopSeeking:
        case EMPXCmdVolumeUp:
        case EMPXCmdVolumeDown:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        default:
            {
            MPX_PERF_CHECKPT("Handle Collection view unknown command");
//#ifdef __UPNP_FRAMEWORK_2_0_
            if ( iUpnpFrameworkSupport )
                {
                if (aCommand >= EMPXCmdUpnpPlayViaRemotePlayer  &&
                     aCommand < EMPXCmdUpnpLastCommandId)
                    {
                    SelectNewPlayerL(aCommand);
                    }
                else
                    {
                    AppUi()->HandleCommandL(aCommand);
                    }
                }
            else
                {
                AppUi()->HandleCommandL(aCommand);
                }
//#endif //__UPNP_FRAMEWORK_2_0_
            break;
            }
        }
    MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleCommandL() exiting");
    }

// ---------------------------------------------------------------------------
// From CAknView
// Handles a view activation.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoActivateL(
    const TVwsViewId& /* aPrevViewId */,
    TUid /* aCustomMessageId */,
    const TDesC8& /* aCustomMessage */)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DoActivateL()");

    iHandleOpenProcessed = EFalse;

    // Add view deactivation observer
    AppUi()->AddViewDeactivationObserverL( this );

    // Set status pane layout if switched here directly from another view,
    // such as when using the AnyKey
    StatusPane()->MakeVisible(ETrue);
    if ( StatusPane()->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL )
        {
        StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }

    if ( !iContainer )
        {
        iContainer = CMPXPodcastCollectionViewContainer::NewL(this, this);
        }
    CCoeControl* coeControl = iContainer->Common()->CoeControl();
    coeControl->SetMopParent( this );
    AppUi()->AddToStackL( *this, coeControl );
    coeControl->SetRect( ClientRect() );
    iContainer->Common()->SetLbxEmptyTextL(KNullDesC);
    iContainer->Common()->ActivateContainerL();

	// media is not valid until the first HandleOpenL call
    CEikButtonGroupContainer* cba = Cba();
    if ( cba )
        {
#ifdef __ENABLE_PODCAST_IN_MUSIC_MENU
        cba->SetCommandSetL( R_MPX_OPTIONS_BACK_CBA_NO_ACTION );
#else
        cba->SetCommandSetL(
            ( iViewUtility->ViewHistoryDepth() == 1 ) ?
            R_MPX_OPTIONS_EXIT_CBA_NO_ACTION : R_MPX_OPTIONS_BACK_CBA_NO_ACTION );
#endif
        cba->DrawDeferred();
        }

    UpdateTitlePaneL();
    UpdateNaviPaneL();

    CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL(cpath);
    if (iViewUtility->PreviousViewType().iUid == KMPXPluginTypePlaybackUid)
        {
        // return back from playback view and in episodes level,
        // highlight the new playing song
        MMPXSource* source = iPlaybackUtility->Source();

        if (source)
            {
            CMPXCollectionPlaylist* playlist = source->PlaylistL();
            if (playlist)
                {
                CleanupStack::PushL(playlist);
                CMPXCollectionPath* plPath =
                    CMPXCollectionPath::NewL(playlist->Path());
                CleanupStack::PushL(plPath);

                // Going from NPV -> Collection, same collection level
                if(plPath->Levels()  == cpath->Levels())
                    {
                    iPossibleJump = ETrue;
                    }
                CleanupStack::PopAndDestroy(plPath);
                CleanupStack::PopAndDestroy(playlist);
                }
            }

        if (cpath->Levels() > 1)
            {
            // valid path in collection
            cpath->Back();
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);
            iCurrentMediaLOp = EMPXOpMediaLGetContainerInfo;

            // don't ask for the title if jumping back to the
            // main podcast menu, since the default title
            // will be used
            if(cpath->Levels() == 1)
                {
                attrs.Append(KMPXMediaGeneralDuration);
                }
            else
                {
                attrs.Append(
                    TMPXAttribute(KMPXMediaIdGeneral,
                        EMPXMediaGeneralTitle |
                        EMPXMediaGeneralDuration));
                }

            iCollectionUtility->Collection().MediaL(*cpath, attrs.Array());
            CleanupStack::PopAndDestroy(&attrs);
            iCollectionUtility->Collection().OpenL();
            }
        else
            {
            TUid defaultView = iViewUtility->DefaultViewUid();
            if (defaultView != KNullUid)
                {
                MPX_DEBUG2("CMPXCollectionViewImp::DoActivateL() Activating default view 0x%x", defaultView.iUid);
                iViewUtility->ActivateViewL(defaultView);
                }
            else
                {
                MPX_DEBUG1("CMPXCollectionViewImp::DoActivateL() Activating main view");
                iViewUtility->ActivateViewL(TUid::Uid(KMPXPluginTypeMainUid));
                }
            }
        }
    else
        {
        cpath->Back();
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        iCurrentMediaLOp = EMPXOpMediaLGetContainerInfo;
        attrs.Append(KMPXMediaGeneralDuration);
        attrs.Append(
            TMPXAttribute(KMPXMediaIdPodcast,
                EMPXMediaPodcastType |
                EMPXMediaPodcastCategoryGroup));
        iCollectionUtility->Collection().MediaL(*cpath, attrs.Array());
        CleanupStack::PopAndDestroy(&attrs);
        iCollectionUtility->Collection().OpenL();
        }
    CleanupStack::PopAndDestroy(cpath);
    }

// ---------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DoDeactivate()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DoDeactivate");
    if (iContainer)
        {
        AppUi()->RemoveFromStack( iContainer->Common()->CoeControl() );
        delete iContainer;
        iContainer = NULL;
        }

    if (iNumEpisode)
        {
        delete iNumEpisode;
        iNumEpisode = NULL;
        TRAP_IGNORE(UpdateNaviPaneL());
        }
    }

// ---------------------------------------------------------------------------
// From CAknView
// Foreground event handling function.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleForegroundEventL(TBool aForeground)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleForegroundEventL");
    CAknView::HandleForegroundEventL(aForeground);
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::DynInitMenuPaneL");

    // check number of items on the list
    TInt count = iContainer->Common()->TotalListItemCount();
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() %d items on list", count);

    TInt currentItem( iContainer->Common()->CurrentLbxItemIndex() );
    TBool isListEmpty( currentItem < 0 );

    // check number of marked items on the list
    TInt marked = iContainer->Common()->CurrentSelectionIndicesL()->Count();
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() %d items marked", marked);

    TInt usbUnblockingStatus;
    RProperty::Get( KMPXViewPSUid,
                    KMPXUSBUnblockingPSStatus, 
                    usbUnblockingStatus);

    switch (aResourceId)
        {
        case R_MPX_PODCAST_COLLECTION_VIEW_PODCAST_MENU:
            {
            if ( !iPlaybackUtility->Source() )
                {
                aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, ETrue );
                }
            // Dim "Go to Podcasting" if Podcasting App is not installed on device
            if (CheckPodcastAppL() == KErrNotFound)
                {
                // Not displayed if Podcasting App is not installed
                aMenuPane->SetItemDimmed(EMPXCmdPodcasting, ETrue);
                }

            aMenuPane->SetItemDimmed( EAknCmdExit, iExitOptionHidden );
            break;
            }
        case R_MPX_PODCAST_COLLECTION_VIEW_MENU_1:
            {
            aMenuPane->SetItemDimmed(EMPXCmdFind, ETrue);
            aMenuPane->SetItemDimmed(EMPXCmdLibraryDetails, ETrue);
            if ( !iPlaybackUtility->Source() )
                {
                aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, ETrue );
                }
            if (usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive)
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Delete");
                aMenuPane->SetItemDimmed(EMPXCmdDelete, ETrue);               
                }
            // Delete is displayed only if list is not empty
            if (count == 0 || isListEmpty)
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Delete");
                aMenuPane->SetItemDimmed(EMPXCmdDelete, ETrue);
                // UPNP options will be displayed if list is not empty
                aMenuPane->SetItemDimmed(EMPXCmdUpnpPlayVia, ETrue);
                aMenuPane->SetItemDimmed(EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue);
                }
            else
                {
//#ifdef __UPNP_FRAMEWORK_2_0_
                if ( iUpnpFrameworkSupport )
                    {
                    HandleDynInitUpnpL(aResourceId, *aMenuPane);
                    }
                else
                    {
                    aMenuPane->SetItemDimmed(EMPXCmdUpnpPlayVia, ETrue);
                    aMenuPane->SetItemDimmed(EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue);
                    }
//#endif //__UPNP_FRAMEWORK_2_0_
                }

            // Dim "Go to Podcasting" if Podcasting App is not installed on device
            if (CheckPodcastAppL() == KErrNotFound)
                {
                // Not displayed if Podcasting App is not installed
                aMenuPane->SetItemDimmed(EMPXCmdPodcasting, ETrue);
                }
            aMenuPane->SetItemDimmed( EAknCmdExit, iExitOptionHidden );
            break;
            }
        case R_MPX_PODCAST_COLLECTION_VIEW_MENU_2:
            {
            aMenuPane->SetItemDimmed(EMPXCmdFind, ETrue);
            if ( !iPlaybackUtility->Source() )
                {
                aMenuPane->SetItemDimmed( EMPXCmdGoToNowPlaying, ETrue );
                }
#ifdef __ENABLE_MSK
            // multiple selection in episodes view
            if (marked && iShowContextMenu)
                {
                MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                iShowContextMenu = EFalse;
                }
            else
                {
                MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
                }
#endif //__ENABLE_MSK
                       
            if (usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive)
                {
                aMenuPane->SetItemDimmed(EMPXCmdDelete, ETrue);
                aMenuPane->SetItemDimmed(EMPXCmdSetPlayed, ETrue);	
                aMenuPane->SetItemDimmed(EMPXCmdSetUnplayed, ETrue);
                }

            // Delete is not displayed if list is empty
            // Mark/Unmark is not displayed if list is empty
            if ( count == 0 || isListEmpty)
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Send");
                aMenuPane->SetItemDimmed(EMPXCmdSend, ETrue);

                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Delete");
                aMenuPane->SetItemDimmed(EMPXCmdDelete, ETrue);

                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Set to played");
                aMenuPane->SetItemDimmed(EMPXCmdSetPlayed, ETrue);

                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Set to unplayed");
                aMenuPane->SetItemDimmed(EMPXCmdSetUnplayed, ETrue);

                // UPNP options will be displayed if list is not empty
                aMenuPane->SetItemDimmed(EMPXCmdUpnpPlayVia, ETrue);
                aMenuPane->SetItemDimmed(EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue);
                }
            else
                {
//#ifdef __UPNP_FRAMEWORK_2_0_
                if ( iUpnpFrameworkSupport )
                    {
                    HandleDynInitUpnpL(aResourceId, *aMenuPane);
                    }
                else
                    {
                    aMenuPane->SetItemDimmed(EMPXCmdUpnpPlayVia, ETrue);
                    aMenuPane->SetItemDimmed(EMPXCmdUPnPAiwCmdCopyToExternalCriteria, ETrue);
                    }
//#endif //__UPNP_FRAMEWORK_2_0_

                TInt completePlayed = 0;
                // check episode state: New, partially played or completely played
                CMPXCommonListBoxArrayBase* baseArray =
                    iContainer->Common()->ListBoxArray();
                CMPXPodcastCollectionViewListBoxArray* array =
                    static_cast<CMPXPodcastCollectionViewListBoxArray*>(baseArray);
                TInt currentItem = iContainer->Common()->CurrentLbxItemIndex();
                const CMPXMedia& media = array->MediaL(currentItem);
                TUint32 playCount = 0;

                if (media.IsSupported(KMPXMediaGeneralPlayCount))
                    {
                    playCount =
                        media.ValueTObjectL<TUint32>(KMPXMediaGeneralPlayCount);
                    }

                MPX_DEBUG2("CMPXCollectionViewImp::DynInitMenuPaneL item PlayCount = %d", playCount);

                // playback engine increments the playcount after a song has finished playing
                if (playCount >= 1)
                    {
                    completePlayed = 1;
                    MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() item is completely played");
                    }

                // Set to played.
                // If marking is off, only displayed if podcast is New Podcast or
                // Partially Played states.
                if (!marked && completePlayed)
                    {
                    MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Set to played");
                    aMenuPane->SetItemDimmed(EMPXCmdSetPlayed, ETrue);
                    }

                // Set to unplayed.
                // If marking is off, only displayed if podcast is Completely
                // Played states.
                if (!marked && !completePlayed)
                    {
                    MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Set to unplayed");
                    aMenuPane->SetItemDimmed(EMPXCmdSetUnplayed, ETrue);
                    }
                aMenuPane->SetItemDimmed(EMPXCmdSend, SendOptionVisibilityL());
                TBool isInvalid = array->IsItemBrokenLinkL(currentItem);
                TBool isCorrupted = array->IsItemCorruptedL(currentItem);
                if (isInvalid || isCorrupted)
                    {
                    aMenuPane->SetItemDimmed(EMPXCmdSetUnplayed, ETrue);
                    aMenuPane->SetItemDimmed(EMPXCmdSetPlayed, ETrue);
                    }
                }
            break;
            }
        case R_AVKON_MENUPANE_MARKABLE_LIST:
            {
            // Mark/Unmark submenu is displayed only if list is not empty
            if (count == 0 ||
                usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive ||
                isListEmpty)
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Mark/Unmark submenu");
                aMenuPane->SetItemDimmed(EAknCmdEditListMenu, ETrue);
                }
            break;
            }
        case R_MPX_PODCAST_COLLECTION_VIEW_MENU_3:
            {
            CMPXCommonListBoxArrayBase* baseArray =
                iContainer->Common()->ListBoxArray();
            CMPXPodcastCollectionViewListBoxArray* array =
                static_cast<CMPXPodcastCollectionViewListBoxArray*>(baseArray);
            TInt currentItem = iContainer->Common()->CurrentLbxItemIndex();

            // Episode details is not displayed if list is empty.
            // Not displayed if marking is on.
            // TODO: Not displayed if item in active focus is tagged as unavailable
            // or corrupt.
            if(isListEmpty)
                {
                aMenuPane->SetItemDimmed( EMPXCmdEpisodeDetails,
                    ETrue);
                }
            else if (count == 0 || marked )
                {
                MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() dimmed Episode details");
                aMenuPane->SetItemDimmed( EMPXCmdEpisodeDetails,
                    FileDetailsOptionVisibilityL() );
                }
            else
                {
                TBool isInvalid = array->IsItemBrokenLinkL(currentItem);
                TBool isCorrupted = array->IsItemCorruptedL(currentItem);
                if (isInvalid || isCorrupted)
                    {
                    aMenuPane->SetItemDimmed( EMPXCmdEpisodeDetails,
                        ETrue);
                    }
                else
                    {
                    aMenuPane->SetItemDimmed( EMPXCmdEpisodeDetails,
                        FileDetailsOptionVisibilityL() );
                    }
                }

            // Dim "Go to Podcasting" if Podcasting App is not installed on device
            if (CheckPodcastAppL() == KErrNotFound)
                {
                // Not displayed if Podcasting App is not installed
                aMenuPane->SetItemDimmed(EMPXCmdPodcasting, ETrue);
                }

            aMenuPane->SetItemDimmed( EAknCmdExit, iExitOptionHidden );

            break;
            }
//#ifdef __UPNP_FRAMEWORK_2_0_
        case R_MPX_PODCAST_COLLECTION_PLAY_VIA_SUB_MENU:
            {
            if ( iUpnpFrameworkSupport )
                {
                AddPlayersNamesToMenuL( *aMenuPane );
                }
            break;
            }
//#endif //__UPNP_FRAMEWORK_2_0_
        default:
            {
            // Do nothing
            break;
            }
        }

    if (!(aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST && usbUnblockingStatus == EMPXUSBUnblockingPSStatusActive))
        {
        // Custom handling of menu pane for markable lists
        iContainer->Common()->HandleMarkableListDynInitMenuPaneL(
            aResourceId,
            aMenuPane );
        }
    }

// ---------------------------------------------------------------------------
// From MEikListBoxObserver
// Handles listbox events.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleListBoxEventL");
    MPX_DEBUG2("CMPXPodcastCollectionViewImp::HandleListBoxEventL iLastDepth = %d", iLastDepth);
    MPX_PERF_CHECKPT("Select collection item to open");

    //handle this thouch screen event to tell to engine the index of selected song
	//to keep/change focus on right song in rename/remove
#ifdef SINGLE_CLICK_INCLUDED
    if ( aEventType == EEventItemSingleClicked )
#else
	if ( aEventType == EEventItemClicked )
#endif
		{
		if ( !iHandlingKeyEvent )
			{
			CMPXCommonListBoxArrayBase* listboxArray( iContainer->Common()->ListBoxArray() );
			TInt currentIndex( iContainer->Common()->CurrentLbxItemIndex() );

			CMPXCommand* command = CMPXCommand::NewL();
			CleanupStack::PushL( command );
			command->SetTObjectValueL<TMPXCommandId>( KMPXCommandGeneralId,
													  KMPXCommandIdCollectionSelect );
			command->SetTObjectValueL( KMPXCommandCollectionSelectIndex, currentIndex );
			command->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
			iCollectionUtility->Collection().CommandL( *command );
			CleanupStack::PopAndDestroy( command );
			}
		}

    if (aEventType == EEventEnterKeyPressed|| aEventType == EEventItemDoubleClicked
#ifdef SINGLE_CLICK_INCLUDED
        || aEventType == EEventItemSingleClicked            
#endif
        )
        {
        if (!iHandlingKeyEvent)
            {
            TMPXPlaybackState pbState( iPlaybackUtility->StateL() );
            TBool isEqual( EFalse );
            if ( pbState == EPbStatePlaying || pbState == EPbStatePaused )
                {
                // in currently playing view, check if current item is playing
                if ( iContainer->Common()->CurrentLbxItemIndex() ==
                    iContainer->PlaybackIndex() )
                    {
                    isEqual = ETrue;
                    }
                }

            if ( isEqual )
                {
                MPX_DEBUG1( "CMPXPodcastCollectionViewImp::HandleListBoxEventL going to now playing view" );
                if ( pbState == EPbStatePaused )
                    {
                    MPX_DEBUG1( "CMPXPodcastCollectionViewImp::HandleListBoxEventL resuming playback" );
                    iPlaybackUtility->CommandL( EPbCmdPlay );
                    }
                HandleCommandL( EMPXCmdGoToNowPlaying );
                }
            else
                {
                CMPXCommonListBoxArrayBase* listboxArray =
                    iContainer->Common()->ListBoxArray();
                TInt currentItem = iContainer->Common()->CurrentLbxItemIndex();
                const CMPXMedia& containerMedia = listboxArray->ContainerMedia();

                CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL(cpath);

                MPX_DEBUG_PATH(*cpath);
                if (cpath->Levels() == (iLastDepth + 1))
                    {
                    // navigated in one level
                    iLastDepth++;
                    iBottomIndex->AppendL(
                        iContainer->Common()->BottomLbxItemIndex());
                    }
                else
                    {
                    // invalid path, rebuild with all 0
                    iBottomIndex->Reset();
                    iLastDepth = cpath->Levels();
                    for (TInt i = 0; i < iLastDepth; i++)
                        {
                        iBottomIndex->AppendL(0);
                        }
                    }
                CleanupStack::PopAndDestroy(cpath);

                MPX_DEBUG2("CMPXPodcastCollectionViewImp::HandleListBoxEventL Opening Item: %d", currentItem);
                iHandlingKeyEvent = ETrue;

                // Needed to reset the status of iPreservedState
                if ( EPbStateStopped == iPlaybackUtility->StateL() )
                    {
                    iPlaybackUtility->CommandL(EPbCmdResetPreserveState);
                    }

                RArray<TMPXAttribute> attrs;
                CleanupClosePushL(attrs);

                iCollectionUtility->Collection().OpenL(currentItem,
                                                       attrs.Array(),
                                                       EMPXOpenDefault);

                CleanupStack::PopAndDestroy(&attrs);

                }
            }
        else
            {
            MPX_DEBUG1("CMPXPodcastCollectionViewImp::HandleListBoxEventL Busy, ignoring command");
            }
        }
    }

// ---------------------------------------------------------------------------
// From MEikCommandObserver
// Processes user commands.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::ProcessCommandL(TInt aCommandId)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::ProcessCommandL");

    switch(aCommandId)
        {
        case EMPXCmdUpArrow:
        case EMPXCmdDownArrow:
            {
#ifdef __COVER_DISPLAY                         	
            iIgnoreNextFocusChangedMessage = ETrue;
#endif                        
            break;
            }
        case EMPXCmdCommonDelete:
            {
            CMPXCollectionPath* cpath = iCollectionUtility->Collection().PathL();
	        CleanupStack::PushL(cpath);
	        TInt depth = cpath->Levels();
#ifndef __HG_COLLECTIONVIEW
	        if(depth > 2) // 'C' key is invalid within the main Podcast Collection
	                      // View menu
#endif // __HG_COLLECTIONVIEW
	            {
	            StoreListboxItemIndexL();
            	DeleteSelectedItemsL();
            	}
            CleanupStack::PopAndDestroy(cpath);
            break;
            }
        case EMPXCmdCommonResourceChange:
            {
            iContainer->Common()->CoeControl()->SetRect( ClientRect() );
            break;
            }
        case EAknSoftkeyEmpty:
            {
            // do nothing.  handled to avoid "unsupported" message
            break;
            }
        case EMPXCmdCommonEnterKey:
            {
            // Handle list box event for enter key pressed
            TInt currentItem = iContainer->Common()->CurrentLbxItemIndex();
            if( currentItem >= 0 )
                {
                HandleListBoxEventL(
                    NULL, // ignore
                    EEventEnterKeyPressed );
                }
            break;
            }
#ifdef __ENABLE_MSK
        case EMPXCmdCommonUpdateMiddleSoftKey:
            {
            // Determine middle soft key labeling
            UpdateMiddleSoftKeyDisplayL(iCurrentMskId);
            break;
            }
        case EAknSoftkeyContextOptions :
            {
            // set context menu flag
            iShowContextMenu = ETrue;
            // open context menu (by processing "Options" command)
            CAknView::ProcessCommandL(EAknSoftkeyOptions);
            break;
            }
#endif // __ENABLE_MSK
		case EAknCmdExit:
			{
			HandleCommandL( aCommandId );
			}
        default:
            {
            CAknView::ProcessCommandL(aCommandId);
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXCollectionViewImp::HandleFindAllL
// Handle callback for "find" operation
// -----------------------------------------------------------------------------
//
//#ifdef __UPNP_FRAMEWORK_2_0_
void CMPXPodcastCollectionViewImp::HandleFindAllL(
    const CMPXMedia& aResults,
    TBool aComplete,
    TInt aError)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::HandleFindAllL");
    if (aError == KErrNone)
        {
        switch (iCurrentFindAllLOp)
            {
//#ifdef __UPNP_FRAMEWORK_2_0_
            case EMPXOpFindAllLUpnp:
                {
                if ( iUpnpFrameworkSupport )
                    {
                    DoHandleCopyToRemoteL( aResults, aComplete );
                    }
                break;
                }
//#endif //__UPNP_FRAMEWORK_2_0_
            default:
                {
                break;
                }
            }
        }
    else
        {
        HandleErrorL(aError);
        }
    iCurrentFindAllLOp = EMPXOpFindAllLIdle;

//#ifndef __UPNP_FRAMEWORK_2_0_
    if ( !iUpnpFrameworkSupport )
        {
        // Remove compilation warnings
        (void)aComplete;
        }
//#endif  //__UPNP_FRAMEWORK_2_0_
    }

// ---------------------------------------------------------------------------
// Check presence of Nokia Podcasting Application.
// ---------------------------------------------------------------------------
//
TInt CMPXPodcastCollectionViewImp::CheckPodcastAppL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::CheckPodcastAppL");

    TApaAppInfo appInfo;
    RApaLsSession session;
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL(session);
    // Check Nokia Podcasting Application
    TInt err = session.GetAppInfo(appInfo, KNPodAppUid);
    CleanupStack::PopAndDestroy(); // Close RApaLsSession session
    if (err == KErrNone)
        {
        MPX_DEBUG2("CMPXPodcastCollectionViewImp::CheckPodcastAppL() found %S", &appInfo.iFullName);
        }
    else if (err == KErrNotFound)
        {
        MPX_DEBUG1("CMPXPodcastCollectionViewImp::CheckPodcastAppL() Podcast app does not exist");
        }
    // else ignore

    return err;
    }

// ---------------------------------------------------------------------------
// Launch Nokia Podcasting Application.
// ---------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::LaunchPodcastAppL()
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::LaunchPodcastAppL");
    TApaTaskList tasList(iEikonEnv->WsSession());
    TApaTask task = tasList.FindApp(KNPodAppUid);

    // The check for whether task exists or not is not needed. The reason is that when
    // process.create() is called, CAknApplicaiton class will first check if an instance of the
    // applicaiton is present. If an instance is present, it will invoke/activate the
    // existing application and then exit.

    //if (task.Exists())
    //    {
    //    MPX_DEBUG1("CMPXPodcastCollectionViewImp::DynInitMenuPaneL() Podcast app exist");
        // Bring Podcast App to foreground
    //    task.BringToForeground();
    //    }
    //else
    //    {

        // Launch Nokia Podcasting Application
        TApaAppInfo appInfo;
        RApaLsSession session;
        User::LeaveIfError( session.Connect() );
        CleanupClosePushL(session);
        TInt err = session.GetAppInfo(appInfo, KNPodAppUid);
        if (err == KErrNone)
            {
            MPX_TRAP(err,
            {
            CApaCommandLine* commandLine = CApaCommandLine::NewLC();
            commandLine->SetDocumentNameL(KNullDesC);
            commandLine->SetExecutableNameL(appInfo.iFullName);
            session.StartApp(*commandLine);
            CleanupStack::PopAndDestroy();
            });
            }
        CleanupStack::PopAndDestroy(); // Close RApaLsSession session
    //    }
    }

// ---------------------------------------------------------------------------
// Helper method that checks to see if the specified item is currently playing
// ---------------------------------------------------------------------------
//
TBool CMPXPodcastCollectionViewImp::IsCurrentlyPlayingL(const TMPXItemId& aItem)
    {
    MPX_FUNC("CMPXPodcastCollectionViewImp::IsCurrentlyPlaying");

    TBool currentlyPlaying(EFalse);

    if ( (iPlaybackUtility->StateL() == EPbStatePlaying ||
          iPlaybackUtility->StateL() == EPbStatePaused) &&
          iContainer )
        {
        MMPXSource* source( iPlaybackUtility->Source() );
        if ( source )
            {
            CMPXCollectionPlaylist* playlist( source->PlaylistL() );
            if ( playlist )
                {
                CleanupStack::PushL( playlist );
                CMPXCollectionPath* pbPath(
                    CMPXCollectionPath::NewL( playlist->Path() ) );
                CleanupStack::PushL( pbPath );

                if (aItem == pbPath->IdOfIndex( playlist->Index() ) )
                    {
                    currentlyPlaying =  ETrue;
                    }
                CleanupStack::PopAndDestroy( pbPath );
                CleanupStack::PopAndDestroy( playlist );
                }
            }
        }
    return currentlyPlaying;
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::HandleViewActivation
// Handle view activation
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleViewActivation(
    const TUid& aCurrentViewType,
    const TUid& /*aPreviousViewType*/ )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewImp::HandleViewActivation" );
    if ( aCurrentViewType.iUid == KMPXPluginTypeCollectionUid )
        {
        TRAP_IGNORE( UpdateNaviPaneL() );
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::HandleViewActivation
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleViewActivation(
    const TVwsViewId& aNewlyActivatedViewId,
    const TVwsViewId& /*aViewIdToBeDeactivated */)
    {
    if( aNewlyActivatedViewId.iAppUid == TUid::Uid(KMusicPlayerAppUidConstant) &&
        aNewlyActivatedViewId.iViewUid == Id() )
        {
        TRAP_IGNORE( UpdateNaviPaneL() );
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::HandleViewDeactivation
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::HandleViewDeactivation(const TVwsViewId& aViewIdToBeDeactivated,
	                                               const TVwsViewId& /*aNewlyActivatedViewId*/)
    {
    if( aViewIdToBeDeactivated.iAppUid == TUid::Uid(KMusicPlayerAppUidConstant) &&
        aViewIdToBeDeactivated.iViewUid == Id() )
        {
        if( iContainer )
            {
            if ( iNaviPane && iNaviDecorator )
                {
                iNaviPane->Pop( iNaviDecorator );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::SetPlayedStatusL
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::SetPlayedStatusL( TBool aPlayed )
    {
    MPX_FUNC( "CMPXPodcastCollectionViewImp::SetPlayedStatus" );

    const CArrayFix<TInt>* array =
        iContainer->Common()->CurrentSelectionIndicesL();
    TInt arrayCount = array->Count();

    CMPXCommonListBoxArrayBase* listboxArray =
        iContainer->Common()->ListBoxArray();

    TMPXGeneralType type = EMPXNoType;
    TMPXGeneralCategory category = EMPXNoCategory;

    if (arrayCount > 0)
        {
        CMPXMediaArray* setArray( CMPXMediaArray::NewL() );
        CleanupStack::PushL( setArray );

        // playback needs to be stopped only once
        TBool checkIfPlaying(ETrue);

        // iterate through and toggle the episode state for each selection
        for (TInt i = 0; i < arrayCount; i++)
            {
            /*CMPXPodcastCollectionViewListBoxArray* podcastArray =
                static_cast<CMPXPodcastCollectionViewListBoxArray*>(listboxArray);
            TBool isInvalid = podcastArray->IsItemBrokenLinkL(array->At(i));
            TBool isCorrupted = podcastArray->IsItemCorruptedL(array->At(i));

            if ( !isInvalid && !isCorrupted )
                {*/
            CMPXMedia* currentSelectedMedia =
                CMPXMedia::NewL(listboxArray->MediaL(array->At(i)));
            CleanupStack::PushL(currentSelectedMedia);

            if (currentSelectedMedia->IsSupported(KMPXMediaGeneralType))
                {
                type =
                    currentSelectedMedia->ValueTObjectL<TMPXGeneralType>(
                    KMPXMediaGeneralType);
                }
            if (currentSelectedMedia->IsSupported(KMPXMediaGeneralCategory))
                {
                category =
                    currentSelectedMedia->ValueTObjectL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory);
                }

            if (( type == EMPXItem ) && ( category == EMPXPodcast ))
                {
                TUint32 playCount = aPlayed;
                TInt lastPlayPos = 0;

                if ( checkIfPlaying &&
                     currentSelectedMedia->IsSupported(KMPXMediaGeneralId) &&
                     IsCurrentlyPlayingL(
                        currentSelectedMedia->ValueTObjectL<TMPXItemId>(
                            KMPXMediaGeneralId)))
                    {
                    iPlaybackUtility->CommandL(EPbCmdStop);
                    checkIfPlaying = EFalse;

                    // save off a copy of the media so Set can
                    // be run after EPbCmdStop returns
                    if(iMediaToSet)
                        {
                        delete iMediaToSet;
                        iMediaToSet = NULL;
                        }

                    iMediaToSet = CMPXMedia::NewL();
                    iMediaToSet->SetTObjectValueL( KMPXMediaGeneralType,
                    	EMPXItem );
                    iMediaToSet->SetTObjectValueL( KMPXMediaGeneralCategory,
                    	EMPXPodcast);
                    iMediaToSet->SetTObjectValueL( KMPXMediaGeneralCollectionId,
                        currentSelectedMedia->ValueTObjectL<TUid>(
                            KMPXMediaGeneralCollectionId));
                    iMediaToSet->SetTObjectValueL( KMPXMediaGeneralId,
                        currentSelectedMedia->ValueTObjectL<TMPXItemId>(
                            KMPXMediaGeneralId));

                    iMediaToSet->SetTObjectValueL<TUint32> (
                        KMPXMediaGeneralPlayCount, playCount);
                    iMediaToSet->SetTObjectValueL (
                        KMPXMediaGeneralLastPlaybackPosition, lastPlayPos);
                    iMediaToSet->SetTObjectValueL<TBool> (
                        KMPXMediaPodcastIsPlaying, EFalse);
                    }
                else
                    {
                    currentSelectedMedia->SetTObjectValueL<TUint32> (
                        KMPXMediaGeneralPlayCount, playCount);
                    currentSelectedMedia->SetTObjectValueL (
                        KMPXMediaGeneralLastPlaybackPosition, lastPlayPos);
                    currentSelectedMedia->SetTObjectValueL<TBool> (
                        KMPXMediaPodcastIsPlaying, EFalse);

                    setArray->AppendL( *currentSelectedMedia );
                    //DoSetCollectionL( currentSelectedMedia, ETrue );
                    }
                }

            CleanupStack::PopAndDestroy(currentSelectedMedia);
                /*}*/
            }

        if( setArray->Count() > 0 )
            {
            CMPXMedia* groupMedia( CMPXMedia::NewL() );
            CleanupStack::PushL( groupMedia );
            groupMedia->SetTObjectValueL<TMPXGeneralType>(KMPXMediaGeneralType, EMPXGroup);
            groupMedia->SetCObjectValueL<CMPXMediaArray>(KMPXMediaArrayContents, setArray );
            CMPXMedia* first( setArray->AtL( 0 ) );
            TUid colId( first->ValueTObjectL<TUid>(KMPXMediaGeneralCollectionId) );
            groupMedia->SetTObjectValueL<TUid>( KMPXMediaGeneralCollectionId, colId );
            DoSetCollectionL( groupMedia, EFalse );
            CleanupStack::PopAndDestroy( groupMedia );
            }
        CleanupStack::PopAndDestroy( setArray );
        }
    else
        {
        TInt currentIndex = iContainer->Common()->CurrentLbxItemIndex();
        CMPXMedia* currentSelectedMedia =
            CMPXMedia::NewL(listboxArray->MediaL(currentIndex));
        CleanupStack::PushL(currentSelectedMedia);

        if (currentSelectedMedia->IsSupported(KMPXMediaGeneralType))
            {
            type =
                currentSelectedMedia->ValueTObjectL<TMPXGeneralType>(
                    KMPXMediaGeneralType);
            }
        if (currentSelectedMedia->IsSupported(KMPXMediaGeneralCategory))
            {
            category =
                currentSelectedMedia->ValueTObjectL<TMPXGeneralCategory>(
                    KMPXMediaGeneralCategory);
            }

        if ((type == EMPXItem) && (category == EMPXPodcast))
            {
            TUint32 playCount = aPlayed;
            TInt lastPlayPos = 0;

            if ( currentSelectedMedia->IsSupported(KMPXMediaGeneralId) &&
                 IsCurrentlyPlayingL(
                    currentSelectedMedia->ValueTObjectL<TMPXItemId>(
                        KMPXMediaGeneralId)) )
                {
                iPlaybackUtility->CommandL(EPbCmdStop);

                // save off a copy of the media so Set can
                // be run after EPbCmdStop returns
                if(iMediaToSet)
                    {
                    delete iMediaToSet;
                    iMediaToSet = NULL;
                    }

                iMediaToSet = CMPXMedia::NewL();
                iMediaToSet->SetTObjectValueL( KMPXMediaGeneralType,
					EMPXItem );
                iMediaToSet->SetTObjectValueL( KMPXMediaGeneralCategory,
                	EMPXPodcast);
                iMediaToSet->SetTObjectValueL( KMPXMediaGeneralCollectionId,
                    currentSelectedMedia->ValueTObjectL<TUid>(
                        KMPXMediaGeneralCollectionId));
                iMediaToSet->SetTObjectValueL( KMPXMediaGeneralId,
                    currentSelectedMedia->ValueTObjectL<TMPXItemId>(
                        KMPXMediaGeneralId));

                iMediaToSet->SetTObjectValueL<TUint32> (
                    KMPXMediaGeneralPlayCount, playCount);
                iMediaToSet->SetTObjectValueL (
                    KMPXMediaGeneralLastPlaybackPosition, lastPlayPos);
                iMediaToSet->SetTObjectValueL<TBool> (
                    KMPXMediaPodcastIsPlaying, EFalse);
                }
            else
                {
                currentSelectedMedia->SetTObjectValueL<TUint32> (
                    KMPXMediaGeneralPlayCount, playCount);
                currentSelectedMedia->SetTObjectValueL (
                    KMPXMediaGeneralLastPlaybackPosition, lastPlayPos);
                currentSelectedMedia->SetTObjectValueL<TBool> (
                    KMPXMediaPodcastIsPlaying, EFalse);

                DoSetCollectionL( currentSelectedMedia, EFalse );
                }
            }
        CleanupStack::PopAndDestroy(currentSelectedMedia);
        }

    // Clear selection
    if(iContainer)
        {
        iContainer->Common()->ClearLbxSelection();
        }
    }

// -----------------------------------------------------------------------------
// CMPXPodcastCollectionViewImp::StoreListboxItemIndexL
// -----------------------------------------------------------------------------
//
void CMPXPodcastCollectionViewImp::StoreListboxItemIndexL()
	{
    TInt currentIndex( iContainer->Common()->CurrentLbxItemIndex() );

	CMPXCommand* command = CMPXCommand::NewL();
	CleanupStack::PushL( command );
	command->SetTObjectValueL<TMPXCommandId>( KMPXCommandGeneralId,
											  KMPXCommandIdCollectionSelect );
	command->SetTObjectValueL( KMPXCommandCollectionSelectIndex, currentIndex );
	command->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
	iCollectionUtility->Collection().CommandL( *command );
	CleanupStack::PopAndDestroy( command );
	}

//  End of File
