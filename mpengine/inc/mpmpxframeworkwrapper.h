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

#ifndef MPMPXFRAMEWORKWRAPPER_H
#define MPMPXFRAMEWORKWRAPPER_H

#include <QObject>
#include "mpmpxcommondefs.h"

class MpMpxFrameworkWrapperPrivate;
class MpSongScannerHelper;
class HbNotificationDialog;
class QStringList;
class QTranslator;

class MpMpxFrameworkWrapper : public QObject
{
    Q_OBJECT

    friend class MpMpxFrameworkWrapperPrivate;

public:

    explicit MpMpxFrameworkWrapper( QObject *parent=0 );
    virtual ~MpMpxFrameworkWrapper();

    void scan();
    void cancelScan();

signals:

    void scanStarted();
    void scanEnded( int count, int error );
    void scanCountChanged( int count );

    void diskEvent( MpxDiskEvents event );
    void usbEvent( MpxUsbEvents event );

private:

    Q_DISABLE_COPY( MpMpxFrameworkWrapper )
    MpMpxFrameworkWrapperPrivate *d_ptr;

};

#endif // MPMPXFRAMEWORKWRAPPER_H
