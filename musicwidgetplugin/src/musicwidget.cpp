/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Music player home screen widget
*
*/

#include "musicwidget.h"

#include <QtGui>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbmarqueeitem.h>
#include <hbanchorlayout.h>
#include <hbwidget.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbcolorscheme.h>

#include "musicwidgetdocumentloader.h"
#include "mptrace.h"
#include "mpenginefactory.h"
#include "mpalbumcoverwidget.h"

// Constants
/** Docml */
const QString MUSIC_WIDGET_DOCML = ":/gfx/music_widget.docml";

/** File name suffix lists for push buttons */
const QStringList PREV_BUTTON_SUFFIXES = (QStringList() << "_l" << "_c" << "_cr");
const QStringList PLAY_BUTTON_SUFFIXES = (QStringList() << "_cl" << "_c" << "_cr");
const QStringList NEXT_BUTTON_SUFFIXES = (QStringList() << "_cl" << "_c" << "_r");

/**  Prefixes for 3 piece button background graphics */
const QLatin1String PUSH_BUTTON_NORMAL("qtg_fr_hsbutton_normal");
const QLatin1String PUSH_BUTTON_PRESSED ("qtg_fr_hsbutton_pressed");
const QLatin1String PUSH_BUTTON_DISABLED ("qtg_fr_hsbutton_disabled");

/**  Push button icon colors for each of the states (Normal, Pressed and Disabled) */
const QLatin1String ICON_COLOR_NORMAL ("qtc_button_normal");
const QLatin1String ICON_COLOR_PRESSED ("qtc_button_pressed");
const QLatin1String ICON_COLOR_DISABLED ("qtc_button_disabled");

/**  Play and Pause icons */
const QLatin1String ICON_PLAY ("qtg_mono_play");
const QLatin1String ICON_PAUSE ("qtg_mono_pause");
/**  Music Player shortcut icon */
const QLatin1String ICON_LARGE_MUSIC_PLAYER ("qtg_large_music_player");
/**  Music Player shortcut icon */
const QLatin1String ICON_FIRST_TIME_USE ("qtg_large_music_empty");
const QLatin1String ICON_DEFAULT_ART ("qtg_large_album_art");

/**  HsWidget normal background */
const QLatin1String WIDGET_BG_NORMAL ("qtg_fr_hswidget_normal");
/**  Text background */
const QLatin1String TEXT_BG_NORMAL ("qtg_fr_multimedia_trans");
/**  Separator : character and space between artist and song name */
const QLatin1String SEPARATOR_TEXT (": ");
/**  Temporary unknown artist */
const QLatin1String TEMPORARY_UNKWNOWN_TEXT ("Unknown");
/**  Localized unknown artist */
const QLatin1String UNKWNOWN_TEXT ("txt_mus_other_unknown_1");

/**  Marquee width */
const int MARQUEE_WIDTH = 170;
/**  Infinite loop value for marquee */
const int MARQUEE_LOOP_FOREVER = -1;

/**  Now playing view with random play */
const QLatin1String SERVICEREQ_FIRST_TIME_USE ("appto://10207C62?activityname=MusicNowPlayingView&launchtype=standalone&shuffle=yes");
/**  Main view */
const QLatin1String SERVICEREQ_MAIN_VIEW ("appto://10207C62?activityname=MusicMainView&launchtype=standalone");
/**  Now playing view */
const QLatin1String SERVICEREQ_NOW_PLAYING_VIEW ("appto://10207C62?activityname=MusicNowPlayingView&launchtype=standalone");

