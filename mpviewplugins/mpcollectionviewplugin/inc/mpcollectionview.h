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

#ifndef MPCOLLECTIONVIEW_H
#define MPCOLLECTIONVIEW_H

#include <QObject>

#include <hbeffect.h>

#include "mpviewbase.h"
#include "mpmpxcollectionviewdefs.h"

class QActionGroup;
class HbMainWindow;
class HbAction;
class HbMenu;
class HbToolBar;
class MpCollectionDocumentLoader;
class MpCollectionContainerFactory;
class MpCollectionContainer;
class MpCollectionDataModel;
class MpEngine;
class MpMpxCollectionData;
class MpNowPlayingWidget;
class MpSnapshotWidget;
class MpCollectionPopupHandler;
class QTranslator;

class MpCollectionView : public MpViewBase
{
    Q_OBJECT

public:

    MpCollectionView();
    virtual ~MpCollectionView();

    void initializeView();
    void activateView();
    void deactivateView();
    void setDefaultView();
    bool isActivated();    
    void openItem( int index );

signals:

    void command( int command );

public slots:

    void setContext( TCollectionContext context );

    void openSongs();
    void openArtists();
    void openAlbums();
    void openPlaylists();
    void openGenres();
    void find();
    void openMusicStore();

    void openIndex( int index );

    void back();
    void exit();

    void startPlaybackView();
    void attachNowPlayingBanner( bool active );
    void containerTransitionComplete( const HbEffect::EffectStatus &status );

    void shufflePlayAll();
    void refreshLibrary();
    void addToPlaylist();
    void deleteSongs();
    void renameCurrentPlaylistContainer();

    void playlistSaved( bool success );
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );

    void prepareToAddToPlaylist();
    void handleIsolatedCollectionOpened( MpMpxCollectionData* collectionData );
    void arrangeSongs();
    void openContextMenu( int index, const QPointF &coords );

    void handleUsbBlocked( bool blocked );
    void handleLibraryAboutToUpdate();
    void handleLibraryUpdated();

private:

    void setMainToolBar();
    void setPlaylistToolBar();

    HbAction *createToolBarAction( QActionGroup *actionsGroup,
        const QString& icon );

    void updateMenu();
    void updateToolBar();
    void setSoftkey();

    void setBannerVisibility( bool visible );
    int generateShuffleIndex();
    void startContainerTransition( TCollectionContext contextFrom, TCollectionContext contextTo );
    void cancelOngoingOperation();

private:

    TCollectionContext              mCollectionContext;

    MpEngine                        *mMpEngine;             // Not own
    MpMpxCollectionData             *mCollectionData;       // Not own

    MpCollectionContainerFactory    *mContainerFactory;     // Own
    MpCollectionContainer           *mCollectionContainer;  // Not own
    MpCollectionDataModel           *mCollectionDataModel;  // Own

    bool                            mActivated;
    MpNowPlayingWidget              *mNowPlayingBanner;     // Own
    bool                            mBannerAttached;

    HbMainWindow                    *mWindow;               // Not own
    HbAction                        *mNavigationQuit;       // Not own
    HbAction                        *mNavigationBack;       // Not own

    MpCollectionDocumentLoader      *mDocumentLoader;       // Own
    HbWidget                        *mMainContainer;        // Own

    HbToolBar                       *mMainToolBar;
    HbToolBar                       *mPlaylistToolBar;

    MpSnapshotWidget                *mSnapshot;

    QTranslator                     *mMpTranslator;         // Own
    QTranslator                     *mCommonTranslator;     // Own

    bool                            mActivationWaiting;
    MpCollectionPopupHandler        *mMpPopupHandler;       // Own

    bool                            mUsbBlocked;

};

#endif  // MPCOLLECTIONVIEW_H
