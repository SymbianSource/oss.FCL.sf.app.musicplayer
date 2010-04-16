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
* Description: HbColorScheme stub for testing MpNowPlayingWidget
*
*/

#ifndef HBCOLORSCHEME_H
#define HBCOLORSCHEME_H

#include <hbglobal.h>
#include <QColor>


class QGraphicsWidget;
class QString;


class HbColorTheme
{
    
public:
    // Stub functions
    ~HbColorTheme();

    static HbColorTheme *global ();
    
    QColor color(const QString &colorRole);
    void setCurrentTheme(int theme);
    
private:
    HbColorTheme ();
    int mTheme;
  
};

class HbColorScheme
{   
public:
    // Stub functions
    static QColor color( const QString &colorRole );
    
};

#endif // HBCOLORSCHEME_H
