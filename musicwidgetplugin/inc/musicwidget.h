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

#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

// System includes
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <hbwidget.h>
#include <xqappmgr.h>

#include "mpplaybackdata.h"

// Forward declarations
class HbLabel;
class HbPushButton;
class HbIcon;
class HbMarqueeItem;
class MpEngine;
class MpAlbumCoverWidget;

/**
 * Media key identifier for push button events
 */
enum MediaKeyIdentifier{
    EPrevious = 1,
    EPlayPause,
    ENext
};

/**
 * Music Player launch source identifier
 */    
enum LaunchIdentifier{
    ELaunchFromPlay,
    ELaunchFromShortcut
};

class MusicWidget: public HbWidget
{
    Q_OBJECT
    
public:    
    MusicWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~MusicWidget();

private:
    
    /**
     * Setup graphics
     */    
    void setupUi();
    
    /**
     * Get latest song data, player state and album art 
     */      
    void refreshData();
    
    /**
     * Launch Music Player to view
     */      
    void launchMusicPlayer(int launchMode = ELaunchFromShortcut);
    
    /**
     * Similar to public slots mediaButtonPressed and mediaButtonReleased, but this oen is called internally
     * Draws the spesified button to disabled state
     */
    void mediaButtonDisabled( int aMediaKeyId );
    
    /**
     * Similar to public slots mediaButtonPressed and mediaButtonReleased, but this oen is called internally
     * Draws the spesified button to enabled state
     */    
    void mediaButtonEnabled( int aMediaKeyId );
    
    /**
     * Disable/enable widget buttons according to Music Player state
     */       
    void toggleButtons();
    
public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    
public slots:

    /**
     * Widget slots
     */    
    void onInitialize();
    void onShow();
    void onHide();
    void onUninitialize();
    
    /**
     * MpEngine slots
     */    
    void libraryAboutToUpdate();
    void libraryUpdated();
    void usbBlocked( bool blocked );    

    /**
     * MpPlaybackData slots
     */        
    void albumArtReady();
    void playbackStateChanged();
    void playbackInfoChanged();    
    
    /**
     * ?????
     */            
    bool eventFilter(QObject *target, QEvent *event);
    
    /**
     * Button specific slots
     */        
    void mediaButtonPressed( int aMediaKeyId );
    void mediaButtonReleased( int aMediaKeyId );
    
protected:
    
    /**
     * Called from mediaButtonPressed and from mediaButtonReleased slots. 
     * Calls defineMediaButton to redraw the media button
     */
    void mediaButtonEvent( MediaKeyIdentifier aMediaKeyId, QString aGraphicsId, QString aIconColor );

    /**
     * Creates the aTarget push button based on params aGraphicsId and aSuffix. 
     * Used to change the look and feel of the button based to the aState
     * param: aTarget is the push button to be changed, must not be Null
     * param: aGraphicsId defines the button background graphics
     * param: aSuffix filename suffix, see KPrevButPrefix for example from musicwidget.cpp
     */
    void defineMediaButton( HbPushButton& aTarget, QString aGraphicsId, QStringList aSuffix, QString aIconColor );

private slots:

    /**
     * Slots to be called after button press
     */    
    void prevSong();
    void playSong();
    void nextSong();
    void shortcutButton();
    
private:  
    // mShortcutButtonLayout items
    HbPushButton *mShortcutArea;

    //mSongDataLayout items
    HbWidget *mSongDataBG;
    HbLabel *mInformationSongName;
    HbMarqueeItem *mMarqueeText;
    
    // mControlButtonsLayout items
    HbPushButton *mPreviousPushButton;
    HbPushButton *mPlayPushButton;
    HbPushButton *mNextPushButton;
    
    // MusicPlayer related state identifiers
    bool mMusicPlayerNoSongData;
    bool mMusicPlayerUpdating;
    bool mMusicPlayerBlocked;
    
    // Album art for background
    MpAlbumCoverWidget *mAlbumArt;
    
    // Artist and song title identifiers 
    QString mArtist;
    QString mTitle;
    
    // Used to launch MusicPlayer
    XQApplicationManager mApplicationManager;
    
    // MusicPlayer control and state updates
    MpEngine* mMpEngine;
    MpPlaybackData* mMpPlaybackData;

};

#endif // MUSICWIDGET_H
