/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Music Player collection popup handler.
*
*/

// INCLUDE FILES
#include <hbaction.h>
#include <hbmenu.h>
#include <hbmessagebox.h>
#include <hbprogressdialog.h>
#include <hbselectiondialog.h>
#include <hbinputdialog.h>
#include <hblabel.h>
#include <hblistview.h>
#include <hbscrollbar.h>

#include "mpenginefactory.h"
#include "mpcollectionview.h"
#include "mpmpxcollectiondata.h"
#include "mpcollectiondatamodel.h"
#include "mpcollectiontbonelistdatamodel.h"
#include "mpcollectionpopuphandler.h"
#include "mptrace.h"

const int KNullIndex = -1;
const int KSongsToDisplayProgressDlg = 100;

// Popups launched by this class
const QString KContextMenu = QString( "ContextMenu" );
const QString KProgressDialog = QString( "ProgressDialog" );
const QString KAddToPlaylistSelectionDialog = QString( "AddToPlaylistSelectionDialog" );
const QString KInputTextDialog = QString( "InputTextDialog" );
const QString KGetModelIndexesListDialog = QString( "GetModelIndexesListDialog" );
const QString KArrangeSongsDialog = QString( "ArrangeSongsDialog" );
const QString KRequestDeleteMessageBox = QString( "RequestDeleteMessageBox" );

// Popups Actions
const QString KOk = QString( "Ok" );
const QString KCancel = QString( "Cancel" );
const QString KNew = QString( "New" );
const QString KOpen = QString( "Open" );
const QString KAdd = QString( "Add" );
const QString KDelete = QString( "Delete" );
const QString KRenamePlayList = QString( "RenamePlayList" );
const QString KDetails = QString( "Details" );


//------------------------------------------------------------------
//    MpPopupHandlerPermanentData
//------------------------------------------------------------------

/*!
    \class MpPopupHandlerPermanentData. This class is used by MpCollectionPopupHandler to
    store permanent data needed during asynchronous popups operation.

*/

class MpPopupHandlerPermanentData : public QObject
{
    public:
        MpPopupHandlerPermanentData( QObject *parent=0 );
        virtual ~MpPopupHandlerPermanentData();
        void clear();
    public:
        MpMpxCollectionData         *mIsolatedCollectionData;        // Not own
        QAbstractItemModel          *mAbstractItemModel;             // Own
        QList<int>                  mSelectedItems;
        QString                     mOriginalName;
        int                         mContextMenuIndex;
};

/*!
 Constructs the popup handler permanent data.
 */
MpPopupHandlerPermanentData::MpPopupHandlerPermanentData( QObject *parent )
    : QObject( parent ),
      mIsolatedCollectionData( 0 ),
      mAbstractItemModel( 0 ),
      mContextMenuIndex( KNullIndex )
{
      TX_ENTRY
      mSelectedItems.clear();
      mOriginalName.clear();
      TX_EXIT
}

/*!
 Destructs the popup handler permanent data.
 */
MpPopupHandlerPermanentData::~MpPopupHandlerPermanentData()
{
    TX_ENTRY
    clear();
    TX_EXIT
}

/*!
 \internal
  Clears all permanent data. 
 */
void MpPopupHandlerPermanentData::clear()
{
    TX_ENTRY
    mSelectedItems.clear();
    mOriginalName.clear();
    mContextMenuIndex = KNullIndex;
    if ( mIsolatedCollectionData ) {
        MpEngineFactory::sharedEngine()->releaseIsolatedCollection();
        mIsolatedCollectionData = 0;
    }
    if ( mAbstractItemModel ) {
        delete mAbstractItemModel;
        mAbstractItemModel = 0;
    }
    //clearing any child Objects.
    foreach (QObject* child, children()) {
        child->deleteLater();
    }
    TX_EXIT
}


//------------------------------------------------------------------
//    MpCollectionPopupHandler
//------------------------------------------------------------------

/*!
    \class MpCollectionPopupHandler
    \brief Music Player collection popup handler.

    This class controls the asynchronous operation of all dialogs 
    used by collection view.
*/

/*!
 Constructs the collection popup handler.
 */
MpCollectionPopupHandler::MpCollectionPopupHandler( MpCollectionView *parent )
    : QObject( parent ),
      mView( parent ),
      mOutstandingPopup( 0 ),
      mMpEngine( 0 ),
      mPermanentData( 0 ),
      mExternalEventsConnected( false )
{
    TX_ENTRY
    mMpEngine = MpEngineFactory::sharedEngine();
    mPermanentData = new MpPopupHandlerPermanentData( this );
    TX_EXIT
}

/*!
 Destructs the collection popup handler.
 */
MpCollectionPopupHandler::~MpCollectionPopupHandler()
{
    TX_ENTRY
    delete mOutstandingPopup;
    TX_EXIT
}

/*!
 Default view context menu.
 */
