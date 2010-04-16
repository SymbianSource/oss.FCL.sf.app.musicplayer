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
* Description: Wrapper for equalizer utility - private implementation.
*
*/

// System includes
#include <qDebug>
#include <QStringList>
#include <audioequalizerutility.h>
#include <EqualizerConstants.h>
#include <mpxplaybackutility.h>

// User includes
#include "mpequalizerwrapper.h"
#include "mpequalizerwrapper_p.h"
#include "mpsettingsmanager.h"
#include "mpcommondefs.h"
#include "mptrace.h"

// CONSTANTS
_LIT( KMPXNullSoundPath, "\\nullsound.mp3" );
const TInt KEqualizerID = 0x101FFC76;

/*!
    \class MpEqualizerWrapperPrivate
    \ingroup musicplayer
    \brief Wrapper for equalizer utility - private implementation.
    
    This is a private implementation of the audio eqalizer wrapper 
    utilties interface.

*/

/*!
    Constructs a new MpEqualizerWrapper with \a parent and initializes its
    private imaplementation.
*/
MpEqualizerWrapperPrivate::MpEqualizerWrapperPrivate(
    MpEqualizerWrapper *wrapper ): q_ptr( wrapper ),
    iEqualizerUtility (NULL), iCurrentPresetIndex(KEqualizerPresetNone)
{
    TX_LOG
}

/*!
    Destructs the class.    
 */
MpEqualizerWrapperPrivate::~MpEqualizerWrapperPrivate()
{
    TX_ENTRY

    delete iEqualizerUtility;
    
    if ( iMdaPlayUtility ) 
    {
        iMdaPlayUtility->Close();
    }
    delete iMdaPlayUtility;

    if ( iMpxPlaybackUtility )
    {
        iMpxPlaybackUtility->Close();
    }

    TX_EXIT
}

/*!
    Calls the DoInit to initialize Symbian based components.
    
    \sa DoInitL()
 */
void MpEqualizerWrapperPrivate::init()
{
    TX_ENTRY

    TRAPD(err, DoInitL());
    if ( err != KErrNone )
    {
        TX_LOG_ARGS("MpEqualizerWrapperPrivate::init err = " << err);
    }
    
    TX_EXIT
}

/*!
    Apply the preset by giving preset index. The index is subtracted by 1 because
    index 0 represent "Off" at UI level.

 */
void MpEqualizerWrapperPrivate::applyPreset( int preset )
{
    TX_ENTRY_ARGS( "preset=" << preset );

    if ( iEqualizerUtility )
    {
        TInt presetKey(KErrNotFound);
        TRAPD(err, presetKey = GetEngineIndexL( preset - 1 ));
        if ( err != KErrNone )
        {
            TX_LOG_ARGS("GetEngineIndexL Error  = " << err);
            return;
        }
        TX_LOG_ARGS( "Preset key in engine =" << presetKey );
        HandlePresetActivation( presetKey );
        iCurrentPresetIndex = preset;
    } 
    else
    {
        TX_LOG_ARGS( "Equalizer not ready." );
    }

    TX_EXIT
}

/*!
    Return current preset index (UI)

 */
int MpEqualizerWrapperPrivate::activePreset()
{
    TX_LOG_ARGS("iCurrentPresetIndex = " << iCurrentPresetIndex );

    return iCurrentPresetIndex;
}

/*!
    Disabling equalizer by setting the preset to -1 and apply it to 
    disable current preset.

 */
void MpEqualizerWrapperPrivate::disableEqualizer()
{
    TX_ENTRY

    TInt nullPreset( KErrNotFound );
    iCurrentPresetIndex = KEqualizerPresetNone;
    HandlePresetActivation( nullPreset );

    TX_EXIT
}

/*!
    Returning the list of availale preset names.  

    \sa CAudioEqualizerUtility
 */
QStringList MpEqualizerWrapperPrivate::presetNames()
{
    TX_ENTRY

    QStringList presetList;

    if ( iEqualizerUtility == NULL )
    {
        TX_LOG_ARGS( "Equalizer not ready." );
        return presetList;
    }

    TArray<TEfAudioEqualizerUtilityPreset> presetArray = iEqualizerUtility->Presets();
    for (TInt i=0; i<presetArray.Count(); i++)
    {
        TPresetName selectedItem = presetArray[i].iPresetName;
        presetList << QString::fromUtf16( selectedItem.Ptr(), selectedItem.Length() );
    }

    TX_EXIT

    return presetList;
}

/*!
 \internal
 */
