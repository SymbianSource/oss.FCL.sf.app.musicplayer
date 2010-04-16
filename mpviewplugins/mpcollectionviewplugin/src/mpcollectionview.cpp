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
* Description: Music Player collection view.
*
*/

// INCLUDE FILES
#include <time.h>
#include <cstdlib>
#include <qtcore>

#include <hbinstance.h>
#include <hbapplication.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <hbmessagebox.h>
#include <hblistdialog.h>
#include <hbinputdialog.h>
#include <hblabel.h>
#include <QTranslator>
#include <QLocale>
#include <hblistview.h>
#include <hbscrollbar.h>

#include "mpcollectionview.h"
#include "mpcollectiondocumentloader.h"
#include "mpcollectioncontainerfactory.h"
#include "mpcollectioncontainer.h"
#include "mpcollectiondatamodel.h"
#include "mpengine.h"
#include "mpmpxframeworkwrapper.h"
#include "mpmpxcollectiondata.h"
#include "mpnowplayingwidget.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpsnapshotwidget.h"
#include "mpsettingsmanager.h"


const char*MUSIC_COLLECTION_DOCML = ":/docml/musiccollection.docml";
const char*EFFECT_SELECT = ":/effects/select.fxml";
const char*EFFECT_SELECT_END = ":/effects/select_end.fxml";

const int KMainToolBarAll = 0;
const int KMainToolBarArtists = 1;
const int KMainToolBarPlaylists = 2;

//const int KPlaylistToolBarAdd = 0;
const int KPlaylistToolBarRemove = 1;
const int KplaylistToolBarReorder = 2;

/*!
    \class MpCollectionView
    \brief Music Player collection view.

    Collection view provides access to music collection in the device.
    This class owns the menu and the toolbar. In addition, it is
    responsible for creating UI containers and the underlying engine
    components and connecting them to work together.
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the collection view.
 */
MpCollectionView::MpCollectionView()
    : mCollectionContext( ECollectionContextUnknown ),
      mMpxWrapper( 0 ),
      mMpEngine( 0 ),
      mContainerFactory( 0 ),
      mCollectionContainer( 0 ),
      mCollectionDataModel( 0 ),
      mActivated( false ),
      mNowPlayingBanner( 0 ),
      mBannerAttached( false ),
      mDocumentLoader( 0 ),
      mMainContainer( 0 ),
      mMainToolBar( 0 ),
      mPlaylistToolBar( 0 ),
      mSnapshot( 0 ),
      mMpTranslator( 0 ),
      mCommonTranslator( 0 ),
      mActivationWaiting( false ),
      mOutstandingPopup( 0 ),
      mUsbBlocked( false )
{
    TX_LOG
}

/*!
 Destructs the collection view.
 */
MpCollectionView::~MpCollectionView()
{
    TX_ENTRY
    delete mSnapshot;
    delete mSoftKeyQuit;
    delete mSoftKeyBack;

    HbToolBar *toolBar = takeToolBar();
    if ( mMainToolBar != toolBar && mPlaylistToolBar != toolBar ) {
        delete toolBar;
    }
    if ( mMainToolBar ) {
        mMainToolBar->deleteLater();
    }
    if ( mPlaylistToolBar ) {
        mPlaylistToolBar->deleteLater();
    }

    delete mCollectionDataModel;
    delete mCollectionContainer;
    delete mContainerFactory;
    delete mMpxWrapper;
    delete mDocumentLoader;
    delete mMpTranslator;
    delete mCommonTranslator;
    TX_EXIT
}

/*!
 Initializes the collection view. Allocates all resources needed by the view.
 */
