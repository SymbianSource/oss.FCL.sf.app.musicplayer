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
* Description: Music Player details view.
*
*/
#include <QWebSettings>
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>
#include <QUrl>
#include <QDomElement>
#include <QList>
#include <QFile>
#include <QGraphicsLinearLayout>
#include <QSizeF>



#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hbpopup.h>
#include <hbdialog.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hblistviewitem.h>
#include <hbprogressbar.h>

#include "mpdetailsview.h"
#include "mpenginefactory.h"
#include "mpcommondefs.h"
#include "mpsongdata.h"
#include "mpsettingsmanager.h"
#include "mpquerymanager.h"
#include "mptrace.h"

#ifdef SHARE_FUNC_ENABLED
#include "mpdetailssharedialog.h"
#endif


const int KRecommendationCount = 2;


/*!
 Constructor
 */
MpDetailsView::MpDetailsView()
    : mMpEngine( 0 ),
      mSongData( 0 ),
      mActivated( false ),
      mSoftKeyBack( 0 ),
      mSongText( 0 ),
      mAlbumText( 0 ),
      mArtistText( 0 ),
      mAlbumArt( 0 ),
      mSongDetailsGroupBox(0),
      mInspireMeGroupBox(0),
      mShareButton(0),
      mDocumentLoader( 0 ),
      mDetailList(0),
      mInspireList(0),
      mInspireMeProgressBar(0),
      mMpQueryManager( 0 ),
      mInspireMeQueryOngoing( false ),
      mInspireMeQueryRendered( false ),
      mInspireMeOpen(true),
      mSongDetailsGbOpen(false)
#ifdef SHARE_FUNC_ENABLED
      , mSharePopup( 0 )
#endif
{
    TX_ENTRY
    bool widgetsOk = false;
    HbMainWindow *mainWindow = hbInstance->allMainWindows()[0];
    mDocumentLoader = new HbDocumentLoader();

    TX_LOG_ARGS("Document loader created")
    if(!mDocumentLoader) {
        TX_LOG_ARGS( "MpDetailsView() Error: HbDocumentLoader not constructed" );
        return;
    }

    TX_LOG_ARGS("Loading :/mpdetailsviewdocml/mpdetailsview.docml")
    mDocumentLoader->load( QString(":/mpdetailsviewdocml/mpdetailsview.docml"), &widgetsOk );
    if(!widgetsOk) {
        TX_LOG_ARGS( "MpDetailsView() Error: invalid mpdetailsview.docml" );
        return;
    }

    TX_LOG_ARGS("Loaded :/mpdetailsviewdocml/mpdetailsview.docml")
    QGraphicsWidget *widget = 0;

    widget = mDocumentLoader->findWidget( QString("content") );
    TX_LOG_ARGS( "MpDetailsView() mDocumentLoader->findWidget for <content>: " << (int)(widget) );
    setWidget(  qobject_cast<HbWidget *>(widget) );

    widget = mDocumentLoader->findWidget( QString("shareButton") );
    mShareButton = qobject_cast<HbPushButton *>(widget);
    TX_LOG_ARGS("MpDetailsView() <shareButton> widget found mShareButton: " << (int)(mShareButton) );

    widget = mDocumentLoader->findWidget( QString("trackTitle") );
    mSongText = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <trackTitle> widget found mSongText: " << (int)(mSongText) );

    widget = mDocumentLoader->findWidget( QString("artist") );
    mArtistText = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <artist> widget found mArtistText: " << (int)(mArtistText) );

    widget = mDocumentLoader->findWidget( QString("albumName") );
    mAlbumText = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <albumName> widget found mAlbumText: " << (int)(mAlbumText) );

    widget = mDocumentLoader->findWidget( QString("albumArt") );
    mAlbumArt = qobject_cast<HbLabel *>(widget);
    TX_LOG_ARGS("MpDetailsView() <albumArt> widget found mAlbumArt: " << (int)(mAlbumArt) );

    widget = mDocumentLoader->findWidget( QString("songDetails") );
    mSongDetailsGroupBox = qobject_cast<HbGroupBox *>(widget);
    TX_LOG_ARGS("MpDetailsView() <songDetails> widget found mSongDetailsGroupBox: " << (int)(mSongDetailsGroupBox) );

    widget = mDocumentLoader->findWidget( QString("songDetailsListWidget") );
    mDetailList = qobject_cast<HbListWidget *>(widget);
    TX_LOG_ARGS("MpDetailsView() <songDetailsListWidget> widget found mDetailList: " << (int)(mDetailList) );

    widget = mDocumentLoader->findWidget( QString("inspireMeGroupBox") );
    mInspireMeGroupBox = qobject_cast<HbGroupBox *>(widget);
    TX_LOG_ARGS("MpDetailsView() <inspireMeGroupBox> widget found mInspireMeGroupBox: " << (int)(mInspireMeGroupBox) );

    widget = mDocumentLoader->findWidget( QString("inspireListWidget") );
    mInspireList = qobject_cast<HbListWidget *>(widget);
    TX_LOG_ARGS("MpDetailsView() <inspireListWidget> widget found mInspireList: " << (int)(mInspireList) );

    widget = mDocumentLoader->findWidget( QString("inspireMeProgressBar") );
    mInspireMeProgressBar = qobject_cast<HbProgressBar *>(widget);
    TX_LOG_ARGS("MpDetailsView() <inspireMeProgressBar> widget found mInspireMeProgressBar: " << (int)(mInspireMeProgressBar) );
    
    // load section HideInspireMe if no ovi store suport
    bool loadingSectionOk = false;
    mDocumentLoader->load( QString(":/mpdetailsviewdocml/mpdetailsview.docml"), QString( "ShowInspireMe" ), &loadingSectionOk );    
    if( loadingSectionOk ) {
        TX_LOG_ARGS( "Loading ShowInspireMe section is successful." );
    } else {
        TX_LOG_ARGS( "Loading ShowInspireMe section fails." );
    }

    TX_EXIT
}