void MpCollectionPopupHandler::openDefaultViewContextMenu( int index, const QPointF &coords )
{
    TX_ENTRY

    if ( mView->isActivated() ) {

        HbMenu *contextMenu = 0;
        HbAction *action;
        bool usbBlocked = mMpEngine->verifyUsbBlocking();

        switch ( mMpEngine->collectionData()->context() ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtistAllSongs:
            case ECollectionContextArtistAlbumsTBone:
            case ECollectionContextAlbumsTBone:
                contextMenu = new HbMenu();
                action = contextMenu->addAction( hbTrId( "txt_common_menu_play_music" ) );
                action->setObjectName( KOpen );
                action = contextMenu->addAction( hbTrId( "txt_mus_menu_add_to_playlist" ) );
                action->setObjectName( KAdd );
                action->setEnabled( !usbBlocked );
                action = contextMenu->addAction( hbTrId( "txt_common_menu_delete" ) );
                action->setObjectName( KDelete );
                action->setEnabled( !usbBlocked );
                action = contextMenu->addAction( hbTrId( "txt_mus_menu_view_details" ) );
                action->setObjectName( KDetails );
                break;
            case ECollectionContextAlbums:
            case ECollectionContextArtists:
            case ECollectionContextArtistAlbums:
                contextMenu = new HbMenu();
                action = contextMenu->addAction( hbTrId( "txt_common_menu_open" ) );
                action->setObjectName( KOpen );
                action = contextMenu->addAction( hbTrId( "txt_mus_menu_add_to_playlist" ) );
                action->setObjectName( KAdd );
                action->setEnabled( !usbBlocked );
                action = contextMenu->addAction( hbTrId( "txt_common_menu_delete" ) );
                action->setObjectName( KDelete );
                action->setEnabled( !usbBlocked );
                break;
            case ECollectionContextPlaylists:
                contextMenu = new HbMenu();
                action = contextMenu->addAction( hbTrId( "txt_common_menu_open" ) );
                action->setObjectName( KOpen );
                if ( !mMpEngine->collectionData()->isAutoPlaylist( index ) ) {
                    action = contextMenu->addAction( hbTrId( "txt_common_menu_delete" ) );
                    action->setObjectName(KDelete);
                    action->setEnabled( !usbBlocked );
                    action = contextMenu->addAction( hbTrId( "txt_common_menu_rename_item" ) );
                    action->setObjectName( KRenamePlayList );
                    action->setEnabled( !usbBlocked );
                }
                break;
            case ECollectionContextPlaylistSongs:
                contextMenu = new HbMenu();
                action = contextMenu->addAction( hbTrId( "txt_common_menu_play_music" ) );
                action->setObjectName( KOpen );
                if ( !mMpEngine->collectionData()->isAutoPlaylist() ) {
                    action = contextMenu->addAction( hbTrId( "txt_common_menu_remove" ) );
                    action->setObjectName( KDelete );
                    action->setEnabled( !usbBlocked );
                }
                action = contextMenu->addAction( hbTrId( "txt_mus_menu_view_details" ) );
                action->setObjectName( KDetails );
                break;
            default:
                break;
        }

        if ( contextMenu ) {
            mPermanentData->mContextMenuIndex = index;
            contextMenu->setPreferredPos( coords );
            contextMenu->setObjectName( KContextMenu );
            contextMenu->setAttribute( Qt::WA_DeleteOnClose );
            setOutstandingPopup( contextMenu );
            contextMenu->open( this, SLOT( defaultContextMenuOptionSelected( HbAction* ) ) );
        }
    }

    TX_EXIT
}

/*!
 Fetch view context menu
 */
void MpCollectionPopupHandler::openFetchViewContextMenu( int index, const QPointF &coords )
{
    TX_ENTRY_ARGS( "index=" << index );

    if ( mView->isActivated() ) {

        HbAction *action;
        HbMenu *contextMenu = 0;

        switch ( mMpEngine->collectionData()->context() ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtistAlbumsTBone:
            case ECollectionContextArtistAllSongs:
            case ECollectionContextAlbumsTBone:
            case ECollectionContextPlaylistSongs:
                contextMenu = new HbMenu();
                action = contextMenu->addAction( hbTrId("txt_common_menu_play_music") );
                action->setEnabled( !mMpEngine->verifyUsbBlocking() );
                break;
            default:
                break;
        }

        if ( contextMenu ) {
            mPermanentData->mContextMenuIndex = index;
            contextMenu->setPreferredPos( coords );
            contextMenu->setAttribute( Qt::WA_DeleteOnClose );
            contextMenu->setObjectName( KContextMenu );
            setOutstandingPopup( contextMenu );
            contextMenu->open( this, SLOT( fetcherContextMenuOptionSelected( HbAction* ) ) );
        }
    }

    TX_EXIT
}

/*!
 Request a string to rename the playlist container.
 */
void MpCollectionPopupHandler::openRenamePlaylistContainerDialog( const QString &currentName )
{
    TX_ENTRY_ARGS( "Current name=" << currentName );
    mPermanentData->mOriginalName = currentName;
    getText( hbTrId( "txt_mus_dialog_enter_name" ), currentName,
             SLOT( handleRenamePlaylistContainer( HbAction* ) ) );
    TX_EXIT
}

/*!
 Request a string to rename the playlist item.
 */
