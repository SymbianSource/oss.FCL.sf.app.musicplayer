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
* Description:  Updates Music Content Publisher
*
*/

#include <mpxlog.h>                     // MPX_DEBUG
#include <implementationproxy.h>
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <aknsconstants.hrh>
#include <musichomescreenicons.mbg>
#include <musichomescreen.rsg>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionframeworkdefs.h>
#include <AknTaskList.h>



#include "cpglobals.h" //to be moved to domain API

#include "mcpmusicplayer.h" 
#include "mcpmusicplayeruids.hrh"
#include "aiplayerpluginengine.h"
#include "filemonitor.h"
#include "applicationmonitor.h"
#include "pnsmonitor.h"

//const TUid  KLocalPlaybackUid = { 0x101FFC06 };
const TInt  KMusicPlayerAppUidAsTInt =  0x102072C3;

const TInt KMmUid3AsTInt( 0x101f4cd2 );
const TInt KMSGUidAsTInt( 0x10003A39 );

namespace TBK
    {
    const TUint  KSkeep_L          = 0x001; //000000000001
    const TUint  KSkeep_R          = 0x002; //000000000010
    const TUint  KPause            = 0x004; //000000000100
    const TUint  KPlay             = 0x008; //000000001000
    const TUint  KSkeep_L_dimmed   = 0x010; //000000010000
    const TUint  KSkeep_R_dimmed   = 0x020; //000000100000
    const TUint  KPause_dimmed     = 0x040; //000001000000
    const TUint  KPlay_dimmed      = 0x080; //000010000000
    const TUint  KPlay_last_played = 0x100; //000100000000
    }

_LIT( KEmpty, "" );
_LIT( KNowPlaying, "LOC:NOW PLAYING" );
_LIT( KLastPlayed, "LOC:LAST PLAYED" );


_LIT( KMIF, "z:\\resource\\apps\\musichomescreenicons.mif");

_LIT( KResourceFile, "z:musichomescreen.rsc");

//for music player action handler - playback command
_LIT( KActionPlaybackCommand , "PlaybackCommand" );
_LIT8( KPBCommand, "command" );

//for music player action handler - message to music player
_LIT( KactionMessageToMusicPlayer, "MessageToMusicPlayer" );
_LIT( KGoToNowPlaying, "GoToNowPlaying" );
_LIT( KGoToLastPlayed, "GoToLastPlayed" );
_LIT( KGoToLastPlayedMinimized, "GoToLastPlayedMinimized" );
_LIT( KGoToAlbumView, "GoToAlbumView" );

_LIT8( KMessage, "message" );
_LIT8( KAdditionalData, "additional_data" );

//for application launcher AHPlugin
_LIT( KCmdLine, "cmd_line" );
_LIT( KLaunchApp, "launch_application" );
_LIT( KMessageWithTail, "message_with_tail" );
_LIT8( KMessageForMMOpenMusicSuiteWithHide, "mm://root/musicsuite?exit=hide");


//for time
//_LIT( KSeparator, "/" );
_LIT( KSeparatorhyphen, " - " );
//_LIT( KMPXZeroDurationMark, "--" );
//_LIT( KTitleNowPlaying, "Now Playing" );
//_LIT( KTitleLastPlayed, "Last Played" );

