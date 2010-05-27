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
* Description: Framework wraper - Private.
*
*/

#include <QRegExp>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>
#include <QTime>
#include <QDate>

#include <mpxplaybackutility.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>

#include "mpmpxdetailsframeworkwrapper_p.h"
#include "mpmpxdetailsframeworkwrapper.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpsongdata.h"

/*!
    \class MpMpxDetailsFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxDetailsFrameworkWrapperPrivate::MpMpxDetailsFrameworkWrapperPrivate( MpMpxDetailsFrameworkWrapper *qq )
    : q_ptr( qq ),
      iSongData(0),
      iPlaybackUtility(0)
{
    TX_LOG
}

/*!
 \internal
 */
MpMpxDetailsFrameworkWrapperPrivate::~MpMpxDetailsFrameworkWrapperPrivate()
{
    TX_ENTRY
    
    if ( iPlaybackUtility )
        {        
        TRAP_IGNORE(iPlaybackUtility->RemoveObserverL( *this ));
        iPlaybackUtility->Close();
        iPlaybackUtility = NULL;
        //do not delete pointer data
        }
    
    delete iSongData;
    iSongData=NULL;
    
    TX_EXIT
}

/*!
 \initialize wrapper
 */
void MpMpxDetailsFrameworkWrapperPrivate::init( TUid hostUid )
{
    mHostUid = hostUid;
    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::DoInitL()
{
    TX_ENTRY

    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( mHostUid );
    iPlaybackUtility->AddObserverL( *this );
    iSongData = new MpSongData();
    User::LeaveIfNull(iSongData);
   
    TX_EXIT
}


/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandlePlaybackMessage(
        CMPXMessage *aMessage,
        TInt aError )
{
    Q_UNUSED( aMessage );
    Q_UNUSED( aError );
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandlePropertyL( TMPXPlaybackProperty /*aProperty*/, TInt /*aValue*/, TInt /*aError*/ )
{
    TX_LOG
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandleSubPlayerNamesL( TUid /*aPlayer*/, const MDesCArray */*aSubPlayers*/, TBool /*aComplete*/, TInt /*aError*/ )
{
    TX_LOG    
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::HandleMediaL(
        const CMPXMedia& aProperties,
        TInt aError )
{
    TX_ENTRY
  
    if ( KErrNone != aError ){
        // TODO: Hkn: Add a proper trace macro and trace
        TX_EXIT
        return;
    }
  
    bool changed = false;
    if ( aProperties.IsSupported( KMPXMediaGeneralTitle ) ) {
        changed |= iSongData->setTitle(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaGeneralTitle ).Ptr(),
                aProperties.ValueText( KMPXMediaGeneralTitle ).Length() ) );
    } else {
        changed |= iSongData->setTitle( QString() );
    }
    
    if ( aProperties.IsSupported( KMPXMediaMusicArtist ) ) {
        changed |= iSongData->setArtist(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicArtist ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicArtist ).Length() ) );
    } else {
        changed |= iSongData->setArtist( QString() );
    }
    
    if ( aProperties.IsSupported( KMPXMediaMusicAlbum ) ) {
        changed |= iSongData->setAlbum(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicAlbum ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicAlbum ).Length() ) );
    } else {
        changed |= iSongData->setAlbum( QString() );
    }
    if ( changed ) {
        iSongData->commitPlaybackInfo( );
    }    
    
    // call back will be called when Album art is retrieved
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) ) ) {        
        iSongData->setAlbumArtUri( 
                QString::fromUtf16(
                        aProperties.ValueText(KMPXMediaMusicAlbumArtFileName).Ptr(),
                        aProperties.ValueText(KMPXMediaMusicAlbumArtFileName).Length() ) );
    } else {
        iSongData->setAlbumArtUri( QString() );
    }
    
    // all following will be for song details
    changed = false;  
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaGeneralUri ) ) ) {
        QString fullName = QString::fromUtf16(
                aProperties.ValueText( KMPXMediaGeneralUri ).Ptr(),
                aProperties.ValueText( KMPXMediaGeneralUri ).Length() );
        
        TX_LOG_ARGS( "File name with path: " << fullName );
        // get size & last modifed information from file system
        QFileInfo info( fullName );
        changed |= iSongData->setSize( info.size() );
        
        QDateTime lastModified = info.lastModified();
        QDate date = lastModified.date();
        int day = date.day();
        int month = date.month();
        int year = date.year();
        QTime time = lastModified.time();
        int sec = time.second();
        int min = time.minute();
        int hr = time.hour();
        
        
        QString lastModifiedStr("%1.%2.%3 %4:%5:%6");
        lastModifiedStr = lastModifiedStr.arg( day ).arg( month ).arg( year ).arg( hr ).arg( min ).arg( sec );
        changed |= iSongData->setModified( lastModifiedStr );
        
        
        // get file name without suffix
        QString file;
        QRegExp rx("(.+)\\..+");
        QString str = info.fileName();
        TX_LOG_ARGS( "File name with suffix = " << str );

        int pos = rx.indexIn( str );
        if( pos > -1 ) {
            file = rx.cap( 1 );
            TX_LOG_ARGS( "File = " << file );
        }

        changed |= iSongData->setFileName( file );
    } else {
        changed |= iSongData->setFileName( QString() );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicComposer ) ) ) {
        TX_LOG_ARGS( "Composer is supported " );
        changed |= iSongData->setComposer(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicComposer ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicComposer ).Length() ) );
    } else {
        changed |= iSongData->setComposer( QString() );
    }

    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicYear ) ) ) {
        TInt64 yearInMicroSeconds = aProperties.ValueTObjectL<TInt64>( KMPXMediaMusicYear );
        TX_LOG_ARGS( "year = " << yearInMicroSeconds );
        TTime yearTime( yearInMicroSeconds );
        changed |= iSongData->setYear( yearTime.DateTime().Year() );
    } else {
        // to clear previous result
        changed |= iSongData->setYear( -1 );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicAlbumTrack ) ) ) {
        changed |= iSongData->setAlbumTrack(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicAlbumTrack ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicAlbumTrack ).Length() ) );
    } else {
        changed |= iSongData->setAlbumTrack( QString() );  
    }

    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicGenre ) ) ) {
        changed |= iSongData->setGenre(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicGenre ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicGenre ).Length() ) );
    } else {
        changed |= iSongData->setGenre( QString() );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaGeneralMimeType ) ) ) {
        QString type = QString::fromUtf16(
                            aProperties.ValueText( KMPXMediaGeneralMimeType ).Ptr(),
                            aProperties.ValueText( KMPXMediaGeneralMimeType ).Length() );
        QString regularExpression(".+/(.+)");                            
        QRegExp rx(regularExpression);
        QString mimeType;

        int pos = rx.indexIn( type );
        if( pos > -1 ) {
            mimeType = rx.cap( 1 );
            mimeType = mimeType.toUpper();
            TX_LOG_ARGS( "MIME type =" << mimeType );
        }        
        
        changed |= iSongData->setMimeType( mimeType );  
    } else {
        changed |= iSongData->setMimeType( QString() );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaGeneralDuration ) ) ) {
        TInt duration( aProperties.ValueTObjectL<TInt>( KMPXMediaGeneralDuration ) );
        changed |= iSongData->setDuration( duration / 1000 );
    } else {
        changed |= iSongData->setDuration( -1 );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaAudioBitrate  ) ) ) {
        TInt bitRate( aProperties.ValueTObjectL<TInt>( KMPXMediaAudioBitrate ) );
        changed |= iSongData->setBitRate( bitRate );
    } else {
        changed |= iSongData->setBitRate( -1 );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaAudioSamplerate  ) ) ) {
        TInt sampleRate( aProperties.ValueTObjectL<TInt>( KMPXMediaAudioSamplerate ) );
        changed |= iSongData->setSampleRate( sampleRate );
    } else {
        changed |= iSongData->setSampleRate( -1 );
    }
        

    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaGeneralCopyright ) ) ) {
        changed |= iSongData->setCopyright(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaGeneralCopyright ).Ptr(),
                aProperties.ValueText( KMPXMediaGeneralCopyright ).Length() ) );
    } else {
        changed |= iSongData->setCopyright( QString() );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaMusicURL ) ) ) {
        changed |= iSongData->setMusicURL(
            QString::fromUtf16(
                aProperties.ValueText( KMPXMediaMusicURL ).Ptr(),
                aProperties.ValueText( KMPXMediaMusicURL ).Length() ) );
    } else {
        changed |= iSongData->setMusicURL( QString() );
    }
    
    if ( aProperties.IsSupported( TMPXAttribute( KMPXMediaDrmProtected ) ) ) {
    TX_LOG_ARGS( "DRM is supported." );
        changed |= iSongData->setDrmProtected( aProperties.ValueTObjectL<TBool>( KMPXMediaDrmProtected ) );
    } else {
        changed |= iSongData->setDrmProtected( false );
    } 
    
    if ( changed ) {
        iSongData->commitSongDetailInfo( );
    }    
    TX_EXIT
}

/*!
 return song data
 */
MpSongData *MpMpxDetailsFrameworkWrapperPrivate::songData()
{
    TX_LOG
    return iSongData;
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::retrieveSong()
{
    TX_ENTRY
    TRAP_IGNORE( doRetrieveSongL() );
    TX_EXIT
}

/*!
 \internal
 */
void MpMpxDetailsFrameworkWrapperPrivate::doRetrieveSongL()
{
    TX_ENTRY
    MMPXSource *mediaSrc = iPlaybackUtility->Source();
    User::LeaveIfNull( mediaSrc );
    RArray<TMPXAttribute> requestedAttr;
    CleanupClosePushL( requestedAttr );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralTitle ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicArtist ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbum ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralUri ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) );

    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicAlbumTrack ) );   
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicComposer ) );   
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicYear ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicGenre ) );   
    
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralMimeType ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralDuration ) );    
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaAudioBitrate ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaAudioSamplerate ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaGeneralCopyright ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaMusicURL ) );
    requestedAttr.AppendL( TMPXAttribute( KMPXMediaDrmProtected ) );    
    
    mediaSrc->MediaL( requestedAttr.Array(), *this );
    CleanupStack::PopAndDestroy( &requestedAttr );
    TX_EXIT
}