void MpCollectionPopupHandler::openRenamePlaylistItemDialog( const QString &currentName )
{
    TX_ENTRY_ARGS( "Current name=" << currentName );
    mPermanentData->mOriginalName = currentName;
    getText( hbTrId( "txt_mus_dialog_enter_name" ), currentName,
             SLOT( handleRenamePlaylistItem( HbAction* ) ) );
    TX_EXIT
}

/*!
 Request to select songs to add to the playlist
 */
void MpCollectionPopupHandler::openAddSongsToPlaylist( QAbstractItemModel* model )
{
    TX_ENTRY
    getModelIndexes( hbTrId( "txt_mus_title_select_songs" ), model,
                     SLOT( handleAddSongsToPlayList( HbAction* ) ) );
    TX_EXIT
}

/*!
 Request to select songs to add to the playlist from TBone
 */
void MpCollectionPopupHandler::openAddSongsToPlaylistFromTBone( )
{
    TX_ENTRY
    MpCollectionTBoneListDataModel *model;
    model = new MpCollectionTBoneListDataModel( mMpEngine->collectionData() );
    //this item will be deleted when clearing permanent data.
    model->setParent(mPermanentData);
    model->refreshModel();
    getModelIndexes( hbTrId( "txt_mus_title_select_songs" ), model,
                     SLOT( handleAddSongsToPlayList( HbAction* ) ) );
    TX_EXIT
}

/*!
 Request to select songs to be deleted
 */
void MpCollectionPopupHandler::openDeleteSongs( QAbstractItemModel* model )
{
    TX_ENTRY
    getModelIndexes( hbTrId( "txt_mus_title_select_songs" ), model, 
                     SLOT( handleDeleteSongs( HbAction* ) ) );
    TX_EXIT
}

/*!
 Request to select songs to be added to the current play list
 */
void MpCollectionPopupHandler::openAddToCurrentPlaylist( MpMpxCollectionData* collectionData )
{
    TX_ENTRY
    mPermanentData->mIsolatedCollectionData = collectionData;
    MpCollectionDataModel *collectionDataModel;
    collectionDataModel = new MpCollectionDataModel( collectionData );
    collectionDataModel->refreshModel();
    mPermanentData->mAbstractItemModel = collectionDataModel;

    getModelIndexes( hbTrId( "txt_mus_title_select_songs" ), collectionDataModel, 
                     SLOT( handleAddToCurrentPlaylist( HbAction* ) ) );
    TX_EXIT
}

/*!
 Request to create a new play list and then add songs to it.
 */
void MpCollectionPopupHandler::openCreateNewPlaylist( MpMpxCollectionData* collectionData )
{
    TX_ENTRY
    mPermanentData->mIsolatedCollectionData = collectionData;

    MpCollectionDataModel *collectionDataModel;
    collectionDataModel = new MpCollectionDataModel( collectionData );
    collectionDataModel->refreshModel();
    mPermanentData->mAbstractItemModel = collectionDataModel;

    QStringList playlists;
    mMpEngine->findPlaylists( playlists );
    queryNewPlaylistName( playlists, SLOT( handleCreateNewPlaylistGetTextFinished( HbAction* ) ) );
    TX_EXIT
}

/*!
 Request to reorder songs
 */
void MpCollectionPopupHandler::openArrangeSongs( )
{
    TX_ENTRY
    launchArrangeSongsDialog();
    TX_EXIT
}

/*!
 Closes any active popup
 */
void MpCollectionPopupHandler::cancelOngoingPopup( bool onlyContextMenu )
{
    TX_ENTRY
    if ( mOutstandingPopup ) {
        if ( onlyContextMenu ) {
            if ( mOutstandingPopup->objectName() == KContextMenu ) {
                mOutstandingPopup->close();
                mPermanentData->clear();
            }
        }
        else {
            mOutstandingPopup->close();
            //Delete/Clear permanent data until current popup gets deleted
            mPermanentData->setParent( mOutstandingPopup );
            //Generate new permanent data for future popups
            mPermanentData = new MpPopupHandlerPermanentData( this );
        }
    }
    else {
        mPermanentData->clear();
    }
    TX_EXIT
}

/*!
 Slot to be called when an option has been selected from default context menu.
 */
void MpCollectionPopupHandler::defaultContextMenuOptionSelected( HbAction *selectedAction )
{
    TX_ENTRY
    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        QList<int> selection;
        selection.append( mPermanentData->mContextMenuIndex );
        if ( objectName == KOpen ) {
            mView->openItem( mPermanentData->mContextMenuIndex );
        }
        if ( objectName == KAdd ) {
            launchAddToPlaylistDialog( selection );
        }
        else if ( objectName == KDelete ) {
            requestDelete( selection );
        }
        else if ( objectName == KRenamePlayList ) {
            QString currentName;
            currentName = mMpEngine->collectionData()->itemData( 
                    mPermanentData->mContextMenuIndex, MpMpxCollectionData::Title );
            openRenamePlaylistItemDialog( currentName );
        }
        else if ( objectName == KDetails ) {
            mView->showItemDetails( mPermanentData->mContextMenuIndex );
        }
    }
    TX_EXIT
}

/*!
 Slot to be called when play option has been selected from fetcher context menu.
 */