/*!
 Destructs the details view.
 */
MpDetailsView::~MpDetailsView()
{
    TX_ENTRY
    delete mSoftKeyBack;
    delete mDocumentLoader;
    delete mMpQueryManager;

#ifdef SHARE_FUNC_ENABLED
    closeShareDialog();
#endif
    TX_EXIT
}


/*!
 Initializes the details view. Allocates all resources needed by the view.
 */
void MpDetailsView::initializeView()
{
    TX_ENTRY

    mSoftKeyBack = new HbAction( Hb::BackNaviAction, this );

    mMpEngine = MpEngineFactory::sharedEngine();
    mSongData = mMpEngine->songData();

    // TODO: might need later
    setupMenu();
    mMpQueryManager = new MpQueryManager();

    connect( mSoftKeyBack, SIGNAL( triggered() ), this, SLOT( back() ) );
    connect( mSongData, SIGNAL( albumArtReady() ), this, SLOT( albumArtChanged() ) );
    connect( mSongData, SIGNAL( playbackInfoChanged() ), this, SLOT( handlePlaybackInfoChanged() ) );
    connect( mSongData, SIGNAL( songDetailInfoChanged() ), this, SLOT( songDetailInfoChanged() ) );
    connect( mSongDetailsGroupBox, SIGNAL( toggled( bool ) ), this, SLOT( handleDetailsGroupBoxToggled( bool ) ) );
    connect( mInspireMeGroupBox, SIGNAL( toggled( bool ) ), this, SLOT( handleInspireMeGroupBoxToggled( bool ) ) );
    connect( mMpQueryManager, SIGNAL( networkError() ), this ,SLOT( handleNetworkError() ) );
    connect( mMpQueryManager, SIGNAL(searchUrlRetrieved(const QString&)), this, SLOT(updateSharedData(const QString&)));
    connect( mMpQueryManager, SIGNAL(recommendationAlbumArtsReady()), this, SLOT(RenderInspireMeGroupBox()));

#ifdef SHARE_FUNC_ENABLED
    connect( mShareButton, SIGNAL( clicked() ), this, SLOT( share() ) );

    // Preload the share popup
    preloadShareDialog();
#endif
    TX_EXIT
}

/*!
 Activates the details view.
 */
