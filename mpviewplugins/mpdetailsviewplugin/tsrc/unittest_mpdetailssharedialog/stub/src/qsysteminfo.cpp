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


#include "qsysteminfo.h"

QSystemInfo::QSystemInfo( QObject *parent )
    : QObject( parent )
{
    //TX_ENTRY
    //TX_EXIT
}

QSystemInfo::~QSystemInfo()
{
    //TX_ENTRY
    //TX_EXIT
}

QString QSystemInfo::currentLanguage() { return "fi"; };
QString QSystemInfo::currentCountryCode() { return "Fi"; };
