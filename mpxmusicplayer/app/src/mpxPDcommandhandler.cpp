/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles commands passed in by other appilcations
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <apgtask.h>
#include <eikenv.h>
#include <coeutils.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxviewutility.h>
#include <mpxmusicplayerviewplugin.hrh>
#include <mpxparameter.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessagedefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>
#include <AiwGenericParam.h>
#include <pathinfo.h>
#include <mpxcommandgeneraldefs.h>
#include <caf/caferr.h>
#include <StringLoader.h>

#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionplugin.hrh>
#include <mpxdrmmediautility.h>
#include <mpxmediadrmdefs.h>

#include "mpxtlshelper.h"
#include "mpxconstants.h"
#include "mpxPDcommandhandler.h"
#include "mpxlog.h"
#include "mpxcommonuihelper.h"
#include <mpxmediakeyhandler.rsg>

// CONSTANTS
#define KProgressDownloadUid  0x10207BCD

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXPdCommandHandler* CMPXPdCommandHandler::NewL(
    MMPXPlaybackUtility* aPlaybackUtility)
    {
    CMPXPdCommandHandler* self = NewLC(aPlaybackUtility);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXPdCommandHandler* CMPXPdCommandHandler::NewLC(
    MMPXPlaybackUtility* aPlaybackUtility)
    {
    CMPXPdCommandHandler* self = new ( ELeave ) CMPXPdCommandHandler( aPlaybackUtility );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// C++ constructor can NOT contain any code that might leave.
// ----------------------------------------------------------------------------
//
CMPXPdCommandHandler::CMPXPdCommandHandler(
    MMPXPlaybackUtility* aPlaybackUtility ):
    iPlaybackUtility( aPlaybackUtility )
    ,iPlaybackCompleted(EFalse)
    ,iPrevousPlaybackState(EPbStateNotInitialised)
    {
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CMPXPdCommandHandler::~CMPXPdCommandHandler()
    {
    if ( iPdPlaybackUtility )
        {
        TRAP_IGNORE( iPdPlaybackUtility->RemoveObserverL( *this ) );
        TRAP_IGNORE( iPdPlaybackUtility->CommandL( EPbCmdStop ) );
        TRAP_IGNORE( iPdPlaybackUtility->CommandL( EPbCmdClose ) );
        iPdPlaybackUtility->PlayerManager().ClearSelectPlayersL();
        iPdPlaybackUtility->Close();
        }
    if ( iViewUtility )
        {
        iViewUtility->Close();
        }
    delete iDrmMediaUtility;
    delete iCommonUiHelper;
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXPdCommandHandler::ConstructL()
    {
    iViewUtility = MMPXViewUtility::UtilityL();
    }


// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CMPXPdCommandHandler::HandlePlaybackMessage(
    CMPXMessage* aMessage, TInt aError )
    {
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    }


// -----------------------------------------------------------------------------
// Handle playback message
// -----------------------------------------------------------------------------
//
void CMPXPdCommandHandler::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    MPX_FUNC( "CMPXPdCommandHandler::DoHandlePlaybackMessageL" );
    TMPXMessageId id( *aMessage.Value<TMPXMessageId>( KMPXMessageGeneralId ) );
    if ( KMPXMessageGeneral == id )
        {
        switch ( *aMessage.Value<TInt>( KMPXMessageGeneralEvent ) )
            {
            case TMPXPlaybackMessage::EInitializeComplete:
                break;

            case TMPXPlaybackMessage::EDownloadCmdCancelDownload:
                iPdPlaybackUtility->PlayerManager().ClearSelectPlayersL();
                iPlaybackCompleted = ETrue;
                break;

            case TMPXPlaybackMessage::EStateChanged:
                {
                TInt type( *aMessage.Value<TInt>( KMPXMessageGeneralType ) );
                MPX_DEBUG2( "CMPXPdSbPlaybackViewImp::DoHandlePlaybackMessageL - EStateChanged(%d)", type );

                TMPXPlaybackState state =
                    static_cast<TMPXPlaybackState>( type );
                DoHandleStateChangedL( state );
                iPrevousPlaybackState = state;
                break;
                }
            case TMPXPlaybackMessage::EMediaChanged:

                iPlaybackCompleted = ETrue;
                break;
            default:
                {
                // ignore other playback messages
                break;
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CMPXAppUi::HandleCommandParametersL
// -----------------------------------------------------------------------------
//
TInt CMPXPdCommandHandler::HandleCommandParametersL(
    const TDesC8& aParams )
    {
    CBufFlat* paramBuf = CBufFlat::NewL( /*anExpandSize*/1 );
    CleanupStack::PushL( paramBuf );
    paramBuf->InsertL( 0, aParams );
    RBufReadStream readStream( *paramBuf );

    CAiwGenericParamList* genParList = CAiwGenericParamList::NewLC( readStream );

    // Unpack download ID
    TInt index = 0;

    const TAiwGenericParam* genParam = genParList->FindFirst( index,
        EGenericParamDownloadId, EVariantTypeTInt32 );
    TInt32 downloadID;
    if ( genParam )
        {
        genParam->Value().Get( downloadID );
        }
    else
        {
        CleanupStack::PopAndDestroy( genParList );
        CleanupStack::PopAndDestroy( paramBuf );
        if (!iPdPlaybackUtility)
            {
            iPlaybackCompleted = ETrue;
            }          
        
        return KErrNotFound;   
        }

    index = 0;
    // Unpack fileName
    const TAiwGenericParam* paramFileName =
        genParList->FindFirst( index, EGenericParamFile, EVariantTypeDesC );
    TPtrC fileName;
    if ( index == KErrNotFound )
        {
        fileName.Set( KNullDesC );
        }
    else
        {
        fileName.Set( paramFileName->Value().AsDes() );
        }
    if (!ConeUtils::FileExists(fileName))
        {
        CleanupStack::PopAndDestroy( genParList );
        CleanupStack::PopAndDestroy( paramBuf );
        if (!iPdPlaybackUtility)
            {
            iPlaybackCompleted = ETrue;
            } 
        return KErrNotFound;        
        }
    if ( downloadID == KErrNotFound )
        {
        if ( iPdPlaybackUtility )
            {
            if ( iPdPlaybackUtility->StateL() == EPbStatePlaying )
                {
                iPdPlaybackUtility->CommandL(EPbCmdPause);
                }
            }
        iPlaybackUtility->InitL(fileName);
            MMPXPlayer* player =
                iPlaybackUtility->PlayerManager().CurrentPlayer();
            TUid pluginUid( KNullUid );
            RArray<TUid> array;
            CleanupClosePushL( array );

            if ( player )
                {
                array.AppendL( player->UidL() );
                }
            else
                {
                array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ) );
                }

            // add the collection plugin UID to ensure the correct
            // last playback view is activated
            MMPXSource* source = iPlaybackUtility->Source();
            if( source )
                {
                CMPXCollectionPlaylist* pl = source->PlaylistL();
                if (pl)
                    {
                    CleanupStack::PushL( pl );
                    const CMPXCollectionPath& path = pl->Path();
                    if(path.Levels() > 0)
                        {
                        array.AppendL(TUid::Uid(path.Id(0)));
                        }
                    CleanupStack::PopAndDestroy( pl );
                    }
                }
            iViewUtility->ActivateViewL( array );
            CleanupStack::PopAndDestroy( &array );

        }
    else if ( iCurrentFileName == fileName && iPdPlaybackUtility)
        {
        if ( iPlaybackUtility )
            {
             if ( iPlaybackUtility->StateL() == EPbStatePlaying )
                {
                iPlaybackUtility->CommandL(EPbCmdPause);
                }
            }
        if ( iPdPlaybackUtility->StateL() == EPbStatePaused )
            {
            iPdPlaybackUtility->CommandL( EPbCmdPlay );
            }
        ActivatePlaybackViewL();
        }
    else
        {
        // Create a new playback utility instance
        if (iPdPlaybackUtility)
            {
            iPdPlaybackUtility->RemoveObserverL( *this );
            iPdPlaybackUtility->PlayerManager().ClearSelectPlayersL();
            iPdPlaybackUtility->Close();
            }
        iPdPlaybackUtility = MMPXPlaybackUtility::UtilityL( 
                TUid::Uid(KProgressDownloadUid) );
        MMPXPlayerManager& manager = iPdPlaybackUtility->PlayerManager();
        manager.SelectPlayerL(TUid::Uid(KProgressDownloadUid));
        iPdPlaybackUtility->AddObserverL(*this);

        SendCustomCommandToMPXL( downloadID, EPbCmdStartPd );
        iPlaybackCompleted = EFalse;
        if ( iPlaybackUtility && iPlaybackUtility->StateL() == EPbStatePlaying )
            {
            iPlaybackUtility->CommandL(EPbCmdPause);
            }
        iPdPlaybackUtility->InitL(fileName);
        ActivatePlaybackViewL();
        iCurrentFileName=fileName;
        }
    CleanupStack::PopAndDestroy( genParList );
    CleanupStack::PopAndDestroy( paramBuf );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handle media key commands.
// ---------------------------------------------------------------------------
//
void CMPXPdCommandHandler::HandleCommandL(
    TMPXPlaybackCommand aCommand )
    {
    MPX_FUNC( "CMPXPdCommandHandler::HandleCommandL" );
	MMPXPlaybackUtility* activePlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeActivePlayer );
	if (activePlaybackUtility)
		{
		CleanupClosePushL(*activePlaybackUtility);
		activePlaybackUtility->CommandL(aCommand);
		CleanupStack::PopAndDestroy(activePlaybackUtility);
		}
    }

// ---------------------------------------------------------------------------
// Current state of player
// ---------------------------------------------------------------------------
//
TMPXPlaybackState CMPXPdCommandHandler::PdStateL() const
    {
    MPX_FUNC( "CMPXPdCommandHandler::PdStateL" );
    TMPXPlaybackState pbPlayerState( EPbStateNotInitialised );
    if ( iPdPlaybackUtility )
        {
        pbPlayerState = iPdPlaybackUtility->StateL();
        }
    
    return pbPlayerState;
    }

// ---------------------------------------------------------
// CMPXPdCommandHandler::ActivatePlaybackViewL()
// ---------------------------------------------------------
//
void CMPXPdCommandHandler::ActivatePlaybackViewL()
    {
    MMPXPlayer* player = iPdPlaybackUtility->PlayerManager().CurrentPlayer();
    TUid pluginUid( KNullUid );
    RArray<TUid> array;
    CleanupClosePushL( array );
    if ( player )
        {
        pluginUid = player->UidL();
        array.AppendL( pluginUid );
        }
    array.AppendL( TUid::Uid( KMPXPluginTypePlaybackUid ));
    iViewUtility->ActivateViewL( array );
    CleanupStack::PopAndDestroy( &array );
    }

// --------------------------------------------------------------
// CMPXPdCommandHandler::SendCustomCommandToMPXL()
// --------------------------------------------------------------
//
void CMPXPdCommandHandler::SendCustomCommandToMPXL(
                        TUint aTransactionID,
                        TMPXPlaybackPdCommand aCustomCmd)
    {
    MPX_DEBUG2("CMPXPdCommandHandler::SendCustomCommandToMPXL(aEvent=%d) (aCustomCmd=%d)", aCustomCmd);

    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType, aCustomCmd );
    cmd->SetTObjectValueL<TUint>( KMPXCommandPlaybackPDTransactionID, aTransactionID );
    iPdPlaybackUtility->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    }



// ---------------------------------------------------------------------------
// Handle playback state changed.
// ---------------------------------------------------------------------------
//
void CMPXPdCommandHandler::DoHandleStateChangedL(
    TMPXPlaybackState aState )
    {
    MPX_DEBUG2( "CMPXPdCommandHandler::DoHandleStateChangedL(%d)", aState );

    switch ( aState )
        {
        case EPbStateStopped:
            {
            iPlaybackCompleted = ETrue;
            }
            break;
        case EPbStatePaused:
            {
            if (iPrevousPlaybackState == EPbStateInitialising)
                {
                iPdPlaybackUtility->CommandL( EPbCmdPlay ); //Automatically play when EMC is done initializing.
                }
            }
            break;
        default:
            break;
        }
    MPX_DEBUG1("CMPXPdCommandHandler::DoHandleStateChangedL()");
    }


// --------------------------------------------------------------
// CMPXPdCommandHandler::IsPlaybackCompleted()
// --------------------------------------------------------------
//
TBool CMPXPdCommandHandler::IsPlaybackCompleted()
    {
    return iPlaybackCompleted;
    }
// End of File
