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

#include "mpcollectionview.h"
#include "mpcollectiondocumentloader.h"
#include "mpcollectioncontainerfactory.h"
#include "mpcollectioncontainer.h"
#include "mpcollectiondatamodel.h"
#include "mpcollectionsongscanner.h"
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
    : mCollectionContext(ECollectionContextUnknown),
      mMpxWrapper(0),
      mContainerFactory(0),
      mCollectionContainer(0),
      mCollectionDataModel(0),
      mActivated(false),
      mNowPlayingBanner(0),
      mBannerAttached(false),
      mEffectOnGoing(false),
      mDocumentLoader(0),
      mMainContainer(0),
      mMainToolBar(0),
      mPlaylistToolBar(0),
      mSnapshot(0),
      mSongScanner(0),
      mScanning(false)
{
    TX_LOG
}

/*!
 Destructs the collection view.
 */
MpCollectionView::~MpCollectionView()
{
    TX_ENTRY
    delete mSongScanner;
    delete mSnapshot;
    delete mSoftKeyQuit;
    delete mSoftKeyBack;

    HbToolBar *toolBar = takeToolBar();
    if ( mMainToolBar != toolBar && mPlaylistToolBar != toolBar ) {
        delete toolBar;
    }
    if ( mMainToolBar) {
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
    TX_EXIT
}

/*!
 Initializes the collection view. Allocates all resources needed by the view.
 */
void MpCollectionView::initializeView()
{
    TX_ENTRY

    mWindow = mainWindow();

    // Create softkey actions
    mSoftKeyQuit = new HbAction(Hb::QuitAction, this);
    connect( mSoftKeyQuit, SIGNAL(triggered()), this, SLOT(back()) );

    mSoftKeyBack = new HbAction(Hb::BackAction, this);
    connect( mSoftKeyBack, SIGNAL(triggered()), this, SLOT(back()) );

    mMpxWrapper = new MpMpxFrameworkWrapper(mViewMode);
    connect( mMpxWrapper, SIGNAL(collectionPlaylistOpened()), this, SLOT(startPlaybackView()) );
    connect( mMpxWrapper, SIGNAL(playlistSaved(bool)), this, SLOT(playlistSaved(bool)), Qt::QueuedConnection );
    connect( mMpxWrapper, SIGNAL(songsDeleted(bool)), this, SLOT(songsDeleted(bool)), Qt::QueuedConnection );
    connect( mMpxWrapper, SIGNAL(playlistsRenamed(bool)), this, SLOT(playlistsRenamed(bool)), Qt::QueuedConnection );
    mCollectionData = mMpxWrapper->collectionData();
    connect( mCollectionData, SIGNAL(contextChanged(TCollectionContext)), this, SLOT(setContext(TCollectionContext)) );
    mCollectionDataModel = new MpCollectionDataModel( mCollectionData );

    mDocumentLoader = new MpCollectionDocumentLoader();
    bool ok = false;
    mDocumentLoader->load( MUSIC_COLLECTION_DOCML, &ok );
    if ( ok ) {
        QGraphicsWidget *widget;

        widget = mDocumentLoader->findWidget(QString("nowPlaying"));
        mNowPlayingBanner = qobject_cast<MpNowPlayingWidget*>(widget);

        if ( mViewMode == MpCommon::FetchView ) {
            // Banner is not needed since playback is stopped when returning
            // from playback preview. Disable the banner from updating.
            mNowPlayingBanner->setEnabled(false);
        }
        else {
            connect( mNowPlayingBanner, SIGNAL(clicked()), this, SLOT(nowPlayingBannerActivated()) );
            connect( mNowPlayingBanner, SIGNAL(playbackAttachmentChanged(bool)), this, SLOT(attachNowPlayingBanner(bool)) );
            HbEffect::add( QString("banner"), EFFECT_SELECT, QString("chosen") );
            HbEffect::add( QString("banner"), EFFECT_SELECT_END, QString("chosenEnd") );
        }

        widget = mDocumentLoader->findWidget(QString("mainContainer"));
        mMainContainer = qobject_cast<HbWidget*>(widget);

        setWidget(mMainContainer);

        HbEffect::add(QString("container"),
                QString(":/effects/slide_out_to_left.fxml"),
                QString("slide_out_to_left") );

        HbEffect::add(QString("container"),
                QString(":/effects/slide_out_to_right.fxml"),
                QString("slide_out_to_right") );

        HbEffect::add(QString("container"),
                QString(":/effects/slide_out_to_top.fxml"),
                QString("slide_out_to_top") );

        HbEffect::add(QString("container"),
                QString(":/effects/slide_in_to_right_and_fade_in.fxml"),
                QString("slide_in_to_right_and_fade_in") );

        HbEffect::add(QString("container"),
                QString(":/effects/slide_in_to_left_and_fade_in.fxml"),
                QString("slide_in_to_left_and_fade_in") );

        HbEffect::add(QString("container"),
                QString(":/effects/slide_in_to_top_and_fade_in.fxml"),
                QString("slide_in_to_top_and_fade_in") );
    }
    else {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X(ok, "MpCollectionView::initializeView", "invalid xml file");
    }

    mContainerFactory = new MpCollectionContainerFactory(this, mDocumentLoader);

    if ( MpSettingsManager::firstStartup() ) {
        mSongScanner = new MpCollectionSongScanner(mMpxWrapper);
        connect(mSongScanner, SIGNAL(scanEnded()), this, SLOT(handleScanningComplete()));
        mScanning = true;
        mSongScanner->scan();
    }

    TX_EXIT
}

/*!
 Activates the collection view.
 */
void MpCollectionView::activateView()
{
    if ( mScanning ) {
        return;
    }
    mActivated = true;
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
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
    clearSoftkey();
    mActivated = false;
    TX_EXIT
}

/*!
 Loads the correct layout based on the \a orientation.
 */
void MpCollectionView::orientationChange( Qt::Orientation orientation )
{
    if ( mCollectionContainer ) {
        mCollectionContainer->orientationChange(orientation);
    }

    if ( mBannerAttached ) {
        if ( orientation == Qt::Vertical ) {
            setBannerVisibility(true);
        }
        else {
            setBannerVisibility(false);
        }
    }
}

/*!
 Slot to be called when collection context is changed as a result of Open
 operation.
 */
void MpCollectionView::setContext( TCollectionContext context )
{
    TX_ENTRY_ARGS("context=" << context);
    if (mActivated) {
        startContainerTransition(mCollectionContext, context);
    }
    mCollectionContext = context;
    mCollectionContainer = mContainerFactory->createContainer(context);
    mCollectionContainer->setViewMode(mViewMode);
    mCollectionDataModel->refreshModel();
    mCollectionContainer->setDataModel(mCollectionDataModel);

    // Reset softkey and the menu
    if (mActivated) {
        clearSoftkey();
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
    mMpxWrapper->openCollection( ECollectionContextAllSongs );
    TX_EXIT
}

/*!
 Slot to be called when 'Artists' action is triggered from the toolbar.
 */
void MpCollectionView::openArtists()
{
    TX_ENTRY
    mMpxWrapper->openCollection( ECollectionContextArtistAlbums );
    TX_EXIT
}

/*!
 Slot to be called when 'Playlists' action is triggered from the toolbar.
 */
void MpCollectionView::openPlaylists()
{
    TX_ENTRY
    mMpxWrapper->openCollection( ECollectionContextPlaylists );
    TX_EXIT
}

/*!
 Slot to be called when 'Genres' action is triggered from the toolbar.
 */
void MpCollectionView::openGenres()
{
    TX_ENTRY
    mMpxWrapper->openCollection( ECollectionContextGenres );
    TX_EXIT
}

/*!
 Slot to be called when 'Find' action is triggered from the toolbar.
 */
void MpCollectionView::find()
{
    // Todo
    HbMessageBox messageBox("Not ready!", HbMessageBox::MessageTypeInformation);
    messageBox.exec();
    updateToolBar();
}

/*!
 Slot to be called when 'Music Store' action is triggered from the toolbar.
 */
void MpCollectionView::openMusicStore()
{
    // Todo
    HbMessageBox messageBox("Not ready!", HbMessageBox::MessageTypeInformation);
    messageBox.exec();
    updateToolBar();
}

/*!
 Slot to be called when an item is selected by the user.
 */
void MpCollectionView::openIndex( int index )
{
    TX_ENTRY_ARGS("index=" << index);
    bool doOpen = true;
    if ( mViewMode == MpCommon::FetchView ) {
        QString songUri;
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
            case ECollectionContextAlbumSongs:
            case ECollectionContextPlaylistSongs:
            case ECollectionContextGenreSongs:
                doOpen = false;
                songUri = mCollectionData->itemData(index, MpMpxCollectionData::Uri);
                emit songSelected(songUri);
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
    TX_ENTRY_ARGS("mCollectionContext=" << mCollectionContext);
    bool doExit(false);
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
        case ECollectionContextArtistAlbums:
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
            emit songSelected("");
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
 Slot to be called when 'Now Playing Banner' is clicked by the user.
 */
void MpCollectionView::nowPlayingBannerActivated()
{
    if ( !mEffectOnGoing ) {
        HbEffect::start(mNowPlayingBanner, QString("banner"), QString("chosen"), this, "nowPlayingBannerChosenFxComplete1");
        mEffectOnGoing = true;
    }
}

/*!
 Slot for 'Now Playing Banner' effects part 1.
 */
void MpCollectionView::nowPlayingBannerChosenFxComplete1( const HbEffect::EffectStatus &status )
{
    Q_UNUSED(status);
    HbEffect::start(mNowPlayingBanner, QString("banner"), QString("chosenEnd"), this, "nowPlayingBannerChosenFxComplete2");
}

/*!
 Slot for 'Now Playing Banner' effects part 2. The end.
 */
void MpCollectionView::nowPlayingBannerChosenFxComplete2( const HbEffect::EffectStatus &status )
{
    Q_UNUSED(status);
    mEffectOnGoing = false;
    startPlaybackView();
}

/*!
 Slot for container transition end.
 */
void MpCollectionView::containerTransitionComplete( const HbEffect::EffectStatus &status )
{
    Q_UNUSED(status);
    qobject_cast<QGraphicsView *>(mWindow)->scene()->removeItem(mSnapshot);
    delete mSnapshot;
    mSnapshot = 0;
}

/*!
 Slot to be called when 'Shuffle play all' is clicked by the user from the menu.
 */
void MpCollectionView::shufflePlayAll()
{
    mMpxWrapper->setShuffle(true);
    MpSettingsManager::setShuffle(true);
    int index = generateShuffleIndex();
    openIndex(index);
}


/*!
 Slot to be called when 'Refresh Library' is clicked by the user from the menu.
 */
void MpCollectionView::refreshLibrary()
{
    if ( !mSongScanner ) {
        mSongScanner = new MpCollectionSongScanner(mMpxWrapper);
        connect(mSongScanner, SIGNAL(scanEnded()), this, SLOT(handleScanningComplete()));
    }
    mScanning = true;
    mSongScanner->scan();
}

/*!
 Slot to be called when 'Add to playlist' is clicked by the user from the menu.
 */
void MpCollectionView::addToPlaylist()
{
    QModelIndexList SelectedModelIndexes;
    bool ok;
    SelectedModelIndexes = HbListDialog::getModelIndexes(QString(tr("Select songs:")), 
            mCollectionDataModel,
            &ok,
            HbAbstractItemView::MultiSelection);
                
    if (ok && SelectedModelIndexes.count()) {
        QList<int> selection;
        for ( int i = 0; i < SelectedModelIndexes.size(); ++i ) {
            selection.append( SelectedModelIndexes.at(i).row() );
        }
    launchAddToPlaylistDialog(selection);
    } 
}

/*!
 Slot to be called when 'Delete' is clicked by the user from the menu.
 */
void MpCollectionView::deleteSongs()
{
    QModelIndexList SelectedModelIndexes;
    bool ok;
    SelectedModelIndexes = HbListDialog::getModelIndexes(QString(tr("Select songs:")), 
            mCollectionDataModel,
            &ok,
            HbAbstractItemView::MultiSelection);
                
    if (ok && SelectedModelIndexes.count()) {
        QList<int> selection;
        for ( int i = 0; i < SelectedModelIndexes.size(); ++i ) {
            selection.append( SelectedModelIndexes.at(i).row() );
        }
    requestDelete(selection);
    updateMenu();
    }
}

/*!
 Slot to be called when 'Rename Playlist' is clicked by the user from the menu.
 */
void MpCollectionView::renameCurrentPlaylistContainer()
{
    QString currentName;
    currentName = mCollectionData->collectionTitle();
    bool ok = false;
    QString newName;
    newName = HbInputDialog::getText(QString(tr("Enter name:")), currentName, &ok);
    if ( ok && ( currentName != newName ) ) 
    mMpxWrapper->renamePlaylist( newName );   
}


/*!
 Slot to be called when 'Add to playlist' operation has completed.
 */
void MpCollectionView::playlistSaved( bool success )
{
    if (success && mCollectionContext == ECollectionContextPlaylists) {
	      mMpxWrapper->reopenCollection();
    }
}

/*!
 Slot to be called when 'Delete' operation has completed.
 */
void MpCollectionView::songsDeleted( bool success )
{
    if ( success ) {
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
 Slot to be called when scan completes.
 */
void MpCollectionView::handleScanningComplete()
{
    TX_ENTRY
    mScanning = false;
    if ( mActivated ) {
        mMpxWrapper->reopenCollection();
    }
    else {
        activateView();
    }
    TX_EXIT
}

/*!
 Slot to be called when an item is long pressed by the user.
 */
void MpCollectionView::openContextMenu( int index, const QPointF &coords )
{
    TX_ENTRY_ARGS("index=" << index);
    switch ( mViewMode ) {
        case MpCommon::DefaultView:
            openDefaultViewContextMenu(index, coords);
            break;
        case MpCommon::FetchView:
            openFetchViewContextMenu(index, coords);
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Default view context menu.
 */
void MpCollectionView::openDefaultViewContextMenu(int index, const QPointF &coords)
{
    HbMenu *contextMenu = 0;
    HbAction *action;
    switch (mCollectionContext) {
        case ECollectionContextAllSongs:
        case ECollectionContextAlbumSongs:
            contextMenu = new HbMenu();
            action = contextMenu->addAction(QString(tr("Add to playlist")));
            action->setObjectName("add");
            action = contextMenu->addAction(QString(tr("Delete")));
            action->setObjectName("delete");
            break;
        case ECollectionContextArtistAlbums:
            contextMenu = new HbMenu();
            action = contextMenu->addAction(QString(tr("Add to playlist")));
            action->setObjectName("add");
            action = contextMenu->addAction(QString(tr("Delete")));
            action->setObjectName("delete");
            break;
        case ECollectionContextPlaylists:
            if ( !mCollectionData->isAutoPlaylist(index) ) {
                contextMenu = new HbMenu();
                action = contextMenu->addAction(QString(tr("Delete")));
                action->setObjectName("delete");
                action = contextMenu->addAction(QString(tr("Rename")));
                action->setObjectName("rename playlist");
            }
            break;
        case ECollectionContextPlaylistSongs:
            if ( !mCollectionData->isAutoPlaylist() ) {
                contextMenu = new HbMenu();
                action = contextMenu->addAction(QString(tr("Remove")));
                action->setObjectName("delete");
            }
            break;
        default:
            break;
    }

    if ( contextMenu ) {
        HbAction *selectedAction = contextMenu->exec(coords);
        if ( selectedAction ) {
            QString objectName = selectedAction->objectName();
            QList<int> selection;
            selection.append(index);
            if ( objectName == "add" ) {
                launchAddToPlaylistDialog(selection);
            }
            else if ( objectName == "delete" ) {
                requestDelete(selection);
            }
            else if ( objectName == "rename playlist" ) {
                QString currentName;
                currentName = mCollectionData->itemData(index, MpMpxCollectionData::Title);
                bool ok = false;
                QString newName;
                newName = HbInputDialog::getText(QString(tr("Enter name:")), currentName, &ok);
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
    TX_ENTRY_ARGS("index=" << index);

    HbMenu *contextMenu = 0;
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
        case ECollectionContextAlbumSongs:
        case ECollectionContextPlaylistSongs:
        case ECollectionContextGenreSongs:
            contextMenu = new HbMenu();
            contextMenu->addAction(QString(tr("Play")));
            break;
        default:
            break;
    }

    if ( contextMenu) {
        if ( contextMenu->exec(coords) ) {
            // Start the playback process. View will switch to playbackview.
            mMpxWrapper->previewItem( index );
        }
    }
    contextMenu->deleteLater();
    TX_EXIT
}

/*!
 \internal
 Sets the main (default) toolbar for the view.
 */
void MpCollectionView::setMainToolBar()
{
    TX_ENTRY
    if ( !mMainToolBar ) {
        //Create the toolbar.
        mMainToolBar = new HbToolBar();
        mMainToolBar->setOrientation(Qt::Horizontal);
        QActionGroup *actionsGroup = new QActionGroup( mMainToolBar );
        HbAction *action;
    
        // All Songs
        action = createToolBarAction(actionsGroup,
                ":/icons/all_songs_on",
                ":/icons/all_songs",
                tr("All"));
        connect( action, SIGNAL(triggered(bool)), this, SLOT(openSongs()) );
        mMainToolBar->addAction(action);
    
        // Artists
        action = createToolBarAction(actionsGroup,
                ":/icons/artists_on",
                ":/icons/artists",
                tr("Artists"));
        connect( action, SIGNAL(triggered(bool)), this, SLOT(openArtists()) );
        mMainToolBar->addAction(action);
    
        // Playlists
        action = createToolBarAction(actionsGroup,
                ":/icons/playlists_on",
                ":/icons/playlists",
                tr("Playlists"));
        connect( action, SIGNAL(triggered(bool)), this, SLOT(openPlaylists()) );
        mMainToolBar->addAction(action);
    
        // Genres
        action = createToolBarAction(actionsGroup,
                ":/icons/search_on",
                ":/icons/search",
                tr("Search"));
        connect( action, SIGNAL(triggered(bool)), this, SLOT(find()) );
        mMainToolBar->addAction(action);
    
        if ( mViewMode != MpCommon::FetchView ) {
            // Music Store
            action = createToolBarAction(actionsGroup,
                    ":/icons/ovi_on",
                    ":/icons/ovi",
                    tr("Ovi"));
            connect( action, SIGNAL(triggered(bool)), this, SLOT(openMusicStore()) );
            mMainToolBar->addAction(action);
        }
    }
    HbAction* action = 0;
    switch ( mCollectionContext ) {
        case ECollectionContextAllSongs:
            action = qobject_cast<HbAction*>(mMainToolBar->actions()[KMainToolBarAll]);
            break;
        case ECollectionContextArtistAlbums:
        case ECollectionContextAlbumSongs:
            action = qobject_cast<HbAction*>(mMainToolBar->actions()[KMainToolBarArtists]);
            break;
        case ECollectionContextPlaylists:
        case ECollectionContextPlaylistSongs:
            action = qobject_cast<HbAction*>(mMainToolBar->actions()[KMainToolBarPlaylists]);
            break;
    }
    if ( action ) {
        action->setChecked(true);
    }
    if ( toolBar() != mMainToolBar ) {
        HbToolBar *tmpToolBar = takeToolBar();
        if ( tmpToolBar && tmpToolBar->actions().empty() ) {
            tmpToolBar->deleteLater();
        }
        setToolBar(mMainToolBar);
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
        mPlaylistToolBar->setOrientation(Qt::Horizontal);
        HbAction *action;      
        
        action = new HbAction( tr("Add") );
        connect( action, SIGNAL(triggered(bool)), this, SLOT(notimplemented()) );
        mPlaylistToolBar->addAction(action);
        
        action = new HbAction( tr("Remove") );
        connect( action, SIGNAL(triggered(bool)), this, SLOT(deleteSongs()));
        mPlaylistToolBar->addAction(action);
    
        action = new HbAction( tr("Reorder") );
        connect( action, SIGNAL(triggered(bool)), this, SLOT(notimplemented()) );
        mPlaylistToolBar->addAction(action);
    }
 
    int items = mCollectionData->count();
        
    //no use for remove if there are no items.
    mPlaylistToolBar->actions()[KPlaylistToolBarRemove]->setEnabled(items > 0); 
     
    //no use for reorder if there is no more than 1 item.
    mPlaylistToolBar->actions()[KplaylistToolBarReorder]->setEnabled(items > 1);
        
    if ( toolBar() != mPlaylistToolBar ) {
        HbToolBar *tmpToolBar = takeToolBar();
        if (tmpToolBar && tmpToolBar->actions().empty ()) {
            tmpToolBar->deleteLater();
        }
        setToolBar(mPlaylistToolBar);
    }
    TX_EXIT
}

/*!
 \internal
 Creates action associated with the action group for the toolbar.
 */
HbAction *MpCollectionView::createToolBarAction(
    QActionGroup *actionsGroup,
    const QString& iconOn,
    const QString& iconOff,
    const QString& toolTip )
{
    HbIcon actionIcon(iconOff);
    // button pressed icon
    actionIcon.setIconName(iconOn, QIcon::Normal, QIcon::On );

    HbAction *action = new HbAction(actionsGroup);
    action->setToolTip(toolTip);
    action->setIcon(actionIcon);
    action->setCheckable(true);
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
        if ( mBannerAttached ) {
            connect( myMenu->addAction(tr("Go to Now Playing")), SIGNAL(triggered()), this, SLOT(startPlaybackView()) );
        }
        switch (mCollectionContext) {
            case ECollectionContextAllSongs:
                if (items) {
                    connect( myMenu->addAction(tr("Shuffle play all")), SIGNAL(triggered()), this, SLOT(shufflePlayAll()) );
                }
                connect( myMenu->addAction(tr("Refresh library")), SIGNAL(triggered()), this, SLOT(refreshLibrary()) );
                if (items) {
                    connect( myMenu->addAction(tr("Add to playlist")), SIGNAL(triggered()), this, SLOT(addToPlaylist()), Qt::QueuedConnection );
                }
                connect( myMenu->addAction(tr("Exit")), SIGNAL(triggered()), this, SLOT(exit()) );
                break;
            case ECollectionContextArtistAlbums:
                //connect( myMenu->addAction(tr("Add to playlist")), SIGNAL(triggered()), this, SLOT(addToPlaylist()), Qt::QueuedConnection );
                // Todo: View as coverflow
                break;
            case ECollectionContextAlbumSongs:
                if (items) {
                    connect( myMenu->addAction(tr("Add to playlist")), SIGNAL(triggered()), this, SLOT(addToPlaylist()), Qt::QueuedConnection );
                }
                break;
            case ECollectionContextPlaylists:
                // Todo: Create new playlist
                break;
            case ECollectionContextPlaylistSongs:
                if ( !mCollectionData->isAutoPlaylist() ) {
                    connect( myMenu->addAction(tr("Rename playlist")), SIGNAL(triggered()), this, SLOT(renameCurrentPlaylistContainer()), Qt::QueuedConnection );
                }
                break;
            default:
                break;
        }
    }
    else if (mViewMode == MpCommon::FetchView ) {
        if ( mCollectionContext == ECollectionContextAllSongs ) {
            connect( myMenu->addAction(tr("Refresh library")), SIGNAL(triggered()), this, SLOT(refreshLibrary()) );
        }
    }

    setMenu(myMenu);
    TX_EXIT
}

/*!
 \internal
 Updates the Toolbar according to current context.
 */
void MpCollectionView::updateToolBar()
{
    TX_ENTRY

    switch (mCollectionContext) {
        case ECollectionContextPlaylistSongs:
            if ( !mCollectionData->isAutoPlaylist() ) {
                setPlaylistToolBar();
            }
            else if (!toolBar()->actions().empty()) {
                takeToolBar();
                setToolBar(new HbToolBar);
            } 
            break;
        case ECollectionContextAlbumSongs:
        case ECollectionContextGenreSongs:
            if (!toolBar()->actions().empty()) {
                takeToolBar();
                setToolBar(new HbToolBar);
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
        mWindow->addSoftKeyAction(Hb::SecondarySoftKey, mSoftKeyBack);
    }
    else {
        switch ( mCollectionContext ) {
            case ECollectionContextAllSongs:
            case ECollectionContextArtistAlbums:
            case ECollectionContextPlaylists:
            case ECollectionContextGenres:
                mWindow->addSoftKeyAction(Hb::SecondarySoftKey, mSoftKeyQuit);
                break;
            default:
                mWindow->addSoftKeyAction(Hb::SecondarySoftKey, mSoftKeyBack);
                break;
        }
    }
}

/*!
 \internal
 Clears the softkey set by this view. Restore to previous.
 */
void MpCollectionView::clearSoftkey()
{
    mWindow->removeSoftKeyAction(Hb::SecondarySoftKey, mSoftKeyBack);
    mWindow->removeSoftKeyAction(Hb::SecondarySoftKey, mSoftKeyQuit);
}

/*!
 \internal
 Sets the Now Playing Banner visibility based on \a visible.
 */
void MpCollectionView::setBannerVisibility( bool visible )
{
    bool ok = false;
    if ( visible && (hbInstance->allMainWindows()[0]->orientation() == Qt::Vertical)) {
        mDocumentLoader->load(MUSIC_COLLECTION_DOCML, "showBanner", &ok);
        mNowPlayingBanner->show();
    }
    else {
        mDocumentLoader->load(MUSIC_COLLECTION_DOCML, "hideBanner", &ok);
        mNowPlayingBanner->hide();
    }

    if ( !ok ) {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X(ok, "MpCollectionView::setBannerVisibility", "invalid xml file");
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
    time(&seconds);
    srand((unsigned int) seconds);

    int index = rand() % (high - low + 1) + low;
    return index;
}

/*!
 \internal
 Launches the 'Add to playlist' dialog.
 */
void MpCollectionView::launchAddToPlaylistDialog( QList<int> selection )
{
    QString newPlaylistName;
    int playlistIndex;
    bool canceled = false;

    forever {
        QStringList list;
        mMpxWrapper->findPlaylists(list);
        if ( list.count()) {
            HbListDialog dialog;
            dialog.setStringItems(list);
            dialog.setSelectionMode(HbAbstractItemView::SingleSelection);
            dialog.setHeadingWidget(new HbLabel(QString(tr("Choose a playlist:"))));
            dialog.setPrimaryAction(new HbAction(QString(tr("New"))));
            dialog.setSecondaryAction(new HbAction(QString(tr("Cancel"))));
            HbAction *selectedAction = dialog.exec();
            if ( selectedAction == dialog.secondaryAction() ) {
                // Cancel
                break;
            }
            else if ( selectedAction != dialog.primaryAction()) {
                // User selected existing playlist
                playlistIndex = dialog.selectedItems().at(0);
                mMpxWrapper->saveToPlaylist(playlistIndex, selection);
                break;
            }
        }
        else if (canceled) {
            break;
        }

        // New - create a suggested name for the playlist
        QString suggestedPlaylistName(tr("Playlist"));
        int i = 0;
        for (;
             list.contains( QString( suggestedPlaylistName + "(" + QString::number(i) + ")" ) ) ;
             i++ ) {};
        suggestedPlaylistName += QString("(" + QString::number( i ) + ")");
        // Loop until the user cancels or enters a valid name
        forever {
            QString suggestedText;
            bool ok = false;
            suggestedText = HbInputDialog::getText(QString(tr("Enter a name for the new playlist:")), suggestedPlaylistName, &ok);
            if ( !ok ) {
                canceled = true;
                break;
            }
            if ( !list.contains(suggestedText) ) {
            	  newPlaylistName = suggestedText;
                mMpxWrapper->createPlaylist(newPlaylistName, selection);
                return;
            }
        }
    }
}

/*!
 \internal
 starts a transition of the main container with a decoy snapshot.
 */
void MpCollectionView::startContainerTransition(TCollectionContext contextFrom, TCollectionContext contextTo)
{
    if (!mSnapshot)
      mSnapshot = new MpSnapshotWidget();
    mSnapshot->capture(mWindow, mMainContainer );
    mWindow->scene()->addItem(mSnapshot);


    if ( ( contextFrom == ECollectionContextArtistAlbums && contextTo == ECollectionContextAlbumSongs ) ||
            ( contextFrom == ECollectionContextPlaylists && contextTo == ECollectionContextPlaylistSongs ) ||
            ( contextFrom == ECollectionContextGenres && contextTo == ECollectionContextGenreSongs ) ){
        HbEffect::start(mSnapshot,
                QString("container"),
                QString("slide_out_to_left"));

        HbEffect::start(mMainContainer,
                QString("container"),
                QString("slide_in_to_left_and_fade_in"),
                this,
                "containerTransitionComplete");
    }
    else if(( contextFrom == ECollectionContextAlbumSongs && contextTo == ECollectionContextArtistAlbums) ||
            ( contextFrom == ECollectionContextPlaylistSongs && contextTo == ECollectionContextPlaylists) ||
            ( contextFrom == ECollectionContextGenreSongs && contextTo == ECollectionContextGenres)) {
        HbEffect::start(mSnapshot,
                QString("container"),
                QString("slide_out_to_right"));

        HbEffect::start(mMainContainer,
                QString("container"),
                QString("slide_in_to_right_and_fade_in"),
                this,
                "containerTransitionComplete");
    }
    else {
        HbEffect::start(mSnapshot,
                QString("container"),
                QString("slide_out_to_top"));

        HbEffect::start(mMainContainer,
                QString("container"),
                QString("slide_in_to_top_and_fade_in"),
                this,
                "containerTransitionComplete");
    }

}

/*!
 \internal
 request a delete operation always it has been confirmed.
 */
void MpCollectionView::requestDelete(QList<int> selection)
{           
    bool confirmation(false);
    // Todo: Use HbMessageBox::question when time-out removed from it
    HbMessageBox dialog(HbMessageBox::MessageTypeQuestion);
    QString message;
    HbAction *action;
    
    switch (mCollectionContext) {
        case ECollectionContextAllSongs:
        case ECollectionContextAlbumSongs:
            message = QString(tr("Delete song?"));
            dialog.setText(message);
            dialog.setTimeout(HbPopup::NoTimeout);
            action = dialog.exec();
            if (action == dialog.primaryAction()) {
                confirmation = true;
            }
            break;
        case ECollectionContextArtistAlbums:
            message = QString(tr("Delete album?"));
            dialog.setText(message);
            dialog.setTimeout(HbPopup::NoTimeout);
            action = dialog.exec();
            if (action == dialog.primaryAction()) {
                confirmation = true;
            }
            break;
        case ECollectionContextPlaylists:
            message = QString(tr("Delete playlist?"));
            dialog.setText(message);
            dialog.setTimeout(HbPopup::NoTimeout);
            action = dialog.exec();
            if (action == dialog.primaryAction()) {
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
            TX_LOG_ARGS("Invalid Collection Context:" << mCollectionContext);
            break;
    }

    if ( confirmation ) {
        mMpxWrapper->deleteSongs(selection);
    }    
}
