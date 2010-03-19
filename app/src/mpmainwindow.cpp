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

#include "mpmainwindow.h"
#include "mpmediakeyhandler.h"
#include "mpcommondefs.h"
#include "mpviewbase.h"
#include "musicfetcher.h"
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
      mMediaKeyHandler(0),
      mCollectionViewPlugin(0),
      mPlaybackViewPlugin(0),
      mCurrentViewPlugin(0),
      mPreviousViewPlugin(0),
      mMusicFetcher(0)
{
    TX_LOG
}

/*!
 Destructs the main window.
 */
MpMainWindow::~MpMainWindow()
{
    TX_ENTRY
    delete mMediaKeyHandler;
    delete mMusicFetcher;

    if (mCollectionViewPlugin) {
        mCollectionViewPlugin->destroyView();
        delete mCollectionViewPlugin;
    }
    if (mPlaybackViewPlugin) {
        mPlaybackViewPlugin->destroyView();
        delete mPlaybackViewPlugin;
    }

    TX_EXIT
}


/*!
Initialize and activate the collection view
 */
void MpMainWindow::initialize()
{
    TX_ENTRY

#ifdef _DEBUG
    QList<XQPluginInfo> impls;
    XQPluginLoader::listImplementations("org.nokia.mmdt.MpxViewPlugin/1.0", impls);
    TX_LOG_ARGS("Available plugins:");
    for ( int i = 0; i < impls.size(); i++ ) {
        TX_LOG_ARGS("Plugin " << i << ": " << impls[i].uid() << ", " << impls[i].dllName() );
    }
#endif // _DEBUG

    if ( XQServiceUtil::isService() ) {
        // Music Fetcher mode
        // Set the Collection View and Playback View to fetcher mode
        mMusicFetcher = new MusicFetcher();
    }
    
    setViewSwitchingEnabled(false);
    XQPluginLoader collectionLoader(MpCommon::KCollectionViewUid);
    XQPluginLoader playbackLoader(MpCommon::KPlaybackViewUid);

    // Loading the collection plugin
    QObject* collectionInstance = collectionLoader.instance();
    TX_LOG_ARGS("collection instance: " << reinterpret_cast<int>(collectionInstance));
    mCollectionViewPlugin = qobject_cast<MpxViewPlugin*>( collectionInstance )->viewPlugin();
    TX_LOG_ARGS("collection view plugin: " << reinterpret_cast<int>(mCollectionViewPlugin));
    if ( mCollectionViewPlugin ) {
        mCollectionViewPlugin->createView();
        MpViewBase* collectionView = reinterpret_cast<MpViewBase*>(mCollectionViewPlugin->getView());
        if ( mMusicFetcher ) {
            collectionView->setViewMode(MpCommon::FetchView);
            collectionView->setTitle(mMusicFetcher->contextTitle());
            int err = connect(collectionView, SIGNAL(songSelected(QString)), mMusicFetcher, SLOT(itemSelected(QString)));
            TX_LOG_ARGS("connection error: " << err);
            err = connect(mMusicFetcher, SIGNAL(titleReady(const QString&)), collectionView, SLOT(setTitle(const QString&)));
            TX_LOG_ARGS("connection error: " << err);
        }
        else {
            collectionView->setTitle("Music");
        }
        // Collection view is the default view.
        activateView(MpMainWindow::CollectionView);
    }

    // Pre-load the playback plugin
    QObject* playbackInstance = playbackLoader.instance();
    TX_LOG_ARGS("playback instance: " << reinterpret_cast<int>(playbackInstance));
    mPlaybackViewPlugin = qobject_cast<MpxViewPlugin*>( playbackInstance )->viewPlugin();
    TX_LOG_ARGS("playback view plugin: " << reinterpret_cast<int>(mPlaybackViewPlugin));
    if ( mPlaybackViewPlugin ) {
        mPlaybackViewPlugin->createView();
        MpViewBase* playbackView = reinterpret_cast<MpViewBase*>(mPlaybackViewPlugin->getView());
        if ( mMusicFetcher ) {
            playbackView->setViewMode(MpCommon::FetchView);
            playbackView->setTitle(mMusicFetcher->contextTitle());
            int err = connect(playbackView, SIGNAL(songSelected(QString)), mMusicFetcher, SLOT(itemSelected(QString)));
            TX_LOG_ARGS("connection error: " << err);
            err = connect(mMusicFetcher, SIGNAL(titleReady(const QString&)), playbackView, SLOT(setTitle(const QString&)));
            TX_LOG_ARGS("connection error: " << err);
        }
        else {
            playbackView->setTitle("Music");
        }
    }

    mMediaKeyHandler = new MpMediaKeyHandler();
    TX_EXIT
}

/*!
 Returns the current view plugin.
 */
MpxViewPlugin* MpMainWindow::currentViewPlugin()
{
    TX_LOG
    return mCurrentViewPlugin;
}

/*!
 Handle the \a commandCode.
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
    }
    TX_EXIT
}

/*!
 Activate the \a viewType view.
 */
void MpMainWindow::activateView(MpMainWindow::ViewType viewType)
{
    TX_ENTRY_ARGS("viewType=" << viewType );

    if ( mCurrentViewPlugin ) {
        
        disconnectView();
        mCurrentViewPlugin->deactivateView();
        mPreviousViewPlugin = mCurrentViewPlugin;
        mCurrentViewPlugin = 0;
    }

    if ( viewType == MpMainWindow::CollectionView && mCollectionViewPlugin ) {
        mCurrentViewPlugin = mCollectionViewPlugin;
    }
    else if ( viewType == MpMainWindow::PlaybackView && mPlaybackViewPlugin ) {
        mCurrentViewPlugin = mPlaybackViewPlugin;
    }

    if ( mCurrentViewPlugin ) {
        addView( mCurrentViewPlugin->getView() );
        mCurrentViewPlugin->activateView();
        connectView();
    }

    if ( mPreviousViewPlugin ) {
        removeView( mPreviousViewPlugin->getView() );
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

	QObject::connect(this,
					 SIGNAL( orientationChanged( Qt::Orientation ) ),
					 mCurrentViewPlugin,
					 SLOT( orientationChange( Qt::Orientation ) ) );
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

	QObject::disconnect(this,
					 SIGNAL( orientationChanged( Qt::Orientation ) ),
					 mCurrentViewPlugin,
					 SLOT( orientationChange( Qt::Orientation ) ) );

    TX_EXIT
}