void MpDetailsView::activateView()
{
    TX_ENTRY
    setNavigationAction( mSoftKeyBack );

    mActivated = true;
    mInspireMeOpen = MpSettingsManager::inspireMe();
    mSongDetailsGbOpen = MpSettingsManager::songDetailsGb();
    TX_LOG_ARGS( "InspireMeVal: " << mInspireMeOpen );
    TX_LOG_ARGS( "SongDetailsGbVal: " << mSongDetailsGbOpen );
    mInspireMeGroupBox->setCollapsed(!mInspireMeOpen);
    mSongDetailsGroupBox->setCollapsed(!mSongDetailsGbOpen);
    TX_EXIT
}

/*!
 Deactivates the details view.
 */
void MpDetailsView::deactivateView()
{
    TX_ENTRY
    if (mInspireMeGroupBox->isCollapsed() ) {
      MpSettingsManager::setInspireMe(false);
      } else {
      MpSettingsManager::setInspireMe(true);
      }

    if (mSongDetailsGroupBox->isCollapsed() ) {
      MpSettingsManager::setSongDetailsGb(false);
      } else {
      MpSettingsManager::setSongDetailsGb(true);
      }

    setNavigationAction( 0 );
    mActivated = false;
    TX_EXIT
}

/*!
 Setup the menu.
 */
void MpDetailsView::setupMenu()
{
    TX_ENTRY

    TX_EXIT
}

/*!
 Slot to handle back command from softkey.
 */
void MpDetailsView::back()
{
    TX_ENTRY
    emit command( MpCommon::ActivatePreviousView );
    TX_EXIT
}

/*!
 Slot to handle Album art changed.
 */
void MpDetailsView::albumArtChanged( )
{
    TX_ENTRY
    HbIcon icon;
    mSongData->albumArt( icon );
    mAlbumArt->setIcon( icon );
    TX_EXIT
}

/*!
 Slot to handle network error.
 */
void MpDetailsView::handleNetworkError()
{
    TX_ENTRY
    mInspireMeQueryOngoing = false;
    mInspireMeQueryRendered = false;
    clearInspireMe();
    mInspireMeGroupBox->setCollapsed( true );
    TX_EXIT
}

/*!
 Render inspireme groupbox after album arts downloaded
 */
void MpDetailsView::RenderInspireMeGroupBox()
{
    TX_ENTRY
    mInspireMeQueryOngoing = false;
    mInspireMeQueryRendered = true;
    mInspireMeProgressBar->hide();
    if( mMpQueryManager->recommendationAlbumArtsMap().count() ) {
        TX_LOG_ARGS( "There are recommendations." )
        // we have recommendations
        for ( int i = 0; i < KRecommendationCount; i++ ) {
            // configure the layout properties
            if(!mInspireList->count()) {
                // we havent configured the prototype before
                HbListViewItem *prototype = mInspireList->listItemPrototype();
                prototype->setGraphicsSize( HbListViewItem::LargeIcon );
            }
            // create the item
            HbListWidgetItem  *inspireMeItem = new HbListWidgetItem();
            HbIcon icon( QIcon( mMpQueryManager->recommendationAlbumArtsMap().value(mMpQueryManager->recommendationAlbumArtsLink().at( i ) ) ) );
            inspireMeItem->setIcon( icon );
            inspireMeItem->setText( mMpQueryManager->recommendationSongs().at( i ) );
            inspireMeItem->setSecondaryText( mMpQueryManager->recommendationArtists().at( i ) );
            mInspireList->addItem( inspireMeItem );
        }
    }
    else {
        TX_LOG_ARGS( "There is NO recommendation." )
        // we dont have recommendations
        // we havent configured the prototype before
        HbListViewItem *prototype = mInspireList->listItemPrototype();
        const int maxTextRowCount = 20;
        const int minTextRowCount = 1;
        prototype->setSecondaryTextRowCount(minTextRowCount,maxTextRowCount);

        HbListWidgetItem  *inspireMeItem = new HbListWidgetItem();
        QString info(tr("There are no recommendations for this track, but you can always discover new music on Ovi"));
        inspireMeItem->setText( QString( " " ) );
        inspireMeItem->setSecondaryText( info );
        mInspireList->addItem( inspireMeItem );
    }

    TX_EXIT
}

