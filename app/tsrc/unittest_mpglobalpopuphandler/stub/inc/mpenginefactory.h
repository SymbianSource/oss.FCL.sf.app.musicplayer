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
* Description: MpEngineFactory stub 
*
*/

#ifndef MPENGINEFACTORY_H
#define MPENGINEFACTORY_H

#include <QObject>
#include "mpcommondefs.h"
#include "mpengine.h"

class MpEngineFactory : QObject
{
    Q_OBJECT
private:
    explicit MpEngineFactory();

public:
    virtual ~MpEngineFactory();
    static MpEngineFactory * instance();

    static MpEngine *createSharedEngine( quint32 clientSecureId=MpCommon::KMusicPlayerUid, MpEngine::EngineMode mode=MpEngine::StandAlone );
    static MpEngine *createIsolatedEngine( MpEngine::EngineMode mode );
    static MpEngine *sharedEngine();
    static void close();
	
private:
    
    Q_DISABLE_COPY( MpEngineFactory )
    
    MpEngine             *mSharedEngine;
    QList<MpEngine *>    mEngines;
};


#endif // MPENGINEFACTORY_H
