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
#include <QTranslator>
#include <QLocale>

#include "mpplaybackview.h"
#include "mpplaybackwidget.h"
#include "mpengine.h"
#include "mpplaybackdata.h"
#include "mpsettingsmanager.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpequalizerwidget.h"


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
    : mMpEngine( 0 ),
      mPlaybackData( 0 ),
      mPlaybackWidget( 0 ),
      mEqualizerWidget( new MpEqualizerWidget() ),
      mSoftKeyBack( 0 ),
      mActivated( false ),
      mPlayIcon( 0 ),
      mPauseIcon( 0 ),
      mShuffleOnIcon( 0 ),
      mShuffleOffIcon( 0 ),
      mMpTranslator( 0 ),
      mCommonTranslator( 0 )
{
    TX_LOG
}

/*!
 Destructs the playback view.
 */
MpPlaybackView::~MpPlaybackView()
{
    TX_ENTRY
    delete mSoftKeyBack;
    delete mPlayIcon;
    delete mPauseIcon;
    delete mShuffleOnIcon;
    delete mShuffleOffIcon;
    delete mEqualizerWidget;
    delete mMpTranslator;
    delete mCommonTranslator;
    TX_EXIT
}


/*!
 Initializes the playback view.
 */
void MpPlaybackView::initializeView()
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

    mSoftKeyBack = new HbAction( Hb::BackAction, this );
    connect( mSoftKeyBack, SIGNAL( triggered() ), this, SLOT( back() ) );

    mMpEngine = MpEngine::instance();
    mPlaybackData = mMpEngine->playbackData();
    connect( mPlaybackData, SIGNAL( playbackStateChanged() ),
             this, SLOT( playbackStateChanged() ) );

    mPlaybackWidget = new MpPlaybackWidget( mPlaybackData );
    connect( mPlaybackWidget, SIGNAL( setPlaybackPosition( int ) ), mMpEngine, SLOT( setPosition( int ) ) );

    setWidget( mPlaybackWidget );
    setupMenu();
    setupToolbar();

    mEqualizerWidget->prepareDialog();

    // Observe changes in settings.
    connect( MpSettingsManager::instance(), SIGNAL( shuffleChanged( bool ) ),
             this, SLOT( shuffleChanged( bool ) ) );
    connect( MpSettingsManager::instance(), SIGNAL( repeatChanged( bool ) ),
             this, SLOT( repeatChanged( bool ) ) );

    TX_EXIT
}

/*!
 Activates the playback view.
 */
void MpPlaybackView::activateView()
{
    TX_ENTRY
    mActivated = true;
    setNavigationAction( mSoftKeyBack );
    TX_EXIT
}

/*!
 Deactivates the playback view.
 */
void MpPlaybackView::deactivateView()
{
    TX_ENTRY
    if ( mEqualizerWidget->isActive() ) {
        mEqualizerWidget->close();
    }

    menu()->close();

    setNavigationAction( 0 );
    mActivated = false;
    TX_EXIT
}

/*!
 Slot to be called to activate settings view.
 */
void MpPlaybackView::startSettingsView()
{
    TX_LOG
    emit command( MpCommon::ActivateSettingsView );
}

/*!
 Slot to handle back command from softkey.
 */
