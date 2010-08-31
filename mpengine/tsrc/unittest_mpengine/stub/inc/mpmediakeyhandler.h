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
* Description: stub for media key handler.
*
*/

#ifndef MPMEDIAKEYHANDLER_H
#define MPMEDIAKEYHANDLER_H

#include <QObject>

class MpMediaKeyHandler : public QObject
{
    Q_OBJECT
public:

    explicit MpMediaKeyHandler( QObject *parent = 0 );
    virtual ~MpMediaKeyHandler();

    void setEnabled( bool enable );
    
public: 
    bool mEnable;
};

#endif // MPMEDIAKEYHANDLER_H