void MpCollectionView::initializeView()
{
    TX_ENTRY

    //Load musicplayer and common translators
    QString lang = QLocale::system().name();
    QString path = QString( "z:/resource/qt/translations/" );
    bool translatorLoaded = false;

    mMpTranslator = new QTranslator( this );
    translatorLoaded = mMpTranslator->load( path + "musicplayer_" + lang );
    TX_LOG_ARGS( "Loading translator ok=" << translatorLoaded );
    if ( translatorLoaded ) {
        qApp->installTranslator( mMpTranslator );
    }

    mCommonTranslator = new QTranslator( this );
    translatorLoaded = mCommonTranslator->load( path + "common_" + lang );
    TX_LOG_ARGS( "Loading common translator ok=" << translatorLoaded );
    if ( translatorLoaded ) {
        qApp->installTranslator( mCommonTranslator );
    }

    mWindow = mainWindow();

    // Create softkey actions
    mSoftKeyQuit = new HbAction( Hb::QuitAction, this );
    connect( mSoftKeyQuit, SIGNAL( triggered() ), this, SLOT( back() ) );

    mSoftKeyBack = new HbAction( Hb::BackAction, this );
    connect( mSoftKeyBack, SIGNAL( triggered() ), this, SLOT( back() ) );

    mMpxWrapper = new MpMpxFrameworkWrapper( mViewMode );
    connect( mMpxWrapper,
            SIGNAL( collectionPlaylistOpened() ),
            this,
            SLOT( startPlaybackView() ),
            Qt::QueuedConnection );
    connect( mMpxWrapper,
            SIGNAL( playlistSaved( bool ) ),
            this,
            SLOT( playlistSaved( bool ) ),
            Qt::QueuedConnection );
    connect( mMpxWrapper,
            SIGNAL( songsDeleted( bool ) ),
            this,
            SLOT( songsDeleted( bool ) ),
            Qt::QueuedConnection );
    connect( mMpxWrapper,
            SIGNAL( playlistsRenamed( bool ) ),
            this,
            SLOT( playlistsRenamed( bool ) ),
            Qt::QueuedConnection );
    connect( mMpxWrapper,
            SIGNAL( isolatedCollectionOpened( MpMpxCollectionData* ) ),
            this,
            SLOT( handleIsolatedCollectionOpened( MpMpxCollectionData* ) ),
            Qt::QueuedConnection );

    mCollectionData = mMpxWrapper->collectionData();
    connect( mCollectionData, SIGNAL( contextChanged( TCollectionContext ) ), this, SLOT( setContext( TCollectionContext ) ) );
    mCollectionDataModel = new MpCollectionDataModel( mCollectionData );

    mDocumentLoader = new MpCollectionDocumentLoader();
    bool ok = false;
    mDocumentLoader->load( MUSIC_COLLECTION_DOCML, &ok );
    if ( ok ) {
        QGraphicsWidget *widget;

        widget = mDocumentLoader->findWidget( QString( "nowPlaying" ) );
        mNowPlayingBanner = qobject_cast<MpNowPlayingWidget*>( widget );

        if ( mViewMode == MpCommon::FetchView ) {
            // Banner is not needed since playback is stopped when returning
            // from playback preview. Disable the banner from updating.
            mNowPlayingBanner->setEnabled( false );
        }
        else {
            connect( mNowPlayingBanner, SIGNAL( clicked() ), this, SLOT( startPlaybackView() ) );
            connect( mNowPlayingBanner, SIGNAL( playbackAttachmentChanged( bool ) ), this, SLOT( attachNowPlayingBanner( bool ) ) );
        }

        widget = mDocumentLoader->findWidget( QString( "mainContainer" ) );
        mMainContainer = qobject_cast<HbWidget*>( widget );

        setWidget( mMainContainer );

        HbEffect::add( QString( "container" ),
                QString( ":/effects/slide_out_to_left.fxml" ),
                QString( "slide_out_to_left" ) );

        HbEffect::add( QString( "container" ),
                QString( ":/effects/slide_out_to_right.fxml" ),
                QString( "slide_out_to_right" ) );

        HbEffect::add( QString( "container" ),
                QString( ":/effects/slide_out_to_top.fxml" ),
                QString( "slide_out_to_top" ) );

        HbEffect::add( QString( "container" ),
                QString( ":/effects/slide_in_to_right_and_fade_in.fxml" ),
                QString( "slide_in_to_right_and_fade_in" ) );

        HbEffect::add( QString( "container" ),
                QString( ":/effects/slide_in_to_left_and_fade_in.fxml" ),
                QString( "slide_in_to_left_and_fade_in" ) );

        HbEffect::add( QString( "container" ),
                QString( ":/effects/slide_in_to_top_and_fade_in.fxml" ),
                QString( "slide_in_to_top_and_fade_in" ) );
    }
    else {
        TX_LOG_ARGS( "Error: invalid xml file." );
        Q_ASSERT_X( ok, "MpCollectionView::initializeView", "invalid xml file" );
    }

    mContainerFactory = new MpCollectionContainerFactory( this, mDocumentLoader );

    mMpEngine = MpEngine::instance();
    connect( mMpEngine, SIGNAL( usbBlocked(bool) ),
             this, SLOT( handleUsbBlocked(bool) ) );
    connect( mMpEngine, SIGNAL( libraryAboutToRefresh() ),
             this, SLOT( handleLibraryAboutToUpdate() ) );
    connect( mMpEngine, SIGNAL( libraryRefreshed() ),
             this, SLOT( handleLibraryUpdated() ) );
    mUsbBlocked = mMpEngine->verifyUsbBlocking();

    if ( MpSettingsManager::firstStartup() ) {
        mActivationWaiting = true;
        mMpEngine->refreshLibrary();
    }

    TX_EXIT
}

/*!
 Activates the collection view.
 */
void MpCollectionView::activateView()
{
    if ( mActivationWaiting ) {
        return;
    }
    mActivated = true;
    TX_ENTRY_ARGS( "mCollectionContext=" << mCollectionContext );
    if ( mCollectionContext == ECollectionContextUnknown ) {
        // Open 'All Songs' by default
        mMpxWrapper->openCollection( ECollectionContextAllSongs );
    }
    else {
        // This true when returning from other views, e.g. playback view
        setSoftkey();
    }
    TX_EXIT
}

/*!
 Deactivates the collection view.
 */
void MpCollectionView::deactivateView()
{
    TX_ENTRY
    mActivated = false;
	
    if ( mOutstandingPopup ) {
        mOutstandingPopup->close();
    }        
    menu()->close();

    setNavigationAction( 0 );
    TX_EXIT
}

/*!
 Sets the default collection - AllSongs
 */
void MpCollectionView::setDefaultView()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextAllSongs ) {
        // Open 'All Songs' by default
        mMpxWrapper->openCollection( ECollectionContextAllSongs );
    }

    if ( mBannerAttached ) {
        setBannerVisibility( false );
    }

    TX_EXIT
}

/*!
 Loads the correct layout based on the \a orientation.
 */
void MpCollectionView::orientationChange( Qt::Orientation orientation )
{
    if ( mCollectionContainer ) {
        mCollectionContainer->orientationChange( orientation );
    }

    if ( mBannerAttached ) {
        if ( orientation == Qt::Vertical ) {
            setBannerVisibility( true );
        }
        else {
            setBannerVisibility( false );
        }
    }
}

/*!
 Slot to be called when collection context is changed as a result of Open
 operation.
 */
void MpCollectionView::setContext( TCollectionContext context )
{
    TX_ENTRY_ARGS( "context=" << context );
    if ( mActivated ) {
        startContainerTransition( mCollectionContext, context );
    }
    mCollectionContext = context;
    mCollectionContainer = mContainerFactory->createContainer( context );
    mCollectionContainer->setViewMode( mViewMode );
    mCollectionDataModel->refreshModel();
    mCollectionContainer->setDataModel( mCollectionDataModel );

    // Reset softkey and the menu
    if ( mActivated ) {
        setSoftkey();
    }
    updateToolBar();
    updateMenu();
    TX_EXIT
}

/*!
 Slot to be called when 'All Songs' action is triggered from the toolbar.
 */
