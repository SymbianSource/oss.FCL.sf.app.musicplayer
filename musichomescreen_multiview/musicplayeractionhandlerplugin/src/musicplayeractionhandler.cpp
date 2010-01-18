/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Music Player Action Handler Plugin
*
*/


#include <liwvariant.h>                 // TLiwVariant
#include <apgcli.h>                     // RApaLsSession, TApaAppInfo
#include <apacmdln.h>                   // CApaCommandLine
#include <apgtask.h>                    // TApaTaskList
#include <mpxlog.h>                     // MPX_DEBUG
#include <mpxparameter.h>               // CMPXParameter
#include <mpxplaybackutility.h>         // MMPXPlaybackUtility
#include <mpxconstants.h>               // KAppUidMusicPlayerX
#include <mpxmusicplayerviewplugin.hrh> // KMPXPluginTypePlaybackUid
#include <AknTaskList.h>                // CAknTaskList

#include <mpxcommonuihelper.h>
#include <mpxcollectionhelperfactory.h>
#include "musicplayeractionhandler.h"
#include <mpxcollectionpath.h>




const TInt KPlayerMusicPlayerParameterGranularity = 50;
const TUid  KMusicPlayerAppUid = { 0x102072C3 };

// RProperty key to identify the case when Music Player launching
// in the background
const TInt KMPXLaunchingOnBackground( 100 );

//map values
_LIT( KActionPlaybackCommand , "PlaybackCommand" );
_LIT( KactionMessageToMusicPlayer, "MessageToMusicPlayer" );
_LIT( KGoToNowPlaying, "GoToNowPlaying" );
_LIT( KGoToLastPlayed, "GoToLastPlayed" );
_LIT( KGoToLastPlayedMinimized, "GoToLastPlayedMinimized" );
_LIT( KGoToMusicLibrary, "GoToMusicLibrary" );
_LIT( KGoToAlbumView, "GoToAlbumView" );

//map keys
_LIT8( KType, "type" );
_LIT8( KCommand, "command" );
_LIT8( KMessage, "message" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMusicPlayerActionHandler::ConstructL()
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::ConstructL()");
    
    //iPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer );
    //iPlaybackUtility = MMPXPlaybackUtility::NewL( KMusicPlayerAppUid);

    MPX_DEBUG1("<--CMusicPlayerActionHandler::ConstructL()");
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMusicPlayerActionHandler* CMusicPlayerActionHandler::NewL()
    {
    CMusicPlayerActionHandler* self = CMusicPlayerActionHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMusicPlayerActionHandler* CMusicPlayerActionHandler::NewLC()
    {
    CMusicPlayerActionHandler* self = new( ELeave ) CMusicPlayerActionHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
CMusicPlayerActionHandler::~CMusicPlayerActionHandler()
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::~CMusicPlayerActionHandler()");
    if ( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }
    MPX_DEBUG1("<--CMusicPlayerActionHandler::~CMusicPlayerActionHandler()");
    }

// ---------------------------------------------------------------------------
// Executes Playback command on current player
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::ExecutePlaybackCommandL(const CLiwMap* aMap)
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::ExecutePlaybackCommandL()");
    TInt errCode;
    TLiwVariant variant;
    variant.PushL();
    errCode = ExtractVariantL( aMap, variant, KCommand );
    if ( errCode == KErrNone )
        {
        if ( !iPlaybackUtility ) // Ensure that the utility exists
            {
            iPlaybackUtility = MMPXPlaybackUtility::NewL( KMusicPlayerAppUid);
            }	
        TMPXPlaybackCommand Command;
        Command = static_cast<TMPXPlaybackCommand>( variant.AsTInt32() );
        iPlaybackUtility->CommandL( Command );
        }
    CleanupStack::PopAndDestroy( &variant);
    MPX_DEBUG1("<--CMusicPlayerActionHandler::ExecutePlaybackCommandL()");
    return errCode;
    }

// ---------------------------------------------------------------------------
// Sends a message to music player
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::ExecuteMessageToMusicPlayerL(
        const CLiwMap* aMap )
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::"
            "ExecuteMessageToMusicPlayerL()");
    TInt errCode;
    RBuf msg;
    CleanupClosePushL( msg );
    errCode = ExtractDesL( aMap, msg, KMessage );
    if ( errCode == KErrNone )
        {
        if ( ! msg.CompareF( KGoToNowPlaying ) )
            {
            errCode = GoToNowPlayingL();
            }
        else if ( ! msg.CompareF( KGoToLastPlayed ) )
            {
            errCode = GoToLastPlayedL( EFalse );
            }
        else if ( ! msg.CompareF( KGoToLastPlayedMinimized ) )
            {
            errCode = GoToLastPlayedL( ETrue );
            }
        else if ( ! msg.CompareF( KGoToMusicLibrary ) )
            {
            errCode = GoToMusicLibraryL();
            }  
        else if ( ! msg.CompareF( KGoToAlbumView ) )
        	{
            errCode = GoToAlbumViewL();
            } 
        }
    CleanupStack::PopAndDestroy( &msg );
    MPX_DEBUG1("<--CMusicPlayerActionHandler::"
            "ExecuteMessageToMusicPlayerL()");
    return errCode;
    }

