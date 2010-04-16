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
* Description: Music Player now playing widget - Private.
*
*/
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <hbstackedlayout.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbiconitem.h>
#include <hbdocumentloader.h>
#include <hbinstance.h>
#include <hbcolorscheme.h>

#include "mpnowplayingwidget_p.h"
#include "mpnowplayingwidget.h"
#include "mpnowplayingbackend.h"
#include "mptrace.h"

const char *NOW_PLAYING_XML = ":/xml/nowplaying.docml";

/*!
    \internal
    \class MpNowPlayingWidgetPrivate
    \brief Custom HB widget that displays currently playing song information.

    This is a composite widget made out of HB widgets, it displays information
    for the current playing song on the playback engine indicated at
    construction time.
*/

/*!
 \internal
 Constructs the now playing widget private.
 */
MpNowPlayingWidgetPrivate::MpNowPlayingWidgetPrivate(long int playerId, MpNowPlayingWidget *qq )
    : q_ptr( qq ),
      mPrimaryText(0),
      mSecondaryText(0),
      mState( NotPlaying ),
      mIcon(0),
      mDocumentLoader(0)
{
    TX_ENTRY_ARGS( "Player ID =" << playerId << " Q pointer=" << ( void * )qq )
    QGraphicsWidget *widget;
    bool widgetsOk = false;

    //TODO final color resources should be qtc_multimedia_trans_normal when available
    QColor normalColor( HbColorScheme::color("foreground") );
    //TODO final color resources should be qtc_multimedia_trans_pressed when available
    QColor pressedColor( HbColorScheme::color("popupbackground") );
    
    mPlayIconNormal = new HbIcon( QString("qtg_mono_play"));
    mPlayIconNormal->setColor( normalColor );
    mPauseIconNormal = new HbIcon( QString("qtg_mono_pause"));
    mPauseIconNormal->setColor( normalColor );
    
    mPlayIconPressed = new HbIcon( QString("qtg_mono_play"));
    mPlayIconPressed->setColor( pressedColor );
    mPauseIconPressed = new HbIcon( QString("qtg_mono_pause"));
    mPauseIconPressed->setColor( pressedColor );

    mDocumentLoader = new HbDocumentLoader();
    if (mDocumentLoader) {
            mDocumentLoader->load( NOW_PLAYING_XML, &widgetsOk);
    }

    if (widgetsOk) {

        // find the main widget container and all of its child items
        widget = mDocumentLoader->findWidget(QString("nowPlayingContainer"));
        HbWidget* container = qobject_cast<HbWidget*>(widget);
        container->setParentItem(q_ptr);

        widget = mDocumentLoader->findWidget(QString("primaryText"));
        mPrimaryText = qobject_cast<HbLabel*>(widget);
        //TODO final color resource should be qtc_multimedia_trans_normal when available
        mPrimaryText->setTextColor( normalColor );
       

        widget = mDocumentLoader->findWidget(QString("secondaryText"));
        mSecondaryText = qobject_cast<HbLabel*>(widget);
        //TODO final color resource should be qtc_multimedia_trans_normal when available
        mSecondaryText->setTextColor( normalColor );
        
        widget = mDocumentLoader->findWidget(QString("playPause"));
        mIcon = qobject_cast<HbLabel*>(widget);

        HbStackedLayout *mylayout;
        mylayout = new HbStackedLayout(q_ptr);
        mylayout->addItem( container );

        q_ptr->setLayout( mylayout );
    }
    else {
        TX_LOG_ARGS("Error: invalid xml file.");
        Q_ASSERT_X(widgetsOk, "MpNowPlayingWidgetPrivate", "invalid xml file");
    }

    mBackEnd = new MpNowPlayingBackEnd( playerId );
    setEnabled(true);
    TX_EXIT
}


/*!
 \internal
 Destructs the now playing widget private.
 */
MpNowPlayingWidgetPrivate::~MpNowPlayingWidgetPrivate()
{
    TX_ENTRY
    delete mBackEnd;
    delete mPlayIconNormal;
    delete mPauseIconNormal;
    delete mPlayIconPressed;
    delete mPauseIconPressed;
    delete mDocumentLoader;
    TX_EXIT
}

/*!
 If \a enabled is true, the item is enabled; otherwise, it is disabled.
 Item is enabled by default.
 */