void MpCollectionView::openSongs()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextAllSongs ) {
        mMpxWrapper->openCollection( ECollectionContextAllSongs );
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Artists' action is triggered from the toolbar.
 */
void MpCollectionView::openArtists()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextAlbums ) {
        mMpxWrapper->openCollection( ECollectionContextAlbums );
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Playlists' action is triggered from the toolbar.
 */
void MpCollectionView::openPlaylists()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextPlaylists) {
        mMpxWrapper->openCollection( ECollectionContextPlaylists );
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Genres' action is triggered from the toolbar.
 */
void MpCollectionView::openGenres()
{
    TX_ENTRY
    if ( mCollectionContext != ECollectionContextGenres ) {
        mMpxWrapper->openCollection( ECollectionContextGenres );
    }
    TX_EXIT
}

/*!
 Slot to be called when 'Find' action is triggered from the toolbar.
 */
void MpCollectionView::find()
{
    // Todo
    HbMessageBox messageBox( "Not ready!", HbMessageBox::MessageTypeInformation );
    messageBox.exec();
    updateToolBar();
}

/*!
 Slot to be called when 'Music Store' action is triggered from the toolbar.
 */
void MpCollectionView::openMusicStore()
{
    // Todo
    HbMessageBox messageBox( "Not ready!", HbMessageBox::MessageTypeInformation );
    messageBox.exec();
    updateToolBar();
}

/*!
 Slot to be called when an item is selected by the user.
 */
void MpCollectionView::openIndex( int index )
{
    TX_ENTRY_ARGS( "index=" << index );
    bool doOpen = true;
    if ( mViewMode == MpCommon::FetchView ) {
        QString songUri;
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
            case ECollectionContextAlbumSongs:
            case ECollectionContextPlaylistSongs:
            case ECollectionContextGenreSongs:
                doOpen = false;
                songUri = mCollectionData->itemData( index, MpMpxCollectionData::Uri );
                emit songSelected( songUri );
                break;
            default:
                break;
        }
    }
    if ( doOpen ) {
        mMpxWrapper->openCollectionItem( index );
    }
    TX_EXIT
}

/*!
 Slot to be called when back/quit softkey is pressed.
 */
void MpCollectionView::back()
{
    TX_ENTRY_ARGS( "mCollectionContext=" << mCollectionContext );
    bool doExit( false );
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
        case ECollectionContextAlbums:
        case ECollectionContextPlaylists:
        case ECollectionContextGenres:
            // Exit from these levels.
            doExit = true;
            break;
        case ECollectionContextAlbumSongs:
        case ECollectionContextPlaylistSongs:
        case ECollectionContextGenreSongs:
            mMpxWrapper->back();
            break;
        default:
            doExit = true;
            break;
    }

    if ( doExit ) {
        if ( mViewMode == MpCommon::FetchView ) {
            // Send an empty string to indicate that user has cancelled
            // the fetch operation
            emit songSelected( "" );
        }
        else {
            emit command( MpCommon::Exit );
        }
    }
    TX_EXIT
}

/*!
 Slot to be called to exit.
 */
void MpCollectionView::exit()
{
    TX_ENTRY
    emit command( MpCommon::Exit );
    TX_EXIT
}


/*!
 Slot to be called to activate playback view.
 */
void MpCollectionView::startPlaybackView()
{
    TX_LOG
    emit command( MpCommon::ActivatePlaybackView );
}

/*!
 Slot to be called when 'Now Playing Banner' state changes. If active, 'Now
 Playing Banner' is attached/shown in the collection view.
 */
void MpCollectionView::attachNowPlayingBanner( bool active )
{
    TX_ENTRY
    setBannerVisibility( active );
    mBannerAttached = active;
    updateMenu();
    TX_EXIT
}

/*!
 Slot for container transition end.
 */
void MpCollectionView::containerTransitionComplete( const HbEffect::EffectStatus &status )
{
    Q_UNUSED( status );
    qobject_cast<QGraphicsView *>( mWindow )->scene()->removeItem( mSnapshot );
    delete mSnapshot;
    mSnapshot = 0;
}

/*!
 Slot to be called when 'Shuffle play all' is clicked by the user from the menu.
 */
void MpCollectionView::shufflePlayAll()
{
    mMpxWrapper->setShuffle( true );
    MpSettingsManager::setShuffle( true );
    int index = generateShuffleIndex();
    openIndex( index );
}


/*!
 Slot to be called when 'Refresh Library' is clicked by the user from the menu.
 */
void MpCollectionView::refreshLibrary()
{
    if ( !mMpEngine->verifyUsbBlocking( true ) ) {
        mMpEngine->refreshLibrary();
    }
}

/*!
 Slot to be called when 'Add to playlist' is clicked by the user from the menu.
 */
void MpCollectionView::addToPlaylist()
{
    if ( !mMpEngine->verifyUsbBlocking( true ) ) {
        QModelIndexList SelectedModelIndexes;
        bool ok;
        SelectedModelIndexes = getModelIndexes( hbTrId( "txt_mus_title_select_songs" ),
                mCollectionDataModel,
                ok);

        if ( ok && SelectedModelIndexes.count() ) {
            QList<int> selection;
            for ( int i = 0; i < SelectedModelIndexes.size(); ++i ) {
                selection.append( SelectedModelIndexes.at( i ).row() );
            }
            launchAddToPlaylistDialog( selection );
        }
    }
}

/*!
 Slot to be called when 'Delete' is clicked by the user from the menu.
 */
void MpCollectionView::deleteSongs()
{
    QModelIndexList SelectedModelIndexes;
    bool ok;
    SelectedModelIndexes = getModelIndexes( hbTrId( "txt_mus_title_select_songs" ),
            mCollectionDataModel,
            ok);

    if ( ok && SelectedModelIndexes.count() ) {
        QList<int> selection;
        for ( int i = 0; i < SelectedModelIndexes.size(); ++i ) {
            selection.append( SelectedModelIndexes.at( i ).row() );
        }
        requestDelete( selection );
        updateMenu();
    }
}

/*!
 Slot to be called when 'Rename Playlist' is clicked by the user from the menu.
 */
void MpCollectionView::renameCurrentPlaylistContainer()
{
    if ( !mMpEngine->verifyUsbBlocking( true ) ) {
        QString currentName;
        currentName = mCollectionData->collectionTitle();
        bool ok = false;
        QString newName;
    newName = getText( hbTrId( "txt_mus_dialog_enter_name" ), currentName, ok );
        if ( ok && ( currentName != newName ) && !mMpEngine->verifyUsbBlocking( true ) ) {
            mMpxWrapper->renamePlaylist( newName );
        }
    }
}


/*!
 Slot to be called when 'Add to playlist' operation has completed.
 */