// ---------------------------------------------------------------------------
// Opens musicplayer in library view, even if it is already open and playing.
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::GoToMusicLibraryL()
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::GoToMusicLibraryL()");
    //Launch player
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CAknTaskList *taskList = CAknTaskList::NewL( wsSession );
    TApaTask task = taskList->FindRootApp( KAppUidMusicPlayerX );
    delete taskList;
    CMPXParameter* param = new ( ELeave ) CMPXParameter();
    CleanupStack::PushL( param );
    param->iType.iUid = KMPXPluginTypeCollectionUid;
    param->iCmdForward = EMPXCmdFwdNone;

    MMPXCollectionUiHelper* collectionHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    CMPXCollectionPath* path = collectionHelper->MusicMenuPathL();
	path->AppendL(3); // Albums
    if (path)
        {
        param->iCollectionPath = path;
        param->iPathType = EMPXTypeCollectionPath;
        }
    collectionHelper->Close();
    
    CBufBase* buffer =
            CBufFlat::NewL( KPlayerMusicPlayerParameterGranularity );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    param->ExternalizeL( writeStream );
    writeStream.CommitL();
    buffer->Compress();
    CleanupStack::PopAndDestroy( &writeStream );
    if ( task.Exists() )
        {
//        task.SendMessage( KAppUidMusicPlayerX, buffer->Ptr( 0 ));
        wsSession.SendMessageToWindowGroup( task.WgId(), KAppUidMusicPlayerX,
                    buffer->Ptr( 0 ) );
        }
    else        //Application needs to be launched. The first view is library view.
        {
        // Launch Music Player Application
        RApaLsSession   appArcSession;
        _LIT(KEmpty,"");
        User::LeaveIfError(appArcSession.Connect());    // connect to AppArc server
        TThreadId id;
        appArcSession.StartDocument( KEmpty, KAppUidMusicPlayerX, id );
        appArcSession.Close();                                
        }
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( param );
    wsSession.Close();
    MPX_DEBUG1("<--CMusicPlayerActionHandler::GoToMusicLibraryL()");
    return KErrNone;
    }
	