bool MpDetailsView::canQueryRecommendations() const
{
    bool result = ( ( !mSongData->album().isEmpty() ) ||
                    ( !mSongData->artist().isEmpty() ) ) &&
                  !( mInspireMeGroupBox->isCollapsed() );
    TX_LOG_ARGS( "Can query recommendations:" << result );
    return result;
}

bool MpDetailsView::canQuerySharePlayerLink() const
{
  bool result = ( !mSongData->title().isEmpty() ) &&
                ( !mSongData->artist().isEmpty() ) ;
  TX_LOG_ARGS( "Can query share player link:" << result );
  return result;
}

/*!
 Slot to handle basic song information
 */
void MpDetailsView::handlePlaybackInfoChanged()
{
    TX_ENTRY
    mMpQueryManager->clearNetworkReplies();
    clearInspireMe();
    mInspireMeQueryRendered = false;

    // Clear the song data link until new query has been made.
    mSongData->setLink( "" );

    if ( !mSongData->title().isEmpty () ) {
        mSongText->setPlainText( mSongData->title() );
    } else {
        mSongText->setPlainText( mSongData->fileName() );
    }

    if ( !mSongData->album().isEmpty () ) {
        mAlbumText->setPlainText( mSongData->album() );
    } else {
        mAlbumText->setPlainText( tr( "Unknown") );
    }

    if ( !mSongData->artist().isEmpty() ) {
        mArtistText->setPlainText( mSongData->artist() );
    } else {
        mArtistText->setPlainText( tr( "Unknown") );
    }

    if (canQuerySharePlayerLink() ) {
        mMpQueryManager->queryLocalMusicStore(mSongData->artist(),mSongData->album(),mSongData->title());
    }

    if (canQueryRecommendations()) {
        // start inspire me area progress bar
        // TODO: currently, till we get to this callback from MPX the bar not shown
        // TODO: check if inspireMe is ON, if not, dont show
        mInspireMeProgressBar->show();
        mMpQueryManager->queryInspireMeItems(mSongData->artist(),mSongData->album(),mSongData->title());
        mInspireMeQueryOngoing = true;
    }
    else {
        // metadata to query for inspire me items not available
        // show information note
      if (!mInspireMeGroupBox->isCollapsed())
        RenderInspireMeGroupBox();
    }
    TX_EXIT
}

void MpDetailsView::clearInspireMe()
{
    TX_ENTRY
    mInspireList->clear();
    mMpQueryManager->clearRecommendations();
    TX_EXIT
}
/*!
 Slot to handle detail song information
 */
