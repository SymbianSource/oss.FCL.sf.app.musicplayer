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
* Description: Stub of QSystemInfo
*
*/

#ifndef QSYSTEMINFO_H
#define QSYSTEMINFO_H

#include <QObject>
#include <QString>


//class declaration
class QSystemInfo : public QObject
{
    Q_OBJECT

public:
    QSystemInfo( QObject *parent = 0 );
    ~QSystemInfo();
    
    QString currentLanguage();
    QString currentCountryCode();    
};

#define QTM_USE_NAMESPACE ;

#endif // QSYSTEMINFO_H