void MpCollectionView::playlistSaved( bool success )
{
    if ( success &&
            ( ECollectionContextPlaylists == mCollectionContext ||
            ECollectionContextPlaylistSongs == mCollectionContext ) ) {
     mMpxWrapper->reopenCollection();
    }
}

/*!
 Slot to be called when 'Delete' operation has completed.
 */
void MpCollectionView::songsDeleted( bool success )
{
    // Update list if delete succeded or if delete interrupted by an USB MTP Event
    if ( success || mMpEngine->verifyUsbBlocking( true ) ) {
        mMpxWrapper->reopenCollection();
    }
}
/*!
 Slot to be called when 'Rename' operation has completed.
 */
void MpCollectionView::playlistsRenamed( bool success )
{
    if ( success ) {
        mMpxWrapper->reopenCollection();
    }
}

/*!
 Slot to be called to get ready for add to playlist using an isolated collection.
 */
void MpCollectionView::prepareToAddToPlaylist()
{
    TX_ENTRY
    if ( !mMpEngine->verifyUsbBlocking( true ) ) {
        //We dismiss dialogs here since after open is complete we are triggering a dialog.
        setOutstandingPopup( 0 );
        mMpxWrapper->openIsolatedCollection( ECollectionContextAllSongs );
    }
    TX_EXIT
}

/*!
 Slot to be called when isolated collection is oppened.
 */
void MpCollectionView::handleIsolatedCollectionOpened( MpMpxCollectionData* collectionData )
{
    TX_ENTRY
    if ( mActivated && !mOutstandingPopup ) {

        if (ECollectionContextPlaylistSongs == mCollectionContext) {
           addToCurrentPlaylist( collectionData );
        }
        else if (ECollectionContextPlaylists == mCollectionContext) {
            createNewPlaylist( collectionData );
        }
    }
    //Playlist is saved asynchronosly by the default collection, it is OK to release now.
    mMpxWrapper->releaseIsolatedCollection();
    TX_EXIT
}


/*!
 Slot to be called to add items to current playlist.
 */
void MpCollectionView::addToCurrentPlaylist( MpMpxCollectionData* collectionData )
{
    MpCollectionDataModel *collectionDataModel;
    collectionDataModel = new MpCollectionDataModel( collectionData );
    collectionDataModel->refreshModel();
    QModelIndexList SelectedModelIndexes;
    bool ok;
    SelectedModelIndexes = getModelIndexes( hbTrId( "txt_mus_title_select_songs" ),
            collectionDataModel,
            ok);

    if ( ok && SelectedModelIndexes.count() ) {
        QList<int> selection;
        for ( int i = 0; i < SelectedModelIndexes.size(); ++i ) {
            selection.append( SelectedModelIndexes.at( i ).row() );
        }
        if ( !mMpEngine->verifyUsbBlocking( true ) ) {
            mMpxWrapper->saveToCurrentPlaylist( selection, collectionData );
        }
    }
    delete collectionDataModel;
}

/*!
 Slot to be called to add items to new playlist.
 */
void MpCollectionView::createNewPlaylist( MpMpxCollectionData* collectionData )
{
    MpCollectionDataModel *collectionDataModel;
    collectionDataModel = new MpCollectionDataModel( collectionData );
    collectionDataModel->refreshModel();

    QString newPlaylistName;
    bool ok;
    QStringList playlists;
    mMpxWrapper->findPlaylists( playlists );
    ok = queryNewPlaylistName(newPlaylistName , playlists);
    if ( ok ) {
        QModelIndexList SelectedModelIndexes;
        SelectedModelIndexes = getModelIndexes( hbTrId( "txt_mus_title_select_songs" ),
                collectionDataModel,
                ok);
        QList<int> selection;
        if ( ok && SelectedModelIndexes.count() ) {
            for ( int i = 0; i < SelectedModelIndexes.size(); ++i ) {
                selection.append( SelectedModelIndexes.at( i ).row() );
            }
        }
        //Creating Playlist even when there is no selection.
        if ( !mMpEngine->verifyUsbBlocking( true ) ) {
            mMpxWrapper->createPlaylist( newPlaylistName, selection, collectionData );
        }
    }
    delete collectionDataModel;
}

/*!
 Slot to be called to arrange songs in a playlist.
 */
void MpCollectionView::arrangeSongs( )
{
    HbListView *listView = new HbListView();
    listView->setItemRecycling(true);
    listView->setScrollingStyle( HbListView::PanOrFlick );
    listView->setClampingStyle( HbListView::BounceBackClamping );
    HbScrollBar *scrollbar = listView->verticalScrollBar();
    scrollbar->show();
    scrollbar->setInteractive(true);
    listView->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAsNeeded);
    MpCollectionDataModel *model;
    //Ownership of the model is passed to the listView as a child object.
    model = new MpCollectionDataModel( mCollectionData, listView );
    model->refreshModel();
    connect( model,
            SIGNAL( orderChanged( int, int, int, int ) ),
            mMpxWrapper,
            SLOT( reorderPlaylist( int, int, int, int ) ) );
    listView->setModel( model );
    listView->setArrangeMode( true );
    HbDialog *popup = new HbDialog();
    popup->setAttribute( Qt::WA_DeleteOnClose );
    popup->setDismissPolicy( HbPopup::NoDismiss );
    popup->setTimeout( HbPopup::NoTimeout );

    HbLabel *label = new HbLabel( hbTrId( "txt_mus_title_arrange"  ) );
    popup->setHeadingWidget( label );
    popup->setContentWidget( listView );
    popup->setPrimaryAction( new HbAction( hbTrId( "txt_common_button_ok" ), popup ) );
    popup->setModal( true );
    connect( popup, SIGNAL( aboutToClose() ), this, SLOT( outstandingPopupClosing() ) );
    //Reopen the collection so the ordinals get fixed on the view list, if we
    //delete items the index will not match to the item on the collection.
    connect( popup, SIGNAL( aboutToClose() ), mMpxWrapper, SLOT( reopenCollection() ) );
    setOutstandingPopup(popup);
    popup->show();
}

/*!
 Slot to be called when an item is long pressed by the user.
 */
