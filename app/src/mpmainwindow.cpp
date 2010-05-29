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
* Description: MpMainWindow implementation.
*
*/


#include <hbapplication.h>
#include <hbactivitymanager.h>
#include <hbview.h>
#include <mpxviewpluginqt.h>
#include <xqpluginloader.h>
#include <xqplugininfo.h>
#include <xqserviceutil.h>
#include <xqsharablefile.h>
#include <QTranslator>
#include <QLocale>

#include "mpmainwindow.h"
#include "mpviewbase.h"
#include "musicservices.h"
#include "mpenginefactory.h"
#include "mpsettingsmanager.h"
#include "mptrace.h"

/*!
   \class MpMainWindow
   \brief The MpMainWindow class provides Main Window functionalities.

   MpMainWindow handles activation of views

*/

/*!
    Constructs the main winsdow.
 */

MpMainWindow::MpMainWindow()
    : MpxViewFramework(),
      mCollectionViewPlugin(0),
      mPlaybackViewPlugin(0),
      mSettingsViewPlugin(0),
      mDetailsViewPlugin(0),
      mMediaWallViewPlugin(0),
      mCurrentViewPlugin(0),
      mVerticalViewType( CollectionView ),
      mMusicServices(0)
{
    TX_LOG
}

/*!
 Destructs the main window.
 */
MpMainWindow::~MpMainWindow()
{
    TX_ENTRY
    if(mMusicServices){
        delete mMusicServices;
    }
    
    if (mCollectionViewPlugin) {
        mCollectionViewPlugin->destroyView();
        delete mCollectionViewPlugin;
    }
    if (mPlaybackViewPlugin) {
        mPlaybackViewPlugin->destroyView();
        delete mPlaybackViewPlugin;
    }
    if (mSettingsViewPlugin) {
        mSettingsViewPlugin->destroyView();
        delete mSettingsViewPlugin;
    }
    if (mDetailsViewPlugin) {
        mDetailsViewPlugin->destroyView();
        delete mDetailsViewPlugin;
    }
    if (mMediaWallViewPlugin) {
        mMediaWallViewPlugin->destroyView();
        delete mMediaWallViewPlugin;
    }    
    MpEngineFactory::close();


    TX_EXIT
}


/*!
Initialize and activate the collection view
 */
void MpMainWindow::initialize( ActivityMode mode )
{
    TX_ENTRY

    //Load musicplayer translator
    QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = QString("z:/resource/qt/translations/");

    bool translatorLoaded = false;
    translatorLoaded = translator.load(path + "musicplayer_" + lang);
    TX_LOG_ARGS("Loading musicplayer translator ok=" << translatorLoaded);
    if ( translatorLoaded ) {
        qApp->installTranslator( &translator );
    }

#ifdef _DEBUG
    QList<XQPluginInfo> impls;
    XQPluginLoader::listImplementations("org.nokia.mmdt.MpxViewPlugin/1.0", impls);
    TX_LOG_ARGS("Available plugins:");
    for ( int i = 0; i < impls.size(); i++ ) {
        TX_LOG_ARGS("Plugin " << i << ": " << impls[i].uid() << ", " << impls[i].dllName() );
    }
#endif // _DEBUG

    if ( XQServiceUtil::isService() ) {
        // Music Service mode
        // Set the Collection View and Playback View to fetcher mode
        mMusicServices = new MusicServices();
        int err = connect(mMusicServices, SIGNAL(serviceActive( TUid )), this, SLOT(initializeServiceView( TUid )));
        TX_LOG_ARGS("connection error: " << err);
        XQServiceUtil::toBackground( false );
    }

    if ( !mMusicServices ) {
        MpEngineFactory::createSharedEngine();
        if ( orientation() == Qt::Vertical ) {
            // If first startup ignore shuffleAll and send to collection view to refresh library
            if ( mode == MusicMainView  || MpSettingsManager::firstStartup() ) {
                loadView(CollectionView);
                activateView(CollectionView);
                loadView(MediaWallView);
                loadView( PlaybackView );
            } else if (mode == MusicNowPlayingViewShuffleAll ) {
                MpEngineFactory::sharedEngine()->shuffleAll();
                loadView( PlaybackView );
                activateView(PlaybackView);
                loadView(CollectionView);
                loadView(MediaWallView);
            }
        }
        else {
            // If first startup ignore shuffleAll and send to refresh library
            if( mode == MusicNowPlayingViewShuffleAll && !MpSettingsManager::firstStartup() ) {
                MpEngineFactory::sharedEngine()->shuffleAll();
                mVerticalViewType = PlaybackView;
            }
            loadView(MediaWallView);
            activateView(MediaWallView);
            loadView(CollectionView);
            loadView( PlaybackView );
        }
        connect(this, SIGNAL( orientationChanged( Qt::Orientation ) ), SLOT( switchView( Qt::Orientation ) ) );
        connect( MpEngineFactory::sharedEngine(), SIGNAL( libraryUpdated() ), SLOT( handleLibraryUpdated() ) );
        MpEngineFactory::sharedEngine()->checkForSystemEvents();
        //Register to application manager to wait for activities
        HbApplication* app = qobject_cast<HbApplication*>(qApp);
        app->activityManager()->waitActivity();
        connect( app, SIGNAL( activate() ), this , SLOT( handleActivity() ) );
        
    }
    else {
        setOrientation(Qt::Vertical, true);//This sould prevent media wall activation.
    }
    TX_EXIT
}

