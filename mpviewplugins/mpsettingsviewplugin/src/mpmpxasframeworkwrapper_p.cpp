/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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
* Description: Framework wraper for Audio Settings - Private.
*
*/

#include <mpxplaybackutility.h>

#include "mpmpxasframeworkwrapper_p.h"
#include "mpxaudioeffectengine.h"
#include "mpcommondefs.h"
#include "mptrace.h"


/*!
    \class MpMpxAsFrameworkWrapperPrivate
    \brief Wrapper for mpx framework utilities - private implementation.

    This is a private implementation of the mpx framework wrapper utilties interface.
*/

/*!
 \internal
 */
MpMpxAsFrameworkWrapperPrivate::MpMpxAsFrameworkWrapperPrivate()
    :iPlaybackUtility( 0 ),
     iAudioEffectProperties( new CMPXAudioEffectProperties() )
{
    
}

/*!
 \internal
 */
MpMpxAsFrameworkWrapperPrivate::~MpMpxAsFrameworkWrapperPrivate()
{
    if( iPlaybackUtility )
        {
        iPlaybackUtility->Close();
        }
    delete iAudioEffectProperties;
}

/*!
 \internal
 */

void MpMpxAsFrameworkWrapperPrivate::init()
{
    TRAPD( err, doInitL() );
    if ( err != KErrNone ) 
        {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
        }
}

/*!
 \internal
 */
void MpMpxAsFrameworkWrapperPrivate::setBalance( int balance )
{
    TRAPD( err, doSetBalanceL( balance ) );
    if ( err != KErrNone ) 
        {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
        }
}

/*!
 \internal
 */
void MpMpxAsFrameworkWrapperPrivate::setLoudness( bool mode )
{
    TRAPD( err, doSetLoudnessL( mode ) );
    if ( err != KErrNone ) 
        {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
        }
}

/*!
 \internal
 */
int MpMpxAsFrameworkWrapperPrivate::balance()
{
    int ret = 0;
    TRAPD( err, ret = balanceL() );
    if ( err != KErrNone ) 
        {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
        return 0;
        }   
    return ret;
}

/*!
 \internal
 */
bool MpMpxAsFrameworkWrapperPrivate::loudness()
{
    bool ret = true;
    TRAPD( err, ret = loudnessL() );
    if ( err != KErrNone ) 
        {
        TX_LOG_ARGS( "Error: " << err << "; should never get here." );
        return false;
        }   
    return ret;
}

/*!
 \internal
 */
void MpMpxAsFrameworkWrapperPrivate::doInitL()
{
    TRAPD( err, iAudioEffectProperties->LoadFromFileL() );
    if( KErrNone != err )
        {
        iAudioEffectProperties->Reset();
        User::Leave( err );
        }
}

/*!
 \internal
 */
void MpMpxAsFrameworkWrapperPrivate::doSetBalanceL( int balance )
{
    iAudioEffectProperties->SetBalance( balance );
    iAudioEffectProperties->SaveToFileL();
    if ( !iPlaybackUtility ) 
        {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid( MpCommon::KMusicPlayerUid ) ) ;
        }
   iPlaybackUtility ->SetL( EPbPropertyBalance, balance );
   
}

/*!
 \internal
 */
void MpMpxAsFrameworkWrapperPrivate::doSetLoudnessL( bool mode )
{
    if( loudnessL() != mode )  //do not set same value twice
        {
        iAudioEffectProperties->SetLoudness( mode );
        iAudioEffectProperties->SaveToFileL();
        if ( !iPlaybackUtility ) 
            {
            iPlaybackUtility = MMPXPlaybackUtility::UtilityL( TUid::Uid( MpCommon::KMusicPlayerUid ) ) ;
            }
        iPlaybackUtility->CommandL( EPbApplyEffect, KAudioEffectsID );
        }
}

/*!
 \internal
 */
int MpMpxAsFrameworkWrapperPrivate::balanceL()
{
    return iAudioEffectProperties->Balance();
}

/*!
 \internal
 */
bool MpMpxAsFrameworkWrapperPrivate::loudnessL()
{
    return iAudioEffectProperties->Loudness();
}




