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

#include <hbview.h>
#include <hbeffect.h>

#include "mpmpxcollectionviewdefs.h"
#include "mpviewbase.h"

class QActionGroup;
class HbMainWindow;
class HbAction;
class HbMenu;
class HbToolBar;
class MpCollectionDocumentLoader;
class MpCollectionContainerFactory;
class MpCollectionContainer;
class MpCollectionDataModel;
class MpMpxFrameworkWrapper;
class MpMpxCollectionData;
class MpNowPlayingWidget;
class MpSnapshotWidget;
class MpCollectionSongScanner;

class MpCollectionView : public MpViewBase
{
    Q_OBJECT

public:

    MpCollectionView();
    virtual ~MpCollectionView();

    void initializeView();
    void activateView();
    void deactivateView();

    void orientationChange( Qt::Orientation orientation );

signals:

    void command( int command );

public slots:

    void setContext( TCollectionContext context );

    void openSongs();
    void openArtists();
    void openPlaylists();
    void openGenres();
    void find();
    void openMusicStore();

    void openIndex( int index );

    void back();
    void exit();

    void startPlaybackView();
    void attachNowPlayingBanner( bool active );
    void nowPlayingBannerActivated();
    void nowPlayingBannerChosenFxComplete1( const HbEffect::EffectStatus &status );
    void nowPlayingBannerChosenFxComplete2( const HbEffect::EffectStatus &status );
    void containerTransitionComplete( const HbEffect::EffectStatus &status );

    void shufflePlayAll();
    void refreshLibrary();
    void addToPlaylist();
    void deleteSongs();
    void renameCurrentPlaylistContainer();

    void playlistSaved( bool success );
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );

    void handleScanningComplete();

    void openContextMenu( int index, const QPointF &coords );

private:

    void openDefaultViewContextMenu( int index, const QPointF &coords );
    void openFetchViewContextMenu( int index, const QPointF &coords );

    void setMainToolBar();
    void setPlaylistToolBar();

    HbAction *createToolBarAction( QActionGroup *actionsGroup,
        const QString& iconOn,
        const QString& iconOff,
        const QString& toolTip );

    void updateMenu();
    void updateToolBar();
    void setSoftkey();
    void clearSoftkey();

    void setBannerVisibility( bool visible );
    int generateShuffleIndex();
    void launchAddToPlaylistDialog( QList<int> selection );
    void startContainerTransition( TCollectionContext contextFrom, TCollectionContext contextTo );
    void requestDelete(QList<int> selection);

private:

    TCollectionContext              mCollectionContext;

    MpMpxFrameworkWrapper           *mMpxWrapper;           // Own
    MpMpxCollectionData             *mCollectionData;       // Not own

    MpCollectionContainerFactory    *mContainerFactory;     // Own
    MpCollectionContainer           *mCollectionContainer;  // Not own
    MpCollectionDataModel           *mCollectionDataModel;  // Own

    bool                            mActivated;
    MpNowPlayingWidget              *mNowPlayingBanner;     // Own
    bool                            mBannerAttached;
    bool                            mEffectOnGoing;

    HbMainWindow                    *mWindow;               // Not own
    HbAction                        *mSoftKeyQuit;          // Not own
    HbAction                        *mSoftKeyBack;          // Not own

    MpCollectionDocumentLoader      *mDocumentLoader;       // Own
    HbWidget                        *mMainContainer;        // Own

    HbToolBar                       *mMainToolBar;
    HbToolBar                       *mPlaylistToolBar;

    MpSnapshotWidget                *mSnapshot;
    MpCollectionSongScanner         *mSongScanner;          // Own
    bool                            mScanning;

};

#endif  // MPCOLLECTIONVIEW_H