/*!
 Slot to be called to handle the \a commandCode.
 */
void MpMainWindow::handleCommand( int commandCode )
{
    TX_ENTRY_ARGS("commandCode=" << commandCode );
    
    switch ( commandCode ) {
        case MpCommon::Exit:
            if ( mCurrentViewPlugin ) {
                disconnectView();
            }
            qApp->quit();
            break;
        case MpCommon::SendToBackground:
            lower();
            break;
        case MpCommon::ActivateCollectionView:
            activateView(CollectionView);
            break;
        case MpCommon::ActivatePlaybackView:
            activateView(PlaybackView);
            break;
        case MpCommon::ActivateSettingsView:
            activateView(SettingsView);
            break;
        case MpCommon::ActivateDetailsView:
            activateView(DetailsView);
            break;
    }
    TX_EXIT
}

/*!
 Slot to be called to switch view based on \a orientation.
 */
void MpMainWindow::switchView( Qt::Orientation orientation )
{
        if (orientation == Qt::Vertical) {
            activateView( mVerticalViewType );   
        }
        else {
            activateView(MediaWallView);
        }
}

/*!
 Activate the \a viewType view.
 */
void MpMainWindow::activateView(MpMainWindow::ViewType viewType)
{
    TX_ENTRY_ARGS("viewType=" << viewType );

    bool doTransition = true;
    Hb::ViewSwitchFlags  transitionFlags = Hb::ViewSwitchDefault;
    
    if ( mCurrentViewPlugin ) {     
        disconnectView();
        mCurrentViewPlugin->deactivateView();
        if ( viewType  == MediaWallView || mCurrentViewPlugin == mMediaWallViewPlugin ) {
            doTransition = false;
        }
        else if ( viewType  == CollectionView && mCurrentViewPlugin == mPlaybackViewPlugin ||
                  viewType  == PlaybackView && mCurrentViewPlugin == mDetailsViewPlugin ||
                  viewType  == PlaybackView && mCurrentViewPlugin == mSettingsViewPlugin ) {
            transitionFlags |= Hb::ViewSwitchUseBackAnim;
        }
        mCurrentViewPlugin = 0;
    }

    // loadView will not reload the view if is already loaded.
    mCurrentViewPlugin = loadView( viewType );
    Q_ASSERT( mCurrentViewPlugin );

    if ( mCurrentViewPlugin ) {
        if ( viewType != MediaWallView  ) {
            mVerticalViewType = viewType;
        }
        addView( reinterpret_cast<HbView*>( mCurrentViewPlugin->getView() ) );
        setCurrentView( reinterpret_cast<HbView*>( mCurrentViewPlugin->getView() ) , doTransition , transitionFlags);
        mCurrentViewPlugin->activateView();
        connectView();
    }
    TX_EXIT
}


/*!
 Connect the signals to the current active view.
 */
void MpMainWindow::connectView()
{
    TX_ENTRY

	QObject::connect(mCurrentViewPlugin,
					 SIGNAL(command(int)),
					 this,
					 SLOT(handleCommand(int)));
    TX_EXIT
}

/*!
 Disonnect the signals from the current active view.
 */
void MpMainWindow::disconnectView()
{
    TX_ENTRY

	QObject::disconnect(mCurrentViewPlugin,
					 SIGNAL(command(int)),
					 this,
					 SLOT(handleCommand(int)));
    TX_EXIT
}

/*!
 Slot to be called when Library has changed.
 */
void MpMainWindow::handleLibraryUpdated()
{
    TX_ENTRY

    // If library changed while playing back, always return to AllSongs collection view.
    if ( mPlaybackViewPlugin &&
         mCurrentViewPlugin == mPlaybackViewPlugin &&
         mCollectionViewPlugin ) {

        activateView( CollectionView );
        MpViewBase* collectionView = reinterpret_cast<MpViewBase*>(mCollectionViewPlugin->getView());
        collectionView->setDefaultView();
    }

    TX_EXIT
}

/*!
 Reimp.
 */