/**  DOCML object name for main widget */
const QLatin1String DOCML_CONTAINER_WIDGET ("containerWidget");
/**  DOCML object name for widget background */
const QLatin1String DOCML_BG_ICON ("widget_background_icon");
/**  DOCML object name for album art image */
const QLatin1String DOCML_ALBUM_ART ("album_art_image");
/**  DOCML object name for shortcut icon background */
const QLatin1String DOCML_SHORTCUT_ICON_BG ("shortcut_icon_background");
/**  DOCML object name for shortcut icon */
const QLatin1String DOCML_SHORTCUT_ICON ("shortcut_icon");
/**  DOCML object name for scrolling text background */
const QLatin1String DOCML_TEXT_BG ("text_bg");
/**  DOCML object name for scrolling text */
const QLatin1String DOCML_SCROLLING_TEXT ("scrolling_text");
/**  DOCML object name for button group container */
const QLatin1String DOCML_BUTTON_GROUP_CONTAINER ("button_group_container");
/**  DOCML object name for prev button */
const QLatin1String DOCML_PREV_BUTTON ("btn_previous");
/**  DOCML object name for play button */
const QLatin1String DOCML_PLAY_BUTTON ("btn_play");
/**  DOCML object name for next button */
const QLatin1String DOCML_NEXT_BUTTON ("btn_next");


MusicWidget::MusicWidget(QGraphicsItem* parent, Qt::WindowFlags flags):
    HbWidget(parent, flags),
    mShortcutArea(0),
    mSongDataBG(0),
    mInformationSongName(0),
    mMarqueeText(0),    
    mPreviousPushButton(0),
    mPlayPushButton(0),    
    mNextPushButton(0),
    mMusicPlayerNoSongData(1),
    mMusicPlayerUpdating(0),
    mMusicPlayerBlocked(0),
    mAlbumArt(0),
    mArtist(0),
    mTitle(0),    
    mMpEngine(0),
    mMpPlaybackData(0)
{
    TX_ENTRY
    
    // Setup UI
    setupUi();
    
    // Connect to MP engine and playback data
    mMpEngine = MpEngineFactory::createSharedEngine();
    Q_ASSERT_X(mMpEngine, "music_widget", "no music player engine");
    TX_LOG_ARGS("got mp engine")
    
    mMpPlaybackData = mMpEngine->playbackData();
    Q_ASSERT_X(mMpPlaybackData, "music_widget", "no playback data");
    TX_LOG_ARGS("got playback data")
        
    // Connect outgoing signals and slots
    QObject::connect(mShortcutArea, SIGNAL(clicked()), this, SLOT(shortcutButton()));    
    
    // use signal mapper to indicate button identifiers to button event slots 
    QSignalMapper* signalMapperPressed = new QSignalMapper( this ); 
    signalMapperPressed->setMapping( mPreviousPushButton, EPrevious );
    signalMapperPressed->setMapping( mPlayPushButton, EPlayPause);
    signalMapperPressed->setMapping( mNextPushButton, ENext);

    // need to use different signal mapper for pressed and released events, 
    // both have same mappings but they are mapped to different slots
    QSignalMapper* signalMapperReleased = new QSignalMapper( this );
    signalMapperReleased->setMapping( mPreviousPushButton, EPrevious );
    signalMapperReleased->setMapping( mPlayPushButton, EPlayPause);
    signalMapperReleased->setMapping( mNextPushButton, ENext);
        
    // Connect button events to signal maps
    QObject::connect(mPreviousPushButton, SIGNAL(pressed()), signalMapperPressed, SLOT (map()));
    QObject::connect(mPreviousPushButton, SIGNAL(released()), signalMapperReleased, SLOT (map()));

    QObject::connect(mPlayPushButton, SIGNAL(pressed()), signalMapperPressed, SLOT (map()));
    QObject::connect(mPlayPushButton, SIGNAL(released()), signalMapperReleased, SLOT (map()));

    QObject::connect(mNextPushButton, SIGNAL(pressed()), signalMapperPressed, SLOT (map()));
    QObject::connect(mNextPushButton, SIGNAL(released()), signalMapperReleased, SLOT (map()));

    // Connect mapper signals to self implemented slots
    QObject::connect( signalMapperPressed, SIGNAL(mapped(int)), this, SLOT(mediaButtonPressed(int)));
    QObject::connect( signalMapperReleased, SIGNAL(mapped(int)), this, SLOT(mediaButtonReleased(int)));

    // Connect clicked events
    QObject::connect(mPreviousPushButton, SIGNAL(clicked()), this, SLOT(prevSong()));
    QObject::connect(mPlayPushButton, SIGNAL(clicked()), this, SLOT(playSong()));   
    QObject::connect(mNextPushButton, SIGNAL(clicked()), this, SLOT(nextSong()));

    // MpEngine
    QObject::connect(mMpEngine, SIGNAL(libraryAboutToUpdate()), this, SLOT(libraryAboutToUpdate()));
    QObject::connect(mMpEngine, SIGNAL(libraryUpdated()), this, SLOT(libraryUpdated()));
    QObject::connect(mMpEngine, SIGNAL(usbBlocked(bool)), this, SLOT(usbBlocked(bool)));

    //MpPlaybackData
    QObject::connect(mMpPlaybackData, SIGNAL(albumArtReady()), this, SLOT(albumArtReady()));
    QObject::connect(mMpPlaybackData, SIGNAL(playbackStateChanged()), this, SLOT(playbackStateChanged()));
    QObject::connect(mMpPlaybackData, SIGNAL(playbackInfoChanged()), this, SLOT(playbackInfoChanged()));  
    
    // Initial data from Music Player get by onShow method call
    TX_EXIT
}