const TInt KMusicPlayerOpeningTimerInterval = 15000000; // 15 seconds

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMCPMusicPlayer::CMCPMusicPlayer(MMCPPluginObserver* aObserver)
    {
    iMHObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::ConstructL()
    {
    MPX_DEBUG1("CMCPMusicPlayer::ConstructL <---");
    MPX_DEBUG1("CMCPMusicPlayer::ConstructL creating engine");
    iEngine = CAiPlayerPluginEngine::NewL( *this );
    MPX_DEBUG1("CMCPMusicPlayer::ConstructL engine created");
    AknsUtils::InitSkinSupportL();
    //Load Loc strings
    MPX_DEBUG1("CMCPMusicPlayer::ConstructL getting resource file");
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);  
    TFileName fileName;
    TParse* parseObj = new(ELeave) TParse();
    TInt errInt = parseObj->Set( KResourceFile(),&KDC_APP_RESOURCE_DIR,NULL );
    if(KErrNone != errInt)
      {
      delete parseObj;
      User::Leave(errInt);
      }
    fileName = parseObj->FullName();
    delete parseObj;
    BaflUtils::NearestLanguageFile(fs,fileName);
    if(!BaflUtils::FileExists(fs,fileName))
        {
        User::Leave(KErrNotFound);
        }
    RResourceFile resourceFile;
    resourceFile.OpenL(fs,fileName);
    CleanupClosePushL(resourceFile);
    resourceFile.ConfirmSignatureL();
    GetLocalizedStringL(resourceFile, iGoToMusicBuffer, R_MUSICHOMESCREEN_GO_TO_MUSIC);
    GetLocalizedStringL(resourceFile, iOpeningBuffer, R_MUSICHOMESCREEN_OPENING);
    CleanupStack::PopAndDestroy(&resourceFile);
    CleanupStack::PopAndDestroy(&fs);
    MPX_DEBUG1("CMCPMusicPlayer::ConstructL resources loaded");
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRAPD(err, iArtBitmapMask = AknsUtils::CreateBitmapL( skin, 
            KAknsIIDQgnHomePhotoMask ) );
    if (err == KErrNone)
        {
        AknIconUtils::DisableCompression(iArtBitmapMask);
        TSize size( 70 , 70);
        AknIconUtils::SetSize( iArtBitmapMask, size );
        if (iArtBitmapMask->SetDisplayMode( EGray256 ) != KErrNone)
            {
            delete iArtBitmapMask;
            iArtBitmapMask = NULL;
            }
        }
    else
        {
        delete iArtBitmapMask;
        iArtBitmapMask = NULL;
        }
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iMusicPlayerOpeningTimer = CPeriodic::NewL( CActive::EPriorityLow );
    MPX_DEBUG1("CMCPMusicPlayer::ConstructL --->");
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMCPMusicPlayer* CMCPMusicPlayer::NewL(MMCPPluginObserver* aObserver)
    {
    CMCPMusicPlayer* self = new ( ELeave ) 
    CMCPMusicPlayer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCPMusicPlayer::~CMCPMusicPlayer()
    {
    MPX_DEBUG1("CMCPMusicPlayer::~CMCPMusicPlayer <---");
    if (iArtCached && iArtBitmap != iCachedArtBitmap)
        {
        delete iCachedArtBitmap;
        }
    delete iArtBitmap;
    delete iArtBitmapMask;
    delete iEngine;
    delete iFileMonitor;
    delete iAppMonitor;
    delete iPNSMonitor;
    delete iGoToMusicBuffer;
    delete iOpeningBuffer;
    if ( iCollectionUtility )
    	{
    	iCollectionUtility->Close();
    	}
    delete iMusicPlayerOpeningTimer;
    MPX_DEBUG1("CMCPMusicPlayer::~CMCPMusicPlayer --->");
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CMCPMusicPlayer::IsOKToPublishData()
    {
    return iActive;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::InstallPlaybackCommandActionL(TMPXPlaybackCommand aCmd,
        TMCPTriggerDestination aDestination)
    {
    MPX_DEBUG1("CMCPMusicPlayer::InstallPlaybackCommandActionL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();
    
    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10207C16 ) ) );
    mapData->InsertL( KType, TLiwVariant( KActionPlaybackCommand ) );
    TInt32 cmd;
    cmd=static_cast<TInt32>( aCmd );
    mapData->InsertL( KPBCommand, TLiwVariant( cmd ) );
    mapTrigger->InsertL( KData , TLiwVariant( mapData ) );
    
    iMHObserver->PublishActionL( this, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData  );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMCPMusicPlayer::InstallPlaybackCommandActionL --->");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::InstallLaunchMusicPlayerL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMCPMusicPlayer::InstallLaunchMusicPlayerL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();
    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10282E5F ) ) );
    mapData->InsertL( KType, TLiwVariant( KLaunchApp ) );
    mapData->InsertL( KLaunchMethod, TLiwVariant( KCmdLine ) );
    mapData->InsertL(KApplicationUid,
            TLiwVariant(TInt32(KMusicPlayerAppUidAsTInt) ) );
    mapTrigger->InsertL( KData, TLiwVariant( mapData ) );
    
    iMHObserver->PublishActionL( this, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMCPMusicPlayer::InstallLaunchMusicPlayerL --->");
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::InstallGoToNowPlayingL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToNowPlayingL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();
    
    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10207C16 ) ) );
    mapData->InsertL( KType, TLiwVariant( KactionMessageToMusicPlayer ) );
    mapData->InsertL( KMessage, TLiwVariant( KGoToNowPlaying ) );
    mapTrigger->InsertL( KData , TLiwVariant( mapData ) );

    iMHObserver->PublishActionL( this, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData  );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToNowPlayingL --->");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::InstallGoToLastPlayedL(
        TMCPTriggerDestination aDestination, TBool aMinimized )
    {
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToLastPlayedL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();
    
    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10207C16 ) ) );
    mapData->InsertL( KType, TLiwVariant( KactionMessageToMusicPlayer ) );
    if (aMinimized)
        {
        mapData->InsertL( KMessage, TLiwVariant( KGoToLastPlayedMinimized ) );
        }
    else
        {
        mapData->InsertL( KMessage, TLiwVariant( KGoToLastPlayed ) );
        }
    mapTrigger->InsertL( KData , TLiwVariant( mapData ) );

    iMHObserver->PublishActionL( this, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData  );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToLastPlayedL --->");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::InstallGoToMusicL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToMusicL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();

    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10282E5F ) ) );
    
    mapData->InsertL( KType, TLiwVariant( KLaunchApp ) );
    mapData->InsertL( KLaunchMethod, TLiwVariant( KMessageWithTail ) );
    mapData->InsertL(KApplicationUid,
            TLiwVariant(TInt32(KMmUid3AsTInt) ) );
        mapData->InsertL( KAdditionalData, 
            TLiwVariant( KMessageForMMOpenMusicSuiteWithHide ) );
    mapData->InsertL(KMessageUid,
            TLiwVariant(TInt32(KMSGUidAsTInt) ) );
    
    mapTrigger->InsertL( KData, TLiwVariant( mapData ) );
    
    iMHObserver->PublishActionL( this, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToMusicL --->");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::InstallGoToAlbumL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToAlbumL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();
    
    mapTrigger->InsertL( KPluginUid, TLiwVariant( TUid::Uid( 0x10207C16 ) ) );
       mapData->InsertL( KType, TLiwVariant( KactionMessageToMusicPlayer ) );
       mapData->InsertL( KMessage, TLiwVariant( KGoToAlbumView ) );
    mapTrigger->InsertL( KData, TLiwVariant( mapData ) );
    
    iMHObserver->PublishActionL( this, aDestination, mapTrigger );
    
    CleanupStack::PopAndDestroy( mapData );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMCPMusicPlayer::InstallGoToAlbumL --->");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::InstallEmptyActionL(
        TMCPTriggerDestination aDestination )
    {
    MPX_DEBUG1("CMCPMusicPlayer::InstallEmptyActionL <---");
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    iMHObserver->PublishActionL( this, aDestination, mapTrigger );
    CleanupStack::PopAndDestroy( mapTrigger );
    MPX_DEBUG1("CMCPMusicPlayer::InstallEmptyActionL --->");
    }

// ---------------------------------------------------------------------------
// Installs actions and icons in the toolbar.
// ---------------------------------------------------------------------------
//

