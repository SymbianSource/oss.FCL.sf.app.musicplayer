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
* Description: Music Player engine.
*
*/

#ifndef MPENGINE_H
#define MPENGINE_H

#include <QObject>
#include "mpmpxcommondefs.h"

class QStringList;
class QTranslator;
class HbNotificationDialog;
class MpMpxFrameworkWrapper;
class MpSongScanner;
class MpMediaKeyHandler;

#if defined(BUILD_MPENGINE_LIB)
#define MPENGINE_EXPORT Q_DECL_EXPORT
#else
#define MPENGINE_EXPORT Q_DECL_IMPORT
#endif


class MPENGINE_EXPORT MpEngine : public QObject
{
    Q_OBJECT

private:

    enum UsbBlockingState {
        USB_NotConnected,          // Not connected
        USB_Connected,             // Connected in MTP mode but not synchronizing
        USB_Synchronizing          // Connected in MassStorage mode or MTP mode and synchronizing
    };

private:
    explicit MpEngine();

public:

    virtual ~MpEngine();
    static MpEngine *instance();

    void refreshLibrary();
    bool verifyUsbBlocking( bool showMessage = false );

signals:

    void libraryAboutToRefresh();
    void libraryRefreshed();
    void usbBlocked( bool blocked );
    void exitApplication();

public slots:

    void handleScanStarted();
    void handleScanEnded( int count, int error );
    void handleDiskEvent( MpxDiskEvents event );
    void handleUsbEvent( MpxUsbEvents event );

private:

    void handleUsbMassStorageStartEvent();
    void handleUsbMassStorageEndEvent();
    void handleUsbMtpStartEvent();
    void handleUsbMtpEndEvent();
    void handleUsbMtpNotActive();

    void changeUsbBlockingState( UsbBlockingState state );

private:

    Q_DISABLE_COPY( MpEngine )

    MpMpxFrameworkWrapper       *mMpxWrapper;       // Own
    MpSongScanner               *mSongScanner;      // Own
    MpMediaKeyHandler           *mMediaKeyHandler;  // Own

    HbNotificationDialog        *mUsbBlockingNote;  // Own
    QTranslator                 *mMpTranslator;     // Own

    UsbBlockingState            mUsbBlockingState;
    UsbBlockingState            mPreviousUsbState;

};

#endif // MPENGINE_H