void MpCollectionView::openContextMenu( int index, const QPointF &coords )
{
    TX_ENTRY_ARGS( "index=" << index );
    switch ( mViewMode ) {
        case MpCommon::DefaultView:
            openDefaultViewContextMenu( index, coords );
            break;
        case MpCommon::FetchView:
            openFetchViewContextMenu( index, coords );
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called when a dialog is about to close.
 */
void MpCollectionView::outstandingPopupClosing()
{
   HbPopup *popup = qobject_cast<HbPopup *>( sender() );
   if ( popup ) {
       Q_ASSERT( popup != mOutstandingPopup );
       mOutstandingPopup = 0;
   }
}

/*!
 Slot to be called when USB blocking status changes.
 */
void MpCollectionView::handleUsbBlocked( bool blocked )
{
    TX_ENTRY_ARGS( "blocked=" << blocked );
    mUsbBlocked = blocked;
    
    // Hide/Show usb blocked options
    updateMenu();
    if ( mCollectionContext == ECollectionContextPlaylistSongs ) {
         updateToolBar();
    }
    TX_EXIT
}

/*!
 Slot to be called when library is going to be updated.
 */
void MpCollectionView::handleLibraryAboutToUpdate()
{
    TX_ENTRY

    if ( mActivated ) {
        if ( mOutstandingPopup ) {
            mOutstandingPopup->close();
        }        
        menu()->close();
    }
    TX_EXIT
}

/*!
 Slot to be called when refreshing completes or library has been updated.
 */
void MpCollectionView::handleLibraryUpdated()
{
    TX_ENTRY
    if ( mActivationWaiting ) {
        mActivationWaiting = false;
        activateView();
    }
    else {
        //Update cache, even if collection is in background.
        //Library refreshing could be triggered at any point due USB connect./disconnect.
        mMpxWrapper->reopenCollection();
    }
    TX_EXIT
}

/*!
 Default view context menu.
 */
void MpCollectionView::openDefaultViewContextMenu( int index, const QPointF &coords )
{
    HbMenu *contextMenu = 0;
    HbAction *action;
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
        case ECollectionContextAlbumSongs:
            contextMenu = new HbMenu();
            if ( !mUsbBlocked ) {
                action = contextMenu->addAction( hbTrId( "txt_mus_menu_add_to_playlist" ) );
                action->setObjectName( "add" );
                action = contextMenu->addAction( hbTrId( "txt_common_menu_delete" ) );
                action->setObjectName( "delete" );
            }
            break;
        case ECollectionContextAlbums:
            contextMenu = new HbMenu();
            if ( !mUsbBlocked ) {
                action = contextMenu->addAction( hbTrId( "txt_mus_menu_add_to_playlist" ) );
                action->setObjectName( "add" );
                action = contextMenu->addAction( hbTrId( "txt_common_menu_delete" ) );
                action->setObjectName( "delete" );
            }
            break;
        case ECollectionContextPlaylists:
            if ( !mCollectionData->isAutoPlaylist( index ) ) {
                contextMenu = new HbMenu();
                if ( !mUsbBlocked ) {
                    action = contextMenu->addAction( hbTrId( "txt_common_menu_delete" ) );
                    action->setObjectName("delete");
                    action = contextMenu->addAction( hbTrId( "txt_common_menu_rename_item" ) );
                    action->setObjectName( "rename playlist" );
                }
            }
            break;
        case ECollectionContextPlaylistSongs:
            if ( !mCollectionData->isAutoPlaylist() ) {
                contextMenu = new HbMenu();
                if ( !mUsbBlocked ) {
                    action = contextMenu->addAction( hbTrId( "txt_common_menu_remove" ) );
                    action->setObjectName( "delete" );
                }
            }
            break;
        default:
            break;
    }

    if ( contextMenu ) {
        setOutstandingPopup( contextMenu );
        HbAction *selectedAction = mActivated ? contextMenu->exec( coords ) : 0;
        setOutstandingPopup( 0 );
        if ( selectedAction && !mMpEngine->verifyUsbBlocking( true ) ) {
            QString objectName = selectedAction->objectName();
            QList<int> selection;
            selection.append( index );
            if ( objectName == "add" ) {
                launchAddToPlaylistDialog( selection );
            }
            else if ( objectName == "delete" ) {
                requestDelete( selection );
            }
            else if ( objectName == "rename playlist" ) {
                QString currentName;
                currentName = mCollectionData->itemData( index, MpMpxCollectionData::Title );
                bool ok = false;
                QString newName;
                newName = getText( hbTrId("txt_mus_dialog_enter_name" ), currentName, ok );
                if ( ok && ( currentName != newName ) ) {
                    mMpxWrapper->renamePlaylist( newName, index );   
                }
            }
        }
    }
    contextMenu->deleteLater();
    TX_EXIT
}

/*!
 Fetch view context menu
 */
void MpCollectionView::openFetchViewContextMenu( int index, const QPointF &coords )
{
    TX_ENTRY_ARGS( "index=" << index );

    HbMenu *contextMenu = 0;
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
        case ECollectionContextAlbumSongs:
        case ECollectionContextPlaylistSongs:
        case ECollectionContextGenreSongs:
            contextMenu = new HbMenu();
            contextMenu->addAction( hbTrId("txt_common_menu_play_music") );
            break;
        default:
            break;
    }

    if ( contextMenu ) {
        setOutstandingPopup( contextMenu );
        if ( mActivated ? contextMenu->exec( coords ) : 0 ) {
            // Start the playback process. View will switch to playbackview.
            mMpxWrapper->previewItem( index );
        }
        setOutstandingPopup( 0 );
    }
    contextMenu->deleteLater();
    TX_EXIT
}

/*!
 \internal
 Sets the main ( default ) toolbar for the view.
 */
