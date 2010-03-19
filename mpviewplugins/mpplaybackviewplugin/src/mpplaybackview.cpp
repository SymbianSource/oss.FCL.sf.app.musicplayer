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
* Description: Music Player Playback view.
*
*/

#include <qpainter>
#include <qtgui>

#include <hbmenu.h>
#include <hbinstance.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbicon.h>

#include "mpplaybackview.h"
#include "mpplaybackwidget.h"
#include "mpmpxpbframeworkwrapper.h"
#include "mpplaybackdata.h"
#include "mpsettingsmanager.h"
#include "mpcommondefs.h"
#include "mptrace.h"


/*!
    \class MpPlaybackView
    \brief Music Player playback view.

    Playback view provides access to music playback in the device.
    This class owns the menu and the toolbar. In addition, it is
    responsible for creating UI widgets and the underlying engine
    components and connecting them to work together.
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the playback view.
 */
MpPlaybackView::MpPlaybackView()
    : mFrameworkWrapper(0),
      mPlaybackData(0),
      mPlaybackWidget(0),
      mSoftKeyBack(0),
      mActivated(false),
      mPlayIcon(0),
      mPauseIcon(0),
      mShuffleOnIcon(0),
      mShuffleOffIcon(0)
{
    TX_LOG
}

/*!
 Destructs the playback view.
 */
MpPlaybackView::~MpPlaybackView()
{
    TX_ENTRY
    delete mFrameworkWrapper;
    delete mSoftKeyBack;
    delete mPlayIcon;
    delete mPauseIcon;
    delete mShuffleOnIcon;
    delete mShuffleOffIcon;
    TX_EXIT
}


/*!
 Initializes the playback view.
 */
void MpPlaybackView::initializeView()
{
    TX_ENTRY

    mWindow = mainWindow();

    mSoftKeyBack = new HbAction(Hb::BackAction, this);
    connect( mSoftKeyBack, SIGNAL(triggered()), this, SLOT(back()) );

    mFrameworkWrapper = new MpMpxPbFrameworkWrapper();
    mPlaybackData = mFrameworkWrapper->playbackData();
    connect( mPlaybackData, SIGNAL(playbackStateChanged()),
             this, SLOT(playbackStateChanged()) );

    mPlaybackWidget = new MpPlaybackWidget(mPlaybackData);
    connect( mPlaybackWidget, SIGNAL(setPlaybackPosition(int)), mFrameworkWrapper, SLOT( setPosition(int) ) );

    setWidget(mPlaybackWidget);
    setupMenu();
    setupToolbar();

    // Observe changes in settings.
    connect( MpSettingsManager::instance(), SIGNAL(shuffleChanged(bool)),
             this, SLOT(shuffleChanged(bool)) );
    connect( MpSettingsManager::instance(), SIGNAL(repeatChanged(bool)),
             this, SLOT(repeatChanged(bool)) );

    TX_EXIT
}

/*!
 Activates the playback view.
 */
void MpPlaybackView::activateView()
{
    TX_ENTRY
    mActivated = true;
    mWindow->addSoftKeyAction(Hb::SecondarySoftKey, mSoftKeyBack);
    TX_EXIT
}

/*!
 Deactivates the playback view.
 */
void MpPlaybackView::deactivateView()
{
    TX_ENTRY
    mWindow->removeSoftKeyAction(Hb::SecondarySoftKey, mSoftKeyBack);
    mActivated = false;
    TX_EXIT
}

/*!
 Slot to be called to activate collection view.
 */
void MpPlaybackView::startCollectionView()
{
    TX_LOG
    emit command( MpCommon::ActivateCollectionView );
}

/*!
 Slot to handle back command from softkey.
 */
void MpPlaybackView::back()
{
    TX_ENTRY
    // Stop the playback preview when returning to collection in fetch mode
    if ( mViewMode == MpCommon::FetchView ) {
        mFrameworkWrapper->stop();
    }
    emit command( MpCommon::ActivateCollectionView );
    TX_EXIT
}

/*!
 Slot to be called to exit.
 */
void MpPlaybackView::exit()
{
    TX_ENTRY
    emit command( MpCommon::Exit );
    TX_EXIT
}

/*!
 Slot to handle playback state changed.
 */
