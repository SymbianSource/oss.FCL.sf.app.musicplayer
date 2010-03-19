/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mpxharvesterutility stub for testing MpNowPlayingBackEnd
*
*/


#include <e32debug.h>

#include "stub/inc/mpxharvesterutility.h"


bool gCMPXHarvesterFactoryNewLLeave = false;
int gMMPXHarvesterUtilityCount = 0;
bool gMMPXHarvesterUtilityCheckForSystemEventsLLeave = false;
bool gMMPXHarvesterUtilityScanLLeave = false;
bool gMMPXHarvesterUtilityCancelScanLLeave = false;


//static functions start

void CMPXHarvesterFactory::setNewLLeave()
{
    gCMPXHarvesterFactoryNewLLeave = true;
}

void CMPXHarvesterFactory::NewLLeaveIfDesiredL()
{
    if (gCMPXHarvesterFactoryNewLLeave)
    {
        RDebug::Print(_L("CMPXHarvesterFactory::NewLLeaveIfDesiredL Leave"));
        gCMPXHarvesterFactoryNewLLeave = false;
        User::Leave(KErrGeneral);
    }
}

int MMPXHarvesterUtility::getCount()
{
    return gMMPXHarvesterUtilityCount;
}

void MMPXHarvesterUtility::setCheckForSystemEventsLLeave()
{
    gMMPXHarvesterUtilityCheckForSystemEventsLLeave = true;
}

void MMPXHarvesterUtility::CheckForSystemEventsLLeaveIfDesired()
{
    if (gMMPXHarvesterUtilityCheckForSystemEventsLLeave)
    {
        RDebug::Print(_L("MMPXHarvesterUtility::CheckForSystemEventsLLeaveIfDesired Leave"));
        gMMPXHarvesterUtilityCheckForSystemEventsLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXHarvesterUtility::setScanLLeave()
{
    gMMPXHarvesterUtilityScanLLeave = true;
}

void MMPXHarvesterUtility::ScanLLeaveIfDesired()
{
    if (gMMPXHarvesterUtilityScanLLeave)
    {
        RDebug::Print(_L("MMPXHarvesterUtility::ScanLLeaveIfDesired Leave"));
        gMMPXHarvesterUtilityScanLLeave = false;
        User::Leave(KErrGeneral);
    }
}

void MMPXHarvesterUtility::setCancelScanLLeave()
{
    gMMPXHarvesterUtilityCancelScanLLeave = true;
}

void MMPXHarvesterUtility::CancelScanLLeaveIfDesired()
{
    if (gMMPXHarvesterUtilityCancelScanLLeave)
    {
        RDebug::Print(_L("MMPXHarvesterUtility::CancelScanLLeaveIfDesired Leave"));
        gMMPXHarvesterUtilityCancelScanLLeave = false;
        User::Leave(KErrGeneral);
    }
}

//static functions end

MMPXHarvesterUtility* CMPXHarvesterFactory::NewL()
    {
    RDebug::Print(_L("stub CMPXHarvesterFactory::NewL"));
    
    CMPXHarvesterFactory::NewLLeaveIfDesiredL();
    gMMPXHarvesterUtilityCount++;
    static MMPXHarvesterUtility harvesterUtility;
    return &harvesterUtility;
    }

MMPXHarvesterUtility::MMPXHarvesterUtility()
{
}

MMPXHarvesterUtility::~MMPXHarvesterUtility()
{
}

void MMPXHarvesterUtility::Close()
{
    gMMPXHarvesterUtilityCount--;
}

void MMPXHarvesterUtility::CheckForSystemEventsL()
{
    RDebug::Print(_L("MMPXHarvesterUtility::CheckForSystemEventsL"));
    MMPXHarvesterUtility::setCheckForSystemEventsLLeave();
}

void MMPXHarvesterUtility::ScanL()
{
    RDebug::Print(_L("MMPXHarvesterUtility::ScanL"));
    MMPXHarvesterUtility::ScanLLeaveIfDesired();    
}

void MMPXHarvesterUtility::CancelScanL()
{
    RDebug::Print(_L("MMPXHarvesterUtility::CancelScanL"));
    MMPXHarvesterUtility::CancelScanLLeaveIfDesired();    
}

//end of file