void MpNowPlayingWidgetPrivate::setEnabled( bool enabled )
{
    if ( enabled ) {
        connect( mBackEnd, SIGNAL(stateUpdate(SimplifiedPlayerState)),
                 this, SLOT(setState(SimplifiedPlayerState)) );

        connect( mBackEnd, SIGNAL(titleChanged(QString)),
                 mPrimaryText, SLOT(setPlainText(QString)) );

        connect( mBackEnd, SIGNAL(artistChanged(QString)),
                 mSecondaryText, SLOT(setPlainText(QString)) );
        mBackEnd->update();
    }
    else {
        disconnect( mBackEnd, SIGNAL(stateUpdate(SimplifiedPlayerState)),
                    this, SLOT(setState(SimplifiedPlayerState)) );

        disconnect( mBackEnd, SIGNAL(titleChanged(QString)),
                    mPrimaryText, SLOT(setPlainText(QString)) );

        disconnect( mBackEnd, SIGNAL(artistChanged(QString)),
                    mSecondaryText, SLOT(setPlainText(QString)) );
    }
}

/*!
 \internal
 Offers click \a event to the widget privated side, if event is consumed it returns true.
 */
bool MpNowPlayingWidgetPrivate::handleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ( mIcon->windowFrameGeometry().contains( event->pos() ) ) {
        mBackEnd->playPause();
        return true;
    }
    else {
       return false;
   }
}

/*!
 \internal
 Changes text and icon color when \a pressed
 */
void MpNowPlayingWidgetPrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event, bool pressed)
{   
    //TODO final color resources should be qtc_multimedia_trans_pressed when available
    QColor pressedColor( HbColorScheme::color("popupbackground") );
    //TODO final color resources should be qtc_multimedia_trans_normal when available
    QColor normalColor( HbColorScheme::color("foreground") );
    
    if( mIcon->windowFrameGeometry().contains( event->pos() ) && pressed) {            
        if ( mState == Playing ) {
            mIcon->setIcon( *mPauseIconPressed );
        }
        else {             
            mIcon->setIcon( *mPlayIconPressed );
        }
    }
    else if( q_ptr->rect().contains( event->pos() ) && pressed ){
        mPrimaryText->setTextColor( pressedColor );
        mSecondaryText->setTextColor( pressedColor );
    }
    else { 
        mPrimaryText->setTextColor( normalColor );
        mSecondaryText->setTextColor( normalColor );
        if( mState == Playing){        
            mIcon->setIcon( *mPauseIconNormal );
        }
        else{
            mIcon->setIcon( *mPlayIconNormal );
        }
    }
}

/*!
 \internal
 Handles theme change
 */
void MpNowPlayingWidgetPrivate::handleThemeChange()
{
    //TODO final color resources should be qtc_multimedia_trans_pressed when available
    QColor pressedColor( HbColorScheme::color("popupbackground") );
    //TODO final color resources should be qtc_multimedia_trans_normal when available
    QColor normalColor( HbColorScheme::color("foreground") );
    
    mPrimaryText->setTextColor( normalColor );
    mSecondaryText->setTextColor( normalColor );
    mPauseIconNormal->setColor( normalColor );
    mPlayIconNormal->setColor( normalColor );
    mPauseIconPressed->setColor( pressedColor );
    mPlayIconPressed->setColor( pressedColor );
    
    if( mState == Playing){               
        mIcon->setIcon( *mPauseIconNormal );
    }
    else{        
        mIcon->setIcon( *mPlayIconNormal );
    }
}

/*!
 \internal
 Sets the simplified playback \a state.
 */
void MpNowPlayingWidgetPrivate::setState( SimplifiedPlayerState state )
{
    TX_ENTRY_ARGS( "State = " << state )
    switch ( state ) {
        case NotPlaying:
            if ( mState != NotPlaying )
                emit q_ptr->playbackAttachmentChanged( false );
            break;
        case Playing:
            mIcon->setIcon(*mPauseIconNormal);
            if (mState == NotPlaying)
                emit q_ptr->playbackAttachmentChanged( true );
            break;
        case Paused:
            mIcon->setIcon(*mPlayIconNormal);
            if (mState == NotPlaying)
                emit q_ptr->playbackAttachmentChanged( true );
            break;
        default:
            break;
    }
    mState = state;
    TX_EXIT
}