void 	MpMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case 16842753:
    case Qt::Key_Call:
        if (orientation () == Qt::Vertical) {
            setOrientation(Qt::Horizontal, false);
        }
        else {
            setOrientation(Qt::Vertical, false);
        }
        break;
    default:
        HbMainWindow::keyPressEvent (event);
        break;          
    }
}

void MpMainWindow::initializeServiceView( TUid hostUid )
{
    switch (mMusicServices->currentService()) {
 
    case MusicServices::EUriFetcher:
        {
            MpEngineFactory::createSharedEngine( hostUid , MpEngine::Fetch );
            loadView( CollectionView, MpCommon::FetchView );
            MpViewBase* collectionView = reinterpret_cast<MpViewBase*>(mCollectionViewPlugin->getView());
            connect(collectionView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            activateView( CollectionView );           
            loadView(PlaybackView, MpCommon::FetchView );
            MpViewBase* playbackView = reinterpret_cast<MpViewBase*>(mPlaybackViewPlugin->getView());
            connect(playbackView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            connect( MpEngineFactory::sharedEngine(), SIGNAL( libraryUpdated() ), this, SLOT( handleLibraryUpdated() ) );
            MpEngineFactory::sharedEngine()->checkForSystemEvents();
            break;
        }
    case MusicServices::EPlayback:
        {
            MpEngineFactory::createSharedEngine( hostUid , MpEngine::Embedded );
            loadView(PlaybackView, MpCommon::EmbeddedView );   
            MpViewBase* playbackView = reinterpret_cast<MpViewBase*>(mPlaybackViewPlugin->getView());
            connect(mMusicServices, SIGNAL(playReady(QString)), MpEngineFactory::sharedEngine(), SLOT(playEmbedded(QString)));
            connect(mMusicServices, SIGNAL(playReady(const XQSharableFile&)), MpEngineFactory::sharedEngine(), SLOT(playEmbedded(const XQSharableFile&)));
            connect(playbackView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            activateView( PlaybackView );
            break;
        }
    default:
        Q_ASSERT_X(false, "MpMainWindow::initializeServiceView", "undefined service");
        break;
    }
}

/*!
  loads a view if type \a  type, return the view plugin, if view is already loaded it will not reload the view.
 */
MpxViewPlugin* MpMainWindow::loadView( ViewType type, MpCommon::MpViewMode viewMode )
{
    MpxViewPlugin** plugin = 0;
    long int pluginUid = 0;    
    switch (type) { 
    case CollectionView:
        pluginUid = MpCommon::KCollectionViewUid;
        plugin = &mCollectionViewPlugin;
        break;
    case PlaybackView:
        pluginUid = MpCommon::KPlaybackViewUid;
        plugin = &mPlaybackViewPlugin;
        break;
    case SettingsView:
        pluginUid = MpCommon::KSettingsViewUid;
        plugin = &mSettingsViewPlugin;
        break;
    case DetailsView:
        pluginUid = MpCommon::KDetailsViewUid;
        plugin = &mDetailsViewPlugin;
        break;
    case MediaWallView:
        pluginUid = MpCommon::KMediaWallViewUid;
        plugin = &mMediaWallViewPlugin;
        break;
    default:
        Q_ASSERT_X(false, "MpMainWindow::loadView", "undefined view type");
        break;
    }
    Q_ASSERT( plugin && pluginUid);
    if ( plugin && !(*plugin) ) {
        //plugin was not loded before, loding plugin.
        XQPluginLoader pluginLoader( pluginUid );
        QObject* pluginInstance = pluginLoader.instance();
        (*plugin) = qobject_cast<MpxViewPlugin*>( pluginInstance )->viewPlugin();
        Q_ASSERT( *plugin );
        
        if ( *plugin ) {
            (*plugin)->createView();
            MpViewBase* view = reinterpret_cast<MpViewBase*>( (*plugin)->getView() );
            view->setTitle( mMusicServices ? mMusicServices->contextTitle() : hbTrId("txt_mus_title_music"));
            view->setViewMode( viewMode );
        }
    }
    Q_ASSERT( plugin ? *plugin : 0 );
    return plugin ? *plugin : 0;
}

/*!
  Slot to handle activity switching once the stand alone instance is running and registered 
  in the activity manager to wait for activities.
  Only running activity supported at the moment is "MusicNowPlayingView"
 */
void MpMainWindow::handleActivity()
{
    HbApplication* app = qobject_cast<HbApplication*>(qApp);
    QString activityId = app->activateId();
    
    if( !activityId.compare( "MusicNowPlayingView&launchtype=standalone" ) ) {
        if ( orientation() == Qt::Vertical ) {
            if( mVerticalViewType != PlaybackView ) {
                activateView( PlaybackView );
            }
        }
    }
    HbActivityManager* activityManager = qobject_cast<HbApplication*>(qApp)->activityManager();
    activityManager->waitActivity();
}

