/*
* Copyright ( c ) 2009 Nokia Corporation and/or its subsidiary( -ies ).
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
* Description: Framework wraper for Audio Settings.
*
*/

#include "mpmpxasframeworkwrapper.h"
#include "mpmpxasframeworkwrapper_p.h"
#include "mptrace.h"

/*!
    \class MpMpxAsFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
 Constructor.
 */
MpMpxAsFrameworkWrapper::MpMpxAsFrameworkWrapper()
    :d_ptr( new MpMpxAsFrameworkWrapperPrivate() )
{
    d_ptr->init();
}

/*!
 Destructor.
 */
MpMpxAsFrameworkWrapper::~MpMpxAsFrameworkWrapper()
{
    TX_ENTRY
    delete d_ptr;
    TX_EXIT
}


/*!
 Returns the current persistent balance.
 */
int MpMpxAsFrameworkWrapper::balance()
{
    TX_LOG
    return d_ptr->balance();
}

/*!
 Returns the current persistent loudness.
 */
bool MpMpxAsFrameworkWrapper::loudness()
{
    TX_LOG
    return d_ptr->loudness();
}

/*!
 Slot to handle a change to the balance.
 */
void MpMpxAsFrameworkWrapper::setBalance( int balance )
{
    TX_ENTRY
    d_ptr->setBalance( balance );
    TX_EXIT
}

/*!
 Slot to handle a change to the loudness.
 */
void MpMpxAsFrameworkWrapper::setLoudness( bool mode )
{
    TX_ENTRY
    d_ptr->setLoudness( mode );
    TX_EXIT
}
