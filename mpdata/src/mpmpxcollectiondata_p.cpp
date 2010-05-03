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
* Description: Music Player collection data - private implementation.
*
*/

#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include <mpxmedia.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>

#include "mpmpxcollectiondata_p.h"
#include "mptrace.h"

_LIT( KSong, " song" );
_LIT( KSongs, " songs" );

/*!
    \class MpMpxCollectionDataPrivate
    \brief Music Player collection data - private implementation.

    This is a private implementation of the collection data interface.
*/

/*!
 \internal
 */
MpMpxCollectionDataPrivate::MpMpxCollectionDataPrivate( MpMpxCollectionData *wrapper )
    : q_ptr( wrapper ),
      iContainerMedia(0),
      iMediaArray(0),
      iContext( ECollectionContextUnknown ),
      iCachedRemovedItem ( 0 )
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxCollectionDataPrivate::~MpMpxCollectionDataPrivate()
{
    TX_ENTRY
    delete iContainerMedia;
    delete iCachedRemovedItem;
    TX_EXIT
}

/*!
 \internal
 */
TCollectionContext MpMpxCollectionDataPrivate::context() const
{
    return iContext;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::count() const
{
    if ( iMediaArray ) {
        return iMediaArray->Count();
    }
    return 0;
}

/*!
 \internal
 */
QString MpMpxCollectionDataPrivate::collectionTitle() const
{
    TX_ENTRY
    QString title;
    if ( iContainerMedia && iContainerMedia->IsSupported( KMPXMediaGeneralTitle ) ) {
        const TDesC& titleText = iContainerMedia->ValueText( KMPXMediaGeneralTitle );
        if ( titleText.Compare( KNullDesC ) != 0 ) {
            title = QString::fromUtf16( titleText.Ptr(), titleText.Length() );
        }
    }
    TX_EXIT_ARGS("title =" << title);
    return title;
}

/*!
 \internal
 */
QString MpMpxCollectionDataPrivate::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    TX_ENTRY_ARGS("index=" << index << ", type=" << type);
    QString data;
    TRAPD(err, DoGetDataL(index, type, data));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return data;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::isAutoPlaylist()
{
    TX_ENTRY
    if ( iContext != ECollectionContextPlaylistSongs ) {
        TX_EXIT
        return false;
    }

    bool isAuto = false;
    TRAPD(err, isAuto = DoIsAutoPlaylistL());
    if ( err == KErrNone ) {
        TX_LOG_ARGS("isAuto=" << isAuto);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return isAuto;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::isAutoPlaylist( int index )
{
    TX_ENTRY_ARGS("index=" << index);
    if ( iContext != ECollectionContextPlaylists ) {
        TX_EXIT
        return false;
    }

    bool isAuto = false;
    TRAPD(err, isAuto = DoIsAutoPlaylistL(index));
    if ( err == KErrNone ) {
        TX_LOG_ARGS("isAuto=" << isAuto);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return isAuto;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::itemCount( int index )
{
    TX_ENTRY_ARGS("index=" << index);
    int count = 0;
    TRAPD(err, count = DoGetItemCountL(index));
    if ( err == KErrNone ) {
        TX_LOG_ARGS("count=" << count);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return count;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::containerId()
{
    int id = -1;
    TRAPD( err, id = DoGetContainerIdL() );
    if ( err == KErrNone ) {
        TX_LOG_ARGS("id=" << id);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return id;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::itemId(int index)
{
    TX_ENTRY_ARGS("index=" << index);
    int id = -1;
    TRAPD(err, id = DoGetItemIdL(index));
    if ( err == KErrNone ) {
        TX_LOG_ARGS("id=" << id);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return id;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::removeItem(int index)
{
    TX_ENTRY_ARGS("index=" << index);
     TRAPD(err,  DoRemoveItemL(index));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::testCachedItem( int itemId )
{
    TX_ENTRY_ARGS( "itemId=" << itemId);
    bool match = false;
    TRAPD( err, match = DoTestCachedItemL( itemId ) );
    if ( err == KErrNone ) {
        TX_LOG_ARGS("match=" << match);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
    return match;
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::insertCachedItem(int index)
{
    TX_ENTRY_ARGS("index=" << index);
    iMediaArray->Insert( iCachedRemovedItem, index );
    iCachedRemovedItem = 0; //ownership tranferred above.
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::setMpxMedia( const CMPXMedia& entries )
{
    TX_ENTRY
    TRAPD(err, DoSetMpxMediaL(entries));
    if ( err == KErrNone ) {
        TX_LOG_ARGS("Context changed: iContext=" << iContext);
        emit q_ptr->contextChanged(iContext);
    }
    else {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoGetDataL( int index, MpMpxCollectionData::DataType type, QString& data ) const
{
    TX_ENTRY
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );

    TBuf<256> countBuf;
    TBuf<20> temp;
    TInt count = 0;
    switch ( type ) {
        case MpMpxCollectionData::Title:
            if ( currentMedia->IsSupported( KMPXMediaGeneralTitle ) ) {
                const TDesC& title = currentMedia->ValueText( KMPXMediaGeneralTitle );
                if ( title.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( title.Ptr(), title.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Artist:
            if ( currentMedia->IsSupported( KMPXMediaMusicArtist ) ) {
                const TDesC& artist = currentMedia->ValueText( KMPXMediaMusicArtist );
                if ( artist.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( artist.Ptr(), artist.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Album:
            if ( currentMedia->IsSupported( KMPXMediaMusicAlbum ) ) {
                const TDesC& album = currentMedia->ValueText( KMPXMediaMusicAlbum );
                if ( album.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( album.Ptr(), album.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Count:
            if ( currentMedia->IsSupported( KMPXMediaGeneralCount ) ) {
                count = currentMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount );
            }
            temp.AppendNum( count );
            //AknTextUtils::LanguageSpecificNumberConversion( temp );
            countBuf.Append( temp );
            countBuf.Append( (count > 1 ) ? KSongs() : KSong() );
            data = QString::fromUtf16( countBuf.Ptr(), countBuf.Length() );
            break;
        case MpMpxCollectionData::AlbumArtUri:
            if ( currentMedia->IsSupported( KMPXMediaMusicAlbumArtFileName ) ) {
                const TDesC& album = currentMedia->ValueText( KMPXMediaMusicAlbumArtFileName );
                if ( album.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( album.Ptr(), album.Length() );
                }
            }
            break;
        case MpMpxCollectionData::Uri:
            if ( currentMedia->IsSupported( KMPXMediaGeneralUri ) ) {
                const TDesC& uri = currentMedia->ValueText( KMPXMediaGeneralUri );
                if ( uri.Compare( KNullDesC ) != 0 ) {
                    data = QString::fromUtf16( uri.Ptr(), uri.Length() );
                }
            }
            break;
        default:
            break;
    }
    TX_EXIT
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoIsAutoPlaylistL()
{
    if ( iContainerMedia->IsSupported(KMPXMediaGeneralNonPermissibleActions) ) {
        TMPXGeneralNonPermissibleActions attr(
                iContainerMedia->ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions ) );
        if ( attr & EMPXWrite ) {
            return true;
        }
    }
    return false;
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoIsAutoPlaylistL( int index )
{
    const CMPXMediaArray* containerArray = iContainerMedia->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    User::LeaveIfNull( const_cast<CMPXMediaArray*>( containerArray ));
    CMPXMedia* media( containerArray->AtL(index) );

    if ( media->IsSupported(KMPXMediaGeneralNonPermissibleActions) ) {
        TMPXGeneralNonPermissibleActions attr(
            media->ValueTObjectL<TMPXGeneralNonPermissibleActions>(
                KMPXMediaGeneralNonPermissibleActions ) );
        if ( attr & EMPXWrite ) {
            return true;
        }
    }
    return false;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetItemCountL( int index )
{
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );
    int count = 0;
    if ( currentMedia->IsSupported( KMPXMediaGeneralCount ) ) {
        count = currentMedia->ValueTObjectL<TInt>( KMPXMediaGeneralCount );
    }
    return count;
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetContainerIdL()
{
    if ( !iContainerMedia->IsSupported( KMPXMediaGeneralId ) ) {
        User::Leave(KErrNotFound);
    }
    return iContainerMedia->ValueTObjectL<TInt>( KMPXMediaGeneralId );
}

/*!
 \internal
 */
int MpMpxCollectionDataPrivate::DoGetItemIdL( int index )
{
    CMPXMedia* currentMedia( iMediaArray->AtL( index ) );
    if ( !currentMedia->IsSupported( KMPXMediaGeneralId ) ) {
        User::Leave(KErrNotFound);
    }
    return currentMedia->ValueTObjectL<TInt>( KMPXMediaGeneralId );
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoRemoveItemL( int index )
{
    delete iCachedRemovedItem;
    iCachedRemovedItem = 0;
    iCachedRemovedItem = CMPXMedia::NewL( *iMediaArray->AtL( index ) );
    iMediaArray->Remove( index );
}

/*!
 \internal
 */
bool MpMpxCollectionDataPrivate::DoTestCachedItemL( int itemId )
{
    if ( !iCachedRemovedItem && !iCachedRemovedItem->IsSupported( KMPXMediaGeneralId ) ) {
        User::Leave(KErrNotFound);
    }
    return ( itemId == iCachedRemovedItem->ValueTObjectL<TInt>( KMPXMediaGeneralId ) );
}


/*!
 \internal
 */
void MpMpxCollectionDataPrivate::SetCollectionContextL()
{
    TX_ENTRY
    TMPXGeneralType containerType( EMPXNoType );
    if ( iContainerMedia->IsSupported( KMPXMediaGeneralType ) ) {
        containerType = iContainerMedia->ValueTObjectL<TMPXGeneralType>( KMPXMediaGeneralType );
    }

    TMPXGeneralCategory containerCategory( EMPXNoCategory );
    if( iContainerMedia->IsSupported( KMPXMediaGeneralCategory ) ) {
        containerCategory = iContainerMedia->ValueTObjectL<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
    }
    TX_LOG_ARGS("type=" << containerType << ", category=" << containerCategory );

    iContext = ECollectionContextUnknown;
    if ( containerType == EMPXGroup ) {
        switch (containerCategory) {
            case EMPXSong:
                iContext = ECollectionContextAllSongs;
                break;
            case EMPXArtist:
                iContext = ECollectionContextArtists;
                break;
            case EMPXAlbum:
                iContext = ECollectionContextAlbums;
                break;
            case EMPXPlaylist:
                iContext = ECollectionContextPlaylists;
                break;
            case EMPXGenre:
                iContext = ECollectionContextGenres;
                break;
        }
    }
    else if ( containerType == EMPXItem ) {
        switch (containerCategory) {
            case EMPXArtist:
                iContext = ECollectionContextArtistAlbums;
                break;
            case EMPXAlbum:
                iContext = ECollectionContextAlbumSongs;
                break;
            case EMPXSong:
                iContext = ECollectionContextArtistSongs;
                break;
            case EMPXPlaylist:
                iContext = ECollectionContextPlaylistSongs;
                break;
            case EMPXGenre:
                iContext = ECollectionContextGenreSongs;
                break;
        }
    }
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxCollectionDataPrivate::DoSetMpxMediaL( const CMPXMedia& entries )
{
    TX_ENTRY
    delete iContainerMedia;
    iContainerMedia = 0;
    iContainerMedia = CMPXMedia::NewL(entries);
    iMediaArray = const_cast<CMPXMediaArray*>(iContainerMedia->Value<CMPXMediaArray>( KMPXMediaArrayContents ) );
    TX_LOG_ARGS("media count=" << iMediaArray->Count() );

    SetCollectionContextL();
    TX_EXIT
}

/*!
 \internal
 */
const CMPXMedia& MpMpxCollectionDataPrivate::containerMedia()
{
    return *iContainerMedia;
}