void MpCollectionView::setMainToolBar()
{
    TX_ENTRY
    if ( !mMainToolBar ) {
        //Create the toolbar.
        mMainToolBar = new HbToolBar();
        mMainToolBar->setOrientation( Qt::Horizontal );
        QActionGroup *actionsGroup = new QActionGroup( mMainToolBar );
        HbAction *action;

        // All Songs
        action = createToolBarAction( actionsGroup, "qtg_mono_songs_all" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openSongs() ) );
        mMainToolBar->addAction( action );

        // Albums
        action = createToolBarAction( actionsGroup, "qtg_mono_artists_albums" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openArtists() ) );
        mMainToolBar->addAction( action );

        // Playlists
        action = createToolBarAction( actionsGroup, "qtg_mono_playlist" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openPlaylists() ) );
        mMainToolBar->addAction( action );

        // Genres
        action = createToolBarAction( actionsGroup, "qtg_mono_search" );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( find() ) );
        mMainToolBar->addAction( action );

        if ( mViewMode != MpCommon::FetchView ) {
            // Music Store
            action = createToolBarAction(actionsGroup, "qtg_mono_ovistore" );
            connect( action, SIGNAL( triggered( bool ) ), this, SLOT( openMusicStore() ) );
            mMainToolBar->addAction( action );
        }
    }
    HbAction* action = 0;
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
            action = qobject_cast<HbAction*>( mMainToolBar->actions()[KMainToolBarAll] );
            break;
        case ECollectionContextAlbums:
        case ECollectionContextAlbumSongs:
            action = qobject_cast<HbAction*>( mMainToolBar->actions()[KMainToolBarArtists] );
            break;
        case ECollectionContextPlaylists:
        case ECollectionContextPlaylistSongs:
            action = qobject_cast<HbAction*>( mMainToolBar->actions()[KMainToolBarPlaylists] );
            break;
    }
    if ( action ) {
        action->setChecked( true );
    }
    if ( toolBar() != mMainToolBar ) {
        HbToolBar *tmpToolBar = takeToolBar();
        if ( tmpToolBar && tmpToolBar->actions().empty() ) {
            tmpToolBar->deleteLater();
        }
        setToolBar( mMainToolBar );
    }
    TX_EXIT
}

/*!
 \internal
 Sets the playlist toolbar for the view.
 Used only on the playlist container.
 */
void MpCollectionView::setPlaylistToolBar()
{
    TX_ENTRY
    if ( !mPlaylistToolBar ) {
        mPlaylistToolBar = new HbToolBar();
        mPlaylistToolBar->setOrientation( Qt::Horizontal );
        HbAction *action;
        HbIcon *icon;

        action = new HbAction( this );
        icon = new HbIcon( "qtg_mono_plus" );
        action->setIcon( *icon );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( prepareToAddToPlaylist() ) );
        mPlaylistToolBar->addAction( action );

        action = new HbAction( this );
        icon = new HbIcon( "qtg_mono_minus" );
        action->setIcon( *icon);
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( deleteSongs() ) );
        mPlaylistToolBar->addAction( action );

        action = new HbAction( this );
        icon = new HbIcon( "qtg_mono_organize" );
        action->setIcon( *icon );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( arrangeSongs() ) );
        mPlaylistToolBar->addAction( action );
    }

    if ( !mUsbBlocked ) {
        int items = mCollectionData->count();

        mPlaylistToolBar->setEnabled( true );
		
        //no use for remove if there are no items.
        mPlaylistToolBar->actions()[KPlaylistToolBarRemove]->setEnabled( items > 0 );

        //no use for reorder if there is no more than 1 item.
        mPlaylistToolBar->actions()[KplaylistToolBarReorder]->setEnabled( items > 1 );
    }
    else {
        mPlaylistToolBar->setEnabled( false );
    }

    if ( toolBar() != mPlaylistToolBar ) {
        HbToolBar *tmpToolBar = takeToolBar();
        if ( tmpToolBar && tmpToolBar->actions().empty () ) {
            tmpToolBar->deleteLater();
        }
        setToolBar( mPlaylistToolBar );
    }
    TX_EXIT
}

/*!
 \internal
 Creates action associated with the action group for the toolbar.
 */
HbAction *MpCollectionView::createToolBarAction(
    QActionGroup *actionsGroup,
    const QString& icon )
{
    HbIcon actionIcon( icon );

    HbAction *action = new HbAction( actionsGroup );
    action->setIcon( actionIcon );
    action->setCheckable( true );
    return action;
}

/*!
 \internal
 Updates the options menu according to current context.
 */
void MpCollectionView::updateMenu()
{
    TX_ENTRY
    HbMenu* myMenu = new HbMenu();
    if ( mViewMode == MpCommon::DefaultView ) {
        bool items = mCollectionData->count() != 0;
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
                if (items ) {
                    connect( myMenu->addAction( hbTrId( "txt_mus_dblist_shuffle" ) ), SIGNAL( triggered() ), this, SLOT( shufflePlayAll() ) );
                }
                if ( !mUsbBlocked ) {
                    if (items ) {
                        connect( myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) ), SIGNAL( triggered() ), this, SLOT( addToPlaylist() ) );
                    }
                    connect( myMenu->addAction( hbTrId( "txt_mus_opt_refresh_library" ) ), SIGNAL( triggered() ), this, SLOT( refreshLibrary() ) );
                }
                connect( myMenu->addAction(hbTrId("txt_common_opt_exit")), SIGNAL(triggered()), this, SLOT(exit()) );
                break;
            case ECollectionContextAlbums:
                //connect( myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) ), SIGNAL( triggered() ), this, SLOT( addToPlaylist() ) );
                // Todo: View as coverflow
                if ( !mUsbBlocked ) {
                    connect( myMenu->addAction( hbTrId( "txt_mus_opt_refresh_library" ) ), SIGNAL( triggered() ), this, SLOT( refreshLibrary() ) );
                }
                break;
            case ECollectionContextAlbumSongs:
                if ( items && !mUsbBlocked ) {
                    connect( myMenu->addAction( hbTrId( "txt_mus_opt_add_to_playlist" ) ), SIGNAL( triggered() ), this, SLOT( addToPlaylist() ) );
                }
                break;
            case ECollectionContextPlaylists:
                if ( !mUsbBlocked ) {
                    connect( myMenu->addAction( hbTrId( "txt_mus_opt_new_playlist" ) ), SIGNAL( triggered() ), this, SLOT( prepareToAddToPlaylist() ) );
                }
                break;
            case ECollectionContextPlaylistSongs:
                if ( !mCollectionData->isAutoPlaylist() && !mUsbBlocked ) {
                    connect( myMenu->addAction( hbTrId( "txt_common_menu_rename_item" ) ), SIGNAL( triggered() ), this, SLOT( renameCurrentPlaylistContainer() ) );
                }
                break;
            default:
                break;
        }
    }
    else if ( mViewMode == MpCommon::FetchView ) {
        if ( mCollectionContext == ECollectionContextAllSongs && !mUsbBlocked ) {
            connect( myMenu->addAction( hbTrId( "txt_mus_opt_refresh_library" ) ), SIGNAL( triggered() ), this, SLOT( refreshLibrary() ) );
        }
    }

    setMenu( myMenu );
    TX_EXIT
}