void MpPlaybackView::playbackStateChanged( )
{
    TX_ENTRY
    switch ( mPlaybackData->playbackState() ) {
        case MpPlaybackData::Playing:
            TX_LOG_ARGS("MpPlaybackData::Playing")
            mPlayPauseAction->setIcon( *mPauseIcon );
            break;
        case MpPlaybackData::Paused:
            TX_LOG_ARGS("MpPlaybackData::Paused")
            mPlayPauseAction->setIcon( *mPlayIcon );
            break;
        case MpPlaybackData::Stopped:
            TX_LOG_ARGS("MpPlaybackData::Paused")
            if ( mViewMode == MpCommon::FetchView ) {
                back();
            }
            else {
                // Treat it like pause in default mode
                mPlayPauseAction->setIcon( *mPlayIcon );
            }
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 Slot to handle shuffle toggle.
 */
void MpPlaybackView::toggleShuffle()
{
    mFrameworkWrapper->setShuffle( !mShuffle );
    MpSettingsManager::setShuffle(!mShuffle);
}

/*!
 Slot to handle /a shuffle setting change.
 */
void MpPlaybackView::shuffleChanged( bool shuffle )
{
    mShuffle = shuffle;
    mShuffleAction->setIcon( mShuffle ? *mShuffleOnIcon : *mShuffleOffIcon);
}

/*!
 Slot to handle repeat toggle.
 */
void MpPlaybackView::toggleRepeat()
{
    mFrameworkWrapper->setRepeat(!mRepeat);
    MpSettingsManager::setRepeat(!mRepeat);
}

/*!
 Slot to handle /a repeat setting change.
 */
void MpPlaybackView::repeatChanged(bool repeat)
{
    mRepeat = repeat;
    mRepeatAction->setText( mRepeat ?  tr("Repeat off") : tr("Repeat on"));
}

/*!
 Slot called when a song is selected in fetch mode.
 */
void MpPlaybackView::handleSongSelected()
{
    TX_ENTRY
    mFrameworkWrapper->stop();
    emit songSelected( mPlaybackData->uri() );
    TX_EXIT
}

/*!
 setup the menu.
 */
void MpPlaybackView::setupMenu()
{
    TX_ENTRY
    if ( mViewMode == MpCommon::DefaultView ) {
        mRepeat = MpSettingsManager::repeat();
        HbMenu* myMenu = new HbMenu();
        connect( myMenu->addAction(tr("Go to music library")), SIGNAL(triggered()), this, SLOT(startCollectionView()) );
        mRepeatAction = myMenu->addAction( mRepeat ?  tr("Repeat off") : tr("Repeat on"));
        connect( mRepeatAction , SIGNAL(triggered()), this, SLOT(toggleRepeat()) );
        connect( myMenu->addAction(tr("Exit")), SIGNAL(triggered()), this, SLOT(exit()) );
        setMenu(myMenu);
    }
    TX_EXIT
}

/*!
 setup the toolbar.
 */
void MpPlaybackView::setupToolbar()
{
    TX_ENTRY
    HbToolBar *toolBar = this->toolBar();
    toolBar->setOrientation(Qt::Horizontal);
    QActionGroup *actionsGroup = new QActionGroup( toolBar );

    if ( mViewMode == MpCommon::DefaultView ) {
        mShuffleOnIcon = new HbIcon(":/playbackviewicons/shuffle_on.svg");
        mShuffleOffIcon = new HbIcon( ":/playbackviewicons/shuffle_off.svg");
        mShuffleAction = new HbAction(actionsGroup);
        mShuffle = MpSettingsManager::shuffle();
        mShuffleAction->setIcon( mShuffle ? *mShuffleOnIcon : *mShuffleOffIcon);
        mShuffleAction->setCheckable( false );

        connect( mShuffleAction, SIGNAL(triggered(bool)),
                 this, SLOT(toggleShuffle()) );
        toolBar->addAction( mShuffleAction );

        HbAction *action = new HbAction(actionsGroup);
        action->setIcon( HbIcon( ":/playbackviewicons/prev.svg") );
        action->setCheckable( false );
        connect( action, SIGNAL(triggered(bool)),
                 mFrameworkWrapper, SLOT(skipBackward()) );
        toolBar->addAction( action );

        mPlayPauseAction = new HbAction(actionsGroup);
        mPlayIcon = new HbIcon(":/playbackviewicons/play.svg");
        mPauseIcon = new HbIcon( ":/playbackviewicons/pause.svg" );
        mPlayPauseAction->setIcon( *mPlayIcon );
        mPlayPauseAction->setCheckable( false );
        connect( mPlayPauseAction, SIGNAL(triggered(bool)),
                 mFrameworkWrapper, SLOT(playPause()) );
        toolBar->addAction( mPlayPauseAction );

        action = new HbAction(actionsGroup);
        action->setIcon( HbIcon(":/playbackviewicons/next.svg") );
        action->setCheckable( false );
        connect( action, SIGNAL(triggered(bool)),
                 mFrameworkWrapper, SLOT(skipForward()) );
        toolBar->addAction( action );

        HbIcon icon( ":/playbackviewicons/info.svg" );
        action = new HbAction(actionsGroup);
        action->setIcon( icon );
        action->setCheckable( false );
    /*
        connect( action, SIGNAL(triggered(bool)),
                 this, SLOT( flip()) );
    */
        toolBar->addAction( action );
    }
    else {
        // Fetch mode
        HbAction *action = new HbAction(actionsGroup);
        action->setIcon( HbIcon(":/playbackviewicons/select.png") );
        action->setCheckable( false );
        connect( action, SIGNAL(triggered(bool)),
                 this, SLOT(handleSongSelected()) );
        toolBar->addAction( action );
    
        mPlayPauseAction = new HbAction(actionsGroup);
        mPlayIcon = new HbIcon(":/playbackviewicons/play.svg");
        mPauseIcon = new HbIcon( ":/playbackviewicons/pause.svg" );
        mPlayPauseAction->setIcon( *mPlayIcon );
        mPlayPauseAction->setCheckable( false );
        connect( mPlayPauseAction, SIGNAL(triggered(bool)),
                 mFrameworkWrapper, SLOT(playPause()) );
        toolBar->addAction( mPlayPauseAction );
    }
    TX_EXIT
}

