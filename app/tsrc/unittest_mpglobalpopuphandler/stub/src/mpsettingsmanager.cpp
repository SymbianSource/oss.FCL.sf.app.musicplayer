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
* Description: MpSettingsManager stub for testing MpMtpInfoLink.
*
*/

#include "mpsettingsmanager.h"


/*!
 Constructs the MP Settings Manager.
 */
MpSettingsManager::MpSettingsManager()
{
    mMtpInfoUrl = QString( "http://nokia.com/" );
}

/*!
 Destructs the settings manager.
 */
MpSettingsManager::~MpSettingsManager()
{
}

/*!
 Returns the singleton instance to the settings manager.
 */
MpSettingsManager * MpSettingsManager::instance()
{
    static MpSettingsManager instance;
    return &instance;
}

void MpSettingsManager::SetUrl( QString url )
{
    instance()->mMtpInfoUrl = url;
}

/*!
 Returns mtp info url.
 */
QString MpSettingsManager::mtpInfoUrl()
{
    return instance()->mMtpInfoUrl;
}

/*!
 Returns wheter mtp info should be showed.
 */
bool MpSettingsManager::showMtpInfo()
{
    return instance()->mShowMtpInfo;
}

/*!
 Slot to be called to stop showing mtp info.
 */
void MpSettingsManager::stopShowingMtpInfo()
{

}
