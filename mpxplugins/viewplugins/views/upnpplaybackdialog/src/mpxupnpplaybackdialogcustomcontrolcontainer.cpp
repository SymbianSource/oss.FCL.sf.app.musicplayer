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
* Description:  Music Player UPnP playback dialog implementation
*
*/


// INCLUDE FILES

#include "mpxupnpplaybackdialogcustomcontrolcontainer.h"
#include "mpxupnpplaybackdialogcustomcontrol.h"

// Destructor
CMPXUPnPPlaybackDialogCustomControlContainer::~CMPXUPnPPlaybackDialogCustomControlContainer() 
    { 
    delete iDialogCustomControl; 
    }

// -----------------------------------------------------------------------------
// CMPXUPnPPlaybackDialogCustomControlContainer::Draw
// Removes flicker from CEikDialog's Draw().
// -----------------------------------------------------------------------------
//
void CMPXUPnPPlaybackDialogCustomControlContainer::Draw( const TRect & ) const
    {
    }
    
// ---------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// ---------------------------------------------------------------------------
//    
void CMPXUPnPPlaybackDialogCustomControlContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );    
    }

//  End of File  