// ---------------------------------------------------------------------------
// Opens musicplayer in Album and Artist view, even if it is already open and playing.
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::GoToAlbumViewL()
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::GoToAlbumViewL()");
    //Launch player
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CAknTaskList *taskList = CAknTaskList::NewL( wsSession );
    TApaTask task = taskList->FindRootApp( KAppUidMusicPlayerX );
    delete taskList;
    CMPXParameter* param = new ( ELeave ) CMPXParameter();
    CleanupStack::PushL( param );
    param->iType.iUid = KMPXPluginTypeCollectionUid;
    param->iCmdForward = EMPXCmdFwdNone;
   
    MMPXCollectionUiHelper* collectionHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    CMPXCollectionPath* path = collectionHelper->MusicMenuPathL();
    path->AppendL(3); // Albums
    if (path)
        {
        param->iCollectionPath = path;
        param->iPathType = EMPXTypeCollectionPath;
        }
    collectionHelper->Close();
    
    CBufBase* buffer =
            CBufFlat::NewL( KPlayerMusicPlayerParameterGranularity );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    param->ExternalizeL( writeStream );
    writeStream.CommitL();
    buffer->Compress();
    CleanupStack::PopAndDestroy( &writeStream );
    if ( task.Exists() )
        {
//        task.SendMessage( KAppUidMusicPlayerX, buffer->Ptr( 0 ));
        wsSession.SendMessageToWindowGroup( task.WgId(), KAppUidMusicPlayerX,
                    buffer->Ptr( 0 ) );
        }
    else        //Application needs to be launched. The first view is library view.
        {
        // Launch Music Player Application
        RApaLsSession   appArcSession;
        _LIT(KEmpty,"");
        User::LeaveIfError(appArcSession.Connect());    // connect to AppArc server
        TThreadId id;
        appArcSession.StartDocument( KEmpty, KAppUidMusicPlayerX, id );
        appArcSession.Close();                                
        }
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( param );
    wsSession.Close();
    MPX_DEBUG1("<--CMusicPlayerActionHandler::GoToAlbumViewL()");
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Opens musicplayer, if it is already open and playing it goes to nowplaying.
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::GoToNowPlayingL()
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::GoToNowPlayingL()");
    //Launch player
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CAknTaskList *taskList = CAknTaskList::NewL( wsSession );
    TApaTask task = taskList->FindRootApp( KAppUidMusicPlayerX );
    delete taskList;
    CMPXParameter* param = new ( ELeave ) CMPXParameter();
    CleanupStack::PushL( param );
    param->iType.iUid = KMPXPluginTypePlaybackUid;
    param->iCmdForward = EMPXCmdFwdNone;
    CBufBase* buffer =
            CBufFlat::NewL( KPlayerMusicPlayerParameterGranularity );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    param->ExternalizeL( writeStream );
    writeStream.CommitL();
    buffer->Compress();
    CleanupStack::PopAndDestroy( &writeStream );
    if ( task.Exists() )
        {
        wsSession.SendMessageToWindowGroup( task.WgId(), KAppUidMusicPlayerX,
                    buffer->Ptr( 0 ) );
        
        
        }
    else
        {
        RApaLsSession ls;
        CleanupClosePushL( ls );
        User::LeaveIfError( ls.Connect() );
        TApaAppInfo appInfo;
        User::LeaveIfError( ls.GetAppInfo( appInfo, KAppUidMusicPlayerX ) );
        CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();
        apaCommandLine->SetExecutableNameL( appInfo.iFullName );
        apaCommandLine->SetTailEndL( buffer->Ptr( 0 ) );
        User::LeaveIfError( ls.StartApp( *apaCommandLine ) );
        CleanupStack::PopAndDestroy(); // apaCommandLine
        CleanupStack::PopAndDestroy(); // ls
        }
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( param );
    wsSession.Close();
    MPX_DEBUG1("<--CMusicPlayerActionHandler::GoToNowPlayingL()");
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Opens Stand alone MP and goes to now playing, it will display last played
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::GoToLastPlayedL( TBool aMinimized )
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::GoToLastPlayed()");
    //Launch player
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CAknTaskList *taskList = CAknTaskList::NewL( wsSession );
    TApaTask task = taskList->FindRootApp( KAppUidMusicPlayerX );
    delete taskList;
    CMPXParameter* param = new ( ELeave ) CMPXParameter();
    CleanupStack::PushL( param );
    param->iType.iUid = KMPXPluginTypeLastPlayedUid;//KMPXPluginTypePlaybackUid;
    param->iCmdForward = EMPXCmdFwdNone;
    CBufBase* buffer =
            CBufFlat::NewL( KPlayerMusicPlayerParameterGranularity );
    CleanupStack::PushL( buffer );
    RBufWriteStream writeStream( *buffer );
    CleanupClosePushL( writeStream );
    param->ExternalizeL( writeStream );
    writeStream.CommitL();
    buffer->Compress();
    CleanupStack::PopAndDestroy( &writeStream );
    if ( task.Exists() )
        {
        wsSession.SendMessageToWindowGroup( task.WgId(), KAppUidMusicPlayerX,
                    buffer->Ptr( 0 ) );
        if (!aMinimized)
            {
            task.BringToForeground();
            }
        }
    else
        {
        RApaLsSession ls;
        CleanupClosePushL( ls );
        User::LeaveIfError( ls.Connect() );
        TApaAppInfo appInfo;
        User::LeaveIfError( ls.GetAppInfo( appInfo, KAppUidMusicPlayerX ) );
        CApaCommandLine* apaCommandLine = CApaCommandLine::NewLC();
        if (aMinimized)
            {
            apaCommandLine->SetCommandL(EApaCommandBackground);
            
            TInt err( RProperty::Define(
                KAppUidMusicPlayerX, KMPXLaunchingOnBackground, RProperty::EInt ) );
            MPX_DEBUG2( "CMusicPlayerActionHandler::GoToLastPlayed() RProperty define err = %d", err );
            if ( err == KErrNone || err == KErrAlreadyExists )
                {
                err = RProperty::Set( KAppUidMusicPlayerX,
                            KMPXLaunchingOnBackground,
                            ETrue );
                MPX_DEBUG2( "CMusicPlayerActionHandler::GoToLastPlayed() RProperty set to true err = %d", err );
                }
            }
        apaCommandLine->SetExecutableNameL( appInfo.iFullName );
        apaCommandLine->SetTailEndL( buffer->Ptr( 0 ) );
        User::LeaveIfError( ls.StartApp( *apaCommandLine ) );
        CleanupStack::PopAndDestroy(); // apaCommandLine
        CleanupStack::PopAndDestroy(); // ls
        
        if ( aMinimized )
            {
            TInt err = RProperty::Set( KAppUidMusicPlayerX,
                        KMPXLaunchingOnBackground,
                        EFalse );
            MPX_DEBUG2( "CMusicPlayerActionHandler::GoToLastPlayed() RProperty set to false err = %d", err );
            }
        }
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( param );
    wsSession.Close();
    MPX_DEBUG1("<--CMusicPlayerActionHandler::GoToLastPlayed()");
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// Extracts a descriptor
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::ExtractDesL( const CLiwMap* aMap,
        RBuf& aString, const TDesC8& aMapName )
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::ExtractDesL()");
    TInt errCode( KErrArgument );
    TLiwVariant variant;
    variant.PushL();
    TPtrC tempString( KNullDesC );
    if ( aMap->FindL( aMapName, variant) )
        {
        variant.Get( tempString );
        aString.ReAllocL( tempString.Length() );
        aString.Append( tempString );
        errCode = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );
    MPX_DEBUG1("<--CMusicPlayerActionHandler::ExtractDesL()");
    return errCode;
    }