/*!
    Destructor
*/
MusicWidget::~MusicWidget()
{
}

/*!
 Creates UI.
 */
void MusicWidget::setupUi()
{
    TX_ENTRY
    
    // Use document loader to load the contents
    MusicWidgetDocumentLoader loader;
    bool ok = false;
    loader.load( MUSIC_WIDGET_DOCML, &ok);
    Q_ASSERT_X(ok, "music_widget", "invalid title docml file");

    TX_LOG_ARGS("loaded docml")
    
    QGraphicsLinearLayout* mainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    QGraphicsWidget* container = loader.findWidget(DOCML_CONTAINER_WIDGET);
    mainLayout->addItem(container);
    //Sets parent
    setLayout( mainLayout );

    // Load and set widget background
    HbWidget *bgWidget = qobject_cast<HbWidget*> (loader.findWidget(DOCML_BG_ICON));
    HbFrameItem *frameItem;
    if (bgWidget)
        {
        HbFrameDrawer *drawer = new HbFrameDrawer(WIDGET_BG_NORMAL,
            HbFrameDrawer::NinePieces);
        frameItem = new HbFrameItem(drawer, bgWidget);
        frameItem->setPreferredSize(bgWidget->preferredSize());
        bgWidget->setZValue(1);
        }

    // Setup album art area
    QGraphicsWidget *tmpWidgetPtr;
    tmpWidgetPtr = loader.findWidget(DOCML_ALBUM_ART);
    mAlbumArt = qobject_cast<MpAlbumCoverWidget*>(tmpWidgetPtr);
    mAlbumArt->setEnabled( false );
    mAlbumArt->setDefaultIcon( HbIcon( ICON_DEFAULT_ART ) );
        
    // Load shortcut background
    HbWidget *shortcutAreaLayout = qobject_cast<HbWidget*> (loader.findWidget(DOCML_SHORTCUT_ICON_BG));
    HbFrameItem *shortcutFrameItem;
    if (shortcutAreaLayout) {
        HbFrameDrawer *drawer = new HbFrameDrawer(WIDGET_BG_NORMAL,
            HbFrameDrawer::NinePieces);
        shortcutFrameItem = new HbFrameItem(drawer, shortcutAreaLayout);
        shortcutFrameItem->setPreferredSize(shortcutAreaLayout->preferredSize());
        shortcutAreaLayout->setZValue(3);
        }

    // Load shortcut widget and set background 
    mShortcutArea = qobject_cast<HbPushButton*> (loader.findWidget(DOCML_SHORTCUT_ICON));
    if ( mShortcutArea )
        {
        // icon needs to be put as a background so that it fills the whole button area
        HbFrameDrawer* drawer = new HbFrameDrawer(ICON_LARGE_MUSIC_PLAYER, HbFrameDrawer::OnePiece);
        mShortcutArea->setFrameBackground( drawer );
        }

    // Set song data background
    mSongDataBG = qobject_cast<HbWidget*> (loader.findWidget(DOCML_TEXT_BG));
    HbFrameItem *scrollTextItem;
    if (mSongDataBG) 
        {
        HbFrameDrawer *drawer = new HbFrameDrawer(TEXT_BG_NORMAL, 
            HbFrameDrawer::NinePieces);
        scrollTextItem = new HbFrameItem(drawer, mSongDataBG);
        scrollTextItem->setPreferredSize(mSongDataBG->preferredSize());
        mSongDataBG->setZValue(4);
        mSongDataBG->hide(); // Hide initially...
        }

    // Set scrolling song data
    mInformationSongName = qobject_cast<HbLabel*> (loader.findWidget(DOCML_SCROLLING_TEXT));
    if ( mInformationSongName )
        {
        // Setting primitive marquee item values from .css doesn't work well, set in code...
        mMarqueeText = new HbMarqueeItem(mInformationSongName);
        HbFontSpec fs(HbFontSpec::Secondary);
        mMarqueeText->setFontSpec(fs);        
        mMarqueeText->setTextColor(mInformationSongName->textColor());
        mMarqueeText->setMinimumWidth(MARQUEE_WIDTH);
        mMarqueeText->setLoopCount(MARQUEE_LOOP_FOREVER);
        }
    
    HbWidget *buttonGroupContainer = qobject_cast<HbWidget*> (loader.findWidget(
            DOCML_BUTTON_GROUP_CONTAINER));
    if (buttonGroupContainer)
        {
        // Define push buttons in  state at this point, check the player status later and update buttons if needed
        mPreviousPushButton = qobject_cast<HbPushButton*> (loader.findWidget(DOCML_PREV_BUTTON));
        defineMediaButton( *mPreviousPushButton, PUSH_BUTTON_NORMAL, PREV_BUTTON_SUFFIXES, ICON_COLOR_NORMAL );
        
        mPlayPushButton = qobject_cast<HbPushButton*> (loader.findWidget(DOCML_PLAY_BUTTON));
        defineMediaButton( *mPlayPushButton, PUSH_BUTTON_NORMAL, PLAY_BUTTON_SUFFIXES, ICON_COLOR_NORMAL );
    
        mNextPushButton = qobject_cast<HbPushButton*> (loader.findWidget(DOCML_NEXT_BUTTON));
        defineMediaButton( *mNextPushButton, PUSH_BUTTON_NORMAL, NEXT_BUTTON_SUFFIXES, ICON_COLOR_NORMAL );
        }
    
    TX_EXIT
}

