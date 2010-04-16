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
* Description: mpxcollectionutility stub for testing mpmpxframeworkwrapper
*
*/


#include <mpxmedia.h>
#include <mpxmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxcollectionplugin.hrh>
#include <mpxcollectionpath.h>
#include "mptrace.h"
#include <e32debug.h>

#include "stub/inc/mpxcollectionutility.h"
const TInt KMPXAllSongsViewIndex = 0;
const TInt KMPXPlaylistViewIndex = 1;

/*!
 Stub function.
*/
MMPXCollectionUtility::MMPXCollectionUtility():iCmd(0)
{   
}

/*!
 Stub function.
*/
MMPXCollectionUtility::~MMPXCollectionUtility()
{    
}

/*!
 Stub function.
*/
MMPXCollectionUtility* MMPXCollectionUtility::NewL(MMPXCollectionObserver* aObs, const TUid& aModeId)
{
    Q_UNUSED(aObs);
    Q_UNUSED(aModeId);
    MMPXCollectionUtility* temp = new MMPXCollectionUtility();
    return temp;
}

/*!
 Stub function.
*/
MMPXCollection& MMPXCollectionUtility::Collection()
{ 
    return *this;
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::Close()
{
    delete this;
}

/*!
 Stub function.
*/
TUid MMPXCollectionUtility::CollectionIDL(const TArray<TUid>& aUids)
{ 
    Q_UNUSED(aUids);
    TX_ENTRY
    return TUid::Uid(EMPXCollectionPluginMusic);
}

/*!
 Stub function.
*/
void MMPXCollectionUtility::CommandL(CMPXCommand& aCmd)
{
    delete iCmd;
    iCmd = 0;
    iCmd = CMPXCommand::NewL(aCmd);
}

//end of file
