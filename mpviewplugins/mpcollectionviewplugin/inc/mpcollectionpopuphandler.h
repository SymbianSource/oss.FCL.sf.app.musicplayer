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
* Description: Music Player popup handler.
*
*/

#ifndef MPCOLLECTIONPOPUPHANDLER_H
#define MPCOLLECTIONPOPUPHANDLER_H

#include <QObject>

class MpCollectionView;
class HbPopup;
class HbAction;
class MpEngine;
class QAbstractItemModel;
class MpMpxCollectionData;
class QPointF;

class MpCollectionPopupHandler : public QObject
{
    Q_OBJECT

public:
    MpCollectionPopupHandler( MpCollectionView *parent );
    virtual ~MpCollectionPopupHandler();

    void openDefaultViewContextMenu( int index, const QPointF &coords );
    void openFetchViewContextMenu( int index, const QPointF &coords );
    void openRenamePlaylistContainerDialog( const QString &currentName );
    void openRenamePlaylistItemDialog( const QString &currentName );
    void openAddSongsToPlaylist( QAbstractItemModel* model );
    void openDeleteSongs( QAbstractItemModel* model );
    void openAddToCurrentPlaylist( MpMpxCollectionData* collectionData );
    void openCreateNewPlaylist( MpMpxCollectionData* collectionData );
    void openArrangeSongs( MpMpxCollectionData* collectionData );
    void cancelOngoingPopup();

    bool showingPopup();

public slots:
    //Context Menus
    void defaultContextMenuOptionSelected( HbAction *selectedAction );
    void fetcherContextMenuOptionSelected( HbAction *selectedAction );

    //Dialogs
    void handleAddToPlaylistDialogFinished( HbAction *selectedAction );
    void handleAddToPlaylistGetTextFinished( HbAction *selectedAction );
    void handleRenamePlaylistContainer( HbAction *selectedAction );
    void handleRenamePlaylistItem( HbAction *selectedAction );
    void handleAddSongsToPlayList( HbAction *selectedAction );
    void handleDeleteSongs( HbAction *selectedAction );
    void handleAddToCurrentPlaylist( HbAction *selectedAction );
    void handleCreateNewPlaylistGetTextFinished( HbAction *selectedAction );
    void handleCreateNewPlaylistGetModelIndexesFinished( HbAction *selectedAction );
    void handleArrangeSongs( HbAction *selectedAction );
    void handleRequestDelete( HbAction *selectedAction );

    void outstandingPopupClosing();

private:
    void setOutstandingPopup( HbPopup *popup );
    void clearOutstandingPopup( HbPopup *popup );    
    void launchAddToPlaylistDialog( QList<int> &selection );
    void queryNewPlaylistName(const QStringList &playlists, const char *handler );
    void getText(const QString &label, const QString &text, const char *handler );
    void getModelIndexes( const QString &label, QAbstractItemModel* model, const char *handler );
    void launchArrangeSongsDialog( MpMpxCollectionData* collectionData );
    void requestDelete( QList<int> &selection );

private:
    MpCollectionView                *mView;                          // Not own
    HbPopup                         *mOutstandingPopup;              // Own
    MpEngine                        *mMpEngine;                      // Not own

    class MpPopupHandlerPermanentData : public QObject
    {
    public:
        MpPopupHandlerPermanentData( QObject *parent=0 );
        virtual ~MpPopupHandlerPermanentData();
        void clear();
    public:
        MpMpxCollectionData         *mIsolatedCollectionData;        // Not own
        QAbstractItemModel          *mAbstractItemModel;             // Own
        QList<int>                  mSelectedItems;
        QString                     mOriginalName;
        int                         mContextMenuIndex;
    };
    
    MpPopupHandlerPermanentData     *mPermanentData;                 // Own
};

#endif  // MPCOLLECTIONPOPUPHANDLER_H