/*!
 \internal
 Updates the Toolbar according to current context.
 */
void MpCollectionView::updateToolBar()
{
    TX_ENTRY

    switch ( mCollectionContext ) {
        case ECollectionContextPlaylistSongs:
            if ( !mCollectionData->isAutoPlaylist() ) {
                setPlaylistToolBar();
            }
            else if ( !toolBar()->actions().empty() ) {
                takeToolBar();
                setToolBar( new HbToolBar );
            }
            break;
        case ECollectionContextAlbumSongs:
        case ECollectionContextGenreSongs:
            if ( !toolBar()->actions().empty() ) {
                takeToolBar();
                setToolBar( new HbToolBar );
            }
            break;
        default:
            setMainToolBar();
            break;
    }
    TX_EXIT
}

/*!
 \internal
 Updates the softkey according to current context.
 */
void MpCollectionView::setSoftkey()
{
    if ( mViewMode == MpCommon::FetchView ) {
        // 'Back' is used in all views in fetch mode because we must
        // appear as an embedded application.
        setNavigationAction( mSoftKeyBack );
    }
    else {
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
            case ECollectionContextAlbums:
            case ECollectionContextPlaylists:
            case ECollectionContextGenres:
                setNavigationAction( mSoftKeyQuit );
                break;
            default:
                setNavigationAction( mSoftKeyBack );
                break;
        }
    }
}

/*!
 \internal
 Sets the Now Playing Banner visibility based on \a visible.
 */
void MpCollectionView::setBannerVisibility( bool visible )
{
    bool ok = false;
    if ( visible && ( hbInstance->allMainWindows()[0]->orientation() == Qt::Vertical ) ) {
        mDocumentLoader->load( MUSIC_COLLECTION_DOCML, "showBanner", &ok );
        mNowPlayingBanner->show();
    }
    else {
        mDocumentLoader->load( MUSIC_COLLECTION_DOCML, "hideBanner", &ok );
        mNowPlayingBanner->hide();
    }

    if ( !ok ) {
        TX_LOG_ARGS( "Error: invalid xml file." );
        Q_ASSERT_X( ok, "MpCollectionView::setBannerVisibility", "invalid xml file" );
    }
}

/*!
 \internal
 Generates a random index for shuffle all.
 */
int MpCollectionView::generateShuffleIndex()
{
    int low = 0;
    int high = mCollectionData->count();

    time_t seconds;
    time( &seconds );
    srand( ( unsigned int ) seconds );

    int index = rand() % ( high - low + 1 ) + low;
    return index;
}

/*!
 \internal
 Launches the 'Add to playlist' dialog.
 */
void MpCollectionView::launchAddToPlaylistDialog( QList<int> &selection )
{
    if (!mActivated) {
       return;
    }
    QString newPlaylistName;
    QStringList playlists;
    mMpxWrapper->findPlaylists( playlists );
    HbListDialog dialog;
    dialog.setStringItems( playlists );
    dialog.setSelectionMode( HbAbstractItemView::SingleSelection );
    dialog.setHeadingWidget(new HbLabel( hbTrId( "txt_mus_title_select_playlist" ) ) );
    dialog.setPrimaryAction(new HbAction( hbTrId( "txt_mus_button_new" ) ) );
    dialog.setSecondaryAction(new HbAction( hbTrId( "txt_common_button_cancel" ) ) );

    forever {
        if ( playlists.count() ) {
            //There are saved playlists, query for a saved playlist or new.
            setOutstandingPopup( &dialog );
            HbAction *selectedAction = mActivated ? dialog.exec() : 0;
            setOutstandingPopup( 0 );
            if ( selectedAction == dialog.primaryAction() ) {
                //User selected pimaryAction "new", fall trough to new playlyst query.
            }
            else if ( dialog.selectedItems().count() ) {//this only works for SingleSelection
                // User selected existing playlist, add songs and exit the loop.
                if ( !mMpEngine->verifyUsbBlocking( true ) ) {
                    mMpxWrapper->saveToPlaylist( dialog.selectedItems().at( 0 ), selection );
                }
                break;
            }
            else {
                //Cancel was pressed or dialog was closed or never executed, exit the loop.
                break;
            }
        }
        //querying for a new playlist name.
        if ( queryNewPlaylistName(newPlaylistName , playlists) ) {
            // user selected a new playlist, save and exit the loop.
            if ( !mMpEngine->verifyUsbBlocking( true ) ) {
                mMpxWrapper->createPlaylist( newPlaylistName, selection );
            }
            break;
        }
        else if (!playlists.count()) {
            // user decided to not provide a new name and there are no saved playlists, exit the loop
            break;
        }
        // user decided to not provide a new name and there are saved playlists, back to the top.
    } //forever
}

/*!
 \internal
 starts a transition of the main container with a decoy snapshot.
 */