void CMCPMusicPlayer::UpdateToolBarL(TUint aToolBarState)
    {
    MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL <---");
    if (IsOKToPublishData())
        {
        TInt updateState;
        //we update if there is a change on the state and if the new propery
        //was set.
        // updateState = ( new ^ old ) & new = new & ~old
        updateState = aToolBarState & ~iToolBarState;
        TAknsItemID iconId;
        
        if ( TBK::KSkeep_L & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbPrev);
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB1, iconId, 
                    KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_prev, 
                    EMbmMusichomescreeniconsQgn_prop_image_tb_prev_mask);
            InstallPlaybackCommandActionL(EPbCmdPrevious, 
                    EMusicWidgetTB1Trigger);
            iToolBarState |= TBK::KPlay_dimmed;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KSkeep_L");
            }
        
        if ( TBK::KSkeep_R & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbNext);
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB3, iconId, 
                    KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_next, 
                    EMbmMusichomescreeniconsQgn_prop_image_tb_next_mask);
            InstallPlaybackCommandActionL(EPbCmdNext, EMusicWidgetTB3Trigger);
            iToolBarState |= TBK::KSkeep_R;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KSkeep_R");
            }
        
        if ( TBK::KPause & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbPause);
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB2, iconId, 
                    KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_pause, 
                    EMbmMusichomescreeniconsQgn_prop_image_tb_pause_mask);
            InstallPlaybackCommandActionL(EPbCmdPlayPause,
                    EMusicWidgetTB2Trigger);
            iToolBarState |= TBK::KPause;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KPause");
            }
        
        if ( TBK::KPlay & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbPlay2 );
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB2, iconId,
                    KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_play2, 
                    EMbmMusichomescreeniconsQgn_prop_image_tb_play2_mask);
            InstallPlaybackCommandActionL(EPbCmdPlayPause, 
                    EMusicWidgetTB2Trigger);
            iToolBarState |= TBK::KPlay;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KPlay");
            }
        
        if ( TBK::KPlay_last_played & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbPlay2 );
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB2, iconId,
                    KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_play2, 
                    EMbmMusichomescreeniconsQgn_prop_image_tb_play2_mask);
            InstallGoToLastPlayedL(EMusicWidgetTB2Trigger, ETrue); 
            iToolBarState |= TBK::KPlay_last_played;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KPlay_last_played");
            }
        
        if ( TBK::KSkeep_L_dimmed & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbPrevDimmed);
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB1, iconId, 
                    KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_prev_dimmed, 
                    EMbmMusichomescreeniconsQgn_prop_image_tb_prev_dimmed_mask);
            InstallEmptyActionL( EMusicWidgetTB1Trigger);
            iToolBarState |= TBK::KSkeep_L_dimmed;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KSkeep_L_dimmed");
            }
        
        if ( TBK::KSkeep_R_dimmed & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbNextDimmed);
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB3, iconId, 
                KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_next_dimmed, 
                EMbmMusichomescreeniconsQgn_prop_image_tb_next_dimmed_mask);
            InstallEmptyActionL( EMusicWidgetTB3Trigger);
            iToolBarState |= TBK::KSkeep_R_dimmed;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KSkeep_R_dimmed");
            }
        
        if ( TBK::KPause_dimmed & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbPauseDimmed);
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB2, iconId, 
                KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_pause_dimmed, 
                EMbmMusichomescreeniconsQgn_prop_image_tb_pause_dimmed_mask);
            InstallEmptyActionL( EMusicWidgetTB2Trigger);
            iToolBarState |= TBK::KPause_dimmed;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KPause_dimmed");
            }
        
        if ( TBK::KPlay_dimmed & updateState )
            {
            iconId.Set( EAknsMajorGeneric, 
                    EAknsMinorGenericQgnPropImageTbPlay2Dimmed);
            iMHObserver->PublishImageL( this, EMusicWidgetToolbarB2, iconId, 
                KMIF, EMbmMusichomescreeniconsQgn_prop_image_tb_play2_dimmed, 
                EMbmMusichomescreeniconsQgn_prop_image_tb_play2_dimmed_mask);
            InstallEmptyActionL( EMusicWidgetTB2Trigger);
            iToolBarState |= TBK::KPlay_dimmed;
            MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL KPlay_dimmed");
            }
        
        iToolBarState = aToolBarState;
        }
    MPX_DEBUG1("CMCPMusicPlayer::UpdateToolBarL <---");
    }

// ---------------------------------------------------------------------------
// Resets the widget an Music Suite.
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::ResetL()
    {
    MPX_DEBUG1("CMCPMusicPlayer::ResetL <---");
    //Note that we are not checking if IsOKToPublishData()This will only get
    //published if the plugin is still active on the music content publisher
    //meaning that no other plugin has preempted us.
    if ( iAppMonitor )
        {
        delete iAppMonitor;
        iAppMonitor = NULL;
        }
    if (iFileMonitor)
        {
        delete iFileMonitor;
        iFileMonitor = NULL;
        }
    if ( iPNSMonitor )
        {
        delete iPNSMonitor;
        iPNSMonitor = NULL;
        }
    //Reset
    iMHObserver->PublishTextL( this,EMusicWidgetDefaultText, iGoToMusicBuffer->Des() );
    iMHObserver->PublishImageL(this,EMusicWidgetImage1,KEmpty);
    InstallGoToAlbumL(EMusicWidgetTrigger1);
    iMHObserver->PublishTextL( this,EMusicWidgetText1, KEmpty );
    iMHObserver->PublishImageL(this,EMusicWidgetToolbarB1,KEmpty);
    iMHObserver->PublishImageL(this,EMusicWidgetToolbarB2,KEmpty);
    iMHObserver->PublishImageL(this,EMusicWidgetToolbarB3,KEmpty);
    InstallEmptyActionL(EMusicWidgetTB1Trigger);
    InstallEmptyActionL(EMusicWidgetTB2Trigger);
    InstallEmptyActionL(EMusicWidgetTB3Trigger);
    InstallGoToAlbumL(EMusicWidgetTrigger2);

    
    //Reset the music menu info
    InstallEmptyActionL(EMusicMenuMusicInfoTrigger);
    iMHObserver->PublishTextL( this, EMusicMenuMusicInfoLine1, 
            KLastPlayed );
    iMHObserver->PublishTextL( this, EMusicMenuMusicInfoLine2, 
            KEmpty );
    iMHObserver->PublishImageL( this, EMusicMenuMusicInfoImage1,
        KAknsIIDQgnIndiMupDefaultAlbum, 
        KMIF,
        EMbmMusichomescreeniconsQgn_indi_mup_default_album,
        EMbmMusichomescreeniconsQgn_indi_mup_default_album_mask); 
    MPX_DEBUG1("CMCPMusicPlayer::ResetL --->");
    }

