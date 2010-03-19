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
* Description: Music Player collection song scanner.
*
*/


#ifndef MPCOLLECTIONSONGSCANNER_H
#define MPCOLLECTIONSONGSCANNER_H

class MpMpxFrameworkWrapper;
class HbProgressNote;

#include <QObject>

class MpCollectionSongScanner : public QObject
{
    Q_OBJECT

public:

    explicit MpCollectionSongScanner( MpMpxFrameworkWrapper *wrapper, QObject *parent=0 );
    virtual ~MpCollectionSongScanner();

    void scan();
    bool isScanning();

signals:

    void scanEnded();

public slots:

    void cancelScan();
    void handleScanStarted();
    void handleScanEnded();
    void handleScanCountChanged(int count);

private:

    MpMpxFrameworkWrapper       *mMpxWrapper;       // NotOwn
    TBool                       mScanning;
    HbProgressNote              *mScanProgressNote; // Own

};

#endif // MPCOLLECTIONSONGSCANNER_H