void MpCollectionPopupHandler::fetcherContextMenuOptionSelected( HbAction *selectedAction )
{
    TX_ENTRY
    if ( selectedAction ) {
        // Start the playback process. View will switch to playbackview.
        mMpEngine->previewItem( mPermanentData->mContextMenuIndex );
    }        
    TX_EXIT
}

/*!
 Slot to be called when AddToPlaylist dialog has finished
 */
void MpCollectionPopupHandler::handleAddToPlaylistDialogFinished( HbAction *selectedAction )
{
    TX_ENTRY
    HbSelectionDialog *dialog = qobject_cast<HbSelectionDialog *>( sender() );
    clearOutstandingPopup( dialog );
    bool operationCompleted( true );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KNew ) {
            QStringList playlists;
            mMpEngine->findPlaylists( playlists );
            queryNewPlaylistName( playlists, SLOT( handleAddToPlaylistGetTextFinished( HbAction* ) ) );
            operationCompleted = false;
        }
    } 
    else if ( dialog->selectedItems().count() ) {//this only works for SingleSelection
        // User selected existing playlist.
        mMpEngine->saveToPlaylist( dialog->selectedItems().at( 0 ).toInt(), mPermanentData->mSelectedItems );
    }
    
    if ( operationCompleted ) {
        mPermanentData->clear();
    }
    TX_EXIT
}

/*!
 Slot to be called when input dialog (getText) has finished, comming from AddToPlayList.
 */
void MpCollectionPopupHandler::handleAddToPlaylistGetTextFinished( HbAction *selectedAction )
{
    TX_ENTRY
    HbInputDialog *dialog = qobject_cast<HbInputDialog *>( sender() );
    clearOutstandingPopup( dialog );
    bool operationCompleted( true );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QString newPlaylistName = dialog->value().toString();
            if ( newPlaylistName.length() ) {
                //Create new playlist with given name
                mMpEngine->createPlaylist( newPlaylistName, mPermanentData->mSelectedItems );
            }
            else {
                //No valid name, prompt for one again.
                getText( hbTrId( "txt_mus_dialog_enter_name" ), newPlaylistName,
                         SLOT( handleAddToPlaylistGetTextFinished( HbAction* ) ) );
                operationCompleted = false;
            }
        }
        else {
            // user decided to not provide a new name, go back to playlist list selection
            QStringList playlists;
            mMpEngine->findPlaylists( playlists );
            if ( playlists.count() ) {
                //are saved playlists, back to playlist selection dialog
                launchAddToPlaylistDialog( mPermanentData->mSelectedItems );
                operationCompleted = false;
            }
        }
    }

    if ( operationCompleted ) {
        mPermanentData->clear();
    }
    TX_EXIT
}

/*!
 Slot to be called when input dialog (getText) has finished, comming from RenamePlaylistContainer.
 */
void MpCollectionPopupHandler::handleRenamePlaylistContainer( HbAction *selectedAction )
{
    TX_ENTRY
    HbInputDialog *dialog = qobject_cast<HbInputDialog *>( sender() );
    clearOutstandingPopup( dialog );
    bool operationCompleted( true );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QString newPlaylistName = dialog->value().toString();
            if ( newPlaylistName.length() ) {
                if ( newPlaylistName != mPermanentData->mOriginalName ) {
                    mMpEngine->renamePlaylist( newPlaylistName );
                }
            }
            else {
                //No valid name, prompt for one again.
                getText( hbTrId( "txt_mus_dialog_enter_name" ), newPlaylistName,
                         SLOT( handleRenamePlaylistContainer( HbAction* ) ) );
                operationCompleted = false;
            }
        }
    }

    if ( operationCompleted ) {
        mPermanentData->clear();
    }
    TX_EXIT
}

/*!
 Slot to be called when input dialog (getText) has finished, comming from RenamePlaylist ContextMenu.
 */
void MpCollectionPopupHandler::handleRenamePlaylistItem( HbAction *selectedAction )
{
    TX_ENTRY
    HbInputDialog *dialog = qobject_cast<HbInputDialog *>( sender() );
    clearOutstandingPopup( dialog );
    bool operationCompleted( true );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QString newPlaylistName = dialog->value().toString();
            if ( newPlaylistName.length() ) {
                if ( newPlaylistName != mPermanentData->mOriginalName ) {
                    mMpEngine->renamePlaylist( newPlaylistName, mPermanentData->mContextMenuIndex );
                }
            }
            else {
                //No valid name, prompt for one again.
                getText( hbTrId( "txt_mus_dialog_enter_name" ), newPlaylistName,
                         SLOT( handleRenamePlaylistItem( HbAction* ) ) );
                operationCompleted = false;
            }
        }
    }

    if ( operationCompleted ) {
        mPermanentData->clear();
    }
    TX_EXIT
}

/*!
 Slot to be called when select songs dialog (getModelIndexes) has finished, coming from AddSongsToPlayList.
 */