// ----------------------------------------------------------------------------
// Get a heap descriptor from the resource file
// ----------------------------------------------------------------------------
//
void CMCPMusicPlayer::GetLocalizedStringL(RResourceFile& aResourceFile ,HBufC*& aRetBuf,
        TInt aResourceId )
   {
   MPX_DEBUG1("CMCPMusicPlayer::GetLocalizedStringL <---");
   HBufC8* dataBuffer = aResourceFile.AllocReadLC(aResourceId);
   TResourceReader theReader;
   theReader.SetBuffer(dataBuffer);
   aRetBuf = theReader.ReadHBufCL();
   CleanupStack::PopAndDestroy(dataBuffer);
   MPX_DEBUG1("CMCPMusicPlayer::GetLocalizedStringL --->");
   }

// ----------------------------------------------------------------------------
// Sets the plugin as active.
// ----------------------------------------------------------------------------
//
void CMCPMusicPlayer::ActivateL()
    {
    MPX_DEBUG1("CMCPMusicPlayer::ActivateL <---");
    if ( iPNSMonitor )
        {
        delete iPNSMonitor;
        iFileMonitor = NULL;
        MPX_DEBUG1("CMCPMusicPlayer::ActivateL pns monitor deleted");
        }
    iPNSMonitor = CPNSMonitor::NewL(*this);
    MPX_DEBUG1("CMCPMusicPlayer::ActivateL pns monitor created");
    iPNSMonitor->Start();
    if ( iAppMonitor )
        {
        delete iAppMonitor;
        iAppMonitor = NULL;
        }
    MPX_DEBUG1("CMCPMusicPlayer::ActivateL app monitor deleted");
    iAppMonitor = CApplicationMonitor::NewL(*this);
    MPX_DEBUG1("CMCPMusicPlayer::ActivateL app monitor created");
    TRAPD(err, 
            iAppMonitor->StartL( TUid::Uid( KMusicPlayerAppUidAsTInt ) ) 
          );
    MPX_DEBUG1("CMCPMusicPlayer::ActivateL app monitor started");
    if (KErrNone != err) //Music player is not running, it died somehow. 
        {
        MPX_DEBUG1("CMCPMusicPlayer::ActivateL music player is not running");
        if (iFileMonitor)
            {
            delete iFileMonitor;
            iFileMonitor = NULL;
            }
        MPX_DEBUG1("CMCPMusicPlayer::ActivateL file monitor deleted");
        const TDesC& uri = iEngine->Uri();
        if ( uri != KNullDesC )
            {
            MPX_DEBUG1("CMCPMusicPlayer::ActivateL uri is not null creatin file monitor");
            iFileMonitor = CFileMonitor::NewL(*this);
            iFileMonitor->StartL( uri );
            }
        iActive = EFalse;
        }
    else //Music player is running
        {
        MPX_DEBUG1("CMCPMusicPlayer::ActivateL music player is running, activating...");
        iMHObserver->BecameActiveL(this);
        iActive = ETrue;
        MPX_DEBUG1("CMCPMusicPlayer::ActivateL we are active now");
        
        iMHObserver->PublishTextL( this, EMusicWidgetDefaultText, KEmpty );
        InstallGoToAlbumL(EMusicWidgetTrigger1);
        InstallGoToLastPlayedL(EMusicMenuMusicInfoTrigger, EFalse);
        iMHObserver->PublishTextL(this, EMusicMenuMusicInfoLine1, 
                 KNowPlaying);
        DoUpdateTrackInfoL(iEngine->TitleL(), iEngine->Artist());
        iToolBarState = 0;
        DoUpdatePlayerStateL( iEngine->PlayerState() );
        DoUpdateAlbumArtL(iArtCached ? iCachedArtBitmap : iArtBitmap);
        }
    MPX_DEBUG1("CMCPMusicPlayer::ActivateL --->");
    }


void CMCPMusicPlayer::DoUpdatePlayerStateL(TMPlayerState aState)
    {
    MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL <---");
    switch ( aState )
            {
            case EMPlayerStatePlaying:
                {
                MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL EMPlayerStatePlaying");
                UpdateToolBarL(TBK::KSkeep_L|TBK::KPause|TBK::KSkeep_R);
                break;
                }
            case EMPlayerStatePaused:
                {
                MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL EMPlayerStatePaused");
                UpdateToolBarL(TBK::KSkeep_L|TBK::KPlay|TBK::KSkeep_R);
                break;
                }
            case EMPlayerStateSeeking:
                {
                MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL EMPlayerStateSeeking");
                UpdateToolBarL(TBK::KSkeep_L_dimmed |
                        TBK::KPlay_dimmed |
                        TBK::KSkeep_R_dimmed);
                break;
                }
            case EMPlayerStateStopped:
                {
                MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL EMPlayerStateStopped");
                if ( iUSBOnGoing )
                    {
                    UpdateToolBarL( TBK::KSkeep_L_dimmed |
                            TBK::KPlay_dimmed |
                            TBK::KSkeep_R_dimmed );
                    }
                else
                    {
                    UpdateToolBarL(TBK::KSkeep_L|TBK::KPlay|TBK::KSkeep_R);
                    }
                break;
                }
            default:
                MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL default");
                break;
            }
    MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL --->");
    }

