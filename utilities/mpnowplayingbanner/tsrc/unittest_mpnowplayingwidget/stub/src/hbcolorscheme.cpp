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


#include "stub/inc/hbcolorscheme.h"

/*!
 *  Stub function
 */
HbColorTheme *HbColorTheme::global ()
{
    static HbColorTheme instance;
    return &instance;
}

/*!
 *  Stub function
 */
QColor HbColorTheme::color( const QString &colorRole )
{
    if ( mTheme == 1 ) {
        if ( colorRole == QString("foreground") ) {
            return QColor(255,0,255);
        }
        else{
            return QColor(0,255,0);
        }
    }
    else {
        if ( colorRole == QString("foreground") ) {
            return QColor(255,255,0);
        }
        else{
            return QColor(0,255,255);
        }
    }
}

/*!
 *  Stub function
 */
void HbColorTheme::setCurrentTheme ( int theme )
{
    mTheme = theme;
    
}

/*!
 *  Stub function
 */
HbColorTheme::HbColorTheme (): mTheme(0)
{
    
}
/*!
 *  Stub function
 */
HbColorTheme::~HbColorTheme ()
{
  
}

/*!
 *  Stub function
 */
QColor HbColorScheme::color( const QString &colorRole )
{
    QColor normalColor(255,255,255);
    return HbColorTheme::global()->color(colorRole);
}

