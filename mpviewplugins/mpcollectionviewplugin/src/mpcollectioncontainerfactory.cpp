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
* Description: Music Player collection container factory.
*
*/

#include <hbdocumentloader.h>

#include "mpcollectioncontainerfactory.h"
#include "mpcollectionview.h"
#include "mpcollectioncontainer.h"
#include "mpcollectioncontainerallsongs.h"
#include "mpcollectioncontainerartists.h"
#include "mpcollectioncontaineralbums.h"
#include "mpcollectioncontainerplaylists.h"
#include "mpcollectioncontainergenres.h"
#include "mptrace.h"

/*!
    \class MpCollectionContainerFactory
    \brief Music Player collection container factory.

    Collection container factory is responsible for creating different
    collection containers according to the current context. It is also
    responsible for making decisions whether to create a new container
    or reuse the existing container depending on the context transition.

    \sa MpCollectionContainer
*/

/*!
 Constructs the collection container factory.
 */
MpCollectionContainerFactory::MpCollectionContainerFactory( MpCollectionView *view, HbDocumentLoader *loader, QObject *parent )
    : QObject(parent),
      mView(view),
      mCurrentContainer(0),
      mCurrentContext(ECollectionContextUnknown),
      mDocumentLoader(loader)
{
    TX_LOG
}

/*!
 Destructs the collection container factory.
 */
MpCollectionContainerFactory::~MpCollectionContainerFactory()
{
    TX_LOG
}

/*!
 Creates a container for the given \a context.
 */
MpCollectionContainer *MpCollectionContainerFactory::createContainer(
    TCollectionContext context )
{
    TX_ENTRY_ARGS("context=" << context);

    switch ( context ) {
    case ECollectionContextAllSongs:
        deleteCurrentContainer();
        mCurrentContainer = new MpCollectionContainerAllSongs(mDocumentLoader);
        mCurrentContainer->initialize();
        connect( mCurrentContainer, SIGNAL(itemActivated(int)), mView, SLOT(openIndex(int)) );
        connect( mCurrentContainer, SIGNAL(itemLongPressed(int, QPointF)), mView, SLOT(openContextMenu(int, QPointF)) );
        break;
    case ECollectionContextArtists:
            if ( ( mCurrentContext != ECollectionContextArtistAlbums ) && ( mCurrentContext != ECollectionContextAlbumSongs) )
                {
                deleteCurrentContainer();
                mCurrentContainer = new MpCollectionContainerArtists(mDocumentLoader);
                mCurrentContainer->initialize();
                connect( mCurrentContainer, SIGNAL(itemActivated(int)), mView, SLOT(openIndex(int)) );
                connect( mCurrentContainer, SIGNAL(itemLongPressed(int, QPointF)), mView, SLOT(openContextMenu(int, QPointF)) );
            }
            break;
    case ECollectionContextAlbums:
        if ( mCurrentContext != ECollectionContextAlbumSongs ) {
            deleteCurrentContainer();
            mCurrentContainer = new MpCollectionContainerAlbums(mDocumentLoader);
            mCurrentContainer->initialize();
            connect( mCurrentContainer, SIGNAL(itemActivated(int)), mView, SLOT(openIndex(int)) );
            connect( mCurrentContainer, SIGNAL(itemLongPressed(int, QPointF)), mView, SLOT(openContextMenu(int, QPointF)) );
        }
        break;
    case ECollectionContextPlaylists:
        if ( mCurrentContext != ECollectionContextPlaylistSongs ) {
            deleteCurrentContainer();
            mCurrentContainer = new MpCollectionContainerPlaylists(mDocumentLoader);
            mCurrentContainer->initialize();
            connect( mCurrentContainer, SIGNAL(itemActivated(int)), mView, SLOT(openIndex(int)) );
            connect( mCurrentContainer, SIGNAL(itemLongPressed(int, QPointF)), mView, SLOT(openContextMenu(int, QPointF)) );
        }
        break;
    case ECollectionContextGenres:
        if ( mCurrentContext != ECollectionContextGenreSongs ) {
            deleteCurrentContainer();
            mCurrentContainer = new MpCollectionContainerGenres(mDocumentLoader);
            mCurrentContainer->initialize();
            connect( mCurrentContainer, SIGNAL(itemActivated(int)), mView, SLOT(openIndex(int)) );
            //connect( mCurrentContainer, SIGNAL(itemLongPressed(int, QPointF)), mView, SLOT(openContextMenu(int, QPointF)) );
        }
        break;
    case ECollectionContextAlbumSongs:
    case ECollectionContextArtistAlbums:
    case ECollectionContextArtistSongs:
    case ECollectionContextPlaylistSongs:
    case ECollectionContextGenreSongs:
        // For this contexts, reuse the same container.
        break;
    }
    TX_EXIT
    mCurrentContext = context;
    return mCurrentContainer;
}

/*!
 \internal
 */
void MpCollectionContainerFactory::deleteCurrentContainer()
{
    TX_ENTRY_ARGS("mCurrentContext=" << mCurrentContext);
    switch ( mCurrentContext ) {
    case ECollectionContextAllSongs:
        delete static_cast<MpCollectionContainerAllSongs *>(mCurrentContainer);
        break;
    case ECollectionContextArtists:
    case ECollectionContextArtistAlbums:
    case ECollectionContextArtistSongs:
        delete static_cast<MpCollectionContainerArtists *>(mCurrentContainer);
        break;
    case ECollectionContextAlbums:
    case ECollectionContextAlbumSongs:
        delete static_cast<MpCollectionContainerAlbums *>(mCurrentContainer);
        break;
    case ECollectionContextPlaylists:
    case ECollectionContextPlaylistSongs:
        delete static_cast<MpCollectionContainerPlaylists *>(mCurrentContainer);
        break;
    case ECollectionContextGenres:
    case ECollectionContextGenreSongs:
        delete static_cast<MpCollectionContainerGenres *>(mCurrentContainer);
        break;
    }
    TX_EXIT
}