void MpCollectionPopupHandler::handleAddSongsToPlayList( HbAction *selectedAction )
{
    TX_ENTRY
    HbSelectionDialog *dialog = qobject_cast<HbSelectionDialog *>( sender() );
    clearOutstandingPopup( dialog );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QModelIndexList selectedModelIndexes;
            selectedModelIndexes = dialog->selectedModelIndexes();
            if ( selectedModelIndexes.count() ) {
                QList<int> selection;
                for ( int i = 0; i < selectedModelIndexes.size(); ++i ) {
                    selection.append( selectedModelIndexes.at( i ).row() );
                }
                launchAddToPlaylistDialog( selection );
            }
        }
        else {
            mPermanentData->clear();
        }
    }

    //Dialog won't use CollectionView main model any more, return it to its original layout.
    MpCollectionDataModel *mpModel = qobject_cast<MpCollectionDataModel *>( dialog->model() );
    if ( mpModel ) {
        //setLayout() only applies for MpCollectionDataModel where we need to 
        //decide which layout to use for the secondary text.
        //MpCollectionTBoneListDataModel doesn't have secondary text.
        mpModel->setLayout( ECollectionListView );
    }
    TX_EXIT
}

/*!
 Slot to be called when select songs dialog (getModelIndexes) has finished, coming from DeleteSongs.
 */
void MpCollectionPopupHandler::handleDeleteSongs( HbAction *selectedAction )
{
    TX_ENTRY
    HbSelectionDialog *dialog = qobject_cast<HbSelectionDialog *>( sender() );
    clearOutstandingPopup( dialog );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QModelIndexList selectedModelIndexes;
            selectedModelIndexes = dialog->selectedModelIndexes();
            if ( selectedModelIndexes.count() ) {
                QList<int> selection;
                for ( int i = 0; i < selectedModelIndexes.size(); ++i ) {
                    selection.append( selectedModelIndexes.at( i ).row() );
                }
                requestDelete( selection );
            }
        }
    }

    //Dialog won't use CollectionView main model any more, return it to its original layout.
    MpCollectionDataModel *mpModel = qobject_cast<MpCollectionDataModel *>( dialog->model() );
    if ( mpModel ) {
        //setLayout() only applies for MpCollectionDataModel where we need to 
        //decide which layout to use for the secondary text.
        //MpCollectionTBoneListDataModel doesn't have secondary text.
        mpModel->setLayout( ECollectionListView );
    }
    TX_EXIT
}

/*!
 Slot to be called when select songs dialog (getModelIndexes) has finished, coming from AddToCurrentPlaylist.
 */
void MpCollectionPopupHandler::handleAddToCurrentPlaylist( HbAction *selectedAction )
{
    TX_ENTRY
    HbSelectionDialog *dialog = qobject_cast<HbSelectionDialog *>( sender() );
    clearOutstandingPopup( dialog );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QModelIndexList selectedModelIndexes;
            selectedModelIndexes = dialog->selectedModelIndexes();
            if ( selectedModelIndexes.count() ) {
                QList<int> selection;
                for ( int i = 0; i < selectedModelIndexes.size(); ++i ) {
                    selection.append( selectedModelIndexes.at( i ).row() );
                }
                mMpEngine->saveToCurrentPlaylist( selection,
                        mPermanentData->mIsolatedCollectionData );
            }
        }
    }

    //Dialog is using an isolated model which will be deleted, no need to change its layout.
    mPermanentData->clear();

    TX_EXIT
}

/*!
 Slot to be called when the input dialog (getText) has finished, comming from CreateNewPlaylist.
 */
void MpCollectionPopupHandler::handleCreateNewPlaylistGetTextFinished( HbAction *selectedAction )
{
    TX_ENTRY
    HbInputDialog *dialog = qobject_cast<HbInputDialog *>( sender() );
    clearOutstandingPopup( dialog );
    bool operationCompleted( true );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QString newPlaylistName = dialog->value().toString();
            if ( newPlaylistName.length() ) {
                //Store the new playlist name and query for the items to be added
                mPermanentData->mOriginalName = newPlaylistName;
                getModelIndexes( hbTrId( "txt_mus_title_select_songs" ), 
                                 mPermanentData->mAbstractItemModel, 
                                 SLOT( handleCreateNewPlaylistGetModelIndexesFinished( HbAction* ) ) );
            }
            else {
                //No valid name, prompt for one again.
                getText( hbTrId( "txt_mus_dialog_enter_name" ), newPlaylistName,
                         SLOT( handleCreateNewPlaylistGetTextFinished( HbAction* ) ) );
            }
            operationCompleted = false;
        }
    }

    if ( operationCompleted ) {
        mPermanentData->clear();
    }
    TX_EXIT
}

/*!
 Slot to be called when the select songs dialog (getModelIndexes) has finished, coming from CreateNewPlaylist-GetText.
 */
void MpCollectionPopupHandler::handleCreateNewPlaylistGetModelIndexesFinished( HbAction *selectedAction )
{
    TX_ENTRY
    HbSelectionDialog *dialog = qobject_cast<HbSelectionDialog *>( sender() );
    clearOutstandingPopup( dialog );

    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            QList<int> selection;
            QModelIndexList selectedModelIndexes;
            selectedModelIndexes = dialog->selectedModelIndexes();
            if ( selectedModelIndexes.count() ) {
                for ( int i = 0; i < selectedModelIndexes.size(); ++i ) {
                    selection.append( selectedModelIndexes.at( i ).row() );
                }
            }
            //Creating Playlist even when there is no selection.
            mMpEngine->createPlaylist( mPermanentData->mOriginalName, selection,
                    mPermanentData->mIsolatedCollectionData );
        }
    }

    //Dialog is using an isolated model which will be deleted, no need to change its layout.
    mPermanentData->clear();

    TX_EXIT
}