void CMCPMusicPlayer::DoUpdateTrackInfoL(const TDesC& aTitle, const TDesC& aArtist)
    {
    MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL <---");
    if ( IsOKToPublishData() )
            {
            HBufC* nowPlayingForMM = HBufC::NewLC( aTitle.Length() + 
            		                               KSeparatorhyphen().Length() + aArtist.Length() );     
            TPtr nowPlayingForMMPtr = nowPlayingForMM->Des();
            if ( &aTitle && aTitle.Length() )
                {
                nowPlayingForMMPtr.Copy( aTitle );
                }
            if ( &aArtist && aArtist.Length() )
                {
                nowPlayingForMMPtr.Append(KSeparatorhyphen);
                nowPlayingForMMPtr.Append(aArtist);
                }            
            iMHObserver->PublishTextL( this,EMusicWidgetText1, *nowPlayingForMM );
            iMHObserver->PublishTextL(this, EMusicMenuMusicInfoLine2,
                    *nowPlayingForMM);
            CleanupStack::PopAndDestroy( nowPlayingForMM );
            MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL deliting file monitor");
            if (iFileMonitor)
                {
                delete iFileMonitor;
                iFileMonitor = NULL;
                }
            
            const TDesC& uri = iEngine->Uri();
            if ( uri != KNullDesC )
                {
                MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL creating file monitor");
                iFileMonitor = CFileMonitor::NewL(*this);
                iFileMonitor->StartL( uri );
                }
            }
    MPX_DEBUG1("CMCPMusicPlayer::DoUpdatePlayerStateL --->");
    }

void CMCPMusicPlayer::DoUpdatePlaybackPositionL(TInt /*aPosition*/)
    {
    /*
     * 
     * This code was left here in case some parts are helpfull
     *  in the future, here is some code that used to work, 
     *  Members and APIs used migh have changed.
     *  
        if ( IsOKToPublishData() )
            {
            const TInt KMPXMinSecSeparatorIndex = 2;
            const TInt KMPXOneSecInMicroSecs = 1000000;
            const TInt KMPXOneHourInSeconds = 60 * 60;
            const TInt KMPXTimeIndicatorLength = 16;
            
            TBuf<KMPXTimeIndicatorLength> elapsed;
            TBuf<KMPXTimeIndicatorLength> total;
        
            TInt64 playbackPosInSeconds;
            TInt64 totalLengthInSeconds;
        
            playbackPosInSeconds = aPosition;
            totalLengthInSeconds = iEngine->Duration();
            HBufC* longFormatString;
            HBufC* shortFormatString;
        
            longFormatString = StringLoader::LoadL(
                    R_QTN_TIME_DURAT_LONG_WITH_ZERO );
            shortFormatString = StringLoader::LoadL(
                    R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO) ;
            TPtrC format = *shortFormatString;
        
            if ( totalLengthInSeconds >= KMPXOneHourInSeconds )
                {
                // For tracks longer than an hour we use different time format and
                //a slightly different layout which has more space for the time
                //labels.
                format.Set( *longFormatString );
                }
        
            TTime elapsedTime( playbackPosInSeconds * KMPXOneSecInMicroSecs );
            TTime totalTime( totalLengthInSeconds * KMPXOneSecInMicroSecs );
        
            if ( aPosition == 0 )
                {
                totalLengthInSeconds = 0;
                }
        
            // Convert total playing time to texts.
            elapsedTime.FormatL( elapsed, format );
        
            if ( totalLengthInSeconds )
                {
                // Time remaining
                totalTime.FormatL( total, format );
                }
            else
                {
                // Time remaining: --:--
                TLocale locale;
                TBuf<KMPXTimeIndicatorLength> pos;
                TChar separator = locale.TimeSeparator(KMPXMinSecSeparatorIndex);
                total = KMPXZeroDurationMark;
                total.Append( separator );
                total += KMPXZeroDurationMark;
                }
        
            AknTextUtils::LanguageSpecificNumberConversion( elapsed );
            AknTextUtils::LanguageSpecificNumberConversion( total );
            TBuf<128> duration;
            TBuf<128> elapsedTimebuf;
        
            elapsedTimebuf.Copy( elapsed );
        
            duration.Copy( elapsed );
            duration.Append( KSeparator );
            duration.Append( total );
        
            iMHObserver->PublishTextL( this, EMusicWidgetText3, duration );
        
            delete longFormatString;
            delete shortFormatString;
            }
     */
    }

