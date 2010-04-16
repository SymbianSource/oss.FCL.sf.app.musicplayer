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
* Description: Framework wrapper
*
*/


#include "stub/inc/mpmpxdetailsframeworkwrapper.h"
//#include "mptrace.h"

/*!
 Constructs the utility wrapper.
 */
MpMpxDetailsFrameworkWrapper::MpMpxDetailsFrameworkWrapper( QObject *parent )
    : QObject(parent)
{
//    TX_ENTRY
    iSongData = new MpSongData();
//    TX_EXIT
}

/*!
 Destructs the utility wrapper.
 */
MpMpxDetailsFrameworkWrapper::~MpMpxDetailsFrameworkWrapper()
{
  //  TX_ENTRY

    //TX_EXIT
}

/*!
 Returns pointer to MpSongData
 */
MpSongData *MpMpxDetailsFrameworkWrapper::songData()
{
    return iSongData;
}

/*!
 Retrieve currently playing song meta data
 */
void MpMpxDetailsFrameworkWrapper::retrieveSong()
{
    //TX_ENTRY
        
    //TX_EXIT
}