void MpDetailsView::songDetailInfoChanged()
{
    TX_ENTRY
    mDetailList->clear();

    if ( !mSongData->albumTrack().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Song number" ) );
        item->setSecondaryText( mSongData->albumTrack() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->year().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Year" ) );
        item->setSecondaryText( mSongData->year() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->genre().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Genre" ) );
        item->setSecondaryText( mSongData->genre() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->composer().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Composer" ) );
        item->setSecondaryText( mSongData->composer() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->fileName().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "File name" ) );
        item->setSecondaryText( mSongData->fileName() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->mimeType().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Format" ) );
        item->setSecondaryText( mSongData->mimeType() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->duration().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Duration" ) );
        item->setSecondaryText( mSongData->duration() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->bitRate().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Bitrate" ) );
        item->setSecondaryText( mSongData->bitRate().append( " Kbps" ) );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->sampleRate().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Sampling rate" ) );
        item->setSecondaryText( mSongData->sampleRate().append( " hz" ) );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->size().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Size" ) );
        item->setSecondaryText( mSongData->size().append( " MB" ) );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->modified().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Modified" ) );
        item->setSecondaryText( mSongData->modified() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->copyright().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Copyright" ) );
        item->setSecondaryText( mSongData->copyright() );
        item->setEnabled( false );
        mDetailList->addItem( item );
    }

    if ( !mSongData->musicURL().isNull() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Web site" ) );
        item->setSecondaryText( mSongData->musicURL() );
        item->setEnabled( true );
        mDetailList->addItem( item );
    }

    if ( mSongData->isDrmProtected() ) {
        HbListWidgetItem *item = new HbListWidgetItem();
        item->setText( tr( "Licences" ) );
        item->setSecondaryText( tr( "Click for details" ) );
        item->setEnabled( true );
        mDetailList->addItem( item );
    }

    TX_EXIT
}

void MpDetailsView::handleDetailsGroupBoxToggled(bool /*state*/)
{
    TX_ENTRY
    if ( !mSongDetailsGroupBox->isCollapsed() ) {
            mInspireMeGroupBox->setCollapsed( true );
    }
    TX_EXIT
}

void MpDetailsView::handleInspireMeGroupBoxToggled(bool /*state*/)
{
    TX_ENTRY
    if ( !mInspireMeGroupBox->isCollapsed() ) {
        TX_LOG_ARGS( "InspireMe is expanded." )
        mSongDetailsGroupBox->setCollapsed( true );
        if ( mInspireMeQueryOngoing ) {
            TX_LOG_ARGS( "Query is ongoing " )
            mInspireMeProgressBar->show();
        } else {
            TX_LOG_ARGS( "Query is NOT ongoing " )
            if ( mInspireMeQueryRendered ) {
                TX_LOG_ARGS( "InspireMe is rendered already. " )
                mInspireMeProgressBar->hide();
            } else {
                if ( canQueryRecommendations() ) {
                    TX_LOG_ARGS( "InspireMe is NOT rendered yet but can query for recommendations. " )
                    mMpQueryManager->queryInspireMeItems(mSongData->artist(),mSongData->album(),mSongData->title());
                    mInspireMeProgressBar->show();
                    mInspireMeQueryOngoing = true;
                } else {
                    TX_LOG_ARGS( "InspireMe is NOT rendered yet and CANNOT query for recommendations either. " )
                    RenderInspireMeGroupBox();
                }
            }
        }
    } else {
        TX_LOG_ARGS( "InspireMe is collapsed." )
        mInspireMeProgressBar->hide();
    }
    TX_EXIT
}

#ifdef SHARE_FUNC_ENABLED
/*!
 Slot to be called when share button is clicked
 */
void MpDetailsView::share()
{
    TX_ENTRY
    createShareDialog();
    if (canQuerySharePlayerLink() )
    {
        mMpQueryManager->queryLocalMusicStore(mSongData->artist(),mSongData->album(),mSongData->title() );
    }
    TX_EXIT
}

/*!
  Method to create the share dialog on demand.
  This will cause the share web view to be created and start loading.
  */
void MpDetailsView::createShareDialog()
{
    TX_ENTRY
    if ( !mSharePopup )
    {
        mSharePopup = new MpDetailsShareDialog();
    }
    if ( !mSharePopup->isInitialized() )
    {
        connect( mSharePopup, SIGNAL( closeShareDialog() ), this, SLOT( closeShareDialog() ) );
        mSharePopup->initialize( mSongData, tr( "Unknown" ) );
    }
    TX_EXIT
}

/*!
  Method to create the share dialog on demand and preload publishing player files.
  This will construct the share dialog but it will still be in "uninitialized" state.
  */
void MpDetailsView::preloadShareDialog()
{
    TX_ENTRY
    if ( !mSharePopup )
    {
        mSharePopup = new MpDetailsShareDialog();
    }
    mSharePopup->cachePublishingPlayerFiles();
    TX_EXIT
}

/*!
 Slot to be called when ok/close button in share dialog is pressed.
 */
void MpDetailsView::closeShareDialog()
{
    TX_ENTRY
    if ( mSharePopup )
    {
        // Dialog uses WA_DeleteOnClose so no need to delete it explicitely here, just close it.
        mSharePopup->close();
        mSharePopup = NULL;
    }
    TX_EXIT
}
#endif

/*!
 Slot to handle the music store URL retrieval from the query manager.
*/
void MpDetailsView::updateSharedData(const QString& url)
{
    TX_ENTRY
    mSongData->setLink( url );
#ifdef SHARE_FUNC_ENABLED
    if ( mSharePopup )
	{
		mSharePopup->updateSharedData();
	}
#endif
    TX_EXIT
}