/*!
    Makes the push button based on information aGraphicsId and aSuffix
 */
void MusicWidget::defineMediaButton( HbPushButton& aTarget, QString aGraphicsId, QStringList aSuffix, QString aIconColor )
    {
    TX_ENTRY
    
    TX_LOG_ARGS("graphics id: " << aGraphicsId)
    TX_LOG_ARGS("icon color: " << aIconColor)
    
    HbFrameDrawer* drawer;

    // First check if the drawer is already created for this push button
    if ( !aTarget.frameBackground()){
        // Nope, create one now
        drawer = new HbFrameDrawer(aGraphicsId, HbFrameDrawer::ThreePiecesHorizontal);
        aTarget.setFrameBackground( drawer );
        TX_LOG_ARGS("created drawer for button bg")
        }
    else {
        // Frame drawer already created, only need to update frame graphics
        drawer = aTarget.frameBackground();
        drawer->setFrameGraphicsName( aGraphicsId );
        TX_LOG_ARGS("using existing drawer for button bg")
        }

    // Set file name suffix list, so that drawer can load correct 3-piece graphic files 
    drawer->setFileNameSuffixList( aSuffix );

    // Update also the icon color
    QColor color = HbColorScheme::color(aIconColor);
    aTarget.icon().setColor( color );

    // Lastly, check if the buttton is disabled
    if ( aIconColor == ICON_COLOR_DISABLED )
        {
        aTarget.setEnabled(false);
        TX_LOG_ARGS("button disabled")
        }
    else
        {
        aTarget.setEnabled(true);
        TX_LOG_ARGS("button enabled")
        }
    
    TX_EXIT
    }

