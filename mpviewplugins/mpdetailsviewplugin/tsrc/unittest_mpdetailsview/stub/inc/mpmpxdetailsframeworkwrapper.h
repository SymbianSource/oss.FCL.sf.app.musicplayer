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

#ifndef MPMPXDETAILSFRAMEWORKWRAPPER_H_
#define MPMPXDETAILSFRAMEWORKWRAPPER_H_

//includes
#include <QObject>
#include "stub/inc/mpsongdata.h"

//forward declartions
class MpSongData;

//class declaration
class MpMpxDetailsFrameworkWrapper : public QObject
{
    Q_OBJECT

public:
    MpMpxDetailsFrameworkWrapper( QObject *parent=0 );
    virtual ~MpMpxDetailsFrameworkWrapper();
    
    MpSongData *songData();
    void retrieveSong();
private:
    MpSongData* iSongData;
    
};


#endif /* MPMPXDETAILSFRAMEWORKWRAPPER_H_ */