void MpPlaybackView::back()
{
    TX_ENTRY
    // Stop the playback preview when returning to collection in fetch mode
    switch ( mViewMode ) {
        case MpCommon::EmbeddedView:
            TX_LOG_ARGS( "MpCommon::EmbeddedView" )
            mMpEngine->stop();
            emit songSelected( "" );
            break;
        case MpCommon::FetchView:
            TX_LOG_ARGS( "MpCommon::FetchView" )
            mMpEngine->stop(); //Intentional fallthrough        
        default: 
            emit command( MpCommon::ActivateCollectionView );
            break;
    }
    
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
void MpPlaybackView::playbackStateChanged()
{
    TX_ENTRY
    switch ( mPlaybackData->playbackState() ) {
        case MpPlaybackData::Playing:
            TX_LOG_ARGS( "MpPlaybackData::Playing" )
            mPlayPauseAction->setIcon( *mPauseIcon );
            break;
        case MpPlaybackData::Paused:
            TX_LOG_ARGS( "MpPlaybackData::Paused" )
            mPlayPauseAction->setIcon( *mPlayIcon );
            break;
        case MpPlaybackData::Stopped:
            TX_LOG_ARGS( "MpPlaybackData::Paused" )
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
 Slot to handle flip action.
 */
void MpPlaybackView::flip()
{
    TX_ENTRY
    emit command( MpCommon::ActivateDetailsView );
    TX_EXIT
}

/*!
 Slot to handle shuffle toggle.
 */
void MpPlaybackView::toggleShuffle()
{
    mMpEngine->setShuffle( !mShuffle );
    MpSettingsManager::setShuffle( !mShuffle );
}

/*!
 Slot to handle /a shuffle setting change.
 */
void MpPlaybackView::shuffleChanged( bool shuffle )
{
    mShuffle = shuffle;
    mShuffleAction->setIcon( mShuffle ? *mShuffleOnIcon : *mShuffleOffIcon );
}

/*!
 Slot to handle repeat toggle.
 */
void MpPlaybackView::toggleRepeat()
{
    mMpEngine->setRepeat( !mRepeat );
    MpSettingsManager::setRepeat( !mRepeat );
}

/*!
 Slot to handle /a repeat setting change.
 */
void MpPlaybackView::repeatChanged( bool repeat )
{
    mRepeat = repeat;
    mRepeatAction->setText( mRepeat ?  hbTrId( "txt_mus_opt_repeat_off" ) : hbTrId( "txt_mus_opt_repeat_on" ) );
}

/*!
 Slot called when a song is selected in fetch mode.
 */
void MpPlaybackView::handleSongSelected()
{
    TX_ENTRY
    mMpEngine->stop();
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
        connect( myMenu->addAction( hbTrId( "txt_mus_opt_equalizer" ) ), SIGNAL( triggered() ), this, SLOT( showEqualizerDialog() ) );
        connect( myMenu->addAction( hbTrId( "txt_mus_opt_audio_effects" ) ), SIGNAL( triggered() ), this, SLOT( startSettingsView() ) );
        mRepeatAction = myMenu->addAction( mRepeat ?  hbTrId( "txt_mus_opt_repeat_off" ) : hbTrId( "txt_mus_opt_repeat_on" ) );
        connect( mRepeatAction , SIGNAL( triggered() ), this, SLOT( toggleRepeat() ) );
        connect( myMenu->addAction( hbTrId( "txt_common_opt_exit" ) ), SIGNAL( triggered() ), this, SLOT( exit() ) );
        setMenu( myMenu );
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
    toolBar->setOrientation( Qt::Horizontal );
    QActionGroup *actionsGroup = new QActionGroup( toolBar );

    if ( mViewMode == MpCommon::DefaultView || mViewMode == MpCommon::EmbeddedView ) {
        mShuffleOnIcon = new HbIcon( "qtg_mono_shuffle" );
        mShuffleOffIcon = new HbIcon( "qtg_mono_shuffle_off" );
        mShuffleAction = new HbAction( actionsGroup );
        mShuffle = MpSettingsManager::shuffle();
        mShuffleAction->setIcon( mShuffle ? *mShuffleOnIcon : *mShuffleOffIcon );
        mShuffleAction->setCheckable( false );
        
        if ( mViewMode == MpCommon::DefaultView ) {
            connect( mShuffleAction, SIGNAL( triggered( bool ) ),
                         this, SLOT( toggleShuffle() ) );
        }
        else {
            mShuffleAction->setEnabled( false );
        }
        
        toolBar->addAction( mShuffleAction );

        HbAction *action = new HbAction( actionsGroup );
        action->setIcon( HbIcon( "qtg_mono_previous" ) );
        action->setCheckable( false );
        connect( action, SIGNAL( triggered( bool ) ),
                 mMpEngine, SLOT( skipBackward() ) );
        toolBar->addAction( action );

        mPlayPauseAction = new HbAction( actionsGroup );
        mPlayIcon = new HbIcon( "qtg_mono_play" );
        mPauseIcon = new HbIcon( "qtg_mono_pause" );
        mPlayPauseAction->setIcon( *mPlayIcon );
        mPlayPauseAction->setCheckable( false );
        connect( mPlayPauseAction, SIGNAL( triggered( bool ) ),
                 mMpEngine, SLOT( playPause() ) );
        toolBar->addAction( mPlayPauseAction );

        action = new HbAction( actionsGroup );
        action->setIcon( HbIcon( "qtg_mono_next" ) );
        action->setCheckable( false );
        connect( action, SIGNAL( triggered( bool ) ),
                 mMpEngine, SLOT( skipForward() ) );
        toolBar->addAction( action );

        HbIcon icon( "qtg_mono_info" );
        action = new HbAction( actionsGroup );
        action->setIcon( icon );
        action->setCheckable( false );
        //TODO: Remove once song details takes hostUid from engine
        if ( mViewMode == MpCommon::DefaultView ) {
            connect( action, SIGNAL( triggered( bool ) ),
                     this, SLOT( flip() ) );
        }
        else {
            action->setEnabled( false );
        }
        toolBar->addAction( action );
    }
    else {
        // Fetch mode
        HbAction *action = new HbAction( actionsGroup );
        action->setIcon( HbIcon( "qtg_mono_tick" ) );
        action->setCheckable( false );
        connect( action, SIGNAL( triggered( bool ) ),
                 this, SLOT( handleSongSelected() ) );
        toolBar->addAction( action );

        mPlayPauseAction = new HbAction( actionsGroup );
        mPlayIcon = new HbIcon( "qtg_mono_play" );
        mPauseIcon = new HbIcon( "qtg_mono_pause" );
        mPlayPauseAction->setIcon( *mPlayIcon );
        mPlayPauseAction->setCheckable( false );
        connect( mPlayPauseAction, SIGNAL( triggered( bool ) ),
                 mMpEngine, SLOT( playPause() ) );
        toolBar->addAction( mPlayPauseAction );
    }
    
    
    TX_EXIT
}

/*!
 Slot to be called to activate equalizer dialog.
 */
void MpPlaybackView::showEqualizerDialog()
{
    TX_ENTRY

    if( mEqualizerWidget )
    {
        mEqualizerWidget->exec();
    }

    TX_EXIT
}