void CMCPMusicPlayer::DoUpdateAlbumArtL( CFbsBitmap* aBitmap )
    {
    MPX_DEBUG1("CMCPMusicPlayer::DoUpdateAlbumArtL <---");
    //Make sure that iCachedArtBitmap is deleted or nulled in case it is an alias.
    //This ensures that the data is not double deleted nor orphan.
    if ( iCachedArtBitmap && iCachedArtBitmap !=  aBitmap  && iCachedArtBitmap != iArtBitmap)
        {
        MPX_DEBUG1("CMCPMusicPlayer::DoUpdateAlbumArtL delete iCachedArtBitmap");
        delete iCachedArtBitmap;
        }
    iCachedArtBitmap = NULL;
    //Store the new bitmap.
    if ( aBitmap != iArtBitmap )
        {
        MPX_DEBUG1("CMCPMusicPlayer::DoUpdateAlbumArtL delete iArtBitmap");
        delete iArtBitmap;
        }
    iArtBitmap = aBitmap;
    iArtCached = EFalse;
    
    if (iArtBitmap)
        {
        TInt handle = iArtBitmap->Handle();
        if (iArtBitmapMask)
            {
            TInt mskHandle = iArtBitmapMask->Handle();
            iMHObserver->PublishImageL(this,EMusicWidgetImage1,handle,
                    mskHandle);
            }
        else
            {
            iMHObserver->PublishImageL(this,EMusicWidgetImage1,handle);
            }
        iMHObserver->PublishImageL( this, EMusicMenuMusicInfoImage1, 
                handle );
        }
    else
        {
        iMHObserver->PublishImageL( this, EMusicMenuMusicInfoImage1,
                    KAknsIIDQgnIndiMupDefaultAlbum, 
                    KMIF, EMbmMusichomescreeniconsQgn_indi_mup_default_album, 
                    EMbmMusichomescreeniconsQgn_indi_mup_default_album_mask); 
        iMHObserver->PublishImageL( this, EMusicWidgetImage1,
                    KAknsIIDQgnIndiMupDefaultAlbum, 
                    KMIF, EMbmMusichomescreeniconsQgn_indi_mup_default_album, 
                    EMbmMusichomescreeniconsQgn_indi_mup_default_album_mask);
        }
    MPX_DEBUG1("CMCPMusicPlayer::DoUpdateAlbumArtL --->");
    }

void CMCPMusicPlayer::DoHandleOpeningL()
    {
    MPX_DEBUG1("CMCPMusicPlayer::DoHandleOpeningL <---");
    if ( IsOKToPublishData() )
        {
        iMHObserver->PublishTextL( this,EMusicWidgetText1, iOpeningBuffer->Des() );
        }
    MPX_DEBUG1("CMCPMusicPlayer::DoHandleOpeningL --->");
    }

void CMCPMusicPlayer::DoHandlePlaylisIsEmptyL()
    {
    MPX_DEBUG1("CMCPMusicPlayer::DoHandlePlaylisIsEmptyL <---");
    iActive = EFalse;
    ResetL();
    MPX_DEBUG1("CMCPMusicPlayer::DoHandlePlaylisIsEmptyL --->");
    }
void CMCPMusicPlayer::DoHandleSkinChangedL()
    {
    MPX_DEBUG1("CMCPMusicPlayer::DoHandleSkinChangedL <---");
    //To make sure the mask gets recreated even if we don't use it right now.
    delete iArtBitmapMask;
    iArtBitmapMask = NULL;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    TRAPD(err, iArtBitmapMask = AknsUtils::CreateBitmapL( skin, 
            KAknsIIDQgnHomePhotoMask ) );
    if (err == KErrNone)
        {
        AknIconUtils::DisableCompression(iArtBitmapMask);
        TSize size( 70 , 70);
        AknIconUtils::SetSize( iArtBitmapMask, size );
        if (iArtBitmapMask->SetDisplayMode( EGray256 ) != KErrNone)
            {
            delete iArtBitmapMask;
            iArtBitmapMask = NULL;
            }
        }
    else
        {
        delete iArtBitmapMask;
        iArtBitmapMask = NULL;
        }
    
    if (iArtBitmap)
        {
        TInt handle = iArtBitmap->Handle();
        if (iArtBitmapMask)
            {
            TInt mskHandle = iArtBitmapMask->Handle();
            iMHObserver->PublishImageL(
                    this,
                    EMusicWidgetImage1,
                    handle, 
                    mskHandle);
            }
        else
            {
            iMHObserver->PublishImageL(
                    this,
                    EMusicWidgetImage1,
                    handle);
            }
        }
    else
        {
        iMHObserver->PublishImageL( this, EMusicMenuMusicInfoImage1,
                    KAknsIIDQgnIndiMupDefaultAlbum, 
                    KMIF, EMbmMusichomescreeniconsQgn_indi_mup_default_album, 
                    EMbmMusichomescreeniconsQgn_indi_mup_default_album_mask); 
        iMHObserver->PublishImageL( this, EMusicWidgetImage1,
                    KAknsIIDQgnIndiMupDefaultAlbum, 
                    KMIF, EMbmMusichomescreeniconsQgn_indi_mup_default_album, 
                    EMbmMusichomescreeniconsQgn_indi_mup_default_album_mask);
        }
       
    TUint tbstate = iToolBarState;
    //To Force a change on the toolbar with the current properties.
    iToolBarState = 0;
    if (!iActive)
        {
        //This is to force the icons to update, but we still want to keep
        //the plugin as incative.
        iActive = ETrue;
        UpdateToolBarL(tbstate);
        iActive = EFalse;
        }
    else
        {
        UpdateToolBarL(tbstate);
        }
    MPX_DEBUG1("CMCPMusicPlayer::DoHandleSkinChangedL --->");
    }
   


// ---------------------------------------------------------------------------
// From CMCPPlugin
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::Deactivate()
    {
    MPX_DEBUG1("CMCPMusicPlayer::Deactivate() <---");
    iActive = EFalse;
    if (iFileMonitor)
        {
        delete iFileMonitor;
        iFileMonitor = NULL;
        }
    if (iAppMonitor)
        {
        delete iAppMonitor;
        iAppMonitor = NULL;
        }
    if ( iPNSMonitor )
        {
        delete iPNSMonitor;
        iPNSMonitor = NULL;
        }
    MPX_DEBUG1("CMCPMusicPlayer::Deactivate() --->");
    }

// ---------------------------------------------------------------------------
// From CMCPPlugin
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::SkinChanged()
    {
    MPX_DEBUG1("CMCPMusicPlayer::SkinChanged() <---");
    TRAP_IGNORE( DoHandleSkinChangedL() );
    MPX_DEBUG1("CMCPMusicPlayer::SkinChanged() --->");
    }

