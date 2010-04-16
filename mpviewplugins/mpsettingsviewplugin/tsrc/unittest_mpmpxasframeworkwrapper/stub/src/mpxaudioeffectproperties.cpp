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
* Description:  audio effect properties stub for testing mpmpxasframeworkwrapper
*
*/

#include  <s32file.h>
#include  <s32std.h>
#include <f32file.h>
#include <mpxlog.h>
#include "stub/inc/mpxaudioeffectproperties.h"


/*!
 Stub function.
*/
CMPXAudioEffectProperties::CMPXAudioEffectProperties():iBalance(0),
                                                       iLoadFromFile(EFalse),
                                                       iSaveToFile(EFalse),
                                                       iLeave(EFalse)
{
}

/*!
 Stub function.
*/
CMPXAudioEffectProperties::~CMPXAudioEffectProperties()
{
}

/*!
 Stub function.
*/
TInt CMPXAudioEffectProperties::Balance()
{
    return iBalance;
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::SetBalance(TInt aBalance)
{
    iBalance = aBalance;
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::Reset()
{
    MPX_DEBUG1("CMPXAudioEffectProperties::Reset");
    iBalance = 0;
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::SaveToFileL()
{
    MPX_DEBUG1("CMPXAudioEffectProperties::SaveToFileL <---");
    iSaveToFile = ETrue;
    MPX_DEBUG1("CMPXAudioEffectProperties::SaveToFileL --->");
}

/*!
 Stub function.
*/
void CMPXAudioEffectProperties::LoadFromFileL()
{
    MPX_DEBUG1("CMPXAudioEffectProperties::LoadFromFileL <---");
    if(iLeave){
        User::Leave(KErrNotFound);
    }else{
        iLoadFromFile = ETrue;
    }
    MPX_DEBUG1("CMPXAudioEffectProperties::LoadFromFileL --->");
}

//end of file
