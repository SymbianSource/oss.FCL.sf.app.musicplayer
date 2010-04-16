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
* Description: mpxplaybackutility stub for testing mpmpxframeworkwrapper
*
*/

#include <Qt>

#include <mpxplaybackcommanddefs.h>
//#include <mpxplaybackframeworkdefs.h>

#include "stub/inc/mpxuser.h"
#include "stub/inc/mpxplaybackutility.h"


/*!
 Stub constructor.
 */
MMPXPlaybackUtility::MMPXPlaybackUtility() :
    commandSent(EFalse)
{

}

/*!
 Stub destructor.
 */
MMPXPlaybackUtility::~MMPXPlaybackUtility()
{

}
/*!
 Stub function.
 */
MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL(const TUid& /*aModeId*/)
{
    static MMPXPlaybackUtility playbackUtilility;
    return &playbackUtilility;
}

void MMPXPlaybackUtility::Close()
{

}

void MMPXPlaybackUtility::CommandL(TMPXPlaybackCommand /*aCmd*/, TInt /*aData*/)
{
    commandSent = ETrue;
}