// ---------------------------------------------------------------------------
// From MAiPlayerPluginEngineObserver
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::PlayerStateChanged( TMPlayerState aState )
    {
    MPX_DEBUG1("CMCPMusicPlayer::PlayerStateChanged <---");
    //we want to activate only with states that can be triggered by the user.
    if (!iActive && 
	        aState == EMPlayerStatePlaying)
        {
        //ActivateL updates the whole widget and music suite data.
        MPX_DEBUG1("CMCPMusicPlayer::PlayerStateChanged activating");
        TRAP_IGNORE( ActivateL() );
        }
    else
        {
        MPX_DEBUG1("CMCPMusicPlayer::PlayerStateChanged updating state");
        TRAP_IGNORE(DoUpdatePlayerStateL(aState));
        }
    MPX_DEBUG1("CMCPMusicPlayer::PlayerStateChanged --->");
    }

// ---------------------------------------------------------------------------
// From MAiPlayerPluginEngineObserver
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::TrackInfoChanged( const TDesC& aTitle, 
        const TDesC& aArtist )
    
    {
    MPX_DEBUG1("CMCPMusicPlayer::TrackInfoChanged <---");
    if (iActive)
        {
        MPX_DEBUG1("CMCPMusicPlayer::TrackInfoChanged updating track info");
        TRAP_IGNORE(DoUpdateTrackInfoL(aTitle, aArtist));
        }
    MPX_DEBUG1("CMCPMusicPlayer::TrackInfoChanged --->");
    }

// ---------------------------------------------------------------------------
// From MAiPlayerPluginEngineObserver
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::PlaybackPositionChanged( TInt aPosition )
    {
    TRAP_IGNORE( DoUpdatePlaybackPositionL( aPosition ) );
    }

// ---------------------------------------------------------------------------
// From MAiPlayerPluginEngineObserver
// ---------------------------------------------------------------------------
// 
void CMCPMusicPlayer::AlbumArtChanged( CFbsBitmap* aBitmap )
    {
    MPX_DEBUG1("CMCPMusicPlayer::AlbumArtChanged <---");
    if (!iActive)
        {
        iCachedArtBitmap = aBitmap;
        iArtCached = ETrue;
        }
    else
        {
        MPX_DEBUG1("CMCPMusicPlayer::AlbumArtChanged updating album art");
        TRAP_IGNORE( DoUpdateAlbumArtL( aBitmap ) );
        }
    MPX_DEBUG1("CMCPMusicPlayer::AlbumArtChanged --->");
    }

// ---------------------------------------------------------------------------
// From MAiPlayerPluginEngineObserver
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::Opening()
    {
    MPX_DEBUG1("CMCPMusicPlayer::Opening <---");
    TRAP_IGNORE( DoHandleOpeningL() );
    MPX_DEBUG1("CMCPMusicPlayer::Opening --->");
    }


// ---------------------------------------------------------------------------
// From MAiPlayerPluginEngineObserver
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::PlaylisIsEmpty()
    {
    //TRAP_IGNORE( DoHandlePlaylisIsEmptyL() );
    }
// ---------------------------------------------------------------------------
// From MAiPlayerPluginEngineObserver
// ---------------------------------------------------------------------------
//  
void CMCPMusicPlayer::HandleFileRemovedL()
    {
    MPX_DEBUG1("CMCPMusicPlayer::HandleFileRemovedL <---");
    iActive = EFalse;
    ResetL();
    MPX_DEBUG1("CMCPMusicPlayer::HandleFileRemovedL --->");
    }


// ---------------------------------------------------------------------------
// From MApplicationMonitorObserver
// ---------------------------------------------------------------------------
// 
void CMCPMusicPlayer::HandleApplicationClosedL(TExitType aReason)
    {
    MPX_DEBUG1("CMCPMusicPlayer::HandleApplicationClosedL <---");
    if (aReason == EExitPanic)
        {
        MPX_DEBUG1("CMCPMusicPlayer::HandleApplicationClosedL panic");
        iActive = EFalse;
        ResetL();
        }
    else
        {
        MPX_DEBUG1("CMCPMusicPlayer::HandleApplicationClosedL normal close");
        UpdateToolBarL( TBK::KSkeep_L_dimmed |
                TBK::KPlay_last_played |
                TBK::KSkeep_R_dimmed );
        if (IsOKToPublishData() )
            {
            iMHObserver->PublishTextL( this, EMusicWidgetDefaultText, KEmpty );
            InstallGoToLastPlayedL(EMusicMenuMusicInfoTrigger, EFalse);       
            iMHObserver->PublishTextL(this, EMusicMenuMusicInfoLine1,
                                KLastPlayed);     
            DoUpdateTrackInfoL(iEngine->TitleL(), iEngine->Artist());
            DoUpdateAlbumArtL(iArtCached ? iCachedArtBitmap : iArtBitmap);
            }
        if ( iAppMonitor )
            {
            MPX_DEBUG1("CMCPMusicPlayer::HandleApplicationClosedL deliting app monitor");
            delete iAppMonitor;
            iAppMonitor = NULL;
            }
        
        iActive = EFalse;
        //in case the app closed before the info was updated.
        if (iEngine->TitleL() == KNullDesC)
            {
            MPX_DEBUG1("CMCPMusicPlayer::HandleApplicationClosedL there is no title - resetting");
            ResetL();
            }
        }
    MPX_DEBUG1("CMCPMusicPlayer::HandleApplicationClosedL --->");
    }
// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection messages.
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr )
    {
    MPX_DEBUG1("--->CMCPMusicPlayer::HandleCollectionMessage()");
    if ( aErr == KErrNone && aMsg )
        {
        TRAP_IGNORE( HandleCollectionMessageL( aMsg ));
        }
    MPX_DEBUG1("<---CMCPMusicPlayer::HandleCollectionMessage()");
    }