/*!

 */
void MusicWidget::mediaButtonEvent( MediaKeyIdentifier aMediaKeyId, QString aGraphicsId, QString aIconColor )
    {
    TX_ENTRY
    
    HbPushButton* target = NULL;
    QStringList suffix;
    
    switch ( aMediaKeyId )
        {
        case EPrevious: {
            TX_LOG_ARGS("previous")
            target = mPreviousPushButton;
            suffix = PREV_BUTTON_SUFFIXES;
            break;
            }
        case EPlayPause: {
            TX_LOG_ARGS("play/pause")
            target = mPlayPushButton;
            suffix = PLAY_BUTTON_SUFFIXES;
            break;
            }
        case ENext: {
            TX_LOG_ARGS("next")
            target = mNextPushButton;
            suffix = NEXT_BUTTON_SUFFIXES;
            break;
            }
        default: {
            // Do proper error handling.. should not be possible to get here ever tough
            TX_LOG_ARGS("unknown button")
            return;
            }
        }
        
    if ( target )
    	{
    	MusicWidget::defineMediaButton( *target, aGraphicsId, suffix, aIconColor );
    	}
    
    TX_EXIT
    }

/*!
   Emited from HbPushButton:pressed() signal, changes the button layout to 'Pressed'
 */
void MusicWidget::mediaButtonPressed( int aMediaKeyId )
    {
    TX_LOG_ARGS("media key: " << aMediaKeyId )
    mediaButtonEvent( (MediaKeyIdentifier)aMediaKeyId, PUSH_BUTTON_PRESSED, ICON_COLOR_PRESSED );
    }

/*!
   Emited from HbPushButton:released() signal, changes the button layout to 'Normal'
 */
void MusicWidget::mediaButtonReleased( int aMediaKeyId )
    {
    TX_LOG_ARGS("media key: " << aMediaKeyId )
    mediaButtonEvent( (MediaKeyIdentifier)aMediaKeyId, PUSH_BUTTON_NORMAL, ICON_COLOR_NORMAL );
    }

/*!
   Disables specified push button
 */
void MusicWidget::mediaButtonDisabled( int aMediaKeyId )
    {
    TX_LOG_ARGS("media key: " << aMediaKeyId )
    mediaButtonEvent( (MediaKeyIdentifier)aMediaKeyId, PUSH_BUTTON_DISABLED, ICON_COLOR_DISABLED );
    }

/*!
   Disables specified push button
 */
void MusicWidget::mediaButtonEnabled( int aMediaKeyId )
    {
    TX_LOG_ARGS("media key: " << aMediaKeyId )
    mediaButtonEvent( (MediaKeyIdentifier)aMediaKeyId, PUSH_BUTTON_NORMAL, ICON_COLOR_NORMAL );
    }

/*!
 Manually update music widget state.
 */
void MusicWidget::refreshData()
{ 
    TX_ENTRY
    // Get current player state
    playbackStateChanged();
    
    // Get current song data, updates button states
    playbackInfoChanged();

    // Get current album art
    albumArtReady();
    TX_EXIT
}

/*!
 Launch music player...
 */
void MusicWidget::launchMusicPlayer(int launchMode)
{  
    TX_ENTRY
    //TODO: Detect if MusicPlayer is running or not properly, handle case where player not available?!
    //TODO: Get service status from XQSettingManager
    //TODO: Service status from signal:
    //TODO: void statusChanged(const XQAiwInterfaceDescriptor& implementation,  ServiceStatus currentStatus);
     
    QUrl url;
    XQAiwRequest* req;
    
    if ( launchMode == ELaunchFromPlay )
        {
        TX_LOG_ARGS("launch from play")
        // Launch to now playing view, random play to be started!
        url.setUrl(SERVICEREQ_FIRST_TIME_USE);        
        }
    else if (mMusicPlayerNoSongData)
        {
        TX_LOG_ARGS("to main view")
        // Launch to main view
        url.setUrl(SERVICEREQ_MAIN_VIEW);    
        }
    else
        {
        TX_LOG_ARGS("to now playing view")
        // Launch to now playing view
        url.setUrl(SERVICEREQ_NOW_PLAYING_VIEW);
        }   
    
    req = mApplicationManager.create(url);    
    if (req == NULL)
        {
        // No handlers for the URI
        TX_LOG_ARGS("no req created")
        return;
        }
    
    req->setBackground(false);
    req->setSynchronous(false);
    
    // Set function parameters
   QList<QVariant> args;
   args << url.toString();
   req->setArguments(args);

   // Send the request
   bool res = req->send();
   if  (!res) 
   {
       // Request failed. 
      int error = req->lastError();
      // Handle error
      TX_LOG_ARGS("req send error: " << error)
   }

   delete req;
   
   TX_EXIT
}