void MpEqualizerWrapperPrivate::DoInitL()
{
    TX_ENTRY

    if (!iMpxPlaybackUtility)
    {
        iMpxPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid( MpCommon::KMusicPlayerUid ) );
    }

    if (iMdaPlayUtility)
    {
        iMdaPlayUtility->Close();
        delete iMdaPlayUtility;
        iMdaPlayUtility = NULL;
    }
    
    iMdaPlayUtility = ( CMdaAudioPlayerUtility* )User::LeaveIfNull(
        CMdaAudioPlayerUtility::NewL(
            *this,
            EMdaPriorityMin,
            EMdaPriorityPreferenceNone ));

    TX_LOG_ARGS("Loading null sound");
    TFileName nullSoundPath( KMPXNullSoundPath );
    User::LeaveIfError( MPXUser::CompleteWithDllPath( nullSoundPath ) );
    TX_LOG_ARGS("Loading null sound done!");
    iMdaPlayUtility->OpenFileL( nullSoundPath );
    
    TX_EXIT
}


/*!
 \internal
 */
void MpEqualizerWrapperPrivate::HandlePresetActivation( TInt aActivatedPreset )
{
    TX_ENTRY_ARGS( "aActivatedPreset=" << aActivatedPreset );

    // Store in CenRep file
    MpSettingsManager::setPreset( aActivatedPreset );
    
    // Notify playback framework of the change.
    TRAPD(error,
    {
        iMpxPlaybackUtility->CommandL( EPbApplyEffect, KEqualizerID );
    });
    
    if (error != KErrNone)
    {
        TX_ENTRY_ARGS( "Equalizer error:" << error );
    }
    
    TX_EXIT
}


/*!
 \internal
 */
void MpEqualizerWrapperPrivate::GetPresetId( TInt& aPresetId )
{
    TX_ENTRY

    // Get preset id from cenrep
    TInt presetId( MpSettingsManager::preset() );

    TBool found( EFalse );
    if ( iEqualizerUtility && presetId != KEqualizerPresetNone )
    {
        TArray<TEfAudioEqualizerUtilityPreset> presetArray = iEqualizerUtility->Presets();
        for ( TInt i = 0; i < presetArray.Count() && found == EFalse; i++ ) {
            TX_LOG_ARGS("presetArray[" << i << "].iPresetNameKey (" 
                << presetArray[i].iPresetNameKey << ") vs presetId (" <<presetId << ")");
            if ( presetArray[i].iPresetNameKey == static_cast<TUint32>( presetId ) ) {
                found = ETrue;
                iCurrentPresetIndex = aPresetId = i + 1;
                break;
            }
        }
    }

    if ( found == EFalse )
    {
        iCurrentPresetIndex = aPresetId = KEqualizerPresetNone;
    }

    TX_EXIT
}


/*!
 \internal
 */
TInt MpEqualizerWrapperPrivate::GetEngineIndexL( const TInt preset ) const
{
    TX_ENTRY_ARGS( "preset=" << preset );

    TInt engineIndex = KErrNotFound;

    TPresetName presetName = iEqualizerUtility->GetPresetL( preset );

    TArray<TEfAudioEqualizerUtilityPreset> presets = iEqualizerUtility->Presets();
    for (TInt count = 0; count < presets.Count(); count++) 
    {
        TPresetName selectedItem = presets[count].iPresetName;
        TX_LOG_ARGS(" Comparing..." << presets[count].iPresetNameKey );
        if ( selectedItem == presetName ) {
            engineIndex = presets[count].iPresetNameKey;
            TX_LOG_ARGS(" Comparing mached engineIndex = " << engineIndex );
            break;
            }
    }

    TX_EXIT

    return engineIndex;
}

/*!
 \internal
 */
void MpEqualizerWrapperPrivate::MapcInitComplete(
    TInt aError,
    const TTimeIntervalMicroSeconds& /* aDuration */ )
{
    TX_ENTRY_ARGS( "aError=" << aError );

    if ( !aError ) 
    {
        TRAPD(err, iEqualizerUtility = CAudioEqualizerUtility::NewL( *iMdaPlayUtility ) );
        if ( err == KErrNone ) {
            TInt preset(KEqualizerPresetNone);
            GetPresetId( preset );
            emit q_ptr->equalizerReady();
        }
        else
        {
            TX_ENTRY_ARGS( "CAudioEquqlizerUtility error=" << err );
        }
    }
    else
    {
        TX_ENTRY_ARGS( "MapcInitComplete error=" << aError );
    }

    TX_EXIT
}

/*!
 \internal
 */
void MpEqualizerWrapperPrivate::MapcPlayComplete(
    TInt /* aError */ )
{
    TX_LOG
    // Do nothing
}

//End of File
