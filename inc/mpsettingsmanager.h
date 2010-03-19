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
* Description: Music Player Settings Manager.
*
*/

#ifndef MPSETTINGSMANAGER_H
#define MPSETTINGSMANAGER_H

#include<QObject>
#include <QtGlobal>

class XQSettingsManager;
class XQSettingsKey;

#ifdef BUILD_MPSETTINGSMANAGER
#define MPSETTINGSMANAGER_EXPORT Q_DECL_EXPORT
#else
#define MPSETTINGSMANAGER_EXPORT Q_DECL_IMPORT
#endif
class  MPSETTINGSMANAGER_EXPORT MpSettingsManager: public QObject
{
    
Q_OBJECT

private:
    explicit MpSettingsManager();
    
public:
    virtual ~MpSettingsManager();
    static MpSettingsManager *instance();
    static bool firstStartup();
    static bool shuffle();
    static bool repeat();

public slots:
    void valueChanged(const XQSettingsKey& key, const QVariant& value);
    static void setShuffle(bool shuffle);
    static void setRepeat(bool repeat);
        
signals:
    void shuffleChanged(bool shuffle);
    void repeatChanged(bool repeat);
    
private:
    Q_DISABLE_COPY(MpSettingsManager)
    XQSettingsManager* mSettingsManager;
    bool mFirstStartup;
    bool mShuffle;
    bool mRepeat;
};

#endif    // MPSETTINGSMANAGER_H