// ---------------------------------------------------------------------------
// Handle collection messages.
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::HandleCollectionMessageL( CMPXMessage* aMsg )
    {
    MPX_DEBUG1("--->CMCPMusicPlayer::HandleCollectionMessageL()");
    TMPXMessageId messageId = aMsg->ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId );
    if ( messageId == KMPXMessageGeneral )
        {
        DoHandleGeneralMessageL( *aMsg );
        }
    MPX_DEBUG1("<---CMCPMusicPlayer::HandleCollectionMessageL()");
    }
// ---------------------------------------------------------------------------
// Handle collection general message.
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::DoHandleGeneralMessageL(const CMPXMessage& aMsg)
    {
    TInt event = aMsg.ValueTObjectL<TInt> ( KMPXMessageGeneralEvent );
    TInt type = aMsg.ValueTObjectL<TInt> ( KMPXMessageGeneralType );
    MPX_DEBUG3("--->CMCPMusicPlayer::DoHandleGeneralMessageL(), event = %d, type = %d", event, type);

    if ( event == TMPXCollectionMessage::EBroadcastEvent
        && ( type == EMcMsgUSBMassStorageStart || type == EMcMsgUSBMTPStart ))
        {
        iUSBOnGoing = ETrue;
        MPX_DEBUG2("CMCPMusicPlayer::DoHandleGeneralMessageL(), iUSBOnGoing changed to: %d", iUSBOnGoing );
        UpdateToolBarL( TBK::KSkeep_L_dimmed |
                TBK::KPlay_dimmed |
                TBK::KSkeep_R_dimmed );
        if ( IsOKToPublishData() )
            {
            InstallGoToAlbumL(EMusicMenuMusicInfoTrigger);
            }
        }
    else if ( event == TMPXCollectionMessage::EBroadcastEvent
            && ( type == EMcMsgUSBMassStorageEnd || type == EMcMsgUSBMTPEnd ))
        {
        iUSBOnGoing = EFalse;
        MPX_DEBUG2("CMCPMusicPlayer::DoHandleGeneralMessageL(), iUSBOnGoing changed to: %d", iUSBOnGoing );
        DoUpdatePlayerStateL( iEngine->PlayerState() );
        if ( IsOKToPublishData() )
            {
            iMHObserver->PublishTextL( this, EMusicWidgetDefaultText,
                    KEmpty );
            InstallGoToLastPlayedL( EMusicMenuMusicInfoTrigger, ETrue );
            iMHObserver->PublishTextL( this, EMusicMenuMusicInfoLine1,
                    KLastPlayed );
            DoUpdateTrackInfoL( iEngine->TitleL(), iEngine->Artist() );
            DoUpdateAlbumArtL( iArtCached ? iCachedArtBitmap : iArtBitmap );
            }
        }
    MPX_DEBUG3("<---CMCPMusicPlayer::DoHandleGeneralMessageL(), event = %d, type = %d", event, type);
    }
// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// ---------------------------------------------------------------------------  
//
void CMCPMusicPlayer::HandleOpenL( const CMPXMedia& /*aEntries*/,
        TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// ---------------------------------------------------------------------------
//
void CMCPMusicPlayer::HandleOpenL( const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/ )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionMediaObserver
// ---------------------------------------------------------------------------
void CMCPMusicPlayer::HandleCollectionMediaL( const CMPXMedia& /*aMedia*/, TInt /*aError*/ )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// From MPNSMonitorObserver
// ---------------------------------------------------------------------------
// 
void CMCPMusicPlayer::OpeningMusicPlayer()
    {
    MPX_DEBUG1("CMCPMusicPlayer::OpeningMusicPlayer <---");
    // in this position, iActive is EFalse, so we can not call function
    // DoHandleOpeningL() directly.
    TRAP_IGNORE( 
            iMHObserver->PublishTextL( this,
                    EMusicWidgetText1,
                    iOpeningBuffer->Des() ) );
    
    TCallBack cb( MusicPlayerOpeningTimerCallback, this );
    iMusicPlayerOpeningTimer->Cancel();
    iMusicPlayerOpeningTimer->Start( KMusicPlayerOpeningTimerInterval,
            KMusicPlayerOpeningTimerInterval, cb );

    MPX_DEBUG1("CMCPMusicPlayer::OpeningMusicPlayer --->");
    }

// ----------------------------------------------------------------------------
// Music Player opening timer callback
// ----------------------------------------------------------------------------
//
TInt CMCPMusicPlayer::MusicPlayerOpeningTimerCallback( TAny* aPtr )
    {
    MPX_DEBUG1("CMCPMusicPlayer::MusicPlayerOpeningTimerCallback <---");
    RWsSession wsSession;
    TInt error = wsSession.Connect();
    if ( error != KErrNone )
        {
        return error;
        }
    
    TBool taskExists( EFalse );
    CAknTaskList* taskList( NULL );
    TRAPD( err, taskList = CAknTaskList::NewL( wsSession ) );
    if ( err == KErrNone )
        {
        MPX_DEBUG2("CMCPMusicPlayer::MusicPlayerOpeningTimerCallback "
                "error = %d occur when creating CAknTaskList", err);
        TApaTask task = taskList->FindRootApp( TUid::Uid( KMusicPlayerAppUidAsTInt ) );
        delete taskList;
        taskExists = task.Exists();
        }
    wsSession.Close();
    
    if ( !taskExists )
        {
        MPX_DEBUG1("CMCPMusicPlayer::MusicPlayerOpeningTimerCallback Music Player not opened");
        //Reset the widget and menu, music player is not running
        TRAP_IGNORE( static_cast<CMCPMusicPlayer*>(aPtr)->ResetL() );
        }
    static_cast<CMCPMusicPlayer*>(aPtr)->iMusicPlayerOpeningTimer->Cancel();
    
    MPX_DEBUG1("CMCPMusicPlayer::MusicPlayerOpeningTimerCallback --->");
    return KErrNone;
    }
 //  End of File