/*!
 Slot to be called when arrange songs dialog has finished.
 */
void MpCollectionPopupHandler::handleArrangeSongs( HbAction *selectedAction )
{
    TX_ENTRY
    Q_UNUSED( selectedAction );
    HbDialog *dialog = qobject_cast<HbDialog *>( sender() );
    clearOutstandingPopup( dialog );
    
    //Reopen the collection so the ordinals get fixed on the view list, if we
    //delete items the index will not match to the item on the collection.
    mMpEngine->reopenCollection();
            
    TX_EXIT
}

/*!
 Slot to be called when delete confirmation dialog has finished.
 */
void MpCollectionPopupHandler::handleRequestDelete( HbAction *selectedAction )
{
    TX_ENTRY
    HbMessageBox *dialog = qobject_cast<HbMessageBox *>( sender() );
    clearOutstandingPopup( dialog );
    
    if ( selectedAction ) {
        QString objectName = selectedAction->objectName();
        if ( objectName == KOk ) {
            mMpEngine->deleteSongs( mPermanentData->mSelectedItems );
        }
        selectedAction->setEnabled( false );
    }

    mPermanentData->clear();

    TX_EXIT
}

/*!
 Slot called upon notification from MpEngine indicating start of
 deleting process.
 */
void MpCollectionPopupHandler::handleDeleteStarted( TCollectionContext context, int count )
{
    TX_ENTRY
    if ( context == ECollectionContextPlaylistSongs ) {
        if (count >= KSongsToDisplayProgressDlg) {  //show progress dialog if removing more than 100 songs
            launchProgressDialog( "txt_mus_info_removing_songs" );
        }
    }
    else if (context != ECollectionContextPlaylists ) {  //no progress dialog for delete playlist
        launchProgressDialog( "txt_common_info_deleting" );
    }
    TX_EXIT
}

/*!
 Slot called upon notification from MpEngine indicating 'count' songs are going to be added.
 */
void MpCollectionPopupHandler::handleAddingSongs( int count )
{
    TX_ENTRY
    if ( count >= KSongsToDisplayProgressDlg )
    {
        HbProgressDialog *addSongsWaitNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
        connect( addSongsWaitNote, SIGNAL( cancelled() ), mMpEngine, SLOT( cancelCollectionRequest() ) );
        addSongsWaitNote->setModal( true );
        addSongsWaitNote->setText( hbTrId( "txt_mus_info_adding_songs" ) );
        addSongsWaitNote->setObjectName( KProgressDialog );
        addSongsWaitNote->setAttribute( Qt::WA_DeleteOnClose );
        setOutstandingPopup( addSongsWaitNote );
        addSongsWaitNote->show();
    }
    TX_EXIT
}

/*!
 Slot called upon notification from MpEngine indicating end of
 deleting or adding process.
 */
void MpCollectionPopupHandler::handleOperationEnded( bool success )
{
    TX_ENTRY
    Q_UNUSED( success );
    if ( mOutstandingPopup && ( mOutstandingPopup->objectName() == KProgressDialog ) ) {
        HbProgressDialog *dialog = qobject_cast<HbProgressDialog *>( mOutstandingPopup );
        dialog->cancel();
    }
    TX_EXIT
}

/*!
 Slot to be called when a popup is getting closed. Usefull when a dialog is closed before it finishes
 (dialog not closed by a direct user action).
 */
void MpCollectionPopupHandler::outstandingPopupClosing()
{
    TX_ENTRY
    HbPopup *popup = qobject_cast<HbPopup *>( sender() );
    if ( popup ) {
        Q_ASSERT( popup == mOutstandingPopup );
        mOutstandingPopup = 0;
    }
    TX_EXIT
}

/*!
 \internal
 sets \a popup as the current outstanding popup and cancels any other previous popup.
 */