/*!
    Return bounding rect
*/
QRectF MusicWidget::boundingRect() const
{
    return childrenBoundingRect();
}

/*!
    Return shape
*/
QPainterPath MusicWidget::shape() const
{   
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/*!
 Initializes the widget.
 */
void MusicWidget::onInitialize()
{
    TX_LOG
}

/*!
 Shows the widget.
 */
void MusicWidget::onShow()
{
    TX_LOG
    refreshData();
}

/*!
 Hides the widget.
 */
void MusicWidget::onHide()
{
    TX_LOG
}

/*!
 Uninitializes the widget.
 */
void MusicWidget::onUninitialize()
{
    TX_LOG
}

/*!
 Slot for shortcut button clicked.
 */
void MusicWidget::shortcutButton()
{
    TX_LOG
    launchMusicPlayer(ELaunchFromShortcut);
}

/*!
 Slot for previous button clicked.
 */
void MusicWidget::prevSong()
{ 
    TX_LOG
    mMpEngine->skipBackward();
}

/*!
 Slot for play button clicked. Handles both Play and Pause!
 */
void MusicWidget::playSong()
{
    TX_ENTRY
    if (mMusicPlayerNoSongData && mMpPlaybackData->playbackState() != MpPlaybackData::Playing )
        {
        //Signal music player to start playing all songs with random
        launchMusicPlayer(ELaunchFromPlay);
        }

    mMpEngine->playPause();
    TX_EXIT
}
/*!
 Slot for next button clicked.
 */
void MusicWidget::nextSong()
{
    TX_LOG
    mMpEngine->skipForward();
}

/*!

 */
bool MusicWidget::eventFilter(QObject *target, QEvent *event)
 {
    // pass the event on to the parent class
    return HbWidget::eventFilter(target, event);    
}

/*!
 MpEngine related
 */
void MusicWidget::libraryAboutToUpdate()
{
    TX_LOG
    //Refresh Library start
    mMusicPlayerUpdating = true;
    
    // Update button enabled/disabled state
    toggleButtons();
}

/*!
 MpEngine related
 */
void MusicWidget::libraryUpdated()
{
    TX_LOG
    //Refresh Library done
    mMusicPlayerUpdating = false;
    
    // Update button enabled/disabled state
    toggleButtons();
}

/*!
 MpEngine related
 */
void MusicWidget::usbBlocked( bool blocked )
{
    TX_LOG_ARGS("blocked: " << blocked)
    //Blocked state
    //Mass storage mode
    //Media transfer mode
    //Ovi Suite mode
    //Insert memory card
    
    mMusicPlayerBlocked = blocked;
    
    // Update button enabled/disabled state
    toggleButtons();
}

/*!
 MpPlaybackData related
 */
void MusicWidget::albumArtReady()
{   
    TX_ENTRY
    HbIcon icon;
    
    if ( mMusicPlayerNoSongData )
        {
        TX_LOG_ARGS("1st time album art")
        icon = HbIcon(ICON_FIRST_TIME_USE);    
        }
    else
        {
        TX_LOG_ARGS("playback data album art")
        mMpPlaybackData->albumArt( icon );
        }
    
    mAlbumArt->setIcon( icon );
    mAlbumArt->setEnabled( true );
    
    TX_EXIT
}

void MusicWidget::playbackStateChanged()
{
    TX_ENTRY
    // Set play/pause icon and start/stop marquee.   
    QString iconName;
    
    int state = mMpPlaybackData->playbackState();
    
    TX_LOG_ARGS("state: " << state)
    
    switch (state)
        {
        case MpPlaybackData::Playing:
            {
            TX_LOG_ARGS("pause icon, start marquee")
            iconName.append(ICON_PAUSE);
            mMarqueeText->startAnimation();
            break;
            }
        case MpPlaybackData::Paused:
        case MpPlaybackData::Stopped:
        case MpPlaybackData::NotPlaying:       
            {
            TX_LOG_ARGS("play icon, stop marquee")
            iconName.append(ICON_PLAY);
            mMarqueeText->stopAnimation();
            break;
            }
        }

    HbIcon iconPlayPause(iconName);
    mPlayPushButton->setIcon(iconPlayPause);
    
    TX_EXIT
}

/*!
 MpPlaybackData related
 */
void MusicWidget::playbackInfoChanged()
{ 
    TX_ENTRY
    // Initialize maqruee with something (space), to ensure
    // text updates to display as well.
    QString songData;
    
    mArtist = mMpPlaybackData->artist();
    mTitle = mMpPlaybackData->title();
    
    if ( !mTitle.length() )
        {
        mArtist.clear();
        mTitle.clear();
        songData = " ";

        //1st time launch
        mMusicPlayerNoSongData = true;
        
        TX_LOG_ARGS("no song data")
        }
    else
        {
        if ( !mArtist.length() )
            {
            //Show localized "Unknown" if there is no artist name
            
            //TODO: Widget needs own localization?!?!
            //mArtist.append( hbTrId(UNKWNOWN_TEXT) );

            //TODO: Remove temporary unknown string when localization resolved.
            mArtist.append( TEMPORARY_UNKWNOWN_TEXT );
            }
        
        songData.append(mArtist);
        songData.append(SEPARATOR_TEXT);
        songData.append(mTitle);
        
        mMusicPlayerNoSongData = false;
        
        TX_LOG_ARGS("song data received")
        }    
    
    mMarqueeText->setText(songData);
    TX_LOG_ARGS("marquee text: " << songData)

    // Show dark BG to songdata only if there is songdata...
    if ( mMusicPlayerNoSongData && mSongDataBG->isVisible() )
        {
        TX_LOG_ARGS("hide marquee bg")
        mSongDataBG->hide();
        }
    else if ( !mMusicPlayerNoSongData && !mSongDataBG->isVisible() )
        {
        TX_LOG_ARGS("show marquee bg")
        mSongDataBG->show();  
        }
    
    // Update button enabled/disabled state
    toggleButtons();
    
    TX_EXIT
}

/*!
 Toggle buttons to disabled/enabled as required.
 */
void MusicWidget::toggleButtons()
{ 
    TX_ENTRY
    // All buttons disabled if updating
    if ( mMusicPlayerUpdating )
        {
        TX_LOG_ARGS("updating")
        mediaButtonDisabled( EPlayPause );
        mediaButtonDisabled( EPrevious );
        mediaButtonDisabled( ENext );
        }
    
    // All buttons disabled if blocked
    else if ( mMusicPlayerBlocked )
        {
        TX_LOG_ARGS("blocked")
        mediaButtonDisabled( EPlayPause );
        mediaButtonDisabled( EPrevious );
        mediaButtonDisabled( ENext );
        }    
    
    // Only play button enabled if there is no song data, 1st time use
    else if ( mMusicPlayerNoSongData )
        {
        TX_LOG_ARGS("no song data")
        mediaButtonEnabled( EPlayPause );
        mediaButtonDisabled( EPrevious );
        mediaButtonDisabled( ENext );
        }
    
    // Enable all buttons if there is song data
    else
        {
        TX_LOG_ARGS("enable all buttons")
        mediaButtonEnabled( EPlayPause );      
        mediaButtonEnabled( EPrevious );
        mediaButtonEnabled( ENext );      
        }
    TX_EXIT
}
