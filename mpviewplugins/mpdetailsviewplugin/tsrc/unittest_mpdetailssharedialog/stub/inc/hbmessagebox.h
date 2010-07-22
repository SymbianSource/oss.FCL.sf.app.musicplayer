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
* Description: stub file for hbmessagebox class.
*
*/
#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <QObject>

class HbMessageBox : public QObject
{
    Q_OBJECT
public:
    explicit HbMessageBox();
    virtual ~HbMessageBox();

public:
    static void information(const QString &warningText);
    static void warning(const QString &warningText);
};

#endif // HB_MESSAGEBOX_H