void MpCollectionView::startContainerTransition( TCollectionContext contextFrom, TCollectionContext contextTo )
{
    if (contextFrom == contextTo) {
        return;
    }

    if ( !mSnapshot ) {
        mSnapshot = new MpSnapshotWidget();
    }
    mSnapshot->capture( mWindow, mMainContainer );
    mWindow->scene()->addItem( mSnapshot );


    if ( ( contextFrom == ECollectionContextAlbums && contextTo == ECollectionContextAlbumSongs ) ||
            ( contextFrom == ECollectionContextPlaylists && contextTo == ECollectionContextPlaylistSongs ) ||
            ( contextFrom == ECollectionContextGenres && contextTo == ECollectionContextGenreSongs ) ) {
        HbEffect::start( mSnapshot,
                QString( "container" ),
                QString( "slide_out_to_left" ) );

        HbEffect::start( mMainContainer,
                QString( "container" ),
                QString( "slide_in_to_left_and_fade_in" ),
                this,
                "containerTransitionComplete" );
    }
    else if( ( contextFrom == ECollectionContextAlbumSongs && contextTo == ECollectionContextAlbums ) ||
            ( contextFrom == ECollectionContextPlaylistSongs && contextTo == ECollectionContextPlaylists ) ||
            ( contextFrom == ECollectionContextGenreSongs && contextTo == ECollectionContextGenres ) ) {
        HbEffect::start( mSnapshot,
                QString( "container" ),
                QString( "slide_out_to_right" ) );

        HbEffect::start( mMainContainer,
                QString( "container" ),
                QString( "slide_in_to_right_and_fade_in" ),
                this,
                "containerTransitionComplete" );
    }
    else {
        HbEffect::start( mSnapshot,
                QString( "container" ),
                QString( "slide_out_to_top" ) );

        HbEffect::start( mMainContainer,
                QString( "container" ),
                QString( "slide_in_to_top_and_fade_in" ),
                this,
                "containerTransitionComplete" );
    }

}

/*!
 \internal
 request a delete operation always it has been confirmed.
 */
void MpCollectionView::requestDelete( QList<int> &selection )
{
    bool confirmation( false );
    // Todo: Use HbMessageBox::question when time-out removed from it
    HbMessageBox dialog( HbMessageBox::MessageTypeQuestion );

    QString message;
    HbAction *action = 0;

    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
        case ECollectionContextAlbumSongs:
            message = hbTrId( "txt_mus_delete_song" );
            dialog.setText( message );
            dialog.setTimeout( HbPopup::NoTimeout );
            setOutstandingPopup( &dialog );
            action = mActivated ? dialog.exec() : 0;
            setOutstandingPopup( 0 );
            if ( action && action == dialog.primaryAction() ) {
                confirmation = true;
            }
            break;
        case ECollectionContextAlbums:
            message = hbTrId( "txt_mus_delete_album" );
            dialog.setText( message );
            dialog.setTimeout( HbPopup::NoTimeout );
            setOutstandingPopup( &dialog );
            action = mActivated ? dialog.exec() : 0;
            setOutstandingPopup( 0 );
            if ( action && action == dialog.primaryAction() ) {
                confirmation = true;
            }
            break;
        case ECollectionContextPlaylists:
            message = hbTrId( "txt_mus_delete_playlist" );
            dialog.setText( message );
            dialog.setTimeout( HbPopup::NoTimeout );
            setOutstandingPopup( &dialog );
            action = mActivated ? dialog.exec() : 0;
            setOutstandingPopup( 0 );
            if ( action && action == dialog.primaryAction() ) {
                confirmation = true;
            }
            break;
        case ECollectionContextPlaylistSongs:
        case ECollectionContextGenres:
        case ECollectionContextGenreSongs:
            confirmation = true;
            break;
        case ECollectionContextUnknown:
        default:
            // We shouldn't be here
            TX_LOG_ARGS( "Invalid Collection Context:" << mCollectionContext );
            break;
    }

    if ( confirmation && !mMpEngine->verifyUsbBlocking( true ) ) {
        mMpxWrapper->deleteSongs( selection );
    }
}

/*!
 \internal
 Returns a list of itmes selected.
 */
QModelIndexList MpCollectionView::getModelIndexes( const QString &label, QAbstractItemModel* model, bool &ok )
{
    QModelIndexList result;

    if ( !mActivated ) {
       ok = false;
       return result;
    }

    HbListDialog *dlg = new HbListDialog();
    dlg->setHeadingWidget( new HbLabel( label ) );
    dlg->setSelectionMode( HbAbstractItemView::MultiSelection );
    dlg->setModel( model );
    setOutstandingPopup( dlg );
    HbAction* action = mActivated ? dlg->exec() : 0;
    setOutstandingPopup( 0 );
    if( action == dlg->primaryAction() ){ //OK was pressed
        ok = true;
        result = dlg->selectedModelIndexes();
    }
    else{ //Cancel was pressed or dialog was closed or never executed.
        ok = false;
    }
    dlg->setModel( 0 );
    delete dlg;
    return result;
}

/*!
 \internal
 Returns a string from user input.
 */
QString MpCollectionView::getText( const QString &label,const QString &text,
                                bool &ok )
{
    
    QString result;
    
    if ( !mActivated ) {
       ok = false;
       return result;
    }
    
    HbInputDialog *dlg = new HbInputDialog();
    dlg->setPromptText( label );
    dlg->setInputMode( HbInputDialog::TextInput );
    dlg->setValue( text );
    setOutstandingPopup( dlg ); 
    HbAction* action = mActivated ? dlg->exec() : 0;
    setOutstandingPopup( 0 );
    if( action == dlg->primaryAction() ) { //OK was pressed
        ok = true;
        result = dlg->value().toString();
    } else {  //Cancel was pressed or dialog was closed or never executed.
        ok = false;
    }
    delete dlg;
    return result;  
}

/*!
 \internal
 sets \a popup as the current outstanding popup and cancels any otstanding popup.
 */
void MpCollectionView::setOutstandingPopup( HbPopup *popup )
{
    if ( mOutstandingPopup ) {
        mOutstandingPopup->close();
    } 
    mOutstandingPopup = popup;
}

/*!
 \internal
 sets \a newPlaylistName with imput name from the user, uses \a playlists to 
 generate a suggested playlist name, retrns true if the user confirmed the query.
 */
bool MpCollectionView::queryNewPlaylistName(QString &newPlaylistName , const QStringList &playlists ) 
{
    bool ret= false;
    if (!mActivated) {
       return ret;
    }
    int i = 0;
    for ( ;
         playlists.contains( hbTrId( "txt_mus_dialog_enter_name_entry_playlist_l1" ).arg( i ) ) ;
         i++ ) {};
    QString suggestedPlaylistName = hbTrId( "txt_mus_dialog_enter_name_entry_playlist_l1" ).arg( i );
    QString suggestedText;
    bool ok = false;
    suggestedText = getText( hbTrId("txt_mus_dialog_enter_name" ), suggestedPlaylistName, ok);
    if ( ok ) {
        newPlaylistName = suggestedText;
        ret = true;
    }
    return ret;
}