void MpCollectionPopupHandler::setOutstandingPopup( HbPopup *popup )
{
    TX_ENTRY
    if ( mOutstandingPopup ) {
        TX_LOG_ARGS( "Warning: Multiple popups attempted to be displayed" );
        mOutstandingPopup->close();
    }

    connect( popup, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
    mOutstandingPopup = popup;
    TX_EXIT
}

/*!
 \internal
 clears and disconnects \a popup. In some cases it's needed to open a new dialog when a current one has just finished.
 */
void MpCollectionPopupHandler::clearOutstandingPopup( HbPopup *popup )
{
    TX_ENTRY
    disconnect( popup, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
    mOutstandingPopup = 0;
    TX_EXIT
}

/*!
 \internal
 Launches the 'Add to playlist' dialog.
 */
void MpCollectionPopupHandler::launchAddToPlaylistDialog( QList<int> &selection )
{
    TX_ENTRY

    QStringList playlists;
    mPermanentData->mSelectedItems = selection;
    mMpEngine->findPlaylists( playlists );
    if ( playlists.count() ) {
        //There are saved playlists, query for a saved playlist or new.
        HbAction *action;
        HbSelectionDialog *dialog = new HbSelectionDialog();
        dialog->setStringItems( playlists );
        dialog->setSelectionMode( HbAbstractItemView::SingleSelection );
        dialog->setHeadingWidget(new HbLabel( hbTrId( "txt_mus_title_select_playlist" ) ) );
        dialog->clearActions();
        action = new HbAction( hbTrId( "txt_mus_button_new" ) );
        action->setObjectName( KNew );
        dialog->addAction( action );
        action = new HbAction( hbTrId( "txt_common_button_cancel" ) );
        action->setObjectName( KCancel );
        dialog->addAction( action );
        dialog->setObjectName( KAddToPlaylistSelectionDialog );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        setOutstandingPopup( dialog );
        dialog->open( this, SLOT( handleAddToPlaylistDialogFinished( HbAction* ) ) );
    }
    else {
        //querying for a new playlist name.
        queryNewPlaylistName( playlists, SLOT( handleAddToPlaylistGetTextFinished( HbAction* ) ) );
    }
    TX_EXIT
}

/*!
 \internal
 Trigger an imput text dialog with a suggested playlist name.
 uses \a playlists to generate a suggested playlist name
 \handler Slot that should be called when input dialog finished 
 */
void MpCollectionPopupHandler::queryNewPlaylistName(const QStringList &playlists,
                                                    const char *handler ) 
{
    TX_ENTRY
    int i = 0;
    for ( ;
         playlists.contains( hbTrId( "txt_mus_dialog_enter_name_entry_playlist_l1" ).arg( i ) ) ;
         i++ ) {};
    QString suggestedPlaylistName = hbTrId( "txt_mus_dialog_enter_name_entry_playlist_l1" ).arg( i );
    getText( hbTrId( "txt_mus_dialog_enter_name" ), suggestedPlaylistName, handler );
    TX_EXIT
}

/*!
 \internal
 Launches an input text dialog.
 \a label Dialog title.
 \a text Suggested text.
 \a handler Slot to be called when input dialog finishes.
 */
void MpCollectionPopupHandler::getText( const QString &label, const QString &text,
                                        const char *handler )
{
    TX_ENTRY
    HbAction *action;
    HbInputDialog *dialog = new HbInputDialog();
    dialog->setPromptText( label );
    dialog->setInputMode( HbInputDialog::TextInput );
    dialog->setValue( text );
    dialog->clearActions();
    action = new HbAction( hbTrId( "txt_common_button_ok" ) );
    action->setObjectName( KOk );
    dialog->addAction( action );
    action = new HbAction( hbTrId( "txt_common_button_cancel" ) );
    action->setObjectName( KCancel );
    dialog->addAction( action );
    dialog->setObjectName( KInputTextDialog );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    setOutstandingPopup( dialog );
    dialog->open( this, handler );
    TX_EXIT
}

/*!
 \internal
 Launches a list dialog to select items.
 \a label Dialog title.
 \a model List model.
 \a handler Slot to be called when list dialog finishes.
 */
void MpCollectionPopupHandler::getModelIndexes( const QString &label, QAbstractItemModel* model,
                                                const char *handler)
{   
    TX_ENTRY

    connectExternalEvents();

    HbAction *action;
    HbSelectionDialog *dialog = new HbSelectionDialog();
    dialog->setHeadingWidget( new HbLabel( label ) );
    dialog->setSelectionMode( HbAbstractItemView::MultiSelection );
    MpCollectionDataModel *mpModel = qobject_cast<MpCollectionDataModel *>( model );
    if ( mpModel ) {
        //setLayout() only applies for MpCollectionDataModel where we need to 
        //decide which layout to use for the secondary text.
        //MpCollectionTBoneListDataModel doesn't have secondary text.
        mpModel->setLayout( ECollectionSelectionDialog );
    }
    dialog->setModel( model );
    dialog->clearActions();
    action = new HbAction( hbTrId( "txt_common_button_ok" ) );
    action->setObjectName( KOk );
    dialog->addAction( action );
    action = new HbAction( hbTrId( "txt_common_button_cancel" ) );
    action->setObjectName( KCancel );
    dialog->addAction( action );
    dialog->setObjectName( KGetModelIndexesListDialog );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    setOutstandingPopup( dialog );
    dialog->open( this, handler );

    TX_EXIT
}

/*!
 \internal
 Launches a list dialog to reorder them.
 */
void MpCollectionPopupHandler::launchArrangeSongsDialog()
{   
    TX_ENTRY

    HbListView *listView = new HbListView();
    listView->setItemRecycling( true );
    listView->setScrollingStyle( HbListView::PanOrFlick );
    listView->setClampingStyle( HbListView::BounceBackClamping );
    HbScrollBar *scrollbar = listView->verticalScrollBar();
    scrollbar->show();
    scrollbar->setInteractive( true);
    listView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
    MpCollectionDataModel *model;
    //Ownership of the model is passed to the listView as a child object.
    model = new MpCollectionDataModel( mMpEngine->collectionData() , mMpEngine->playbackData(), listView );
    model->refreshModel();
    connect( model,
             SIGNAL( orderChanged( int, int, int, int ) ),
             mMpEngine,
             SLOT( reorderPlaylist( int, int, int, int ) ) );
    MpCollectionDataModel *mpModel = qobject_cast<MpCollectionDataModel *>( model );
    if ( mpModel ) {
        //setLayout() only applies for MpCollectionDataModel where we need to 
        //decide which layout to use for the secondary text.
        //MpCollectionTBoneListDataModel doesn't have secondary text.
        mpModel->setLayout( ECollectionArrangeSongsDialog );
    }
    listView->setModel( model );
    listView->setArrangeMode( true );
    HbDialog *dialog = new HbDialog();
    dialog->setDismissPolicy( HbPopup::NoDismiss );
    dialog->setTimeout( HbPopup::NoTimeout );

    HbLabel *label = new HbLabel( hbTrId( "txt_mus_title_arrange"  ) );
    dialog->setHeadingWidget( label );
    dialog->setContentWidget( listView );
    dialog->clearActions();
    HbAction *action;
    action = new HbAction( hbTrId( "txt_common_button_ok" ) );
    dialog->addAction( action );
    dialog->setObjectName( KArrangeSongsDialog );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    setOutstandingPopup(dialog);
    dialog->open( this, SLOT( handleArrangeSongs( HbAction* ) ) );

    TX_EXIT
}

/*!
 \internal
 Request a delete operation always it has been confirmed.
 \a selection Items selected to be deleted.
 */
void MpCollectionPopupHandler::requestDelete( QList<int> &selection )
{
    TX_ENTRY
    QString message;
    mPermanentData->mSelectedItems = selection;
    bool needsConfirmation = true;
    connectExternalEvents();

    switch ( mMpEngine->collectionData()->context() ) {
        case ECollectionContextAllSongs:
        case ECollectionContextArtistAlbumsTBone:
        case ECollectionContextArtistAllSongs:
        case ECollectionContextAlbumsTBone:
            message = hbTrId( "txt_mus_delete_song" );
            break;
        case ECollectionContextArtists:
            message = hbTrId( "txt_mus_delete_artist" );
            break;
        case ECollectionContextAlbums:
        case ECollectionContextArtistAlbums:
            message = hbTrId( "txt_mus_delete_album" );
            break;
        case ECollectionContextPlaylists:
            message = hbTrId( "txt_mus_delete_playlist" );
            break;
        case ECollectionContextPlaylistSongs:
            needsConfirmation = false;
            mMpEngine->deleteSongs( mPermanentData->mSelectedItems );
            mPermanentData->clear();
            break;
        case ECollectionContextUnknown:
        default:
            // We shouldn't be here
            needsConfirmation = false;
            mPermanentData->clear();
            TX_LOG_ARGS( "Invalid Collection Context" );
            break;
    }

    if ( needsConfirmation ) {
        HbAction *action;
        HbMessageBox *dialog = new HbMessageBox( HbMessageBox::MessageTypeQuestion );

        dialog->setText( message );
        dialog->setTimeout( HbPopup::NoTimeout );
        dialog->clearActions();
        action = new HbAction( hbTrId( "txt_common_button_yes" ) );
        action->setObjectName( KOk );
        dialog->addAction( action );
        action = new HbAction( hbTrId( "txt_common_button_no" ) );
        action->setObjectName( KCancel );
        dialog->addAction( action );
        dialog->setObjectName( KRequestDeleteMessageBox );
        dialog->setAttribute( Qt::WA_DeleteOnClose );
        setOutstandingPopup( dialog );
        dialog->open( this, SLOT( handleRequestDelete( HbAction* ) ) );
    }

    TX_EXIT
}

/*!
 \internal
 Launches a waiting progress note.
 \a content String id to display in the dialog.
 */
void MpCollectionPopupHandler::launchProgressDialog( const char *id )
{
    HbProgressDialog *deleteProgressNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
    connect( deleteProgressNote, SIGNAL( cancelled() ), mMpEngine, SLOT( cancelCollectionRequest() ) );
    deleteProgressNote->setModal( true );
    deleteProgressNote->setDismissPolicy( HbPopup::NoDismiss );
    deleteProgressNote->setText( hbTrId( id ) );
    deleteProgressNote->setObjectName( KProgressDialog );
    deleteProgressNote->setAttribute( Qt::WA_DeleteOnClose );
    setOutstandingPopup( deleteProgressNote );
    deleteProgressNote->show();
}

/*!
 \internal
 Connects MpEngine signals on demand in order to save time at start up.
  */
void MpCollectionPopupHandler::connectExternalEvents()
{
    if ( !mExternalEventsConnected ) {
        connect( mMpEngine, SIGNAL( deleteStarted( TCollectionContext, int ) ),
                this, SLOT( handleDeleteStarted( TCollectionContext, int ) ) );
        connect( mMpEngine, SIGNAL( songsDeleted( bool ) ), this, SLOT( handleOperationEnded( bool ) ) );
        connect( mMpEngine, SIGNAL( aboutToAddSongs( int ) ), this, SLOT( handleAddingSongs( int ) ) );
        connect( mMpEngine, SIGNAL( playlistSaved( bool ) ), this, SLOT( handleOperationEnded( bool ) ) );
        mExternalEventsConnected = true;
    }
}

