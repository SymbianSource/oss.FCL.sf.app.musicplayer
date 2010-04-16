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
* Description: Wrapper for mpx framework utilities.
*
*/

#include "mpmpxframeworkwrapper.h"
#include "mpmpxframeworkwrapper_p.h"

/*!
    \class MpMpxFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
    \fn void scanStarted()

    This signal is emitted when scan operation is started.

 */

/*!
    \fn void scanEnded( int count, int error )

    This signal is emitted when scan operation ends. It indicates the number
    of songs added and whether there was an error.

 */

/*!
    \fn void scanCountChanged( int count )

    This signal is emitted when scan count is updated.

 */

/*!
    \fn void diskEvent( MpxDiskEvent event )

    This signal is emitted when a disk event is received from MPX framework.

 */

 /*!
     \fn void usbEvent( MpxUsbEvent event )

     This signal is emitted when an USB event is received from MPX framework.

 */

/*!
 Constructs the utility wrapper.
 */
MpMpxFrameworkWrapper::MpMpxFrameworkWrapper( QObject *parent )
    : QObject( parent )
{
    d_ptr = new MpMpxFrameworkWrapperPrivate( this );
    d_ptr->init();
}

/*!
 Destructs the utility wrapper.
 */
MpMpxFrameworkWrapper::~MpMpxFrameworkWrapper()
{
    delete d_ptr;
}

/*!
 Initiates song scanning.
 */
void MpMpxFrameworkWrapper::scan()
{
    d_ptr->scan();
}

/*!
 Cancels ongoing song scanning.
 */
void MpMpxFrameworkWrapper::cancelScan()
{
    d_ptr->cancelScan();
}

