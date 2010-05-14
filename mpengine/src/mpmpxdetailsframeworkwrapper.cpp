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


#include "mpmpxdetailsframeworkwrapper.h"
#include "mpmpxdetailsframeworkwrapper_p.h"
#include "mptrace.h"

/*!
    \class MpMpxDetailsFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/


/*!
 Constructs the utility wrapper.
 */
MpMpxDetailsFrameworkWrapper::MpMpxDetailsFrameworkWrapper( TUid hostUid, QObject *parent )
    : QObject(parent)      
{
    TX_ENTRY
    d_ptr = q_check_ptr( new MpMpxDetailsFrameworkWrapperPrivate( this ) );  //from Qt 4.6
    d_ptr->init( hostUid );
    TX_EXIT
}

/*!
 Destructs the utility wrapper.
 */
MpMpxDetailsFrameworkWrapper::~MpMpxDetailsFrameworkWrapper()
{
    TX_ENTRY
    delete d_ptr;
    TX_EXIT
}

/*!
 Returns pointer to MpSongData
 */
MpSongData *MpMpxDetailsFrameworkWrapper::songData()
{
    TX_LOG
    return d_ptr->songData();
}

/*!
 Retrieve currently playing song meta data
 */
void MpMpxDetailsFrameworkWrapper::retrieveSong()
{
    TX_LOG
    d_ptr->retrieveSong();
}

