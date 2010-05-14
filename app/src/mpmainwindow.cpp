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
#include "mpengine.h"
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

    MpEngine::instance()->close();


    TX_EXIT
}


/*!
Initialize and activate the collection view
 */
void MpMainWindow::initialize()
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

    MpEngine *engine = MpEngine::instance();

    if ( !mMusicServices ) {
        engine->initialize(MpCommon::DefaultView);
      
        loadView(CollectionView);
        activateView(CollectionView);

        connect(this, SIGNAL( orientationChanged( Qt::Orientation ) ), SLOT( switchView( Qt::Orientation ) ) );
        connect( engine, SIGNAL( libraryUpdated() ), SLOT( handleLibraryUpdated() ) );
        engine->checkForSystemEvents();
        loadView( PlaybackView );
    }
    setOrientation(Qt::Vertical, true);//This sould prevent media wall activation.
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
        if ( viewType  == CollectionView && mCurrentViewPlugin == mPlaybackViewPlugin ||
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
        
        mVerticalViewType = viewType;
        
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


void MpMainWindow::initializeServiceView( TUid hostUid ){
    
    MpEngine *engine = MpEngine::instance();
        
    switch (mMusicServices->currentService()) {
 
    case MusicServices::EUriFetcher:
        {
            engine->initialize( MpCommon::FetchView, hostUid );
            loadView( CollectionView, MpCommon::FetchView );
            MpViewBase* collectionView = reinterpret_cast<MpViewBase*>(mCollectionViewPlugin->getView());
            connect(collectionView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            activateView( CollectionView );           
            loadView(PlaybackView, MpCommon::FetchView );
            MpViewBase* playbackView = reinterpret_cast<MpViewBase*>(mPlaybackViewPlugin->getView());
            connect(playbackView, SIGNAL(songSelected(QString)), mMusicServices, SLOT(itemSelected(QString)));
            connect( engine, SIGNAL( libraryUpdated() ), this, SLOT( handleLibraryUpdated() ) );
            engine->checkForSystemEvents();
            break;
        }
    case MusicServices::EPlayback:
        {
            engine->initialize( MpCommon::EmbeddedView, hostUid );
            loadView(PlaybackView, MpCommon::EmbeddedView );   
            MpViewBase* playbackView = reinterpret_cast<MpViewBase*>(mPlaybackViewPlugin->getView());
            connect(mMusicServices, SIGNAL(playReady(QString)), engine, SLOT(playEmbedded(QString)));
            connect(mMusicServices, SIGNAL(playReady(const XQSharableFile&)), engine, SLOT(playEmbedded(const XQSharableFile&)));
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