// ---------------------------------------------------------------------------
// Extract variant
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::ExtractVariantL( const CLiwMap* aMap,
        TLiwVariant& aVariant, const TDesC8& aMapName )
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::ExtractVariantL()");
    TInt errCode( KErrNone );
    if ( ! aMap->FindL( aMapName, aVariant ) )
        {
        errCode = KErrArgument;
        }
    MPX_DEBUG1("<--CMusicPlayerActionHandler::ExtractVariantL()");
    return errCode;
    }

// ---------------------------------------------------------------------------
// From CAHPlugin
// Executes provided action
// ---------------------------------------------------------------------------
//
TInt CMusicPlayerActionHandler::ExecuteActionL( const CLiwMap* aMap )
    {
    MPX_DEBUG1("-->CMusicPlayerActionHandler::ExecuteActionL()");
    TInt errCode;
    RBuf type;
    CleanupClosePushL( type );
    errCode = ExtractDesL( aMap, type, KType );
    if ( errCode == KErrNone )
        {
        if ( ! type.CompareF( KActionPlaybackCommand ) )
            {
            errCode = ExecutePlaybackCommandL( aMap );
            }
        else
            if ( ! type.CompareF( KactionMessageToMusicPlayer ) )
                {
                errCode = ExecuteMessageToMusicPlayerL( aMap );
                }
        }
    CleanupStack::PopAndDestroy( &type );
    MPX_DEBUG1("<--CMusicPlayerActionHandler::ExecuteActionL()");
    return errCode;
    }

//  End of File

